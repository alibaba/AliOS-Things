/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_SOCKET_H_
#define _SAL_SOCKET_H_

#include <stddef.h> /* for size_t */
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AF_UNSPEC       0
#define AF_INET         2
#define AF_INET6        10
#define PF_INET         AF_INET
#define PF_INET6        AF_INET6
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3


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

#define DNS_MAX_NAME_LENGTH 256

struct sockaddr {
    u8_t        sa_len;
    sa_family_t sa_family;
    char        sa_data[14];
};

/* members are in network byte order */
struct sockaddr_in {
    u8_t            sin_len;
    sa_family_t     sin_family;
    in_port_t       sin_port;
    struct in_addr  sin_addr;
#define SIN_ZERO_LEN 8
    char            sin_zero[SIN_ZERO_LEN];
};

struct sockaddr_in6 {
    u8_t            sin6_len;      /* length of this structure    */
    sa_family_t     sin6_family;   /* AF_INET6                    */
    in_port_t       sin6_port;     /* Transport layer port #      */
    u32_t           sin6_flowinfo; /* IPv6 flow information       */
    struct in6_addr sin6_addr;     /* IPv6 address                */
    u32_t           sin6_scope_id; /* Set of interfaces for scope */
};

/* If your port already typedef's socklen_t, define SOCKLEN_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(socklen_t) && !defined(SOCKLEN_T_DEFINED)
typedef u32_t socklen_t;
#endif

struct hostent {
    char  *h_name;      /* Official name of the host. */
    char **h_aliases;   /* A pointer to an array of pointers to alternative
                           host names, terminated by a null pointer. */
    int    h_addrtype;  /* Address type. */
    int    h_length;    /* The length, in bytes, of the address. */
    char **h_addr_list; /* A pointer to an array of pointers to network
                           addresses (in network byte order) for the host,
                           terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
};

struct addrinfo {
    int               ai_flags;      /* Input flags. */
    int               ai_family;     /* Address family of socket. */
    int               ai_socktype;   /* Socket type. */
    int               ai_protocol;   /* Protocol of socket. */
    socklen_t         ai_addrlen;    /* Length of socket address. */
    struct sockaddr  *ai_addr;       /* Socket address of socket. */
    char             *ai_canonname;  /* Canonical name of service location. */
    struct addrinfo  *ai_next;       /* Pointer to next in list. */
};

#define  SOL_SOCKET  0xfff    /* options for socket level */

#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

#define IPPROTO_IPV6    41
#define IPPROTO_ICMPV6  58

#define IPPROTO_UDPLITE 136
#define IPPROTO_RAW     255

/* Flags we can use with send and recv. */
#define MSG_PEEK       0x01    /* Peeks at an incoming message */
#define MSG_WAITALL    0x02    /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB        0x04    /* Unimplemented: Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */

#define MEMP_NUM_NETCONN     5//(MAX_SOCKETS_TCP + MAX_LISTENING_SOCKETS_TCP + MAX_SOCKETS_UDP)

#ifndef SAL_SOCKET_OFFSET
#define  SAL_SOCKET_OFFSET 0
#endif

/* FD_SET used for event_select */
#ifndef FD_SET
#undef  FD_SETSIZE
/* Make FD_SETSIZE match NUM_SOCKETS in socket.c */
#define FD_SETSIZE    MEMP_NUM_NETCONN

#define FDSETSAFESET(n, code) do { \
  if (((n) - SAL_SOCKET_OFFSET < MEMP_NUM_NETCONN *2) && (((int)(n) - SAL_SOCKET_OFFSET) >= 0)) { \
  code; }} while(0)
#define FDSETSAFEGET(n, code) (((n) - SAL_SOCKET_OFFSET < MEMP_NUM_NETCONN *2) && (((int)(n) - SAL_SOCKET_OFFSET) >= 0) ?\
  (code) : 0)
#define FD_SET(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-SAL_SOCKET_OFFSET)/8] |=  (1 << (((n)-SAL_SOCKET_OFFSET) & 7)))
#define FD_CLR(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-SAL_SOCKET_OFFSET)/8] &= ~(1 << (((n)-SAL_SOCKET_OFFSET) & 7)))
#define FD_ISSET(n,p) FDSETSAFEGET(n, (p)->fd_bits[((n)-SAL_SOCKET_OFFSET)/8] &   (1 << (((n)-SAL_SOCKET_OFFSET) & 7)))
#define FD_ZERO(p)    memset((void*)(p), 0, sizeof(*(p)))

