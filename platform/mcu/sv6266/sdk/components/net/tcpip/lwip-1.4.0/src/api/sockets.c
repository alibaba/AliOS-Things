/**
 * @file
 * Sockets BSD-Like API module
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * Improved by Marc Boucher <marc@mbsi.ca> and David Haas <dhaas@alum.rpi.edu>
 *
 */

#include "lwip/opt.h"

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/lwip_pbuf.h"
#if LWIP_CHECKSUM_ON_COPY
#include "lwip/inet_chksum.h"
#endif

#include <string.h>

#define IP4ADDR_PORT_TO_SOCKADDR(sin, ipXaddr, port) do { \
      (sin)->sin_len = sizeof(struct sockaddr_in); \
      (sin)->sin_family = AF_INET; \
      (sin)->sin_port = htons((port)); \
      inet_addr_from_ipaddr(&(sin)->sin_addr, ipX_2_ip(ipXaddr)); \
      memset((sin)->sin_zero, 0, SIN_ZERO_LEN); }while(0)
#define SOCKADDR4_TO_IP4ADDR_PORT(sin, ipXaddr, port) do { \
    inet_addr_to_ipaddr(ipX_2_ip(ipXaddr), &((sin)->sin_addr)); \
    (port) = ntohs((sin)->sin_port); }while(0)

#if LWIP_IPV6
#define IS_SOCK_ADDR_LEN_VALID(namelen)  (((namelen) == sizeof(struct sockaddr_in)) || \
                                         ((namelen) == sizeof(struct sockaddr_in6)))
#define IS_SOCK_ADDR_TYPE_VALID(name)    (((name)->sa_family == AF_INET) || \
                                         ((name)->sa_family == AF_INET6))
#define SOCK_ADDR_TYPE_MATCH(name, sock) \
       ((((name)->sa_family == AF_INET) && !(NETCONNTYPE_ISIPV6((sock)->conn->type))) || \
       (((name)->sa_family == AF_INET6) && (NETCONNTYPE_ISIPV6((sock)->conn->type))))
#define IP6ADDR_PORT_TO_SOCKADDR(sin6, ipXaddr, port) do { \
      (sin6)->sin6_len = sizeof(struct sockaddr_in6); \
      (sin6)->sin6_family = AF_INET6; \
      (sin6)->sin6_port = htons((port)); \
      (sin6)->sin6_flowinfo = 0; \
      inet6_addr_from_ip6addr(&(sin6)->sin6_addr, ipX_2_ip6(ipXaddr)); \
(sin6)->sin6_scope_id = 0;}while(0)
#define IPXADDR_PORT_TO_SOCKADDR(isipv6, sockaddr, ipXaddr, port) do { \
    if (isipv6) { \
      IP6ADDR_PORT_TO_SOCKADDR((struct sockaddr_in6*)(void*)(sockaddr), ipXaddr, port); \
    } else { \
      IP4ADDR_PORT_TO_SOCKADDR((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port); \
    } } while(0)
#define SOCKADDR6_TO_IP6ADDR_PORT(sin6, ipXaddr, port) do { \
    inet6_addr_to_ip6addr(ipX_2_ip6(ipXaddr), &((sin6)->sin6_addr)); \
    (port) = ntohs((sin6)->sin6_port); }while(0)
#define SOCKADDR_TO_IPXADDR_PORT(isipv6, sockaddr, ipXaddr, port) do { \
    if (isipv6) { \
      SOCKADDR6_TO_IP6ADDR_PORT((struct sockaddr_in6*)(void*)(sockaddr), ipXaddr, port); \
    } else { \
      SOCKADDR4_TO_IP4ADDR_PORT((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port); \
    } } while(0)
#define DOMAIN_TO_NETCONN_TYPE(domain, type) (((domain) == AF_INET) ? \
  (type) : (enum netconn_type)((type) | NETCONN_TYPE_IPV6))
#else /* LWIP_IPV6 */
#define IS_SOCK_ADDR_LEN_VALID(namelen)  ((namelen) == sizeof(struct sockaddr_in))
#define IS_SOCK_ADDR_TYPE_VALID(name)    ((name)->sa_family == AF_INET)
#define SOCK_ADDR_TYPE_MATCH(name, sock) 1
#define IPXADDR_PORT_TO_SOCKADDR(isipv6, sockaddr, ipXaddr, port) \
        IP4ADDR_PORT_TO_SOCKADDR((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port)
#define SOCKADDR_TO_IPXADDR_PORT(isipv6, sockaddr, ipXaddr, port) \
      SOCKADDR4_TO_IP4ADDR_PORT((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port)
#define DOMAIN_TO_NETCONN_TYPE(domain, netconn_type) (netconn_type)
#endif /* LWIP_IPV6 */

#define IS_SOCK_ADDR_TYPE_VALID_OR_UNSPEC(name)    (((name)->sa_family == AF_UNSPEC) || \
                                                    IS_SOCK_ADDR_TYPE_VALID(name))
#define SOCK_ADDR_TYPE_MATCH_OR_UNSPEC(name, sock) (((name)->sa_family == AF_UNSPEC) || \
                                                    SOCK_ADDR_TYPE_MATCH(name, sock))
#define IS_SOCK_ADDR_ALIGNED(name)      ((((mem_ptr_t)(name)) % 4) == 0)

#define LWIP_SOCKOPT_CHECK_OPTLEN(optlen, opttype) do { if((optlen) < sizeof(opttype)) { return EINVAL; }}while(0)
#define LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, optlen, opttype) do { \
  LWIP_SOCKOPT_CHECK_OPTLEN(optlen, opttype); \
  if ((sock)->conn == NULL) { return EINVAL; } }while(0)
#define LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, opttype) do { \
  LWIP_SOCKOPT_CHECK_OPTLEN(optlen, opttype); \
  if (((sock)->conn == NULL) || ((sock)->conn->pcb.tcp == NULL)) { return EINVAL; } }while(0)
#define LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, opttype, netconntype) do { \
  LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, opttype); \
  if (NETCONNTYPE_GROUP(netconn_type((sock)->conn)) != netconntype) { return ENOPROTOOPT; } }while(0)


#define LWIP_SETGETSOCKOPT_DATA_VAR_REF(name)     API_VAR_REF(name)
#define LWIP_SETGETSOCKOPT_DATA_VAR_DECLARE(name) API_VAR_DECLARE(struct lwip_setgetsockopt_data, name)
#define LWIP_SETGETSOCKOPT_DATA_VAR_FREE(name)    API_VAR_FREE(MEMP_SOCKET_SETGETSOCKOPT_DATA, name)
#if LWIP_MPU_COMPATIBLE
#define LWIP_SETGETSOCKOPT_DATA_VAR_ALLOC(name, sock) do { \
  name = (struct lwip_setgetsockopt_data *)memp_malloc(MEMP_SOCKET_SETGETSOCKOPT_DATA); \
  if (name == NULL) { \
    sock_set_errno(sock, ENOMEM); \
    return -1; \
  } }while(0)
#else /* LWIP_MPU_COMPATIBLE */
#define LWIP_SETGETSOCKOPT_DATA_VAR_ALLOC(name, sock)
#endif /* LWIP_MPU_COMPATIBLE */

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
#define LWIP_SO_SNDRCVTIMEO_OPTTYPE int
#define LWIP_SO_SNDRCVTIMEO_SET(optval, val) (*(int *)(optval) = (val))
#define LWIP_SO_SNDRCVTIMEO_GET_MS(optval)   ((s32_t)*(int*)(optval))
#else
#define LWIP_SO_SNDRCVTIMEO_OPTTYPE struct timeval
#define LWIP_SO_SNDRCVTIMEO_SET(optval, val)  do { \
  s32_t loc = (val); \
  ((struct timeval *)(optval))->tv_sec = (loc) / 1000U; \
  ((struct timeval *)(optval))->tv_usec = ((loc) % 1000U) * 1000U; }while(0)
#define LWIP_SO_SNDRCVTIMEO_GET_MS(optval) ((((struct timeval *)(optval))->tv_sec * 1000U) + (((struct timeval *)(optval))->tv_usec / 1000U))
#endif

#define NUM_SOCKETS MEMP_NUM_NETCONN

/** Contains all internal pointers and states used for a socket */
struct lwip_sock {
  /** sockets currently are built on netconns, each socket has one netconn */
  struct netconn *conn;
  /** data that was left from the previous read */
  void *lastdata;
  /** offset in the data that was left from the previous read */
  u16_t lastoffset;
  /** number of times data was received, set by event_callback(),
      tested by the receive and select functions */
  s16_t rcvevent;
  /** number of times data was ACKed (free send buffer), set by event_callback(),
      tested by select */
  u16_t sendevent;
  /** error happened for this socket, set by event_callback(), tested by select */
  u16_t errevent; 
  /** last error that occurred on this socket */
  int err;
  /** counter of how many threads are waiting for this socket using select */
  int select_waiting;
};

/** Description for a task waiting in select */
struct lwip_select_cb {
  /** Pointer to the next waiting task */
  struct lwip_select_cb *next;
  /** Pointer to the previous waiting task */
  struct lwip_select_cb *prev;
  /** readset passed to select */
  fd_set *readset;
  /** writeset passed to select */
  fd_set *writeset;
  /** unimplemented: exceptset passed to select */
  fd_set *exceptset;
  /** don't signal the same semaphore twice: set to 1 when signalled */
  int sem_signalled;
  /** semaphore to wake up a task waiting for select */
  sys_sem_t sem;
};

/** This struct is used to pass data to the set/getsockopt_internal
 * functions running in tcpip_thread context (only a void* is allowed) */
struct lwip_setgetsockopt_data {
  /** socket struct for which to change options */
  struct lwip_sock *sock;
#ifdef LWIP_DEBUG
  /** socket index for which to change options */
  int s;
#endif /* LWIP_DEBUG */
  /** level of the option to process */
  int level;
  /** name of the option to process */
  int optname;
  /** set: value to set the option to
    * get: value of the option is stored here */
  void *optval;
  /** size of *optval */
  socklen_t *optlen;
  /** if an error occures, it is temporarily stored here */
  err_t err;
};

/** A struct sockaddr replacement that has the same alignment as sockaddr_in/
 *  sockaddr_in6 if instantiated.
 */
union sockaddr_aligned {
   struct sockaddr sa;
#if LWIP_IPV6
   struct sockaddr_in6 sin6;
#endif /* LWIP_IPV6 */
   struct sockaddr_in sin;
};

#if LWIP_IGMP || LWIP_IPV6_MLD
#ifndef LWIP_SOCKET_MAX_MEMBERSHIPS
#define LWIP_SOCKET_MAX_MEMBERSHIPS NUM_SOCKETS
#endif
#define SOCK_TYPE_V4   (0)
#define SOCK_TYPE_V6   (1)

struct lwip_socket_multicast_pair {
  /** the socket (+1 to not require initialization) */
  int type;
  
  int sa;
  /** the interface address */
  ip4_addr_t if_addr;
  /** the group address */
  ip4_addr_t multi_addr;
#if LWIP_IPV6_MLD
  ip6_addr_t if6_addr;

  ip6_addr_t multi6_addr;
#endif
};

struct lwip_socket_multicast_pair socket_multicast_memberships[LWIP_SOCKET_MAX_MEMBERSHIPS];

static int  lwip_socket_register_membership(int s, int type, void *if_addr, void *multi_addr);
static void lwip_socket_unregister_membership(int s, int type, void *if_addr, void *multi_addr);
static void lwip_socket_drop_registered_memberships(int s);
#endif

/** The global array of available sockets */
static struct lwip_sock sockets[NUM_SOCKETS];
/** The global list of tasks waiting for select */
static struct lwip_select_cb *select_cb_list;
/** This counter is increased from lwip_select when the list is chagned
    and checked in event_callback to see if it has changed. */
static volatile int select_cb_ctr;

/** Table to quickly map an lwIP error (err_t) to a socket error
  * by using -err as an index */
