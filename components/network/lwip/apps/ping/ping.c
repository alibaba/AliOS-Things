/**
 * @file
 * Ping sender module
 *
 */

/*
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
 */

/**
 * This is an example of a "ping" sender (with raw API and socket API).
 * It can be used as a start point to maintain opened a network connection, or
 * like a network "watchdog" for your device.
 *
 */

#include "lwip/opt.h"

#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include "lwip/apps/ping.h"

#include <stdlib.h>
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip4.h"
#include "lwip/sockets.h"

#if PING_USE_SOCKETS
#include "lwip/inet.h"
#include <string.h>
#endif /* PING_USE_SOCKETS */

#define LWIP_IPV6 0

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 1000
#endif

/** ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/** ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

#ifndef PING_STACK_SIZE
#define PING_STACK_SIZE   2048
#endif

/* ping variables */
static int ping_send_count = -1;
static int ping_rcv_timeo = PING_RCV_TIMEO;
static int ping_delay = PING_DELAY;
static int ping_data_size = PING_DATA_SIZE;
static const ip_addr_t* ping_target;
static ip_addr_t addr;
static u16_t ping_seq_num;
#ifdef LWIP_DEBUG
static u32_t ping_time;
#endif /* LWIP_DEBUG */
#if !PING_USE_SOCKETS
static struct raw_pcb *ping_pcb;
#endif /* PING_USE_SOCKETS */
#if PING_USE_SOCKETS
static char ping_started = 0;
static char ping_do_exit = 0;
#endif
/** Prepare a echo ICMP request */
static void
ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);

  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id     = PING_ID;
  iecho->seqno  = lwip_htons(++ping_seq_num);

  /* fill the additional data buffer with some data */
  for(i = 0; i < data_len; i++) {
    ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

  iecho->chksum = inet_chksum(iecho, len);
}

#if PING_USE_SOCKETS

/* Ping using the socket ip */
static err_t
ping_send(int s, const ip_addr_t *addr)
{
  int err;
  struct icmp_echo_hdr *iecho;
  struct sockaddr_storage to;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + ping_data_size;
  LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

#if LWIP_IPV6
  if(IP_IS_V6(addr) && !ip6_addr_isipv6mappedipv4(ip_2_ip6(addr))) {
    /* todo: support ICMP6 echo */
    return ERR_VAL;
  }
#endif /* LWIP_IPV6 */

  iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
  if (!iecho) {
    return ERR_MEM;
  }

  ping_prepare_echo(iecho, (u16_t)ping_size);

#if LWIP_IPV4
  if(IP_IS_V4(addr)) {
    struct sockaddr_in *to4 = (struct sockaddr_in*)&to;
    to4->sin_len    = sizeof(to4);
    to4->sin_family = AF_INET;
    inet_addr_from_ipaddr(&to4->sin_addr, ip_2_ip4(addr));
  }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  if(IP_IS_V6(addr)) {
    struct sockaddr_in6 *to6 = (struct sockaddr_in6*)&to;
    to6->sin6_len    = sizeof(to6);
    to6->sin6_family = AF_INET6;
    inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(addr));
  }
#endif /* LWIP_IPV6 */

  err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));
  mem_free(iecho);

  return (err ? ERR_OK : ERR_VAL);
}

static void
ping_recv(int s)
{
  char buf[64];
  int len;
  struct sockaddr_storage from;
  int fromlen = sizeof(from);

  while((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {
    if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) {
      ip_addr_t fromaddr;
      memset(&fromaddr, 0, sizeof(fromaddr));

#if LWIP_IPV4
      if(from.ss_family == AF_INET) {
        struct sockaddr_in *from4 = (struct sockaddr_in*)&from;
        inet_addr_to_ipaddr(ip_2_ip4(&fromaddr), &from4->sin_addr);
        IP_SET_TYPE_VAL(fromaddr, IPADDR_TYPE_V4);
      }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
      if(from.ss_family == AF_INET6) {
        struct sockaddr_in6 *from6 = (struct sockaddr_in6*)&from;
        inet6_addr_to_ip6addr(ip_2_ip6(&fromaddr), &from6->sin6_addr);
        IP_SET_TYPE_VAL(fromaddr, IPADDR_TYPE_V6);
      }
#endif /* LWIP_IPV6 */

      LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
      ip_addr_debug_print_val(PING_DEBUG, fromaddr);
      LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms\n", (sys_now() - ping_time)));

      /* todo: support ICMP6 echo */
#if LWIP_IPV4
      if (IP_IS_V4_VAL(fromaddr)) {
        struct ip_hdr *iphdr;
        struct icmp_echo_hdr *iecho;

        iphdr = (struct ip_hdr *)buf;
        iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
        if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_seq_num))) {
          /* do some ping result processing */
          PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
          return;
        } else {
          LWIP_DEBUGF( PING_DEBUG, ("ping: drop\n"));
        }
      }
#endif /* LWIP_IPV4 */
    }
    fromlen = sizeof(from);
  }

  if (len == 0) {
    LWIP_DEBUGF( PING_DEBUG, ("ping: recv - %"U32_F" ms - timeout\n", (sys_now()-ping_time)));
  }

  /* do some ping result processing */
  PING_RESULT(0);
}

