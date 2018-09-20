/**
 * @file    wm_sockets2.0.3.h
 *
 * @brief   sockets2.0.3 apis
 *
 * @author  winnermicro
 *
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_SOCKET_API2_0_3_H
#define WM_SOCKET_API2_0_3_H

#include <stdio.h>
#include "wm_type_def.h"
#include "wm_config.h"

/** If your port already typedef's sa_family_t, define SA_FAMILY_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(sa_family_t) && !defined(SA_FAMILY_T_DEFINED)
typedef u8_t sa_family_t;
#endif
/** If your port already typedef's in_port_t, define IN_PORT_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(in_port_t) && !defined(IN_PORT_T_DEFINED)
typedef u16_t in_port_t;
#endif

/** If your port already typedef's in_addr_t, define IN_ADDR_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(in_addr_t) && !defined(IN_ADDR_T_DEFINED)
typedef u32_t in_addr_t;
#endif

struct in_addr {
  in_addr_t s_addr;
};

struct in6_addr {
  union {
    u32_t u32_addr[4];
    u8_t  u8_addr[16];
  } un;
#define s6_addr  un.u8_addr
};

/** 255.255.255.255 */
#define IPADDR_NONE         ((u32_t)0xffffffffUL)
/** 127.0.0.1 */
#define IPADDR_LOOPBACK     ((u32_t)0x7f000001UL)
/** 0.0.0.0 */
#define IPADDR_ANY          ((u32_t)0x00000000UL)
/** 255.255.255.255 */
#define IPADDR_BROADCAST    ((u32_t)0xffffffffUL)

/** 255.255.255.255 */
#define INADDR_NONE         IPADDR_NONE
/** 127.0.0.1 */
#define INADDR_LOOPBACK     IPADDR_LOOPBACK
/** 0.0.0.0 */
#define INADDR_ANY          IPADDR_ANY
/** 255.255.255.255 */
#define INADDR_BROADCAST    IPADDR_BROADCAST

/** This macro can be used to initialize a variable of type struct in6_addr
    to the IPv6 wildcard address. */
#define IN6ADDR_ANY_INIT {{{0,0,0,0}}}
/** This macro can be used to initialize a variable of type struct in6_addr
    to the IPv6 loopback address. */
#define IN6ADDR_LOOPBACK_INIT {{{0,0,0,PP_HTONL(1)}}}
/** This variable is initialized by the system to contain the wildcard IPv6 address. */
extern const struct in6_addr in6addr_any;

#if TLS_CONFIG_IPV4
/** members are in network byte order */
struct sockaddr_in {
  u8_t            sin_len;
  sa_family_t     sin_family;
  in_port_t       sin_port;
  struct in_addr  sin_addr;
#define SIN_ZERO_LEN 8
  char            sin_zero[SIN_ZERO_LEN];
};
#endif /* TLS_CONFIG_IPV4 */

#if TLS_CONFIG_IPV6
struct sockaddr_in6 {
  u8_t            sin6_len;      /* length of this structure    */
  sa_family_t     sin6_family;   /* AF_INET6                    */
  in_port_t       sin6_port;     /* Transport layer port #      */
  u32_t           sin6_flowinfo; /* IPv6 flow information       */
  struct in6_addr sin6_addr;     /* IPv6 address                */
  u32_t           sin6_scope_id; /* Set of interfaces for scope */
};
#endif /* TLS_CONFIG_IPV6 */

struct sockaddr {
  u8_t        sa_len;
  sa_family_t sa_family;
  char        sa_data[14];
};

struct sockaddr_storage {
  u8_t        s2_len;
  sa_family_t ss_family;
  char        s2_data1[2];
  u32_t       s2_data2[3];
#if TLS_CONFIG_IPV6
  u32_t       s2_data3[3];
#endif /* TLS_CONFIG_IPV6 */
};

struct hostent {
    char  *h_name;      /* Official name of the host. */
    char **h_aliases;   /* A pointer to an array of pointers to alternative host names,
                           terminated by a null pointer. */
    int    h_addrtype;  /* Address type. */
    int    h_length;    /* The length, in bytes, of the address. */
    char **h_addr_list; /* A pointer to an array of pointers to network addresses (in
                           network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
};

struct sockaddr_store {
  u8_t        s2_len;
  sa_family_t ss_family;
  char        s2_data1[2];
  u32_t       s2_data2[3];
#if TLS_CONFIG_IPV6
  u32_t       s2_data3[3];
#endif /* TLS_CONFIG_IPV6 */
};

/** If your port already typedef's socklen_t, define SOCKLEN_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(socklen_t) && !defined(SOCKLEN_T_DEFINED)
typedef u32_t socklen_t;
#endif

struct lwip_sock;

/** Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

/**
 * Option flags per-socket. These must match the SOF_ flags in ip.h (checked in init.c)
 */
#define SO_REUSEADDR   0x0004 /* Allow local address reuse */
#define SO_KEEPALIVE   0x0008 /* keep connections alive */
#define SO_BROADCAST   0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */


/**
 * Additional options, not kept in so_options.
 */
#define SO_DEBUG       0x0001 /* Unimplemented: turn on debugging info recording */
#define SO_ACCEPTCONN  0x0002 /* socket has had listen() */
#define SO_DONTROUTE   0x0010 /* Unimplemented: just use interface addresses */
#define SO_USELOOPBACK 0x0040 /* Unimplemented: bypass hardware when possible */
#define SO_LINGER      0x0080 /* linger on close if data present */
#define SO_DONTLINGER  ((int)(~SO_LINGER))
#define SO_OOBINLINE   0x0100 /* Unimplemented: leave received OOB data in line */
#define SO_REUSEPORT   0x0200 /* Unimplemented: allow local address & port reuse */
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

/**
 * Structure used for manipulating linger option.
 */
struct linger {
       int l_onoff;                /* option on/off */
       int l_linger;               /* linger time in seconds */
};

/**
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define  SOL_SOCKET  0xfff    /* options for socket level */


#define AF_UNSPEC       0
#define AF_INET         2
#if TLS_CONFIG_IPV6
#define AF_INET6        10
#else /* TLS_CONFIG_IPV6 */
#define AF_INET6        AF_UNSPEC
#endif /* TLS_CONFIG_IPV6 */
#define PF_INET         AF_INET
#define PF_INET6        AF_INET6
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#if TLS_CONFIG_IPV6
#define IPPROTO_IPV6    41
#define IPPROTO_ICMPV6  58
#endif /* TLS_CONFIG_IPV6 */
#define IPPROTO_UDPLITE 136
#define IPPROTO_RAW     255

/** Flags we can use with send and recv. */
#define MSG_PEEK       0x01    /* Peeks at an incoming message */
#define MSG_WAITALL    0x02    /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB        0x04    /* Unimplemented: Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */


/**
 * Options for level IPPROTO_IP
 */
#define IP_TOS             1
#define IP_TTL             2


/*
 * Options for level IPPROTO_TCP
 */
#define TCP_NODELAY    0x01    /* don't delay send to coalesce packets */
#define TCP_KEEPALIVE  0x02    /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPIDLE   0x03    /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPINTVL  0x04    /* set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPCNT    0x05    /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */

