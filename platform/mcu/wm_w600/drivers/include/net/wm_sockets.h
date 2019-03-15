/**
 * @file    wm_sockets.h
 *
 * @brief   socket apis
 *
 * @author  winnermicro
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_SOCKET_API_H
#define WM_SOCKET_API_H
#include "wm_config.h"
#if TLS_CONFIG_LWIP_VER2_0_3
#include "lwip/sockets.h"
#else

#include <stdio.h>
#include "wm_type_def.h"
#include "wm_netif.h"

#ifdef socklen_t
#undef socklen_t
#endif
/** Type for length arguments in socket calls */
typedef unsigned int socklen_t;
/* If your port already typedef's sa_family_t, define SA_FAMILY_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(sa_family_t) && !defined(SA_FAMILY_T_DEFINED)
typedef u8 sa_family_t;
#endif
/* If your port already typedef's in_port_t, define IN_PORT_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(in_port_t) && !defined(IN_PORT_T_DEFINED)
typedef u16 in_port_t;
#endif

/** Internet address */
struct in_address{
  u32 s_addr;    /**< address */
};

struct in6_addr {
  union {
    u32 u32_addr[4];
    u8  u8_addr[16];
  } un;
#define s6_addr  un.u8_addr
};

/** Structure describing an Internet (IP) socket address,
    members are in network byte order */
struct sockaddr_in {
  u8 sin_len;       /**< Address length */
  u8 sin_family;    /**< Address family */
  u16 sin_port;     /**< Port number */
  struct in_address sin_addr;/**< Structure describing an Internet (IP) socket address */
  char sin_zero[8]; /**< Pad to size of `struct sockaddr' */
};

struct sockaddr_in6 {
  u8            sin6_len;      /* length of this structure    */
  sa_family_t     sin6_family;   /* AF_INET6                    */
  in_port_t       sin6_port;     /* Transport layer port #      */
  u32           sin6_flowinfo; /* IPv6 flow information       */
  struct in6_addr sin6_addr;     /* IPv6 address                */
  u32           sin6_scope_id; /* Set of interfaces for scope */
};

/** Structure describing a generic socket address */
struct sockaddr {
  u8 sa_len;           /**< address length */
  u8 sa_family;        /**< address family */
  char sa_data[14];    /**< Address data */
};

/** Structure describing a host entry */
struct hostent {
    char  *h_name;      /**< Official name of the host. */
    char **h_aliases;   /**< A pointer to an array of pointers to alternative host names,
                             terminated by a null pointer. */
    int    h_addrtype;  /**< Address type. */
    int    h_length;    /**< The length, in bytes, of the address. */
    char **h_addr_list; /**< A pointer to an array of pointers to network addresses (in
                             network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /**< for backward compatibility */
};


/** Socket protocol types: TCP */
#define SOCK_STREAM     1
/** Socket protocol types: UDP */
#define SOCK_DGRAM      2
/** Socket protocol types: RAW */
#define SOCK_RAW        3

/*
 * Option flags per-socket. These must match the SOF_ flags in ip.h (checked in init.c)
 */
/** Unimplemented: turn on debugging info recording */
#define  SO_DEBUG       0x0001
/** socket has had listen() */
#define  SO_ACCEPTCONN  0x0002
/** Allow local address reuse */
#define  SO_REUSEADDR   0x0004
/** keep connections alive */
#define  SO_KEEPALIVE   0x0008
/** Unimplemented: just use interface addresses */
#define  SO_DONTROUTE   0x0010
/** permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */
#define  SO_BROADCAST   0x0020
/** Unimplemented: bypass hardware when possible */
#define  SO_USELOOPBACK 0x0040
/** linger on close if data present */
#define  SO_LINGER      0x0080
/** Unimplemented: leave received OOB data in line */
#define  SO_OOBINLINE   0x0100
/** Unimplemented: allow local address & port reuse */
#define  SO_REUSEPORT   0x0200
/** do not linger on close if data present */
#define  SO_DONTLINGER   ((int)(~SO_LINGER))

/*
 * Additional options, not kept in so_options.
 */
