#ifndef PPP_H
#define PPP_H

#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <netinet/in.h>

#include "triton.h"
#include "list.h"
#include "iputils.h"

/*
 * Packet header = Code, id, length.
 */
#define PPP_HEADERLEN	4
#define PPP_MTU 1500


/*
 * Protocol field values.
 */
#define PPP_IP		0x21	/* Internet Protocol */
#define PPP_AT		0x29	/* AppleTalk Protocol */
#define PPP_IPX		0x2b	/* IPX protocol */
#define	PPP_VJC_COMP	0x2d	/* VJ compressed TCP */
#define	PPP_VJC_UNCOMP	0x2f	/* VJ uncompressed TCP */
#define PPP_IPV6	0x57	/* Internet Protocol Version 6 */
#define PPP_COMP	0xfd	/* compressed packet */
#define PPP_IPCP	0x8021	/* IP Control Protocol */
#define PPP_ATCP	0x8029	/* AppleTalk Control Protocol */
#define PPP_IPXCP	0x802b	/* IPX Control Protocol */
#define PPP_IPV6CP	0x8057	/* IPv6 Control Protocol */
#define PPP_CCP		0x80fd	/* Compression Control Protocol */
#define PPP_ECP		0x8053	/* Encryption Control Protocol */
#define PPP_LCP		0xc021	/* Link Control Protocol */
#define PPP_PAP		0xc023	/* Password Authentication Protocol */
#define PPP_LQR		0xc025	/* Link Quality Report protocol */
#define PPP_CHAP	0xc223	/* Cryptographic Handshake Auth. Protocol */
#define PPP_CBCP	0xc029	/* Callback Control Protocol */
#define PPP_EAP		0xc227	/* Extensible Authentication Protocol */

#define PPP_SESSIONID_LEN 16
#define PPP_IFNAME_LEN 10

#define PPP_STATE_STARTING  1
#define PPP_STATE_ACTIVE    2
#define PPP_STATE_FINISHING 3

#define TERM_USER_REQUEST 1
#define TERM_SESSION_TIMEOUT 2
#define TERM_ADMIN_RESET 3
#define TERM_USER_ERROR 4
#define TERM_NAS_ERROR 5
#define TERM_NAS_REQUEST 6
#define TERM_NAS_REBOOT 7
#define TERM_AUTH_ERROR 8
#define TERM_LOST_CARRIER 9

#define CTRL_TYPE_PPTP  1
#define CTRL_TYPE_L2TP  2
#define CTRL_TYPE_PPPOE 3

#define MPPE_UNSET   -2
#define MPPE_ALLOW   -1
#define MPPE_DENY    0
#define MPPE_PREFER  1
#define MPPE_REQUIRE 2

struct ppp_t;

struct ipv4db_item_t;
struct ipv6db_item_t;

struct ppp_ctrl_t
{
	struct triton_context_t *ctx;
	int type;
	const char *name;
	const char *def_pool;
	int max_mtu;
	int mppe;
	char *calling_station_id;
	char *called_station_id;
	void (*started)(struct ppp_t*);
	void (*finished)(struct ppp_t*);
};

struct ppp_pd_t
{
	struct list_head entry;
	void *key;
};

struct ppp_t
{
	struct list_head entry;
	struct triton_md_handler_t chan_hnd;
	struct triton_md_handler_t unit_hnd;
	int fd;
	int chan_fd;
	int unit_fd;

	int chan_idx;
	int unit_idx;

	int state;
	char *chan_name;
	char ifname[PPP_IFNAME_LEN];
	int ifindex;
	char sessionid[PPP_SESSIONID_LEN+1];
	time_t start_time;
	time_t stop_time;
	char *username;
	struct ipv4db_item_t *ipv4;
	struct ipv6db_item_t *ipv6;
	char *ipv4_pool_name;
	char *ipv6_pool_name;
	const char *comp;

	struct ppp_ctrl_t *ctrl;

	int terminating:1;
	int terminated:1;
	int terminate_cause;

	void *buf;
	int buf_size;

	struct list_head chan_handlers;
	struct list_head unit_handlers;

	struct list_head layers;
	
	struct ppp_lcp_t *lcp;

	struct list_head pd_list;
	
	uint32_t acct_rx_bytes;
	uint32_t acct_tx_bytes;
	uint32_t acct_input_gigawords;
	uint32_t acct_output_gigawords;
	uint32_t acct_rx_packets_i;
	uint32_t acct_tx_packets_i;
	uint32_t acct_rx_bytes_i;
	uint32_t acct_tx_bytes_i;
};

struct ppp_layer_t;
struct layer_node_t;
struct ppp_layer_data_t
{
	struct list_head entry;
	struct ppp_layer_t *layer;
	struct layer_node_t *node;
	int passive:1;
	int optional:1;
	int starting:1;
	int started:1;
	int finished:1;
};

struct ppp_layer_t
{
	struct list_head entry;
	struct ppp_layer_data_t *(*init)(struct ppp_t *);
	int (*start)(struct ppp_layer_data_t*);
	void (*finish)(struct ppp_layer_data_t*);
	void (*free)(struct ppp_layer_data_t *);
};

struct ppp_handler_t
{
	struct list_head entry;
	int proto;
	void (*recv)(struct ppp_handler_t*);
	void (*recv_proto_rej)(struct ppp_handler_t *h);
};

struct ppp_stat_t
{
	unsigned int active;
	unsigned int starting;
	unsigned int finishing;
};

struct ppp_t *alloc_ppp(void);
void ppp_init(struct ppp_t *ppp);
int establish_ppp(struct ppp_t *ppp);
int ppp_chan_send(struct ppp_t *ppp, void *data, int size);
int ppp_unit_send(struct ppp_t *ppp, void *data, int size);
void lcp_send_proto_rej(struct ppp_t *ppp, uint16_t proto);
void ppp_recv_proto_rej(struct ppp_t *ppp, uint16_t proto);

void ppp_ifup(struct ppp_t *ppp);
void ppp_ifdown(struct ppp_t *ppp);

struct ppp_fsm_t* ppp_lcp_init(struct ppp_t *ppp);
void ppp_layer_started(struct ppp_t *ppp,struct ppp_layer_data_t*);
void ppp_layer_finished(struct ppp_t *ppp,struct ppp_layer_data_t*);
void ppp_layer_passive(struct ppp_t *ppp,struct ppp_layer_data_t*);

void ppp_terminate(struct ppp_t *ppp, int hard, int cause);

void ppp_register_chan_handler(struct ppp_t *, struct ppp_handler_t *);
void ppp_register_unit_handler(struct ppp_t * ,struct ppp_handler_t *);
void ppp_unregister_handler(struct ppp_t *, struct ppp_handler_t *);

int ppp_register_layer(const char *name, struct ppp_layer_t *);
void ppp_unregister_layer(struct ppp_layer_t *);
struct ppp_layer_data_t *ppp_find_layer_data(struct ppp_t *, struct ppp_layer_t *);

int ppp_read_stats(struct ppp_t *ppp,  struct rtnl_link_stats *stats);

extern int ppp_shutdown;
void ppp_shutdown_soft(void);

int ppp_ipv6_nd_start(struct ppp_t *ppp, uint64_t intf_id);

extern int conf_ppp_verbose;
extern int conf_single_session;

extern pthread_rwlock_t ppp_lock;
extern struct list_head ppp_list;

extern struct ppp_stat_t ppp_stat;

extern int sock_fd; // internet socket for ioctls
extern int sock6_fd; // internet socket for ioctls
extern int urandom_fd;
#endif