typedef struct fd_set {
    unsigned char fd_bits [(FD_SETSIZE * 2 + 7) / 8];
} fd_set;

#elif SAL_SOCKET_OFFSET
#error SAL_SOCKET_OFFSET does not work with external FD_SET!
#else
#include <fcntl.h>
#endif /* FD_SET */

/*
 * Options and types related to multicast membership
 */
#define IP_ADD_MEMBERSHIP  3
#define IP_DROP_MEMBERSHIP 4

#define IP_MULTICAST_TTL   5
#define IP_MULTICAST_IF    6
#define IP_MULTICAST_LOOP  7

typedef struct ip_mreq {
    struct in_addr imr_multiaddr; /* IP multicast address of group */
    struct in_addr imr_interface; /* local IP address of interface */
} ip_mreq;

/*
 * Option flags per-socket. These must match the SOF_ flags in ip.h (checked in init.c)
 */
#define SO_REUSEADDR   0x0004 /* Allow local address reuse */
#define SO_KEEPALIVE   0x0008 /* keep connections alive */
#define SO_BROADCAST   0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */

/*
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

int sal_select(int maxfdp1, fd_set *readset, fd_set *writeset,
               fd_set *exceptset, struct timeval *timeout);

int sal_socket(int domain, int type, int protocol);

int sal_write(int s, const void *data, size_t size);

int sal_connect(int s, const struct sockaddr *name, socklen_t namelen);

int sal_bind(int s, const struct sockaddr *name, socklen_t namelen);

int sal_eventfd(unsigned int initval, int flags);

int sal_setsockopt(int s, int level, int optname,
                   const void *optval, socklen_t optlen);

int sal_getsockopt(int s, int level, int optname,
                   void *optval, socklen_t *optlen);

struct hostent *sal_gethostbyname(const char *name);

int sal_close(int s);

int sal_init(void);

int sal_sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);

int sal_send(int s, const void *data, size_t size, int flags);

int sal_shutdown(int s, int how);

int sal_recvfrom(int s, void *mem, size_t len, int flags,
                 struct sockaddr *from, socklen_t *fromlen);

int sal_recv(int s, void *mem, size_t len, int flags);

int sal_read(int s, void *mem, size_t len);

void sal_freeaddrinfo(struct addrinfo *ai);

int sal_getaddrinfo(const char *nodename, const char *servname,
                    const struct addrinfo *hints, struct addrinfo **res);

void sal_freeaddrinfo(struct addrinfo *ai);

int sal_shutdown(int s, int how);

int sal_getaddrinfo(const char *nodename, const char *servname,
                    const struct addrinfo *hints, struct addrinfo **res);

int sal_fcntl(int s, int cmd, int val);
#define select(maxfdp1,readset,writeset,exceptset,timeout) \
        sal_select(maxfdp1,readset,writeset,exceptset,timeout)

#define write(s,data,size) \
        sal_write(s,data,size)

#define socket(domain,type,protocol) \
        sal_socket(domain,type,protocol)

#define connect(s,name,namelen) \
        sal_connect(s,name,namelen)

#define bind(s,name,namelen) \
        sal_bind(s,name,namelen)

#define shutdown(s,how) \
        sal_shutdown(s,how)

#define eventfd(initval,flags) \
        sal_eventfd(initval,flags)

#define setsockopt(s,level,optname,optval,optlen) \
        sal_setsockopt(s,level,optname,optval,optlen)

#define getsockopt(s,level,optname,optval,optlen) \
        sal_getsockopt(s,level,optname,optval,optlen)

#define gethostbyname(name) \
        sal_gethostbyname(name)

#define close(s) \
        sal_close(s)

#define sendto(s,dataptr,size,flags,to,tolen) \
        sal_sendto(s,dataptr,size,flags,to,tolen)

#define recvfrom(s,mem,len,flags,from,fromlen) \
            sal_recvfrom(s,mem,len,flags,from,fromlen)

#define send(s,data,size,flags) \
        sal_send(s,data,size,flags)

#define recv(s,data,size,flags) \
        sal_recv(s,data,size,flags)

#define read(s,data,size) \
        sal_read(s,data,size)

#define freeaddrinfo(addrinfo) sal_freeaddrinfo(addrinfo)

#define getaddrinfo(nodname, servname, hints, res) \
       sal_getaddrinfo(nodname, servname, hints, res)

#define fcntl(s,cmd,val)  sal_fcntl(s,cmd,val)

#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0)


#ifdef __cplusplus
}
#endif

#endif
