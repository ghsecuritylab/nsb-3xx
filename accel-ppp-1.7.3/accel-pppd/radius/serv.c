#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"
#include "triton.h"
#include "events.h"
#include "cli.h"
#include "utils.h"
#include "radius_p.h"

#include "memdebug.h"

static int num;
static LIST_HEAD(serv_list);

static void __free_server(struct rad_server_t *);

static struct rad_server_t *__rad_server_get(int type, struct rad_server_t *exclude)
{
	struct rad_server_t *s, *s0 = NULL;
	struct timespec ts;
	
	clock_gettime(CLOCK_MONOTONIC, &ts);

	list_for_each_entry(s, &serv_list, entry) {
		if (s == exclude)
			continue;

		if (s->fail_time && ts.tv_sec < s->fail_time)
			continue;

		if (type == RAD_SERV_AUTH && !s->auth_port)
			continue;
		else if (type == RAD_SERV_ACCT && !s->acct_port)
			continue;

		if (!s0) {
			s0 = s;
			continue;
		}

		if (s->client_cnt[type] < s0->client_cnt[type])
			s0 = s;
	}

	if (!s0)
		return NULL;

	__sync_add_and_fetch(&s0->client_cnt[type], 1);

	return s0;
}

struct rad_server_t *rad_server_get(int type)
{
	return __rad_server_get(type, NULL);
}

void rad_server_put(struct rad_server_t *s, int type)
{
	__sync_sub_and_fetch(&s->client_cnt[type], 1);

	if (s->need_free && !s->client_cnt[0] && !s->client_cnt[1])
		__free_server(s);
}

int rad_server_req_enter(struct rad_req_t *req)
{
	struct timespec ts;
	//struct rad_req_t *r = NULL;
	
	if (req->serv->need_free)
		return -1;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	if (ts.tv_sec < req->serv->fail_time)
		return -1;

	if (!req->serv->req_limit)
		return 0;

	pthread_mutex_lock(&req->serv->lock);
	
	if (ts.tv_sec < req->serv->fail_time) {
		pthread_mutex_unlock(&req->serv->lock);
		return -1;
	}

	if (req->serv->req_cnt >= req->serv->req_limit) {
		list_add_tail(&req->entry, &req->serv->req_queue);
		req->serv->queue_cnt++;

		pthread_mutex_unlock(&req->serv->lock);
		triton_context_schedule();
		pthread_mutex_lock(&req->serv->lock);

		clock_gettime(CLOCK_MONOTONIC, &ts);

		req->serv->queue_cnt--;
		if (ts.tv_sec < req->serv->fail_time || req->serv->need_free) {
			/*if (!list_empty(&req->serv->req_queue)) {
				r = list_entry(req->serv->req_queue.next, typeof(*r), entry);
				list_del(&r->entry);
			}*/
			pthread_mutex_unlock(&req->serv->lock);

			/*if (r)
				triton_context_wakeup(r->rpd->ppp->ctrl->ctx);*/

			return -1;
		}
	}

	req->serv->req_cnt++;
	pthread_mutex_unlock(&req->serv->lock);
	
	return 0;
}

void rad_server_req_exit(struct rad_req_t *req)
{
	struct rad_req_t *r = NULL;
	
	if (!req->serv->req_limit)
		return;

	pthread_mutex_lock(&req->serv->lock);
	req->serv->req_cnt--;
	if (req->serv->req_cnt < req->serv->req_limit && !list_empty(&req->serv->req_queue)) {
		r = list_entry(req->serv->req_queue.next, typeof(*r), entry);
		list_del(&r->entry);
	}
	pthread_mutex_unlock(&req->serv->lock);

	if (r)
		triton_context_wakeup(r->rpd->ppp->ctrl->ctx);
}

int rad_server_realloc(struct rad_req_t *req)
{
	struct rad_server_t *s = __rad_server_get(req->type, req->serv);

	if (!s)
		return -1;

	if (req->serv)
		rad_server_put(req->serv, req->type);

	req->serv = s;

	if (req->hnd.fd != -1) {
		if (req->hnd.tpd)
			triton_md_unregister_handler(&req->hnd);
		close(req->hnd.fd);
		req->hnd.fd = -1;
	}

	req->server_addr = req->serv->addr;
	if (req->type == RAD_SERV_ACCT)
		req->server_port = req->serv->acct_port;
	else
		req->server_port = req->serv->auth_port;

	return 0;
}

