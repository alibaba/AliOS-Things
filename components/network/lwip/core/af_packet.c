/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "lwip/opt.h"

#if LWIP_PACKET
#include "lwip/sockets.h"
#include "lwip/af_packet.h"
#include "lwip/pbuf.h"
#include "netif/ethernet.h"
#include "lwip/netif.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/debug.h"

#include <string.h>

#define PACKET_THREAD_PRIO  ( tskIDLE_PRIORITY + 3 )

#define ARPHRD_ETHER    1       /* ethernet hardware format */

#ifndef ETH_HWADDR_LEN
#define ETH_HWADDR_LEN    6
#endif

#ifndef ETHARP_HWADDR_LEN
#define ETHARP_HWADDR_LEN     ETH_HWADDR_LEN
#endif

#define PACKET_HOST         0                                           /*  To us                       */
#define PACKET_BROADCAST    1                                           /*  To all                      */
#define PACKET_MULTICAST    2                                           /*  To group                    */
#define PACKET_OTHERHOST    3                                           /*  To someone else             */
#define PACKET_OUTGOING     4                                           /*  Originated by us            */

#define DEFAULT_PKTMBOX_SIZE 8

#define ETH_HLEN            14
#define ETH_FRAME_LEN       1514
#define ETH_DATA_LEN        1500

#define PACKETS_DEBUG                   LWIP_DBG_OFF
//#define PACKETS_ERR                     LWIP_DBG_ON

/** The global array of available sockets */
static struct packet_sock psockets[NUM_PACKET_SOCKETS];

#if LWIP_SOCKET_SET_ERRNO
#ifndef set_errno
#define set_errno(err) do { if (err) { errno = (err); } } while(0)
#endif
#else /* LWIP_SOCKET_SET_ERRNO */
#define set_errno(err)
#endif /* LWIP_SOCKET_SET_ERRNO */

#define sock_set_errno(sk, e) do { \
  const int sockerr = (e); \
  sk->err = (u8_t)sockerr; \
  set_errno(sockerr); \
} while (0)

int
packet_eth_hdr_info(struct pbuf* p, struct sockaddr_ll *sll, int ifindex, pkt_direct direct);

err_t
packet_input_hook (struct pbuf* p, struct netif *inp);

err_t
packet_output_hook (struct pbuf* p, struct netif *inp);

struct packet_sock*
tryget_pkt_socket(int s)
{
  s -= LWIP_PACKET_SOCKET_OFFSET;
  if ((s < 0) || (s >= NUM_PACKET_SOCKETS)) {
    set_errno(EBADF);
    return NULL;
  }

  if (!psockets[s].is_used) {
    set_errno(EBADF);
    return NULL;
  }
  return &psockets[s];
}

struct packet_sock*
get_pkt_socket(int s)
{
  struct packet_sock *sock;

  LWIP_DEBUGF(PACKETS_DEBUG, ("get_pkt_socket(%d) LWIP_PACKET_SOCKET_OFFSET(%d)\n", s, LWIP_PACKET_SOCKET_OFFSET));
  s -= LWIP_PACKET_SOCKET_OFFSET;

  if ((s < 0) || (s >= NUM_PACKET_SOCKETS)) {
    LWIP_DEBUGF(PACKETS_DEBUG, ("get_pkt_socket(%d): invalid\n", s + LWIP_SOCKET_OFFSET));
    set_errno(EBADF);
    return NULL;
  }

  sock = &psockets[s];

  if (!sock->is_used) {
    LWIP_DEBUGF(PACKETS_DEBUG, ("get_socket(%d): not active\n", s + LWIP_SOCKET_OFFSET));
    set_errno(EBADF);
    return NULL;
  }

  return sock;
}

