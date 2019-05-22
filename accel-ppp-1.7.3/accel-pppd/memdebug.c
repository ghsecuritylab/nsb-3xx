#undef MEMDEBUG

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <signal.h>

#include "spinlock.h"
#include "list.h"

#define __init __attribute__((constructor))
#define __export __attribute__((visibility("default")))

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})


#define MAGIC1 0x1122334455667788llu

struct mem_t
{
	struct list_head entry;
	const char *fname;
	int line;
	size_t size;
	uint64_t magic2;
	uint64_t magic1;
	char data[0];
};

static LIST_HEAD(mem_list);
static spinlock_t mem_list_lock = SPINLOCK_INITIALIZER;

struct mem_t *_md_malloc(size_t size, const char *fname, int line)
{
	struct mem_t *mem = malloc(sizeof(*mem) + size + 8);

	if (size > 4096)
		line = 0;

	mem->fname = fname;
	mem->line = line;
	mem->size = size;
	mem->magic1 = MAGIC1;
	mem->magic2 = (uint64_t)random() * (uint64_t)random();
	*(uint64_t*)(mem->data + size) = mem->magic2;

	spin_lock(&mem_list_lock);
	list_add_tail(&mem->entry, &mem_list);
	spin_unlock(&mem_list_lock);

	return mem;
}

void __export *md_malloc(size_t size, const char *fname, int line)
{
	struct mem_t *mem = _md_malloc(size, fname, line);

	return mem->data;
}

void __export md_free(void *ptr, const char *fname, int line)
{
	struct mem_t *mem = container_of(ptr, typeof(*mem), data);

	if (!ptr) {
		printf("free null pointer at %s:%i\n", fname, line);
		abort();
	}
	
	if (mem->magic1 != MAGIC1) {
		printf("memory corruption:\nfree at %s:%i\n", fname, line);
		abort();
	}

	if (mem->magic2 != *(uint64_t*)(mem->data + mem->size)) {
		printf("memory corruption:\nmalloc(%lu) at %s:%i\nfree at %s:%i\n", (long unsigned)mem->size, mem->fname, mem->line, fname, line);
		abort();
	}
	
	mem->magic1 = 0;
	mem->magic2 = 0;

	spin_lock(&mem_list_lock);
	list_del(&mem->entry);
	spin_unlock(&mem_list_lock);

	free(mem);
	return;
}

void __export *md_realloc(void *ptr, size_t size, const char *fname, int line)
{
	struct mem_t *mem = container_of(ptr, typeof(*mem), data);
	struct mem_t *mem2;
	
	if (mem->magic1 != MAGIC1) {
		printf("memory corruption:\nfree at %s:%i\n", fname, line);
		abort();
	}

	if (mem->magic2 != *(uint64_t*)(mem->data + mem->size)) {
		printf("memory corruption:\nmalloc(%lu) at %s:%i\nfree at %s:%i\n", (long unsigned)mem->size, mem->fname, mem->line, fname, line);
		abort();
	}

	mem2 = _md_malloc(size, fname, line);
	memcpy(mem2->data, mem->data, mem->size);
	
	md_free(mem->data, fname, line);

	return mem2->data;
}

char __export *md_strdup(const char *ptr, const char *fname, int line)
{
	struct mem_t *mem = _md_malloc(strlen(ptr) + 1, fname, line);
	memcpy(mem->data, ptr, strlen(ptr) + 1);
	return mem->data;
}

char __export *md_strndup(const char *ptr, size_t n, const char *fname, int line)
{
	struct mem_t *mem = _md_malloc(n + 1, fname, line);
	memcpy(mem->data, ptr, n);
	mem->data[n] = 0;
	return mem->data;
}

static void siginfo(int num)
{
	struct mem_t *mem;
	size_t total = 0;

	spin_lock(&mem_list_lock);
	list_for_each_entry(mem, &mem_list, entry) {
		printf("%s:%i %lu\n", mem->fname, mem->line, (long unsigned)mem->size);
		total += mem->size;
	}
	spin_unlock(&mem_list_lock);
	printf("total = %lu\n", (long unsigned)total);
}

static void siginfo2(int num)
{
	struct mem_t *mem;

	spin_lock(&mem_list_lock);
	list_for_each_entry(mem, &mem_list, entry) {
		if (mem->magic1 != MAGIC1 || mem->magic2 != *(uint64_t*)(mem->data + mem->size))
			printf("%s:%i %lu\n", mem->fname, mem->line, (long unsigned)mem->size);
	}
	spin_unlock(&mem_list_lock);
}

void __export md_check(void *ptr)
{
	struct mem_t *mem = container_of(ptr, typeof(*mem), data);

	if (!ptr)
		abort();
	
	if (mem->magic1 != MAGIC1)
		abort();

	if (mem->magic2 != *(uint64_t*)(mem->data + mem->size))
		abort();
}

static void __init init(void)
{
	signal(36, siginfo);
	signal(37, siginfo2);
}