#if TLS_CONFIG_IPV6
/**
 * Options for level IPPROTO_IPV6
 */
#define IPV6_CHECKSUM       7  /* RFC3542: calculate and insert the ICMPv6 checksum for raw sockets. */
#define IPV6_V6ONLY         27 /* RFC3493: boolean control to restrict AF_INET6 sockets to IPv6 communications only. */
#endif /* TLS_CONFIG_IPV6 */

/**
 * Options for level IPPROTO_UDPLITE
 */
#define UDPLITE_SEND_CSCOV 0x01 /* sender checksum coverage */
#define UDPLITE_RECV_CSCOV 0x02 /* minimal receiver checksum coverage */

#if TLS_CONFIG_IGMP
/**
 * Options and types for UDP multicast traffic handling
 */
#define IP_MULTICAST_TTL   5
#define IP_MULTICAST_IF    6
#define IP_MULTICAST_LOOP  7
#endif /* TLS_CONFIG_IGMP */

#if 1// TLS_CONFIG_IGMP
/**
 * Options and types related to multicast membership
 */
#define IP_ADD_MEMBERSHIP  3
#define IP_DROP_MEMBERSHIP 4

typedef struct ip_mreq {
    struct in_addr imr_multiaddr; /* IP multicast address of group */
    struct in_addr imr_interface; /* local IP address of interface */
} ip_mreq;

#if TLS_CONFIG_IPV6
#define IPV6_JOIN_GROUP    38
#define IPV6_LEAVE_GROUP    39
typedef struct ipv6_mreq {
    struct in6_addr ipv6mr_multiaddr; /* IP multicast address of group */
    int ipv6mr_interface; /* index of interface */
} ipv6_mreq;
#endif
#endif /* TLS_CONFIG_IGMP */

/**
 * @brief The Type of Service provides an indication of the abstract
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
 * @brief The Network Control precedence designation is intended to be used
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


/*
 * @brief Commands for ioctlsocket(),  taken from the BSD file fcntl.h.
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

#ifndef FIONREAD
#define FIONREAD    _IOR('f', 127, unsigned long) /* get # bytes to read */
#endif
#ifndef FIONBIO
#define FIONBIO     _IOW('f', 126, unsigned long) /* set/clear non-blocking i/o */
#endif

/** Socket I/O Controls: unimplemented */
#ifndef SIOCSHIWAT
#define SIOCSHIWAT  _IOW('s',  0, unsigned long)  /* set high watermark */
#define SIOCGHIWAT  _IOR('s',  1, unsigned long)  /* get high watermark */
#define SIOCSLOWAT  _IOW('s',  2, unsigned long)  /* set low watermark */
#define SIOCGLOWAT  _IOR('s',  3, unsigned long)  /* get low watermark */
#define SIOCATMARK  _IOR('s',  7, unsigned long)  /* at oob mark? */
#endif

/** commands for fnctl */
#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif

/** File status flags and file access modes for fnctl,
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

/** FD_SET used for lwip_select */
#ifndef FD_SET
#undef  FD_SETSIZE

#ifndef LWIP_SOCKET_OFFSET
#define LWIP_SOCKET_OFFSET              0
#endif

#ifndef MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN                8
#endif

/** Make FD_SETSIZE match NUM_SOCKETS in socket.c */
#define FD_SETSIZE    MEMP_NUM_NETCONN
#define FDSETSAFESET(n, code) do { \
  if (((n) - LWIP_SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n) - LWIP_SOCKET_OFFSET) >= 0)) { \
  code; }} while(0)
#define FDSETSAFEGET(n, code) (((n) - LWIP_SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n) - LWIP_SOCKET_OFFSET) >= 0) ?\
  (code) : 0)
#define FD_SET(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] |=  (1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_CLR(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] &= ~(1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_ISSET(n,p) FDSETSAFEGET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] &   (1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_ZERO(p)    memset((void*)(p), 0, sizeof(*(p)))

typedef struct fd_set
{
  unsigned char fd_bits [(FD_SETSIZE+7)/8];
} fd_set;

#elif LWIP_SOCKET_OFFSET
#error LWIP_SOCKET_OFFSET does not work with external FD_SET!
#endif /* FD_SET */

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
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);

int bind(int s, const struct sockaddr *name, socklen_t namelen);

int shutdown(int s, int how);

int closesocket(int s);

int connect(int s, const struct sockaddr *name, socklen_t namelen);

int getsockname(int s, struct sockaddr *name, socklen_t *namelen);

int getpeername(int s, struct sockaddr *name, socklen_t *namelen);

int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);

int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);

int listen(int s, int backlog);

int recv(int s, void *mem, size_t len, int flags);

int recvfrom(int s, void *mem, size_t len, int flags,
        struct sockaddr *from, socklen_t *fromlen);

int send(int s, const void *data, size_t size, int flags);

int sendto(int s, const void *data, size_t size, int flags,
       const struct sockaddr *to, socklen_t tolen);

int socket(int domain, int type, int protocol);

int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout);

int ioctlsocket(int s, long cmd, void *argp);

//int fcntl(int s, int cmd, int val);

struct hostent* gethostbyname(const char *name);

/** @ingroup socket */
#define read(s,mem,len)                           recv(s,mem,len,0)
/** @ingroup socket */
#define write(s,dataptr,len)                      send(s,dataptr,len,0)
/** @ingroup socket */
#define close(s)                                  closesocket(s)
/** @ingroup socket */
#define ioctl(s,cmd,argp)                         ioctlsocket(s,cmd,argp)

u32_t ipaddr_addr(const char *cp);

#ifdef htonl
#undef htonl
#endif /* htonl */
#ifdef ntohl
#undef ntohl
#endif /* ntohl */
#ifdef htons
#undef htons
#endif /* htons */
#ifdef ntohs
#undef ntohs
#endif /* ntohs */

#define htons(n) 				(((n & 0xff) << 8) | ((n & 0xff00) >> 8))
#define htonl(n) 				(((n & 0xff) << 24) |\
			    				((n & 0xff00) << 8) |\
			   				((n & 0xff0000UL) >> 8) |\
			    				((n & 0xff000000UL) >> 24))
#define ntohs(n) 				htons(n)
#define ntohl(n) 				htonl(n)

/** Create u32_t value from bytes */
#define LWIP_MAKEU32(a,b,c,d) (((u32_t)((a) & 0xff) << 24) | \
                               ((u32_t)((b) & 0xff) << 16) | \
                               ((u32_t)((c) & 0xff) << 8)  | \
                                (u32_t)((d) & 0xff))
#define PP_HTONL(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))

#if TLS_CONFIG_IPV4
/** This is the aligned version of ip4_addr_t,
   used as local variable, on the stack, etc. */
struct ip4_addr {
  u32_t addr;
};

/** ip4_addr_t uses a struct for convenience only, so that the same defines can
 * operate both on ip4_addr_t as well as on ip4_addr_p_t. */
typedef struct ip4_addr ip4_addr_t;

/** Set an IP address given by the four byte-parts */
#define IP4_ADDR(ipaddr, a,b,c,d)  (ipaddr)->addr = PP_HTONL(LWIP_MAKEU32(a,b,c,d))