static const int err_to_errno_table[] = {
  0,             /* ERR_OK          0      No error, everything OK. */
  ENOMEM,        /* ERR_MEM        -1      Out of memory error.     */
  ENOBUFS,       /* ERR_BUF        -2      Buffer error.            */
  EWOULDBLOCK,   /* ERR_TIMEOUT    -3      Timeout                  */
  EHOSTUNREACH,  /* ERR_RTE        -4      Routing problem.         */
  EINPROGRESS,   /* ERR_INPROGRESS -5      Operation in progress    */
  EINVAL,        /* ERR_VAL        -6      Illegal value.           */
  EWOULDBLOCK,   /* ERR_WOULDBLOCK -7      Operation would block.   */
  EADDRINUSE,    /* ERR_USE        -8      Address in use.          */
  EALREADY,      /* ERR_ISCONN     -9      Already connected.       */
  ECONNABORTED,  /* ERR_ABRT       -10     Connection aborted.      */
  ECONNRESET,    /* ERR_RST        -11     Connection reset.        */
  ENOTCONN,      /* ERR_CLSD       -12     Connection closed.       */
  ENOTCONN,      /* ERR_CONN       -13     Not connected.           */
  EIO,           /* ERR_ARG        -14     Illegal argument.        */
  -1,            /* ERR_IF         -15     Low-level netif error    */
};

#define ERR_TO_ERRNO_TABLE_SIZE \
  (sizeof(err_to_errno_table)/sizeof(err_to_errno_table[0]))

#define err_to_errno(err) \
  ((unsigned)(-(err)) < ERR_TO_ERRNO_TABLE_SIZE ? \
    err_to_errno_table[-(err)] : EIO)


#ifdef ERRNO
int errno = 0;

#ifndef set_errno
#define set_errno(err) errno = (err)
#endif
#else /* ERRNO */
#define set_errno(err)
#endif /* ERRNO */

#define sock_set_errno(sk, e) do { \
  sk->err = (e); \
  set_errno(sk->err); \
} while (0)

/* Forward delcaration of some functions */
static void event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len);
static void lwip_getsockopt_internal(void *arg);
static void lwip_setsockopt_internal(void *arg);

/**
 * Initialize this module. This function has to be called before any other
 * functions in this module!
 */
void
lwip_socket_init(void)
{
}

/**
 * Map a externally used socket index to the internal socket representation.
 *
 * @param s externally used socket index
 * @return struct lwip_sock for the socket or NULL if not found
 */
static struct lwip_sock *
get_socket(int s)
{
  struct lwip_sock *sock;

  if ((s < 0) || (s >= NUM_SOCKETS)) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("get_socket(%d): invalid\r\n", s));
    set_errno(EBADF);
    return NULL;
  }

  sock = &sockets[s];

  if (!sock->conn) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("get_socket(%d): not active\r\n", s));
    set_errno(EBADF);
    return NULL;
  }

  return sock;
}

/**
 * Same as get_socket but doesn't set errno
 *
 * @param s externally used socket index
 * @return struct lwip_sock for the socket or NULL if not found
 */
static struct lwip_sock *
tryget_socket(int s)
{
  if ((s < 0) || (s >= NUM_SOCKETS)) {
    return NULL;
  }
  if (!sockets[s].conn) {
    return NULL;
  }
  return &sockets[s];
}

/**
 * Allocate a new socket for a given netconn.
 *
 * @param newconn the netconn for which to allocate a socket
 * @param accepted 1 if socket has been created by accept(),
 *                 0 if socket has been created by socket()
 * @return the index of the new socket; -1 on error
 */
static int
alloc_socket(struct netconn *newconn, int accepted)
{
  int i;
  SYS_ARCH_DECL_PROTECT(lev);

  /* allocate a new socket identifier */
  for (i = 0; i < NUM_SOCKETS; ++i) {
    /* Protect socket array */
    SYS_ARCH_PROTECT(lev);
    if (!sockets[i].conn) {
      sockets[i].conn       = newconn;
      /* The socket is not yet known to anyone, so no need to protect
         after having marked it as used. */
      SYS_ARCH_UNPROTECT(lev);
      sockets[i].lastdata   = NULL;
      sockets[i].lastoffset = 0;
      sockets[i].rcvevent   = 0;
      /* TCP sendbuf is empty, but the socket is not yet writable until connected
       * (unless it has been created by accept()). */
      sockets[i].sendevent  = (NETCONNTYPE_GROUP(newconn->type) == NETCONN_TCP ? (accepted != 0) : 1);
      sockets[i].errevent   = 0;
      sockets[i].err        = 0;
      sockets[i].select_waiting = 0;
      return i;
    }
    SYS_ARCH_UNPROTECT(lev);
  }
  return -1;
}

/** Free a socket. The socket's netconn must have been
 * delete before!
 *
 * @param sock the socket to free
 * @param is_tcp != 0 for TCP sockets, used to free lastdata
 */
static void
free_socket(struct lwip_sock *sock, int is_tcp)
{
  void *lastdata;
  SYS_ARCH_DECL_PROTECT(lev);

  lastdata         = sock->lastdata;
  sock->lastdata   = NULL;
  sock->lastoffset = 0;
  sock->err        = 0;

  /* Protect socket array */
  SYS_ARCH_PROTECT(lev);
  sock->conn       = NULL;
  SYS_ARCH_UNPROTECT(lev);
  /* don't use 'sock' after this line, as another task might have allocated it */

  if (lastdata != NULL) {
    if (is_tcp) {
      pbuf_free((struct pbuf *)lastdata);
    } else {
      netbuf_delete((struct netbuf *)lastdata);
    }
  }
}

/* Below this, the well-known socket functions are implemented.
 * Use google.com or opengroup.org to get a good description :-)
 *
 * Exceptions are documented!
 */

int
lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
  struct lwip_sock *sock, *nsock;
  struct netconn *newconn;
  ipX_addr_t naddr;
  u16_t port = 0;
  int newsock;
  err_t err;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d)...\r\n", s));
  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (netconn_is_nonblocking(sock->conn) && (sock->rcvevent <= 0)) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d): returning EWOULDBLOCK\r\n", s));
    sock_set_errno(sock, EWOULDBLOCK);
    return -1;
  }

  /* wait for a new connection */
  err = netconn_accept(sock->conn, &newconn);
  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d): netconn_acept failed, err=%d\r\n", s, err));
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      sock_set_errno(sock, EOPNOTSUPP);
      return EOPNOTSUPP;
    }
    sock_set_errno(sock, err_to_errno(err));
    return -1;
  }
  LWIP_ASSERT("newconn != NULL", newconn != NULL);
  /* Prevent automatic window updates, we do this on our own! */
  netconn_set_noautorecved(newconn, 1);

  /* Note that POSIX only requires us to check addr is non-NULL. addrlen must
   * not be NULL if addr is valid.
   */
  if (addr != NULL) {
    union sockaddr_aligned tempaddr;
    /* get the IP address and port of the remote host */
    err = netconn_peer(newconn, ipX_2_ip(&naddr), &port);
    if (err != ERR_OK) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d): netconn_peer failed, err=%d\r\n", s, err));
      netconn_delete(newconn);
      sock_set_errno(sock, err_to_errno(err));
      return -1;
    }
    LWIP_ASSERT("addr valid but addrlen NULL", addrlen != NULL);

    IPXADDR_PORT_TO_SOCKADDR(NETCONNTYPE_ISIPV6(newconn->type), &tempaddr, &naddr, port);
    if (*addrlen > tempaddr.sa.sa_len) {
      *addrlen = tempaddr.sa.sa_len;
    }
    MEMCPY(addr, &tempaddr, *addrlen);
  }

  newsock = alloc_socket(newconn, 1);
  if (newsock == -1) {
    netconn_delete(newconn);
    sock_set_errno(sock, ENFILE);
    return -1;
  }
  LWIP_ASSERT("invalid socket index", (newsock >= 0) && (newsock < NUM_SOCKETS));
  LWIP_ASSERT("newconn->callback == event_callback", newconn->callback == event_callback);
  nsock = &sockets[newsock];

  /* See event_callback: If data comes in right away after an accept, even
   * though the server task might not have created a new socket yet.
   * In that case, newconn->socket is counted down (newconn->socket--),
   * so nsock->rcvevent is >= 1 here!
   */
  SYS_ARCH_PROTECT(lev);
  nsock->rcvevent += (s16_t)(-1 - newconn->socket);
  newconn->socket = newsock;
  SYS_ARCH_UNPROTECT(lev);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d) returning new sock=%d", s, newsock));
  if (addr != NULL) {
    LWIP_DEBUGF(SOCKETS_DEBUG, (" addr="));
    ipX_addr_debug_print(NETCONNTYPE_ISIPV6(newconn->type), SOCKETS_DEBUG, &naddr);
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F"\r\n", port));
  }

  sock_set_errno(sock, 0);
  return newsock;
}

int
lwip_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
  struct lwip_sock *sock;
  ipX_addr_t local_addr;
  u16_t local_port;
  err_t err;

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (!SOCK_ADDR_TYPE_MATCH(name, sock)) {
    /* sockaddr does not match socket type (IPv4/IPv6) */
    sock_set_errno(sock, err_to_errno(ERR_VAL));
    return -1;
  }

  /* check size, familiy and alignment of 'name' */
  LWIP_ERROR("lwip_bind: invalid address", (IS_SOCK_ADDR_LEN_VALID(namelen) &&
             IS_SOCK_ADDR_TYPE_VALID(name) && IS_SOCK_ADDR_ALIGNED(name)),
             sock_set_errno(sock, err_to_errno(ERR_ARG)); return -1;);
  LWIP_UNUSED_ARG(namelen);

  SOCKADDR_TO_IPXADDR_PORT((name->sa_family == AF_INET6), name, &local_addr, local_port);
  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d, addr=", s));
  ipX_addr_debug_print(name->sa_family == AF_INET6, SOCKETS_DEBUG, &local_addr);
  LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F")\r\n", local_port));

  err = netconn_bind(sock->conn, ipX_2_ip(&local_addr), local_port);

  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d) failed, err=%d\r\n", s, err));
    sock_set_errno(sock, err_to_errno(err));
    return -1;
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d) succeeded\r\n", s));
  sock_set_errno(sock, 0);
  return 0;
}

int
lwip_close(int s)
{
  struct lwip_sock *sock;
  int is_tcp = 0;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_close(%d)\r\n", s));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if(sock->conn != NULL) {
    is_tcp = NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP;
  } else {
    LWIP_ASSERT("sock->lastdata == NULL", sock->lastdata == NULL);
  }

  netconn_delete(sock->conn);

  free_socket(sock, is_tcp);
  set_errno(0);
  return 0;
}

int
lwip_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
  struct lwip_sock *sock;
  err_t err;

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (!SOCK_ADDR_TYPE_MATCH_OR_UNSPEC(name, sock)) {
    /* sockaddr does not match socket type (IPv4/IPv6) */
   sock_set_errno(sock, err_to_errno(ERR_VAL));
   return -1;
  }

  /* check size, familiy and alignment of 'name' */
  LWIP_ERROR("lwip_connect: invalid address", IS_SOCK_ADDR_LEN_VALID(namelen) &&
             IS_SOCK_ADDR_TYPE_VALID_OR_UNSPEC(name) && IS_SOCK_ADDR_ALIGNED(name),
             sock_set_errno(sock, err_to_errno(ERR_ARG)); return -1;);
  LWIP_UNUSED_ARG(namelen);
  if (name->sa_family == AF_UNSPEC) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d, AF_UNSPEC)\r\n", s));
    err = netconn_disconnect(sock->conn);
  } else {
    ipX_addr_t remote_addr;
    u16_t remote_port;
    SOCKADDR_TO_IPXADDR_PORT((name->sa_family == AF_INET6), name, &remote_addr, remote_port);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d, addr=", s));
    ipX_addr_debug_print(name->sa_family == AF_INET6, SOCKETS_DEBUG, &remote_addr);
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F")\r\n", remote_port));

    err = netconn_connect(sock->conn, ipX_2_ip(&remote_addr), remote_port);
  }

  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d) failed, err=%d\r\n", s, err));
    sock_set_errno(sock, err_to_errno(err));
    return -1;
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d) succeeded\r\n", s));
  sock_set_errno(sock, 0);
  return 0;
}

