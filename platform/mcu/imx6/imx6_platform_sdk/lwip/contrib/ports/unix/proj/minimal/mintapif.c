/*-----------------------------------------------------------------------------------*/
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

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/socket.h>

#if defined(linux)
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#define DEVTAP "/dev/net/tun"
#define IFCONFIG_ARGS "tap0 inet %d.%d.%d.%d"

#elif defined(openbsd)
#define DEVTAP "/dev/tun0"
#define IFCONFIG_ARGS "tun0 inet %d.%d.%d.%d link0"

#else /* freebsd, cygwin? */
#define DEVTAP "/dev/tap0"
#define IFCONFIG_ARGS "tap0 inet %d.%d.%d.%d"
#endif

#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/mem.h"
#include "netif/etharp.h"

#include "mintapif.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 't'

struct mintapif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  u32_t lasttime;
  int fd;
};

/* Forward declarations. */
static void  mintapif_input(struct netif *netif);

/*-----------------------------------------------------------------------------------*/
static void
low_level_init(struct netif *netif)
{
  struct mintapif *mintapif;
  char buf[1024];

  mintapif = netif->state;
  
  /* Obtain MAC address from network interface. */
  mintapif->ethaddr->addr[0] = 1;
  mintapif->ethaddr->addr[1] = 2;
  mintapif->ethaddr->addr[2] = 3;
  mintapif->ethaddr->addr[3] = 4;
  mintapif->ethaddr->addr[4] = 5;
  mintapif->ethaddr->addr[5] = 6;

  /* Do whatever else is needed to initialize interface. */  
  
  mintapif->fd = open(DEVTAP, O_RDWR);
  if (mintapif->fd == -1) {
    perror("tapif: tapif_init: open");
    exit(1);
  }

#ifdef linux
  {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
    if (ioctl(mintapif->fd, TUNSETIFF, (void *) &ifr) < 0) {
      perror(buf);
      exit(1);
    }
  }
#endif /* Linux */

  snprintf(buf, sizeof(buf), "/sbin/ifconfig " IFCONFIG_ARGS,
           ip4_addr1(&(netif->gw)),
           ip4_addr2(&(netif->gw)),
           ip4_addr3(&(netif->gw)),
           ip4_addr4(&(netif->gw)));
  
  system(buf);

  mintapif->lasttime = 0;

}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
/*-----------------------------------------------------------------------------------*/

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct mintapif *mintapif;
  struct pbuf *q;
  char buf[1514];
  char *bufptr;
  int written;

  mintapif = netif->state;
  
  /* initiate transfer(); */
  
  bufptr = &buf[0];
  
  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */    
    /* send data from(q->payload, q->len); */
    memcpy(bufptr, q->payload, q->len);
    bufptr += q->len;
  }

  /* signal that packet should be sent(); */
  written = write(mintapif->fd, buf, p->tot_len);
  if (written == -1) {
    snmp_inc_ifoutdiscards(netif);
    perror("tapif: write");
  }
  else {
    snmp_add_ifoutoctets(netif, written);
  }
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */
/*-----------------------------------------------------------------------------------*/
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  char buf[1514];
  char *bufptr;
  struct mintapif *mintapif;

  mintapif = netif->state;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = read(mintapif->fd, buf, sizeof(buf));
  snmp_add_ifinoctets(netif,len);

  /*  if (((double)rand()/(double)RAND_MAX) < 0.1) {
    printf("drop\n");
    return NULL;
    }*/
  
  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_LINK, len, PBUF_POOL);
  
  if (p != NULL) {
    /* We iterate over the pbuf chain until we have read the entire
       packet into the pbuf. */
    bufptr = &buf[0];
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
         available data in the pbuf is given by the q->len
         variable. */
      /* read data into(q->payload, q->len); */
      memcpy(q->payload, bufptr, q->len);
      bufptr += q->len;
    }
    /* acknowledge that packet has been read(); */
  } else {
    /* drop packet(); */
    snmp_inc_ifindiscards(netif);
    printf("Could not allocate pbufs\n");
  }

  return p;  
}
/*-----------------------------------------------------------------------------------*/
/*
 * mintapif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
mintapif_input(struct netif *netif)
{
  struct pbuf *p;

  p = low_level_input(netif);
  if (p != NULL) {
#if LINK_STATS
    lwip_stats.link.recv++;
#endif /* LINK_STATS */

    netif->input(p, netif);
  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * mintapif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t
mintapif_init(struct netif *netif)
{
  struct mintapif *mintapif;
    
  mintapif = mem_malloc(sizeof(struct mintapif));
  if (mintapif == NULL)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("cs8900_init: out of memory for mintapif\n"));
    return ERR_MEM;
  }
  netif->state = mintapif;
#if LWIP_SNMP
  /* ifType is other(1), there doesn't seem
     to be a proper type for the tunnel if */
  netif->link_type = 1;
  /* @todo get this from struct tunif? */
  netif->link_speed = 0;
  netif->ts = 0;
  netif->ifinoctets = 0;
  netif->ifinucastpkts = 0;
  netif->ifinnucastpkts = 0;
  netif->ifindiscards = 0;
  netif->ifoutoctets = 0;
  netif->ifoutucastpkts = 0;
  netif->ifoutnucastpkts = 0;
  netif->ifoutdiscards = 0;
#endif

  netif->hwaddr_len = 6;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  netif->mtu = 1500;
  
  mintapif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  low_level_init(netif);

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
enum mintapif_signal
mintapif_wait(struct netif *netif, u16_t time)
{
  fd_set fdset;
  struct timeval tv, now;
  struct timezone tz;
  int ret;
  struct mintapif *mintapif;

  mintapif = netif->state;

  while (1) {
  
    if (mintapif->lasttime >= (u32_t)time * 1000) {
      mintapif->lasttime = 0;
      return MINTAPIF_TIMEOUT;
    }
    
    tv.tv_sec = 0;
    tv.tv_usec = (u32_t)time * 1000 - mintapif->lasttime;
    
    
    FD_ZERO(&fdset);
    FD_SET(mintapif->fd, &fdset);
    
    gettimeofday(&now, &tz);
    ret = select(mintapif->fd + 1, &fdset, NULL, NULL, &tv);
    if (ret == 0) {
      mintapif->lasttime = 0;    
      return MINTAPIF_TIMEOUT;
    } 
    gettimeofday(&tv, &tz);
    mintapif->lasttime += (tv.tv_sec - now.tv_sec) * 1000000 + (tv.tv_usec - now.tv_usec);

    mintapif_input(netif);
  }
  
  return MINTAPIF_PACKET;
}

int
mintapif_select(struct netif *netif)
{
  fd_set fdset;
  int ret;
  struct timeval tv;
  struct mintapif *mintapif;

  mintapif = netif->state;

  tv.tv_sec = 0;
  tv.tv_usec = 0; /* usec_to; */
  
  FD_ZERO(&fdset);
  FD_SET(mintapif->fd, &fdset);

  ret = select(mintapif->fd + 1, &fdset, NULL, NULL, &tv);
  if (ret > 0) {
    mintapif_input(netif);   
  }
  return ret;
}
