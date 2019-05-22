#include <linux/module.h>
#include <linux/dmapool.h>
#include <crypto/hash.h>
#include <crypto/internal/hash.h>

#include "cryptoapi.h"
#include "elpspaccdrv.h"

static struct dma_pool *spacc_hash_pool;

static int spacc_hash_init_dma(struct device *dev, struct ahash_request *req)
{
   struct spacc_crypto_reqctx *ctx = ahash_request_ctx(req);
   gfp_t mflags = GFP_ATOMIC;
   int rc;

   if (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP)
      mflags = GFP_KERNEL;

   ctx->digest_buf = dma_pool_alloc(spacc_hash_pool, mflags, &ctx->digest_dma);
   if (!ctx->digest_buf)
      return -ENOMEM;

   rc = pdu_ddt_init(&ctx->dst, 1|0x80000000);
   if (rc < 0) {
      rc = pdu_error_code(rc);
      goto err_free_digest;
   }
   pdu_ddt_add(&ctx->dst, ctx->digest_dma, SPACC_MAX_DIGEST_SIZE);

   rc = spacc_sg_to_ddt(dev, req->src, req->nbytes, &ctx->src, DMA_TO_DEVICE);
   if (rc < 0)
      goto err_free_dst;
   ctx->src_nents = rc;

   return 0;
err_free_dst:
   pdu_ddt_free(&ctx->dst);
err_free_digest:
   dma_pool_free(spacc_hash_pool, ctx->digest_buf, ctx->digest_dma);
   return rc;
}

static void
spacc_hash_cleanup_dma(struct device *dev, struct ahash_request *req)
{
   struct spacc_crypto_reqctx *ctx = ahash_request_ctx(req);

   dma_unmap_sg(NULL, req->src, ctx->src_nents, DMA_TO_DEVICE);
   pdu_ddt_free(&ctx->src);

   dma_pool_free(spacc_hash_pool, ctx->digest_buf, ctx->digest_dma);
   pdu_ddt_free(&ctx->dst);
}

static void spacc_digest_cb(void *spacc, void *tfm)
{
   struct ahash_cb_data *cb = tfm;
   int err;
   unsigned long lock_flag;

   memcpy(cb->req->result, cb->ctx->digest_buf, crypto_ahash_digestsize(crypto_ahash_reqtfm(cb->req)));
   spacc_hash_cleanup_dma(cb->tctx->dev, cb->req);

// manually close the handle ... since we can't call spacc_close() from this context (?)
   err = pdu_error_code(cb->spacc->job[cb->new_handle].job_err);
   PDU_LOCK(&cb->spacc->ctx_lock, lock_flag);
   cb->spacc->job[cb->new_handle].job_used = SPACC_JOB_IDX_UNUSED;
   cb->spacc->ctx[cb->spacc->job[cb->new_handle].ctx_idx].ref_cnt--;
   if (cb->spacc->ctx[cb->spacc->job[cb->new_handle].ctx_idx].ref_cnt == 0) {
      cb->spacc->ctx[cb->spacc->job[cb->new_handle].ctx_idx].ncontig = 0;
   }
   PDU_UNLOCK(&cb->spacc->ctx_lock, lock_flag);

// call complete
   cb->req->base.complete(&cb->req->base, err);
}

static int spacc_hash_digest(struct ahash_request *req)
{
   struct crypto_ahash *reqtfm = crypto_ahash_reqtfm(req);
   struct spacc_crypto_ctx *tctx = crypto_ahash_ctx(reqtfm);
   struct spacc_crypto_reqctx *ctx = ahash_request_ctx(req);
   struct spacc_priv *priv = dev_get_drvdata(tctx->dev);
   int rc;

   dev_dbg(tctx->dev, "%s (%u)\n", __func__, req->nbytes);

   if (tctx->handle < 0 || !tctx->ctx_valid || req->nbytes > priv->max_msg_len)
      goto fallback;

   rc = spacc_hash_init_dma(tctx->dev, req);
   if (rc < 0)
      goto fallback;

   ctx->acb.new_handle = spacc_clone_handle(&priv->spacc, tctx->handle, &ctx->acb);
   if (ctx->acb.new_handle < 0) {
      spacc_hash_cleanup_dma(tctx->dev, req);
      goto fallback;
   }

   ctx->acb.tctx  = tctx;
   ctx->acb.ctx   = ctx;
   ctx->acb.req   = req;
   ctx->acb.spacc = &priv->spacc;

   rc = spacc_packet_enqueue_ddt(&priv->spacc, ctx->acb.new_handle,
                                 &ctx->src, &ctx->dst, req->nbytes,
                                 0, req->nbytes, 0, 0, 0);

   if (rc < 0) {
      spacc_hash_cleanup_dma(tctx->dev, req);

      if (rc != CRYPTO_FIFO_FULL) {
         dev_err(tctx->dev, "failed to enqueue job: %s\n", spacc_error_msg(rc));
      } else if (!(req->base.flags & CRYPTO_TFM_REQ_MAY_BACKLOG)) {
         return -EBUSY;
      }

      goto fallback;
   }

   return -EINPROGRESS;
fallback:
   dev_dbg(tctx->dev, "Using SW fallback\n");

   /* Start from scratch as init is not called before digest. */
   ctx->fb.hash_req.base = req->base;
   ahash_request_set_tfm(&ctx->fb.hash_req, tctx->fb.hash);

   ctx->fb.hash_req.nbytes = req->nbytes;
   ctx->fb.hash_req.src    = req->src;
   ctx->fb.hash_req.result = req->result;

   return crypto_ahash_digest(&ctx->fb.hash_req);
}

