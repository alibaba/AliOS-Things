/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

//#include <ssv_ex_lib.h>
//#include <host_config.h>
#include "osal.h"
//#include <log.h>

#define CONFIG_MAX_NETIF        2
#define WLAN_MAX_STA            4
#define CONFIG_PLATFORM_CHECK   1
#define CONFIG_MEMP_DEBUG       0

/* 0 for minimal resources, 1 for double resource of setting 0 with higher performance, 2 for default, 3 for maximal resources */
/* RAM Usage:
   0: 46KB
   1: 77KB
   2: 99KB
   3: 177KB
*/
#define LWIP_PARAM_SET                  0
/***********************************************************/

/* Functions used to obtain and release exclusive access to the Tx buffer.  The
Get function will block if the Tx buffer is not available - use with care! */
//signed char *pcLwipBlockingGetTxBuffer( void );
//void vLwipAppsReleaseTxBuffer( void );
#ifdef WAC_EN
#define LWIP_IPV6                       (1)
#else
#define LWIP_IPV6                       (0)
#endif

#if LWIP_IPV6
#define DRV_TRX_HDR_LEN					(60)      //length should be divide by 4
#else
#define DRV_TRX_HDR_LEN					(40)      //length should be divide by 4
#endif

#if CONFIG_MEMP_DEBUG
#define STRUCT_PBUF_LEN                 (16+12)
#else
#define STRUCT_PBUF_LEN                 16
#endif


/* SSI options. */
#define TCPIP_THREAD_NAME              "tcpip_task"
#define LWIP_HTTPD_MAX_TAG_NAME_LEN 20
#define LWIP_HTTPD_MAX_TAG_INSERT_LEN 1024
#define TCPIP_THREAD_PRIO TCPIP_PRIORITY
#define TCPIP_THREAD_STACKSIZE TASK_IDLE_STK_SIZE * 16

#define DEFAULT_ACCEPTMBOX_SIZE         5
#define TCPIP_MBOX_SIZE			 		10//10

#define NO_SYS							0
#define LWIP_SOCKET						(NO_SYS==0)
#define LWIP_NETCONN              		1

#define LWIP_SNMP						0
#define LWIP_IGMP						1
#define LWIP_ICMP						1

/* Enable DNS so that we can use gethostbyname() API. */
#define LWIP_DNS						1
#define DNS_TABLE_SIZE                  2
/*
 *Modified by Zoe
 *Cause socket ip:127.0.0.1
 */
#ifdef WAC_EN
#define LWIP_HAVE_LOOPIF				1
#else
#define LWIP_HAVE_LOOPIF				0
#endif
#define TCP_LISTEN_BACKLOG				0
#define LWIP_SO_RCVTIMEO		   		1
#define LWIP_SO_SNDTIMEO		   		1
#define LWIP_SO_RCVBUF			 		1
//Remember to close
#define LWIP_DEBUG						0

#if LWIP_DEBUG

#define LWIP_DBG_MIN_LEVEL         LWIP_DBG_LEVEL_ALL



#define MEM_DEBUG                  LWIP_DBG_ON|LWIP_DBG_MOD_MEM
#define MEMP_DEBUG                 LWIP_DBG_ON|LWIP_DBG_MOD_MEM
#define PBUF_DEBUG                 LWIP_DBG_ON|LWIP_DBG_MOD_MEM

#define API_LIB_DEBUG              LWIP_DBG_ON|LWIP_DBG_MOD_L3_API
#define API_MSG_DEBUG              LWIP_DBG_ON|LWIP_DBG_MOD_L3_API
#define NETIF_DEBUG                LWIP_DBG_ON|LWIP_DBG_MOD_L3_API
#define SOCKETS_DEBUG              LWIP_DBG_ON|LWIP_DBG_MOD_L3_SOCKET

#define PPP_DEBUG                  LWIP_DBG_ON|LWIP_DBG_MOD_L3_OTHER_PROTO
#define DNS_DEBUG                  LWIP_DBG_ON|LWIP_DBG_MOD_L3_OTHER_PROTO
#define AUTOIP_DEBUG               LWIP_DBG_ON|LWIP_DBG_MOD_L3_OTHER_PROTO
#define DHCP_DEBUG                 LWIP_DBG_ON|LWIP_DBG_MOD_L3_OTHER_PROTO

#define IP_DEBUG                   LWIP_DBG_ON|LWIP_DBG_MOD_L3_IP
#define IP_REASS_DEBUG             LWIP_DBG_ON|LWIP_DBG_MOD_L3_IP
#define ICMP_DEBUG                 LWIP_DBG_ON|LWIP_DBG_MOD_L3_IP
#define IGMP_DEBUG                 LWIP_DBG_ON|LWIP_DBG_MOD_L3_IP

#define UDP_DEBUG                  LWIP_DBG_ON|LWIP_DBG_MOD_L3_UDP