/** Unimplemented: send buffer size */
#define SO_SNDBUF    0x1001
/** receive buffer size */
#define SO_RCVBUF    0x1002
/** Unimplemented: send low-water mark */
#define SO_SNDLOWAT  0x1003
/** Unimplemented: receive low-water mark */
#define SO_RCVLOWAT  0x1004
/** Unimplemented: send timeout */
#define SO_SNDTIMEO  0x1005
/** receive timeout */
#define SO_RCVTIMEO  0x1006
/** get error status and clear */
#define SO_ERROR     0x1007
/** get socket type */
#define SO_TYPE      0x1008
/** Unimplemented: connect timeout */
#define SO_CONTIMEO  0x1009
/** don't create UDP checksum */
#define SO_NO_CHECK  0x100a


/** Structure used for manipulating linger option. */
struct linger {
       int l_onoff;                /**< option on/off */
       int l_linger;               /**< linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
/** options for socket level */
#define  SOL_SOCKET  0xfff

/*
 * Address families.
 */
/** Unspecified */
#define AF_UNSPEC       0
/** IP Address family */
#define AF_INET         2
/** IP protocol family */
#define PF_INET         AF_INET
/** Unspecified */
#define PF_UNSPEC       AF_UNSPEC
/** IP version 6	*/
#define AF_INET6	10

/*
 * Standard well-defined IP protocols.
 */
/** Dummy protocol for TCP		*/
#define IPPROTO_IP      0
/** Transmission Control Protocol	*/
#define IPPROTO_TCP     6
/** User Datagram Protocol		*/
#define IPPROTO_UDP     17
/** UDP-Lite (RFC 3828)			*/
#define IPPROTO_UDPLITE 136

/* Flags we can use with send and recv. */
/** Peeks at an incoming message */
#define MSG_PEEK       0x01
/** Unimplemented: Requests that the function block
    until the full amount of data requested can be returned */
#define MSG_WAITALL    0x02
/** Unimplemented: Requests out-of-band data.
    The significance and semantics of out-of-band data are protocol-specific */
#define MSG_OOB        0x04
/** Nonblocking i/o for this operation only */
#define MSG_DONTWAIT   0x08
/** Sender will send more */
#define MSG_MORE       0x10


/*
 * Options for level IPPROTO_IP
 */
/** int; IP type of service and precedence.  */
#define IP_TOS             1
/** int; IP time to live.  */
#define IP_TTL             2

#if LWIP_TCP
/*
 * Options for level IPPROTO_TCP
 */
/** don't delay send to coalesce packets */
#define TCP_NODELAY    0x01
/** send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPALIVE  0x02
/** set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPIDLE   0x03
/** set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPINTVL  0x04
/** set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */
#define TCP_KEEPCNT    0x05
#endif /* LWIP_TCP */

#if LWIP_UDP && LWIP_UDPLITE
/*
 * Options for level IPPROTO_UDPLITE
 */
/** sender checksum coverage */
#define UDPLITE_SEND_CSCOV 0x01
/** minimal receiver checksum coverage */
#define UDPLITE_RECV_CSCOV 0x02
#endif /* LWIP_UDP && LWIP_UDPLITE*/


#if 1
/*
 * Options and types for UDP multicast traffic handling
 */
/** ip_mreq; add an IP group membership */
#define IP_ADD_MEMBERSHIP  3
/** ip_mreq; drop an IP group membership */
#define IP_DROP_MEMBERSHIP 4
/** u_char; set/get IP multicast ttl */
#define IP_MULTICAST_TTL   5
/** in_addr; set/get IP multicast i/f */
#define IP_MULTICAST_IF    6
/** i_char; set/get IP multicast loopback */
#define IP_MULTICAST_LOOP  7

/** Request struct for multicast socket ops */
typedef struct ip_mreq {
    struct in_address imr_multiaddr; /**< IP multicast address of group */
    struct in_address imr_interface; /**< local IP address of interface */
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
/** parameters must be < 128 bytes */
#define IOCPARM_MASK    0x7fU
/** no parameters */
#define IOC_VOID        0x20000000UL
/** copy out parameters */
#define IOC_OUT         0x40000000UL
/** copy in parameters */
#define IOC_IN          0x80000000UL
#define IOC_INOUT       (IOC_IN|IOC_OUT)
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))

#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#endif /* !defined(FIONREAD) || !defined(FIONBIO) */

