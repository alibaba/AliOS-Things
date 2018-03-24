/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_SOCKETS_INTERNAL_H_
#define _SAL_SOCKETS_INTERNAL_H_

#include <sys/time.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "vfs_conf.h"
#include "sal_arch.h"
#include "sal_def.h"
#include "sal_ipaddr.h"
#include "sal_err.h"
#include "sal_pcb.h"
#include "sal_arch_internal.h"
#include "sal.h"
#include "sal_sockets.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MEMP_NUM_NETCONN     5//(MAX_SOCKETS_TCP + MAX_LISTENING_SOCKETS_TCP + MAX_SOCKETS_UDP)

#define SAL_TAG  "sal"

#ifdef SAL_USE_DEBUG
#define SAL_DEBUG(format, ...)  LOGD(SAL_TAG, format, ##__VA_ARGS__)
#else
#define SAL_DEBUG(format, ...)
#endif

#define SAL_ERROR(format, ...)  LOGE(SAL_TAG, format, ##__VA_ARGS__)
#define SAL_ASSERT(msg, assertion) do { if (!(assertion)) { \
        LOGE(SAL_TAG, msg);} \
    } while (0)

/** IPv4 only: set the IP address given as an u32_t */
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
/** IPv4 only: get the IP address as an u32_t */
#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)

/* Helpers to process several netconn_types by the same code */
#define NETCONNTYPE_GROUP(t)         ((t)&0xF0)
#define NETCONNTYPE_DATAGRAM(t)      ((t)&0xE0)

#if SAL_NETCONN_SEM_PER_THREAD
#define SELECT_SEM_T        sal_sem_t*
#define SELECT_SEM_PTR(sem) (sem)
#else /* SAL_NETCONN_SEM_PER_THREAD */
#define SELECT_SEM_T        sal_sem_t
#define SELECT_SEM_PTR(sem) (&(sem))
#endif /* SAL_NETCONN_SEM_PER_THREAD */

/* Flags for struct netconn.flags (u8_t) */
/** Should this netconn avoid blocking? */
#define NETCONN_FLAG_NON_BLOCKING             0x02
/** Was the last connect action a non-blocking one? */
#define NETCONN_FLAG_IN_NONBLOCKING_CONNECT   0x04

/** Set the blocking status of netconn calls (@todo: write/send is missing) */
#define netconn_set_nonblocking(conn, val)  do { if(val) { \
  (conn)->flags |= NETCONN_FLAG_NON_BLOCKING; \
} else { \
  (conn)->flags &= ~ NETCONN_FLAG_NON_BLOCKING; }} while(0)
/** Get the blocking status of netconn calls (@todo: write/send is missing) */
#define netconn_is_nonblocking(conn)        (((conn)->flags & NETCONN_FLAG_NON_BLOCKING) != 0)


// #if defined(AOS_CONFIG_VFS_DEV_NODES)
// #define SAL_SOCKET_OFFSET              AOS_CONFIG_VFS_DEV_NODES
// #endif

#define NETDB_ELEM_SIZE           (sizeof(struct addrinfo) + sizeof(struct sockaddr_storage) + DNS_MAX_NAME_LENGTH + 1)

typedef struct sal_netbuf{
    void      *payload;
    u16_t     len;
    ip_addr_t addr;
    u16_t     port;
}sal_netbuf_t;

typedef struct sal_outputbuf{
    void *payload;
    u16_t len;
    u16_t remote_port;
    char  remote_ip[16];
}sal_outputbuf_t;

/** Description for a task waiting in select */
struct sal_select_cb {
    /** Pointer to the next waiting task */
    struct sal_select_cb *next;
    /** Pointer to the previous waiting task */
    struct sal_select_cb *prev;
    /** readset passed to select */
    fd_set *readset;
    /** writeset passed to select */
    fd_set *writeset;
    /** unimplemented: exceptset passed to select */
    fd_set *exceptset;
    /** don't signal the same semaphore twice: set to 1 when signalled */
    int sem_signalled;
    /** semaphore to wake up a task waiting for select */
    SELECT_SEM_T sem;
};

/** Current state of the netconn. Non-TCP netconns are always
 * in state NETCONN_NONE! */
enum netconn_state {
    NETCONN_NONE,
    NETCONN_WRITE,
    NETCONN_LISTEN,
    NETCONN_CONNECT,
    NETCONN_CLOSE
};

/* Flags for struct netconn.flags (u8_t) */
/** Should this netconn avoid blocking? */
#define NETCONN_FLAG_NON_BLOCKING             0x02
/** Was the last connect action a non-blocking one? */
#define NETCONN_FLAG_IN_NONBLOCKING_CONNECT   0x04
/** If a nonblocking write has been rejected before, poll_tcp needs to
    check if the netconn is writable again */
#define NETCONN_FLAG_CHECK_WRITESPACE         0x10
/** If this flag is set then only IPv6 communication is allowed on the
    netconn. As per RFC#3493 this features defaults to OFF allowing
    dual-stack usage by default. */
#define NETCONN_FLAG_IPV6_V6ONLY              0x20

/** @ingroup netconn_common
 * Protocol family and type of the netconn
 */
