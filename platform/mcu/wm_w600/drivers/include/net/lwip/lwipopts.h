/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
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
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "wm_config.h"
#include "wm_mem.h"


/**
 * Loopback demo related options.
 */
#define LWIP_NETIF_LOOPBACK             1
#define LWIP_HAVE_LOOPIF                1
#define LWIP_LOOPBACK_MAX_PBUFS         8

#define LWIP_TIMERS                     1
#define LWIP_TCPIP_TIMEOUT              1


#define TCPIP_THREAD_NAME               "tcp/ip"
#define TCPIP_THREAD_STACKSIZE          2048
#define TCPIP_THREAD_PRIO               14

#define DEFAULT_THREAD_STACKSIZE        200

/* Disable lwIP asserts */
#define LWIP_NOASSERT			1

#define LWIP_DEBUG                      0
#define SOCKETS_DEBUG                   0
#define LWIP_DEBUG_TRACE                0
#define PKTPRINT_DEBUG                  0
#define IPERF_DEBUG                     0
#define PING_DEBUG                      0
/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   4

#define MAX_SOCKETS_TCP 12
#define MAX_LISTENING_SOCKETS_TCP 4

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */

#define MEM_SIZE (20*1024)

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB                MAX_SOCKETS_TCP

#define MEMP_NUM_TCP_PCB_LISTEN         MAX_LISTENING_SOCKETS_TCP

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */

#define MEMP_NUM_TCPIP_MSG_INPKT        16
#define TCPIP_MBOX_SIZE                 32


/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */

#define MEMP_NUM_NETBUF                 16

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
#define LWIP_NETIF_API			1

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */
#define SO_REUSE                        1
#define SO_REUSE_RXTOALL 				1

/**
 * Enable TCP_KEEPALIVE
 */
#define LWIP_TCP_KEEPALIVE              1

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#define LWIP_STATS                      0


/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#define LWIP_DHCP                       1
#define LWIP_NETIF_STATUS_CALLBACK      1

/**
 * DNS related options, revisit later to fine tune.
 */
#define LWIP_DNS                       1
#define DNS_TABLE_SIZE                 4
#define DNS_MAX_SERVERS                2  // number of DNS servers, default 2
#define DNS_DOES_NAME_CHECK            1  // compare received name with given,def 0

#define MEMP_NUM_UDP_PCB		            8
/* NOTE: some times the socket() call for SOCK_DGRAM might fail if you dont
 * have enough MEMP_NUM_UDP_PCB */

/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#define LWIP_IGMP                       1

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#define LWIP_SO_SNDTIMEO                1

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1


#define LWIP_PROVIDE_ERRNO		1
#include <errno.h>
#define ERRNO				1

/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/
/**
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#define LWIP_NETIF_HOSTNAME             1


//#define TCP_MSS                 (1500 - 40)
#define TCP_MSS                 1024
/* TCP receive window. */
#define TCP_WND                 (6*TCP_MSS)
/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             (16*TCP_MSS)

/* ARP before DHCP causes multi-second delay  - turn it off */
#define DHCP_DOES_ARP_CHECK            (0)

#define MEM_LIBC_MALLOC                (1)
#define MEMP_MEM_MALLOC                (1)


#define DEFAULT_ACCEPTMBOX_SIZE         8
#define DEFAULT_RAW_RECVMBOX_SIZE       4
#define DEFAULT_UDP_RECVMBOX_SIZE       16
#define DEFAULT_TCP_RECVMBOX_SIZE       16


#define LWIP_IPV4    TLS_CONFIG_IPV4
#define LWIP_IPV6    TLS_CONFIG_IPV6

#define TCP_LISTEN_BACKLOG              1
#define TCP_DEFAULT_LISTEN_BACKLOG      8

#define ETHARP_SUPPORT_STATIC_ENTRIES   1

#define LWIP_TCPIP_CORE_LOCKING         0

#define MEMP_NUM_NETCONN                8

#endif /* __LWIPOPTS_H__ */

