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

#include <stdio.h>

#include "lwip/opt.h"

#if (LWIP_IPV4 || LWIP_IPV6) && LWIP_TCP /* @todo: fix IPv6 */

#include "netif/tcpdump.h"
#include "lwip/ip.h"
#include "lwip/ip4.h"
#include "lwip/ip6.h"
#include "lwip/ip_addr.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"

#ifndef TCPDUMP_DEBUG
#define TCPDUMP_DEBUG LWIP_DBG_OFF
#endif

static FILE *file = NULL;

/*-----------------------------------------------------------------------------------*/
void
tcpdump_init(void)
{
#define TCPDUMP_FNAME "/tmp/tcpdump"
  file = fopen(TCPDUMP_FNAME, "w");
  if (file == NULL) {
    perror("tcpdump_init: cannot open \""TCPDUMP_FNAME"\" for writing");
  }
  LWIP_DEBUGF(TCPDUMP_DEBUG, ("tcpdump: file %s\n", TCPDUMP_FNAME));
}
/*-----------------------------------------------------------------------------------*/
void
tcpdump(struct pbuf *p)
{
#if LWIP_IPV4
  ip_addr_t src, dst;
  struct ip_hdr *iphdr;
#if LWIP_UDP
  struct udp_hdr *udphdr;
#endif
#if LWIP_TCP
  struct tcp_hdr *tcphdr;
  char flags[5];
  int i;
  int len;
  int offset;
#endif

  if (file == NULL) {
    return;
  }
  iphdr = (struct ip_hdr *)p->payload;
  
#if LWIP_IPV6
  if(IPH_V(iphdr) == 6) {
    struct ip6_hdr *ip6hdr = (struct ip6_hdr*)iphdr;
    ip_addr_copy_from_ip6(src, ip6hdr->src);
    ip_addr_copy_from_ip6(dst, ip6hdr->dest);
    fprintf(file, "%s > %s: (IPv6, unsupported) ",
            ip_ntoa(&src),
            ip_ntoa(&dst));
    return; /* not supported */
  }
#endif

  if(IPH_V(iphdr) == 4) {
    ip_addr_copy_from_ip4(src, iphdr->src);
    ip_addr_copy_from_ip4(dst, iphdr->dest);
  }
  
  switch (IPH_PROTO(iphdr)) {
#if LWIP_TCP
  case IP_PROTO_TCP:
    tcphdr = (struct tcp_hdr *)((char *)iphdr + IPH_HL(iphdr));

    pbuf_header(p, -IP_HLEN);
    if (ip_chksum_pseudo(p, IP_PROTO_TCP, p->tot_len, &src, &dst) != 0) {
      LWIP_DEBUGF(TCPDUMP_DEBUG, ("tcpdump: IP checksum failed!\n"));
      /*
      fprintf(file, "chksum 0x%lx ", tcphdr->chksum);
      tcphdr->chksum = 0;
      fprintf(file, "should be 0x%lx ", inet_chksum_pseudo(p, (ip_addr_t *)&(iphdr->src),
              (ip_addr_t *)&(iphdr->dest), IP_PROTO_TCP, p->tot_len));*/
      fprintf(file, "!chksum ");
    }

    i = 0;
    if (TCPH_FLAGS(tcphdr) & TCP_SYN) {
      flags[i++] = 'S';
    }
    if (TCPH_FLAGS(tcphdr) & TCP_PSH) {
      flags[i++] = 'P';
    }
    if (TCPH_FLAGS(tcphdr) & TCP_FIN) {
      flags[i++] = 'F';
    }
    if (TCPH_FLAGS(tcphdr) & TCP_RST) {
      flags[i++] = 'R';
    }
    if (i == 0) {
      flags[i++] = '.';
    }
    flags[i++] = 0;

    fprintf(file, "%s.%u > %s.%u: ",
            ip_ntoa(&src),
            lwip_ntohs(tcphdr->src),
            ip_ntoa(&dst),
            lwip_ntohs(tcphdr->dest));
    offset = TCPH_HDRLEN(tcphdr);

    len = lwip_ntohs(IPH_LEN(iphdr)) - offset * 4 - IP_HLEN;
    if (len != 0 || flags[0] != '.') {
      fprintf(file, "%s %u:%u(%u) ", flags, lwip_ntohl(tcphdr->seqno),
              lwip_ntohl(tcphdr->seqno) + len, len);
    }
    if (TCPH_FLAGS(tcphdr) & TCP_ACK) {
      fprintf(file, "ack %u ", lwip_ntohl(tcphdr->ackno));
    }
    fprintf(file, "wnd %u\n", lwip_ntohs(tcphdr->wnd));

    fflush(file);

    pbuf_header(p, IP_HLEN);
    break;
#endif /* LWIP_TCP */

#if LWIP_UDP
  case IP_PROTO_UDP:
    udphdr = (struct udp_hdr *)((char *)iphdr + IPH_HL(iphdr));

    pbuf_header(p, -IP_HLEN);
    if (ip_chksum_pseudo(p, IP_PROTO_UDP, p->tot_len, &src, &dst) != 0) {
      LWIP_DEBUGF(TCPDUMP_DEBUG, ("tcpdump: IP checksum failed!\n"));
      /*
      fprintf(file, "chksum 0x%lx ", tcphdr->chksum);
      tcphdr->chksum = 0;
      fprintf(file, "should be 0x%lx ", ip_chksum_pseudo(p, &src, &dst, IP_PROTO_TCP, p->tot_len));*/
      fprintf(file, "!chksum ");
    }

    fprintf(file, "%s.%u > %s.%u: ",
            ip_ntoa(&src),
            lwip_ntohs(udphdr->src),
            ip_ntoa(&dst),
            lwip_ntohs(udphdr->dest));
    fprintf(file, "U ");
    len = lwip_ntohs(IPH_LEN(iphdr)) - sizeof(struct udp_hdr) - IP_HLEN;
    fprintf(file, " %d\n", len);

    fflush(file);

    pbuf_header(p, IP_HLEN);
    break;
#endif /* LWIP_UDP */
  default:
    LWIP_DEBUGF(TCPDUMP_DEBUG, ("unhandled IP protocol: %d\n", (int)IPH_PROTO(iphdr)));
    break;
  }
#else
  LWIP_UNUSED_ARG(p);
#endif
}

#endif /* LWIP_IPV4 && LWIP_TCP */

/*-----------------------------------------------------------------------------------*/