/** MEMCPY-like copying of IP addresses where addresses are known to be
 * 16-bit-aligned if the port is correctly configured (so a port could define
 * this to copying 2 u16_t's) - no NULL-pointer-checking needed. */
#ifndef IPADDR2_COPY
#define IPADDR2_COPY(dest, src) SMEMCPY(dest, src, sizeof(ip4_addr_t))
#endif

/** Copy IP address - faster than ip4_addr_set: no NULL check */
#define ip4_addr_copy(dest, src) ((dest).addr = (src).addr)
/** Safely copy one IP address to another (src may be NULL) */
#define ip4_addr_set(dest, src) ((dest)->addr = \
                                    ((src) == NULL ? 0 : \
                                    (src)->addr))
/** Set complete address to zero */
#define ip4_addr_set_zero(ipaddr)     ((ipaddr)->addr = 0)
/** Set address to IPADDR_ANY (no need for lwip_htonl()) */
#define ip4_addr_set_any(ipaddr)      ((ipaddr)->addr = IPADDR_ANY)
/** Set address to loopback address */
#define ip4_addr_set_loopback(ipaddr) ((ipaddr)->addr = PP_HTONL(IPADDR_LOOPBACK))
/** Check if an address is in the loopback region */
#define ip4_addr_isloopback(ipaddr)    (((ipaddr)->addr & PP_HTONL(IP_CLASSA_NET)) == PP_HTONL(((u32_t)IP_LOOPBACKNET) << 24))
/** Safely copy one IP address to another and change byte order
 * from host- to network-order. */
#define ip4_addr_set_hton(dest, src) ((dest)->addr = \
                               ((src) == NULL ? 0:\
                               lwip_htonl((src)->addr)))
/** IPv4 only: set the IP address given as an u32_t */
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
/** IPv4 only: get the IP address as an u32_t */
#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)

/** Get the network address by combining host address with netmask */
#define ip4_addr_get_network(target, host, netmask) do { ((target)->addr = ((host)->addr) & ((netmask)->addr)); } while(0)

/**
 * @brief Determine if two address are on the same network.
 *
 * @arg addr1 IP address 1
 * @arg addr2 IP address 2
 * @arg mask network identifier mask
 * @return !0 if the network identifiers of both address match
 */
#define ip4_addr_netcmp(addr1, addr2, mask) (((addr1)->addr & \
                                              (mask)->addr) == \
                                             ((addr2)->addr & \
                                              (mask)->addr))
#define ip4_addr_cmp(addr1, addr2) ((addr1)->addr == (addr2)->addr)

#define ip4_addr_isany_val(addr1)   ((addr1).addr == IPADDR_ANY)
#define ip4_addr_isany(addr1) ((addr1) == NULL || ip4_addr_isany_val(*(addr1)))

#define ip_addr_netmask_valid(netmask) ip4_addr_netmask_valid((netmask)->addr)
u8_t ip4_addr_netmask_valid(u32_t netmask);

#define ip4_addr_ismulticast(addr1) (((addr1)->addr & PP_HTONL(0xf0000000UL)) == PP_HTONL(0xe0000000UL))

#define ip4_addr_islinklocal(addr1) (((addr1)->addr & PP_HTONL(0xffff0000UL)) == PP_HTONL(0xa9fe0000UL))

#define ip4_addr_debug_print_parts(debug, a, b, c, d) \
  LWIP_DEBUGF(debug, ("%" U16_F ".%" U16_F ".%" U16_F ".%" U16_F, a, b, c, d))
#define ip4_addr_debug_print(debug, ipaddr) \
  ip4_addr_debug_print_parts(debug, \
                      (u16_t)((ipaddr) != NULL ? ip4_addr1_16(ipaddr) : 0),       \
                      (u16_t)((ipaddr) != NULL ? ip4_addr2_16(ipaddr) : 0),       \
                      (u16_t)((ipaddr) != NULL ? ip4_addr3_16(ipaddr) : 0),       \
                      (u16_t)((ipaddr) != NULL ? ip4_addr4_16(ipaddr) : 0))
#define ip4_addr_debug_print_val(debug, ipaddr) \
  ip4_addr_debug_print_parts(debug, \
                      ip4_addr1_16(&(ipaddr)),       \
                      ip4_addr2_16(&(ipaddr)),       \
                      ip4_addr3_16(&(ipaddr)),       \
                      ip4_addr4_16(&(ipaddr)))

/** Get one byte from the 4-byte address */
#define ip4_addr1(ipaddr) (((const u8_t*)(&(ipaddr)->addr))[0])
#define ip4_addr2(ipaddr) (((const u8_t*)(&(ipaddr)->addr))[1])
#define ip4_addr3(ipaddr) (((const u8_t*)(&(ipaddr)->addr))[2])
#define ip4_addr4(ipaddr) (((const u8_t*)(&(ipaddr)->addr))[3])
/** These are cast to u16_t, with the intent that they are often arguments
 * to printf using the U16_F format from cc.h. */
#define ip4_addr1_16(ipaddr) ((u16_t)ip4_addr1(ipaddr))
#define ip4_addr2_16(ipaddr) ((u16_t)ip4_addr2(ipaddr))
#define ip4_addr3_16(ipaddr) ((u16_t)ip4_addr3(ipaddr))
#define ip4_addr4_16(ipaddr) ((u16_t)ip4_addr4(ipaddr))

#define IP4ADDR_STRLEN_MAX  16

/** For backwards compatibility */
#define ip_ntoa(ipaddr)  ipaddr_ntoa(ipaddr)

u32_t ipaddr_addr(const char *cp);
int ip4addr_aton(const char *cp, ip4_addr_t *addr);
/** returns ptr to static buffer; not reentrant! */
char *ip4addr_ntoa(const ip4_addr_t *addr);
char *ip4addr_ntoa_r(const ip4_addr_t *addr, char *buf, int buflen);


/** directly map this to the lwip internal functions */
#define inet_addr(cp)                   ipaddr_addr(cp)
#define inet_aton(cp, addr)             ip4addr_aton(cp, (ip4_addr_t*)addr)
#define inet_ntoa(addr)       ipaddr_ntoa((ip_addr_t*)&(addr))
#endif

#if TLS_CONFIG_IPV6
/** This is the aligned version of ip6_addr_t,
    used as local variable, on the stack, etc. */
struct ip6_addr {
  u32_t addr[4];
};

/** IPv6 address */
typedef struct ip6_addr ip6_addr_t;

/** Set an IPv6 partial address given by byte-parts */
#define IP6_ADDR_PART(ip6addr, index, a,b,c,d) \
  (ip6addr)->addr[index] = PP_HTONL(LWIP_MAKEU32(a,b,c,d))

/** Set a full IPv6 address by passing the 4 u32_t indices in network byte order
    (use PP_HTONL() for constants) */
#define IP6_ADDR(ip6addr, idx0, idx1, idx2, idx3) do { \
  (ip6addr)->addr[0] = idx0; \
  (ip6addr)->addr[1] = idx1; \
  (ip6addr)->addr[2] = idx2; \
  (ip6addr)->addr[3] = idx3; } while(0)

