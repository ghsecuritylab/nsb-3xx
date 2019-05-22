#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "triton_p.h"

#include "memdebug.h"

extern int max_events;

static int epoll_fd;
static struct epoll_event *epoll_events;

static pthread_t md_thr;
static void *md_thread(void *arg);

static mempool_t *md_pool;

static pthread_mutex_t freed_list_lock = PTHREAD_MUTEX_INITIALIZER;
static LIST_HEAD(freed_list);
static LIST_HEAD(freed_list2);

int md_init(void)
{
	epoll_fd = epoll_create(1);
	if (epoll_fd < 0) {
		perror("md:epoll_create");
		return -1;
	}

	epoll_events = _malloc(max_events * sizeof(struct epoll_event));
	if (!epoll_events) {
		fprintf(stderr,"md:cann't allocate memory\n");
		return -1;
	}

	md_pool = mempool_create(sizeof(struct _triton_md_handler_t));

	return 0;
}
void md_run(void)
{
	if (pthread_create(&md_thr, NULL, md_thread, NULL)) {
		triton_log_error("md:pthread_create: %s", strerror(errno));
		_exit(-1);
	}
}

void md_terminate(void)
{
	pthread_cancel(md_thr);	
	pthread_join(md_thr, NULL);	
}

static void *md_thread(void *arg)
{
	int i,n,r;
	struct _triton_md_handler_t *h;
	sigset_t set;

	sigfillset(&set);
	sigdelset(&set, SIGKILL);
	sigdelset(&set, SIGSTOP);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	while(1) {
		n = epoll_wait(epoll_fd, epoll_events, max_events, -1);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			triton_log_error("md:epoll_wait: %s", strerror(errno));
			_exit(-1);
		}
		
		for(i = 0; i < n; i++) {
			h = (struct _triton_md_handler_t *)epoll_events[i].data.ptr;
			if (!h->ud)
				continue;
			spin_lock(&h->ctx->lock);
			if (h->ud) {
				h->trig_epoll_events |= epoll_events[i].events;
				if (!h->pending) {
					list_add_tail(&h->entry2, &h->ctx->pending_handlers);
					h->pending = 1;
					__sync_add_and_fetch(&triton_stat.md_handler_pending, 1);
					r = triton_queue_ctx(h->ctx);
				} else
					r = 0;
			} else
				r = 0;
			spin_unlock(&h->ctx->lock);
			if (r)
				triton_thread_wakeup(h->ctx->thread);
		}

		while (!list_empty(&freed_list2)) {
			h = list_entry(freed_list2.next, typeof(*h), entry);
			list_del(&h->entry);
			mempool_free(h);
		}
		
		pthread_mutex_lock(&freed_list_lock);
		while (!list_empty(&freed_list)) {
			h = list_entry(freed_list.next, typeof(*h), entry);
			list_del(&h->entry);
			list_add(&h->entry, &freed_list2);
		}
		pthread_mutex_unlock(&freed_list_lock);
	}

	return NULL;
}

void __export triton_md_register_handler(struct triton_context_t *ctx, struct triton_md_handler_t *ud)
{
	struct _triton_md_handler_t *h = mempool_alloc(md_pool);
	memset(h, 0, sizeof(*h));
	h->ud = ud;
	h->epoll_event.data.ptr = h;
	if (ctx)
		h->ctx = (struct _triton_context_t *)ctx->tpd;
	else
		h->ctx = (struct _triton_context_t *)default_ctx.tpd;
	ud->tpd = h;
	spin_lock(&h->ctx->lock);
	list_add_tail(&h->entry, &h->ctx->handlers);
	spin_unlock(&h->ctx->lock);

	triton_stat.md_handler_count++;
}
void __export triton_md_unregister_handler(struct triton_md_handler_t *ud)
{
	struct _triton_md_handler_t *h = (struct _triton_md_handler_t *)ud->tpd;
	triton_md_disable_handler(ud, MD_MODE_READ | MD_MODE_WRITE);
	
	spin_lock(&h->ctx->lock);
	h->ud = NULL;
	list_del(&h->entry);
	if (h->pending) {
		list_del(&h->entry2);
		__sync_sub_and_fetch(&triton_stat.md_handler_pending, 1);
	}
	spin_unlock(&h->ctx->lock);

	sched_yield();

	pthread_mutex_lock(&freed_list_lock);
	list_add_tail(&h->entry, &freed_list);
	pthread_mutex_unlock(&freed_list_lock);

	ud->tpd = NULL;

	triton_stat.md_handler_count--;
}
int __export triton_md_enable_handler(struct triton_md_handler_t *ud, int mode)
{
	struct _triton_md_handler_t *h = (struct _triton_md_handler_t *)ud->tpd;
	int r;
	int events = h->epoll_event.events;

	if (mode & MD_MODE_READ)
		h->epoll_event.events |= EPOLLIN;
	if (mode & MD_MODE_WRITE)
		h->epoll_event.events |= EPOLLOUT;
	
	if (!h->trig_level)
		h->epoll_event.events |= EPOLLET;
	
	if (events)
		r = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, h->ud->fd, &h->epoll_event);
	else
		r = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, h->ud->fd, &h->epoll_event);

	if (r) {
		triton_log_error("md:epoll_ctl: %s",strerror(errno));
		abort();
	}

	return r;
}
int __export triton_md_disable_handler(struct triton_md_handler_t *ud,int mode)
{
	struct _triton_md_handler_t *h = (struct _triton_md_handler_t *)ud->tpd;
	int r=0;

	if (!h->epoll_event.events)
		return -1;
	
	if (mode & MD_MODE_READ)
		h->epoll_event.events &= ~EPOLLIN;
	if (mode & MD_MODE_WRITE)
		h->epoll_event.events &= ~EPOLLOUT;

	if (h->epoll_event.events & (EPOLLIN | EPOLLOUT))
		r = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, h->ud->fd, &h->epoll_event);
	else {
		h->epoll_event.events = 0;
		r = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, h->ud->fd, NULL);
	}

	if (r) {
		triton_log_error("md:epoll_ctl: %s",strerror(errno));
		abort();
	}

	return r;
}

void __export triton_md_set_trig(struct triton_md_handler_t *ud, int mode)
{
	struct _triton_md_handler_t *h = (struct _triton_md_handler_t *)ud->tpd;
	h->trig_level = mode;
}