/* This function maybe have some risk */
int
get_pkt_socket_by_netif(struct netif* netif)
{
    int i;

    /* allocate a new socket identifier */
    for (i = 0; i < NUM_PACKET_SOCKETS; ++i)
    {
        LWIP_DEBUGF(PACKETS_DEBUG, ("get_pkt_socket_by_netif netif(%c%c) %p netif_num %d" 
                           "psockets[%d].ifindex %d is_used[%d] netif_find_by_index %p\n",
                           netif->name[0], netif->name[1],(void*)netif,  netif->num, i, 
                           (int) psockets[i].ifindex, psockets[i].is_used, 
                           (void*)netif_find_by_index(psockets[i].ifindex)));
        if (psockets[i].is_used)
        {
            LOCK_TCPIP_CORE();
            if((netif == netif_find_by_index(psockets[i].ifindex)))
            {
                UNLOCK_TCPIP_CORE();
                return i + LWIP_PACKET_SOCKET_OFFSET;
            }
            UNLOCK_TCPIP_CORE();
        }
    }
    set_errno(EBADF);
    return -1;
}

int
alloc_pkt_socket(int type, int protocol)
{
    int i;

    LWIP_DEBUGF(PACKETS_DEBUG, ("alloc_pkt_socket (type=%d, protocol=%d)\n", type, protocol));

    for (i = 0; i < NUM_PACKET_SOCKETS; ++i)
    {
        if(psockets[i].is_used == 1)
        {
            continue;
        }

        /* choose the right psockets */
        psockets[i].flag     = 0;
        psockets[i].ifindex  = 0;
        psockets[i].hdrlen   = 0;
        psockets[i].reserve  = 0;

        if(sys_mbox_new(&psockets[i].pktmbox, DEFAULT_PKTMBOX_SIZE) != ERR_OK )
        {
            LWIP_DEBUGF(PACKETS_DEBUG, ("alloc_pkt_socket pktmbox(%d) new failed\n", i));
            return -1;
        }

        psockets[i].is_used      = 1;
        psockets[i].recv_output  = false;
        psockets[i].type         = type;
        psockets[i].protocol     = protocol;
        return i+ LWIP_PACKET_SOCKET_OFFSET;
    }

    return -1;
}

int
free_pkt_socket(int s)
{
    struct packet_sock* sock;

    LWIP_DEBUGF(PACKETS_DEBUG, ("free_pkt_socket %d\n", s));
    sock = tryget_pkt_socket(s);
    if (!sock)
    {
        LWIP_DEBUGF(PACKETS_DEBUG, ("free_pkt_socket %d failed\n", s));
        return -1;
    }

    sys_mbox_free(&sock->pktmbox);
    sock->is_used = 0;
    sock->ifindex = 0;
    sock->type    = 0;
    sock->protocol= 0;
    sock->recv_output = false;
    sock->err = 0;

    return 0;
}

int
packet_socket(int domain, int type, int protocol)
{
    int s;

    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_socket (%d, %d, %d)\n", domain, type, protocol));
    if(domain != AF_PACKET)
    {
        return -1;
    }

    if((type != SOCK_DGRAM)&&
       (type != SOCK_RAW))
    {
        return -1;
    }

    s = alloc_pkt_socket(type, protocol);
    if(s == -1)
    {
        return -1;
    }

    return s;
}

int
packet_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
  struct packet_sock *sock;
  struct sockaddr_ll* addrll = (struct sockaddr_ll *) name;

  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_bind (%d, 0x%x, %d)\n", s, (unsigned int)name, (int)namelen));
  sock = get_pkt_socket(s);
  if (!sock) {
    return -1;
  }

  if(!name ||(namelen < sizeof(struct sockaddr_ll)))
  {
    return -1;
  }

  sock->ifindex = addrll->sll_ifindex;

  return 0;
}

int
packet_listen(int s, int backlog)
{
  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_listen is not supported\n"));
  /* not support */
  return -1;
}

int
packet_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_accept is not supported\n"));
  /* not support */
  return -1;
}

int
packet_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_connect is not supported\n"));
  /* not support */
  return -1;
}

