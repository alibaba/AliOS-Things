/**
 * @file
 * Socket API (to be used from non-TCPIP threads)
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
 */


#ifndef LWIP_HDR_SOCKETS_H
#define LWIP_HDR_SOCKETS_H

#include "lwip/opt.h"

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include <stddef.h> /* for size_t */

#ifndef LWIP_PRIVATE_FD_SET
#include <sys/time.h>
#endif

#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/inet.h"
#include "lwip/errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/* If your port already typedef's sa_family_t, define SA_FAMILY_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(sa_family_t) && !defined(SA_FAMILY_T_DEFINED)
typedef u8_t sa_family_t;
#endif
/* If your port already typedef's in_port_t, define IN_PORT_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(in_port_t) && !defined(IN_PORT_T_DEFINED)
typedef u16_t in_port_t;
#endif

#if LWIP_IPV4
/* members are in network byte order */
struct sockaddr_in {
  u8_t            sin_len;
  sa_family_t     sin_family;
  in_port_t       sin_port;
  struct in_addr  sin_addr;
#define SIN_ZERO_LEN 8
  char            sin_zero[SIN_ZERO_LEN];
};
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
struct sockaddr_in6 {
  u8_t            sin6_len;      /* length of this structure    */
  sa_family_t     sin6_family;   /* AF_INET6                    */
  in_port_t       sin6_port;     /* Transport layer port #      */
  u32_t           sin6_flowinfo; /* IPv6 flow information       */
  struct in6_addr sin6_addr;     /* IPv6 address                */
  u32_t           sin6_scope_id; /* Set of interfaces for scope */
};
#endif /* LWIP_IPV6 */

struct sockaddr {
  u8_t        sa_len;
  sa_family_t sa_family;
#if LWIP_PACKET
/* increase sa_data length from 14 to 26(same size with in6)*/
  char        sa_data[26];
#else
  char        sa_data[14];
#endif
};

struct sockaddr_storage {
  u8_t        s2_len;
  sa_family_t ss_family;
  char        s2_data1[2];
  u32_t       s2_data2[3];
#if LWIP_IPV6
  u32_t       s2_data3[3];
#endif /* LWIP_IPV6 */
};

/* If your port already typedef's socklen_t, define SOCKLEN_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(socklen_t) && !defined(SOCKLEN_T_DEFINED)
typedef u32_t socklen_t;
#endif

struct lwip_sock;

#if !LWIP_TCPIP_CORE_LOCKING
/** Maximum optlen used by setsockopt/getsockopt */
#define LWIP_SETGETSOCKOPT_MAXOPTLEN 16

/** This struct is used to pass data to the set/getsockopt_internal
 * functions running in tcpip_thread context (only a void* is allowed) */
struct lwip_setgetsockopt_data {
  /** socket index for which to change options */
  int s;
  /** level of the option to process */
  int level;
  /** name of the option to process */
  int optname;
  /** set: value to set the option to
    * get: value of the option is stored here */
#if LWIP_MPU_COMPATIBLE
  u8_t optval[LWIP_SETGETSOCKOPT_MAXOPTLEN];
#else
  union {
     void *p;
     const void *pc;
  } optval;
#endif
  /** size of *optval */
  socklen_t optlen;
  /** if an error occurs, it is temporarily stored here */
  err_t err;
  /** semaphore to wake up the calling task */
  void* completed_sem;
};
#endif /* !LWIP_TCPIP_CORE_LOCKING */

#if !defined(iovec)
struct iovec {
  void  *iov_base;
  size_t iov_len;
};
#endif

struct msghdr {
  void         *msg_name;
  socklen_t     msg_namelen;
  struct iovec *msg_iov;
  int           msg_iovlen;
  void         *msg_control;
  socklen_t     msg_controllen;
  int           msg_flags;
};

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

/*
 * Option flags per-socket. These must match the SOF_ flags in ip.h (checked in init.c)
 */
#define SO_REUSEADDR   0x0004 /* Allow local address reuse */
#define SO_KEEPALIVE   0x0008 /* keep connections alive */
#define SO_BROADCAST   0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */
#define SO_TCPSACK     0x0040 /* Allow TCP SACK (Selective acknowledgment) */