static void
ping_thread(void *arg)
{
  int s;
  int ret;
  int count = ping_send_count;

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
  int timeout = ping_rcv_timeo;
#else
  struct timeval timeout;
  timeout.tv_sec = ping_rcv_timeo/1000;
  timeout.tv_usec = (ping_rcv_timeo%1000)*1000;
#endif
  LWIP_UNUSED_ARG(arg);
#if LWIP_IPV6
  if(IP_IS_V4(ping_target) || ip6_addr_isipv6mappedipv4(ip_2_ip6(ping_target))) {
    s = lwip_socket(AF_INET6, SOCK_RAW, IP_PROTO_ICMP);
  } else {
    s = lwip_socket(AF_INET6, SOCK_RAW, IP6_NEXTH_ICMP6);
  }
#else
  s = lwip_socket(AF_INET,  SOCK_RAW, IP_PROTO_ICMP);
#endif
  if (s < 0) {
    LWIP_DEBUGF( PING_DEBUG, ("ping: create socket failed"));
    goto exit;
  }

  ret = lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  LWIP_ASSERT("setting receive timeout failed", ret == 0);
  LWIP_UNUSED_ARG(ret);

  while (1){
    if(count >= 0) {
        if(count == 0) {
            LWIP_DEBUGF( PING_DEBUG, ("ping: send %"U32_F"times finished", ping_send_count));
            goto exit;
        }
        count --;
    }
    if(ping_do_exit) {
        LWIP_DEBUGF( PING_DEBUG, ("ping: recv ping exit command"));
        goto exit;
    }
    if (ping_send(s, ping_target) == ERR_OK) {
      LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
      ip_addr_debug_print(PING_DEBUG, ping_target);
      LWIP_DEBUGF( PING_DEBUG, ("\n"));

#ifdef LWIP_DEBUG
      ping_time = sys_now();
#endif /* LWIP_DEBUG */
      ping_recv(s);
    } else {
      LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
      ip_addr_debug_print(PING_DEBUG, ping_target);
      LWIP_DEBUGF( PING_DEBUG, (" - error\n"));
    }
    sys_msleep(ping_delay);
  }
exit:
    lwip_close(s);
    ping_started = 0;
    ping_do_exit = 0;
    aos_task_exit(0);
}

#else /* PING_USE_SOCKETS */

/* Ping using the raw ip */
static u8_t
ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
  struct icmp_echo_hdr *iecho;
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);
  LWIP_ASSERT("p != NULL", p != NULL);

  if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) &&
      pbuf_remove_header(p, PBUF_IP_HLEN) == 0) {
    iecho = (struct icmp_echo_hdr *)p->payload;

    if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_seq_num))) {
      LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
      ip_addr_debug_print(PING_DEBUG, addr);
      LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms\n", (sys_now()-ping_time)));

      /* do some ping result processing */
      PING_RESULT(1);
      pbuf_free(p);
      return 1; /* eat the packet */
    }
    /* not eaten, restore original packet */
    pbuf_add_header(p, PBUF_IP_HLEN);
  }

  return 0; /* don't eat the packet */
}

static void
ping_send(struct raw_pcb *raw, const ip_addr_t *addr)
{
  struct pbuf *p;
  struct icmp_echo_hdr *iecho;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + ping_data_size;

  LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
  ip_addr_debug_print(PING_DEBUG, addr);
  LWIP_DEBUGF( PING_DEBUG, ("\n"));
  LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

  p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
  if (!p) {
    return;
  }
  if ((p->len == p->tot_len) && (p->next == NULL)) {
    iecho = (struct icmp_echo_hdr *)p->payload;

    ping_prepare_echo(iecho, (u16_t)ping_size);

    raw_sendto(raw, p, addr);
#ifdef LWIP_DEBUG
    ping_time = sys_now();
#endif /* LWIP_DEBUG */
  }
  pbuf_free(p);
}

