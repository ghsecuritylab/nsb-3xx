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
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "elpreuser.h"

struct list {
   char *name;
   uint32_t val;
};

struct list ciphers[] = {
   { "null",      RE_CIPHER_NULL },
   { "aes128cbc", RE_CIPHER_AES128_CBC },
   { "aes256cbc", RE_CIPHER_AES256_CBC },
   { "aes128gcm", RE_CIPHER_AES128_GCM },
   { "aes256gcm", RE_CIPHER_AES256_GCM },
   { "descbc",    RE_CIPHER_DES_CBC },
   { "3descbc",   RE_CIPHER_3DES_CBC },
   { "rc440",     RE_CIPHER_RC4_40 },
   { "rc4128",    RE_CIPHER_RC4_128 },
   { NULL, 0 },
};

struct list hashes[] = {
   { "null",     RE_HASH_NULL },
   { "md5",      RE_HASH_MD5_128 },
   { "md580",    RE_HASH_MD5_80 },
   { "sha1",     RE_HASH_SHA1_160 },
   { "sha180",   RE_HASH_SHA1_80 },
   { "sha256",   RE_HASH_SHA256_256 },
   { NULL, 0 },
};

uint32_t find_list(struct list *foo, char *str)
{
   uint32_t x;
   for (x = 0; foo[x].name != NULL; x++) {
      if (!strcmp(foo[x].name, str)) {
         fprintf(stderr, "Picked %s\n", str);
         return foo[x].val;
      }
   }
   fprintf(stderr, "Invalid option [%s]...\n", str);
   return 0;
}


int worker(uint32_t psize, uint32_t pruns, uint32_t cipher, uint32_t hash, uint32_t usedma, struct elp_spacc_re_usr *fd)
{
   unsigned char in[RE_MAX_SIZE], out[RE_MAX_SIZE];
   uint32_t cnt, outlen, x;
   int err;

   for (cnt = 0; cnt < pruns; cnt++) {
      outlen = RE_MAX_SIZE;
      if (re_dev_do_record(fd, in, psize, in, outlen, 0, 0, RE_DEV_WRITE_MODE) < 0) {
         printf("Failed to run encrypt packet: %d\n", RE_USR_ERR(fd));
         return -1;
      }
   }
   re_dev_close(fd);
}

struct worker_bundle {
   uint32_t psize,  pruns,  cipher,  hash, usedma;
   struct elp_spacc_re_usr fd;
};

void * work_thread(void *d)
{
   struct worker_bundle *w = d;
   printf("Starting thread...\n");
   worker(w->psize, w->pruns, w->cipher, w->hash, w->usedma, &w->fd);
   printf("Thread done...\n");
   return d;
}

int main(int argc, char **argv)
{
   unsigned char in[RE_MAX_SIZE], out[RE_MAX_SIZE], key[256], mackey[256], iv[256], params[2], seq[8];
   uint32_t cnt, outlen, psize, pmode, pruns, cipher, hash, x, threads;
   struct elp_spacc_re_usr fd;
   struct rusage usage;
   uint32_t stime, utime, wtime;
   struct timeval t1, t2;
   struct worker_bundle wb[16];
   pthread_t tid[16];
   int err;

   psize   = 4096;
   pruns   = 8192;
   threads = 1;
   cipher  = RE_CIPHER_AES128_CBC;
   hash    = RE_HASH_SHA1_160;

   for (x = 1; x < argc; x++) {
      if (!strcmp(argv[x], "--size"))   { psize  = atoi(argv[x+1]); ++x; }
      if (!strcmp(argv[x], "--runs"))   { pruns  = atoi(argv[x+1]); ++x; }
      if (!strcmp(argv[x], "--cipher")) { cipher = find_list(ciphers, argv[x+1]); ++x; }
      if (!strcmp(argv[x], "--hash"))   { hash   = find_list(hashes,  argv[x+1]); ++x; }
      if (!strcmp(argv[x], "--threads")) { threads = atoi(argv[x+1]); ++x; }
   }

   if (threads < 1 || threads > 16) {
      threads = 1;
   }

   fprintf(stderr, "Running %zu runs of %zu bytes in %d threads ...\n", pruns, psize, threads);
   gettimeofday(&t1, NULL);

   err = re_dev_open(&fd, 1); // TLS 1.0
   if (err < 0) {
      perror("open");
      return -1;
   }
   re_dev_register(&fd);

   ELP_RE_PARAMS(params, hash, cipher, 0);

   re_dev_set_write_context(&fd, iv, 16, key, 16, mackey, 32, params, 2, seq, 8);
   outlen = RE_MAX_SIZE;
   in[0] = 1;
   re_dev_do_record(&fd, in, 1, out, outlen, 0, 0, RE_DEV_CCS);

   wb[0].psize = psize;
   wb[0].pruns = pruns/threads;
   wb[0].cipher = cipher;
   wb[0].hash   = hash;
   wb[0].usedma = 1;
   wb[0].fd     = fd;

   for (x = 0; x < threads; x++) {
      if (x) {
         wb[x] = wb[0];
         re_dev_open_bind(&fd, &wb[x].fd);
      }
      pthread_create(&tid[x], NULL, &work_thread, &wb[x]);
   }
   for (x = 0; x < threads; x++) {
      pthread_join(tid[x], NULL);
   }

   getrusage(RUSAGE_SELF, &usage);
   gettimeofday(&t2, NULL);
   wtime = (t2.tv_sec*1000000+t2.tv_usec) - (t1.tv_sec*1000000+t1.tv_usec);
   stime = usage.ru_stime.tv_sec*1000000+usage.ru_stime.tv_usec;
   utime = usage.ru_utime.tv_sec*1000000+usage.ru_utime.tv_usec;
   printf("%g,%g,%g,%g,%g,%g,%g\n", (double)wtime/1000000.0, (double)utime/1000000.0, (double)stime/1000000.0, ((double)(utime+stime)/(double)wtime)*100.00, ((double)(pruns*psize*8)/(double)wtime), (double)pruns/((double)wtime/1000000.0), ((double)pruns/((double)wtime/1000000.0))/( ((double)(utime+stime)/(double)wtime)*100.00));

   return 0;
}