/*
 * Additional options, not kept in so_options.
 */
#define SO_DEBUG       0x0001 /* Unimplemented: turn on debugging info recording */
#define SO_ACCEPTCONN  0x0002 /* socket has had listen() */
#define SO_DONTROUTE   0x0010 /* Unimplemented: just use interface addresses */
#define SO_USELOOPBACK 0x0080 /* Unimplemented: bypass hardware when possible */
#define SO_LINGER      0x0100 /* linger on close if data present */
#define SO_DONTLINGER  ((int)(~SO_LINGER))
#define SO_OOBINLINE   0x0200 /* Unimplemented: leave received OOB data in line */
#define SO_REUSEPORT   0x0400 /* Unimplemented: allow local address & port reuse */
#define SO_SNDBUF      0x1001 /* Unimplemented: send buffer size */
#define SO_RCVBUF      0x1002 /* receive buffer size */
#define SO_SNDLOWAT    0x1003 /* Unimplemented: send low-water mark */
#define SO_RCVLOWAT    0x1004 /* Unimplemented: receive low-water mark */
#define SO_SNDTIMEO    0x1005 /* send timeout */
#define SO_RCVTIMEO    0x1006 /* receive timeout */
#define SO_ERROR       0x1007 /* get error status and clear */
#define SO_TYPE        0x1008 /* get socket type */
#define SO_CONTIMEO    0x1009 /* Unimplemented: connect timeout */
#define SO_NO_CHECK    0x100a /* don't create UDP checksum */
#define SO_BIO         0x100b /* set socket into blocking mode */
#define SO_NONBLOCK    0x100c /* set/get blocking mode via optval param */
#define SO_NBIO        0x100d /* set socket into NON-blocking mode */

/*
 * Structure used for manipulating linger option.
 */