static void
ping_timeout(void *arg)
{
  struct raw_pcb *pcb = (struct raw_pcb*)arg;

  LWIP_ASSERT("ping_timeout: no pcb given!", pcb != NULL);

  ping_send(pcb, ping_target);

  sys_timeout(ping_delay, ping_timeout, pcb);
}

static void
ping_raw_init(void)
{
  ping_pcb = raw_new(IP_PROTO_ICMP);
  LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);

  raw_recv(ping_pcb, ping_recv, NULL);
  raw_bind(ping_pcb, IP_ADDR_ANY);
  sys_timeout(ping_delay, ping_timeout, ping_pcb);
}

void
ping_send_now(void)
{
  LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);
  ping_send(ping_pcb, ping_target);
}

#endif /* PING_USE_SOCKETS */

void
ping_init(const ip_addr_t* ping_addr)
{
  ping_target = ping_addr;

#if PING_USE_SOCKETS
  if(!ping_started) {
      ping_started = 1;
      aos_task_new("ping_thread", ping_thread, NULL, PING_STACK_SIZE);
  }
  else {
      LWIP_DEBUGF( PING_DEBUG, ("ping: ping task is already running\n"));
      LWIP_DEBUGF( PING_DEBUG, ("ping: use command \"ping -e\" to exit running ping task\n"));
      LWIP_DEBUGF( PING_DEBUG, ("ping: and then run Ping command again\n"));
  }
#else /* PING_USE_SOCKETS */
  ping_raw_init();
#endif /* PING_USE_SOCKETS */
}

void ping_run( int argc, char **argv )
{
    int i;
    int temp;
    struct addrinfo hints, *res;

    for(i = 0; i < argc; i++) {
        if ( strcmp( (char *) argv[i], "-c" ) == 0 ) {
            i++;
            if((i < argc) && ((temp = atoi(argv[i])) > 0)) {
                ping_send_count = temp;
            }
            else {
                LWIP_DEBUGF( PING_DEBUG, ("ping: bad number of packets to transmit\n"));
                return;
            }
        }
        else if ( strcmp( (char *) argv[i], "-i" ) == 0 ) {
            i++;
            if((i < argc) && ((temp = atoi(argv[i])) > 0)) {
                ping_delay = temp;
            }
            else {
                LWIP_DEBUGF( PING_DEBUG, ("ping: bad timing interval\n"));
                return;
            }
        }
        else if ( strcmp( (char *) argv[i], "-w" ) == 0 ) {
            i++;
            if((i < argc) && ((temp = atoi(argv[i])) > 0)) {
                ping_rcv_timeo = temp;
            }
            else {
                LWIP_DEBUGF( PING_DEBUG, ("ping: bad recv timeout\n"));
                return;
            }
        }
        else if ( strcmp( (char *) argv[i], "-s" ) == 0 ) {
            i++;
            if((i < argc) && ((temp = atoi(argv[i])) > 0)) {
                ping_data_size = temp;
            }
            else {
                LWIP_DEBUGF( PING_DEBUG, ("ping: bad packet data size\n"));
                return;
            }
        }
        else if ( strcmp( (char *) argv[i], "-e" ) == 0 ) {
            ping_do_exit = 1;
            return ;
        }
        else if ( argc != i + 1){
           extern void _cli_ping_help_command( int argc, char **argv );
           LWIP_DEBUGF( PING_DEBUG, ("ping: Invalid command format\n"));
           _cli_ping_help_command( 0, NULL );
           return;
        }
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;

    if((temp = getaddrinfo(argv[argc-1], NULL, &hints, &res)) != 0) {
        LWIP_DEBUGF( PING_DEBUG, ("ping: Getaddrinfo error%"U32_F"\n", temp));
        return;
    }

    memset(&addr, 0, sizeof(ip_addr_t));
#if LWIP_IPV4
    if(((struct sockaddr*)(res->ai_addr))->sa_family == AF_INET) {
      struct sockaddr_in *addr4_in = (struct sockaddr_in*)(res->ai_addr);
      inet_addr_to_ipaddr(ip_2_ip4(&addr), &addr4_in->sin_addr);
      IP_SET_TYPE_VAL(addr, IPADDR_TYPE_V4);
    }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    if(((struct sockaddr*)(res->ai_addr))->sa_family == AF_INET6) {
      struct sockaddr_in6 *addr6_in = (struct sockaddr_in6*)(res->ai_addr);
      inet6_addr_to_ip6addr(ip_2_ip6(&addr), &addr6_in->sin6_addr);
      IP_SET_TYPE_VAL(addr, IPADDR_TYPE_V6);
    }
#endif /* LWIP_IPV6 */
    
    freeaddrinfo(res);

    ping_init(&addr);
}
#endif /* LWIP_RAW */