/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK1(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[0]) >> 16) & 0xffff))
/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK2(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[0])) & 0xffff))
/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK3(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[1]) >> 16) & 0xffff))
/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK4(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[1])) & 0xffff))
/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK5(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[2]) >> 16) & 0xffff))
/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK6(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[2])) & 0xffff))
/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK7(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[3]) >> 16) & 0xffff))
/** Access address in 16-bit block */
#define IP6_ADDR_BLOCK8(ip6addr) ((u16_t)((lwip_htonl((ip6addr)->addr[3])) & 0xffff))

/** Copy IPv6 address - faster than ip6_addr_set: no NULL check */
#define ip6_addr_copy(dest, src) do{(dest).addr[0] = (src).addr[0]; \
                                    (dest).addr[1] = (src).addr[1]; \
                                    (dest).addr[2] = (src).addr[2]; \
                                    (dest).addr[3] = (src).addr[3];}while(0)
/** Safely copy one IPv6 address to another (src may be NULL) */
#define ip6_addr_set(dest, src) do{(dest)->addr[0] = (src) == NULL ? 0 : (src)->addr[0]; \
                                   (dest)->addr[1] = (src) == NULL ? 0 : (src)->addr[1]; \
                                   (dest)->addr[2] = (src) == NULL ? 0 : (src)->addr[2]; \
                                   (dest)->addr[3] = (src) == NULL ? 0 : (src)->addr[3];}while(0)

/** Set complete address to zero */
#define ip6_addr_set_zero(ip6addr)    do{(ip6addr)->addr[0] = 0; \
                                         (ip6addr)->addr[1] = 0; \
                                         (ip6addr)->addr[2] = 0; \
                                         (ip6addr)->addr[3] = 0;}while(0)

/** Set address to ipv6 'any' (no need for lwip_htonl()) */
#define ip6_addr_set_any(ip6addr)       ip6_addr_set_zero(ip6addr)
/** Set address to ipv6 loopback address */
#define ip6_addr_set_loopback(ip6addr) do{(ip6addr)->addr[0] = 0; \
                                          (ip6addr)->addr[1] = 0; \
                                          (ip6addr)->addr[2] = 0; \
                                          (ip6addr)->addr[3] = PP_HTONL(0x00000001UL);}while(0)
/** Safely copy one IPv6 address to another and change byte order
 * from host- to network-order. */
#define ip6_addr_set_hton(dest, src) do{(dest)->addr[0] = (src) == NULL ? 0 : lwip_htonl((src)->addr[0]); \
                                        (dest)->addr[1] = (src) == NULL ? 0 : lwip_htonl((src)->addr[1]); \
                                        (dest)->addr[2] = (src) == NULL ? 0 : lwip_htonl((src)->addr[2]); \
                                        (dest)->addr[3] = (src) == NULL ? 0 : lwip_htonl((src)->addr[3]);}while(0)


/**
 * @brief  Determine if two IPv6 address are on the same network.
 *
 * @arg addr1 IPv6 address 1
 * @arg addr2 IPv6 address 2
 * @return !0 if the network identifiers of both address match
 */
#define ip6_addr_netcmp(addr1, addr2) (((addr1)->addr[0] == (addr2)->addr[0]) && \
                                       ((addr1)->addr[1] == (addr2)->addr[1]))

#define ip6_addr_cmp(addr1, addr2) (((addr1)->addr[0] == (addr2)->addr[0]) && \
                                    ((addr1)->addr[1] == (addr2)->addr[1]) && \
                                    ((addr1)->addr[2] == (addr2)->addr[2]) && \
                                    ((addr1)->addr[3] == (addr2)->addr[3]))

#define ip6_get_subnet_id(ip6addr)   (lwip_htonl((ip6addr)->addr[2]) & 0x0000ffffUL)

#define ip6_addr_isany_val(ip6addr) (((ip6addr).addr[0] == 0) && \
                                     ((ip6addr).addr[1] == 0) && \
                                     ((ip6addr).addr[2] == 0) && \
                                     ((ip6addr).addr[3] == 0))
#define ip6_addr_isany(ip6addr) (((ip6addr) == NULL) || ip6_addr_isany_val(*(ip6addr)))

#define ip6_addr_isloopback(ip6addr) (((ip6addr)->addr[0] == 0UL) && \
                                      ((ip6addr)->addr[1] == 0UL) && \
                                      ((ip6addr)->addr[2] == 0UL) && \
                                      ((ip6addr)->addr[3] == PP_HTONL(0x00000001UL)))

#define ip6_addr_isglobal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xe0000000UL)) == PP_HTONL(0x20000000UL))

#define ip6_addr_islinklocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xffc00000UL)) == PP_HTONL(0xfe800000UL))

#define ip6_addr_issitelocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xffc00000UL)) == PP_HTONL(0xfec00000UL))

#define ip6_addr_isuniquelocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xfe000000UL)) == PP_HTONL(0xfc000000UL))

#define ip6_addr_isipv4mappedipv6(ip6addr) (((ip6addr)->addr[0] == 0) && ((ip6addr)->addr[1] == 0) && (((ip6addr)->addr[2]) == PP_HTONL(0x0000FFFFUL)))

#define ip6_addr_ismulticast(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xff000000UL)) == PP_HTONL(0xff000000UL))
#define ip6_addr_multicast_transient_flag(ip6addr)  ((ip6addr)->addr[0] & PP_HTONL(0x00100000UL))
#define ip6_addr_multicast_prefix_flag(ip6addr)     ((ip6addr)->addr[0] & PP_HTONL(0x00200000UL))
#define ip6_addr_multicast_rendezvous_flag(ip6addr) ((ip6addr)->addr[0] & PP_HTONL(0x00400000UL))
#define ip6_addr_multicast_scope(ip6addr) ((lwip_htonl((ip6addr)->addr[0]) >> 16) & 0xf)
#define IP6_MULTICAST_SCOPE_RESERVED            0x0
#define IP6_MULTICAST_SCOPE_RESERVED0           0x0
#define IP6_MULTICAST_SCOPE_INTERFACE_LOCAL     0x1
#define IP6_MULTICAST_SCOPE_LINK_LOCAL          0x2
#define IP6_MULTICAST_SCOPE_RESERVED3           0x3
#define IP6_MULTICAST_SCOPE_ADMIN_LOCAL         0x4
#define IP6_MULTICAST_SCOPE_SITE_LOCAL          0x5
#define IP6_MULTICAST_SCOPE_ORGANIZATION_LOCAL  0x8
#define IP6_MULTICAST_SCOPE_GLOBAL              0xe
#define IP6_MULTICAST_SCOPE_RESERVEDF           0xf
#define ip6_addr_ismulticast_iflocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xff8f0000UL)) == PP_HTONL(0xff010000UL))
#define ip6_addr_ismulticast_linklocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xff8f0000UL)) == PP_HTONL(0xff020000UL))
#define ip6_addr_ismulticast_adminlocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xff8f0000UL)) == PP_HTONL(0xff040000UL))
#define ip6_addr_ismulticast_sitelocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xff8f0000UL)) == PP_HTONL(0xff050000UL))
#define ip6_addr_ismulticast_orglocal(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xff8f0000UL)) == PP_HTONL(0xff080000UL))
#define ip6_addr_ismulticast_global(ip6addr) (((ip6addr)->addr[0] & PP_HTONL(0xff8f0000UL)) == PP_HTONL(0xff0e0000UL))

