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
  
#include "netif/tcpdump.h"
#include "lwip/ip.h"
#include "lwip/tcp_impl.h"
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
  struct ip_hdr *iphdr;
  struct tcp_hdr *tcphdr;
#if LWIP_UDP
  struct udp_hdr *udphdr;
#endif
  char flags[5];
  int i;
  int len;
  int offset;

  if (file == NULL) {
    return;
  }
#ifdef IPv4
  iphdr = (struct ip_hdr *)p->payload;
  switch (IPH_PROTO(iphdr)) {
#if LWIP_TCP
  case IP_PROTO_TCP:
    tcphdr = (struct tcp_hdr *)((char *)iphdr + IP_HLEN);

    pbuf_header(p, -IP_HLEN);
    if (inet_chksum_pseudo(p, (ip_addr_t *)&(iphdr->src),
			  (ip_addr_t *)&(iphdr->dest),
			  IP_PROTO_TCP, p->tot_len) != 0) {
      LWIP_DEBUGF(TCPDUMP_DEBUG, ("tcpdump: IP checksum failed!\n"));
      /*    fprintf(file, "chksum 0x%lx ", tcphdr->chksum);
	    tcphdr->chksum = 0;
	    fprintf(file, "should be 0x%lx ", inet_chksum_pseudo(p, (ip_addr_t *)&(iphdr->src),
	    (ip_addr_t *)&(iphdr->dest),
	    IP_PROTO_TCP, p->tot_len));*/
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



    fprintf(file, "%d.%d.%d.%d.%u > %d.%d.%d.%d.%u: ",
	    (int)(ntohl(iphdr->src.addr) >> 24) & 0xff,
	    (int)(ntohl(iphdr->src.addr) >> 16) & 0xff,
	    (int)(ntohl(iphdr->src.addr) >> 8) & 0xff,
	    (int)(ntohl(iphdr->src.addr) >> 0) & 0xff,
	    ntohs(tcphdr->src),
	  (int)(ntohl(iphdr->dest.addr) >> 24) & 0xff,
	    (int)(ntohl(iphdr->dest.addr) >> 16) & 0xff,
	    (int)(ntohl(iphdr->dest.addr) >> 8) & 0xff,
	    (int)(ntohl(iphdr->dest.addr) >> 0) & 0xff,
	    ntohs(tcphdr->dest));
    offset = TCPH_HDRLEN(tcphdr);

    len = ntohs(IPH_LEN(iphdr)) - offset * 4 - IP_HLEN;
    if (len != 0 || flags[0] != '.') {
      fprintf(file, "%s %u:%u(%u) ",
	      flags,
	      ntohl(tcphdr->seqno),
	      ntohl(tcphdr->seqno) + len,
	      len);
    }
    if (TCPH_FLAGS(tcphdr) & TCP_ACK) {
      fprintf(file, "ack %u ",
	      ntohl(tcphdr->ackno));
    }
    fprintf(file, "wnd %u\n",
	    ntohs(tcphdr->wnd));

    fflush(file);

    pbuf_header(p, IP_HLEN);
    break;
#endif /* LWIP_TCP */

#if LWIP_UDP
  case IP_PROTO_UDP:
    udphdr = (struct udp_hdr *)((char *)iphdr + IP_HLEN);

    pbuf_header(p, -IP_HLEN);
    if (inet_chksum_pseudo(p, (ip_addr_t *)&(iphdr->src),
			  (ip_addr_t *)&(iphdr->dest),
			  IP_PROTO_UDP, p->tot_len) != 0) {
      LWIP_DEBUGF(TCPDUMP_DEBUG, ("tcpdump: IP checksum failed!\n"));
      /*    fprintf(file, "chksum 0x%lx ", tcphdr->chksum);
	    tcphdr->chksum = 0;
	    fprintf(file, "should be 0x%lx ", inet_chksum_pseudo(p, (ip_addr_t *)&(iphdr->src),
	    (ip_addr_t *)&(iphdr->dest),
	    IP_PROTO_TCP, p->tot_len));*/
      fprintf(file, "!chksum ");
    }

    fprintf(file, "%d.%d.%d.%d.%u > %d.%d.%d.%d.%u: ",
	    (int)(ntohl(iphdr->src.addr) >> 24) & 0xff,
	    (int)(ntohl(iphdr->src.addr) >> 16) & 0xff,
	    (int)(ntohl(iphdr->src.addr) >> 8) & 0xff,
	    (int)(ntohl(iphdr->src.addr) >> 0) & 0xff,
	    ntohs(udphdr->src),
	  (int)(ntohl(iphdr->dest.addr) >> 24) & 0xff,
	    (int)(ntohl(iphdr->dest.addr) >> 16) & 0xff,
	    (int)(ntohl(iphdr->dest.addr) >> 8) & 0xff,
	    (int)(ntohl(iphdr->dest.addr) >> 0) & 0xff,
	    ntohs(udphdr->dest));
    fprintf(file, "U ");
    len = ntohs(IPH_LEN(iphdr)) - sizeof(struct udp_hdr) - IP_HLEN;
    fprintf(file, " %d\n", len);

    fflush(file);

    pbuf_header(p, IP_HLEN);
    break;
#endif /* LWIP_UDP */
  default:
    LWIP_DEBUGF(TCPDUMP_DEBUG, ("unhandled IP protocol: %d\n", (int)IPH_PROTO(iphdr)));
    break;

  }
#endif /* IPv4 */
}
/*-----------------------------------------------------------------------------------*/




