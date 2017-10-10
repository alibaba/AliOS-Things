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

#include "lwip/opt.h"
#include "lwip/pbuf.h"

#if !NO_SYS

#include "lwip/debug.h"

#include <stdlib.h>

#include "lwip/def.h"
#include "netif/delif.h"

#ifdef LWIP_UNIX_LINUX
#include "netif/tapif.h"
#else /* LWIP_UNIX_LINUX */
#include "netif/tunif.h"
#endif /* LWIP_UNIX_LINUX */

#include "lwip/sys.h"
#include "lwip/timeouts.h"

#ifndef DELIF_DEBUG
#define DELIF_DEBUG    LWIP_DBG_OFF
#endif

#define DELIF_INPUT_DROPRATE 0.1
#define DELIF_OUTPUT_DROPRATE 0.1

#define DELIF_INPUT_DELAY  500      /* Miliseconds. */
#define DELIF_OUTPUT_DELAY 500      /* Miliseconds. */

#define DELIF_TIMEOUT 10

struct delif {
  err_t (* input)(struct pbuf *p, struct netif *inp);
  struct netif *netif;
};

struct delif_pbuf {
  struct delif_pbuf *next;
  struct pbuf *p;
  ip_addr_t ipaddr;
  unsigned int time;
};

static struct delif_pbuf *input_list = NULL;
static struct delif_pbuf *output_list = NULL;
/*-----------------------------------------------------------------------------------*/
static void
delif_input_timeout(void *arg)
{
  struct netif *netif;
  struct delif *delif;
  struct delif_pbuf *dp;
  unsigned int timeout, now;

  timeout = DELIF_TIMEOUT;

  netif = (struct netif*)arg;
  delif = (struct delif*)netif->state;

  /* Check if there is anything on the input list. */
  dp = input_list;
  while (dp != NULL) {
    now = sys_now();

    if (dp->time <= now) {
      delif->input(dp->p, netif);
      if (dp->next != NULL) {
        if (dp->next->time > now) {
          timeout = dp->next->time - now;
        } else {
          timeout = 0;
        }
        LWIP_DEBUGF(DELIF_DEBUG, ("delif_output_timeout: timeout %u.\n", timeout));

      }
      input_list = dp->next;
      free(dp);
      dp = input_list;
    } else {
      dp = dp->next;	
    }
  }
  
  sys_timeout(timeout, delif_input_timeout, arg);
}
/*-----------------------------------------------------------------------------------*/
static void
delif_output_timeout(void *arg)
{
  struct netif *netif;
  struct delif *delif;
  struct delif_pbuf *dp;
  unsigned int timeout, now;

  timeout = DELIF_TIMEOUT;

  netif = (struct netif*)arg;
  delif = (struct delif*)netif->state;

  /* Check if there is anything on the output list. */
  dp = output_list;
  while (dp != NULL) {
    now = sys_now();
    if (dp->time <= now) {
      LWIP_DEBUGF(DELIF_DEBUG, ("delif_output_timeout: now %u dp->time %u\n",
        now, dp->time));

#if LWIP_IPV4
      if(!IP_IS_V6_VAL(dp->ipaddr)) {
        delif->netif->output(delif->netif, dp->p, ip_2_ip4(&dp->ipaddr));
      }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
      if(IP_IS_V6_VAL(dp->ipaddr)) {
        delif->netif->output_ip6(delif->netif, dp->p, ip_2_ip6(&dp->ipaddr));
      }
#endif /* LWIP_IPV6 */

      if (dp->next != NULL) {
        if (dp->next->time > now) {
          timeout = dp->next->time - now;
        } else {
          timeout = 0;
        }
        LWIP_DEBUGF(DELIF_DEBUG, ("delif_output_timeout: timeout %u.\n", timeout));

      }
      pbuf_free(dp->p);

      output_list = dp->next;
      free(dp);
      dp = output_list;
    } else {
      dp = dp->next;	
    }
  }

  sys_timeout(timeout, delif_output_timeout, arg);
}
/*-----------------------------------------------------------------------------------*/
static err_t
delif_output(struct netif *netif, struct pbuf *p, const ip_addr_t *ipaddr)
{
  struct delif_pbuf *dp, *np;
  char *data;

  LWIP_UNUSED_ARG(netif);

  LWIP_DEBUGF(DELIF_DEBUG, ("delif_output\n"));

#ifdef DELIF_OUTPUT_DROPRATE
  if (((double)rand()/(double)RAND_MAX) < DELIF_OUTPUT_DROPRATE) {
    LWIP_DEBUGF(DELIF_DEBUG, ("delif_output: Packet dropped\n"));
    return ERR_BUF;
  }
#endif /* DELIF_OUTPUT_DROPRATE */

  LWIP_DEBUGF(DELIF_DEBUG, ("delif_output\n"));

  dp = (struct delif_pbuf*)malloc(sizeof(struct delif_pbuf));
  data = (char*)malloc(p->tot_len);

  pbuf_copy_partial(p, data, p->tot_len, 0);

  dp->p = pbuf_alloc(PBUF_LINK, 0, PBUF_ROM);
  dp->p->payload = data;
  dp->p->len = p->tot_len;
  dp->p->tot_len = p->tot_len;
  dp->ipaddr = *ipaddr;
  dp->time = sys_now() + DELIF_OUTPUT_DELAY;
  dp->next = NULL;
  if (output_list == NULL) {
    output_list = dp;
  } else {
    for(np = output_list; np->next != NULL; np = np->next);
    np->next = dp;
  }

  return ERR_OK;
}