struct linger {
       int l_onoff;                /* option on/off */
       int l_linger;               /* linger time in seconds */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define  SOL_SOCKET  0xfff    /* options for socket level */
#define  SOL_PACKET  263      /* options for packet level */

#define AF_UNSPEC       0
#define AF_INET         2
#if LWIP_IPV6
#define AF_INET6        10
#else /* LWIP_IPV6 */
#define AF_INET6        AF_UNSPEC
#endif /* LWIP_IPV6 */
#define AF_PACKET       17
#define PF_INET         AF_INET
#define PF_INET6        AF_INET6
#define PF_UNSPEC       AF_UNSPEC
#define PF_PACKET       AF_PACKET

#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#if LWIP_IPV6
#define IPPROTO_IPV6    41
#define IPPROTO_ICMPV6  58
#endif /* LWIP_IPV6 */
#define IPPROTO_UDPLITE 136
#define IPPROTO_RAW     255

/* Flags we can use with send and recv. */
#define MSG_PEEK       0x01    /* Peeks at an incoming message */
#define MSG_WAITALL    0x02    /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB        0x04    /* Unimplemented: Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */


/*
 * Options for level IPPROTO_IP
 */
#define IP_TOS             1
#define IP_TTL             2

#if LWIP_TCP
/*
 * Options for level IPPROTO_TCP
 */
#define TCP_NODELAY    0x01    /* don't delay send to coalesce packets */
#define TCP_KEEPALIVE  0x02    /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPIDLE   0x03    /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPINTVL  0x04    /* set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPCNT    0x05    /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */
#endif /* LWIP_TCP */

#if LWIP_IPV6
/*
 * Options for level IPPROTO_IPV6
 */
#define IPV6_CHECKSUM       7  /* RFC3542: calculate and insert the ICMPv6 checksum for raw sockets. */
#define IPV6_V6ONLY         27 /* RFC3493: boolean control to restrict AF_INET6 sockets to IPv6 communications only. */
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
/*
 * Options for level IPPROTO_UDPLITE
 */
#define UDPLITE_SEND_CSCOV 0x01 /* sender checksum coverage */
#define UDPLITE_RECV_CSCOV 0x02 /* minimal receiver checksum coverage */
#endif /* LWIP_UDP && LWIP_UDPLITE*/


#if LWIP_MULTICAST_TX_OPTIONS
/*
 * Options and types for UDP multicast traffic handling
 */
#define IP_MULTICAST_TTL   5
#define IP_MULTICAST_IF    6
#define IP_MULTICAST_LOOP  7
#endif /* LWIP_MULTICAST_TX_OPTIONS */

#if LWIP_IGMP
/*
 * Options and types related to multicast membership
 */
#define IP_ADD_MEMBERSHIP  3
#define IP_DROP_MEMBERSHIP 4

typedef struct ip_mreq {
    struct in_addr imr_multiaddr; /* IP multicast address of group */
    struct in_addr imr_interface; /* local IP address of interface */
} ip_mreq;
#endif /* LWIP_IGMP */

/*
 * The Type of Service provides an indication of the abstract
 * parameters of the quality of service desired.  These parameters are
 * to be used to guide the selection of the actual service parameters
 * when transmitting a datagram through a particular network.  Several
 * networks offer service precedence, which somehow treats high
 * precedence traffic as more important than other traffic (generally
 * by accepting only traffic above a certain precedence at time of high
 * load).  The major choice is a three way tradeoff between low-delay,
 * high-reliability, and high-throughput.
 * The use of the Delay, Throughput, and Reliability indications may
 * increase the cost (in some sense) of the service.  In many networks
 * better performance for one of these parameters is coupled with worse
 * performance on another.  Except for very unusual cases at most two
 * of these three indications should be set.
 */
#define IPTOS_TOS_MASK          0x1E
#define IPTOS_TOS(tos)          ((tos) & IPTOS_TOS_MASK)
#define IPTOS_LOWDELAY          0x10
#define IPTOS_THROUGHPUT        0x08
#define IPTOS_RELIABILITY       0x04
#define IPTOS_LOWCOST           0x02
#define IPTOS_MINCOST           IPTOS_LOWCOST

/*
 * The Network Control precedence designation is intended to be used
 * within a network only.  The actual use and control of that
 * designation is up to each network. The Internetwork Control
 * designation is intended for use by gateway control originators only.
 * If the actual use of these precedence designations is of concern to
 * a particular network, it is the responsibility of that network to
 * control the access to, and use of, those precedence designations.
 */
#define IPTOS_PREC_MASK                 0xe0
#define IPTOS_PREC(tos)                ((tos) & IPTOS_PREC_MASK)
#define IPTOS_PREC_NETCONTROL           0xe0
#define IPTOS_PREC_INTERNETCONTROL      0xc0
#define IPTOS_PREC_CRITIC_ECP           0xa0
#define IPTOS_PREC_FLASHOVERRIDE        0x80
#define IPTOS_PREC_FLASH                0x60
#define IPTOS_PREC_IMMEDIATE            0x40
#define IPTOS_PREC_PRIORITY             0x20
#define IPTOS_PREC_ROUTINE              0x00

#if LWIP_PACKET
//set if namesize = 16
#define IF_NAMESIZE     16

#ifndef IFNAMSIZ
#define IFNAMSIZ            IF_NAMESIZE
#endif

struct ifreq {
#define IFHWADDRLEN         6
    union {
        char                ifrn_name[IFNAMSIZ];                /* if name, e.g. "en1"                  */
    } ifr_ifrn;
    union {
        struct sockaddr     ifru_addr;
        struct sockaddr     ifru_dstaddr;
        struct sockaddr     ifru_broadaddr;
        struct sockaddr     ifru_netmask;
        struct sockaddr     ifru_hwaddr;
        short               ifru_flags;
        int                 ifru_ifindex;
        int                 ifru_mtu;
        int                 ifru_metric;
        int                 ifru_type;
        void               *ifru_data;
    } ifr_ifru;
};

#define ifr_name            ifr_ifrn.ifrn_name
#define ifr_addr            ifr_ifru.ifru_addr
#define ifr_dstaddr         ifr_ifru.ifru_dstaddr
#define ifr_netmask         ifr_ifru.ifru_netmask
#define ifr_broadaddr       ifr_ifru.ifru_broadaddr
#define ifr_hwaddr          ifr_ifru.ifru_hwaddr
#define ifr_flags           ifr_ifru.ifru_flags
#define ifr_ifindex         ifr_ifru.ifru_ifindex
#define ifr_mtu             ifr_ifru.ifru_mtu
#define ifr_metric          ifr_ifru.ifru_metric
#define ifr_type            ifr_ifru.ifru_type
#define ifr_data            ifr_ifru.ifru_data
#endif /* LWIP_PACKET */

/*
 * Commands for ioctlsocket(),  taken from the BSD file fcntl.h.
 * lwip_ioctl only supports FIONREAD and FIONBIO, for now
 *
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */
#if !defined(FIONREAD) || !defined(FIONBIO)
#define IOCPARM_MASK    0x7fU           /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000UL    /* no parameters */
#define IOC_OUT         0x40000000UL    /* copy out parameters */
#define IOC_IN          0x80000000UL    /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))

#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#endif /* !defined(FIONREAD) || !defined(FIONBIO) */

#if LWIP_PACKET

#define _IOC(inout,group,num,len) \
        (u_int32_t) ((u_int32_t)inout | \
         (u_int32_t) ((u_int32_t)((u_int32_t)len & IOCPARM_MASK) << 16) | \
         (u_int32_t)((group) << 8) | \
         (u_int32_t)(num))


#define _IOWR(g,n,t)    _IOC(IOC_INOUT, (g), (n), sizeof(t))
#endif /* LWIP_PACKET */

#ifndef FIONREAD
#define FIONREAD    _IOR('f', 127, unsigned long) /* get # bytes to read */
#endif
#ifndef FIONBIO
#define FIONBIO     _IOW('f', 126, unsigned long) /* set/clear non-blocking i/o */
#endif

/* Socket I/O Controls: unimplemented */
#ifndef SIOCSHIWAT
#define SIOCSHIWAT  _IOW('s',  0, unsigned long)  /* set high watermark */
#define SIOCGHIWAT  _IOR('s',  1, unsigned long)  /* get high watermark */
#define SIOCSLOWAT  _IOW('s',  2, unsigned long)  /* set low watermark */
#define SIOCGLOWAT  _IOR('s',  3, unsigned long)  /* get low watermark */
#define SIOCATMARK  _IOR('s',  7, unsigned long)  /* at oob mark? */
#endif

#if LWIP_PACKET
#define SIOCGSIZIFCONF      _IOR('i', 106, int)
#define SIOCGIFCONF         _IOWR('i', 20, struct ifconf)
#define SIOCGIFNUM          _IOR('i',  20, int)

#define SIOCSIFADDR         _IOW('i', 12, struct ifreq)
#define SIOCSIFNETMASK      _IOW('i', 22, struct ifreq)
#define SIOCSIFDSTADDR      _IOW('i', 14, struct ifreq)
#define SIOCSIFBRDADDR      _IOW('i', 19, struct ifreq)
#define SIOCSIFFLAGS        _IOW('i', 16, struct ifreq)

#define SIOCGIFADDR         _IOWR('i', 33, struct ifreq)
#define SIOCGIFNETMASK      _IOWR('i', 37, struct ifreq)
#define SIOCGIFDSTADDR      _IOWR('i', 34, struct ifreq)
#define SIOCGIFBRDADDR      _IOWR('i', 35, struct ifreq)
#define SIOCGIFFLAGS        _IOWR('i', 17, struct ifreq)

#define SIOCGIFMETRIC       _IOWR('i', 23, struct ifreq)
#define SIOCSIFMETRIC       _IOW( 'i', 24, struct ifreq)

#define SIOCGIFTYPE         _IOR('i',  49, struct ifreq)
#define SIOCGIFNAME         _IOWR('i', 50, struct ifreq)
#define SIOCGIFINDEX        _IOWR('i', 51, struct ifreq)

#define SIOCGIFMTU          _IOWR('i', 52, struct ifreq)
#define SIOCSIFMTU          _IOW('i',  53, struct ifreq)

#define SIOCGIFHWADDR       _IOWR('i', 54, struct ifreq)
#define SIOCSIFHWADDR       _IOW('i',  55, struct ifreq)

#define SIOCADDMULTI        _IOW('i', 60, struct ifreq)
#define SIOCDELMULTI        _IOW('i', 61, struct ifreq)
#endif /* LWIP_PACKET */

/* commands for fnctl */
#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif

/* File status flags and file access modes for fnctl,
   these are bits in an int. */
#ifndef O_NONBLOCK
#define O_NONBLOCK  1 /* nonblocking I/O */
#endif
#ifndef O_NDELAY
#define O_NDELAY    1 /* same as O_NONBLOCK, for compatibility */
#endif

#ifndef SHUT_RD
  #define SHUT_RD   0
  #define SHUT_WR   1
  #define SHUT_RDWR 2
#endif

/* FD_SET used for lwip_select */
#ifndef FD_SET
#undef  FD_SETSIZE
/* Make FD_SETSIZE match NUM_SOCKETS in socket.c */
#define FD_SETSIZE    MEMP_NUM_NETCONN
#define FDSETSAFESET(n, code) do { \
  if (((n) - LWIP_SOCKET_OFFSET < (MEMP_NUM_NETCONN * 2)) && (((int)(n) - LWIP_SOCKET_OFFSET) >= 0)) { \
  code; }} while(0)
