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

#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#include "lwip/opt.h"

#include "lwip/init.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"

#include "lwip/stats.h"

#include "lwip/tcp_impl.h"
#include "lwip/inet_chksum.h"

#include "lwip/tcpip.h"
#include "lwip/sockets.h"

#include "netif/tapif.h"
#include "netif/tunif.h"

#include "netif/unixif.h"
#include "netif/dropif.h"
#include "netif/pcapif.h"

#include "netif/tcpdump.h"

#if PPP_SUPPORT
#include "netif/ppp/ppp.h"
#define PPP_PTY_TEST 1
#include <termios.h>
#endif

#include "lwip/ip_addr.h"
#include "arch/perf.h"

#include "httpd.h"
#include "udpecho.h"
#include "tcpecho.h"
#include "shell.h"

#if LWIP_RAW
#include "lwip/icmp.h"
#include "lwip/raw.h"
#endif

/* (manual) host IP configuration */
static ip_addr_t ipaddr, netmask, gw;

/* ping out destination cmd option */
static unsigned char ping_flag;
static ip_addr_t ping_addr;

/* nonstatic debug cmd option, exported in lwipopts.h */
unsigned char debug_flags;

/** @todo add options for selecting netif, starting DHCP client etc */
static struct option longopts[] = {
  /* turn on debugging output (if build with LWIP_DEBUG) */
  {"debug", no_argument,        NULL, 'd'},
  /* help */
  {"help", no_argument, NULL, 'h'},
  /* gateway address */
  {"gateway", required_argument, NULL, 'g'},
  /* ip address */
  {"ipaddr", required_argument, NULL, 'i'},
  /* netmask */
  {"netmask", required_argument, NULL, 'm'},
  /* ping destination */
  {"ping",   required_argument, NULL, 'p'},
  /* new command line options go here! */
  {NULL,   0,                 NULL,  0}
};
#define NUM_OPTS ((sizeof(longopts) / sizeof(struct option)) - 1)

static void init_netifs(void);

static void usage(void)
{
  unsigned char i;

  printf("options:\n");
  for (i = 0; i < NUM_OPTS; i++) {
    printf("-%c --%s\n",longopts[i].val, longopts[i].name);
  }
}

#if 0
static void
tcp_debug_timeout(void *data)
{
  LWIP_UNUSED_ARG(data);
#if TCP_DEBUG
  tcp_debug_print_pcbs();
#endif /* TCP_DEBUG */
  sys_timeout(5000, tcp_debug_timeout, NULL);
}
#endif

static void
tcpip_init_done(void *arg)
{
  sys_sem_t *sem;
  sem = (sys_sem_t *)arg;

  init_netifs();

  sys_sem_signal(sem);
}

#if PPP_SUPPORT
void 
pppLinkStatusCallback(void *ctx, int errCode, void *arg)
{
    switch(errCode) {
    case PPPERR_NONE:               /* No error. */
        {
        struct ppp_addrs *ppp_addrs = arg;

        printf("pppLinkStatusCallback: PPPERR_NONE");
        printf(" our_ipaddr=%s", _inet_ntoa(ppp_addrs->our_ipaddr.addr));
        printf(" his_ipaddr=%s", _inet_ntoa(ppp_addrs->his_ipaddr.addr));
        printf(" netmask=%s", _inet_ntoa(ppp_addrs->netmask.addr));
        printf(" dns1=%s", _inet_ntoa(ppp_addrs->dns1.addr));
        printf(" dns2=%s\n", _inet_ntoa(ppp_addrs->dns2.addr));
        }
        break;

    case PPPERR_PARAM:             /* Invalid parameter. */
        printf("pppLinkStatusCallback: PPPERR_PARAM\n");
        break;

    case PPPERR_OPEN:              /* Unable to open PPP session. */
        printf("pppLinkStatusCallback: PPPERR_OPEN\n");
        break;

    case PPPERR_DEVICE:            /* Invalid I/O device for PPP. */
        printf("pppLinkStatusCallback: PPPERR_DEVICE\n");
        break;

    case PPPERR_ALLOC:             /* Unable to allocate resources. */
        printf("pppLinkStatusCallback: PPPERR_ALLOC\n");
        break;

    case PPPERR_USER:              /* User interrupt. */
        printf("pppLinkStatusCallback: PPPERR_USER\n");
        break;

    case PPPERR_CONNECT:           /* Connection lost. */
        printf("pppLinkStatusCallback: PPPERR_CONNECT\n");
        break;

    case PPPERR_AUTHFAIL:          /* Failed authentication challenge. */
        printf("pppLinkStatusCallback: PPPERR_AUTHFAIL\n");
        break;

    case PPPERR_PROTOCOL:          /* Failed to meet protocol. */
        printf("pppLinkStatusCallback: PPPERR_PROTOCOL\n");
        break;

    default:
        printf("pppLinkStatusCallback: unknown errCode %d\n", errCode);
        break;
    }
}
#endif

