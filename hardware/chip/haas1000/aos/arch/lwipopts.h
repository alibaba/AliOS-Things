/**
  ******************************************************************************
  * @file    lwipopts.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   lwIP Options Configuration.
  *          This file is based on Utilities\lwip-1.3.1\src\include\lwip\opt.h
  *          and contains the lwIP configuration for the STM32F107 demonstration.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__
#include "cmsis_os.h"

#define LWIP_NOASSERT			1

// move from opt.h for bes adapter
#define LWIP_TCPIP_CORE_LOCKING         1
#define MEMP_NUM_SELECT_CB              8
#define MEMP_NUM_TCPIP_MSG_API          16
#define ETH_PAD_SIZE                    2
#define LWIP_RAW                        1
#define LWIP_DNS                        1
#define SO_REUSE                        1
#define SO_REUSE_RXTOALL                1
#define SNTP_SERVER_DNS            	1
#define SNTP_STARTUP_DELAY          	0
#define SNTP_RECV_TIMEOUT           	5000


#define MAX_MSG_IN_LWIP_MBOX 50

/*----------------Thread Priority---------------------------------------------*/
//#define TCPIP_THREAD_PRIO	    	osPriorityHigh
//#define DEFAULT_THREAD_PRIO	    	osPriorityNormal
#define TCPIP_THREAD_PRIO	    	31
#define DEFAULT_THREAD_PRIO	    	32

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    1

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  0

/* ---------- Memory options ---------- */
#define MEM_LIBC_MALLOC                 1
extern void *rt_malloc(int size);
extern void *rt_calloc(int count, int size);
extern void rt_free(void *rmem);
#define mem_clib_free rt_free
#define mem_clib_malloc rt_malloc
#define mem_clib_calloc rt_calloc

#define MEMP_MEM_MALLOC                 1

#define LWIP_COMPAT_MUTEX_ALLOWED       (1)

/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE                (20*1024) // used for Lwip malloc

#define MAX_SOCKETS_TCP 12
#define MAX_LISTENING_SOCKETS_TCP 4
#define MAX_SOCKETS_UDP 22
#define TCP_SND_BUF_COUNT 5

#define TCP_MEM_SIZE     (MAX_SOCKETS_TCP * \
							PBUF_POOL_BUFSIZE * (TCP_SND_BUF/TCP_MSS))

/* Buffer size needed for UDP: Max. number of UDP sockets * Size of pbuf
 */
#define UDP_MEM_SIZE (MAX_SOCKETS_UDP * PBUF_POOL_BUFSIZE)

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           10
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        (MAX_SOCKETS_UDP + 2)
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        MAX_SOCKETS_TCP
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN MAX_LISTENING_SOCKETS_TCP
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts.
 * (requires NO_SYS==0)
 */
#define MEMP_NUM_SYS_TIMEOUT            12

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */

#define MEMP_NUM_NETBUF                 16

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 *
 * This number corresponds to the maximum number of active sockets at any
 * given point in time. This number must be sum of max. TCP sockets, max. TCP
 * sockets used for listening, and max. number of UDP sockets
 */
#define MEMP_NUM_NETCONN	(MAX_SOCKETS_TCP + \
	MAX_LISTENING_SOCKETS_TCP + MAX_SOCKETS_UDP)

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          30

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       1536

/* Enable IPv4 Auto IP	*/
#ifdef CONFIG_AUTOIP
#define LWIP_AUTOIP                     1
#define LWIP_DHCP_AUTOIP_COOP           1
#define LWIP_DHCP_AUTOIP_COOP_TRIES		5
#endif

#define DNS_TABLE_SIZE                 2  // number of table entries, default 4
//#define DNS_MAX_NAME_LENGTH            64  // max. name length, default 256
#define DNS_MAX_SERVERS                2  // number of DNS servers, default 2
#define DNS_DOES_NAME_CHECK            1  // compare received name with given,def 0
#define DNS_MSG_SIZE                   512
#define MDNS_MSG_SIZE                   512

#define MDNS_TABLE_SIZE                 1  // number of mDNS table entries
#define MDNS_MAX_SERVERS                1  // number of mDNS multicast addresses
/* TODO: Number of active UDP PCBs is equal to number of active UDP sockets plus
 * two. Need to find the users of these 2 PCBs
 */


/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define TCP_TTL                 255


#define TCPIP_THREAD_STACKSIZE                  25600 //8192

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#define MEMP_NUM_TCPIP_MSG_INPKT        64
#define TCPIP_MBOX_SIZE                 64

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE         8
#define DEFAULT_RAW_RECVMBOX_SIZE       4
#define DEFAULT_TCP_RECVMBOX_SIZE       20

/* TCP Maximum segment size. */
#define TCP_MSS                 (1500 - 40)      /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             (44*TCP_MSS)

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN        (4* TCP_SND_BUF/TCP_MSS)

/* TCP receive window. */
#define TCP_WND                 (10*TCP_MSS)

/**
 * Enable TCP_KEEPALIVE
 */
#define LWIP_TCP_KEEPALIVE              1

/**
 * TCP_SYNMAXRTX: Maximum number of retransmissions of SYN segments.
 */
#define TCP_SYNMAXRTX                   10

#define TCP_MAX_ACCEPT_CONN 5
#define MEMP_NUM_TCP_SEG               (TCP_SND_QUEUELEN*2)

#define IP_REASS_MAX_PBUFS              0
#define IP_REASSEMBLY                   0
#define IP_REASS_MAX_PBUFS              0
#define IP_REASSEMBLY                   0
#define MEMP_NUM_REASSDATA              0
#define IP_FRAG                         0

