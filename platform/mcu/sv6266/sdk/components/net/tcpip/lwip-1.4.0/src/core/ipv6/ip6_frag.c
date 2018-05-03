/**
 * @file
 *
 * IPv6 fragmentation and reassembly.
 */

/*
 * Copyright (c) 2010 Inico Technologies Ltd.
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
 * Author: Ivan Delamer <delamer@inicotech.com>
 *
 *
 * Please coordinate changes and requests with Ivan Delamer
 * <delamer@inicotech.com>
 */

#include "lwip/opt.h"
#include "lwip/ip6_frag.h"
#include "lwip/ip6.h"
#include "lwip/icmp6.h"
#include "lwip/nd6.h"
#include "lwip/ip.h"

//#include "lwip/pbuf.h"
#include "lwip/memp.h"
#include "lwip/stats.h"

#include <string.h>

#if LWIP_IPV6 && LWIP_IPV6_REASS  /* don't build if not configured for use in lwipopts.h */


/** Setting this to 0, you can turn off checking the fragments for overlapping
 * regions. The code gets a little smaller. Only use this if you know that
 * overlapping won't occur on your network! */
#ifndef IP_REASS_CHECK_OVERLAP
#define IP_REASS_CHECK_OVERLAP 1
#endif /* IP_REASS_CHECK_OVERLAP */

/** Set to 0 to prevent freeing the oldest datagram when the reassembly buffer is
 * full (IP_REASS_MAX_PBUFS pbufs are enqueued). The code gets a little smaller.
 * Datagrams will be freed by timeout only. Especially useful when MEMP_NUM_REASSDATA
 * is set to 1, so one datagram can be reassembled at a time, only. */
#ifndef IP_REASS_FREE_OLDEST
#define IP_REASS_FREE_OLDEST 1
#endif /* IP_REASS_FREE_OLDEST */

#define IP_REASS_FLAG_LASTFRAG 0x01

/** This is a helper struct which holds the starting
 * offset and the ending offset of this fragment to
 * easily chain the fragments.
 */
struct ip6_reass_helper {
  struct ip6_reass_helper *next;
  struct pbuf *p;
  u16_t start;
  u16_t end;
};

/* static variables */
static struct ip6_reassdata *reassdatagrams;
static u16_t ip6_reass_pbufcount;

/* Forward declarations. */
static void ip6_reass_free_complete_datagram(struct ip6_reassdata *ipr);
#if IP_REASS_FREE_OLDEST
static void ip6_reass_remove_oldest_datagram(struct ip6_reassdata *ipr, int pbufs_needed);
#endif /* IP_REASS_FREE_OLDEST */

void
ip6_reass_tmr(void)
{
  struct ip6_reassdata *r, *tmp;

  r = reassdatagrams;
  while (r != NULL) {
    /* Decrement the timer. Once it reaches 0,
     * clean up the incomplete fragment assembly */
    if (r->timer > 0) {
      r->timer--;
      r = r->next;
    } else {
      /* reassembly timed out */
      tmp = r;
      /* get the next pointer before freeing */
      r = r->next;
      /* free the helper struct and all enqueued pbufs */
      ip6_reass_free_complete_datagram(tmp);
     }
   }
}

/**
 * Free a datagram (struct ip6_reassdata) and all its pbufs.
 * Updates the total count of enqueued pbufs (ip6_reass_pbufcount),
 * sends an ICMP time exceeded packet.
 *
 * @param ipr datagram to free
 */