/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
#if LWIP_RAW

static int seq_num;

#if 0
/* Ping using the raw api */
static int
ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
  printf("ping recv\n");
  return 1; /* eat the event */
}

static void
ping_send(struct raw_pcb *raw, ip_addr_t *addr)
{
  struct pbuf *p;
  struct icmp_echo_hdr *iecho;

  p = pbuf_alloc(PBUF_IP,sizeof(struct icmp_echo_hdr),PBUF_RAM);
  if (!p) return;

  iecho = p->payload;
  ICMPH_TYPE_SET(iecho,ICMP_ECHO);
  iecho->chksum = 0;
  iecho->seqno = htons(seq_num);

  iecho->chksum = inet_chksum(iecho, p->len);
  raw_send_to(raw,p,addr);

  pbuf_free(p);

  seq_num++;
}

static void
ping_thread(void *arg)
{
  struct raw_pcb *raw;

  if (!(raw = raw_new(IP_PROTO_ICMP))) return;

  raw_recv(raw,ping_recv,NULL);

  while (1)
  {
    printf("ping send\n");
    ping_send(raw,&ping_addr);
    sleep(1);
  }
  /* Never reaches this */
  raw_remove(raw);
}
#else
/* Ping using the socket api */

static void
ping_send(int s, ip_addr_t *addr)
{
  struct icmp_echo_hdr *iecho;
  struct sockaddr_in to;

  if (!(iecho = (struct icmp_echo_hdr *)malloc(sizeof(struct icmp_echo_hdr))))
    return;

  ICMPH_TYPE_SET(iecho,ICMP_ECHO);
  iecho->chksum = 0;
  iecho->seqno = htons(seq_num);
  iecho->chksum = inet_chksum(iecho, sizeof(*iecho));

  to.sin_len = sizeof(to);
  to.sin_family = AF_INET;
  to.sin_addr.s_addr = addr->addr;

  lwip_sendto(s,iecho,sizeof(*iecho),0,(struct sockaddr*)&to,sizeof(to));

  free(iecho);
  seq_num++;
}

static void
ping_recv(int s, ip_addr_t *addr)
{
  char buf[200];
  socklen_t fromlen;
  int len;
  struct sockaddr_in from;
  LWIP_UNUSED_ARG(addr);

  len = lwip_recvfrom(s, buf,sizeof(buf),0,(struct sockaddr*)&from,&fromlen);

  printf("Received %d bytes from %x\n",len,ntohl(from.sin_addr.s_addr));
}

static void
ping_thread(void *arg)
{
  int s;
  LWIP_UNUSED_ARG(arg);

  if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
    return;
  }

  while (1) {
    printf("sending ping\n");
    ping_send(s,&ping_addr);
    ping_recv(s,&ping_addr);
    sleep(1);
  }
}
#endif

#endif

struct netif netif;

static void
init_netifs(void)
{
#if PPP_SUPPORT
  pppInit();
#if PPP_PTY_TEST
  ppp_sio = sio_open(2);
#else
  ppp_sio = sio_open(0);
#endif
  if(!ppp_sio)
  {
      perror("Error opening device: ");
      exit(1);
  }

#ifdef LWIP_PPP_CHAP_TEST
  pppSetAuth(PPPAUTHTYPE_CHAP, "lwip", "mysecret");
#endif

  pppOpen(ppp_sio, pppLinkStatusCallback, NULL);
#endif /* PPP_SUPPORT */
  
#if LWIP_DHCP
  {
    IP4_ADDR(&gw, 0,0,0,0);
    IP4_ADDR(&ipaddr, 0,0,0,0);
    IP4_ADDR(&netmask, 0,0,0,0);

    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, tapif_init,
              tcpip_input);
    netif_set_default(&netif);
    dhcp_start(&netif);
  }