/** @todo define get/set for well-know multicast addresses, e.g. ff02::1 */
#define ip6_addr_isallnodes_iflocal(ip6addr) (((ip6addr)->addr[0] == PP_HTONL(0xff010000UL)) && \
    ((ip6addr)->addr[1] == 0UL) && \
    ((ip6addr)->addr[2] == 0UL) && \
    ((ip6addr)->addr[3] == PP_HTONL(0x00000001UL)))

#define ip6_addr_isallnodes_linklocal(ip6addr) (((ip6addr)->addr[0] == PP_HTONL(0xff020000UL)) && \
    ((ip6addr)->addr[1] == 0UL) && \
    ((ip6addr)->addr[2] == 0UL) && \
    ((ip6addr)->addr[3] == PP_HTONL(0x00000001UL)))
#define ip6_addr_set_allnodes_linklocal(ip6addr) do{(ip6addr)->addr[0] = PP_HTONL(0xff020000UL); \
                (ip6addr)->addr[1] = 0; \
                (ip6addr)->addr[2] = 0; \
                (ip6addr)->addr[3] = PP_HTONL(0x00000001UL);}while(0)

#define ip6_addr_isallrouters_linklocal(ip6addr) (((ip6addr)->addr[0] == PP_HTONL(0xff020000UL)) && \
    ((ip6addr)->addr[1] == 0UL) && \
    ((ip6addr)->addr[2] == 0UL) && \
    ((ip6addr)->addr[3] == PP_HTONL(0x00000002UL)))
#define ip6_addr_set_allrouters_linklocal(ip6addr) do{(ip6addr)->addr[0] = PP_HTONL(0xff020000UL); \
                (ip6addr)->addr[1] = 0; \
                (ip6addr)->addr[2] = 0; \
                (ip6addr)->addr[3] = PP_HTONL(0x00000002UL);}while(0)

#define ip6_addr_issolicitednode(ip6addr) ( ((ip6addr)->addr[0] == PP_HTONL(0xff020000UL)) && \
        ((ip6addr)->addr[2] == PP_HTONL(0x00000001UL)) && \
        (((ip6addr)->addr[3] & PP_HTONL(0xff000000UL)) == PP_HTONL(0xff000000UL)) )

#define ip6_addr_set_solicitednode(ip6addr, if_id) do{(ip6addr)->addr[0] = PP_HTONL(0xff020000UL); \
                (ip6addr)->addr[1] = 0; \
                (ip6addr)->addr[2] = PP_HTONL(0x00000001UL); \
                (ip6addr)->addr[3] = (PP_HTONL(0xff000000UL) | (if_id));}while(0)

#define ip6_addr_cmp_solicitednode(ip6addr, sn_addr) (((ip6addr)->addr[0] == PP_HTONL(0xff020000UL)) && \
                                    ((ip6addr)->addr[1] == 0) && \
                                    ((ip6addr)->addr[2] == PP_HTONL(0x00000001UL)) && \
                                    ((ip6addr)->addr[3] == (PP_HTONL(0xff000000UL) | (sn_addr)->addr[3])))

/* IPv6 address states. */
#define IP6_ADDR_INVALID      0x00
#define IP6_ADDR_TENTATIVE    0x08
#define IP6_ADDR_TENTATIVE_1  0x09 /* 1 probe sent */
#define IP6_ADDR_TENTATIVE_2  0x0a /* 2 probes sent */
#define IP6_ADDR_TENTATIVE_3  0x0b /* 3 probes sent */
#define IP6_ADDR_TENTATIVE_4  0x0c /* 4 probes sent */
#define IP6_ADDR_TENTATIVE_5  0x0d /* 5 probes sent */
#define IP6_ADDR_TENTATIVE_6  0x0e /* 6 probes sent */
#define IP6_ADDR_TENTATIVE_7  0x0f /* 7 probes sent */
#define IP6_ADDR_VALID        0x10 /* This bit marks an address as valid (preferred or deprecated) */
#define IP6_ADDR_PREFERRED    0x30
#define IP6_ADDR_DEPRECATED   0x10 /* Same as VALID (valid but not preferred) */

#define IP6_ADDR_TENTATIVE_COUNT_MASK 0x07 /* 1-7 probes sent */

#define ip6_addr_isinvalid(addr_state) (addr_state == IP6_ADDR_INVALID)
#define ip6_addr_istentative(addr_state) (addr_state & IP6_ADDR_TENTATIVE)
#define ip6_addr_isvalid(addr_state) (addr_state & IP6_ADDR_VALID) /* Include valid, preferred, and deprecated. */
#define ip6_addr_ispreferred(addr_state) (addr_state == IP6_ADDR_PREFERRED)
#define ip6_addr_isdeprecated(addr_state) (addr_state == IP6_ADDR_DEPRECATED)

#define ip6_addr_debug_print_parts(debug, a, b, c, d, e, f, g, h) \
  LWIP_DEBUGF(debug, ("%" X16_F ":%" X16_F ":%" X16_F ":%" X16_F ":%" X16_F ":%" X16_F ":%" X16_F ":%" X16_F, \
                      a, b, c, d, e, f, g, h))
#define ip6_addr_debug_print(debug, ipaddr) \
  ip6_addr_debug_print_parts(debug, \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK1(ipaddr) : 0),    \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK2(ipaddr) : 0),    \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK3(ipaddr) : 0),    \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK4(ipaddr) : 0),    \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK5(ipaddr) : 0),    \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK6(ipaddr) : 0),    \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK7(ipaddr) : 0),    \
                      (u16_t)((ipaddr) != NULL ? IP6_ADDR_BLOCK8(ipaddr) : 0))
#define ip6_addr_debug_print_val(debug, ipaddr) \
  ip6_addr_debug_print_parts(debug, \
                      IP6_ADDR_BLOCK1(&(ipaddr)),    \
                      IP6_ADDR_BLOCK2(&(ipaddr)),    \
                      IP6_ADDR_BLOCK3(&(ipaddr)),    \
                      IP6_ADDR_BLOCK4(&(ipaddr)),    \
                      IP6_ADDR_BLOCK5(&(ipaddr)),    \
                      IP6_ADDR_BLOCK6(&(ipaddr)),    \
                      IP6_ADDR_BLOCK7(&(ipaddr)),    \
                      IP6_ADDR_BLOCK8(&(ipaddr)))

#define IP6ADDR_STRLEN_MAX    46

int ip6addr_aton(const char *cp, ip6_addr_t *addr);
/** returns ptr to static buffer; not reentrant! */
char *ip6addr_ntoa(const ip6_addr_t *addr);
char *ip6addr_ntoa_r(const ip6_addr_t *addr, char *buf, int buflen);
                                    
#endif

/** @ingroup ipaddr
 * IP address types for use in ip_addr_t.type member.
 * @see tcp_new_ip_type(), udp_new_ip_type(), raw_new_ip_type().
 */