#ifndef FIONREAD
/** get # bytes to read */
#define FIONREAD    _IOR('f', 127, unsigned long)
#endif
#ifndef FIONBIO
/** set/clear non-blocking i/o */
#define FIONBIO     _IOW('f', 126, unsigned long)
#endif

/* Socket I/O Controls: unimplemented */
#ifndef SIOCSHIWAT
/** set high watermark */
#define SIOCSHIWAT  _IOW('s',  0, unsigned long)
/** get high watermark */
#define SIOCGHIWAT  _IOR('s',  1, unsigned long)
/** set low watermark */
#define SIOCSLOWAT  _IOW('s',  2, unsigned long)
/** get low watermark */
#define SIOCGLOWAT  _IOR('s',  3, unsigned long)
/** at oob mark */
#define SIOCATMARK  _IOR('s',  7, unsigned long)
#endif

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
/** nonblocking I/O */
#define O_NONBLOCK  1
#endif
#ifndef O_NDELAY
/** same as O_NONBLOCK, for compatibility */
#define O_NDELAY    1
#endif

#ifndef SHUT_RD
/** No more receptions */
#define SHUT_RD   0
/** No more transmissions */
#define SHUT_WR   1
/** No more receptions or transmissions */
#define SHUT_RDWR 2
#endif

/* FD_SET used for lwip_select */
#ifndef FD_SET
  #undef  FD_SETSIZE
  /* Make FD_SETSIZE match NUM_SOCKETS in socket.c */
  #define FD_SETSIZE    8
  #define FD_SET(n, p)  ((p)->fd_bits[(n)/8] |=  (1 << ((n) & 7)))
  #define FD_CLR(n, p)  ((p)->fd_bits[(n)/8] &= ~(1 << ((n) & 7)))
  #define FD_ISSET(n,p) ((p)->fd_bits[(n)/8] &   (1 << ((n) & 7)))
  #define FD_ZERO(p)    memset((void*)(p),0,sizeof(*(p)))

  typedef struct fd_set {
          unsigned char fd_bits [(FD_SETSIZE+7)/8];
        } fd_set;

#endif /* FD_SET */

/** LWIP_TIMEVAL_PRIVATE: if you want to use the struct timeval provided
 * by your system, set this to 0 and include <sys/time.h> in cc.h */
#ifndef LWIP_TIMEVAL_PRIVATE
#define LWIP_TIMEVAL_PRIVATE 1
#endif

#if LWIP_TIMEVAL_PRIVATE
/** A time value that is accurate to the nearest
    microsecond but also has a range of years */
struct timeval {
  long    tv_sec;         /**< seconds */
  long    tv_usec;        /**< and microseconds */
};
#endif /* LWIP_TIMEVAL_PRIVATE */

#define ip_addr_get_ip4_u32(ipaddr)    ((ipaddr)->addr)

/**
 * @defgroup Net_APIs Net APIs
 * @brief Net related APIs
 */

/**
 * @addtogroup Net_APIs
 * @{
 */

/**
 * @defgroup SOCKET_APIs SOCKET APIs
 * @brief SOCKET APIs
 */

/**
 * @addtogroup SOCKET_APIs
 * @{
 */

/**
 * @brief          await a connection on socket ID
 *
 * @param[in]      s            socket ID
 * @param[out]     *addr        pointer to the address of the connecting
 * @param[out]     *addrlen     pointer to the address's actual length
 *
 * @retval         -1           failed
 * @retval         other        the new socket's descriptor
 *
 * @note           None
 */
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);

/**
 * @brief          give the socket ID the local address
 *
 * @param[in]      s            socket ID
 * @param[in]      *name        the local address
 * @param[in]      namelen      the local address length
 *
 * @retval          0           success
 * @retval         -1           failed
 *
 * @note           None
 */
int bind(int s, const struct sockaddr *name, socklen_t namelen);

/**
 * @brief          shut down all or part of the connection open on socket ID
 *
 * @param[in]      s        socket ID
 * @param[in]      how      HOW determines what to shut down:
                            SHUT_RD   = No more receptions;
                            SHUT_WR   = No more transmissions;
                            SHUT_RDWR = No more receptions or transmissions.
 *
 * @retval          0       success
 * @retval         -1       failed
 *
 * @note           None
 */
int shutdown(int s, int how);

