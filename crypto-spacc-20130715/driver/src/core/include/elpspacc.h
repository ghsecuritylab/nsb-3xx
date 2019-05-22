/*
 * Copyright (c) 2013 Elliptic Technologies Inc.
 */
#ifndef _ELPSPACC_H_
#define _ELPSPACC_H_

#include "elpspaccmodes.h"
#include "elppdu.h"
#include "elpspacchw.h"

enum {
   SPACC_DMA_UNDEF = 0,
   SPACC_DMA_DDT,
   SPACC_DMA_LINEAR
};

enum {
   SPACC_OP_MODE_IRQ = 0,
   SPACC_OP_MODE_WD  = 1, // watchdog
};

#define OP_ENCRYPT          0
#define OP_DECRYPT          1

#define SPACC_CRYPTO_OPERATION   1
#define SPACC_HASH_OPERATION     2

#define SPACC_AADCOPY_FLAG        0x80000000

#define SPACC_AUTO_SIZE (-1)

#define SPACC_WD_LIMIT  0x80


#ifndef MAX_DDT_ENTRIES
   /* add one for null at end of list */
   #define MAX_DDT_ENTRIES (SPACC_MAX_MSG_MALLOC_SIZE / SPACC_MAX_PARTICLE_SIZE) + 1
#endif

#ifndef SPACC_MAX_JOBS
   #define SPACC_MAX_JOBS  (1U<<SPACC_SW_CTRL_ID_W)
#endif

#ifndef SPACC_MAX_JOB_BUFFERS
   #define SPACC_MAX_JOB_BUFFERS 32
#endif

// job descriptor
typedef void (*spacc_callback)(void *spacc_dev, void *data);

typedef struct
{
  unsigned long
      enc_mode,                 // Encription Algorith mode
      hash_mode,                // HASH Algorith mode
      icv_len,
      icv_offset,
      op,                       // Operation
      ctrl,                     // CTRL shadoe register
      first_use,                // indicates that context just has been initialized/taken
                                // and this is the first use
      pre_aad_sz, post_aad_sz,  // size of AAD for the latest packet
      hkey_sz,
      ckey_sz;

   unsigned auxinfo_dir, auxinfo_bit_align; // Direction and bit alignment parameters for the AUX_INFO reg
   unsigned auxinfo_cs_mode; // AUX info setting for CBC-CS

   uint32_t ctx_idx;
   unsigned job_used, job_swid, job_done, job_err, job_secure;

   spacc_callback cb;
   void           *cbdata;

} spacc_job;

#define SPACC_CTX_IDX_UNUSED  0xFFFFFFFF
#define SPACC_JOB_IDX_UNUSED  0xFFFFFFFF

#define DDT_ENTRY_SIZE (sizeof(ddt_entry)*MAX_DDT_ENTRIES)


typedef struct {
   uint32_t *ciph_key;        // Memory context to store cipher keys
   uint32_t *hash_key;        // Memory context to store hash keys
   uint32_t *rc4_key;         // Memory context to store internal RC4 keys
   int      ref_cnt;          // reference count of jobs using this context
   int      ncontig;          // number of contexts following related to this one
} spacc_ctx;

/* forward declaration for callback functions */
struct _spacc_device;

typedef struct _spacc_device {
   void                  *regmap;

   // hardware configuration
   struct {
      unsigned
          version,
          pdu_version,
          project;
      uint32_t
          max_msg_size;    // max PROCLEN value;

      unsigned char
          modes[CRYPTO_MODE_LAST];

      int num_ctx,         // # of contexts
          num_rc4_ctx,     // # of RC4 contexts
          num_sec_ctx,     // # of SKP contexts
          sec_ctx_page_size, // page size of SKP context in bytes
          ciph_page_size,  // cipher context page size in bytes
          hash_page_size,  // hash context page size in bytes
          is_qos,          // QOS spacc?
          is_pdu,          // PDU spacc?
          is_hsm_virtual,  // Is this an HSM Virtual spacc?
          is_hsm_shared,   // Is this an HSM Shared spacc?
          is_secure,       // OR of the previous two bits (basically is this an HSM or not)
          is_secure_port,  // Are we on the secure port?
          is_partial,      // Is partial processing enabled?
          is_ivimport,     // is ivimport enabled?
          dma_type,        // Which type of DMA linear or scattergather
          idx,             // Which virtual spacc IDX is this?
          ctx_mask,        // CTX mask used by shared HSM
          cmd0_fifo_depth, // CMD FIFO depths
          cmd1_fifo_depth,
          cmd2_fifo_depth,
          stat_fifo_depth, // depth of STATUS FIFO
          fifo_cnt,
          ideal_stat_level;
      uint32_t
          wd_timer;
   } config;

   struct spacc_job_buffer {
      int active;
      int job_idx;
      pdu_ddt *src, *dst;
      uint32_t proc_sz, aad_offset, pre_aad_sz, post_aad_sz, iv_offset, prio;
   } job_buffer[SPACC_MAX_JOB_BUFFERS];
   int job_buffer_use;

   int op_mode,            // operating mode and watchdog functionality
       wdcnt,              // number of pending WD IRQs
       job_tally;          // # of jobs not dequeued

   spacc_ctx *ctx;        // This size changes per configured device
   spacc_job *job;        // allocate memory for [SPACC_MAX_JOBS];
   int job_lookup[SPACC_MAX_JOBS];  // correlate SW_ID back to job index

   PDU_LOCK_TYPE lock;           // lock for register access
   PDU_LOCK_TYPE ctx_lock;       // lock for context manager

   /* callback functions for IRQ processing */
   void (*irq_cb_cmdx)(struct _spacc_device *spacc, int x);
   void (*irq_cb_stat)(struct _spacc_device *spacc);
   void (*irq_cb_stat_wd)(struct _spacc_device *spacc);
   void (*irq_cb_rc4_dma)(struct _spacc_device *spacc);

   // this is called after jobs have been popped off the STATUS FIFO
   // useful so you can be told when there might be space available in the CMD FIFO
   void (*spacc_notify_jobs)(struct _spacc_device *spacc);

   // cache
   struct {
      uint32_t src_ptr,
               dst_ptr,
               proc_len,
               icv_len,
               icv_offset,
               pre_aad,
               post_aad,
               iv_offset,
               offset,
               aux;
   } cache;
} spacc_device;

