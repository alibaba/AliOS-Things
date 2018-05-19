#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip/sys.h"

extern sys_sem_t lwip_sta_netif_linked;
extern sys_sem_t lwip_sta_netif_has_addr;
extern sys_sem_t lwip_ap_netif_linked;
extern struct netif lwip_sta_netif;
extern struct netif lwip_ap_netif;

const ip_addr_t * rda59xx_get_netif_ip(struct netif *netif)
{
    if (!netif_is_up(netif)) {
        return NULL;
    }

    if (!ip4_addr_isany(netif_ip4_addr(netif))) {
        return netif_ip_addr4(netif);;
    }

    return NULL;
}

static err_t rda59xx_etharp_output(struct netif *netif, struct pbuf *q, const ip_addr_t *ipaddr)
{
  /* Only send packet is link is up */
  if (netif->flags & NETIF_FLAG_LINK_UP)
    return etharp_output(netif, q, ipaddr);

  return ERR_CONN;
}

static err_t rda59xx_low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;

    /* rda5996 initiate transfer */
    u32_t actual_txlen = 0;
    u8_t **data;
    LWIP_DEBUGF(NETIF_DEBUG, ("rda59xx_low_level_output enter, p:%08x\n", p));

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    data = (void*)rda59xx_get_databuf();

    if(data == NULL){
        LWIP_DEBUGF(NETIF_DEBUG, ("rda59xx_low_level_output, no PKT buf\r\n"));
        return ERR_BUF;
    }

    for(q = p; q != NULL; q = q->next)
    {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        MEMCPY(&((*data)[actual_txlen+2]), q->payload, q->len);//reserve wid header length
        actual_txlen += q->len;
        if(actual_txlen > 1514 || actual_txlen > p->tot_len)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("rda59xx_low_level_output err, actual_txlen:%d, tot_len%d\n",
                    actual_txlen, p->tot_len));
            return ERR_BUF;
        }
    }

    /* Signal rda59xx that packet should be sent */
    if(actual_txlen == p->tot_len)
    {
        if(netif->name[0] == 's' && netif->name[1] == 't')
            rda59xx_txip_data((void*)data, actual_txlen, 0);
        else if(netif->name[0] == 'a' && netif->name[1] == 'p')
            rda59xx_txip_data((void*)data, actual_txlen, 1);

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.xmit);

        return ERR_OK;
    }

    LWIP_DEBUGF(NETIF_DEBUG, ("low_level_output pkt len mismatch, actual_txlen:%d, tot_len%d\n",
            actual_txlen, p->tot_len));


    return ERR_BUF;
}

static struct pbuf *rda59xx_low_level_input(struct netif *netif, u8_t *data, u32_t len)
{
    struct pbuf *p, *q;
    u16_t index = 0;

    LWIP_DEBUGF(NETIF_DEBUG, ("rda59xx_low_level_input enter, rxlen:%d\n", len));

    /* Obtain the size of the packet and put it into the "len"
     variable. */
    if(!len)
    {
        return NULL;
    }

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    if (p != NULL)
    {

#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for(q = p; q != NULL; q = q->next)
        {
            /* Read enough bytes to fill this pbuf in the chain. The
            * available data in the pbuf is given by the q->len
            * variable.
            * This does not necessarily have to be a memcpy, you can also preallocate
            * pbufs for a DMA-enabled MAC and after receiving truncate it to the
            * actually received size. In this case, ensure the tot_len member of the
            * pbuf is the sum of the chained pbuf len members.
            */
            /* load rx data from 96 to local mem_pool */
            MEMCPY(q->payload, &data[index], q->len);
            index += q->len;

            if(index >= len)
            {
                break;
            }
        }

        /* acknowledge rda5996 that packet has been read(); */
        //Nothing to do here

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    }
    else
    {
        /* Drop this packet */
        LWIP_DEBUGF(NETIF_DEBUG, ("rda59xx_low_level_input pbuf_alloc fail, rxlen:%d\n", len));
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);