/**
 * Set a socket into listen mode.
 * The socket may not have been used for another connection previously.
 *
 * @param s the socket to set to listening mode
 * @param backlog (ATTENTION: needs TCP_LISTEN_BACKLOG=1)
 * @return 0 on success, non-zero on failure
 */
int
lwip_listen(int s, int backlog)
{
  struct lwip_sock *sock;
  err_t err;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_listen(%d, backlog=%d)\r\n", s, backlog));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  /* limit the "backlog" parameter to fit in an u8_t */
  backlog = LWIP_MIN(LWIP_MAX(backlog, 0), 0xff);

  err = netconn_listen_with_backlog(sock->conn, (u8_t)backlog);

  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_listen(%d) failed, err=%d\r\n", s, err));
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      sock_set_errno(sock, EOPNOTSUPP);
      return EOPNOTSUPP;
    }
    sock_set_errno(sock, err_to_errno(err));
    return -1;
  }

  sock_set_errno(sock, 0);
  return 0;
}

int
lwip_recvfrom(int s, void *mem, size_t len, int flags,
              struct sockaddr *from, socklen_t *fromlen)
{
  struct lwip_sock *sock;
  void             *buf = NULL;
  struct pbuf      *p;
  u16_t            buflen, copylen;
  int              off = 0;
  u8_t             done = 0;
  err_t            err;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d, %p, %d, 0x%x, ..)\r\n", s, mem, len, flags));
  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  do {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: top while sock->lastdata=%p\r\n", sock->lastdata));
    /* Check if there is data left from the last recv operation. */
    if (sock->lastdata) {
      buf = sock->lastdata;
    } else {
      /* If this is non-blocking call, then check first */
      if (((flags & MSG_DONTWAIT) || netconn_is_nonblocking(sock->conn)) && 
          (sock->rcvevent <= 0)) {
        if (off > 0) {
          /* update receive window */
          netconn_recved(sock->conn, (u32_t)off);
          /* already received data, return that */
          sock_set_errno(sock, 0);
          return off;
        }
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): returning EWOULDBLOCK\r\n", s));
        sock_set_errno(sock, EWOULDBLOCK);
        return -1;
      }

      /* No data was left from the previous operation, so we try to get
         some from the network. */
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
        err = netconn_recv_tcp_pbuf(sock->conn, (struct pbuf **)&buf);
      } else {
        err = netconn_recv(sock->conn, (struct netbuf **)&buf);
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: netconn_recv err=%d, netbuf=%p\r\n",
        err, buf));

      if (err != ERR_OK) {
        if (off > 0) {
          /* update receive window */
          netconn_recved(sock->conn, (u32_t)off);
          /* already received data, return that */
          sock_set_errno(sock, 0);
          return off;
        }
        /* We should really do some error checking here. */
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): buf == NULL, error is \"%s\"!\r\n",
          s, lwip_strerr(err)));
        sock_set_errno(sock, err_to_errno(err));
        if (err == ERR_CLSD) {
          return 0;
        } else {
          return -1;
        }
      }
      LWIP_ASSERT("buf != NULL", buf != NULL);
      sock->lastdata = buf;
    }

    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
      p = (struct pbuf *)buf;
    } else {
      p = ((struct netbuf *)buf)->p;
    }
    buflen = p->tot_len;
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: buflen=%d len=%d off=%d sock->lastoffset=%"U16_F"\r\n",
      buflen, len, off, sock->lastoffset));

    buflen -= sock->lastoffset;

    if (len > buflen) {
      copylen = buflen;
    } else {
      copylen = (u16_t)len;
    }

    /* copy the contents of the received buffer into
    the supplied memory pointer mem */
    pbuf_copy_partial(p, (u8_t*)mem + off, copylen, sock->lastoffset);

    off += copylen;

    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
      LWIP_ASSERT("invalid copylen, len would underflow", len >= copylen);
      len -= copylen;
      if ( (len <= 0) || 
           (p->flags & PBUF_FLAG_PUSH) || 
           (sock->rcvevent <= 0) || 
           ((flags & MSG_PEEK)!=0)) {
        done = 1;
      }
    } else {
      done = 1;
    }

    /* Check to see from where the data was.*/
    if (done) {
#if !SOCKETS_DEBUG
      if (from && fromlen)
#endif /* !SOCKETS_DEBUG */
      {
        u16_t port;
        ipX_addr_t tmpaddr;
        ipX_addr_t *fromaddr;
        union sockaddr_aligned saddr;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): addr=", s));
        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
          fromaddr = &tmpaddr;
          /* @todo: this does not work for IPv6, yet */
          netconn_getaddr(sock->conn, ipX_2_ip(fromaddr), &port, 0);
        } else {
          port = netbuf_fromport((struct netbuf *)buf);
          fromaddr = netbuf_fromaddr_ipX((struct netbuf *)buf);
        }
        IPXADDR_PORT_TO_SOCKADDR(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
          &saddr, fromaddr, port);
        ipX_addr_debug_print(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
          SOCKETS_DEBUG, fromaddr);
        LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F" len=%d\r\n", port, off));
#if SOCKETS_DEBUG
        if (from && fromlen)
#endif /* SOCKETS_DEBUG */
        {
          if (*fromlen > saddr.sa.sa_len) {
            *fromlen = saddr.sa.sa_len;
          }
          MEMCPY(from, &saddr, *fromlen);
        }
      }
    }

    /* If we don't peek the incoming message... */
    if ((flags & MSG_PEEK) == 0) {
      /* If this is a TCP socket, check if there is data left in the
         buffer. If so, it should be saved in the sock structure for next
         time around. */
      if ((NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) && (buflen - copylen > 0)) {
        sock->lastdata = buf;
        sock->lastoffset += copylen;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: lastdata now netbuf=%p\r\n", buf));
      } else {
        sock->lastdata = NULL;
        sock->lastoffset = 0;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: deleting netbuf=%p\r\n", buf));
        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
          pbuf_free((struct pbuf *)buf);
        } else {
          netbuf_delete((struct netbuf *)buf);
        }
      }
    }
  } while (!done);

  if ((off > 0) && (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP)) {
    /* update receive window */
    netconn_recved(sock->conn, (u32_t)off);
  }
  sock_set_errno(sock, 0);
  return off;
}

int
lwip_read(int s, void *mem, size_t len)
{
  return lwip_recvfrom(s, mem, len, 0, NULL, NULL);
}

int
lwip_recv(int s, void *mem, size_t len, int flags)
{
  return lwip_recvfrom(s, mem, len, flags, NULL, NULL);
}

int
lwip_send(int s, const void *data, size_t size, int flags)
{
  struct lwip_sock *sock;
  err_t err;
  u8_t write_flags;
  size_t written;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_send(%d, data=%p, size=%d, flags=0x%x)\r\n",
                              s, data, size, flags));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
#if (LWIP_UDP || LWIP_RAW)
    return lwip_sendto(s, data, size, flags, NULL, 0);
#else /* (LWIP_UDP || LWIP_RAW) */
    sock_set_errno(sock, err_to_errno(ERR_ARG));
    return -1;
#endif /* (LWIP_UDP || LWIP_RAW) */
  }

  write_flags = NETCONN_COPY |
    ((flags & MSG_MORE)     ? NETCONN_MORE      : 0) |
    ((flags & MSG_DONTWAIT) ? NETCONN_DONTBLOCK : 0);
  written = 0;
  err = netconn_write_partly(sock->conn, data, size, write_flags, &written);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_send(%d) err=%d written=%d\r\n", s, err, written));
  sock_set_errno(sock, err_to_errno(err));
  return (err == ERR_OK ? (int)written : -1);
}

int
lwip_sendto(int s, const void *data, size_t size, int flags,
       const struct sockaddr *to, socklen_t tolen)
{
  struct lwip_sock *sock;
  err_t err;
  u16_t short_size;
  u16_t remote_port;
#if !LWIP_TCPIP_CORE_LOCKING
  struct netbuf buf;
#endif

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
#if LWIP_TCP
    return lwip_send(s, data, size, flags);
#else /* LWIP_TCP */
    LWIP_UNUSED_ARG(flags);
    sock_set_errno(sock, err_to_errno(ERR_ARG));
    return -1;
#endif /* LWIP_TCP */
  }

  if ((to != NULL) && !SOCK_ADDR_TYPE_MATCH(to, sock)) {
    /* sockaddr does not match socket type (IPv4/IPv6) */
    sock_set_errno(sock, err_to_errno(ERR_VAL));
    return -1;
  }

  /* @todo: split into multiple sendto's? */
  LWIP_ASSERT("lwip_sendto: size must fit in u16_t", size <= 0xffff);
  short_size = (u16_t)size;
  LWIP_ERROR("lwip_sendto: invalid address", (((to == NULL) && (tolen == 0)) ||
             (IS_SOCK_ADDR_LEN_VALID(tolen) &&
             IS_SOCK_ADDR_TYPE_VALID(to) && IS_SOCK_ADDR_ALIGNED(to))),
             sock_set_errno(sock, err_to_errno(ERR_ARG)); return -1;);
  LWIP_UNUSED_ARG(tolen);

#if LWIP_TCPIP_CORE_LOCKING
  /* Special speedup for fast UDP/RAW sending: call the raw API directly
     instead of using the netconn functions. */
  {
    struct pbuf* p;
    ipX_addr_t *remote_addr;
    ipX_addr_t remote_addr_tmp;

#if LWIP_NETIF_TX_SINGLE_PBUF
    p = pbuf_alloc(PBUF_TRANSPORT, short_size, PBUF_RAM);
    if (p != NULL) {
#if LWIP_CHECKSUM_ON_COPY
      u16_t chksum = 0;
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_RAW) {
        chksum = LWIP_CHKSUM_COPY(p->payload, data, short_size);
      } else
#endif /* LWIP_CHECKSUM_ON_COPY */
      MEMCPY(p->payload, data, size);
#else /* LWIP_NETIF_TX_SINGLE_PBUF */
    p = pbuf_alloc(PBUF_TRANSPORT, short_size, PBUF_REF);
    if (p != NULL) {
      p->payload = (void*)data;
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */

      if (to != NULL) {
        SOCKADDR_TO_IPXADDR_PORT(to->sa_family == AF_INET6,
          to, &remote_addr_tmp, remote_port);
        remote_addr = &remote_addr_tmp;
      } else {
        remote_addr = &sock->conn->pcb.ip->remote_ip;
#if LWIP_UDP
        if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_UDP) {
          remote_port = sock->conn->pcb.udp->remote_port;
        } else
#endif /* LWIP_UDP */
        {
          remote_port = 0;
        }
      }

      LOCK_TCPIP_CORE();
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_RAW) {
#if LWIP_RAW
        err = sock->conn->last_err = raw_sendto(sock->conn->pcb.raw, p, ipX_2_ip(remote_addr));
#else /* LWIP_RAW */
        err = ERR_ARG;
#endif /* LWIP_RAW */
      }
#if LWIP_UDP && LWIP_RAW
      else
#endif /* LWIP_UDP && LWIP_RAW */
      {
#if LWIP_UDP
#if LWIP_CHECKSUM_ON_COPY && LWIP_NETIF_TX_SINGLE_PBUF
        err = sock->conn->last_err = udp_sendto_chksum(sock->conn->pcb.udp, p,
          ipX_2_ip(remote_addr), remote_port, 1, chksum);
#else /* LWIP_CHECKSUM_ON_COPY && LWIP_NETIF_TX_SINGLE_PBUF */
        err = sock->conn->last_err = udp_sendto(sock->conn->pcb.udp, p,
          ipX_2_ip(remote_addr), remote_port);
#endif /* LWIP_CHECKSUM_ON_COPY && LWIP_NETIF_TX_SINGLE_PBUF */
#else /* LWIP_UDP */
        err = ERR_ARG;
#endif /* LWIP_UDP */
      }
      UNLOCK_TCPIP_CORE();
      
      pbuf_free(p);
    } else {
      err = ERR_MEM;
    }
  }
#else /* LWIP_TCPIP_CORE_LOCKING */
  /* initialize a buffer */
  buf.p = buf.ptr = NULL;
#if LWIP_CHECKSUM_ON_COPY
  buf.flags = 0;