/**
 * @brief          destroy all or part of the connection open on socket ID
 *
 * @param[in]      s     socket ID
 *
 * @retval         0     success
 * @retval        -1     failed
 *
 * @note           None
 */
int closesocket(int s);

/**
 * @brief          Open a connection on socket FD to peer at name.
 *                 For connectionless socket types, just set the default address
 *                 to send to and the only address from which to accept transmissions
 *
 * @param[in]      s            socket ID
 * @param[in]      *name        the address
 * @param[in]      namelen      the address length
 *
 * @retval         0            success
 * @retval        -1            failed
 *
 * @note           None
 */
int connect(int s, const struct sockaddr *name, socklen_t namelen);

/**
 * @brief          Put the local address of ID into *name and its length in *namelen
 *
 * @param[in]      s           socket ID
 * @param[out]     *name       the address
 * @param[out]     *namelen    the address length
 *
 * @retval         0           success
 * @retval        -1           failed
 *
 * @note           None
 */
int getsockname(int s, struct sockaddr *name, socklen_t *namelen);

/**
 * @brief          Put the address of the peer connected to socket FD into *name (which
 *                 is *namelen bytes long), and its actual length into *namelen
 *
 * @param[in]      s            socket ID
 * @param[out]     *name        the peer
 * @param[out]     *namelen     the peer length
 *
 * @retval         0            success
 * @retval        -1            failed
 *
 * @note           None
 */
int getpeername(int s, struct sockaddr *name, socklen_t *namelen);

/**
 * @brief          Set socket ID's option optname at protocol level to *optval (which
 *                 is optlen bytes long)
 *
 * @param[in]      s            socket ID
 * @param[in]      level        protocol level
 * @param[in]      optname      option name
 * @param[in]      *optval      option value
 * @param[in]      optlen       option value length
 *
 * @retval         0            success
 * @retval        -1            failed
 *
 * @note           None
 */
int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);

/**
 * @brief          Put the current value for socket ID's option optname at protocol
 *                 level into optval (which is *optlen bytes long), and set *optlen
 *                 to the value's actual length
 *
 * @param[in]      s            socket ID
 * @param[in]      level        protocol level
 * @param[in]      optname      option name
 * @param[out]     *optval      option value
 * @param[out]     *optlen      option value length
 *
 * @retval         0            success
 * @retval        -1            failed
 *
 * @note           None
 */
int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);

/**
 * @brief          Prepare to accept connections on socket ID
 *
 * @param[in]      s          socket ID
 * @param[in]      backlog    the maximum length to which the queue of pending connections
 *
 * @retval         0          success
 * @retval        -1          failed
 *
 * @note
 * N connection requests will be queued before further requests are refused.
 */
int listen(int s, int backlog);

/**
 * @brief          Read len bytes into mem from socket ID
 *
 * @param[in]      s        socket ID
 * @param[out]     *mem     pointer to mem
 * @param[in]      len      length of Read bytes
 * @param[in]      flags    option of read
 *
 * @retval         -1       failed
 * @retval         other    the number read actually
 *
 * @note           None
 */
int recv(int s, void *mem, size_t len, int flags);

/**
 * @brief          Read len bytes into mem through socket ID
 *
 * @param[in]      s            socket ID
 * @param[out]     *mem         pointer to mem
 * @param[in]      len          length of Read bytes
 * @param[in]      flags        option of read
 * @param[out]     *from        address of sender
 * @param[out]     *fromlen     length of address
 *
 * @retval         -1           failed
 * @retval         other        the number of bytes read actually
 *
 * @note
 * If from is not NULL, fill in *fromlen bytes of it with tha address of
 * the sender, and store the actual size of the address in *fromlen.
 */
int recvfrom(int s, void *mem, size_t len, int flags,
        struct sockaddr *from, socklen_t *fromlen);

/**
 * @brief          Send size bytes of data to socket ID
 *
 * @param[in]      s        socket ID
 * @param[in]      *data    pointer to data will be sent
 * @param[in]      size     length of data
 * @param[in]      flags    option of sent
 *
 * @retval         -1       failed
 * @retval         other    the number sent actually
 *
 * @note           None
 */
int send(int s, const void *data, size_t size, int flags);

