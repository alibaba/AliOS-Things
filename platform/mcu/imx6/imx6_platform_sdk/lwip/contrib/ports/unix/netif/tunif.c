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

#include "netif/tunif.h"

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>


#include "lwip/debug.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/mem.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"


#define IFNAME0 't'
#define IFNAME1 'n'

#ifndef TUNIF_DEBUG
#define TUNIF_DEBUG LWIP_DBG_OFF
#endif

#define IFCONFIG_CALL "/sbin/ifconfig tun0 inet %d.%d.%d.%d %d.%d.%d.%d"

struct tunif {
  /* Add whatever per-interface state that is needed here. */
  int fd;
};

/* Forward declarations. */
static void  tunif_input(struct netif *netif);
static err_t tunif_output(struct netif *netif, struct pbuf *p,
			       ip_addr_t *ipaddr);

static void tunif_thread(void *data);

/*-----------------------------------------------------------------------------------*/
static void
low_level_init(struct netif *netif)
{
  struct tunif *tunif;
  char buf[sizeof(IFCONFIG_CALL) + 50];

  tunif = (struct tunif *)netif->state;

  /* Obtain MAC address from network interface. */

  /* Do whatever else is needed to initialize interface. */

  tunif->fd = open("/dev/tun0", O_RDWR);
  LWIP_DEBUGF(TUNIF_DEBUG, ("tunif_init: fd %d\n", tunif->fd));
  if (tunif->fd == -1) {
    perror("tunif_init");
    exit(1);
  }
  sprintf(buf, IFCONFIG_CALL,
           ip4_addr1(&(netif->gw)),
           ip4_addr2(&(netif->gw)),
           ip4_addr3(&(netif->gw)),
           ip4_addr4(&(netif->gw)),
           ip4_addr1(&(netif->ip_addr)),
           ip4_addr2(&(netif->ip_addr)),
           ip4_addr3(&(netif->ip_addr)),
           ip4_addr4(&(netif->ip_addr)));

  LWIP_DEBUGF(TUNIF_DEBUG, ("tunif_init: system(\"%s\");\n", buf));
  system(buf);
  sys_thread_new("tunif_thread", tunif_thread, netif, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

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
low_level_output(struct tunif *tunif, struct pbuf *p)
{
  struct pbuf *q;
  char buf[1500];
  char *bufptr;
  int rnd_val;

  /* initiate transfer(); */

  rnd_val = rand();
  if (((double)rnd_val/(double)RAND_MAX) < 0.4) {
    printf("drop\n");
    return ERR_OK;
  }


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
  if (write(tunif->fd, buf, p->tot_len) == -1) {
    perror("tunif: write");
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
low_level_input(struct tunif *tunif)
{
  struct pbuf *p, *q;
  u16_t len;
  char buf[1500];
  char *bufptr;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = read(tunif->fd, buf, sizeof(buf));

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
  }

  return p;
}
/*-----------------------------------------------------------------------------------*/
static void 
tunif_thread(void *arg)
{
  struct netif *netif;
  struct tunif *tunif;
  fd_set fdset;
  int ret;

  netif = (struct netif *)arg;
  tunif = (struct tunif *)netif->state;

  while (1) {
    FD_ZERO(&fdset);
    FD_SET(tunif->fd, &fdset);

    /* Wait for a packet to arrive. */
    ret = select(tunif->fd + 1, &fdset, NULL, NULL, NULL);

    if (ret == 1) {
      /* Handle incoming packet. */
      tunif_input(netif);
    } else if (ret == -1) {
      perror("tunif_thread: select");
    }
  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * tunif_output():
 *
 * This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output() to
 * do the actuall transmission of the packet.
 *
 */
/*-----------------------------------------------------------------------------------*/
static err_t
tunif_output(struct netif *netif, struct pbuf *p,
		  ip_addr_t *ipaddr)
{
  struct tunif *tunif;
  LWIP_UNUSED_ARG(ipaddr);

  tunif = (struct tunif *)netif->state;

  return low_level_output(tunif, p);

}
/*-----------------------------------------------------------------------------------*/
/*
 * tunif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
tunif_input(struct netif *netif)
{
  struct tunif *tunif;
  struct pbuf *p;


  tunif = (struct tunif *)netif->state;

  p = low_level_input(tunif);

  if (p == NULL) {
    LWIP_DEBUGF(TUNIF_DEBUG, ("tunif_input: low_level_input returned NULL\n"));
    return;
  }

#if 0
/* CS: ip_lookup() was removed */
  if (ip_lookup(p->payload, netif)) {
#endif
    netif->input(p, netif);
#if 0
  }
#endif
}
/*-----------------------------------------------------------------------------------*/
/*
 * tunif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t
tunif_init(struct netif *netif)
{
  struct tunif *tunif;

  tunif = (struct tunif *)mem_malloc(sizeof(struct tunif));
  if (!tunif) {
    return ERR_MEM;
  }
  netif->state = tunif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = tunif_output;


  low_level_init(netif);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