#define TCPIP_DEBUG                LWIP_DBG_ON|LWIP_DBG_MOD_L3_API
#define TCP_DEBUG                  LWIP_DBG_ON|LWIP_DBG_MOD_L3_TCP
#define TCP_INPUT_DEBUG            LWIP_DBG_ON|LWIP_DBG_MOD_L3_TCP
#define TCP_OUTPUT_DEBUG           LWIP_DBG_ON|LWIP_DBG_MOD_L3_TCP
#define TCP_RTO_DEBUG              LWIP_DBG_ON|LWIP_DBG_LEVEL_WARNING|LWIP_DBG_MOD_L3_TCP
#define TCP_CWND_DEBUG             LWIP_DBG_ON|LWIP_DBG_MOD_L3_TCP
#define TCP_WND_DEBUG              LWIP_DBG_ON|LWIP_DBG_MOD_L3_TCP
#define TCP_FR_DEBUG               LWIP_DBG_ON|LWIP_DBG_LEVEL_WARNING|LWIP_DBG_MOD_L3_TCP
#define TCP_QLEN_DEBUG             LWIP_DBG_ON|LWIP_DBG_MOD_L3_TCP
#define TCP_RST_DEBUG              LWIP_DBG_ON|LWIP_DBG_MOD_L3_TCP
#endif

#define LWIP_DBG_TYPES_ON         (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)

#define CONFIG_LOG_MODULE				(LWIP_DBG_ALL_MODULES)
#define CONFIG_LOG_LEVEL				(LWIP_DBG_LEVEL_SEVERE)

#ifndef ETH_PAD_SIZE
#define ETH_PAD_SIZE           0
#endif

//Define the resource combination
#ifndef LWIP_PARAM_SET
#define LWIP_PARAM_SET                  0
#endif

/* ---------- Memory options ---------- */
#define MEM_LIBC_MALLOC         1
#define MEMP_MEM_MALLOC         1

/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
/* MSVC port: intel processors don't need 4-byte alignment,
   but are faster that way! */
#define MEM_ALIGNMENT			4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE				(1024*1024)//1073741824 1MB

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
//#define MEMP_NUM_PBUF			40//10

/* MEMP_NUM_RAW_PCB: the number of UDP protocol control blocks. One
   per active RAW "connection". */
#define LWIP_RAW				1		//enable raw connection
#define MEMP_NUM_RAW_PCB		4

/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB		8

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB		8

/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 8

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#if (LWIP_PARAM_SET == 0)
#define MEMP_NUM_TCP_SEG		32
#elif (LWIP_PARAM_SET == 1)
#define MEMP_NUM_TCP_SEG		8
#elif (LWIP_PARAM_SET == 2)
#define MEMP_NUM_TCP_SEG		36
#else
#define MEMP_NUM_TCP_SEG		88
#endif

/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT	15

/* The following four are used only with the sequential API and can be
   set to 0 if the application only will use the raw API. */
/* MEMP_NUM_NETBUF: the number of struct netbufs. */
#define MEMP_NUM_NETBUF      8

/* MEMP_NUM_NETCONN: the number of struct netconns. */
//#define MEMP_NUM_NETCONN        30

/* MEMP_NUM_TCPIP_MSG_*: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in
   src/api/tcpip.c. */
#define MEMP_NUM_TCPIP_MSG_API   5//4
#define MEMP_NUM_TCPIP_MSG_INPKT 10//4

#define MEMP_NUM_ARP_QUEUE		20//5

/* ---------- Pbuf options ---------- */


#define LWIP_NETIF_TX_SINGLE_PBUF   1

//#define CHECKSUM_CHECK_IP               0
//#define CHECKSUM_CHECK_UDP              1
//#define CHECKSUM_CHECK_TCP              0


/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#if (LWIP_PARAM_SET == 0)
#define PBUF_POOL_SIZE			32
#elif (LWIP_PARAM_SET == 1)
#define PBUF_POOL_SIZE			35
#elif (LWIP_PARAM_SET == 2)
#define PBUF_POOL_SIZE			42
#else
#define PBUF_POOL_SIZE			88
#endif

#define DEFAULT_TCP_RECVMBOX_SIZE       (PBUF_POOL_SIZE/2)
/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE		LWIP_MEM_ALIGN_SIZE(TCP_MSS+12+PBUF_LINK_HLEN+DRV_TRX_HDR_LEN+STRUCT_PBUF_LEN)
#define MAX_RECV_BUF	                           (TCP_MSS+40+PBUF_LINK_HLEN)


#define DEFAULT_RAW_RECVMBOX_SIZE    5
#define DEFAULT_UDP_RECVMBOX_SIZE    10


/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
#define PBUF_LINK_HLEN			16

#define MEMP_USE_CUSTOM_POOLS           0

/** SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT	(NO_SYS==0)


/* ---------- TCP options ---------- */
#define LWIP_TCP				1
#define TCP_TTL					255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ			1