static void
ip6_reass_free_complete_datagram(struct ip6_reassdata *ipr)
{
  struct ip6_reassdata **pipr;
  u16_t pbufs_freed = 0;
  u8_t clen;
  struct pbuf *p;
  struct ip6_reass_helper *iprh;

  /* First, free all received pbufs.  The individual pbufs need to be released
     separately as they have not yet been chained */
  iprh = ipr->iprh;
  while (iprh != NULL) {
    struct ip6_reass_helper *next = iprh->next;
    p = iprh->p;

#if LWIP_ICMP6
    /* If the first fragment was received, send ICMP time exceeded. */
    if (iprh->start == 0) {
      SMEMCPY(ipr->iphdr0, &ipr->iphdr, IP6_HLEN);
      if (pbuf_header(p, (u8_t *)p->payload - (u8_t *)ipr->iphdr0) == 0) {
        icmp6_time_exceeded(p, ICMP6_TE_FRAG);
      }
      else {
        LWIP_ASSERT("ip6_reass_free: moving p->payload to ip6 header failed\r\n", 0);
      }
    }
#endif /* LWIP_ICMP6 */

    clen = pbuf_clen(p);
    LWIP_ASSERT("pbufs_freed + clen <= 0xffff", pbufs_freed + clen <= 0xffff);
    pbufs_freed += clen;
    pbuf_free(p);

    iprh = next;
  }

  /* Then, unchain the struct ip6_reassdata from the list and free it. */
  for (pipr = &reassdatagrams; *pipr != NULL; pipr = &(*pipr)->next) {
    if (*pipr == ipr) {
      (*pipr) = ipr->next;
      break;
    }
  }
  memp_free(MEMP_IP6_REASSDATA, ipr);

  /* Finally, update number of pbufs in reassembly queue */
  LWIP_ASSERT("ip_reass_pbufcount >= clen", ip6_reass_pbufcount >= pbufs_freed);
  ip6_reass_pbufcount -= pbufs_freed;
}

#if IP_REASS_FREE_OLDEST
/**
 * Free the oldest datagram to make room for enqueueing new fragments.
 * The datagram ipr is not freed!
 *
 * @param ipr ip6_reassdata for the current fragment
 * @param pbufs_needed number of pbufs needed to enqueue
 *        (used for freeing other datagrams if not enough space)
 */
static void
ip6_reass_remove_oldest_datagram(struct ip6_reassdata *ipr, int pbufs_needed)
{
  struct ip6_reassdata *r, *oldest;

  /* Free datagrams until being allowed to enqueue 'pbufs_needed' pbufs,
   * but don't free the current datagram! */
  do {
    r = oldest = reassdatagrams;
    while (r != NULL) {
      if (r != ipr) {
        if (r->timer <= oldest->timer) {
          /* older than the previous oldest */
          oldest = r;
        }
      }
      r = r->next;
    }
    if (oldest != NULL) {
      ip6_reass_free_complete_datagram(oldest);
    }
  } while (((ip6_reass_pbufcount + pbufs_needed) > IP_REASS_MAX_PBUFS) && (reassdatagrams != NULL));
}
#endif /* IP_REASS_FREE_OLDEST */

/**
 * Reassembles incoming IPv6 fragments into an IPv6 datagram.
 *
 * @param p points to the IPv6 Fragment Header
 * @param len the length of the payload (after Fragment Header)
 * @return NULL if reassembly is incomplete, pbuf pointing to
 *         IPv6 Header if reassembly is complete
 */
