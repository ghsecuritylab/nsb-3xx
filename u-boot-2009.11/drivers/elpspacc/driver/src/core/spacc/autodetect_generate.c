/*
 * Copyright (c) 2013 Elliptic Technologies Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// This program uses Ellipsys to generate the KAT used by the autodetect logic.
#include "elpsoft_defines.h"
#include <elpsoft.h>

void dump(char *s, unsigned char *p, int n)
{
   int x;

   printf("//%s\n { ", s);
   for (x = 0; x < n; x++) {
      printf("0x%02x, ", p[x]);
   }
   printf(" }\n");
}

int ks[4] = { 16, 32, 64, 128 };  // hash key sizes


int sslmac_memory(const elphash_plugin *hash, const unsigned char * mackey, unsigned long maclen, const unsigned char *inbuff, unsigned long inlen, unsigned char * outbuff, unsigned long *outlen)
{
  unsigned long length;
  unsigned long padding;
  unsigned char intermediate_hash[80];
  unsigned char macpad[48];
  elphash_state md;

  //Set up the md5 or sha1
  if (strcmp(hash->OID.name, "md5") == 0){
    padding = 48;
    length = 16;
  } else if (strcmp(hash->OID.name, "sha1") == 0){
    padding = 40;
    length = 20;
  }
  //set the mac padding to 0x36
  memset(macpad, 0x36, padding);

  //Hash all of the inputs together, the mackey, the padding and the input
  hash->init(0, &md);
  hash->process(mackey, maclen,  &md);
  hash->process(macpad, padding, &md);
  hash->process(inbuff, inlen, &md);
  hash->done(intermediate_hash + padding + length, &length, &md);

  //set everything up in the intermediate_buffer
  memcpy(intermediate_hash, mackey, length);
  memset(intermediate_hash + length, 0x5C, padding);

  //Hash the intermediate buffer, and append it to the output
  elphash_memory(hash, intermediate_hash, 80, outbuff, outlen);
  return ELPEOK;
}

int main(void)
{
   unsigned char buf[256], key[128];
   elpprng_state prng;
   elpsymmetric_key sk;
   elpsymmetric_cbc cbc;
   elpsymmetric_ctr ctr;
   elpsymmetric_ofb ofb;
   elpsymmetric_cfb cfb;
   elpsymmetric_f8  f8;
   elpsymmetric_xts xex;
   unsigned long taglen;

   int x;
   for (x = 0; x < 128; x++) key[x] = x;

   // RC4_40
      memset(buf, 0, sizeof buf);
      elprc4_start(&prng);
      elprc4_add_entropy(key, 5, &prng);
      elprc4_ready(&prng);
      elprc4_read(buf, 16, &prng);
      dump("RC4_40", buf, 16);

   // RC4_128
      memset(buf, 0, sizeof buf);
      elprc4_start(&prng);
      elprc4_add_entropy(key, 16, &prng);
      elprc4_ready(&prng);
      elprc4_read(buf, 16, &prng);
      dump("RC4_128", buf, 16);

   // AESECB 128/192/256
         memset(buf, 0, sizeof buf);
         elpaes_setup(key, 16, 0, &sk);
         elpaes_encrypt(buf, buf, &sk);
         dump("AESECB_128", buf, 16);

         memset(buf, 0, sizeof buf);
         elpaes_setup(key, 24, 0, &sk);
         elpaes_encrypt(buf, buf, &sk);
         dump("AESECB_192", buf, 16);

         memset(buf, 0, sizeof buf);
         elpaes_setup(key, 32, 0, &sk);
         elpaes_encrypt(buf, buf, &sk);
         dump("AESECB_256", buf, 16);

   // AESCBC 128/192/256
         memset(buf, 0, sizeof buf);
         elpcbc_start_ex(&elpaes_plugin, key, 16, 0, key, &cbc);
         elpcbc_encrypt(buf, buf, 1, &cbc);
         dump("AESCBC_128", buf, 16);

         memset(buf, 0, sizeof buf);
         elpcbc_start_ex(&elpaes_plugin, key, 24, 0, key, &cbc);
         elpcbc_encrypt(buf, buf, 1, &cbc);
         dump("AESCBC_192", buf, 16);

         memset(buf, 0, sizeof buf);
         elpcbc_start_ex(&elpaes_plugin, key, 32, 0, key, &cbc);
         elpcbc_encrypt(buf, buf, 1, &cbc);
         dump("AESCBC_256", buf, 16);

   // AESCTR 128/192/256
         memset(buf, 0, sizeof buf);
         elpctr_start_ex(&elpaes_plugin, key, 16, key, 0, ELPCTRBIG|2, &ctr);
         elpctr_process(buf, buf, 16, &ctr);
         dump("AESCTR_128", buf, 16);

         memset(buf, 0, sizeof buf);
         elpctr_start_ex(&elpaes_plugin, key, 24, key, 0, ELPCTRBIG|2, &ctr);
         elpctr_process(buf, buf, 16, &ctr);
         dump("AESCTR_192", buf, 16);

         memset(buf, 0, sizeof buf);
         elpctr_start_ex(&elpaes_plugin, key, 32, key, 0, ELPCTRBIG|2, &ctr);
         elpctr_process(buf, buf, 16, &ctr);
         dump("AESCTR_256", buf, 16);

   // AESCCM
         memset(buf, 0, sizeof buf);
         taglen = 16; elpccm_memory_ex(&elpaes_plugin, key, 16, NULL, "", 0, NULL, 0, buf, 16, buf, buf + 16, &taglen, ELP_CCM_ENCRYPT, buf, key, 2);
         dump("AESCCM_16", buf, 32);

         memset(buf, 0, sizeof buf);
         taglen = 16; elpccm_memory_ex(&elpaes_plugin, key, 24, NULL, "", 0, NULL, 0, buf, 16, buf, buf + 16, &taglen, ELP_CCM_ENCRYPT, buf, key, 2);
         dump("AESCCM_24", buf, 32);

         memset(buf, 0, sizeof buf);
         taglen = 16; elpccm_memory_ex(&elpaes_plugin, key, 32, NULL, "", 0, NULL, 0, buf, 16, buf, buf + 16, &taglen, ELP_CCM_ENCRYPT, buf, key, 2);
         dump("AESCCM_32", buf, 32);

   // AESGCM
         memset(buf, 0, sizeof buf);
         taglen = 16; elpgcm_memory(&elpaes_plugin, key, 16, NULL, key, 12, buf, 16, buf, 16, buf, buf+16, &taglen, ELP_GCM_ENCRYPT);
         dump("AESGCM_16", buf, 32);


         memset(buf, 0, sizeof buf);
         taglen = 16; elpgcm_memory(&elpaes_plugin, key, 24, NULL, key, 12, buf, 16, buf, 16, buf, buf+16, &taglen, ELP_GCM_ENCRYPT);
         dump("AESGCM_24", buf, 32);

         memset(buf, 0, sizeof buf);
         taglen = 16; elpgcm_memory(&elpaes_plugin, key, 32, NULL, key, 12, buf, 16, buf, 16, buf, buf+16, &taglen, ELP_GCM_ENCRYPT);
         dump("AESGCM_32", buf, 32);

   // AES CBC-CSx
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_1, key, 16, 0, key, buf, 31, buf); dump("AESCS1_128", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_1, key, 24, 0, key, buf, 31, buf); dump("AESCS1_224", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_1, key, 32, 0, key, buf, 31, buf); dump("AESCS1_256", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_2, key, 16, 0, key, buf, 31, buf); dump("AESCS2_128", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_2, key, 24, 0, key, buf, 31, buf); dump("AESCS2_224", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_2, key, 32, 0, key, buf, 31, buf); dump("AESCS2_256", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_3, key, 16, 0, key, buf, 31, buf); dump("AESCS3_128", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_3, key, 24, 0, key, buf, 31, buf); dump("AESCS3_224", buf, 31);
         memset(buf, 0, sizeof buf); elpcbc_cs_encrypt(&elpaes_plugin, ELP_CBCCS_3, key, 32, 0, key, buf, 31, buf); dump("AESCS3_256", buf, 31);

   // AES XTS
         memset(buf, 0, sizeof buf);
         elpxex_start(&elpaes_plugin, key, key+16, 16, 0, &xex);
         elpxex_encrypt(buf, 32*8, buf, key, &xex);
         dump("AESXTS_128", buf, 32); XDUMP();

         memset(buf, 0, sizeof buf);
         elpxex_start(&elpaes_plugin, key, key+32, 32, 0, &xex);
         elpxex_encrypt(buf, 32*8, buf, key, &xex);
         dump("AESXTS_256", buf, 32);

    // DES/3DES CBC
         memset(buf, 0, sizeof buf);
         elpcbc_start_ex(&elpdes_plugin, key, 8, 0, key, &cbc);
         elpcbc_encrypt(buf, buf, 2, &cbc);
         dump("DESCBC_64", buf, 16);

         memset(buf, 0, sizeof buf);
         elpcbc_start_ex(&elptdes_plugin, key, 24, 0, key, &cbc);
         elpcbc_encrypt(buf, buf, 2, &cbc);
         dump("DESCBC_168", buf, 16);

    // DES/3DES ECB
         memset(buf, 0, sizeof buf);
         elpdes_setup(key, 8, 0, &sk);
         elpdes_encrypt(buf, buf, &sk);
         elpdes_encrypt(buf+8, buf+8, &sk);
         dump("DESECB_64", buf, 16);

         memset(buf, 0, sizeof buf);
         elptdes_setup(key, 24, 0, &sk);
         elptdes_encrypt(buf, buf, &sk);
         elptdes_encrypt(buf+8, buf+8, &sk);
         dump("DESECB_168", buf, 16);

    // KASUMI ECB
         memset(buf, 0, sizeof buf);
         elpkasumi_setup(key, 16, 0, &sk);
         elpkasumi_encrypt(buf, buf, &sk);
         elpkasumi_encrypt(buf+8, buf+8, &sk);
         dump("KASUMIECB_128", buf, 16);

    // KASUMI F8
         memset(buf, 0, sizeof buf);
         elpf8_start(&elpkasumi_plugin, key, 16, key, NULL, 0, 0, &f8);
         elpf8_encrypt(buf, buf, 32, &f8);
         dump("KASUMIF8_128", buf, 32);

   // SNOW3G
      memset(buf, 0, sizeof buf);
      elpsnow3g_start(&prng);
      elpsnow3g_add_entropy(key, 32, &prng);
      elpsnow3g_ready(&prng);
      elpsnow3g_read(buf, 16, &prng);
      dump("SNOW3G", buf, 16);

   // ZUC
      memset(buf, 0, sizeof buf);
      elpzuc_start(&prng);
      elpzuc_add_entropy(key, 32, &prng);
      elpzuc_ready(&prng);
      elpzuc_read(buf, 16, &prng);
      dump("ZUC", buf, 16);


// hashes
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpmd5_plugin, buf, 32, buf, &taglen);         dump("MD5_HASH", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpsha1_plugin, buf, 32, buf, &taglen);         dump("SHA1_HASH", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpsha224_plugin, buf, 32, buf, &taglen);         dump("SHA224_HASH", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpsha256_plugin, buf, 32, buf, &taglen);         dump("SHA256_HASH", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpsha384_plugin, buf, 32, buf, &taglen);         dump("SHA384_HASH", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpsha512_plugin, buf, 32, buf, &taglen);         dump("SHA512_HASH", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpsha512_224_plugin, buf, 32, buf, &taglen);         dump("SHA512_224_HASH", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elphash_memory(&elpsha512_256_plugin, buf, 32, buf, &taglen);         dump("SHA512_256_HASH", buf, taglen);


         for (x = 0; x < 3; x++) {
            printf("KS == %d\n", 8 * ks[x]);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpmd5_plugin, key, ks[x], buf, 32, buf, &taglen); dump("MD5_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha1_plugin, key, ks[x], buf, 32, buf, &taglen); dump("SHA1_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha224_plugin, key, ks[x], buf, 32, buf, &taglen); dump("SHA224_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha256_plugin, key, ks[x], buf, 32, buf, &taglen); dump("SHA256_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha384_plugin, key, ks[x], buf, 32, buf, &taglen); dump("SHA384_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha512_plugin, key, ks[x], buf, 32, buf, &taglen); dump("SHA512_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha512_224_plugin, key, ks[x], buf, 32, buf, &taglen); dump("SHA512_224_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha512_256_plugin, key, ks[x], buf, 32, buf, &taglen); dump("SHA512_256_HMAC", buf, taglen);
          }
          printf("KS == 1024\n");
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha384_plugin, key, ks[3], buf, 32, buf, &taglen); dump("SHA384_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha512_plugin, key, ks[3], buf, 32, buf, &taglen); dump("SHA512_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha512_224_plugin, key, ks[3], buf, 32, buf, &taglen); dump("SHA512_224_HMAC", buf, taglen);
            memset(buf, 0, sizeof buf); taglen = 64; elphmac_memory(&elpsha512_256_plugin, key, ks[3], buf, 32, buf, &taglen); dump("SHA512_256_HMAC", buf, taglen);

         memset(buf, 0, sizeof buf); taglen = 64; sslmac_memory(&elpmd5_plugin, key, 16, buf, 32, buf, &taglen); dump("SSLMAC_MD5", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; sslmac_memory(&elpsha1_plugin, key, 20, buf, 32, buf, &taglen); dump("SSLMAC_SHA1", buf, taglen);


// MACs
   // XCBC
         memset(buf, 0, sizeof buf); taglen = 64; elpxcbc_memory(&elpaes_plugin, key, 48 | ELP_XCBC_3KEY, NULL, buf, 32, buf, &taglen); dump("XCBC_128", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elpxcbc_memory(&elpaes_plugin, key, 56 | ELP_XCBC_3KEY, NULL, buf, 32, buf, &taglen); dump("XCBC_192", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elpxcbc_memory(&elpaes_plugin, key, 64 | ELP_XCBC_3KEY, NULL, buf, 32, buf, &taglen); dump("XCBC_256", buf, taglen);

   // CMAC
         memset(buf, 0, sizeof buf); taglen = 64; elpcmac_memory(&elpaes_plugin, key, 16, NULL, buf, 32, buf, &taglen); dump("AES_CMAC_128", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elpcmac_memory(&elpaes_plugin, key, 24, NULL, buf, 32, buf, &taglen); dump("AES_CMAC_192", buf, taglen);
         memset(buf, 0, sizeof buf); taglen = 64; elpcmac_memory(&elpaes_plugin, key, 32, NULL, buf, 32, buf, &taglen); dump("AES_CMAC_256", buf, taglen);

   // KASUMI F9 (note: need to append byte indicating alignment or somesuch)
   memset(buf, 0, sizeof buf); buf[32] = 0x40; taglen = 64; elpf9_memory(&elpkasumi_plugin, key, 16, NULL, buf, 32+1, buf, &taglen); dump("KASUMI_F9_128", buf, taglen);

   // SNOW3G UIA2
   memset(buf, 0, sizeof buf);
   taglen = 16;
   elpsnow3g_uia_memory(key, key+16, buf, 32*8, buf);
   dump("SNOW3G_UIA", buf, 4);

   // ZUC UIA3
   memset(buf, 0, sizeof buf);
   taglen = 16;
   elp128_eia3_memory(&elpzuc_plugin, key, 16, key+16, buf, 32*8, buf);
   dump("ZUC_UIA", buf, 4);
}