/* TCP Maximum segment size. */
#define TCP_MSS					1460

#if ((LWIP_PARAM_SET == 0) || (LWIP_PARAM_SET == 1))
/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF				( TCP_MSS * 8 )
/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN		(2 * TCP_SND_BUF/TCP_MSS)
/* TCP receive window. */
#define TCP_WND					(TCP_MSS * 8)
/**
 * TCP_WND_UPDATE_THRESHOLD: difference in window to trigger an
 * explicit window update
 */
#define TCP_WND_UPDATE_THRESHOLD   (TCP_MSS * 4)
/**
 * TCP_SNDQUEUELOWAT: TCP writable bufs (pbuf count). This must be less
 * than TCP_SND_QUEUELEN. If the number of pbufs queued on a pcb drops below
 * this number, select returns writable (combined with TCP_SNDLOWAT).
 */
//#define TCP_SNDQUEUELOWAT  2
#elif (LWIP_PARAM_SET == 3)
#define TCP_SND_BUF				( TCP_MSS * 12 )
#define TCP_SND_QUEUELEN		(2 * TCP_SND_BUF/TCP_MSS)
#define TCP_WND					(TCP_MSS * 12)
#define TCP_WND_UPDATE_THRESHOLD   (TCP_MSS * 2)
#else
#define TCP_SND_BUF				( TCP_MSS * 12 )
#define TCP_SND_QUEUELEN		(3 * TCP_SND_BUF/TCP_MSS)
#define TCP_WND					(TCP_MSS * 8)
#define TCP_WND_UPDATE_THRESHOLD   (TCP_MSS * 2)
#endif

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT			(TCP_SND_BUF/2)

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX				12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX			4


/* ---------- ARP options ---------- */
#define LWIP_ARP				1
#define ARP_TABLE_SIZE			5
#define ARP_QUEUEING			1


/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD				1

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#define IP_REASSEMBLY			1
#define IP_REASS_MAX_PBUFS		5//10
#define MEMP_NUM_REASSDATA		5//10
#define IP_FRAG					0


/* ---------- ICMP options ---------- */
#define ICMP_TTL				255


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP				1

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK		(LWIP_DHCP)


/* ---------- AUTOIP options ------- */
#ifdef WAC_EN
#define LWIP_AUTOIP				1
#else
#define LWIP_AUTOIP				0
#endif
#define LWIP_DHCP_AUTOIP_COOP	(LWIP_DHCP && LWIP_AUTOIP)


/* ---------- UDP options ---------- */
#define LWIP_UDP				1
#define LWIP_UDPLITE			1
#define UDP_TTL					255


/* ---------- Statistics options ---------- */

#define LWIP_STATS				0
#define MEMP_OVERFLOW_CHECK         0
#define ASSERT_ERROR_WITH_MSG         0


#if LWIP_STATS    
    #define LINK_STATS				1
	#define IP_STATS				1
	#define ICMP_STATS				0
	#define IGMP_STATS				0
	#define IPFRAG_STATS			0
	#define UDP_STATS				1
	#define TCP_STATS				1
	#define MEM_STATS				1
	#define MEMP_STATS				1
	#define PBUF_STATS				1
	#define SYS_STATS				1
    #define LWIP_STATS_DISPLAY		1    
#endif /* LWIP_STATS */


/* ---------- PPP options ---------- */

#define PPP_SUPPORT			 0	  /* Set > 0 for PPP */

#if PPP_SUPPORT

	#define NUM_PPP					1	  /* Max PPP sessions. */

	/* Select modules to enable.  Ideally these would be set in the makefile but
	 * we're limited by the command line length so you need to modify the settings
	 * in this file.
	 */
	#define PPPOE_SUPPORT			1
	#define PPPOS_SUPPORT			1
	#define PAP_SUPPORT				1	  /* Set > 0 for PAP. */
	#define CHAP_SUPPORT			1	  /* Set > 0 for CHAP. */
	#define MSCHAP_SUPPORT			0	  /* Set > 0 for MSCHAP (NOT FUNCTIONAL!) */
	#define CBCP_SUPPORT			0	  /* Set > 0 for CBCP (NOT FUNCTIONAL!) */
	#define CCP_SUPPORT				0	  /* Set > 0 for CCP (NOT FUNCTIONAL!) */
	#define VJ_SUPPORT				1	  /* Set > 0 for VJ header compression. */
	#define MD5_SUPPORT				1	  /* Set > 0 for MD5 (see also CHAP) */

#endif /* PPP_SUPPORT */

#define HTTPD_SUPPORT 0
#define MULTICAST_IPv4_RX_SUPPORT 1

#if HTTPD_SUPPORT
#define HTTPD_USE_MEM_POOL 1
#endif

#define LWIP_NETIF_API  1

#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_REMOVE_CALLBACK      1

#endif /* __LWIPOPTS_H__ */
