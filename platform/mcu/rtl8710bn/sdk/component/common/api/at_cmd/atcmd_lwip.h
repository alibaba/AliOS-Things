#ifndef __ATCMD_LWIP_H__
#define __ATCMD_LWIP_H__

#include "main.h"
#include "osdep_service.h"
#include <lwip/opt.h>
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#include "lwip/netdb.h"
#include "lwip_netconf.h"

#define	_AT_TRANSPORT_MODE_					"ATP1"
#define	_AT_TRANSPORT_LOCAL_PORT_			"ATP2"
#define	_AT_TRANSPORT_REMOTE_IP_			"ATP3"
#define	_AT_TRANSPORT_REMOTE_PORT_			"ATP4"
#define	_AT_TRANSPORT_START_SERVER_		"ATP5"
#define	_AT_TRANSPORT_START_CLIENT_			"ATP6"
#define	_AT_TRANSPORT_SHOW_SETTING_		"ATP?"
#define	_AT_TRANSPORT_RECEIVE_DATA_		"ATR0"
#define	_AT_TRANSPORT_RECEIVE_PACKET_SIZE_	"ATR1"
#define	_AT_TRANSPORT_WRITE_DATA_			"ATRA"
#define	_AT_TRANSPORT_WRITE_PACKET_SIZE_	"ATRB"

#define NODE_MODE_TCP		0
#define NODE_MODE_UDP		1
#define NODE_MODE_SSL		2

#define NODE_ROLE_SERVER	0
#define NODE_ROLE_CLIENT	1
#define NODE_ROLE_SEED		2

#define INVALID_SOCKET_ID	(-1)

//parameters
#ifndef NET_IF_NUM
#define NET_IF_NUM 	2
#endif

#define ATCMD_LWIP_TASK_PRIORITY        (tskIDLE_PRIORITY + 1)

#if ATCMD_VER == ATVER_2 

#ifndef ATCMD_SUPPORT_SSL
#define ATCMD_SUPPORT_SSL 0
#endif

#define SERVER "127.0.0.1"

#define NUM_NS (MEMP_NUM_NETCONN) //maximum number of node and seed, same as NUM_SOCKETS

#define ETH_MAX_MTU 1500

#define INVALID_CON_ID		(-1)

#define RECV_SELECT_TIMEOUT_SEC		(0)
#define RECV_SELECT_TIMEOUT_USEC		(20000) //20ms

typedef struct ns
{
	int con_id;
	int sockfd;
	s8_t role;
	int protocol;
	u32_t addr;
	u16_t  port;
	u32_t local_addr;
	u16_t local_port;
	xTaskHandle handletask;
	struct ns* next;
	struct ns* nextseed;
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	void *context;
#endif
} node;

extern xTaskHandle atcmd_lwip_tt_task;
extern _sema atcmd_lwip_tt_sema;
extern volatile int atcmd_lwip_tt_datasize;
extern volatile int atcmd_lwip_tt_lasttickcnt;
#define ATCMD_LWIP_TT_MAX_DELAY_TIME_MS	(20) //transparent transmission interval

extern int atcmd_lwip_is_tt_mode(void);
extern void atcmd_lwip_set_tt_mode(int enable);
int atcmd_lwip_send_data(node *curnode, u8 *data, u16 data_sz, struct sockaddr_in cli_addr);
int atcmd_lwip_receive_data(node *curnode, u8 *buffer, u16 buffer_size, int *recv_size, 
	u8_t *udp_clientaddr, u16_t *udp_clientport);
node* create_node(int mode, s8_t role);
void init_node_pool(void);
void delete_node(node *n);
int hang_node(node* insert_node);
int hang_seednode(node* main_node ,node* insert_node);
node *seek_node(int con_id);
node *tryget_node(int n);
#endif


#endif //#ifndef __ATCMD_LWIP_H__