int
packet_recvfrom(int s, void *mem, size_t len, int flags,
                struct sockaddr *from, socklen_t *fromlen)
{
    struct packet_sock *sock;
    struct pbuf      *p;
    void             *recv_mem = NULL;
    int              ret_len = -1;
    struct sockaddr_ll *sll = (struct sockaddr_ll *) from;
    int              hdr_len;
    int              msg_len;
    int              copy_offset;
    struct tcpip_msg *msg;
    pkt_direct       direct;

    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_recvfrom(%d, %p, %"SZT_F", 0x%x, ..)\n", s, mem, len, flags));
    sock = get_pkt_socket(s);

    if (!sock)
    {
        return -1;
    }

    if (!mem || !len)
    {
        return  -1;
    }

    if (from && fromlen && (*fromlen < sizeof(struct sockaddr_ll)))
    {
         return  -1;
    }

    /* check if got rcvevent */
    if ((flags & MSG_DONTWAIT) && (sock->rcvevent <= 0))
    {
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_recvfrom(%d): returning EWOULDBLOCK\n", s));
        sock_set_errno(sock, EWOULDBLOCK);
        return -1;
    }

    while(sys_mbox_valid(&sock->pktmbox))
    {
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_recvfrom try to fetch data from sock %p pktmbox %p\n", 
                                    (void*)sock, (void*)&sock->pktmbox));
#if 0
        if(SYS_MBOX_EMPTY == sys_arch_mbox_tryfetch(&sock->pktmbox, &recv_mem))
        {
            sock_set_errno(sock, EWOULDBLOCK);
            return -1;
        }
#endif
        sys_arch_mbox_fetch(&sock->pktmbox, &recv_mem, 0);
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_recvfrom recvmem %p\n", (void*)recv_mem));
        
        if ((mem != NULL) && (recv_mem != NULL))
        {
            msg = (struct tcpip_msg*)recv_mem;
            packet_select_action(s, PACKET_ACTION_EVTRCVMINUS);
	    if(msg->type != TCPIP_MSG_INPKT)
            {
                memp_free(MEMP_TCPIP_MSG_INPKT, msg);
                return -1;
            }
            p = msg->msg.inp.p;
            if(msg->msg.inp.input_fn == packet_input_hook)
            {
                direct = INCOMING;
            }
            else if(msg->msg.inp.input_fn == packet_output_hook)
            {
                direct = OUTGOING;
            }
            else
            {
                pbuf_free(p);
	        memp_free(MEMP_TCPIP_MSG_INPKT, msg);
                return -1;
            }

            hdr_len = packet_eth_hdr_info(p, sll, sock->ifindex, direct);
            if(sock->type == SOCK_DGRAM)
            {
                msg_len = p->tot_len - hdr_len - ETH_PAD_SIZE;
                copy_offset = (u16_t)(hdr_len + ETH_PAD_SIZE);
            }
            else
            {
                msg_len = p->tot_len - ETH_PAD_SIZE;
                copy_offset = ETH_PAD_SIZE;
            }
            if(msg_len > len)
            {
                pbuf_copy_partial(p, mem, (u16_t)len, copy_offset);
                ret_len = len;
            }
            else
            {
                pbuf_copy_partial(p, mem, (u16_t)msg_len, copy_offset);
                ret_len = msg_len;
            }
            pbuf_free(p);
            memp_free(MEMP_TCPIP_MSG_INPKT, msg);
            break;
        }
    }

    if(ret_len > 0)
    {
        if(fromlen)
        {
            sock_set_errno(sock, 0);
            *fromlen = sizeof(struct sockaddr_ll);
        }
    }

    return ret_len;
}

err_t
packet_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout)
{
    return -1;;
}