static int
spacc_hash_setkey(struct crypto_ahash *tfm, const u8 *key, unsigned keylen)
{
   const struct spacc_alg *salg = spacc_tfm_alg(&tfm->base);
   struct spacc_crypto_ctx *tctx = crypto_ahash_ctx(tfm);
   struct spacc_priv *priv = dev_get_drvdata(tctx->dev);
   int rc;
   unsigned char xkey[48];

   dev_dbg(tctx->dev, "%s\n", __func__);
   tctx->ctx_valid = false;

   rc = crypto_ahash_setkey(tctx->fb.hash, key, keylen);
   if (rc < 0)
      return rc;

   if (tctx->handle < 0)
      return 0;

   /*
    * If keylen > hash block len, the key is supposed to be hashed so that it
    * is less than the block length.  This is kind of a useless property of
    * HMAC as you can just use that hash as the key directly.  We will just
    * not use the hardware in this case to avoid the issue.
    *
    * This test was meant for hashes but it works for cmac/xcbc since we only intend
    * to support 128-bit keys...
    */
   if (keylen > crypto_tfm_alg_blocksize(&tfm->base)) {
      return 0;
   }
   if (salg->mode->id == CRYPTO_MODE_MAC_XCBC) {
      rc = spacc_compute_xcbc_key(&priv->spacc, key, keylen, xkey);
      if (rc < 0) {
         dev_warn(tctx->dev, "failed to compute XCBC key: %d\n", rc);
         return 0;
      }
      rc = spacc_write_context(&priv->spacc, tctx->handle, SPACC_HASH_OPERATION,
                               xkey, 32+keylen, NULL, 0);
      memset(xkey, 0, sizeof xkey);
   } else {
      rc = spacc_write_context(&priv->spacc, tctx->handle, SPACC_HASH_OPERATION,
                               key, keylen, NULL, 0);
   }
   if (rc < 0) {
      dev_warn(tctx->dev, "failed to write SPAcc context %d: %s\n",
                          tctx->handle, spacc_error_msg(rc));

      /* Non-fatal; we continue with the software fallback. */
      return 0;
   }

   tctx->ctx_valid = true;
   return 0;
}

static int spacc_hash_cra_init(struct crypto_tfm *tfm)
{
   const struct spacc_alg *salg = spacc_tfm_alg(tfm);
   struct spacc_crypto_ctx *tctx = crypto_tfm_ctx(tfm);
   struct spacc_priv *priv = dev_get_drvdata(salg->dev);
   int handle, rc;

   dev_dbg(salg->dev, "%s: %s\n", __func__, salg->calg->cra_name);

   *tctx = (struct spacc_crypto_ctx) {
      .dev = get_device(salg->dev),
      .handle = -1,
   };

   handle = spacc_open(&priv->spacc, CRYPTO_MODE_NULL, salg->mode->id, -1, 0,
                       spacc_digest_cb, tfm);
   if (handle < 0) {
      dev_dbg(salg->dev, "failed to open SPAcc context\n");
      goto fallback;
   }

   /*
    * As setkey is not called for non-HMAC contexts, we must set something
    * valid to start with.
    */
   rc = spacc_write_context(&priv->spacc, handle, SPACC_HASH_OPERATION,
                                                  NULL, 0, NULL, 0);
   if (rc < 0) {
      dev_dbg(salg->dev, "failed to write SPAcc context\n");
      goto fallback;
   }

   rc = spacc_set_operation(&priv->spacc, handle, OP_ENCRYPT,
                            ICV_HASH, IP_ICV_OFFSET, 0, 0, 0);
   if (rc < 0) {
      spacc_close(&priv->spacc, handle);
      goto fallback;
   }

   tctx->handle    = handle;
   tctx->ctx_valid = true;

fallback:
   tctx->fb.hash = crypto_alloc_ahash(salg->calg->cra_name, 0,
                                      CRYPTO_ALG_NEED_FALLBACK);
   if (IS_ERR(tctx->fb.hash)) {
      if (tctx->handle >= 0) {
         spacc_close(&priv->spacc, tctx->handle);
      }

      put_device(tctx->dev);
      return PTR_ERR(tctx->fb.hash);
   }

   crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
                            sizeof (struct spacc_crypto_reqctx)
                            + crypto_ahash_reqsize(tctx->fb.hash));

   return 0;
}