enum lwip_ip_addr_type {
  /** IPv4 */
  IPADDR_TYPE_V4 =   0U,
  /** IPv6 */
  IPADDR_TYPE_V6 =   6U,
  /** IPv4+IPv6 ("dual-stack") */
  IPADDR_TYPE_ANY = 46U
};

#if TLS_CONFIG_IPV4&&TLS_CONFIG_IPV6
/**
 * @ingroup ipaddr
 * A union struct for both IP version's addresses.
 * ATTENTION: watch out for its size when adding IPv6 address scope!
 */
typedef struct ip_addr {
  union {
    ip6_addr_t ip6;
    ip4_addr_t ip4;
  } u_addr;
  /** @ref lwip_ip_addr_type */
  u8_t type;
} ip_addr_t;

extern const ip_addr_t ip_addr_any_type;

/** @ingroup ip4addr */
#define IPADDR4_INIT(u32val)          { { { { u32val, 0ul, 0ul, 0ul } } }, IPADDR_TYPE_V4 }
/** @ingroup ip4addr */
#define IPADDR4_INIT_BYTES(a,b,c,d)   IPADDR4_INIT(PP_HTONL(LWIP_MAKEU32(a,b,c,d)))
/** @ingroup ip6addr */
#define IPADDR6_INIT(a, b, c, d)      { { { { a, b, c, d } } }, IPADDR_TYPE_V6 }
/** @ingroup ip6addr */
#define IPADDR6_INIT_HOST(a, b, c, d) { { { { PP_HTONL(a), PP_HTONL(b), PP_HTONL(c), PP_HTONL(d) } } }, IPADDR_TYPE_V6 }

/** @ingroup ipaddr */
#define IP_IS_ANY_TYPE_VAL(ipaddr)    (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_ANY)
/** @ingroup ipaddr */
#define IPADDR_ANY_TYPE_INIT          { { { { 0ul, 0ul, 0ul, 0ul } } }, IPADDR_TYPE_ANY }

/** @ingroup ip4addr */
#define IP_IS_V4_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_V4)
/** @ingroup ip6addr */
#define IP_IS_V6_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_V6)
/** @ingroup ip4addr */
#define IP_IS_V4(ipaddr)              (((ipaddr) == NULL) || IP_IS_V4_VAL(*(ipaddr)))
/** @ingroup ip6addr */
#define IP_IS_V6(ipaddr)              (((ipaddr) != NULL) && IP_IS_V6_VAL(*(ipaddr)))

#define IP_SET_TYPE_VAL(ipaddr, iptype) do { (ipaddr).type = (iptype); }while(0)
#define IP_SET_TYPE(ipaddr, iptype)     do { if((ipaddr) != NULL) { IP_SET_TYPE_VAL(*(ipaddr), iptype); }}while(0)
#define IP_GET_TYPE(ipaddr)           ((ipaddr)->type)

#define IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr) (IP_GET_TYPE(&pcb->local_ip) == IP_GET_TYPE(ipaddr))
#define IP_ADDR_PCB_VERSION_MATCH(pcb, ipaddr) (IP_IS_ANY_TYPE_VAL(pcb->local_ip) || IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr))

/** @ingroup ip6addr
 * Convert generic ip address to specific protocol version
 */
#define ip_2_ip6(ipaddr)   (&((ipaddr)->u_addr.ip6))
/** @ingroup ip4addr
 * Convert generic ip address to specific protocol version
 */
#define ip_2_ip4(ipaddr)   (&((ipaddr)->u_addr.ip4))

/** @ingroup ip4addr */
#define IP_ADDR4(ipaddr,a,b,c,d)      do { IP4_ADDR(ip_2_ip4(ipaddr),a,b,c,d); \
                                           IP_SET_TYPE_VAL(*(ipaddr), IPADDR_TYPE_V4); } while(0)
/** @ingroup ip6addr */
#define IP_ADDR6(ipaddr,i0,i1,i2,i3)  do { IP6_ADDR(ip_2_ip6(ipaddr),i0,i1,i2,i3); \
                                           IP_SET_TYPE_VAL(*(ipaddr), IPADDR_TYPE_V6); } while(0)
/** @ingroup ip6addr */
#define IP_ADDR6_HOST(ipaddr,i0,i1,i2,i3)  IP_ADDR6(ipaddr,PP_HTONL(i0),PP_HTONL(i1),PP_HTONL(i2),PP_HTONL(i3))

/** @ingroup ipaddr */
#define ip_addr_copy(dest, src)      do{ IP_SET_TYPE_VAL(dest, IP_GET_TYPE(&src)); if(IP_IS_V6_VAL(src)){ \
  ip6_addr_copy(*ip_2_ip6(&(dest)), *ip_2_ip6(&(src))); }else{ \
  ip4_addr_copy(*ip_2_ip4(&(dest)), *ip_2_ip4(&(src))); }}while(0)
/** @ingroup ip6addr */
#define ip_addr_copy_from_ip6(dest, src)      do{ \
  ip6_addr_copy(*ip_2_ip6(&(dest)), src); IP_SET_TYPE_VAL(dest, IPADDR_TYPE_V6); }while(0)
/** @ingroup ip4addr */
#define ip_addr_copy_from_ip4(dest, src)      do{ \
  ip4_addr_copy(*ip_2_ip4(&(dest)), src); IP_SET_TYPE_VAL(dest, IPADDR_TYPE_V4); }while(0)
/** @ingroup ip4addr */
#define ip_addr_set_ip4_u32(ipaddr, val)  do{if(ipaddr){ip4_addr_set_u32(ip_2_ip4(ipaddr), val); \
  IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }}while(0)
/** @ingroup ip4addr */
#define ip_addr_get_ip4_u32(ipaddr)  (((ipaddr) && IP_IS_V4(ipaddr)) ? \
  ip4_addr_get_u32(ip_2_ip4(ipaddr)) : 0)
/** @ingroup ipaddr */
#define ip_addr_set(dest, src) do{ IP_SET_TYPE(dest, IP_GET_TYPE(src)); if(IP_IS_V6(src)){ \
  ip6_addr_set(ip_2_ip6(dest), ip_2_ip6(src)); }else{ \
  ip4_addr_set(ip_2_ip4(dest), ip_2_ip4(src)); }}while(0)
/** @ingroup ipaddr */
#define ip_addr_set_ipaddr(dest, src) ip_addr_set(dest, src)
/** @ingroup ipaddr */
#define ip_addr_set_zero(ipaddr)     do{ \
  ip6_addr_set_zero(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, 0); }while(0)
/** @ingroup ip5addr */
#define ip_addr_set_zero_ip4(ipaddr)     do{ \
  ip6_addr_set_zero(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }while(0)
/** @ingroup ip6addr */
#define ip_addr_set_zero_ip6(ipaddr)     do{ \
  ip6_addr_set_zero(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V6); }while(0)