#endif /* LWIP_CHECKSUM_ON_COPY */
  if (to) {
    SOCKADDR_TO_IPXADDR_PORT((to->sa_family) == AF_INET6, to, &buf.addr, remote_port);
  } else {
    remote_port = 0;
    ipX_addr_set_any(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)), &buf.addr);
  }
  netbuf_fromport(&buf) = remote_port;


  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_sendto(%d, data=%p, short_size=%"U16_F", flags=0x%x to=",
              s, data, short_size, flags));
  ipX_addr_debug_print(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
    SOCKETS_DEBUG, &buf.addr);
  LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F"\r\n", remote_port));

  /* make the buffer point to the data that should be sent */
#if LWIP_NETIF_TX_SINGLE_PBUF
  /* Allocate a new netbuf and copy the data into it. */
  if (netbuf_alloc(&buf, short_size) == NULL) {
    err = ERR_MEM;
  } else {
#if LWIP_CHECKSUM_ON_COPY
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_RAW) {
      u16_t chksum = LWIP_CHKSUM_COPY(buf.p->payload, data, short_size);
      netbuf_set_chksum(&buf, chksum);
      err = ERR_OK;
    } else
#endif /* LWIP_CHECKSUM_ON_COPY */
    {
      err = netbuf_take(&buf, data, short_size);
    }
  }
#else /* LWIP_NETIF_TX_SINGLE_PBUF */
  err = netbuf_ref(&buf, data, short_size);
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */
  if (err == ERR_OK) {
    /* send the data */
    err = netconn_send(sock->conn, &buf);
  }

  /* deallocated the buffer */
  netbuf_free(&buf);
#endif /* LWIP_TCPIP_CORE_LOCKING */
  sock_set_errno(sock, err_to_errno(err));
  return (err == ERR_OK ? short_size : -1);
}

int
lwip_socket(int domain, int type, int protocol)
{
  struct netconn *conn;
  int i;

#if !LWIP_IPV6
  LWIP_UNUSED_ARG(domain); /* @todo: check this */
#endif /* LWIP_IPV6 */

  /* create a netconn */
  switch (type) {
  case SOCK_RAW:
    conn = netconn_new_with_proto_and_callback(DOMAIN_TO_NETCONN_TYPE(domain, NETCONN_RAW),
                                               (u8_t)protocol, event_callback);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%s, SOCK_RAW, %d) = ",
                                 domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
    break;
  case SOCK_DGRAM:
    conn = netconn_new_with_callback(DOMAIN_TO_NETCONN_TYPE(domain,
                 ((protocol == IPPROTO_UDPLITE) ? NETCONN_UDPLITE : NETCONN_UDP)) ,
                 event_callback);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%s, SOCK_DGRAM, %d) = ",
                                 domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
    break;
  case SOCK_STREAM:
    conn = netconn_new_with_callback(DOMAIN_TO_NETCONN_TYPE(domain, NETCONN_TCP), event_callback);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%s, SOCK_STREAM, %d) = ",
                                 domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
    if (conn != NULL) {
      /* Prevent automatic window updates, we do this on our own! */
      netconn_set_noautorecved(conn, 1);
    }
    break;
  default:
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%d, %d/UNKNOWN, %d) = -1\r\n",
                                 domain, type, protocol));
    set_errno(EINVAL);
    return -1;
  }

  if (!conn) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("-1 / ENOBUFS (could not create netconn)\r\n"));
    set_errno(ENOBUFS);
    return -1;
  }

  i = alloc_socket(conn, 0);

  if (i == -1) {
    netconn_delete(conn);
    set_errno(ENFILE);
    return -1;
  }
  conn->socket = i;
  LWIP_DEBUGF(SOCKETS_DEBUG, ("%d\r\n", i));
  set_errno(0);
  return i;
}

int
lwip_write(int s, const void *data, size_t size)
{
  return lwip_send(s, data, size, 0);
}

/**
 * Go through the readset and writeset lists and see which socket of the sockets
 * set in the sets has events. On return, readset, writeset and exceptset have
 * the sockets enabled that had events.
 *
 * exceptset is not used for now!!!
 *
 * @param maxfdp1 the highest socket index in the sets
 * @param readset_in:    set of sockets to check for read events
 * @param writeset_in:   set of sockets to check for write events
 * @param exceptset_in:  set of sockets to check for error events
 * @param readset_out:   set of sockets that had read events
 * @param writeset_out:  set of sockets that had write events
 * @param exceptset_out: set os sockets that had error events
 * @return number of sockets that had events (read/write/exception) (>= 0)
 */
static int
lwip_selscan(int maxfdp1, fd_set *readset_in, fd_set *writeset_in, fd_set *exceptset_in,
             fd_set *readset_out, fd_set *writeset_out, fd_set *exceptset_out)
{
  int i, nready = 0;
  fd_set lreadset, lwriteset, lexceptset;
  struct lwip_sock *sock;
  SYS_ARCH_DECL_PROTECT(lev);

  FD_ZERO(&lreadset);
  FD_ZERO(&lwriteset);
  FD_ZERO(&lexceptset);

  /* Go through each socket in each list to count number of sockets which
     currently match */
  for(i = 0; i < maxfdp1; i++) {
    void* lastdata = NULL;
    s16_t rcvevent = 0;
    u16_t sendevent = 0;
    u16_t errevent = 0;
    /* First get the socket's status (protected)... */
    SYS_ARCH_PROTECT(lev);
    sock = tryget_socket(i);
    if (sock != NULL) {
      lastdata = sock->lastdata;
      rcvevent = sock->rcvevent;
      sendevent = sock->sendevent;
      errevent = sock->errevent;
    }
    SYS_ARCH_UNPROTECT(lev);
    /* ... then examine it: */
    /* See if netconn of this socket is ready for read */
    if (readset_in && FD_ISSET(i, readset_in) && ((lastdata != NULL) || (rcvevent > 0))) {
      FD_SET(i, &lreadset);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for reading\r\n", i));
      nready++;
    }
    /* See if netconn of this socket is ready for write */
    if (writeset_in && FD_ISSET(i, writeset_in) && (sendevent != 0)) {
      FD_SET(i, &lwriteset);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for writing\r\n", i));
      nready++;
    }
    /* See if netconn of this socket had an error */
    if (exceptset_in && FD_ISSET(i, exceptset_in) && (errevent != 0)) {
      FD_SET(i, &lexceptset);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for exception\r\n", i));
      nready++;
    }
  }
  /* copy local sets to the ones provided as arguments */
  *readset_out = lreadset;
  *writeset_out = lwriteset;
  *exceptset_out = lexceptset;

  LWIP_ASSERT("nready >= 0", nready >= 0);
  return nready;
}

/**
 * Processing exceptset is not yet implemented.
 */
#if 0
int
lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout)
{
  u32_t waitres = 0;
  int nready;
  fd_set lreadset, lwriteset, lexceptset;
  u32_t msectimeout;
  struct lwip_select_cb select_cb;
  err_t err;
  int i;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select(%d, %p, %p, %p, tvsec=%"S32_F" tvusec=%"S32_F")\r\n",
                  maxfdp1, (void *)readset, (void *) writeset, (void *) exceptset,
                  timeout ? (s32_t)timeout->tv_sec : (s32_t)-1,
                  timeout ? (s32_t)timeout->tv_usec : (s32_t)-1));

  /* Go through each socket in each list to count number of sockets which
     currently match */
  nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);

  /* If we don't have any current events, then suspend if we are supposed to */
  if (!nready) {
    if (timeout && timeout->tv_sec == 0 && timeout->tv_usec == 0) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: no timeout, returning 0\r\n"));
      /* This is OK as the local fdsets are empty and nready is zero,
         or we would have returned earlier. */
      goto return_copy_fdsets;
    }

    /* None ready: add our semaphore to list:
       We don't actually need any dynamic memory. Our entry on the
       list is only valid while we are in this function, so it's ok
       to use local variables. */

    select_cb.next = NULL;
    select_cb.prev = NULL;
    select_cb.readset = readset;
    select_cb.writeset = writeset;
    select_cb.exceptset = exceptset;
    select_cb.sem_signalled = 0;
    err = sys_sem_new(&select_cb.sem, 0);
    if (err != ERR_OK) {
      /* failed to create semaphore */
      set_errno(ENOMEM);
      return -1;
    }

    /* Protect the select_cb_list */
    SYS_ARCH_PROTECT(lev);

    /* Put this select_cb on top of list */
    select_cb.next = select_cb_list;
    if (select_cb_list != NULL) {
      select_cb_list->prev = &select_cb;
    }
    select_cb_list = &select_cb;
    /* Increasing this counter tells even_callback that the list has changed. */
    select_cb_ctr++;

    /* Now we can safely unprotect */
    SYS_ARCH_UNPROTECT(lev);

    /* Increase select_waiting for each socket we are interested in */
    for(i = 0; i < maxfdp1; i++) {
      if ((readset && FD_ISSET(i, readset)) ||
          (writeset && FD_ISSET(i, writeset)) ||
          (exceptset && FD_ISSET(i, exceptset))) {
        struct lwip_sock *sock = tryget_socket(i);
        LWIP_ASSERT("sock != NULL", sock != NULL);
        SYS_ARCH_PROTECT(lev);
        sock->select_waiting++;
        LWIP_ASSERT("sock->select_waiting > 0", sock->select_waiting > 0);
        SYS_ARCH_UNPROTECT(lev);
      }
    }

    /* Call lwip_selscan again: there could have been events between
       the last scan (whithout us on the list) and putting us on the list! */
    nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
    if (!nready) {
      /* Still none ready, just wait to be woken */
      if (timeout == 0) {
        /* Wait forever */
        msectimeout = 0;
      } else {
        msectimeout =  ((timeout->tv_sec * 1000) + ((timeout->tv_usec + 500)/1000));
        if (msectimeout == 0) {
          /* Wait 1ms at least (0 means wait forever) */
          msectimeout = 1;
        }
      }

      waitres = sys_arch_sem_wait(&select_cb.sem, msectimeout);
    }
    /* Increase select_waiting for each socket we are interested in */
    for(i = 0; i < maxfdp1; i++) {
      if ((readset && FD_ISSET(i, readset)) ||
          (writeset && FD_ISSET(i, writeset)) ||
          (exceptset && FD_ISSET(i, exceptset))) {
        struct lwip_sock *sock = tryget_socket(i);
        LWIP_ASSERT("sock != NULL", sock != NULL);
        SYS_ARCH_PROTECT(lev);
        sock->select_waiting--;
        LWIP_ASSERT("sock->select_waiting >= 0", sock->select_waiting >= 0);
        SYS_ARCH_UNPROTECT(lev);
      }
    }
    /* Take us off the list */
    SYS_ARCH_PROTECT(lev);
    if (select_cb.next != NULL) {
      select_cb.next->prev = select_cb.prev;
    }
    if (select_cb_list == &select_cb) {
      LWIP_ASSERT("select_cb.prev == NULL", select_cb.prev == NULL);
      select_cb_list = select_cb.next;
    } else {
      LWIP_ASSERT("select_cb.prev != NULL", select_cb.prev != NULL);
      select_cb.prev->next = select_cb.next;
    }
    /* Increasing this counter tells even_callback that the list has changed. */
    select_cb_ctr++;
    SYS_ARCH_UNPROTECT(lev);

    sys_sem_free(&select_cb.sem);
    if (waitres == SYS_ARCH_TIMEOUT)  {
      /* Timeout */
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: timeout expired\r\n"));
      /* This is OK as the local fdsets are empty and nready is zero,
         or we would have returned earlier. */
      goto return_copy_fdsets;
    }

    /* See what's set */
    nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: nready=%d\r\n", nready));
