/**
 * @file
 *
 * lwIP Options Configuration
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
#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

#include "lwip/arch.h"
#define LWIP_NETIF_API 1

#define LWIP_PRIVATE_FD_SET
/**
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */

#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 1
/*
   -------------- NO SYS --------------
*/
#define NO_SYS                          0
#define SYS_LIGHTWEIGHT_PROT            (NO_SYS == 0)

#ifndef LWIP_NETIF_API
#define LWIP_NETIF_API 1
#endif
/*
   ---------- Memory options ----------
*/
#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        (3*1024)  //(10*1024)

#define MEM_LIBC_MALLOC                 1
#if MEM_LIBC_MALLOC
#include <aos/kernel.h>
#define mem_clib_malloc aos_malloc
#define mem_clib_free aos_free
#define mem_clib_calloc(n, m) aos_zalloc( (n) * (m) )
#endif

#define MEMP_MEM_MALLOC                 1
#define MEMP_OVERFLOW_CHECK             1


/*
   ---------- Internal Memory Pool Sizes ----------
*/
#define MEMP_NUM_PBUF                   8
#define MEMP_NUM_RAW_PCB                8
#define MEMP_NUM_UDP_PCB                8
#define MEMP_NUM_TCP_PCB                8
#define MEMP_NUM_TCP_PCB_LISTEN         8
#define MEMP_NUM_TCP_SEG                12
#define MEMP_NUM_REASSDATA              4
#define MEMP_NUM_FRAG_PBUF              8
#define MEMP_NUM_ARP_QUEUE              8
#define MEMP_NUM_NETBUF                 8
#define MEMP_NUM_NETCONN                10
#define MEMP_NUM_TCPIP_MSG_API          8
#define MEMP_NUM_TCPIP_MSG_INPKT        12
#define PBUF_POOL_SIZE                  24

/*
   ---------- ARP options ----------
*/
#define LWIP_ARP                        1

/*
   ---------- IP options ----------
*/
#define LWIP_IPV4                       1
#define LWIP_IPV6                       0
#define IP_FORWARD                      0
#define IP_OPTIONS_ALLOWED              1
#define IP_REASSEMBLY                   0
#define IP_FRAG                         0
#define IP_REASS_MAXAGE                 3
#define IP_REASS_MAX_PBUFS              4
#define IP_FRAG_USES_STATIC_BUF         0
#define IP_DEFAULT_TTL                  255

/*
   ---------- ICMP options ----------
*/
#define LWIP_ICMP                       1
#define LWIP_ICMP6                      1
#define CHECKSUM_CHECK_ICMP6            0
#define LWIP_MULTICAST_PING             1

/*
   ---------- RAW options ----------
*/
#define LWIP_RAW                        1

/*
   ---------- DHCP options ----------
*/
#define LWIP_DHCP                       1
#define LWIP_NETIF_STATUS_CALLBACK      1

/*
   ---------- AUTOIP options ----------
*/
#define LWIP_AUTOIP                     0

/*
   ---------- SNMP options ----------
*/
#define LWIP_SNMP                       0

/*
   ---------- IGMP options ----------
*/
#define LWIP_IGMP                       1

/*
   ---------- DNS options -----------
*/
#define LWIP_DNS                        1

/*
   ---------- UDP options ----------
*/
#define LWIP_UDP                        1

/*
   ---------- TCP options ----------
*/
#define LWIP_TCP                        1
#define LWIP_LISTEN_BACKLOG             0
#define TCP_MSS                         1460   //1792
#define TCP_WND                         (5 * TCP_MSS)
#define TCP_SND_BUF                     (10 * TCP_MSS)
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   12

/*
   ---------- Pbuf options ----------
*/
#define PBUF_LINK_HLEN                  16

//#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)
#define PBUF_POOL_BUFSIZE               512  //1500

/*
   ---------- Network Interfaces options ----------
*/
#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  #define CHECKSUM_GEN_ICMP               0
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
   ---------- LOOPIF options ----------
*/
#define LWIP_NETIF_LOOPBACK             1
#define LWIP_HAVE_LOOPIF                1
#define LWIP_NETIF_LOOPBACK_MULTITHREADING       1
#define LWIP_LOOPBACK_MAX_PBUFS         8

/*
   ---------- Thread options ----------
*/
#define TCPIP_MBOX_SIZE                 60
#define DEFAULT_ACCEPTMBOX_SIZE         10
#define DEFAULT_RAW_RECVMBOX_SIZE       10
#define DEFAULT_UDP_RECVMBOX_SIZE       20
#define DEFAULT_TCP_RECVMBOX_SIZE       10

#define LWIP_TCPIP_CORE_LOCKING         1
#define LWIP_TCPIP_CORE_LOCKING_INPUT   1

#define ETHIF_IN_TASK_STACKSIZE         512 /* unit 4 byte */
#define ETHIF_IN_TASK_PRIO              10
#define TCPIP_THREAD_STACKSIZE          2048//10240/* unit 4 byte */
#define TCPIP_THREAD_PRIO               21//0

/*
   ---------- Sequential layer options ----------
*/

#define LWIP_NETCONN                    8

/*
   ---------- Socket options ----------
*/
#define LWIP_SOCKET                     1
#define LWIP_COMPAT_SOCKETS             1
#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#if !defined(FD_SET) && defined(RHINO_CONFIG_VFS_DEV_NODES)
#define LWIP_SOCKET_OFFSET              RHINO_CONFIG_VFS_DEV_NODES
#endif
#define LWIP_SO_SNDTIMEO                1
#define LWIP_SO_RCVTIMEO                1
#define SO_REUSE                        1

/*
   ---------- Statistics options ----------
*/
#define LWIP_STATS                      1
#define LWIP_STATS_DISPLAY              1

/*
   ---------- Checksum options ----------
*/

/*
   ---------- IPv6 options ---------------
*/

/*
   ---------- Hook options ---------------
*/

#ifdef CONFIG_AOS_MESH
#define LWIP_DECLARE_HOOK \
    struct netif *lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest); \
    int lwip_hook_mesh_is_mcast_subscribed(const ip6_addr_t *dest);
#define LWIP_HOOK_IP6_ROUTE(src, dest)           lwip_hook_ip6_route(src, dest)
#define LWIP_HOOK_MESH_IS_MCAST_SUBSCRIBED(dest) lwip_hook_mesh_is_mcast_subscribed(dest)
#endif

/*
   ---------- Debugging options ----------
*/
//#define LWIP_DEBUG 1

#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON               (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)

#define MEM_DEBUG                       LWIP_DBG_OFF
#define MEMP_DEBUG                      LWIP_DBG_OFF
#define PBUF_DEBUG                      LWIP_DBG_OFF
#define API_LIB_DEBUG                   LWIP_DBG_ON
#define API_MSG_DEBUG                   LWIP_DBG_ON
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_ON
#define IP_DEBUG                        LWIP_DBG_OFF
#define IP_REASS_DEBUG                  LWIP_DBG_OFF
#define RAW_DEBUG                       LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define UDP_DEBUG                       LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
#define TCP_RST_DEBUG                   LWIP_DBG_OFF

/*
   ---------- Performance tracking options ----------
*/

/*
   ---------- PPP options ----------
*/
#define PPP_SUPPORT                     0

#endif /* LWIP_LWIPOPTS_H */