#define FDSETSAFEGET(n, code) (((n) - LWIP_SOCKET_OFFSET < (MEMP_NUM_NETCONN * 2)) && (((int)(n) - LWIP_SOCKET_OFFSET) >= 0) ?\
  (code) : 0)
#define FD_SET(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] |=  (1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_CLR(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] &= ~(1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_ISSET(n,p) FDSETSAFEGET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] &   (1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_ZERO(p)    memset((void*)(p), 0, sizeof(*(p)))

typedef struct fd_set
{
  unsigned char fd_bits [(FD_SETSIZE * 2 + 7) / 8];
} fd_set;

#elif LWIP_SOCKET_OFFSET
#error LWIP_SOCKET_OFFSET does not work with external FD_SET!
#else
#include <fcntl.h>
#endif /* FD_SET */

/*****************************************************************
 *                                                               *
 *standard io offset    af packet fd offset                      *
 *   |    socket fd offset      |        event fd offset         *
 *   |         |                |             |                  *
 *   |=========|================|=============|================| *
 *    reserved    socket fd       af_packet fd      event fd     *
 *                                                               *
 *****************************************************************/

/* reserved socketd for posix standard file descriptors */
#define POSIX_EVB_SOCKET_RESERVED_START 0
#define POSIX_EVB_SOCKET_RESERVED_END 2
#define NUM_PACKET_SOCKETS  4 
#define LWIP_PACKET_SOCKET_OFFSET (FD_AOS_SOCKET_OFFSET + FD_AOS_NUM_SOCKETS)
#define FD_AOS_NUM_SOCKETS (MEMP_NUM_NETCONN-(POSIX_EVB_SOCKET_RESERVED_END-POSIX_EVB_SOCKET_RESERVED_START+1))
#define FD_AOS_NUM_EVENTS MEMP_NUM_NETCONN
#define FD_AOS_SOCKET_OFFSET (LWIP_SOCKET_OFFSET+(POSIX_EVB_SOCKET_RESERVED_END-POSIX_EVB_SOCKET_RESERVED_START+1))
#define FD_AOS_EVENT_OFFSET (FD_AOS_SOCKET_OFFSET + FD_AOS_NUM_SOCKETS + NUM_PACKET_SOCKETS)