void rad_server_fail(struct rad_server_t *s)
{
	struct rad_req_t *r;
	struct timespec ts;
	
	clock_gettime(CLOCK_MONOTONIC, &ts);

	pthread_mutex_lock(&s->lock);

	if (ts.tv_sec > s->fail_time) {
		s->fail_time = ts.tv_sec + s->conf_fail_time;
		log_ppp_warn("radius: server(%i) not responding\n", s->id);
		log_warn("radius: server(%i) not responding\n", s->id);
	}

	if (s->conf_fail_time) {
		while (!list_empty(&s->req_queue)) {
			r = list_entry(s->req_queue.next, typeof(*r), entry);
			list_del(&r->entry);
			triton_context_wakeup(r->rpd->ppp->ctrl->ctx);
		}
	}

	pthread_mutex_unlock(&s->lock);
}

void rad_server_timeout(struct rad_server_t *s)
{
	if (__sync_add_and_fetch(&s->timeout_cnt, 1) >= conf_max_try)
		rad_server_fail(s);
}

void rad_server_reply(struct rad_server_t *s)
{
	__sync_synchronize();
	s->timeout_cnt = 0;
}


static void show_stat(struct rad_server_t *s, void *client)
{
	char addr[17];
	struct timespec ts;

	u_inet_ntoa(s->addr, addr);
	clock_gettime(CLOCK_MONOTONIC, &ts);

	cli_sendv(client, "radius(%i, %s):\r\n", s->id, addr);

	if (s->conf_fail_time > 1) {
		if (ts.tv_sec < s->fail_time)
			cli_send(client, "  state: failed\r\n");
		else
			cli_send(client, "  state: active\r\n");

		cli_sendv(client, "  fail count: %lu\r\n", s->stat_fail_cnt);
	}
		
	cli_sendv(client, "  request count: %i\r\n", s->req_cnt);
	cli_sendv(client, "  queue length: %i\r\n", s->queue_cnt);

	if (s->auth_port) {
		cli_sendv(client, "  auth sent: %lu\r\n", s->stat_auth_sent);
		cli_sendv(client, "  auth lost(total/5m/1m): %lu/%lu/%lu\r\n",
			s->stat_auth_lost, stat_accm_get_cnt(s->stat_auth_lost_5m), stat_accm_get_cnt(s->stat_auth_lost_1m));
		cli_sendv(client, "  auth avg query time(5m/1m): %lu/%lu ms\r\n",
			stat_accm_get_avg(s->stat_auth_query_5m), stat_accm_get_avg(s->stat_auth_query_1m));
	}

	if (s->acct_port) {
		cli_sendv(client, "  acct sent: %lu\r\n", s->stat_acct_sent);
		cli_sendv(client, "  acct lost(total/5m/1m): %lu/%lu/%lu\r\n",
			s->stat_acct_lost, stat_accm_get_cnt(s->stat_acct_lost_5m), stat_accm_get_cnt(s->stat_acct_lost_1m));
		cli_sendv(client, "  acct avg query time(5m/1m): %lu/%lu ms\r\n",
			stat_accm_get_avg(s->stat_acct_query_5m), stat_accm_get_avg(s->stat_acct_query_1m));

		cli_sendv(client, "  interim sent: %lu\r\n", s->stat_interim_sent);
		cli_sendv(client, "  interim lost(total/5m/1m): %lu/%lu/%lu\r\n",
			s->stat_interim_lost, stat_accm_get_cnt(s->stat_interim_lost_5m), stat_accm_get_cnt(s->stat_interim_lost_1m));
		cli_sendv(client, "  interim avg query time(5m/1m): %lu/%lu ms\r\n",
			stat_accm_get_avg(s->stat_interim_query_5m), stat_accm_get_avg(s->stat_interim_query_1m));
	}
}

static int show_stat_exec(const char *cmd, char * const *fields, int fields_cnt, void *client)
{
	struct rad_server_t *s;

	list_for_each_entry(s, &serv_list, entry)
		show_stat(s, client);

	return CLI_CMD_OK;
}

