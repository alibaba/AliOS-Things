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

#define LWIP_DBG_MIN_LEVEL 0
#define LWIP_COMPAT_SOCKETS 1
#define TAPIF_DEBUG LWIP_DBG_ON
#define TUNIF_DEBUG LWIP_DBG_OFF
#define UNIXIF_DEBUG LWIP_DBG_OFF
#define DELIF_DEBUG LWIP_DBG_OFF
#define SIO_FIFO_DEBUG LWIP_DBG_OFF
#define TCPDUMP_DEBUG LWIP_DBG_ON

#define PPP_DEBUG        LWIP_DBG_OFF
#define MEM_DEBUG        LWIP_DBG_OFF
#define MEMP_DEBUG       LWIP_DBG_OFF
#define PBUF_DEBUG       LWIP_DBG_OFF
#define API_LIB_DEBUG    LWIP_DBG_ON
#define API_MSG_DEBUG    LWIP_DBG_ON
#define TCPIP_DEBUG      LWIP_DBG_ON
#define NETIF_DEBUG      LWIP_DBG_ON
#define SOCKETS_DEBUG    LWIP_DBG_ON
#define DEMO_DEBUG       LWIP_DBG_ON
#define IP_DEBUG         LWIP_DBG_ON
#define IP_REASS_DEBUG   LWIP_DBG_ON
#define RAW_DEBUG        LWIP_DBG_ON
#define ICMP_DEBUG       LWIP_DBG_ON
#define UDP_DEBUG        LWIP_DBG_ON
#define TCP_DEBUG        LWIP_DBG_ON
#define TCP_INPUT_DEBUG  LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG LWIP_DBG_ON
#define TCP_RTO_DEBUG    LWIP_DBG_ON
#define TCP_CWND_DEBUG   LWIP_DBG_ON
#define TCP_WND_DEBUG    LWIP_DBG_ON
#define TCP_FR_DEBUG     LWIP_DBG_ON
#define TCP_QLEN_DEBUG   LWIP_DBG_ON
#define TCP_RST_DEBUG    LWIP_DBG_ON

extern unsigned char debug_flags;
#define LWIP_DBG_TYPES_ON debug_flags

#define NO_SYS                     0
#define LWIP_SOCKET                (NO_SYS==0)
#define LWIP_NETCONN               (NO_SYS==0)


/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
/* MSVC port: intel processors don't need 4-byte alignment,
   but are faster that way! */
#define MEM_ALIGNMENT           4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE               10240 

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           16
/* MEMP_NUM_RAW_PCB: the number of UDP protocol control blocks. One
   per active RAW "connection". */
#define MEMP_NUM_RAW_PCB        3
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        4
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        5
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 8
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        16
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT    3

/* The following four are used only with the sequential API and can be
   set to 0 if the application only will use the raw API. */
/* MEMP_NUM_NETBUF: the number of struct netbufs. */
#define MEMP_NUM_NETBUF         2
/* MEMP_NUM_NETCONN: the number of struct netconns. */
#define MEMP_NUM_NETCONN        10
/* MEMP_NUM_TCPIP_MSG_*: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in
   src/api/tcpip.c. */
#define MEMP_NUM_TCPIP_MSG_API   16
#define MEMP_NUM_TCPIP_MSG_INPKT 16

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          120

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       128

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
#define PBUF_LINK_HLEN          16 

/** SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT           1

/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define TCP_TTL                 255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 1024

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             2048

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN        (4 * TCP_SND_BUF/TCP_MSS)

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT		(TCP_SND_BUF/2)

/* TCP receive window. */
#define TCP_WND                 8096

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4

/* ---------- ARP options ---------- */
#define LWIP_ARP                1
#define ARP_TABLE_SIZE          10
#define ARP_QUEUEING            1

/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD              1


/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#define IP_REASSEMBLY     1
#define IP_REASS_MAX_PBUFS      10
#define MEMP_NUM_REASSDATA      10
#define IP_FRAG           1

/* ---------- ICMP options ---------- */
#define ICMP_TTL                255

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               0

/* 1 if you want to do an ARP check on the offered address
   (recommended if using DHCP). */