int packet_select_action(int s, packet_action_t action)
{
    packet_sock *sock;
    int err = 0;

    sock = get_pkt_socket(s);
    if (!sock)
    {   
        return -1;
    }

    SYS_ARCH_DECL_PROTECT(lev);
    SYS_ARCH_PROTECT(lev);
    switch(action)
    {
     case PACKET_ACTION_EVTRCVPLUS:
        sock->rcvevent++;
        break;
     case PACKET_ACTION_EVTRCVMINUS:
        sock->rcvevent--;
        break;
     case PACKET_ACTION_SELWAITPLUS:
        sock->select_waiting++;
        break;
     case PACKET_ACTION_SELWAITMINUS:
        sock->select_waiting--;
        break;
     default:
        err = -1;
        break;
    }

    if (sock->select_waiting == 0) {
       SYS_ARCH_UNPROTECT(lev);
       /* noone is waiting for this socket, no need to check select_cb_list */
       return 0;
    }
    SYS_ARCH_UNPROTECT(lev);
    /* af_packet not support sendevent and errevent */
    lwip_try_wakeup(s, sock->rcvevent, 0, 0);

    return err;
}

int
packet_selscan(int maxfdp1, fd_set *readset_in, fd_set *writeset_in, fd_set *exceptset_in,
             fd_set *readset_out, fd_set *writeset_out, fd_set *exceptset_out)
{
    int i, nready = 0;
    fd_set lreadset;
    struct packet_sock *sock;

    /* packet_selscan do not support write or except event, just warning. */
    if(writeset_in != NULL || exceptset_in != NULL)
    {
       LWIP_DEBUGF(PACKETS_DEBUG, ("af_packet do not support write or except sets."));
    }

    SYS_ARCH_DECL_PROTECT(lev);

    FD_ZERO(&lreadset);

    /* Go through each socket in each list to count number of sockets which
     currently match */
    for (i = LWIP_PACKET_SOCKET_OFFSET; i < maxfdp1; i++) {
    /* if this FD is not in the set, continue */
    if (!(readset_in && FD_ISSET(i, readset_in))) {
      continue;
    }
    /* First get the socket's status (protected)... */
    SYS_ARCH_PROTECT(lev);
    sock = tryget_pkt_socket(i);
    if (sock != NULL) {
      s16_t rcvevent = sock->rcvevent;
      SYS_ARCH_UNPROTECT(lev);
      /* See if this socket is ready for read */
      if (readset_in && FD_ISSET(i, readset_in) && (rcvevent > 0)) {
        FD_SET(i, &lreadset);
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_selscan: fd=%d ready for reading\n", i));
        nready++;
      }
    } else {
      SYS_ARCH_UNPROTECT(lev);
      /* continue on to next FD in list */
    }
  }

  /* copy local sets to the ones provided as arguments */
  *readset_out = lreadset;

  LWIP_ASSERT("nready >= 0", nready >= 0);
  return nready;
}

int
packet_recv(int s, void *mem, size_t len, int flags)
{
    return (packet_recvfrom(s, mem, len, flags, NULL, NULL));
}

err_t
packet_send(int s, const void *data, size_t size, int flags)
{
  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_send is not supported\n"));
//not support
    return -1;
}