static void spacc_hash_cra_exit(struct crypto_tfm *tfm)
{
   struct spacc_crypto_ctx *tctx = crypto_tfm_ctx(tfm);
   struct spacc_priv *priv = dev_get_drvdata(tctx->dev);

   dev_dbg(tctx->dev, "%s\n", __func__);

   crypto_free_ahash(tctx->fb.hash);

   if (tctx->handle >= 0) {
      spacc_close(&priv->spacc, tctx->handle);
   }

   put_device(tctx->dev);
}

/*
 * Due to limitations of the SPAcc, the incremental ahash functions are not
 * supported; only the one-shot mode can work.  For the incremental functions,
 * just pass the state directly to the fallback implementation.
 */
static int spacc_hash_init(struct ahash_request *req)
{
   struct crypto_ahash *reqtfm = crypto_ahash_reqtfm(req);
   struct spacc_crypto_ctx *tctx = crypto_ahash_ctx(reqtfm);
   struct spacc_crypto_reqctx *ctx = ahash_request_ctx(req);

   dev_dbg(tctx->dev, "%s\n", __func__);

   ctx->fb.hash_req.base = req->base;
   ahash_request_set_tfm(&ctx->fb.hash_req, tctx->fb.hash);

   return crypto_ahash_init(&ctx->fb.hash_req);
}

static int spacc_hash_update(struct ahash_request *req)
{
   struct crypto_ahash *reqtfm = crypto_ahash_reqtfm(req);
   struct spacc_crypto_ctx *tctx = crypto_ahash_ctx(reqtfm);
   struct spacc_crypto_reqctx *ctx = ahash_request_ctx(req);

   dev_dbg(tctx->dev, "%s\n", __func__);

   ctx->fb.hash_req.base.flags = req->base.flags;
   ctx->fb.hash_req.nbytes     = req->nbytes;
   ctx->fb.hash_req.src        = req->src;

   return crypto_ahash_update(&ctx->fb.hash_req);
}

static int spacc_hash_final(struct ahash_request *req)
{
   struct crypto_ahash *reqtfm = crypto_ahash_reqtfm(req);
   struct spacc_crypto_ctx *tctx = crypto_ahash_ctx(reqtfm);
   struct spacc_crypto_reqctx *ctx = ahash_request_ctx(req);

   dev_dbg(tctx->dev, "%s\n", __func__);

   ctx->fb.hash_req.base.flags = req->base.flags;
   ctx->fb.hash_req.result     = req->result;

   return crypto_ahash_final(&ctx->fb.hash_req);
}

static int spacc_hash_finup(struct ahash_request *req)
{
   struct crypto_ahash *reqtfm = crypto_ahash_reqtfm(req);
   struct spacc_crypto_ctx *tctx = crypto_ahash_ctx(reqtfm);
   struct spacc_crypto_reqctx *ctx = ahash_request_ctx(req);

   dev_dbg(tctx->dev, "%s\n", __func__);

   ctx->fb.hash_req.base.flags = req->base.flags;
   ctx->fb.hash_req.nbytes     = req->nbytes;
   ctx->fb.hash_req.src        = req->src;
   ctx->fb.hash_req.result     = req->result;

   return crypto_ahash_finup(&ctx->fb.hash_req);
}

const struct ahash_alg spacc_hash_template __devinitconst = {
   .init   = spacc_hash_init,
   .update = spacc_hash_update,
   .final  = spacc_hash_final,
   .finup  = spacc_hash_finup,
   .digest = spacc_hash_digest,
   .setkey = spacc_hash_setkey,

   .halg.base = {
      .cra_priority = 300,
      .cra_module   = THIS_MODULE,
      .cra_init     = spacc_hash_cra_init,
      .cra_exit     = spacc_hash_cra_exit,
      .cra_ctxsize  = sizeof (struct spacc_crypto_ctx),
      .cra_flags    = CRYPTO_ALG_TYPE_AHASH
                    | CRYPTO_ALG_ASYNC
                    | CRYPTO_ALG_NEED_FALLBACK
   },
};

int __init spacc_hash_module_init(void)
{
   spacc_hash_pool = dma_pool_create("spacc-digest", NULL,
                                     SPACC_MAX_DIGEST_SIZE,
                                     SPACC_DMA_ALIGN, SPACC_DMA_BOUNDARY);
   if (!spacc_hash_pool)
      return -ENOMEM;

   return 0;
}

void spacc_hash_module_exit(void)
{
   if (spacc_hash_pool)
      dma_pool_destroy(spacc_hash_pool);
}