/** @ingroup ipaddr */
#define ip_addr_set_any(is_ipv6, ipaddr)      do{if(is_ipv6){ \
  ip6_addr_set_any(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V6); }else{ \
  ip4_addr_set_any(ip_2_ip4(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }}while(0)
/** @ingroup ipaddr */
#define ip_addr_set_loopback(is_ipv6, ipaddr) do{if(is_ipv6){ \
  ip6_addr_set_loopback(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V6); }else{ \
  ip4_addr_set_loopback(ip_2_ip4(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }}while(0)
/** @ingroup ipaddr */
#define ip_addr_set_hton(dest, src)  do{if(IP_IS_V6(src)){ \
  ip6_addr_set_hton(ip_2_ip6(ipaddr), (src)); IP_SET_TYPE(dest, IPADDR_TYPE_V6); }else{ \
  ip4_addr_set_hton(ip_2_ip4(ipaddr), (src)); IP_SET_TYPE(dest, IPADDR_TYPE_V4); }}while(0)
/** @ingroup ipaddr */
#define ip_addr_get_network(target, host, netmask) do{if(IP_IS_V6(host)){ \
  ip4_addr_set_zero(ip_2_ip4(target)); IP_SET_TYPE(target, IPADDR_TYPE_V6); } else { \
  ip4_addr_get_network(ip_2_ip4(target), ip_2_ip4(host), ip_2_ip4(netmask)); IP_SET_TYPE(target, IPADDR_TYPE_V4); }}while(0)
/** @ingroup ipaddr */
#define ip_addr_netcmp(addr1, addr2, mask) ((IP_IS_V6(addr1) && IP_IS_V6(addr2)) ? \
  0 : \
  ip4_addr_netcmp(ip_2_ip4(addr1), ip_2_ip4(addr2), mask))
/** @ingroup ipaddr */
#define ip_addr_cmp(addr1, addr2)    ((IP_GET_TYPE(addr1) != IP_GET_TYPE(addr2)) ? 0 : (IP_IS_V6_VAL(*(addr1)) ? \
  ip6_addr_cmp(ip_2_ip6(addr1), ip_2_ip6(addr2)) : \
  ip4_addr_cmp(ip_2_ip4(addr1), ip_2_ip4(addr2))))
/** @ingroup ipaddr */
#define ip_addr_isany(ipaddr)        ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_isany(ip_2_ip6(ipaddr)) : \
  ip4_addr_isany(ip_2_ip4(ipaddr)))
/** @ingroup ipaddr */
#define ip_addr_isany_val(ipaddr)        ((IP_IS_V6_VAL(ipaddr)) ? \
  ip6_addr_isany_val(*ip_2_ip6(&(ipaddr))) : \
  ip4_addr_isany_val(*ip_2_ip4(&(ipaddr))))
/** @ingroup ipaddr */
#define ip_addr_isbroadcast(ipaddr, netif) ((IP_IS_V6(ipaddr)) ? \
  0 : \
  ip4_addr_isbroadcast(ip_2_ip4(ipaddr), netif))
/** @ingroup ipaddr */
#define ip_addr_ismulticast(ipaddr)  ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_ismulticast(ip_2_ip6(ipaddr)) : \
  ip4_addr_ismulticast(ip_2_ip4(ipaddr)))
/** @ingroup ipaddr */
#define ip_addr_isloopback(ipaddr)  ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_isloopback(ip_2_ip6(ipaddr)) : \
  ip4_addr_isloopback(ip_2_ip4(ipaddr)))
/** @ingroup ipaddr */
#define ip_addr_islinklocal(ipaddr)  ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_islinklocal(ip_2_ip6(ipaddr)) : \
  ip4_addr_islinklocal(ip_2_ip4(ipaddr)))
#define ip_addr_debug_print(debug, ipaddr) do { if(IP_IS_V6(ipaddr)) { \
  ip6_addr_debug_print(debug, ip_2_ip6(ipaddr)); } else { \
  ip4_addr_debug_print(debug, ip_2_ip4(ipaddr)); }}while(0)
#define ip_addr_debug_print_val(debug, ipaddr) do { if(IP_IS_V6_VAL(ipaddr)) { \
  ip6_addr_debug_print_val(debug, *ip_2_ip6(&(ipaddr))); } else { \
  ip4_addr_debug_print_val(debug, *ip_2_ip4(&(ipaddr))); }}while(0)
/** @ingroup ipaddr */
#define ipaddr_ntoa(addr)   (((addr) == NULL) ? "NULL" : \
  ((IP_IS_V6(addr)) ? ip6addr_ntoa(ip_2_ip6(addr)) : ip4addr_ntoa(ip_2_ip4(addr))))
/** @ingroup ipaddr */
#define ipaddr_ntoa_r(addr, buf, buflen)   (((addr) == NULL) ? "NULL" : \
  ((IP_IS_V6(addr)) ? ip6addr_ntoa_r(ip_2_ip6(addr), buf, buflen) : ip4addr_ntoa_r(ip_2_ip4(addr), buf, buflen)))
int ipaddr_aton(const char *cp, ip_addr_t *addr);

/** @ingroup ipaddr */
#define IPADDR_STRLEN_MAX   IP6ADDR_STRLEN_MAX

/** @ingroup ipaddr */
#define ip4_2_ipv4_mapped_ipv6(ip6addr, ip4addr) do { \
  (ip6addr)->addr[3] = (ip4addr)->addr; \
  (ip6addr)->addr[2] = PP_HTONL(0x0000FFFFUL); \
  (ip6addr)->addr[1] = 0; \
  (ip6addr)->addr[0] = 0; } while(0);

/** @ingroup ipaddr */
#define unmap_ipv4_mapped_ipv6(ip4addr, ip6addr) \
  (ip4addr)->addr = (ip6addr)->addr[3];

#define IP46_ADDR_ANY(type) (((type) == IPADDR_TYPE_V6)? IP6_ADDR_ANY : IP4_ADDR_ANY)

#else
#define IP_ADDR_PCB_VERSION_MATCH(addr, pcb)         1
#define IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr) 1

#if TLS_CONFIG_IPV4
typedef ip4_addr_t ip_addr_t;
#define IPADDR4_INIT(u32val)                    { u32val }
#define IPADDR4_INIT_BYTES(a,b,c,d)             IPADDR4_INIT(PP_HTONL(LWIP_MAKEU32(a,b,c,d)))
#define IP_IS_V4_VAL(ipaddr)                    1
#define IP_IS_V6_VAL(ipaddr)                    0
#define IP_IS_V4(ipaddr)                        1
#define IP_IS_V6(ipaddr)                        0
#define IP_IS_ANY_TYPE_VAL(ipaddr)              0
#define IP_SET_TYPE_VAL(ipaddr, iptype)
#define IP_SET_TYPE(ipaddr, iptype)
#define IP_GET_TYPE(ipaddr)                     IPADDR_TYPE_V4
#define ip_2_ip4(ipaddr)                        (ipaddr)
#define IP_ADDR4(ipaddr,a,b,c,d)                IP4_ADDR(ipaddr,a,b,c,d)