extern int spacc_endian;

int spacc_init(void *baseaddr, spacc_device *spacc, pdu_info *info);
void spacc_fini(spacc_device *spacc);


int spacc_request_hsm_semaphore(spacc_device *spacc);
void spacc_free_hsm_semaphore(spacc_device *spacc);

int spacc_load_skp(spacc_device *spacc, uint32_t *key, int keysz, int idx, int alg, int mode, int size, int enc, int dec);
void spacc_set_secure_mode (spacc_device *spacc, int src, int dst, int ddt, int global_lock);
void spacc_set_debug (int dflag);
unsigned char * spacc_error_msg (int err);
void spacc_dump_ctx(spacc_device *spacc, int ctx);

int spacc_open (spacc_device *spacc, int enc, int hash, int ctx, int secure_mode, spacc_callback cb, void *cbdata);
int spacc_clone_handle(spacc_device *spacc, int old_handle, void *cbdata);
int spacc_close (spacc_device *spacc, int job_idx);

int spacc_set_operation (spacc_device *spacc, int job_idx, int op, uint32_t prot, uint32_t icvcmd, uint32_t icvoff, uint32_t icvsz, uint32_t sec_key);
int spacc_set_key_exp(spacc_device *spacc, int job_idx);

int spacc_set_auxinfo (spacc_device *spacc, int job_idx, uint32_t direction, uint32_t bitsize);
void spacc_virtual_set_weight(spacc_device *spacc, int weight);
int spacc_virtual_request_rc4(spacc_device *spacc);
int spacc_packet_enqueue_ddt_ex(spacc_device *spacc, int use_jb, int job_idx, pdu_ddt *src_ddt, pdu_ddt *dst_ddt, uint32_t proc_sz, uint32_t aad_offset, uint32_t pre_aad_sz, uint32_t post_aad_sz, uint32_t iv_offset, uint32_t prio);
int spacc_packet_enqueue_ddt(spacc_device *spacc, int job_idx, pdu_ddt *src_ddt, pdu_ddt *dst_ddt, uint32_t proc_sz, uint32_t aad_offset, uint32_t pre_aad_sz, uint32_t post_aad_sz, uint32_t iv_offset, uint32_t prio);
int spacc_pop_packets (spacc_device * spacc, int *num_popped);
int spacc_pop_packets_ex (spacc_device * spacc, int *num_popped, unsigned long *lock_flag);
int spacc_packet_dequeue (spacc_device *spacc, int job_idx);

uint32_t spacc_get_version (void);

/* IRQ handling functions */
void spacc_irq_cmdx_enable (spacc_device *spacc, int cmdx, int cmdx_cnt);
void spacc_irq_cmdx_disable (spacc_device *spacc, int cmdx);
void spacc_irq_stat_enable (spacc_device *spacc, int stat_cnt);
void spacc_irq_stat_disable (spacc_device *spacc);
void spacc_irq_stat_wd_enable (spacc_device *spacc);
void spacc_irq_stat_wd_disable (spacc_device *spacc);
void spacc_irq_rc4_dma_enable (spacc_device *spacc);
void spacc_irq_rc4_dma_disable (spacc_device *spacc);
void spacc_irq_glbl_enable (spacc_device *spacc);
void spacc_irq_glbl_disable (spacc_device *spacc);
uint32_t spacc_process_irq(spacc_device *spacc);
void spacc_set_wd_count(spacc_device *spacc, uint32_t val);

/* Context Manager */
void spacc_ctx_init_all (spacc_device *spacc);
int spacc_ctx_request (spacc_device *dev, int ctx_idx, int ncontig);
int spacc_ctx_release (spacc_device *dev, int ctx_idx);

/* SPAcc specific manipulation of context memory */
int spacc_write_context (spacc_device *spacc, int job_idx, int op, const unsigned char * key, int ksz, const unsigned char * iv, int ivsz);
int spacc_read_context (spacc_device * spacc, int job_idx, int op, unsigned char * key, int ksz, unsigned char * iv, int ivsz);
int spacc_write_rc4_context (spacc_device *spacc, int job_idx, unsigned char i, unsigned char j, const unsigned char *ctxdata);
int spacc_read_rc4_context (spacc_device *spacc, int job_idx, unsigned char *i, unsigned char *j, unsigned char *ctxdata);


/* Job Manager */
void spacc_job_init_all (spacc_device *spacc);
int spacc_job_request (spacc_device * dev, int job_idx);
int spacc_job_release (spacc_device * dev, int job_idx);
/* helper functions */
spacc_ctx *context_lookup_by_job (spacc_device *spacc, int job_idx);
spacc_job *job_lookup_by_swid (spacc_device * spacc, int swid);

int spacc_autodetect(spacc_device *spacc);
int spacc_isenabled(spacc_device *spacc, int mode, int keysize);
int spacc_compute_xcbc_key(spacc_device *spacc, const unsigned char *key, int keylen, unsigned char *xcbc_out);

#ifdef __KERNEL__
/* provided by kernel side... */
extern int spacc_endian;
#endif

#endif
