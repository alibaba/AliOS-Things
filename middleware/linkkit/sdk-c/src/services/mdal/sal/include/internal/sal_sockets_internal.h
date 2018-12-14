/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_SOCKETS_INTERNAL_H_
#define _SAL_SOCKETS_INTERNAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SAL_USE_AOS_HAL
    #include "aos/kernel.h"
#else
    #include "iotx_log.h"
#endif

#include "sal_opts.h"
#include "sal_arch.h"
#include "sal_def.h"
#include "sal_ipaddr.h"
#include "sal_err.h"
#include "sal_pcb.h"
#include "sal_arch_internal.h"
#include "sal_import.h"
#include "sal_sockets.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MEMP_NUM_NETCONN     5 /* (MAX_SOCKETS_TCP + MAX_LISTENING_SOCKETS_TCP + MAX_SOCKETS_UDP) */

#define SAL_TAG  "sal"

#ifdef SAL_USE_AOS_HAL
#if SAL_USE_DEBUG
#define SAL_DEBUG(format, ...)  LOGD(SAL_TAG, format, ##__VA_ARGS__)
#define SAL_ERROR(format, ...)  LOGE(SAL_TAG, format, ##__VA_ARGS__)
#define SAL_ASSERT(msg, assertion) do { if (!(assertion)) { \
            LOGE(SAL_TAG, msg);} \
        } while (0)
#else
#define SAL_DEBUG(format, ...)
#define SAL_ERROR(format, ...)
#define SAL_ASSERT(msg, assertion)
#endif

#define SAL_SNPRINTF snprintf
#else
#if SAL_USE_DEBUG
#define SAL_DEBUG(format, ...)  log_debug(SAL_TAG, format, ##__VA_ARGS__)
#define SAL_ERROR(format, ...)  log_err(SAL_TAG, format, ##__VA_ARGS__)
#define SAL_ASSERT(msg, assertion) do { if (!(assertion)) { \
            log_err(SAL_TAG, msg);} \
        } while (0)
#else
#define SAL_DEBUG(format, ...)
#define SAL_ERROR(format, ...)
#define SAL_ASSERT(msg, assertion)
#endif

#define SAL_SNPRINTF HAL_Snprintf
#endif

#define SAL_SOCKET_UNSUPPORT -1

/* Helpers to process several netconn_types by the same code */
#define NETCONNTYPE_GROUP(t)         ((t)&0xF0)

#define SELECT_SEM_T        sal_sem_t
#define SELECT_SEM_PTR(sem) (&(sem))

#define NUM_SOCKETS MEMP_NUM_NETCONN
#define NUM_EVENTS  MEMP_NUM_NETCONN

#define SAL_DRAIN_SENDMBOX_WAIT_TIME   50

#define SAL_EVENT_OFFSET (NUM_SOCKETS + SAL_SOCKET_OFFSET)

#ifndef SELWAIT_T
#define SELWAIT_T uint8_t
#endif

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

#define NETDB_ELEM_SIZE           (sizeof(struct addrinfo) + sizeof(struct sockaddr_storage) + DNS_MAX_NAME_LENGTH + 1)

typedef struct sal_netbuf {
    void      *payload;
    u16_t     len;
    ip_addr_t addr;
    u16_t     port;
} sal_netbuf_t;

typedef struct sal_outputbuf {
    void *payload;
    u16_t len;
    u16_t remote_port;
    char  remote_ip[16];
} sal_outputbuf_t;

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

/** @ingroup netconn_common
 * Protocol family and type of the netconn
 */
enum netconn_type {
    NETCONN_INVALID     = 0,
    /** TCP IPv4 */
    NETCONN_TCP         = 0x10,
    /** UDP IPv4 */
    NETCONN_UDP         = 0x20,
    /** UDP IPv4 lite */
    NETCONN_UDPLITE     = 0x21,
    /** UDP IPv4 no checksum */
    NETCONN_UDPNOCHKSUM = 0x22,
    /** Raw connection IPv4 */
    NETCONN_RAW         = 0x40
};

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
#if SAL_UDP_CLIENT_ENABLED
        struct udp_pcb *udp;
#endif
    } pcb;
    /** the last error this netconn had */
    err_t last_err;

    /** mbox where received packets are stored until they are fetched
        by the neconn application thread. */
    sal_mbox_t recvmbox;

#if SAL_PACKET_SEND_MODE_ASYNC
    sal_mbox_t sendmbox;