return_copy_fdsets:
  set_errno(0);
  if (readset) {
    *readset = lreadset;
  }
  if (writeset) {
    *writeset = lwriteset;
  }
  if (exceptset) {
    *exceptset = lexceptset;
  }
  return nready;
}
#else
int
lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout)
{
  u32_t waitres = 0;
  int nready;
  fd_set lreadset, lwriteset, lexceptset;
  u32_t msectimeout;
  struct lwip_select_cb *select_cb;
  err_t err;
  int i;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select(%d, %p, %p, %p, tvsec=%"S32_F" tvusec=%"S32_F")\r\n",
                  maxfdp1, (void *)readset, (void *) writeset, (void *) exceptset,
                  timeout ? (s32_t)timeout->tv_sec : (s32_t)-1,
                  timeout ? (s32_t)timeout->tv_usec : (s32_t)-1));

  /* Go through each socket in each list to count number of sockets which
     currently match */
  nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);

  /* If we don't have any current events, then suspend if we are supposed to */
  if (!nready) {
    if (timeout && timeout->tv_sec == 0 && timeout->tv_usec == 0) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: no timeout, returning 0\r\n"));
      /* This is OK as the local fdsets are empty and nready is zero,
         or we would have returned earlier. */
      goto return_copy_fdsets;
    }

    /* None ready: add our semaphore to list:
       We don't actually need any dynamic memory. Our entry on the
       list is only valid while we are in this function, so it's ok
       to use local variables. */
       
    select_cb = (struct lwip_select_cb *)OS_MemAlloc(sizeof(struct lwip_select_cb));
    select_cb->next = NULL;
    select_cb->prev = NULL;
    select_cb->readset = readset;
    select_cb->writeset = writeset;
    select_cb->exceptset = exceptset;
    select_cb->sem_signalled = 0;
    err = sys_sem_new(&(select_cb->sem), 0);
    if (err != ERR_OK) {
      /* failed to create semaphore */
      set_errno(ENOMEM);
      OS_MemFree(select_cb);
      return -1;
    }

    /* Protect the select_cb_list */
    SYS_ARCH_PROTECT(lev);

    /* Put this select_cb on top of list */
    select_cb->next = select_cb_list;
    if (select_cb_list != NULL) {
      select_cb_list->prev = select_cb;
    }
    select_cb_list = select_cb;
    /* Increasing this counter tells even_callback that the list has changed. */
    select_cb_ctr++;

    /* Now we can safely unprotect */
    SYS_ARCH_UNPROTECT(lev);

    /* Increase select_waiting for each socket we are interested in */
    for(i = 0; i < maxfdp1; i++) {
      if ((readset && FD_ISSET(i, readset)) ||
          (writeset && FD_ISSET(i, writeset)) ||
          (exceptset && FD_ISSET(i, exceptset))) {
        struct lwip_sock *sock = tryget_socket(i);
        LWIP_ASSERT("sock != NULL", sock != NULL);
        SYS_ARCH_PROTECT(lev);
        sock->select_waiting++;
        LWIP_ASSERT("sock->select_waiting > 0", sock->select_waiting > 0);
        SYS_ARCH_UNPROTECT(lev);
      }
    }

    /* Call lwip_selscan again: there could have been events between
       the last scan (whithout us on the list) and putting us on the list! */
    nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
    if (!nready) {
      /* Still none ready, just wait to be woken */
      if (timeout == 0) {
        /* Wait forever */
        msectimeout = 0;
      } else {
        msectimeout =  ((timeout->tv_sec * 1000) + ((timeout->tv_usec + 500)/1000));
        if (msectimeout == 0) {
          /* Wait 1ms at least (0 means wait forever) */
          msectimeout = 1;
        }
      }

      waitres = sys_arch_sem_wait(&(select_cb->sem), msectimeout);
    }
    /* Increase select_waiting for each socket we are interested in */
    for(i = 0; i < maxfdp1; i++) {
      if ((readset && FD_ISSET(i, readset)) ||
          (writeset && FD_ISSET(i, writeset)) ||
          (exceptset && FD_ISSET(i, exceptset))) {
        struct lwip_sock *sock = tryget_socket(i);
        LWIP_ASSERT("sock != NULL", sock != NULL);
        SYS_ARCH_PROTECT(lev);
        sock->select_waiting--;
        LWIP_ASSERT("sock->select_waiting >= 0", sock->select_waiting >= 0);
        SYS_ARCH_UNPROTECT(lev);
      }
    }
    /* Take us off the list */
    SYS_ARCH_PROTECT(lev);
    if (select_cb->next != NULL) {
      select_cb->next->prev = select_cb->prev;
    }
    if (select_cb_list == select_cb) {
      LWIP_ASSERT("select_cb->prev == NULL", select_cb->prev == NULL);
      select_cb_list = select_cb->next;
    } else {
      LWIP_ASSERT("select_cb->prev != NULL", select_cb->prev != NULL);
      select_cb->prev->next = select_cb->next;
    }
    /* Increasing this counter tells even_callback that the list has changed. */
    select_cb_ctr++;
    SYS_ARCH_UNPROTECT(lev);

    sys_sem_free(&(select_cb->sem));
    OS_MemFree(select_cb);
    if (waitres == SYS_ARCH_TIMEOUT)  {
      /* Timeout */
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: timeout expired\r\n"));
      /* This is OK as the local fdsets are empty and nready is zero,
         or we would have returned earlier. */
      goto return_copy_fdsets;
    }

    /* See what's set */
    nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: nready=%d\r\n", nready));
return_copy_fdsets:
  set_errno(0);
  if (readset) {
    *readset = lreadset;
  }
  if (writeset) {
    *writeset = lwriteset;
  }
  if (exceptset) {
    *exceptset = lexceptset;
  }
  return nready;
}
#endif
/**
 * Callback registered in the netconn layer for each socket-netconn.
 * Processes recvevent (data available) and wakes up tasks waiting for select.
 */
static void
event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len)
{
  int s;
  struct lwip_sock *sock;
  struct lwip_select_cb *scb;
  int last_select_cb_ctr;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_UNUSED_ARG(len);

  /* Get socket */
  if (conn) {
    s = conn->socket;
    if (s < 0) {
      /* Data comes in right away after an accept, even though
       * the server task might not have created a new socket yet.
       * Just count down (or up) if that's the case and we
       * will use the data later. Note that only receive events
       * can happen before the new socket is set up. */
      SYS_ARCH_PROTECT(lev);
      if (conn->socket < 0) {
        if (evt == NETCONN_EVT_RCVPLUS) {
          conn->socket--;
        }
        SYS_ARCH_UNPROTECT(lev);
        return;
      }
      s = conn->socket;
      SYS_ARCH_UNPROTECT(lev);
    }

    sock = get_socket(s);
    if (!sock) {
      return;
    }
  } else {
    return;
  }

  SYS_ARCH_PROTECT(lev);
  /* Set event as required */
  switch (evt) {
    case NETCONN_EVT_RCVPLUS:
      sock->rcvevent++;
      break;
    case NETCONN_EVT_RCVMINUS:
      sock->rcvevent--;
      break;
    case NETCONN_EVT_SENDPLUS:
      sock->sendevent = 1;
      break;
    case NETCONN_EVT_SENDMINUS:
      sock->sendevent = 0;
      break;
    case NETCONN_EVT_ERROR:
      sock->errevent = 1;
      break;
    default:
      LWIP_ASSERT("unknown event", 0);
      break;
  }

  if (sock->select_waiting == 0) {
    /* noone is waiting for this socket, no need to check select_cb_list */
    SYS_ARCH_UNPROTECT(lev);
    return;
  }

  /* Now decide if anyone is waiting for this socket */
  /* NOTE: This code goes through the select_cb_list list multiple times
     ONLY IF a select was actually waiting. We go through the list the number
     of waiting select calls + 1. This list is expected to be small. */

  /* At this point, SYS_ARCH is still protected! */
again:
  for (scb = select_cb_list; scb != NULL; scb = scb->next) {
    if (scb->sem_signalled == 0) {
      /* semaphore not signalled yet */
      int do_signal = 0;
      /* Test this select call for our socket */
      if (sock->rcvevent > 0) {
        if (scb->readset && FD_ISSET(s, scb->readset)) {
          do_signal = 1;
        }
      }
      if (sock->sendevent != 0) {
        if (!do_signal && scb->writeset && FD_ISSET(s, scb->writeset)) {
          do_signal = 1;
        }
      }
      if (sock->errevent != 0) {
        if (!do_signal && scb->exceptset && FD_ISSET(s, scb->exceptset)) {
          do_signal = 1;
        }
      }
      if (do_signal) {
        scb->sem_signalled = 1;
        /* Don't call SYS_ARCH_UNPROTECT() before signaling the semaphore, as this might
           lead to the select thread taking itself off the list, invalidagin the semaphore. */
        sys_sem_signal(&scb->sem);
      }
    }
    /* unlock interrupts with each step */
    last_select_cb_ctr = select_cb_ctr;
    SYS_ARCH_UNPROTECT(lev);
    /* this makes sure interrupt protection time is short */
    SYS_ARCH_PROTECT(lev);
    if (last_select_cb_ctr != select_cb_ctr) {
      /* someone has changed select_cb_list, restart at the beginning */
      goto again;
    }
  }
  SYS_ARCH_UNPROTECT(lev);
}

/**
 * Unimplemented: Close one end of a full-duplex connection.
 * Currently, the full connection is closed.
 */
int
lwip_shutdown(int s, int how)
{
  struct lwip_sock *sock;
  err_t err;
  u8_t shut_rx = 0, shut_tx = 0;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_shutdown(%d, how=%d)\r\n", s, how));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (sock->conn != NULL) {
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      sock_set_errno(sock, EOPNOTSUPP);
      return EOPNOTSUPP;
    }
  } else {
    sock_set_errno(sock, ENOTCONN);
    return ENOTCONN;
  }

  if (how == SHUT_RD) {
    shut_rx = 1;
  } else if (how == SHUT_WR) {
    shut_tx = 1;
  } else if(how == SHUT_RDWR) {
    shut_rx = 1;
    shut_tx = 1;
  } else {
    sock_set_errno(sock, EINVAL);
    return EINVAL;
  }
  err = netconn_shutdown(sock->conn, shut_rx, shut_tx);

  sock_set_errno(sock, err_to_errno(err));
  return (err == ERR_OK ? 0 : -1);
}

static int
lwip_getaddrname(int s, struct sockaddr *name, socklen_t *namelen, u8_t local)
{
  struct lwip_sock *sock;
  union sockaddr_aligned saddr;
  ipX_addr_t naddr;
  u16_t port;

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  /* get the IP address and port */
  /* @todo: this does not work for IPv6, yet */
  netconn_getaddr(sock->conn, ipX_2_ip(&naddr), &port, local);
  IPXADDR_PORT_TO_SOCKADDR(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
    &saddr, &naddr, port);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getaddrname(%d, addr=", s));
  ipX_addr_debug_print(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
    SOCKETS_DEBUG, &naddr);
  LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F")\r\n", port));

  if (*namelen > saddr.sa.sa_len) {
    *namelen = saddr.sa.sa_len;
  }
  MEMCPY(name, &saddr, *namelen);

  sock_set_errno(sock, 0);
  return 0;
}

int
lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
  return lwip_getaddrname(s, name, namelen, 0);
}

int
lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
  return lwip_getaddrname(s, name, namelen, 1);
}