#define ip_addr_copy(dest, src)                 ip4_addr_copy(dest, src)
#define ip_addr_copy_from_ip4(dest, src)        ip4_addr_copy(dest, src)
#define ip_addr_set_ip4_u32(ipaddr, val)        ip4_addr_set_u32(ip_2_ip4(ipaddr), val)
#define ip_addr_get_ip4_u32(ipaddr)             ip4_addr_get_u32(ip_2_ip4(ipaddr))
#define ip_addr_set(dest, src)                  ip4_addr_set(dest, src)
#define ip_addr_set_ipaddr(dest, src)           ip4_addr_set(dest, src)
#define ip_addr_set_zero(ipaddr)                ip4_addr_set_zero(ipaddr)
#define ip_addr_set_zero_ip4(ipaddr)            ip4_addr_set_zero(ipaddr)
#define ip_addr_set_any(is_ipv6, ipaddr)        ip4_addr_set_any(ipaddr)
#define ip_addr_set_loopback(is_ipv6, ipaddr)   ip4_addr_set_loopback(ipaddr)
#define ip_addr_set_hton(dest, src)             ip4_addr_set_hton(dest, src)
#define ip_addr_get_network(target, host, mask) ip4_addr_get_network(target, host, mask)
#define ip_addr_netcmp(addr1, addr2, mask)      ip4_addr_netcmp(addr1, addr2, mask)
#define ip_addr_cmp(addr1, addr2)               ip4_addr_cmp(addr1, addr2)
#define ip_addr_isany(ipaddr)                   ip4_addr_isany(ipaddr)
#define ip_addr_isany_val(ipaddr)               ip4_addr_isany_val(ipaddr)
#define ip_addr_isloopback(ipaddr)              ip4_addr_isloopback(ipaddr)
#define ip_addr_islinklocal(ipaddr)             ip4_addr_islinklocal(ipaddr)
#define ip_addr_isbroadcast(addr, netif)        ip4_addr_isbroadcast(addr, netif)
#define ip_addr_ismulticast(ipaddr)             ip4_addr_ismulticast(ipaddr)
#define ip_addr_debug_print(debug, ipaddr)      ip4_addr_debug_print(debug, ipaddr)
#define ip_addr_debug_print_val(debug, ipaddr)  ip4_addr_debug_print_val(debug, ipaddr)
#define ipaddr_ntoa(ipaddr)                     ip4addr_ntoa(ipaddr)
#define ipaddr_ntoa_r(ipaddr, buf, buflen)      ip4addr_ntoa_r(ipaddr, buf, buflen)
#define ipaddr_aton(cp, addr)                   ip4addr_aton(cp, addr)

#define IPADDR_STRLEN_MAX   IP4ADDR_STRLEN_MAX

#define IP46_ADDR_ANY(type) (IP4_ADDR_ANY)
#else
typedef ip6_addr_t ip_addr_t;
#define IPADDR6_INIT(a, b, c, d)                { { a, b, c, d } }
#define IPADDR6_INIT_HOST(a, b, c, d)           { { PP_HTONL(a), PP_HTONL(b), PP_HTONL(c), PP_HTONL(d) } }
#define IP_IS_V4_VAL(ipaddr)                    0
#define IP_IS_V6_VAL(ipaddr)                    1
#define IP_IS_V4(ipaddr)                        0
#define IP_IS_V6(ipaddr)                        1
#define IP_IS_ANY_TYPE_VAL(ipaddr)              0
#define IP_SET_TYPE_VAL(ipaddr, iptype)
#define IP_SET_TYPE(ipaddr, iptype)
#define IP_GET_TYPE(ipaddr)                     IPADDR_TYPE_V6
#define ip_2_ip6(ipaddr)                        (ipaddr)
#define IP_ADDR6(ipaddr,i0,i1,i2,i3)            IP6_ADDR(ipaddr,i0,i1,i2,i3)
#define IP_ADDR6_HOST(ipaddr,i0,i1,i2,i3)       IP_ADDR6(ipaddr,PP_HTONL(i0),PP_HTONL(i1),PP_HTONL(i2),PP_HTONL(i3))

#define ip_addr_copy(dest, src)                 ip6_addr_copy(dest, src)
#define ip_addr_copy_from_ip6(dest, src)        ip6_addr_copy(dest, src)
#define ip_addr_set(dest, src)                  ip6_addr_set(dest, src)
#define ip_addr_set_ipaddr(dest, src)           ip6_addr_set(dest, src)
#define ip_addr_set_zero(ipaddr)                ip6_addr_set_zero(ipaddr)
#define ip_addr_set_zero_ip6(ipaddr)            ip6_addr_set_zero(ipaddr)
#define ip_addr_set_any(is_ipv6, ipaddr)        ip6_addr_set_any(ipaddr)
#define ip_addr_set_loopback(is_ipv6, ipaddr)   ip6_addr_set_loopback(ipaddr)
#define ip_addr_set_hton(dest, src)             ip6_addr_set_hton(dest, src)
#define ip_addr_get_network(target, host, mask) ip6_addr_set_zero(target)
#define ip_addr_netcmp(addr1, addr2, mask)      0
#define ip_addr_cmp(addr1, addr2)               ip6_addr_cmp(addr1, addr2)
#define ip_addr_isany(ipaddr)                   ip6_addr_isany(ipaddr)
#define ip_addr_isany_val(ipaddr)               ip6_addr_isany_val(ipaddr)
#define ip_addr_isloopback(ipaddr)              ip6_addr_isloopback(ipaddr)
#define ip_addr_islinklocal(ipaddr)             ip6_addr_islinklocal(ipaddr)
#define ip_addr_isbroadcast(addr, netif)        0
#define ip_addr_ismulticast(ipaddr)             ip6_addr_ismulticast(ipaddr)
#define ip_addr_debug_print(debug, ipaddr)      ip6_addr_debug_print(debug, ipaddr)
#define ip_addr_debug_print_val(debug, ipaddr)  ip6_addr_debug_print_val(debug, ipaddr)
#define ipaddr_ntoa(ipaddr)                     ip6addr_ntoa(ipaddr)
#define ipaddr_ntoa_r(ipaddr, buf, buflen)      ip6addr_ntoa_r(ipaddr, buf, buflen)
#define ipaddr_aton(cp, addr)                   ip6addr_aton(cp, addr)

#define IPADDR_STRLEN_MAX   IP6ADDR_STRLEN_MAX

#define IP46_ADDR_ANY(type) (IP6_ADDR_ANY)
#endif
#endif


#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN     IP4ADDR_STRLEN_MAX
#endif
#if TLS_CONFIG_IPV6
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN    IP6ADDR_STRLEN_MAX
#endif
#endif

#if TLS_CONFIG_IPV4 && TLS_CONFIG_IPV6
/** @ingroup socket */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((const ip6_addr_t*)(src),(dst),(size)) \
     : (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL))
/** @ingroup socket */
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? ip6addr_aton((src),(ip6_addr_t*)(dst)) \
     : (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0))
#elif TLS_CONFIG_IPV4 /* TLS_CONFIG_IPV4 && TLS_CONFIG_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0)
#else /* TLS_CONFIG_IPV4 && TLS_CONFIG_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((const ip6_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? ip6addr_aton((src),(ip6_addr_t*)(dst)) : 0)
#endif /* TLS_CONFIG_IPV4 && TLS_CONFIG_IPV6 */

extern void print_ipaddr(ip_addr_t *ip);
#endif