#if LWIP_IPV4
static err_t
delif_output4(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
  ip_addr_t ip;
  if (ipaddr != NULL) {
    ip_addr_copy_from_ip4(ip, *ipaddr);
  } else {
    ip = *IP_ADDR_ANY;
  }
  return delif_output(netif, p, &ip);
}
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
static err_t
delif_output6(struct netif *netif, struct pbuf *p, const ip6_addr_t *ipaddr)
{
  ip_addr_t ip;
  if (ipaddr != NULL) {
    ip_addr_copy_from_ip6(ip, *ipaddr);
  } else {
    ip = *IP6_ADDR_ANY;
  }
  return delif_output(netif, p, &ip);
}
#endif /* LWIP_IPV6 */
/*-----------------------------------------------------------------------------------*/
static err_t
delif_input(struct pbuf *p, struct netif *inp)
{
  struct delif_pbuf *dp, *np;

  LWIP_UNUSED_ARG(inp);

  LWIP_DEBUGF(DELIF_DEBUG, ("delif_input\n"));
#ifdef DELIF_INPUT_DROPRATE
  if (((double)rand()/(double)RAND_MAX) < DELIF_INPUT_DROPRATE) {
    LWIP_DEBUGF(DELIF_DEBUG, ("delif_input: Packet dropped\n"));
    pbuf_free(p);
    return ERR_OK;
  }
#endif /* DELIF_INPUT_DROPRATE */

  dp = (struct delif_pbuf*)malloc(sizeof(struct delif_pbuf));
  dp->p = p;
  dp->time = sys_now() + DELIF_INPUT_DELAY;
  dp->next = NULL;

  if (input_list == NULL) {
    input_list = dp;
  } else {
    for(np = input_list; np->next != NULL; np = np->next);
    np->next = dp;
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
err_t
delif_init(struct netif *netif)
{
  struct delif *del;

  del = (struct delif*)malloc(sizeof(struct delif));
  if (!del) {
    return ERR_MEM;
  }
  netif->state = del;
  netif->name[0] = 'd';
  netif->name[1] = 'e';

#if LWIP_IPV4
  netif->output     = delif_output4;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
  netif->output_ip6 = delif_output6;
#endif /* LWIP_IPV6 */

  del->netif = (struct netif*)malloc(sizeof(struct netif));
  if (!del->netif) {
    free(del);
    return ERR_MEM;
  }

#ifdef LWIP_UNIX_LINUX
  tapif_init(del->netif);
#else /* LWIP_UNIX_LINUX */
  tunif_init(del->netif);
#endif /* LWIP_UNIX_LINUX */

  del->input        = netif->input;
  del->netif->input = delif_input;

  sys_timeout(DELIF_TIMEOUT, delif_input_timeout,  netif);
  sys_timeout(DELIF_TIMEOUT, delif_output_timeout, netif);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static void 
delif_thread(void *arg)
{
  struct netif *netif = (struct netif*)arg;
  struct delif *del;
  sys_sem_t sem;

  del = (struct delif*)netif->state;
#ifdef LWIP_UNIX_LINUX
  tapif_init(del->netif);
#else /* LWIP_UNIX_LINUX */
  tunif_init(del->netif);
#endif /* LWIP_UNIX_LINUX */

  sys_timeout(DELIF_TIMEOUT, delif_input_timeout,  netif);
  sys_timeout(DELIF_TIMEOUT, delif_output_timeout, netif);

  if(sys_sem_new(&sem, 0) != ERR_OK) {
    LWIP_ASSERT("Failed to create semaphore", 0);
  }
  sys_sem_wait(&sem);
}
/*-----------------------------------------------------------------------------------*/
err_t
delif_init_thread(struct netif *netif)
{
  struct delif *del;

  LWIP_DEBUGF(DELIF_DEBUG, ("delif_init_thread\n"));

  del = (struct delif*)malloc(sizeof(struct delif));
  if (!del) {
    return ERR_MEM;
  }

  netif->state = del;
  netif->name[0] = 'd';
  netif->name[1] = 'e';

  del->netif = (struct netif*)malloc(sizeof(struct netif));
  if (!del->netif) {
    free(del);
    return ERR_MEM;
  }

#if LWIP_IPV4
  netif->output = delif_output4;
  netif_set_ipaddr(del->netif, netif_ip4_addr(netif));
  netif_set_gw(del->netif, netif_ip4_gw(netif));
  netif_set_netmask(del->netif, netif_ip4_netmask(netif));
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  {
    int i;
    netif->output_ip6 = delif_output6;
    for(i=0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      netif_ip6_addr_set(del->netif, i, netif_ip6_addr(netif, i));
    }
  }
#endif /* LWIP_IPV6 */

  del->input = netif->input;
  del->netif->input = delif_input;

  sys_thread_new("delif_thread", delif_thread, netif, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/

#endif /* !NO_SYS */