/* check more about the details */
int
packet_eth_hdr_info(struct pbuf* p, struct sockaddr_ll *sll, int ifindex, pkt_direct direct)
{
    struct eth_hdr *eth_hdr = (struct eth_hdr *) p->payload;
    struct netif * netif;

    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_hdr_info p=0x%x, ssl=0x%x, ifindex=%d\n", (unsigned int)p, (unsigned int)sll, ifindex));
    if(sll)
    {
        sll->sll_len      = sizeof(struct sockaddr_ll);
        sll->sll_family   = AF_PACKET;
        sll->sll_protocol = eth_hdr->type;
        sll->sll_ifindex  = ifindex;
        sll->sll_hatype   = ARPHRD_ETHER;
        sll->sll_halen    = ETHARP_HWADDR_LEN;

        if(direct == OUTGOING)
        {
            sll->sll_addr[0] = eth_hdr->dest.addr[0];
            sll->sll_addr[1] = eth_hdr->dest.addr[1];
            sll->sll_addr[2] = eth_hdr->dest.addr[2];
            sll->sll_addr[3] = eth_hdr->dest.addr[3];
            sll->sll_addr[4] = eth_hdr->dest.addr[4];
            sll->sll_addr[5] = eth_hdr->dest.addr[5];
        }
        else
        {
            /* direction is INCOMING */
            sll->sll_addr[0] = eth_hdr->src.addr[0];
            sll->sll_addr[1] = eth_hdr->src.addr[1];
            sll->sll_addr[2] = eth_hdr->src.addr[2];
            sll->sll_addr[3] = eth_hdr->src.addr[3];
            sll->sll_addr[4] = eth_hdr->src.addr[4];
            sll->sll_addr[5] = eth_hdr->src.addr[5];
        }

        netif = netif_find_by_index(ifindex);
        if(netif == NULL)
        {
            return -1;
        }

        if(direct == OUTGOING)
        {
            sll->sll_pkttype = PACKET_OUTGOING;
        }
        else if (eth_hdr->dest.addr[0] & 0x01) {
            if (eth_addr_cmp(&eth_hdr->dest, &ethbroadcast)) {
                sll->sll_pkttype = PACKET_BROADCAST;
            } else {
                sll->sll_pkttype = PACKET_MULTICAST;
            }
        }
        else if (memcmp(eth_hdr->dest.addr, netif->hwaddr,
                   ETHARP_HWADDR_LEN) == 0)
        {
            sll->sll_pkttype = PACKET_HOST;
        }
        else {
            sll->sll_pkttype = PACKET_OUTGOING;
        }
    }

    if (eth_hdr->type == PP_HTONS(ETHTYPE_VLAN)) {
        return  (ETH_HLEN + 4);
    } else {
        return  (ETH_HLEN);
    }
}

err_t
packet_eth_datagram_sendto(const void * data, size_t size, struct sockaddr_ll* sll)
{
   struct netif* netif;
   struct pbuf* pbuf_hdr;
   struct pbuf* pbuf_payload;
   struct eth_hdr * eth_hdr;
   err_t err;

   LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_datagram_sendto data=%p, size=%d, sll=%p\n", (void*)data, size, (void*)sll));
   if( size > ETH_DATA_LEN)
   {
       return -1;
   }
   LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_datagram_sendto sll_ifindex(%d)\n", sll->sll_ifindex));
   LOCK_TCPIP_CORE();
   netif = netif_find_by_index(sll->sll_ifindex);
   if(netif == NULL)
   {
      UNLOCK_TCPIP_CORE();
      return -1;
   }

   if(!netif_is_up(netif) || !netif_is_link_up(netif))
   {
      UNLOCK_TCPIP_CORE();
      return -1;
   }

    pbuf_hdr = pbuf_alloc(PBUF_RAW, ETH_HLEN + ETH_PAD_SIZE, PBUF_RAM); /*  alloc pkt header with pad       */
    if (pbuf_hdr == NULL) {
        UNLOCK_TCPIP_CORE();
        set_errno(ENFILE);
        return -1;
    }

    eth_hdr = (struct eth_hdr *)pbuf_hdr->payload;
    eth_hdr->dest.addr[0] = sll->sll_addr[0];
    eth_hdr->dest.addr[1] = sll->sll_addr[1];
    eth_hdr->dest.addr[2] = sll->sll_addr[2];
    eth_hdr->dest.addr[3] = sll->sll_addr[3];
    eth_hdr->dest.addr[4] = sll->sll_addr[4];
    eth_hdr->dest.addr[5] = sll->sll_addr[5];

    eth_hdr->src.addr[0] = netif->hwaddr[0];
    eth_hdr->src.addr[1] = netif->hwaddr[1];
    eth_hdr->src.addr[2] = netif->hwaddr[2];
    eth_hdr->src.addr[3] = netif->hwaddr[3];
    eth_hdr->src.addr[4] = netif->hwaddr[4];
    eth_hdr->src.addr[5] = netif->hwaddr[5];

    eth_hdr->type = sll->sll_protocol;

    pbuf_payload = pbuf_alloc(PBUF_RAW, (u16_t)size, PBUF_REF);
    if (pbuf_payload == NULL) {
        pbuf_free(pbuf_hdr);
        UNLOCK_TCPIP_CORE();
        set_errno(ENFILE);
        return  -1;
    }

    pbuf_payload->payload = (void *)data;

    pbuf_cat(pbuf_hdr, pbuf_payload);

    err = packet_output_hook(pbuf_hdr, netif);

    pbuf_free(pbuf_hdr);

    UNLOCK_TCPIP_CORE();
    if (err) {
        return  -1;
    } else {
        return  0;
    }
}