static void __add_server(struct rad_server_t *s)
{
	struct rad_server_t *s1;

	list_for_each_entry(s1, &serv_list, entry) {
		if (s1->addr == s->addr && s1->auth_port == s->auth_port && s1->acct_port == s->acct_port) {
			s1->conf_fail_time = s->conf_fail_time;
			s1->req_limit = s->req_limit;
			s1->need_free = 0;
			_free(s);
			return;
		}
	}

	s->id = ++num;
	INIT_LIST_HEAD(&s->req_queue);
	pthread_mutex_init(&s->lock, NULL);
	list_add_tail(&s->entry, &serv_list);

	s->stat_auth_lost_1m = stat_accm_create(60);
	s->stat_auth_lost_5m = stat_accm_create(5 * 60);
	s->stat_auth_query_1m = stat_accm_create(60);
	s->stat_auth_query_5m = stat_accm_create(5 * 60);

	s->stat_acct_lost_1m = stat_accm_create(60);
	s->stat_acct_lost_5m = stat_accm_create(5 * 60);
	s->stat_acct_query_1m = stat_accm_create(60);
	s->stat_acct_query_5m = stat_accm_create(5 * 60);

	s->stat_interim_lost_1m = stat_accm_create(60);
	s->stat_interim_lost_5m = stat_accm_create(5 * 60);
	s->stat_interim_query_1m = stat_accm_create(60);
	s->stat_interim_query_5m = stat_accm_create(5 * 60);
}

static void __free_server(struct rad_server_t *s)
{
	log_debug("radius: free(%i)\n", s->id);

	stat_accm_free(s->stat_auth_lost_1m);
	stat_accm_free(s->stat_auth_lost_5m);
	stat_accm_free(s->stat_auth_query_1m);
	stat_accm_free(s->stat_auth_query_5m);

	stat_accm_free(s->stat_acct_lost_1m);
	stat_accm_free(s->stat_acct_lost_5m);
	stat_accm_free(s->stat_acct_query_1m);
	stat_accm_free(s->stat_acct_query_5m);

	stat_accm_free(s->stat_interim_lost_1m);
	stat_accm_free(s->stat_interim_lost_5m);
	stat_accm_free(s->stat_interim_query_1m);
	stat_accm_free(s->stat_interim_query_5m);

	_free(s);
}

static int parse_server_old(const char *opt, in_addr_t *addr, int *port, char **secret)
{
	char *str = _strdup(opt);
	char *p1, *p2;

	p1 = strstr(str, ":");
	p2 = strstr(str, ",");

	if (p1)
		*p1 = 0;
	if (p2)
		*p2 = 0;
	else
		return -1;
	
	*addr = inet_addr(str);
	
	if (p1) {
		*port = atoi(p1 + 1);
		if (*port <=0 )
			return -1;
	}

	*secret = _strdup(p2 + 1);

	_free(str);

	return 0;
}

static void add_server_old(void)
{
	const char *opt;
	in_addr_t auth_addr = 0;
	int auth_port;
	char *auth_secret;
	in_addr_t acct_addr = 0;
	int acct_port;
	char *acct_secret;
	struct rad_server_t *s;

	opt = conf_get_opt("radius", "auth-server");
	if (opt) {
		if (parse_server_old(opt, &auth_addr, &auth_port, &auth_secret)) {
			log_emerg("radius: failed to parse 'auth-server'\n");
			return;
		}
	} else
		return;

	opt = conf_get_opt("radius", "acct-server");
	if (opt) {
		if (parse_server_old(opt, &acct_addr, &acct_port, &acct_secret)) {
			log_emerg("radius: failed to parse 'acct-server'\n");
			return;
		}
		conf_accounting = 1;
	}

	s = _malloc(sizeof(*s));
	memset(s, 0, sizeof(*s));
	s->addr = auth_addr;
	s->secret = auth_secret;
	s->auth_port = auth_port;
	s->conf_fail_time = conf_fail_time;

	if (auth_addr == acct_addr && !strcmp(auth_secret, acct_secret)) {
		s->acct_port = acct_port;
		__add_server(s);
		return;
	}
	
	__add_server(s);

	if (acct_addr) {
		s = _malloc(sizeof(*s));
		memset(s, 0, sizeof(*s));
		s->addr = acct_addr;
		s->secret = acct_secret;
		s->acct_port = acct_port;
		s->conf_fail_time = conf_fail_time;
		__add_server(s);
	}
}