int
lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
  err_t err = ERR_OK;
  struct lwip_sock *sock = get_socket(s);
  struct lwip_setgetsockopt_data data;

  if (!sock) {
    return -1;
  }

  if ((NULL == optval) || (NULL == optlen)) {
    sock_set_errno(sock, EFAULT);
    return -1;
  }

  /* Do length and type checks for the various options first, to keep it readable. */
  switch (level) {
   
/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {
       
    case SO_ACCEPTCONN:
    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_ERROR:
    case SO_KEEPALIVE:
    /* UNIMPL case SO_CONTIMEO: */
#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
#endif /* LWIP_SO_RCVBUF */
    /* UNIMPL case SO_OOBINLINE: */
    /* UNIMPL case SO_SNDBUF: */
    /* UNIMPL case SO_RCVLOWAT: */
    /* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    case SO_TYPE:
    /* UNIMPL case SO_USELOOPBACK: */
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;

    case SO_NO_CHECK:
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
#if LWIP_UDP
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP ||
          ((udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0)) {
        /* this flag is only available for UDP, not for UDP lite */
        err = EAFNOSUPPORT;
      }
#endif /* LWIP_UDP */
      break;

    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\r\n",
                                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
                     
/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    /* UNIMPL case IP_HDRINCL: */
    /* UNIMPL case IP_RCVDSTADDR: */
    /* UNIMPL case IP_RCVIF: */
    case IP_TTL:
    case IP_TOS:
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      if (*optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      break;
    case IP_MULTICAST_IF:
      if (*optlen < sizeof(struct in_addr)) {
        err = EINVAL;
      }
      break;
    case IP_MULTICAST_LOOP:
      if (*optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
#endif /* LWIP_IGMP */

    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\r\n",
                                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
         
#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    if (*optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }
    
    /* If this is no TCP socket, ignore any options. */
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
      return 0;

    switch (optname) {
    case TCP_NODELAY:
    case TCP_KEEPALIVE:
#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
    case TCP_KEEPINTVL:
    case TCP_KEEPCNT:
#endif /* LWIP_TCP_KEEPALIVE */
      break;
       
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\r\n",
                                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP */

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
      /* @todo: this does not work for datagram sockets, yet */
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
        return 0;
      break;
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, UNIMPL: optname=0x%x, ..)\r\n",
                                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
/* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    if (*optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }
    
    /* If this is no UDP lite socket, ignore any options. */
    if (!NETCONNTYPE_ISUDPLITE(netconn_type(sock->conn))) {
      return 0;
    }

    switch (optname) {
    case UDPLITE_SEND_CSCOV:
    case UDPLITE_RECV_CSCOV:
      break;
       
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\r\n",
                                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP && LWIP_UDPLITE*/
/* UNDEFINED LEVEL */
  default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\r\n",
                                  s, level, optname));
      err = ENOPROTOOPT;
  }  /* switch */

   
  if (err != ERR_OK) {
    sock_set_errno(sock, err);
    return -1;
  }

  /* Now do the actual option processing */
  data.sock = sock;
#ifdef LWIP_DEBUG
  data.s = s;
#endif /* LWIP_DEBUG */
  data.level = level;
  data.optname = optname;
  data.optval = optval;
  data.optlen = optlen;
  data.err = err;
  tcpip_callback(lwip_getsockopt_internal, &data);
  sys_arch_sem_wait(&sock->conn->op_completed, 0);
  /* maybe lwip_getsockopt_internal has changed err */
  err = data.err;

  sock_set_errno(sock, err);
  return err ? -1 : 0;
}

static void
lwip_getsockopt_internal(void *arg)
{
  struct lwip_sock *sock;
#ifdef LWIP_DEBUG
  int s;
#endif /* LWIP_DEBUG */
  int level, optname;
  void *optval;
  struct lwip_setgetsockopt_data *data;

  LWIP_ASSERT("arg != NULL", arg != NULL);

  data = (struct lwip_setgetsockopt_data*)arg;
  sock = data->sock;
#ifdef LWIP_DEBUG
  s = data->s;
#endif /* LWIP_DEBUG */
  level = data->level;
  optname = data->optname;
  optval = data->optval;

  switch (level) {

/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {

    /* The option flags */
    case SO_ACCEPTCONN:
    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_KEEPALIVE:
    /* UNIMPL case SO_OOBINCLUDE: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    /*case SO_USELOOPBACK: UNIMPL */
      *(int*)optval = ip_get_option(sock->conn->pcb.ip, optname);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, optname=0x%x, ..) = %s\r\n",
                                  s, optname, (*(int*)optval?"on":"off")));
      break;

    case SO_TYPE:
      switch (NETCONNTYPE_GROUP(netconn_type(sock->conn))) {
      case NETCONN_RAW:
        *(int*)optval = SOCK_RAW;
        break;
      case NETCONN_TCP:
        *(int*)optval = SOCK_STREAM;
        break;
      case NETCONN_UDP:
        *(int*)optval = SOCK_DGRAM;
        break;
      default: /* unrecognized socket type */
        *(int*)optval = netconn_type(sock->conn);
        LWIP_DEBUGF(SOCKETS_DEBUG,
                    ("lwip_getsockopt(%d, SOL_SOCKET, SO_TYPE): unrecognized socket type %d\r\n",
                    s, *(int *)optval));
      }  /* switch (netconn_type(sock->conn)) */
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, SO_TYPE) = %d\r\n",
                  s, *(int *)optval));
      break;

    case SO_ERROR:
      /* only overwrite ERR_OK or tempoary errors */
      if ((sock->err == 0) || (sock->err == EINPROGRESS)) {
        sock_set_errno(sock, err_to_errno(sock->conn->last_err));
      } 
      *(int *)optval = sock->err;
      sock->err = 0;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, SO_ERROR) = %d\r\n",
                  s, *(int *)optval));
      break;

#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
      *(int *)optval = netconn_get_sendtimeout(sock->conn);
      break;
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
      *(int *)optval = netconn_get_recvtimeout(sock->conn);
      break;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
      *(int *)optval = netconn_get_recvbufsize(sock->conn);
      break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_UDP
    case SO_NO_CHECK:
      *(int*)optval = (udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_NOCHKSUM) ? 1 : 0;
      break;
#endif /* LWIP_UDP*/
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    case IP_TTL:
      *(int*)optval = sock->conn->pcb.ip->ttl;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_TTL) = %d\r\n",
                  s, *(int *)optval));
      break;
    case IP_TOS:
      *(int*)optval = sock->conn->pcb.ip->tos;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_TOS) = %d\r\n",
                  s, *(int *)optval));
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      *(u8_t*)optval = sock->conn->pcb.ip->ttl;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_TTL) = %d\r\n",
                  s, *(int *)optval));
      break;
    case IP_MULTICAST_IF:
      inet_addr_from_ipaddr((struct in_addr*)optval, &sock->conn->pcb.udp->multicast_ip);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_IF) = 0x%"X32_F"\r\n",
                  s, *(u32_t *)optval));
      break;
    case IP_MULTICAST_LOOP:
      if ((sock->conn->pcb.udp->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) {
        *(u8_t*)optval = 1;
      } else {
        *(u8_t*)optval = 0;
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_LOOP) = %d\r\n",
                  s, *(int *)optval));
      break;
#endif /* LWIP_IGMP */
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    switch (optname) {
    case TCP_NODELAY:
      *(int*)optval = tcp_nagle_disabled(sock->conn->pcb.tcp);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_NODELAY) = %s\r\n",
                  s, (*(int*)optval)?"on":"off") );
      break;
    case TCP_KEEPALIVE:
      *(int*)optval = (int)sock->conn->pcb.tcp->keep_idle;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPALIVE) = %d\r\n",
                  s, *(int *)optval));
      break;

#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
      *(int*)optval = (int)(sock->conn->pcb.tcp->keep_idle/1000);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPIDLE) = %d\r\n",
                  s, *(int *)optval));
      break;
    case TCP_KEEPINTVL:
      *(int*)optval = (int)(sock->conn->pcb.tcp->keep_intvl/1000);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPINTVL) = %d\r\n",
                  s, *(int *)optval));
      break;
    case TCP_KEEPCNT:
      *(int*)optval = (int)sock->conn->pcb.tcp->keep_cnt;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPCNT) = %d\r\n",
                  s, *(int *)optval));
      break;
#endif /* LWIP_TCP_KEEPALIVE */
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP */

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      *(int*)optval = ((sock->conn->flags & NETCONN_FLAG_IPV6_V6ONLY) ? 1 : 0);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, IPV6_V6ONLY) = %d\r\n",
                  s, *(int *)optval));
      break;
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
  /* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    switch (optname) {
    case UDPLITE_SEND_CSCOV:
      *(int*)optval = sock->conn->pcb.udp->chksum_len_tx;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) = %d\r\n",
                  s, (*(int*)optval)) );
      break;
    case UDPLITE_RECV_CSCOV:
      *(int*)optval = sock->conn->pcb.udp->chksum_len_rx;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) = %d\r\n",
                  s, (*(int*)optval)) );
      break;
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP */
  default:
    LWIP_ASSERT("unhandled level", 0);
    break;
  } /* switch (level) */
  sys_sem_signal(&sock->conn->op_completed);
}

err_t sockbinddevice(struct netconn *conn, char *netifname)
{
    struct netif *netif;
    
    netif = netif_find(netifname);
    if(netif != NULL)
    {
#if LWIP_IPV6
        conn->pcb.udp->local_ip.ip4 = netif->ip_addr;
#else
        conn->pcb.udp->local_ip = netif->ip_addr;
#endif
        return ERR_OK;
    }
    else
        return ERR_VAL;
}

int
lwip_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
  struct lwip_sock *sock = get_socket(s);
  err_t err = ERR_OK;
  struct lwip_setgetsockopt_data data;

  if (!sock) {
    return -1;
  }

  if (NULL == optval) {
    sock_set_errno(sock, EFAULT);
    return -1;
  }

  /* Do length and type checks for the various options first, to keep it readable. */
  switch (level) {

/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {

    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_KEEPALIVE:
    /* UNIMPL case case SO_CONTIMEO: */
#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
#endif /* LWIP_SO_RCVBUF */
    /* UNIMPL case SO_OOBINLINE: */
    /* UNIMPL case SO_SNDBUF: */
    /* UNIMPL case SO_RCVLOWAT: */
    /* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    /* UNIMPL case SO_USELOOPBACK: */
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;
    case SO_NO_CHECK:
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }
#if LWIP_UDP
      if ((NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) ||
          ((udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0)) {
        /* this flag is only available for UDP, not for UDP lite */
        err = EAFNOSUPPORT;
      }
#endif /* LWIP_UDP */
      break;
    case SO_BINDTODEVICE:
      if (optlen < sizeof(struct ifreq) || NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EINVAL;
      }
      break;
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\r\n",
                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;

/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    /* UNIMPL case IP_HDRINCL: */
    /* UNIMPL case IP_RCVDSTADDR: */
    /* UNIMPL case IP_RCVIF: */
    case IP_TTL:
    case IP_TOS:
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      if (optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IP_MULTICAST_IF:
      if (optlen < sizeof(struct in_addr)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IP_MULTICAST_LOOP:
      if (optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
      if (optlen < sizeof(struct ip_mreq)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
#endif /* LWIP_IGMP */
      default:
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\r\n",
                    s, optname));
        err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    if (optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }

    /* If this is no TCP socket, ignore any options. */
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
      return 0;

    switch (optname) {
    case TCP_NODELAY:
    case TCP_KEEPALIVE:
#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
    case TCP_KEEPINTVL:
    case TCP_KEEPCNT:
#endif /* LWIP_TCP_KEEPALIVE */
      break;

    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\r\n",
                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP */

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }

      /* @todo: this does not work for datagram sockets, yet */
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
        return 0;

      break;
#if LWIP_IGMP
    case IPV6_MULTICAST_TTL:
      if (optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IPV6_MULTICAST_IF:
      if (optlen < sizeof(struct in_addr)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IPV6_MULTICAST_LOOP:
      if (optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IPV6_ADD_MEMBERSHIP:
    case IPV6_DROP_MEMBERSHIP:
      if (optlen < sizeof(struct ip_mreq)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
#endif /* LWIP_IGMP */	  
      default:
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IPV6, UNIMPL: optname=0x%x, ..)\r\n",
                    s, optname));
        err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
/* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    if (optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }

    /* If this is no UDP lite socket, ignore any options. */
    if (!NETCONNTYPE_ISUDPLITE(netconn_type(sock->conn)))
      return 0;

    switch (optname) {
    case UDPLITE_SEND_CSCOV:
    case UDPLITE_RECV_CSCOV:
      break;

    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\r\n",
                  s, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP && LWIP_UDPLITE */
/* UNDEFINED LEVEL */
  default:
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\r\n",
                s, level, optname));
    err = ENOPROTOOPT;
  }  /* switch (level) */


  if (err != ERR_OK) {
    sock_set_errno(sock, err);
    return -1;
  }


  /* Now do the actual option processing */
  data.sock = sock;
#ifdef LWIP_DEBUG
  data.s = s;
#endif /* LWIP_DEBUG */
  data.level = level;
  data.optname = optname;
  data.optval = (void*)optval;
  data.optlen = &optlen;
  data.err = err;
  tcpip_callback(lwip_setsockopt_internal, &data);
  sys_arch_sem_wait(&sock->conn->op_completed, 0);
  /* maybe lwip_setsockopt_internal has changed err */
  err = data.err;

  sock_set_errno(sock, err);
  return err ? -1 : 0;
}

static void
lwip_setsockopt_internal(void *arg)
{
  struct lwip_sock *sock;
#ifdef LWIP_DEBUG
  int s;
#endif /* LWIP_DEBUG */
  int level, optname;
  const void *optval;
  struct lwip_setgetsockopt_data *data;
  struct ifreq *ifr;

  LWIP_ASSERT("arg != NULL", arg != NULL);

  data = (struct lwip_setgetsockopt_data*)arg;
  sock = data->sock;
#ifdef LWIP_DEBUG
  s = data->s;
#endif /* LWIP_DEBUG */
  level = data->level;
  optname = data->optname;
  optval = data->optval;

  switch (level) {

/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {

    /* The option flags */
    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_KEEPALIVE:
    /* UNIMPL case SO_OOBINCLUDE: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    /* UNIMPL case SO_USELOOPBACK: */
      if (*(int*)optval) {
        ip_set_option(sock->conn->pcb.ip, optname);
      } else {
        ip_reset_option(sock->conn->pcb.ip, optname);
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x, ..) -> %s\r\n",
                  s, optname, (*(int*)optval?"on":"off")));
      break;
#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
      netconn_set_sendtimeout(sock->conn, (s32_t)*(int*)optval);
      break;
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
      netconn_set_recvtimeout(sock->conn, *(int*)optval);
      break;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
      netconn_set_recvbufsize(sock->conn, *(int*)optval);
      break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_UDP
    case SO_NO_CHECK:
      if (*(int*)optval) {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_NOCHKSUM);
      } else {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_NOCHKSUM);
      }
      break;
#endif /* LWIP_UDP */
    case SO_BINDTODEVICE:
      ifr = (struct ifreq *)optval;
      data->err = sockbinddevice(sock->conn, ifr->ifr_name);
      break;
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    case IP_TTL:
      sock->conn->pcb.ip->ttl = (u8_t)(*(int*)optval);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, IP_TTL, ..) -> %d\r\n",
                  s, sock->conn->pcb.ip->ttl));
      break;
    case IP_TOS:
      sock->conn->pcb.ip->tos = (u8_t)(*(int*)optval);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, IP_TOS, ..)-> %d\r\n",
                  s, sock->conn->pcb.ip->tos));
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      sock->conn->pcb.udp->ttl = (u8_t)(*(u8_t*)optval);
      break;
    case IP_MULTICAST_IF:
      inet_addr_to_ipaddr(&sock->conn->pcb.udp->multicast_ip, (struct in_addr*)optval);
      break;
    case IP_MULTICAST_LOOP:
      if (*(u8_t*)optval) {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_MULTICAST_LOOP);
      } else {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_MULTICAST_LOOP);
      }
      break;
    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
      {
        /* If this is a TCP or a RAW socket, ignore these options. */
        struct ip_mreq *imr = (struct ip_mreq *)optval;
        ip_addr_t if_addr;
        ip_addr_t multi_addr;
        inet_addr_to_ipaddr(&if_addr, &imr->imr_interface);
        inet_addr_to_ipaddr(&multi_addr, &imr->imr_multiaddr);
        if(optname == IP_ADD_MEMBERSHIP){
          data->err = igmp_joingroup(&if_addr, &multi_addr);
        } else {
          data->err = igmp_leavegroup(&if_addr, &multi_addr);
        }
        if(data->err != ERR_OK) {
          data->err = EADDRNOTAVAIL;
        }
      }
      break;