err_t
packet_eth_raw_sendto(const void * data, size_t size, struct sockaddr_ll* sll)
{
   struct netif* netif;
   struct pbuf* pbuf_hdr;
   struct pbuf* pbuf_payload;
   err_t err;

   LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto data=%p, size=%d, sll=%p\n", (void*)data, size, (void*)sll));
   if((size < ETH_HLEN) || (size > ETH_FRAME_LEN))
   {
      return -1;
   }

   LOCK_TCPIP_CORE();
   netif = netif_find_by_index(sll->sll_ifindex);
   if(netif == NULL)
   {
      LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto invalid netif index %d\n", (int)sll->sll_ifindex));
      UNLOCK_TCPIP_CORE();
      return -1;
   }

   if (!(netif->flags & (NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET)))
   {
      LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto invalid netif flags %d\n", (int)netif->flags));
      UNLOCK_TCPIP_CORE();
      return -1;
   }

   if (!netif_is_up(netif) || !netif_is_link_up(netif))
   {
      LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto netif_is_up(%d) netif_is_link_up(%d)\n", 
                                 netif_is_up(netif), netif_is_link_up(netif)));
      UNLOCK_TCPIP_CORE();
      return -1;
   }

#if ETH_PAD_SIZE
    pbuf_hdr = pbuf_alloc(PBUF_RAW, ETH_HLEN + ETH_PAD_SIZE, PBUF_RAM);
    if (pbuf_hdr == NULL) {
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto eth_pad alloc pbuf hdr failed\n"));
        UNLOCK_TCPIP_CORE();
        return  -1;
    }
    memcpy(((u8_t *)pbuf_hdr->payload) + ETH_PAD_SIZE, pvPacket, ETH_HLEN);

    pbuf_payload = pbuf_alloc(PBUF_RAW, (u16_t)(size - ETH_HLEN), PBUF_REF);
    if (pbuf_payload == NULL) {
        pbuf_free(pbuf_hdr);
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto eth_pad alloc pbuf payload failed\n"));
        UNLOCK_TCPIP_CORE();
        return  -1;
    }
    pbuf_payload->payload = (void *)((u8_t *)data + ETH_HLEN);

    pbuf_cat(pbuf_hdr, pbuf_dat);

#else
    (void)pbuf_payload;

    pbuf_hdr = pbuf_alloc(PBUF_RAW, (u16_t)size, PBUF_REF);
    if (pbuf_hdr == NULL) {
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto alloc pbuf hdr failed\n"));
        UNLOCK_TCPIP_CORE();
        return  -1;
    }
    pbuf_hdr->payload = (void *)data;
#endif

    err = packet_output_hook (pbuf_hdr, netif);

    pbuf_free(pbuf_hdr);
    UNLOCK_TCPIP_CORE();

    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_eth_raw_sendto err(%d)\n", err));
    if(err)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int