struct pbuf *
ip6_reass(struct pbuf *p)
{
  struct ip6_reassdata *ipr, **pipr;
  struct ip6_reass_helper *iprh, *iprh_tmp;
  struct ip6_reass_helper **pnext;
  struct ip6_frag_hdr * frag_hdr;
  size_t unfrag_len;
  u16_t offset, len, start, end, validlen;
  u8_t clen;

  IP6_FRAG_STATS_INC(ip6_frag.recv);

  frag_hdr = (struct ip6_frag_hdr *) p->payload;

  clen = pbuf_clen(p);

  offset = ntohs(frag_hdr->_fragment_offset);

  /* Calculate fragment length from IPv6 payload length.
   * Adjust for headers before Fragment Header.
   * And finally adjust by Fragment Header length. */
  len = ntohs(ip6_current_header()->_plen);
  len -= ((u8_t*)p->payload - (u8_t*)ip6_current_header()) - IP6_HLEN;
  len -= IP6_FRAG_HLEN;

  start = (offset & IP6_FRAG_OFFSET_MASK);
  end = start + len;


  /* Look for the datagram the fragment belongs to in the current datagram queue,
   * remembering the previous in the queue for later dequeueing. */
  for (ipr = reassdatagrams; ipr != NULL; ipr = ipr->next) {
    /* Check if the incoming fragment matches the one currently present
       in the reassembly buffer. If so, we proceed with copying the
       fragment into the buffer. */
    if ((frag_hdr->_identification == ipr->identification) &&
        ip6_addr_cmp(ip6_current_src_addr(), &(ipr->iphdr.src)) &&
        ip6_addr_cmp(ip6_current_dest_addr(), &(ipr->iphdr.dest))) {
      IP6_FRAG_STATS_INC(ip6_frag.cachehit);
      break;
    }
  }

  if (ipr == NULL) {
  /* Enqueue a new datagram into the datagram queue */
    ipr = (struct ip6_reassdata *)memp_malloc(MEMP_IP6_REASSDATA);
    if (ipr == NULL) {
#if IP_REASS_FREE_OLDEST
      /* Make room and try again. */
      ip6_reass_remove_oldest_datagram(ipr, clen);
      ipr = (struct ip6_reassdata *)memp_malloc(MEMP_IP6_REASSDATA);
      if (ipr == NULL)
#endif /* IP_REASS_FREE_OLDEST */
      {
        IP6_FRAG_STATS_INC(ip6_frag.memerr);
        IP6_FRAG_STATS_INC(ip6_frag.drop);
        goto nullreturn;
      }
    }

    memset(ipr, 0, sizeof(struct ip6_reassdata));
    ipr->timer = IP_REASS_MAXAGE;

    /* enqueue the new structure to the front of the list */
    ipr->next = reassdatagrams;
    reassdatagrams = ipr;

    /* Use the current IPv6 header for src/dest address reference.
     * Eventually, we will replace it when we get the first fragment
     * (it might be this one, in any case, it is done later). */
    SMEMCPY(&ipr->iphdr, ip6_current_header(), IP6_HLEN);
    if (start == 0) {
      ipr->iphdr0 = (struct ip6_hdr *)ip6_current_header();
    }

    /* copy the fragmented packet id. */
    ipr->identification = frag_hdr->_identification;
  }

  /* If this is the last fragment, save total packet length. */
  if ((offset & IP6_FRAG_MORE_FLAG) == 0) {
#if IP_REASS_CHECK_OVERLAP
    if (ipr->datagram_len != 0) {
      IP6_FRAG_STATS_INC(ip6_frag.proterr);
      IP6_FRAG_STATS_INC(ip6_frag.drop);
      goto nullreturn;
    }
#endif /* IP_REASS_CHECK_OVERLAP */
    ipr->datagram_len = end;
  }

  /* find the place to insert this pbuf */
  validlen = 0;
  for (pnext = &ipr->iprh; *pnext != NULL; pnext = &(*pnext)->next) {
    iprh_tmp = *pnext;

    if (start < iprh_tmp->start) {
      /* the new pbuf should be inserted before this */
#if IP_REASS_CHECK_OVERLAP
      if (end > iprh_tmp->start) {
        /* fragment overlaps with following, throw away */
        IP6_FRAG_STATS_INC(ip6_frag.proterr);
        IP6_FRAG_STATS_INC(ip6_frag.drop);
        goto nullreturn;
      }
#endif /* IP_REASS_CHECK_OVERLAP */
      break;
    }
    else if (start == iprh_tmp->start) {
      /* received the same datagram twice: no need to keep the datagram */
      IP6_FRAG_STATS_INC(ip6_frag.drop);
      goto nullreturn;
    }
#if IP_REASS_CHECK_OVERLAP
    else if (start < iprh_tmp->end) {
      /* overlap: no need to keep the new datagram */
      IP6_FRAG_STATS_INC(ip6_frag.proterr);
      IP6_FRAG_STATS_INC(ip6_frag.drop);
      goto nullreturn;
    }
#endif /* IP_REASS_CHECK_OVERLAP */
    else {
      /* Check if the fragments received so far have no gaps. */
      if (validlen == iprh_tmp->start) {
        validlen = iprh_tmp->end;
      }
      else {
        validlen = 0;
      }
    }
  }

  /* Check if we are allowed to enqueue more datagrams. */
  if ((ip6_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS) {
#if IP_REASS_FREE_OLDEST
    ip6_reass_remove_oldest_datagram(ipr, clen);
    if ((ip6_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS)
#endif /* IP_REASS_FREE_OLDEST */
    {
      /* @todo: send ICMPv6 time exceeded here? */
      /* drop this pbuf */
      IP6_FRAG_STATS_INC(ip6_frag.memerr);
      IP6_FRAG_STATS_INC(ip6_frag.drop);
      goto nullreturn;
    }
  }

  if (start == 0 && ipr->iphdr0 == NULL) {
    /*
     * We've got the fragment with offset 0 out of order, remember its
     * IPv6 header location (in the hidden part of the current pbuf)
     * and update the copy in ip6_reassdata::iphdr.  We don't need to
     * copy complete header since src and dest are the same as in the
     * first fragment we received.
     */
    ipr->iphdr0 = (struct ip6_hdr *)ip6_current_header();
    SMEMCPY(&ipr->iphdr, ip6_current_header(),
            IP6_HLEN - 2 * sizeof(ip_addr_p_t));
  }

  /* Overwrite IPv6 Header with our own helper struct (aligned). */
  iprh = (struct ip6_reass_helper *)
    (((uintptr_t)(u8_t *)ip6_current_header() + sizeof(void *) - 1)
     & ~(sizeof(void *) - 1));
  iprh->p = p;
  iprh->start = start;
  iprh->end = end;

  /* insert it into the list */
  iprh->next = *pnext;
  *pnext = iprh;

  /* Track the current number of pbufs current 'in-flight', in order to limit
  the number of fragments that may be enqueued at any one time */
  ip6_reass_pbufcount += clen;

  if (ipr->datagram_len == 0) {
    /* We still don't have the last fragment. */
    return NULL;
  }

  if (validlen == start) {
    validlen = end;
  }
  else {
    /* There are gaps before this fragment. */
    return NULL;
  }

  if (validlen != 0) {
    /*
     * We know we have all the data up to the end of this fragment and
     * we know the total length.  Check if the reassembly is complete.
     */
    for (iprh_tmp = iprh->next; iprh_tmp != NULL; iprh_tmp = iprh_tmp->next) {
      if (validlen == iprh_tmp->start) {
        validlen = iprh_tmp->end;
      }
      else {
        validlen = 0;
        break;
      }
    }

    if (validlen != ipr->datagram_len) {
      /* the datagram is not yet reassembled completely */
      return NULL;
    }
  }

  /*
   * All fragments have been received.  Reassemble original datagram
   * and return it to ip6_input() to be processed instead of the final
   * fragment that completed the reassembly.
   */

  /* chain together the pbufs contained within the ip6_reassdata list. */
  p = NULL;
  for (iprh = ipr->iprh; iprh != NULL; iprh = iprh->next) {
    if (p == NULL) {
      p = iprh->p;
    }
    else {
      /* hide the fragment header for every succeeding fragment */
      pbuf_header(iprh->p, -IP6_FRAG_HLEN);
      pbuf_cat(p, iprh->p);
    }
  }

  /* Adjust datagram length by adding preceding header lengths. */
  unfrag_len = (u8_t *)p->payload - (u8_t *)ipr->iphdr0;
  ipr->datagram_len += unfrag_len - IP6_HLEN + IP6_FRAG_HLEN;

  /* Set payload length in ip header. */
  ipr->iphdr._plen = htons(ipr->datagram_len);

  /* restore IPv6 header (overwritten with ip6_reass_helper) */
  SMEMCPY(ipr->iphdr0, &ipr->iphdr, IP6_HLEN);

  /* Mark as "single fragment" packet (see caller). */
  frag_hdr = (struct ip6_frag_hdr *) p->payload;
  frag_hdr->_fragment_offset = 0;

  /* Unlink from the reassdatagrams list */
  for (pipr = &reassdatagrams; *pipr != NULL; pipr = &(*pipr)->next) {
    if (*pipr == ipr) {
      (*pipr) = ipr->next;
      break;
    }
  }
  memp_free(MEMP_IP6_REASSDATA, ipr);

  /* adjust the number of pbufs currently queued for reassembly. */
  ip6_reass_pbufcount -= pbuf_clen(p);

  /* Move pbuf back to IPv6 header. */
  if (pbuf_header(p, unfrag_len) != 0) {
    LWIP_ASSERT("ip6_reass: moving p->payload to ip6 header failed\r\n", 0);
    goto nullreturn;
  }

  /* Return the pbuf chain */
  return p;

nullreturn:
  pbuf_free(p);
  return NULL;
}

#endif /* LWIP_IPV6 && LWIP_IPV6_REASS */

#if LWIP_IPV6 && LWIP_IPV6_FRAG

/** Allocate a new struct pbuf_custom_ref */
static struct pbuf_custom_ref*
ip6_frag_alloc_pbuf_custom_ref(void)
{
  return (struct pbuf_custom_ref*)memp_malloc(MEMP_FRAG_PBUF);
}

/** Free a struct pbuf_custom_ref */
static void
ip6_frag_free_pbuf_custom_ref(struct pbuf_custom_ref* p)
{
  LWIP_ASSERT("p != NULL", p != NULL);
  memp_free(MEMP_FRAG_PBUF, p);
}

/** Free-callback function to free a 'struct pbuf_custom_ref', called by
 * pbuf_free. */
static void
ip6_frag_free_pbuf_custom(struct pbuf *p)
{
  struct pbuf_custom_ref *pcr = (struct pbuf_custom_ref*)p;
  LWIP_ASSERT("pcr != NULL", pcr != NULL);
  LWIP_ASSERT("pcr == p", (void*)pcr == (void*)p);
  if (pcr->original != NULL) {
    pbuf_free(pcr->original);
  }
  ip6_frag_free_pbuf_custom_ref(pcr);
}

/**
 * Fragment an IPv6 datagram if too large for the netif or path MTU.
 *
 * Chop the datagram in MTU sized chunks and send them in order
 * by pointing PBUF_REFs into p
 *
 * @param p ipv6 packet to send
 * @param netif the netif on which to send
 * @param dest destination ipv6 address to which to send
 *
 * @return ERR_OK if sent successfully, err_t otherwise
 */
err_t
ip6_frag(struct pbuf *p, struct netif *netif, ip6_addr_t *dest)
{
  struct ip6_hdr *original_ip6hdr;
  struct ip6_hdr *ip6hdr;
  struct ip6_frag_hdr * frag_hdr;
  struct pbuf *rambuf;
  struct pbuf *newpbuf;
  static u32_t identification;
  u16_t nfb;
  u16_t left, cop;
  u16_t mtu;
  u16_t fragment_offset = 0;
  u16_t last;
  u16_t poff = IP6_HLEN;
  u16_t newpbuflen = 0;
  u16_t left_to_copy;

  identification++;

  original_ip6hdr = (struct ip6_hdr *)p->payload;

  mtu = nd6_get_destination_mtu(dest, netif);

  /* TODO we assume there are no options in the unfragmentable part (IPv6 header). */
  left = p->tot_len - IP6_HLEN;

  nfb = (mtu - (IP6_HLEN + IP6_FRAG_HLEN)) & IP6_FRAG_OFFSET_MASK;

  while (left) {
    last = (left <= nfb);

    /* Fill this fragment */
    cop = last ? left : nfb;

    /* When not using a static buffer, create a chain of pbufs.
     * The first will be a PBUF_RAM holding the link, IPv6, and Fragment header.
     * The rest will be PBUF_REFs mirroring the pbuf chain to be fragged,
     * but limited to the size of an mtu.
     */
    rambuf = pbuf_alloc(PBUF_LINK, IP6_HLEN + IP6_FRAG_HLEN, PBUF_RAM);
    if (rambuf == NULL) {
      IP6_FRAG_STATS_INC(ip6_frag.memerr);
      return ERR_MEM;
    }
    LWIP_ASSERT("this needs a pbuf in one piece!",
                (p->len >= (IP6_HLEN + IP6_FRAG_HLEN)));
    SMEMCPY(rambuf->payload, original_ip6hdr, IP6_HLEN);
    ip6hdr = (struct ip6_hdr *)rambuf->payload;
    frag_hdr = (struct ip6_frag_hdr *)((u8_t*)rambuf->payload + IP6_HLEN);

    /* Can just adjust p directly for needed offset. */
    p->payload = (u8_t *)p->payload + poff;
    p->len -= poff;
    p->tot_len -= poff;

    left_to_copy = cop;
    while (left_to_copy) {
      struct pbuf_custom_ref *pcr;
      newpbuflen = (left_to_copy < p->len) ? left_to_copy : p->len;
      /* Is this pbuf already empty? */
      if (!newpbuflen) {
        p = p->next;
        continue;
      }
      pcr = ip6_frag_alloc_pbuf_custom_ref();
      if (pcr == NULL) {
        pbuf_free(rambuf);
        IP6_FRAG_STATS_INC(ip6_frag.memerr);
        return ERR_MEM;
      }
      /* Mirror this pbuf, although we might not need all of it. */
      newpbuf = pbuf_alloced_custom(PBUF_RAW, newpbuflen, PBUF_REF, &pcr->pc, p->payload, newpbuflen);
      if (newpbuf == NULL) {
        ip6_frag_free_pbuf_custom_ref(pcr);
        pbuf_free(rambuf);
        IP6_FRAG_STATS_INC(ip6_frag.memerr);
        return ERR_MEM;
      }
      pbuf_ref(p);
      pcr->original = p;
      pcr->pc.custom_free_function = ip6_frag_free_pbuf_custom;

      /* Add it to end of rambuf's chain, but using pbuf_cat, not pbuf_chain
       * so that it is removed when pbuf_dechain is later called on rambuf.
       */
      pbuf_cat(rambuf, newpbuf);
      left_to_copy -= newpbuflen;
      if (left_to_copy) {
        p = p->next;
      }
    }
    poff = newpbuflen;

    /* Set headers */
    frag_hdr->_nexth = original_ip6hdr->_nexth;
    frag_hdr->reserved = 0;
    frag_hdr->_fragment_offset = htons((fragment_offset & IP6_FRAG_OFFSET_MASK) | (last ? 0 : IP6_FRAG_MORE_FLAG));
    frag_hdr->_identification = htonl(identification);

    IP6H_NEXTH_SET(ip6hdr, IP6_NEXTH_FRAGMENT);
    IP6H_PLEN_SET(ip6hdr, cop + IP6_FRAG_HLEN);

    /* No need for separate header pbuf - we allowed room for it in rambuf
     * when allocated.
     */
    IP6_FRAG_STATS_INC(ip6_frag.xmit);
    netif->output_ip6(netif, rambuf, dest);

    /* Unfortunately we can't reuse rambuf - the hardware may still be
     * using the buffer. Instead we free it (and the ensuing chain) and
     * recreate it next time round the loop. If we're lucky the hardware
     * will have already sent the packet, the free will really free, and
     * there will be zero memory penalty.
     */

    pbuf_free(rambuf);
    left -= cop;
    fragment_offset += cop;
  }
  return ERR_OK;
}

#endif /* LWIP_IPV6 && LWIP_IPV6_FRAG */