#else
  
  netif_set_default(netif_add(&netif,&ipaddr, &netmask, &gw, NULL, tapif_init,
                  tcpip_input));
  netif_set_up(&netif);

#endif

#if 0
  /* Only used for testing purposes: */
  netif_add(&ipaddr, &netmask, &gw, NULL, pcapif_init, tcpip_input);
#endif
  
#if LWIP_TCP  
  tcpecho_init();
  shell_init();
  httpd_init();
#endif
#if LWIP_UDP  
  udpecho_init();
#endif  
  /*  sys_timeout(5000, tcp_debug_timeout, NULL);*/
}

/*-----------------------------------------------------------------------------------*/
static void
main_thread(void *arg)
{
  sys_sem_t sem;
#if PPP_SUPPORT
  sio_fd_t ppp_sio;
#endif
  LWIP_UNUSED_ARG(arg);

  netif_init();

  if(sys_sem_new(&sem, 0) != ERR_OK) {
    LWIP_ASSERT("Failed to create semaphore", 0);
  }
  tcpip_init(tcpip_init_done, &sem);
  sys_sem_wait(&sem);
  printf("TCP/IP initialized.\n");

#if LWIP_RAW
  /** @todo remove dependency on RAW PCB support */
  if(ping_flag) {
    sys_thread_new("ping_thread", ping_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
  }
#endif

  printf("Applications started.\n");


#ifdef MEM_PERF
  mem_perf_init("/tmp/memstats.client");
#endif /* MEM_PERF */
#if 0
    stats_display();
#endif
  /* Block forever. */
  sys_sem_wait(&sem);
}
/*-----------------------------------------------------------------------------------*/
int
main(int argc, char **argv)
{
  struct in_addr inaddr;
  int ch;
  char ip_str[16] = {0}, nm_str[16] = {0}, gw_str[16] = {0};

  /* startup defaults (may be overridden by one or more opts) */
  IP4_ADDR(&gw, 192,168,0,1);
  IP4_ADDR(&netmask, 255,255,255,0);
  IP4_ADDR(&ipaddr, 192,168,0,2);
  
  ping_flag = 0;
  /* use debug flags defined by debug.h */
  debug_flags = LWIP_DBG_OFF;
  
  while ((ch = getopt_long(argc, argv, "dhg:i:m:p:", longopts, NULL)) != -1) {
    switch (ch) {
      case 'd':
        debug_flags |= (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT);
        break;
      case 'h':
        usage();
        exit(0);
        break;
      case 'g':
        inet_aton(optarg, &inaddr);
        gw.addr = inaddr.s_addr;
        break;
      case 'i':
        inet_aton(optarg, &inaddr);
        ipaddr.addr = inaddr.s_addr;
        break;
      case 'm':
        inet_aton(optarg, &inaddr);
        netmask.addr = inaddr.s_addr;
        break;
      case 'p':
        ping_flag = !0;
        inet_aton(optarg, &inaddr);
        /* lwip inet.h oddity workaround */
        ping_addr.addr = inaddr.s_addr; 
        strncpy(ip_str,inet_ntoa(inaddr),sizeof(ip_str));
        printf("Using %s to ping\n", ip_str);
        break;
      default:
        usage();
        break;
    }
  }
  argc -= optind;
  argv += optind;

  inaddr.s_addr = ipaddr.addr;
  strncpy(ip_str,inet_ntoa(inaddr),sizeof(ip_str));
  inaddr.s_addr = netmask.addr;
  strncpy(nm_str,inet_ntoa(inaddr),sizeof(nm_str));
  inaddr.s_addr = gw.addr;
  strncpy(gw_str,inet_ntoa(inaddr),sizeof(gw_str));
  printf("Host at %s mask %s gateway %s\n", ip_str, nm_str, gw_str);

#ifdef PERF
  perf_init("/tmp/simhost.perf");
#endif /* PERF */

  printf("System initialized.\n");
    
  sys_thread_new("main_thread", main_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
  pause();
  return 0;
}
/*-----------------------------------------------------------------------------------*/