packet_sendto(int s, const void *data, size_t size, int flags,
       const struct sockaddr *to, socklen_t tolen)
{
  struct packet_sock *sock;
  struct sockaddr_ll* sll;
  int  err;

  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_sendto s(%d) data(0x%x) size(%d) flags(%d)" 
             "to(0x%x) tolen(%d)\n", s, (int)data, (int)size, flags, (int)to, (int)tolen));
  sock = get_pkt_socket(s);
  if (!sock) {
    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_sendto invalid s(%d)\n", s));
    return -1;
  }

  if(!data || !size)
  {
     LWIP_DEBUGF(PACKETS_DEBUG, ("packet_sendto invalid data or size\n"));
     return -1;
  }

  if(to && (tolen >= sizeof(struct sockaddr_ll)))
  {
     sll = (struct sockaddr_ll*) to;
  }
  else
  {
     LWIP_DEBUGF(PACKETS_DEBUG, ("packet_sendto invalid tolen(%d) no more than sll size(%d)\n", 
                    (int)tolen, sizeof(struct sockaddr_ll)));
     return -1;
  }

  /* set sock ifindex */
  sock->ifindex = sll->sll_ifindex;

  if(sock->type == SOCK_RAW)
  {
     err = packet_eth_raw_sendto(data, size, sll);
  }
  else
  {
     err = packet_eth_datagram_sendto(data, size, sll);
  }

  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_sendto err(%d)\n", err));
  if(err)
  {
     /* invalid sock ifindex */
     sock->ifindex = 0;
     return -1;
  }
  else
  {
     return (size);
  }
}

int
packet_sendmsg(int s, const struct msghdr *msg, int flags)
{
  return -1;
}

int packet_close(int s)
{
  LWIP_DEBUGF(PACKETS_DEBUG, ("packet_close(%d)\n", s));

  if(free_pkt_socket(s) == -1)
  {
     return -1;
  }

  set_errno(0);
  return 0;
}

int packet_shutdown(int s, int how)
{
   return 0;
}

int  packet_getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
   return 0;
}

int  packet_getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
   return 0;
}

int packet_setsockopt (int s, int level, int optname,
                        const void *optval, socklen_t optlen)
{
   int err = 0;
   struct packet_sock *sock;

   if(!optval || optlen < sizeof(int))
   {
       return -1;
   }

   sock = get_pkt_socket(s);
   if (!sock) {
     LWIP_DEBUGF(PACKETS_DEBUG, ("packet_setsockopt invalid s(%d)\n", s));
     return -1;
   }

   switch(level)
   {
      case SOL_PACKET:
          switch(optname)
          {
             case PACKET_ADD_MEMBERSHIP:
             case PACKET_DROP_MEMBERSHIP:
                //to do
                err = -1;
                break;
             case PACKET_RECV_OUTPUT:
                if(*(int*)optval)
                {
                    sock->recv_output = true;
                }
                else
                {
                    sock->recv_output = false;
                }
                break;
             default:
                LWIP_DEBUGF(PACKETS_DEBUG, ("optname %d is not support\n", optname));
                err = -1;
                break;
          }
          break;
      default:
          LWIP_DEBUGF(PACKETS_DEBUG, ("level %d is not support\n", level));
          err = -1;
          break;
   }

   return err;
}

int  packet_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
  return -1;
}

int  packet_ioctl (int s, int  cmd, void*  arg)
{
  return -1;
}