/** LWIP_TIMEVAL_PRIVATE: if you want to use the struct timeval provided
 * by your system, set this to 0 and include <sys/time.h> in cc.h */
#ifndef LWIP_TIMEVAL_PRIVATE
#define LWIP_TIMEVAL_PRIVATE 1
#endif

#if LWIP_TIMEVAL_PRIVATE
struct timeval {
  long    tv_sec;         /* seconds */
  long    tv_usec;        /* and microseconds */
};
#endif /* LWIP_TIMEVAL_PRIVATE */

#define lwip_socket_init() /* Compatibility define, no init needed. */
void lwip_socket_thread_init(void); /* LWIP_NETCONN_SEM_PER_THREAD==1: initialize thread-local semaphore */
void lwip_socket_thread_cleanup(void); /* LWIP_NETCONN_SEM_PER_THREAD==1: destroy thread-local semaphore */

#if LWIP_COMPAT_SOCKETS == 2
/* This helps code parsers/code completion by not having the COMPAT functions as defines */
#define lwip_accept       accept
#define lwip_bind         bind
#define lwip_shutdown     shutdown
#define lwip_getpeername  getpeername
#define lwip_getsockname  getsockname
#define lwip_setsockopt   setsockopt
#define lwip_getsockopt   getsockopt
#define lwip_close        closesocket
#define lwip_connect      connect
#define lwip_listen       listen
#define lwip_recv         recv
#define lwip_recvfrom     recvfrom
#define lwip_send         send
#define lwip_sendmsg      sendmsg
#define lwip_sendto       sendto
#define lwip_socket       socket
#define lwip_select       select
#define lwip_ioctlsocket  ioctl