#endif

    /** flags holding more netconn-internal state, see NETCONN_FLAG_* defines */
    u8_t flags;
    /** timeout to wait for sending data (which means enqueueing data for sending
        in internal buffers) in milliseconds */
    s32_t send_timeout;
    /** timeout in milliseconds to wait for new data to be received
        (or connections to arrive for listening netconns) */
    int recv_timeout;
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
};

/** Safely copy one IPv6 address to another (src may be NULL) */
#define ip6_addr_set(dest, src) do{(dest)->addr[0] = (src) == NULL ? 0 : (src)->addr[0]; \
        (dest)->addr[1] = (src) == NULL ? 0 : (src)->addr[1]; \
        (dest)->addr[2] = (src) == NULL ? 0 : (src)->addr[2]; \
        (dest)->addr[3] = (src) == NULL ? 0 : (src)->addr[3];}while(0)

/** Safely copy one IP address to another (src may be NULL) */
#define ip4_addr_set(dest, src) ((dest)->addr = \
                                 ((src) == NULL ? 0 : \
                                  (src)->addr))

/** @ingroup ip6addr
 * Convert generic ip address to specific protocol version
 */
#define ip_2_ip6(ipaddr)   (&((ipaddr)->u_addr.ip6))
/** @ingroup ip4addr
 * Convert generic ip address to specific protocol version
 */
#define ip_2_ip4(ipaddr)   (&((ipaddr)->u_addr.ip4))


/** IPv4 only: set the IP address given as an u32_t */
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
/** IPv4 only: get the IP address as an u32_t */
#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)


#define IP_SET_TYPE_VAL(ipaddr, iptype) do { (ipaddr).type = (iptype); }while(0)
#define IP_SET_TYPE(ipaddr, iptype)     do { if((ipaddr) != NULL) { IP_SET_TYPE_VAL(*(ipaddr), iptype); }}while(0)
#define IP_GET_TYPE(ipaddr)           ((ipaddr)->type)
/** @ingroup ip4addr */
#define IP_IS_V4_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_V4)
/** @ingroup ip6addr */
#define IP_IS_V6_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_V6)
/** @ingroup ip4addr */
#define IP_IS_V4(ipaddr)              (((ipaddr) == NULL) || IP_IS_V4_VAL(*(ipaddr)))
/** @ingroup ip6addr */
#define IP_IS_V6(ipaddr)              (IP_IS_V6_VAL(*(ipaddr)))

#define ip_addr_set(dest, src) do{ IP_SET_TYPE(dest, IP_GET_TYPE(src)); if(IP_IS_V6(src)){ \
            ip6_addr_set(ip_2_ip6(dest), ip_2_ip6(src)); }else{ \
            ip4_addr_set(ip_2_ip4(dest), ip_2_ip4(src)); }}while(0)
/** @ingroup ipaddr */
#define ip_addr_set_ipaddr(dest, src) ip_addr_set(dest, src)

#define inet_addr_from_ipaddr(target_inaddr, source_ipaddr) ((target_inaddr)->s_addr = ip4_addr_get_u32(source_ipaddr))
#define inet_addr_to_ipaddr(target_ipaddr, source_inaddr)   (ip4_addr_set_u32(target_ipaddr, (source_inaddr)->s_addr))

#define IP4ADDR_PORT_TO_SOCKADDR(sin, ipaddr, port) do { \
        (sin)->sin_len = sizeof(struct sockaddr_in); \
        (sin)->sin_family = AF_INET; \
        (sin)->sin_port = sal_htons((port)); \
        inet_addr_from_ipaddr(&(sin)->sin_addr, ipaddr); \
        memset((sin)->sin_zero, 0, SIN_ZERO_LEN); }while(0)

#define SOCKADDR4_TO_IP4ADDR_PORT(sin, ipaddr, port) do { \
        inet_addr_to_ipaddr(ip_2_ip4(ipaddr), &((sin)->sin_addr)); \
        (port) = sal_htons((sin)->sin_port); }while(0)

#define IPADDR_PORT_TO_SOCKADDR(sockaddr, ipaddr, port) \
    IP4ADDR_PORT_TO_SOCKADDR((struct sockaddr_in*)(void*)(sockaddr), ip_2_ip4(ipaddr), port)

#define ipaddr_aton(cp,addr) ip4addr_aton(cp,addr)

void ip4_sockaddr_to_ipstr_port(const struct sockaddr *name, char *ip);
int ipstr_to_u32(char *ipstr, uint32_t *ip32);

#ifdef __cplusplus
}
#endif

#endif /* __AOS_EVENTFD_H__ */
