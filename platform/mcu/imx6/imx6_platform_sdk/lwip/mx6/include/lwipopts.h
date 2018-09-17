//
// lwipopts.h
//
// Contains preprocessor definitions to guide a build of the lwIP code.
//
#ifndef _LWIPOPTS_H_
#define _LWIPOPTS_H_

#define NO_SYS (1)

/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/
#define SYS_LIGHTWEIGHT_PROT			(!NO_SYS)

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/

// Use C library malloc() for all memory.
#define MEM_LIBC_MALLOC 1
#define MEMP_MEM_MALLOC 1

#define MEM_ALIGNMENT					4
#define MEM_SIZE						(128 * 1024)

#define MEMP_SEPARATE_POOLS				1		// for PBUF_POOL alignment to cache line size

#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
#define MEMP_NUM_PBUF                   16
#define MEMP_NUM_RAW_PCB				32
#define MEMP_NUM_UDP_PCB				8
#define MEMP_NUM_TCP_PCB				32
#define MEMP_NUM_TCP_PCB_LISTEN			8
#define MEMP_NUM_TCP_SEG				32
#define MEMP_NUM_IGMP_GROUP				8
#define MEMP_NUM_SYS_TIMEOUT			10
#define MEMP_NUM_NETBUF					32
#define MEMP_NUM_NETCONN				48
#define MEMP_NUM_TCPIP_MSG_API          8
#define MEMP_NUM_TCPIP_MSG_INPKT        0
#define MEMP_NUM_SNMP_VARBIND           (2*3)
#define MEMP_NUM_SNMP_VALUE             (3*3)
#define MEMP_NUM_NETDB                  8
#define PBUF_POOL_SIZE					32

/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
*/
#define ETH_PAD_SIZE					2
#define LWIP_ARP						1
#define ARP_TABLE_SIZE					30
#define ARP_QUEUEING					1
#define ETHARP_TRUST_IP_MAC				1
#define ETHARP_SUPPORT_VLAN				0

/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
#define IP_FORWARD						0
#define IP_OPTIONS_ALLOWED				1
#define IP_REASSEMBLY					0
#define IP_FRAG							0

#define IP_DEFAULT_TTL					255

/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
#define LWIP_ICMP						1
#define LWIP_BROADCAST_PING				0
#define LWIP_MULTICAST_PING				0

/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
#define LWIP_RAW						1

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
#define LWIP_DHCP						1

/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
#define LWIP_AUTOIP						1
#define LWIP_DHCP_AUTOIP_COOP			1

/*
   ----------------------------------
   ---------- SNMP options ----------
   ----------------------------------
*/
#define LWIP_SNMP						1

/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
#define LWIP_IGMP						1

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
#define LWIP_DNS						1

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
#define LWIP_UDP						1
#define LWIP_UDPLITE					0

#define LWIP_NETBUF_RECVINFO			0

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
#define LWIP_TCP						1
#define LWIP_TCP_TIMESTAMPS				0

#define TCP_WND							8192
#define TCP_MAXRTX						10
#define TCP_SYNMAXRTX					3
#define TCP_QUEUE_OOSEQ					1
#define TCP_MSS							1460
#define TCP_SND_BUF						(8 * TCP_MSS)
#define TCP_SND_QUEUELEN				(2 * TCP_SND_BUF/TCP_MSS)
#define TCP_SNDLOWAT					(TCP_SND_BUF/2)
#define TCP_LISTEN_BACKLOG				1

/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
#define PBUF_POOL_BUFSIZE				1520
#define PBUF_LINK_HLEN					(14 + ETH_PAD_SIZE)

/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/
#define LWIP_NETIF_HOSTNAME				1
#define LWIP_NETIF_API					(!NO_SYS)
#define LWIP_NETIF_HWADDRHINT			1
#define LWIP_NETIF_LOOPBACK				1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK		1

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
#define LWIP_HAVE_LOOPIF				0

/*
   ------------------------------------
   ---------- Thread options ----------
   ------------------------------------
*/
#define TCPIP_MBOX_SIZE					32
#define DEFAULT_UDP_RECVMBOX_SIZE		32
#define DEFAULT_TCP_RECVMBOX_SIZE		32
#define DEFAULT_ACCEPTMBOX_SIZE			4

#define TCPIP_THREAD_PRIO				5
#define DEFAULT_THREAD_PRIO				10
#define LOW_THREAD_PRIO					29

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
#define LWIP_NETCONN					(!NO_SYS)

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
#define LWIP_SOCKET						(!NO_SYS)
#define LWIP_COMPAT_SOCKETS				1
#define LWIP_TCP_KEEPALIVE				1
#define LWIP_SO_RCVTIMEO				1
#define LWIP_SO_RCVBUF					0
#define SO_REUSE						1
#define SO_REUSE_RXTOALL				1

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
#define LWIP_STATS						1
#define LWIP_STATS_LARGE				1

#if LWIP_STATS
#define LINK_STATS						1
#define IP_STATS						1
#define IPFRAG_STATS					0
#define ICMP_STATS						1
#define IGMP_STATS						1
#define UDP_STATS						1
#define TCP_STATS						1
#define MEM_STATS						1
#define MEMP_STATS						1
#define SYS_STATS						1
#define LWIP_STATS_DISPLAY				1
#endif /* STATS */


#define LWIP_PROVIDE_ERRNO				1


/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/

#if DEBUG
#define LWIP_DEBUG                      (1)
#endif

#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON				LWIP_DBG_STATE
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF
#define PBUF_DEBUG                      LWIP_DBG_OFF
#define API_LIB_DEBUG                   LWIP_DBG_OFF
#define API_MSG_DEBUG                   LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define IGMP_DEBUG                      LWIP_DBG_OFF
#define INET_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                        LWIP_DBG_OFF
#define IP_REASS_DEBUG                  LWIP_DBG_OFF
#define RAW_DEBUG                       LWIP_DBG_OFF
#define MEM_DEBUG                       LWIP_DBG_OFF
#define MEMP_DEBUG                      LWIP_DBG_OFF
#define SYS_DEBUG                       LWIP_DBG_OFF
#define TIMERS_DEBUG                    LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
#define TCP_RST_DEBUG                   LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
#define UDP_DEBUG                       LWIP_DBG_OFF
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#define PPP_DEBUG                       LWIP_DBG_OFF
#define SLIP_DEBUG                      LWIP_DBG_OFF
#define DHCP_DEBUG                      LWIP_DBG_ON
#define AUTOIP_DEBUG                    LWIP_DBG_ON
#define SNMP_MSG_DEBUG                  LWIP_DBG_OFF
#define SNMP_MIB_DEBUG                  LWIP_DBG_OFF
#define DNS_DEBUG                       LWIP_DBG_OFF

#endif // _LWIPOPTS_H_