#ifndef POSIX_DEVICE_IO_NEED
#if LWIP_POSIX_SOCKETS_IO_NAMES
#define lwip_read         read
#define lwip_write        write
#define lwip_writev       writev
#define lwip_eventfd      eventfd
#undef lwip_close
#define lwip_close        close
#define closesocket(s)    close(s)
#define lwip_fcntl        fcntl
#define lwip_ioctl        ioctl
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */
#endif /* LWIP_COMPAT_SOCKETS == 2 */
#endif /* POSIX_DEVICE_IO_NEED */

int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_shutdown(int s, int how);
int lwip_getpeername (int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getsockname (int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
int lwip_close(int s);
int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_listen(int s, int backlog);
int lwip_recv(int s, void *mem, size_t len, int flags);
int lwip_read(int s, void *mem, size_t len);
int lwip_recvfrom(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen);
int lwip_send(int s, const void *dataptr, size_t size, int flags);
int lwip_sendmsg(int s, const struct msghdr *message, int flags);
int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen);
int lwip_socket(int domain, int type, int protocol);
int lwip_write(int s, const void *dataptr, size_t size);
int lwip_writev(int s, const struct iovec *iov, int iovcnt);
int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);
int lwip_ioctl(int s, long cmd, void *argp);
int lwip_fcntl(int s, int cmd, int val);
int lwip_eventfd(unsigned int initval, int flags);


#if LWIP_PACKET
int lwip_try_wakeup(int s, int rcvevent, int sendevent, int errevent);
#endif

#if LWIP_COMPAT_SOCKETS
#if LWIP_COMPAT_SOCKETS != 2

#ifdef POSIX_DEVICE_IO_NEED
static inline int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	return lwip_accept(s, addr, addrlen);
}

static inline int bind(int s, const struct sockaddr *name, socklen_t namelen)
{
	return lwip_bind(s, name, namelen);
}

static inline int shutdown(int s, int how)
{
	return lwip_shutdown(s, how);
}

static inline int getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
	return lwip_getpeername(s, name, namelen);
}
static inline int getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
	return lwip_getsockname(s, name, namelen);
}

static inline int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
	return lwip_getsockopt(s, level, optname, optval, optlen);
}

static inline int setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
{
	return lwip_setsockopt(s, level, optname, optval, optlen);
}

static inline int connect(int s, const struct sockaddr *name, socklen_t namelen)
{
	return lwip_connect(s, name, namelen);
}

static inline int listen(int s, int backlog)
{
	return lwip_listen(s, backlog);
}

static inline int recv(int s, void *mem, size_t len, int flags)
{
	return lwip_recv(s, mem, len, flags);
}

static inline int recvfrom(int s, void *mem, size_t len, int flags,struct sockaddr *from, socklen_t *fromlen)
{
	return lwip_recvfrom(s, mem, len, flags, from, fromlen);
}

static inline int send(int s, const void *dataptr, size_t size, int flags)
{
	return lwip_send(s, dataptr, size, flags);
}

static inline int sendmsg(int s, const struct msghdr *message, int flags)
{
	return lwip_sendmsg(s, message, flags);
}

static inline int sendto(int s, const void *dataptr, size_t size, int flags,	const struct sockaddr *to, socklen_t tolen)
{
	return lwip_sendto(s, dataptr, size, flags, to, tolen);
}

static inline int socket(int domain, int type, int protocol)
{
	return lwip_socket(domain, type, protocol);
}

static inline int writev(int s, const struct iovec *iov, int iovcnt)
{
	return lwip_writev(s, iov, iovcnt);
}