err_t
packet_input(struct pbuf *p, struct netif *inp, pkt_direct direct)
{
    struct eth_hdr       *eth_hdr = NULL;
    struct eth_vlan_hdr  *eth_vlan_hdr = NULL;
    int i;
    struct packet_sock *sock;
    struct pbuf *q;
    struct tcpip_msg *msg;
#if BYTE_ORDER == LITTLE_ENDIAN
    static uint16_t   pall = 0x0300;                                    /*  ETH_P_ALL                   */
#else
    static uint16_t   pall = 0x0003;
#endif
    bool              is_recv = false;

    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input %p/%p\n", (void*)p, (void*)inp));
    if(direct == OUTGOING)
    {
       LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input %p netif %p direct %d\n",(void*)p, (void*)inp, direct));
    }
    if((p->tot_len < (ETH_HLEN + ETH_PAD_SIZE)) ||
        !(inp->flags & (NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET)))
    {
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input tot_len %d flags %d \n",(int)p->tot_len, (int)inp->flags));
        return -1;
    }

    eth_hdr = (struct eth_hdr *)p->payload;

    if(eth_hdr->type == htons(ETHTYPE_VLAN))
    {
        eth_vlan_hdr = (struct eth_vlan_hdr *)(((char *)eth_hdr) + SIZEOF_ETH_HDR);
    } else {
        eth_vlan_hdr = NULL;
    }

    /* get the packet_sock that binding to the netif */
    i = get_pkt_socket_by_netif(inp);
    if(i == -1)
    {
       LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input invalid netif %p \n", (void*) inp));
       return -1;
    }

    //LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d, %p, %"SZT_F", 0x%x, ..)\n", s, mem, len, flags));
    sock = get_pkt_socket(i);

    if (!sock) {
      return -1;
    }

    if((direct == OUTGOING) && (sock->recv_output == false))
    {
      LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input sock->recv_output(%d)\n", (int) sock->recv_output));
      return -1;
    }
    if(eth_vlan_hdr)
    {
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input eth_vlan_hdr 0x%x \n", (int) eth_vlan_hdr));
        LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input left(%d, %d , %d) right (%d) \n", 
                              eth_hdr->type, eth_vlan_hdr->tpid, pall, sock->protocol));
        if((eth_hdr->type == sock->protocol)
          || (eth_vlan_hdr->tpid == sock->protocol)
          || (pall == sock->protocol))
        {
            is_recv = true;
        }
    }
    else
    {
         LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input left(%d, %d) right (%d) \n", eth_hdr->type, pall, sock->protocol));
         if((eth_hdr->type == sock->protocol)
           ||(pall == sock->protocol))
         {
             is_recv = true;
         }
    }

    if(is_recv == false)
    {
         LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input dont recv \n"));
         return -1;
    }

    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input p %p tot_len %d len %d\n", (void*)p, p->tot_len, p->len));
    /* alloc a new buffer */
    q = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_POOL);
    if(!q)
    {
         LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input pbuf alloc failed \n"));
         return -1;
    }else
    {
         if (pbuf_copy(q, p) != ERR_OK)
         {
             pbuf_free(q);
             q = NULL;
             return -1;
         }
    }

    msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_INPKT);
    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input alloc msg %p \n", msg));
    if (msg == NULL) {
       pbuf_free(q);
       LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input msg alloc failed \n"));
       return ERR_MEM;
    }

    msg->type = TCPIP_MSG_INPKT;
    msg->msg.inp.p = q;
    msg->msg.inp.netif = inp;

    if(direct == INCOMING)
    {
        msg->msg.inp.input_fn = packet_input_hook;
    }
    else
    {
        msg->msg.inp.input_fn = packet_output_hook;
    }
    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input sock %p pktmbox %p\n",
                               (void*)sock, (void*)&sock->pktmbox));
    if (sys_mbox_trypost(&sock->pktmbox, msg) != ERR_OK) {
       pbuf_free(q);
       LWIP_DEBUGF(PACKETS_DEBUG, ("packet_input trypost failed \n"));
       memp_free(MEMP_TCPIP_MSG_INPKT, msg);
       return ERR_MEM;
    }

    packet_select_action(i, PACKET_ACTION_EVTRCVPLUS);

    return 0;
}

err_t
packet_input_hook (struct pbuf* p, struct netif* inp)
{
    if (!netif_is_up(inp)) {
        return -1;
    }
    return packet_input(p, inp, INCOMING);
}

err_t
packet_output_hook (struct pbuf*p, struct netif* inp)
{
#if 0
    unsigned char *data = (unsigned char*)p->payload;
    LWIP_DEBUGF(PACKETS_DEBUG, ("===================dump output data start=========================\n"));
    LWIP_DEBUGF(PACKETS_DEBUG, ("af_packet_test send data len = %d\n", p->len));
    for(int j = 0; j <= p->len; j++){
    LWIP_DEBUGF(PACKETS_DEBUG, ("%02x", (unsigned char)data[j]));
    }
    LWIP_DEBUGF(PACKETS_DEBUG, ("\n====================dump output data end==========================\n"));
#endif
    LWIP_DEBUGF(PACKETS_DEBUG, ("packet_output_hook %p/%p\n", (void*)p, (void*)inp));
    packet_input(p, inp, OUTGOING);

    return inp->linkoutput(inp, p);
}

#endif /* LWIP_PACKET */