#endif /* LWIP_IGMP */
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    switch (optname) {
    case TCP_NODELAY:
      if (*(int*)optval) {
        tcp_nagle_disable(sock->conn->pcb.tcp);
      } else {
        tcp_nagle_enable(sock->conn->pcb.tcp);
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_NODELAY) -> %s\r\n",
                  s, (*(int *)optval)?"on":"off") );
      break;
    case TCP_KEEPALIVE:
      sock->conn->pcb.tcp->keep_idle = (u32_t)(*(int*)optval);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPALIVE) -> %"U32_F"\r\n",
                  s, sock->conn->pcb.tcp->keep_idle));
      break;

#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
      sock->conn->pcb.tcp->keep_idle = 1000*(u32_t)(*(int*)optval);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPIDLE) -> %"U32_F"\r\n",
                  s, sock->conn->pcb.tcp->keep_idle));
      break;
    case TCP_KEEPINTVL:
      sock->conn->pcb.tcp->keep_intvl = 1000*(u32_t)(*(int*)optval);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPINTVL) -> %"U32_F"\r\n",
                  s, sock->conn->pcb.tcp->keep_intvl));
      break;
    case TCP_KEEPCNT:
      sock->conn->pcb.tcp->keep_cnt = (u32_t)(*(int*)optval);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPCNT) -> %"U32_F"\r\n",
                  s, sock->conn->pcb.tcp->keep_cnt));
      break;
#endif /* LWIP_TCP_KEEPALIVE */
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP*/

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      if (*(int*)optval) {
        sock->conn->flags |= NETCONN_FLAG_IPV6_V6ONLY;
      } else {
        sock->conn->flags &= ~NETCONN_FLAG_IPV6_V6ONLY;
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IPV6, IPV6_V6ONLY, ..) -> %d\r\n",
                  s, ((sock->conn->flags & NETCONN_FLAG_IPV6_V6ONLY) ? 1 : 0)));
      break;
#if LWIP_IPV6_MLD
    case IPV6_MULTICAST_TTL:
      LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, data->optlen, u8_t, NETCONN_UDP);
      sock->conn->pcb.udp->mcast_ttl = (u8_t)(*(const u8_t*)optval);
      break;
    case IPV6_MULTICAST_IF:
      {
        ip6_addr_t if_addr;
        LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, data->optlen, struct in6_addr, NETCONN_UDP);
        inet6_addr_to_ip6addr(&if_addr, (const struct in6_addr*)optval);
        udp_set_multicast_netif_addr6(sock->conn->pcb.udp, if_addr);
      }
      break;
    case IPV6_MULTICAST_LOOP:
      LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, data->optlen, u8_t, NETCONN_UDP);
      if (*(const u8_t*)optval) {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_MULTICAST_LOOP);
      } else {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_MULTICAST_LOOP);
      }
      break;

    case IPV6_ADD_MEMBERSHIP:
    case IPV6_DROP_MEMBERSHIP:
      {
        /* If this is a TCP or a RAW socket, ignore these options. */
        /* @todo: assign membership to this socket so that it is dropped when closing the socket */
        err_t igmp_err;
        const struct ipv6_mreq *imr = (const struct ipv6_mreq *)optval;
        ip6_addr_t if_addr;
        ip6_addr_t multi_addr;
        LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, data->optlen, struct ipv6_mreq, NETCONN_UDP);
        inet6_addr_to_ip6addr(&if_addr, &imr->ipv6mr_interface);
        inet6_addr_to_ip6addr(&multi_addr, &imr->ipv6mr_multiaddr);
        if (optname == IPV6_ADD_MEMBERSHIP) {
          if (!lwip_socket_register_membership(s, SOCK_TYPE_V6, &if_addr, &multi_addr)) {
            /* cannot track membership (out of memory) */
            data->err = ENOMEM;
            igmp_err = ERR_OK;
          } else {
            igmp_err = mld6_joingroup(&if_addr, &multi_addr);
          }
        } else {
          igmp_err = mld6_leavegroup(&if_addr, &multi_addr);
          lwip_socket_unregister_membership(s, SOCK_TYPE_V6, &if_addr, &multi_addr);
        }
        if (igmp_err != ERR_OK) {
          data->err = EADDRNOTAVAIL;
        }
      }
      break;
#endif /* LWIP_IPV6_MLD */   
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
  /* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    switch (optname) {
    case UDPLITE_SEND_CSCOV:
      if ((*(int*)optval != 0) && ((*(int*)optval < 8) || (*(int*)optval > 0xffff))) {
        /* don't allow illegal values! */
        sock->conn->pcb.udp->chksum_len_tx = 8;
      } else {
        sock->conn->pcb.udp->chksum_len_tx = (u16_t)*(int*)optval;
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) -> %d\r\n",
                  s, (*(int*)optval)) );
      break;
    case UDPLITE_RECV_CSCOV:
      if ((*(int*)optval != 0) && ((*(int*)optval < 8) || (*(int*)optval > 0xffff))) {
        /* don't allow illegal values! */
        sock->conn->pcb.udp->chksum_len_rx = 8;
      } else {
        sock->conn->pcb.udp->chksum_len_rx = (u16_t)*(int*)optval;
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) -> %d\r\n",
                  s, (*(int*)optval)) );
      break;
    default:
      LWIP_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP */
  default:
    LWIP_ASSERT("unhandled level", 0);
    break;
  }  /* switch (level) */
  sys_sem_signal(&sock->conn->op_completed);
}

int
lwip_ioctl(int s, long cmd, void *argp)
{
  struct lwip_sock *sock = get_socket(s);
  u8_t val;
#if LWIP_SO_RCVBUF
  u16_t buflen = 0;
  s16_t recv_avail;
#endif /* LWIP_SO_RCVBUF */

  if (!sock) {
    return -1;
  }

  switch (cmd) {
#if LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE
  case FIONREAD:
    if (!argp) {
      sock_set_errno(sock, EINVAL);
      return -1;
    }
#if LWIP_FIONREAD_LINUXMODE
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      struct pbuf *p;
      if (sock->lastdata) {
        p = ((struct netbuf *)sock->lastdata)->p;
      } else {
        struct netbuf *rxbuf;
        err_t err;
        if (sock->rcvevent <= 0) {
          *((u16_t*)argp) = 0;
        } else {
          err = netconn_recv(sock->conn, &rxbuf);
          if (err != ERR_OK) {
            *((u16_t*)argp) = 0;
          } else {
            sock->lastdata = rxbuf;
            *((u16_t*)argp) = rxbuf->p->tot_len;
          }
        }
      }
      return 0;
    }
#endif /* LWIP_FIONREAD_LINUXMODE */

#if LWIP_SO_RCVBUF
    /* we come here if either LWIP_FIONREAD_LINUXMODE==0 or this is a TCP socket */
    SYS_ARCH_GET(sock->conn->recv_avail, recv_avail);
    if (recv_avail < 0) {
      recv_avail = 0;
    }
    *((u16_t*)argp) = (u16_t)recv_avail;

    /* Check if there is data left from the last recv operation. /maq 041215 */
    if (sock->lastdata) {
      struct pbuf *p = (struct pbuf *)sock->lastdata;
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
        p = ((struct netbuf *)p)->p;
      }
      buflen = p->tot_len;
      buflen -= sock->lastoffset;

      *((u16_t*)argp) += buflen;
    }

    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(%d, FIONREAD, %p) = %"U16_F"\r\n", s, argp, *((u16_t*)argp)));
    sock_set_errno(sock, 0);
    return 0;
#else /* LWIP_SO_RCVBUF */
    break;
#endif /* LWIP_SO_RCVBUF */
#endif /* LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE */

  case FIONBIO:
    val = 0;
    if (argp && *(u32_t*)argp) {
      val = 1;
    }
    netconn_set_nonblocking(sock->conn, val);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(%d, FIONBIO, %d)\r\n", s, val));
    sock_set_errno(sock, 0);
    return 0;

  default:
    break;
  } /* switch (cmd) */
  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(%d, UNIMPL: 0x%lx, %p)\r\n", s, cmd, argp));
  sock_set_errno(sock, ENOSYS); /* not yet implemented */
  return -1;
}

/** A minimal implementation of fcntl.
 * Currently only the commands F_GETFL and F_SETFL are implemented.
 * Only the flag O_NONBLOCK is implemented.
 */
int
lwip_fcntl(int s, int cmd, int val)
{
  struct lwip_sock *sock = get_socket(s);
  int ret = -1;

  if (!sock || !sock->conn) {
    return -1;
  }

  switch (cmd) {
  case F_GETFL:
    ret = netconn_is_nonblocking(sock->conn) ? O_NONBLOCK : 0;
    break;
  case F_SETFL:
    if ((val & ~O_NONBLOCK) == 0) {
      /* only O_NONBLOCK, all other bits are zero */
      netconn_set_nonblocking(sock->conn, val & O_NONBLOCK);
      ret = 0;
    }
    break;
  default:
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_fcntl(%d, UNIMPL: %d, %d)\r\n", s, cmd, val));
    break;
  }
  return ret;
}