static inline int eventfd(unsigned int initval, int flags)
{
	return lwip_eventfd(initval, flags);
}

/** @ingroup socket */
#define select(maxfdp1,readset,writeset,exceptset,timeout)     lwip_select(maxfdp1,readset,writeset,exceptset,timeout)
/** @ingroup socket */
#define ioctlsocket(s,cmd,argp)                   lwip_ioctl(s,cmd,argp)

#else
/** @ingroup socket */
#define accept(s,addr,addrlen)                    lwip_accept(s,addr,addrlen)
/** @ingroup socket */
#define bind(s,name,namelen)                      lwip_bind(s,name,namelen)
/** @ingroup socket */
#define shutdown(s,how)                           lwip_shutdown(s,how)
/** @ingroup socket */
#define getpeername(s,name,namelen)               lwip_getpeername(s,name,namelen)
/** @ingroup socket */
#define getsockname(s,name,namelen)               lwip_getsockname(s,name,namelen)
/** @ingroup socket */
#define setsockopt(s,level,optname,opval,optlen)  lwip_setsockopt(s,level,optname,opval,optlen)
/** @ingroup socket */
#define getsockopt(s,level,optname,opval,optlen)  lwip_getsockopt(s,level,optname,opval,optlen)
/** @ingroup socket */
#define closesocket(s)                            lwip_close(s)
/** @ingroup socket */
#define connect(s,name,namelen)                   lwip_connect(s,name,namelen)
/** @ingroup socket */
#define listen(s,backlog)                         lwip_listen(s,backlog)
/** @ingroup socket */
#define recv(s,mem,len,flags)                     lwip_recv(s,mem,len,flags)
/** @ingroup socket */
#define recvfrom(s,mem,len,flags,from,fromlen)    lwip_recvfrom(s,mem,len,flags,from,fromlen)
/** @ingroup socket */
#define send(s,dataptr,size,flags)                lwip_send(s,dataptr,size,flags)
/** @ingroup socket */
#define sendmsg(s,message,flags)                  lwip_sendmsg(s,message,flags)
/** @ingroup socket */
#define sendto(s,dataptr,size,flags,to,tolen)     lwip_sendto(s,dataptr,size,flags,to,tolen)
/** @ingroup socket */
#define socket(domain,type,protocol)              lwip_socket(domain,type,protocol)
/** @ingroup socket */
#define select(maxfdp1,readset,writeset,exceptset,timeout)     lwip_select(maxfdp1,readset,writeset,exceptset,timeout)
/** @ingroup socket */
#define ioctlsocket(s,cmd,argp)                   lwip_ioctl(s,cmd,argp)
#if LWIP_POSIX_SOCKETS_IO_NAMES
/** @ingroup socket */
#define read(s,mem,len)                           lwip_read(s,mem,len)
/** @ingroup socket */
#define write(s,dataptr,len)                      lwip_write(s,dataptr,len)
/** @ingroup socket */
/** @ingroup socket */
#define ioctl(s,cmd,argp)                         lwip_ioctl(s,cmd,argp)
#define eventfd(v, f)                             lwip_eventfd(v, f)
#define writev(s,iov,iovcnt)                      lwip_writev(s,iov,iovcnt)
/** @ingroup socket */
#define close(s)                                  lwip_close(s)
/** @ingroup socket */
#define fcntl(s,cmd,val)                          lwip_fcntl(s,cmd,val)
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */
#endif /* POSIX_DEVICE_IO_NEED */
#endif /* LWIP_COMPAT_SOCKETS != 2 */

#if LWIP_IPV4 && LWIP_IPV6
/** @ingroup socket */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((const ip6_addr_t*)(src),(dst),(size)) \
     : (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL))
/** @ingroup socket */
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? ip6addr_aton((src),(ip6_addr_t*)(dst)) \
     : (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0))
#elif LWIP_IPV4 /* LWIP_IPV4 && LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0)
#else /* LWIP_IPV4 && LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((const ip6_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? ip6addr_aton((src),(ip6_addr_t*)(dst)) : 0)
#endif /* LWIP_IPV4 && LWIP_IPV6 */

#endif /* LWIP_COMPAT_SOCKETS */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SOCKET */

#endif /* LWIP_HDR_SOCKETS_H */