static int parse_server1(const char *_opt, struct rad_server_t *s)
{
	char *opt = _strdup(_opt);
	char *ptr1, *ptr2, *ptr3, *endptr;

	ptr1 = strchr(opt, ',');
	if (!ptr1)
		goto out;
	
	ptr2 = strchr(ptr1 + 1, ',');

	if (ptr2)
		ptr3 = strchr(ptr2 + 1, ',');
	else
		ptr3 = NULL;
	
	*ptr1 = 0;
	if (ptr2)
		*ptr2 = 0;
	if (ptr3)
		*ptr3 = 0;

	s->addr = inet_addr(opt);

	if (ptr2) {
		if (ptr2[1]) {
			s->auth_port = strtol(ptr2 + 1, &endptr, 10);
			if (*endptr)
				goto out;
		}
	} else
		s->auth_port = 1812;
	
	if (ptr3) {
		if (ptr3[1]) {
			s->acct_port = strtol(ptr3 + 1, &endptr, 10);
			if (*endptr)
				goto out;
		}
	} else
		s->acct_port = 1813;

	s->secret = _strdup(ptr1 + 1);
	s->conf_fail_time = conf_fail_time;
	
	return 0;

out:
	_free(opt);

	return -1;
}

static int parse_server2(const char *_opt, struct rad_server_t *s)
{
	char *opt = _strdup(_opt);
	char *ptr1, *ptr2, *ptr3, *endptr;

	ptr1 = strchr(opt, ',');
	if (!ptr1)
		goto out;
	
	ptr2 = strchr(ptr1 + 1, ',');

	*ptr1 = 0;

	s->addr = inet_addr(opt);

	ptr3 = strstr(ptr2, ",auth-port=");
	if (ptr3) {
		s->auth_port = strtol(ptr3 + 11, &endptr, 10);
		if (*endptr != ',' && *endptr != 0)
			goto out;
	} else
		s->auth_port = 1812;

	ptr3 = strstr(ptr2, ",acct-port=");
	if (ptr3) {
		s->acct_port = strtol(ptr3 + 11, &endptr, 10);
		if (*endptr != ',' && *endptr != 0)
			goto out;
	} else
		s->acct_port = 1813;

	ptr3 = strstr(ptr2, ",req-limit=");
	if (ptr3) {
		s->req_limit = strtol(ptr3 + 11, &endptr, 10);
		if (*endptr != ',' && *endptr != 0)
			goto out;
	} else
		s->req_limit = conf_req_limit;

	ptr3 = strstr(ptr2, ",fail-time=");
	if (ptr3) {
		s->conf_fail_time = strtol(ptr3 + 11, &endptr, 10);
		if (*endptr != ',' && *endptr != 0)
			goto out;
	} else
		s->conf_fail_time = conf_fail_time;

	if (ptr2)
		*ptr2 = 0;

	s->secret = _strdup(ptr1 + 1);
	
	_free(opt);

	return 0;

out:
	_free(opt);

	return -1;
}

static void add_server(const char *opt)
{
	struct rad_server_t *s = _malloc(sizeof(*s));
	
	memset(s, 0, sizeof(*s));

	if (!parse_server1(opt,s))
		goto add;
	
	if (!parse_server2(opt,s))
		goto add;

	log_emerg("radius: failed to parse '%s'\n", opt);
	_free(s);
	return;

add:
	__add_server(s);
}

static void load_config(void)
{
	struct conf_sect_t *sect = conf_get_section("radius");
	struct conf_option_t *opt;
	struct rad_server_t *s;
	struct rad_req_t *r;
	struct list_head *pos, *n;

	list_for_each_entry(s, &serv_list, entry)
		s->need_free = 1;

	list_for_each_entry(opt, &sect->items, entry) {
		if (strcmp(opt->name, "server"))
			continue;
		add_server(opt->val);
	}
	
	list_for_each_safe(pos, n, &serv_list) {
		s = list_entry(pos, typeof(*s), entry);
		if (s->need_free) {
			list_del(&s->entry);

			while (!list_empty(&s->req_queue)) {
				r = list_entry(s->req_queue.next, typeof(*r), entry);
				list_del(&r->entry);
				triton_context_wakeup(r->rpd->ppp->ctrl->ctx);
			}

			if (!s->client_cnt[0] && !s->client_cnt[1])
				__free_server(s);
		}
	}
	
	add_server_old();
	
	conf_accounting = 0;
	list_for_each_entry(s, &serv_list, entry) {
		if (s->acct_port) {
			conf_accounting = 1;
			break;
		}
	}
}

static void init(void)
{
	load_config();

	triton_event_register_handler(EV_CONFIG_RELOAD, (triton_event_func)load_config);

	cli_register_simple_cmd2(show_stat_exec, NULL, 2, "show", "stat");
}

DEFINE_INIT(52, init);