#if LWIP_IGMP
/** Register a new IGMP membership. On socket close, the membership is dropped automatically.
 *
 * ATTENTION: this function is called from tcpip_thread (or under CORE_LOCK).
 *
 * @return 1 on success, 0 on failure
 */
static int
lwip_socket_register_membership(int s, int type, void *if_addr, void *multi_addr)
{
  /* s+1 is stored in the array to prevent having to initialize the array
     (default initialization is to 0) */
  int sa = s + 1;
  int i;

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    if (socket_multicast_memberships[i].sa == 0) {
      socket_multicast_memberships[i].sa = sa;  
      if (type == SOCK_TYPE_V4) {
        socket_multicast_memberships[i].type = SOCK_TYPE_V4;
        ip4_addr_copy(socket_multicast_memberships[i].if_addr, *((const ip4_addr_t *)if_addr));
        ip4_addr_copy(socket_multicast_memberships[i].multi_addr, *((const ip4_addr_t *)multi_addr));
      }
#if LWIP_IPV6_MLD
      else if (type == SOCK_TYPE_V6) {
        socket_multicast_memberships[i].type = SOCK_TYPE_V6;
        ip6_addr_copy(socket_multicast_memberships[i].if6_addr, *((const ip6_addr_t *)if_addr));
        ip6_addr_copy(socket_multicast_memberships[i].multi6_addr, *((const ip6_addr_t *)multi_addr));
      }
#endif     
      return 1;
    }
  }
  return 0;
}

/** Unregister a previously registered membership. This prevents dropping the membership
 * on socket close.
 *
 * ATTENTION: this function is called from tcpip_thread (or under CORE_LOCK).
 */
static void
lwip_socket_unregister_membership(int s, int type, void *if_addr, void *multi_addr)
{
  /* s+1 is stored in the array to prevent having to initialize the array
     (default initialization is to 0) */
  int sa = s + 1;
  int i;

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    if (socket_multicast_memberships[i].sa == sa) {
      if (socket_multicast_memberships[i].type == SOCK_TYPE_V4 &&
          ip_addr_cmp(&socket_multicast_memberships[i].if_addr, (const ip4_addr_t *)if_addr) &&
          ip_addr_cmp(&socket_multicast_memberships[i].multi_addr, (const ip4_addr_t *)multi_addr)) {
        socket_multicast_memberships[i].sa = 0;
        socket_multicast_memberships[i].type = 0;
        ip_addr_set_zero(&socket_multicast_memberships[i].if_addr);
        ip_addr_set_zero(&socket_multicast_memberships[i].multi_addr);
      return;
    }
#if LWIP_IPV6_MLD
      else if (socket_multicast_memberships[i].type == SOCK_TYPE_V6 &&
              ip6_addr_cmp(&socket_multicast_memberships[i].if6_addr, (const ip6_addr_t *)if_addr) &&
              ip6_addr_cmp(&socket_multicast_memberships[i].multi6_addr, (const ip6_addr_t *)multi_addr)) {
        socket_multicast_memberships[i].sa = 0;
        socket_multicast_memberships[i].type = 0;
        ip6_addr_set_zero(&socket_multicast_memberships[i].if6_addr);
        ip6_addr_set_zero(&socket_multicast_memberships[i].multi6_addr);
        return;
      }
#endif
    }    
  }
}

/** Drop all memberships of a socket that were not dropped explicitly via setsockopt.
 *
 * ATTENTION: this function is NOT called from tcpip_thread (or under CORE_LOCK).
 */
static void lwip_socket_drop_registered_memberships(int s)
{
  /* s+1 is stored in the array to prevent having to initialize the array
     (default initialization is to 0) */
  int sa = s + 1;
  int i;
  ip_addr_t multi_addr, if_addr;

  LWIP_ASSERT("socket has no netconn", sockets[s].conn != NULL);

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    if (socket_multicast_memberships[i].sa == sa) {
      if (socket_multicast_memberships[i].type == SOCK_TYPE_V4) {
        ip_addr_copy_from_ip4(multi_addr, socket_multicast_memberships[i].multi_addr);
        ip_addr_copy_from_ip4(if_addr, socket_multicast_memberships[i].if_addr);
        socket_multicast_memberships[i].sa = 0;
        socket_multicast_memberships[i].type = 0;
        ip4_addr_set_zero(&socket_multicast_memberships[i].if_addr);
        ip4_addr_set_zero(&socket_multicast_memberships[i].multi_addr);
      }
#if LWIP_IPV6_MLD
      else if (socket_multicast_memberships[i].type == SOCK_TYPE_V6) {
        ip_addr_copy_from_ip6(multi_addr, socket_multicast_memberships[i].multi6_addr);
        ip_addr_copy_from_ip6(if_addr, socket_multicast_memberships[i].if6_addr);
        socket_multicast_memberships[i].sa = 0;
        socket_multicast_memberships[i].type = 0;
        ip6_addr_set_zero(&socket_multicast_memberships[i].if6_addr);
        ip6_addr_set_zero(&socket_multicast_memberships[i].multi6_addr);
      }
#endif
      netconn_join_leave_group(sockets[s].conn, &multi_addr, &if_addr, NETCONN_LEAVE);
    }
  }
}
#endif /* LWIP_IGMP */


int lwip_recvfrom_ext(int s, void *mem, size_t len, int flags,
              struct sockaddr *from, socklen_t *fromlen, struct sockaddr *dest, socklen_t *destlen)
{
  struct lwip_sock *sock;
  void             *buf = NULL;
  struct pbuf      *p;
  u16_t            buflen, copylen;
  int              off = 0;
  u8_t             done = 0;
  err_t            err;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d, %p, %"SZT_F", 0x%x, ..)\n", s, mem, len, flags));
  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  do {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: top while sock->lastdata=%p\n", sock->lastdata));
    /* Check if there is data left from the last recv operation. */
    if (sock->lastdata) {
      buf = sock->lastdata;
    } else {
      /* If this is non-blocking call, then check first */
      if (((flags & MSG_DONTWAIT) || netconn_is_nonblocking(sock->conn)) &&
          (sock->rcvevent <= 0)) {
        if (off > 0) {
          /* update receive window */
          netconn_recved(sock->conn, (u32_t)off);
          /* already received data, return that */
          sock_set_errno(sock, 0);
          return off;
        }
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): returning EWOULDBLOCK\n", s));
        sock_set_errno(sock, EWOULDBLOCK);
        return -1;
      }

      /* No data was left from the previous operation, so we try to get
         some from the network. */
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
        err = netconn_recv_tcp_pbuf(sock->conn, (struct pbuf **)&buf);
      } else {
        err = netconn_recv(sock->conn, (struct netbuf **)&buf);
      }
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: netconn_recv err=%d, netbuf=%p\n",
        err, buf));

      if (err != ERR_OK) {
        if (off > 0) {
          /* update receive window */
          netconn_recved(sock->conn, (u32_t)off);
          if (err == ERR_CLSD) {
            /* closed but already received data, ensure select gets the FIN, too */
            event_callback(sock->conn, NETCONN_EVT_RCVPLUS, 0);
          }
          /* already received data, return that */
          sock_set_errno(sock, 0);
          return off;
        }
        /* We should really do some error checking here. */
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): buf == NULL, error is \"%s\"!\n",
          s, lwip_strerr(err)));
        sock_set_errno(sock, err_to_errno(err));
        if (err == ERR_CLSD) {
          return 0;
        } else {
          return -1;
        }
      }
      LWIP_ASSERT("buf != NULL", buf != NULL);
      sock->lastdata = buf;
    }

    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
      p = (struct pbuf *)buf;
    } else {
      p = ((struct netbuf *)buf)->p;
    }
    buflen = p->tot_len;
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: buflen=%"U16_F" len=%"SZT_F" off=%d sock->lastoffset=%"U16_F"\n",
      buflen, len, off, sock->lastoffset));

    buflen -= sock->lastoffset;

    if (len > buflen) {
      copylen = buflen;
    } else {
      copylen = (u16_t)len;
    }

    /* copy the contents of the received buffer into
    the supplied memory pointer mem */
    pbuf_copy_partial(p, (u8_t*)mem + off, copylen, sock->lastoffset);

    off += copylen;

    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
      LWIP_ASSERT("invalid copylen, len would underflow", len >= copylen);
      len -= copylen;
      if ( (len <= 0) ||
           (p->flags & PBUF_FLAG_PUSH) ||
           (sock->rcvevent <= 0) ||
           ((flags & MSG_PEEK)!=0)) {
        done = 1;
      }
    } else {
      done = 1;
    }

    /* Check to see from where the data was.*/
    if (done) {
#if !SOCKETS_DEBUG
      if (from && fromlen)
#endif /* !SOCKETS_DEBUG */
      {
        u16_t port;
        //ip_addr_t tmpaddr;
        //ip_addr_t *fromaddr;
        ipX_addr_t tmpaddr;
        ipX_addr_t *fromaddr;
        union sockaddr_aligned saddr;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): addr=", s));
        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
          fromaddr = &tmpaddr;
          netconn_getaddr(sock->conn, fromaddr, &port, 0);
        } else {
          port = netbuf_fromport((struct netbuf *)buf);
          fromaddr = netbuf_fromaddr((struct netbuf *)buf);
        }
        //IPADDR_PORT_TO_SOCKADDR(&saddr, fromaddr, port);
		IPXADDR_PORT_TO_SOCKADDR(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),&saddr, fromaddr, port);
        ip_addr_debug_print(SOCKETS_DEBUG, fromaddr);
        LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F" len=%d\n", port, off));
#if SOCKETS_DEBUG
        if (from && fromlen)
#endif /* SOCKETS_DEBUG */
        {
          if (*fromlen > saddr.sa.sa_len) {
            *fromlen = saddr.sa.sa_len;
          }
          MEMCPY(from, &saddr, *fromlen);
        }
      }
    }

    /* Check to see the packet destination.*/
#if LWIP_NETBUF_RECVINFO
    if (done) {
#if !SOCKETS_DEBUG
      if (dest && destlen)
#endif /* !SOCKETS_DEBUG */
      {
        u16_t port;
        //ip_addr_t *destaddr;
	ipX_addr_t *destaddr;
        union sockaddr_aligned saddr;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom_ext(%d): addr=", s));
        port = netbuf_destport((struct netbuf *)buf);
				//lifang for KEIL
        destaddr = (ip_addr_t *)netbuf_destaddr((struct netbuf *)buf);
        //IPADDR_PORT_TO_SOCKADDR(&saddr, destaddr, port);
		IPXADDR_PORT_TO_SOCKADDR(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),&saddr, destaddr, port);
        ip_addr_debug_print(SOCKETS_DEBUG, destaddr);
        LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F" len=%d\n", port, off));
#if SOCKETS_DEBUG
        if (dest && destlen)
#endif /* SOCKETS_DEBUG */
        {
          if (*destlen > saddr.sa.sa_len) {
            *destlen = saddr.sa.sa_len;
          }
          MEMCPY(dest, &saddr, *destlen);
        }
      }
    }
#endif
    /* If we don't peek the incoming message... */
    if ((flags & MSG_PEEK) == 0) {
      /* If this is a TCP socket, check if there is data left in the
         buffer. If so, it should be saved in the sock structure for next
         time around. */
      if ((NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) && (buflen - copylen > 0)) {
        sock->lastdata = buf;
        sock->lastoffset += copylen;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: lastdata now netbuf=%p\n", buf));
      } else {
        sock->lastdata = NULL;
        sock->lastoffset = 0;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: deleting netbuf=%p\n", buf));
        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
          pbuf_free((struct pbuf *)buf);
        } else {
          netbuf_delete((struct netbuf *)buf);
        }
      }
    }
  } while (!done);

  if ((off > 0) && (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP)) {
    /* update receive window */
    netconn_recved(sock->conn, (u32_t)off);
  }
  sock_set_errno(sock, 0);
  return off;
}

#endif /* LWIP_SOCKET */
