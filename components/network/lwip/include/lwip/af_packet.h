/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef LWIP_HDR_AF_PACKET_H
#define LWIP_HDR_AF_PACKET_H

#include "lwip/opt.h"

#if LWIP_PACKET
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"

/* If your port already typedef's socklen_t, define SOCKLEN_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(socklen_t) && !defined(SOCKLEN_T_DEFINED)
typedef u32_t socklen_t;
#endif

struct sockaddr_ll {
    u_char      sll_len;                                                /* Total length of sockaddr     */
    u_char      sll_family;                                             /* AF_PACKET                    */
    u_short     sll_protocol;                                           /* Physical layer protocol      */
    u_short     sll_hatype;                                             /* ARP hardware type            */
    u_char      sll_pkttype;                                            /* packet type                  */
    u_char      sll_halen;                                              /* Length of address            */
    u_char      sll_addr[8];                                            /* Physical layer address       */
    int         sll_ifindex;                                            /* Interface number             */
};

/** This is overridable for the rare case where more than 255 threads
 * select on the same socket...
 */
#ifndef SELWAIT_T
#define SELWAIT_T u8_t
#endif

typedef struct packet_sock {
    int  flag;           /* block or nonblock    */
    int  type;           /* raw or dgram         */
    int  protocol;       /* protocol             */
    int  ifindex;        /* netif index          */
    int  hdrlen;         /* header length        */
    sys_mbox_t pktmbox;  /* recv incoming pkts   */
    int  reserve;        /* reserve field        */
    int  is_used;        /* is used flag         */
    int  recv_output;    /* if recv output pkt   */
    /** number of times data was received */
    s16_t rcvevent;
    /** last error that occurred on this socket (in fact, all our errnos fit into an u8_t) */
    uint8_t err;
    /** counter of how many threads are waiting for this socket using select */
    SELWAIT_T select_waiting;
} packet_sock;

/* Packet socket data state indicator. */
typedef enum packet_action {
    PACKET_ACTION_EVTRCVPLUS,
    PACKET_ACTION_EVTRCVMINUS,
    PACKET_ACTION_EVTSENDPLUS,
    PACKET_ACTION_EVTSENDMINUS,
    PACKET_ACTION_SELWAITPLUS,
    PACKET_ACTION_SELWAITMINUS,
    PACKET_ACTION_ERROR
} packet_action_t;

#ifndef ETH_P_ALL
#define ETH_P_ALL 0x0003
#endif

#define ARPHRD_ETHER 1  /* ethernet hardware format*/

#define IS_AF_PACKET_SOCKET(s) ((s >= LWIP_PACKET_SOCKET_OFFSET)&&(s < LWIP_PACKET_SOCKET_OFFSET + NUM_PACKET_SOCKETS))

/* Packet socket options */

#define PACKET_ADD_MEMBERSHIP           1
#define PACKET_DROP_MEMBERSHIP          2
#define PACKET_RECV_OUTPUT              3
/* Value 4 is still used by obsolete turbo-packet. */
#define PACKET_RX_RING                  5
#define PACKET_STATISTICS               6
#define PACKET_COPY_THRESH              7
#define PACKET_AUXDATA                  8
#define PACKET_ORIGDEV                  9
#define PACKET_VERSION                  10
#define PACKET_HDRLEN                   11
#define PACKET_RESERVE                  12
#define PACKET_TX_RING                  13
#define PACKET_LOSS                     14
#define PACKET_VNET_HDR                 15
#define PACKET_TX_TIMESTAMP             16
#define PACKET_TIMESTAMP                17
#define PACKET_FANOUT                   18
#define PACKET_TX_HAS_OFF               19
#define PACKET_QDISC_BYPASS             20

typedef enum
{
    INCOMING = 1,
    OUTGOING
} pkt_direct;

int packet_socket(int domain, int type, int protocol);

int packet_bind(int s, const struct sockaddr *name, socklen_t namelen);

int packet_sendto(int s, const void *data, size_t size, int flags,
       const struct sockaddr *to, socklen_t tolen);

int packet_recvfrom(int s, void *mem, size_t len, int flags,
              struct sockaddr *from, socklen_t *fromlen);

int packet_recv(int s, void *mem, size_t len, int flags);

int packet_setsockopt (int s, int level, int optname,
                        const void *optval, socklen_t optlen);

int packet_close(int s);

err_t packet_input_hook (struct pbuf* p, struct netif * inp);

int packet_select_action(int s, packet_action_t action);

int
packet_selscan(int maxfdp1, fd_set *readset_in, fd_set *writeset_in, fd_set *exceptset_in,
             fd_set *readset_out, fd_set *writeset_out, fd_set *exceptset_out);

#endif /* LWIP_PACKET */
#endif /* LWIP_HDR_AF_PACKET_H */