/* ---------- ICMP options ---------- */
#define LWIP_ICMP                       1


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#define LWIP_DHCP               1
#define DHCP_DOES_ARP_CHECK     0


/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define UDP_TTL                 255
#define DEFAULT_UDP_RECVMBOX_SIZE       MAX_MSG_IN_LWIP_MBOX

/* ---------- Statistics options ---------- */
#define LWIP_STATS 0

#define LWIP_IGMP 1
#if LWIP_IGMP
#ifndef LWIP_RAND
#define LWIP_RAND() ((u32_t)aos_rand())
#endif
#endif

#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 1

#define IP_FORWARD 0

#define LWIP_NETIF_STATUS_CALLBACK	1

#include <errno.h>
#define ERRNO				1

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

/*
The STM32F107 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    1
/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     1
#define LWIP_NETIF_API			1

/**
 * LWIP_RECV_CB==1: Enable callback when a socket receives data.
 */
#define LWIP_RECV_CB                1
/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */
#define SO_REUSE                        1
#define SO_REUSE_RXTOALL 				1
#define LWIP_SO_RCVTIMEO                1
#define LWIP_SO_SNDTIMEO                1

#define LWIP_SO_RCVBUF 1
#define LWIP_SO_RCVTCPBUF 1

#define TCP_LISTEN_BACKLOG		1

#define ARP_QUEUEING                    1
#define LWIP_DHCP_MAX_NTP_SERVERS       3

#if defined(LWIP_FULLDUPLEX_SUPPORT)
//#define LWIP_NETCONN_FULLDUPLEX 1
#define LWIP_NETCONN_SEM_PER_THREAD 1
#endif

//#define LWIP_PROVIDE_ERRNO 1
#define LWIP_MAILBOX_QUEUE  1
#define LWIP_TIMEVAL_PRIVATE 0

// add for alios things compatible
#define LWIP_IPV6                       0

#ifdef CONFIG_AOS_MESH
#define LWIP_DECLARE_HOOK \
    struct netif *lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest); \
    int lwip_hook_mesh_is_mcast_subscribed(const ip6_addr_t *dest);
#define LWIP_HOOK_IP6_ROUTE(src, dest)           lwip_hook_ip6_route(src, dest)
#define LWIP_HOOK_MESH_IS_MCAST_SUBSCRIBED(dest) lwip_hook_mesh_is_mcast_subscribed(dest)

#define LWIP_ICMP6                      1
#define CHECKSUM_CHECK_ICMP6            0
#define LWIP_MULTICAST_PING             1

#endif

/*
   ------------------------------------
   --------- customize macros ---------
   ------------------------------------
*/
/**
 * LWIP_THREAD_EXIT_HOOK==1: bes add for thread exit proc
 */
#define LWIP_THREAD_EXIT_HOOK			1
/**
 * LWIP_NTP_HOOK==1: bes add for ntp maintain
 */
#define LWIP_NTP_HOOK					1
/**
 * DHCP_REBIND_PRE_ADDR: bes add for DHCP
 */
/* uncommented below define to try to rebind last offerred address when link get reconnected*/
//#define DHCP_REBIND_PRE_ADDR
#ifdef DHCP_REBIND_PRE_ADDR
/* dhcp retry interval ms */
#define DHCP_RETRY_INTERVAL	500
#endif
/**
 * LWIP_ARP_ENHANCEMENT==1: bes add for arp
 */
#define LWIP_ARP_ENHANCEMENT			1
/**
 * LWIP_NET_STATUS==0: bes add for net flow customize
 */
#define LWIP_NET_STATUS					0
/**
 * LWIP_PBUF_NEW_API==1: bes add new PBUF api
 */
#define LWIP_PBUF_NEW_API				1
/**
 * LWIP_PBUF_NEW_MEMCPY==1: bes add for PBUF use mymemcpy
 */
#define LWIP_PBUF_NEW_MEMCPY			1
/**
 * LWIP_IN_ADDR_TYPE_CSTM==1: bes add for in_addr use u32_t s_addr
 */
#define LWIP_IN_ADDR_TYPE_CSTM			1

/*
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#define LWIP_NETIF_HOSTNAME             1

/**
 * Loopback demo related options.
 */
#define LWIP_NETIF_LOOPBACK             1
#define LWIP_HAVE_LOOPIF                1
#define LWIP_NETIF_LOOPBACK_MULTITHREADING       1
#define LWIP_LOOPBACK_MAX_PBUFS         8

#define LWIP_TIMERS                     1
#define LWIP_TCPIP_TIMEOUT              1

/**
 * TCP_RESOURCE_FAIL_RETRY_LIMIT: limit for retrying sending of tcp segment
 * on resource failure error returned by driver.
 */
#define TCP_RESOURCE_FAIL_RETRY_LIMIT     50

/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/
#define LWIP_DEBUG

#define DNS_DEBUG                       LWIP_DBG_OFF
#define PKTPRINT_DEBUG                  LWIP_DBG_ON
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define REENTER_DEBUG                   LWIP_DBG_OFF
#define ARP_DEBUG                       LWIP_DBG_OFF
#define DNSCLI_DEBUG                    LWIP_DBG_ON
#define IPERF_DEBUG                     LWIP_DBG_ON
#define PING_DEBUG                      LWIP_DBG_ON
#define IFCONFIG_DEBUG                  LWIP_DBG_ON
#define SOCKET_ALLOC_DEBUG              LWIP_DBG_OFF

#endif /* __LWIPOPTS_H__ */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