enum netconn_type {
    NETCONN_INVALID     = 0,
    /** TCP IPv4 */
    NETCONN_TCP         = 0x10,
#if SAL_IPV6
    /** TCP IPv6 */
    NETCONN_TCP_IPV6    = NETCONN_TCP | NETCONN_TYPE_IPV6 /* 0x18 */,
#endif /* SAL_IPV6 */
    /** UDP IPv4 */
    NETCONN_UDP         = 0x20,
    /** UDP IPv4 lite */
    NETCONN_UDPLITE     = 0x21,
    /** UDP IPv4 no checksum */
    NETCONN_UDPNOCHKSUM = 0x22,

#if SAL_IPV6
    /** UDP IPv6 (dual-stack by default, unless you call @ref netconn_set_ipv6only) */
    NETCONN_UDP_IPV6         = NETCONN_UDP | NETCONN_TYPE_IPV6 /* 0x28 */,
    /** UDP IPv6 lite (dual-stack by default, unless you call @ref netconn_set_ipv6only) */
    NETCONN_UDPLITE_IPV6     = NETCONN_UDPLITE | NETCONN_TYPE_IPV6 /* 0x29 */,
    /** UDP IPv6 no checksum (dual-stack by default, unless you call @ref netconn_set_ipv6only) */
    NETCONN_UDPNOCHKSUM_IPV6 = NETCONN_UDPNOCHKSUM | NETCONN_TYPE_IPV6 /* 0x2a */,
#endif /* SAL_IPV6 */

    /** Raw connection IPv4 */
    NETCONN_RAW         = 0x40
#if SAL_IPV6
                          /** Raw connection IPv6 (dual-stack by default, unless you call @ref netconn_set_ipv6only) */
    , NETCONN_RAW_IPV6    = NETCONN_RAW | NETCONN_TYPE_IPV6 /* 0x48 */
#endif /* SAL_IPV6 */
};

struct sal_netconn;
/** A callback prototype to inform about events for a netconn */
typedef void (* netconn_callback)(struct sal_netconn *conn, enum netconn_evt, u16_t len);

/** A netconn descriptor */
typedef struct sal_netconn {
    int socket;
    /** type of the netconn (TCP, UDP or RAW) */
    enum netconn_type type;
    /** current state of the netconn */
    enum netconn_state state;

    /** the SAL internal protocol control block */
    union {
        struct ip_pcb  *ip;
        struct tcp_pcb *tcp;
        struct udp_pcb *udp;
        struct raw_pcb *raw;
    } pcb;
    /** the last error this netconn had */
    err_t last_err;

    /** mbox where received packets are stored until they are fetched
        by the neconn application thread. */
    sal_mbox_t recvmbox;

    sal_mbox_t sendmbox;
    
    /** flags holding more netconn-internal state, see NETCONN_FLAG_* defines */
    u8_t flags;
    /** timeout to wait for sending data (which means enqueueing data for sending
        in internal buffers) in milliseconds */
    s32_t send_timeout;
    /** timeout in milliseconds to wait for new data to be received
        (or connections to arrive for listening netconns) */
    int recv_timeout;
#if SAL_RCVBUF
    /** maximum amount of bytes queued in recvmbox
        not used for TCP: adjust TCP_WND instead! */
    int recv_bufsize;
    /** number of bytes currently in recvmbox to be received,
        tested against recv_bufsize to limit bytes on recvmbox
        for UDP and RAW, used for FIONREAD */
    int recv_avail;
#endif /* SAL_RCVBUF */
    /** A callback function that is informed about events for this netconn */
    netconn_callback callback;
} sal_netconn_t;

/** Set the blocking status of netconn calls (@todo: write/send is missing) */
#define netconn_set_nonblocking(conn, val)  do { if(val) { \
  (conn)->flags |= NETCONN_FLAG_NON_BLOCKING; \
} else { \
  (conn)->flags &= ~ NETCONN_FLAG_NON_BLOCKING; }} while(0)
/** Get the blocking status of netconn calls (@todo: write/send is missing) */
#define netconn_is_nonblocking(conn)        (((conn)->flags & NETCONN_FLAG_NON_BLOCKING) != 0)

#define SAL_SO_SNDRCVTIMEO_GET_MS(optval) ((((const struct timeval *)(optval))->tv_sec * 1000U) + (((const struct timeval *)(optval))->tv_usec / 1000U))

#define SAL_SOCKET_MAX_PAYLOAD_SIZE  1512
#define SAL_SOCKET_IP4_ANY_ADDR      "0.0.0.0"
#define SAL_SOCKET_IP4_ADDR_LEN      16

void sal_deal_event(int s, enum netconn_evt evt);

#define API_EVENT_SIMPLE(s,e) sal_deal_event(s,e)

#define EAI_NONAME      200
#define EAI_SERVICE     201
#define EAI_FAIL        202
#define EAI_MEMORY      203
#define EAI_FAMILY      204

#define HOST_NOT_FOUND  210
#define NO_DATA         211
#define NO_RECOVERY     212
#define TRY_AGAIN       213

/* input flags for struct addrinfo */
#define AI_PASSIVE      0x01
#define AI_CANONNAME    0x02
#define AI_NUMERICHOST  0x04
#define AI_NUMERICSERV  0x08
#define AI_V4MAPPED     0x10
#define AI_ALL          0x20
#define AI_ADDRCONFIG   0x40

struct sockaddr_storage {
    u8_t        s2_len;
    sa_family_t ss_family;
    char        s2_data1[2];
    u32_t       s2_data2[3];
#if LWIP_IPV6
    u32_t       s2_data3[3];
#endif /* LWIP_IPV6 */
};

#ifdef __cplusplus
}
#endif

#endif /* __AOS_EVENTFD_H__ */