        return NULL;
    }

    return p;
}

void rda59xx_netif_input(struct netif *netif, u8_t *data, u32_t len)
{
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = rda59xx_low_level_input(netif, data, len);
  if (p == NULL) {
    return;
  }

  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = (struct eth_hdr*)p->payload;

  switch (htons(ethhdr->type)) {
    case ETHTYPE_IP:
    case ETHTYPE_ARP:
#if PPPOE_SUPPORT
    case ETHTYPE_PPPOEDISC:
    case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
      /* full packet send to tcpip_thread to process */
      if (netif->input(p, netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("rda5991h_enetif_input: IP input error\n"));
        /* Free buffer */
        pbuf_free(p);
      }
      break;

    default:
      /* Return buffer */
      pbuf_free(p);
      break;
  }
}

//used for auth/assoc failed, inform upper timely
void rda59xx_sta_netif_link_failed()
{
    sys_sem_signal(&lwip_sta_netif_linked);
}

void rda59xx_netif_link_down(u32_t netif)
{
    if(netif == 0)
        netif_set_link_down(&lwip_sta_netif);
    else
        netif_set_link_down(&lwip_ap_netif);
}

void rda59xx_netif_link_up(u32_t netif)
{
    if(netif == 0)
        netif_set_link_up(&lwip_sta_netif);
    else
        netif_set_link_up(&lwip_ap_netif);
}

void rda59xx_netif_data_input(u8_t *data, u32_t len, u32_t idx, u32_t netif)
{
    if(netif == 0)
        rda59xx_netif_input(&lwip_sta_netif, data, len);
    else
        rda59xx_netif_input(&lwip_ap_netif, data, len);
}


static void rda59xx_sta_netif_link_irq(struct netif *lwip_netif)
{
    if (netif_is_link_up(lwip_netif)) {
        sys_sem_signal(&lwip_sta_netif_linked);
    }
}

static void rda59xx_sta_netif_status_irq(struct netif *lwip_netif)
{
    if (netif_is_up(lwip_netif) && rda59xx_get_netif_ip(lwip_netif) != NULL) {
            sys_sem_signal(&lwip_sta_netif_has_addr);
        }
    return;    
}


err_t rda59xx_sta_netif_init(struct netif *netif)
{
    netif->name[0] = 's';
    netif->name[1] = 't';
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    if(netif->hostname == NULL)
        netif->hostname = "rda59xx_sta";
#endif /* LWIP_NETIF_HOSTNAME */
    rda59xx_get_macaddr((u8_t *)(netif->hwaddr), 0);
    
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    // TODOETH: check if the flags are correct below
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;

    netif->output = rda59xx_etharp_output;
    netif->linkoutput = rda59xx_low_level_output;
    netif_set_link_callback(&lwip_sta_netif, rda59xx_sta_netif_link_irq);
    netif_set_status_callback(&lwip_sta_netif, rda59xx_sta_netif_status_irq);    
    netif_set_default(&lwip_sta_netif);
    return ERR_OK;
}

static void rda59xx_ap_netif_link_irq(struct netif *lwip_netif)
{
    if (netif_is_link_up(lwip_netif)) {
        sys_sem_signal(&lwip_ap_netif_linked);
    }
    return;
}

err_t rda59xx_ap_netif_init(struct netif *netif)
{
    netif->name[0] = 'a';
    netif->name[1] = 'p';
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    if(netif->hostname == NULL)
        netif->hostname = "rda59xx_ap";
#endif /* LWIP_NETIF_HOSTNAME */
    rda59xx_get_macaddr((u8_t *)(netif->hwaddr), 1);
    
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    // TODOETH: check if the flags are correct below
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;

    netif->output = rda59xx_etharp_output;
    netif->linkoutput = rda59xx_low_level_output;
    netif_set_link_callback(&lwip_ap_netif, rda59xx_ap_netif_link_irq);
    if(netif_default == NULL)
        netif_set_default(&lwip_ap_netif);
    
    return ERR_OK;
}