#define DHCP_DOES_ARP_CHECK     (LWIP_DHCP)

/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP             0

/* ---------- SNMP options ---------- */
/** @todo SNMP is experimental for now
    @note UDP must be available for SNMP transport */
#ifndef LWIP_SNMP
#define LWIP_SNMP               0
#endif

#ifndef SNMP_PRIVATE_MIB
#define SNMP_PRIVATE_MIB        0
#endif

/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define UDP_TTL                 255

/* ---------- RAW options ---------- */
#define LWIP_RAW                1
#define RAW_TTL                 255

/* ---------- Statistics options ---------- */
/* individual STATS options can be turned off by defining them to 0 
 * (e.g #define TCP_STATS 0). All of them are turned off if LWIP_STATS
 * is 0
 * */

#define LWIP_STATS	1

/* ---------- PPP options ---------- */

#define PPP_SUPPORT      0      /* Set > 0 for PPP */

#if PPP_SUPPORT > 0

#define NUM_PPP 1           /* Max PPP sessions. */


/* Select modules to enable.  Ideally these would be set in the makefile but
 * we're limited by the command line length so you need to modify the settings
 * in this file.
 */
#define PAP_SUPPORT      1      /* Set > 0 for PAP. */
#define CHAP_SUPPORT     1      /* Set > 0 for CHAP. */
#define MSCHAP_SUPPORT   0      /* Set > 0 for MSCHAP (NOT FUNCTIONAL!) */
#define CBCP_SUPPORT     0      /* Set > 0 for CBCP (NOT FUNCTIONAL!) */
#define CCP_SUPPORT      0      /* Set > 0 for CCP (NOT FUNCTIONAL!) */
#define VJ_SUPPORT       1      /* Set > 0 for VJ header compression. */
#define MD5_SUPPORT      1      /* Set > 0 for MD5 (see also CHAP) */


/*
 * Timeouts.
 */
#define FSM_DEFTIMEOUT		6	/* Timeout time in seconds */
#define FSM_DEFMAXTERMREQS	2	/* Maximum Terminate-Request transmissions */
#define FSM_DEFMAXCONFREQS	10	/* Maximum Configure-Request transmissions */
#define FSM_DEFMAXNAKLOOPS	5	/* Maximum number of nak loops */

#define UPAP_DEFTIMEOUT		6	/* Timeout (seconds) for retransmitting req */
#define UPAP_DEFREQTIME		30	/* Time to wait for auth-req from peer */

#define CHAP_DEFTIMEOUT		6	/* Timeout time in seconds */
#define CHAP_DEFTRANSMITS	10	/* max # times to send challenge */


/* Interval in seconds between keepalive echo requests, 0 to disable. */
#if 1
#define LCP_ECHOINTERVAL 0
#else
#define LCP_ECHOINTERVAL 10
#endif

/* Number of unanswered echo requests before failure. */
#define LCP_MAXECHOFAILS 3

/* Max Xmit idle time (in jiffies) before resend flag char. */
#define PPP_MAXIDLEFLAG 100

/*
 * Packet sizes
 *
 * Note - lcp shouldn't be allowed to negotiate stuff outside these
 *    limits.  See lcp.h in the pppd directory.
 * (XXX - these constants should simply be shared by lcp.c instead
 *    of living in lcp.h)
 */
#define PPP_MTU     1500     /* Default MTU (size of Info field) */
#if 0
#define PPP_MAXMTU  65535 - (PPP_HDRLEN + PPP_FCSLEN)
#else
#define PPP_MAXMTU  1500 /* Largest MTU we allow */
#endif
#define PPP_MINMTU  64
#define PPP_MRU     1500     /* default MRU = max length of info field */
#define PPP_MAXMRU  1500     /* Largest MRU we allow */
#define PPP_DEFMRU	296		/* Try for this */
#define PPP_MINMRU	128		/* No MRUs below this */


#define MAXNAMELEN      256     /* max length of hostname or name for auth */
#define MAXSECRETLEN    256     /* max length of password or secret */

#endif /* PPP_SUPPORT > 0 */

#endif /* __LWIPOPTS_H__ */