/**
 * @brief          Send size bytes of data on socket ID to peer at address to (which
 *                 is tolen bytes long)
 *
 * @param[in]      s            socket ID
 * @param[in]      *data        pointer to data will be sent
 * @param[in]      size         length of data
 * @param[in]      flags        option of sent
 * @param[in]      *to          pointer to address of destination
 * @param[in]      tolen        length of address
 *
 * @retval         -1           failed
 * @retval         other        the number sent actually
 *
 * @note           None
 */
int sendto(int s, const void *data, size_t size, int flags,
       const struct sockaddr *to, socklen_t tolen);

/**
 * @brief          Create a new socket of type type in domain domain, using
 *                 protocol protocol.  If protocol is zero, one is chosen automatically
 *
 * @param[in]      domain           Address families
 * @param[in]      type             Socket protocol type
 * @param[in]      protocol         Standard well-defined IP protocols
 *
 * @retval         -1               failed
 * @retval         other            the new socket ID
 *
 * @note           None
 */
int socket(int domain, int type, int protocol);

/**
 * @brief          Check the first NFDS descriptors each in READFDS (if not NULL)
 *                 for read readiness, in WRITEFDS (if not NULL) for write readiness,
 *                 and in EXCEPTFDS (if not NULL) for exceptional conditions.
 *
 * @param[in]      maxfdp1          the highest socket index in the sets
 * @param[in]      *readset         set of sockets to check for read events
 * @param[in]      *writeset        set of sockets to check for write events
 * @param[in]      *exceptset       set of sockets to check for error events
 * @param[in]      *timeout         waitting time value
 *
 * @retval         -1               failed
 * @retval         other            the number of ready
 *
 * @note
 * If TIMEOUT is not NULL, time out after waiting the interval specified therein.
 */
int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout);

/**
 * @brief          Set/Get socket ID's option by command
 *
 * @param[in]      s        socket ID
 * @param[in]      cmd      command
 * @param[inout]   *argp    pointer to the parameters
 *
 * @retval         0        success
 * @retval        -1        failed
 *
 * @note           None
 */
int ioctlsocket(int s, long cmd, void *argp);

/**
 * @brief          get entry from host data base for host with name
 *
 * @param[in]      *name    hostname
 *
 * @retval        !NULL     success
 * @retval         NULL     failed
 *
 * @note           None
 */
struct hostent* gethostbyname(const char *name);

/**
 * @brief          Ascii internet address interpretation routine
 *
 * @param[in]      *cp  IP address in ascii represenation (e.g. "127.0.0.1")
 *
 * @return         ip address in network order
 *
 * @note           None
 */
u32 ipaddr_addr(const char *cp);

/**
 * @brief          Convert numeric IP address into decimal dotted ASCII representation
 *
 * @param[in]      *addr    addr ip address in network order to convert
 *
 * @return         pointer to a global static (!) buffer that holds the ASCII represenation of addr
 *
 * @note           returns ptr to static buffer; not reentrant!
 */
char * ipaddr_ntoa(const ip_addr_t *addr);

/**
 * @}
 */

/**
 * @}
 */

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

/** convert 16bit between host and network byte order */
#define htons(n)               ((((n) & 0xff) << 8)       | (((n) & 0xff00) >> 8))
/** convert 32bit between host and network byte order */
#define htonl(n)               ((((n) & 0xff) << 24)      | (((n) & 0xff00) << 8) |\
                                (((n) & 0xff0000UL) >> 8) | (((n) & 0xff000000UL) >> 24))
/** convert 16bit between network and host byte order */
#define ntohs(n) 				htons(n)
/** convert 32bit between network and host byte order */
#define ntohl(n) 				htonl(n)

/** Get first byte from the 4-byte address */
#define ip4_addr1(ipaddr) (((u8*)(ipaddr))[0])
/** Get second byte from the 4-byte address */
#define ip4_addr2(ipaddr) (((u8*)(ipaddr))[1])
/** Get third byte from the 4-byte address */
#define ip4_addr3(ipaddr) (((u8*)(ipaddr))[2])
/** Get fourth byte from the 4-byte address */
#define ip4_addr4(ipaddr) (((u8*)(ipaddr))[3])
/** Convert numeric IP address into decimal dotted ASCII representation */
#define inet_ntoa(addr)       ipaddr_ntoa((ip_addr_t*)&(addr))

#endif
#endif

