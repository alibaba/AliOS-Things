#include "lwip/netifapi.h"
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
        return netif_ip_addr4(netif);
    }

    return NULL;
}

void rda59xx_add_dns_addr(struct netif *netif)
{
    // Do nothing if not brought up
    const ip_addr_t *ip_addr = rda59xx_get_netif_ip(netif);
    if(!ip_addr)
        return;
    // Check for existing dns server
    for (char numdns = 0; numdns < DNS_MAX_SERVERS; numdns++) {
        const ip_addr_t *dns_ip_addr = dns_getserver(numdns);
        if (!ip_addr_isany(dns_ip_addr)) {
            return;
        }
    }
   if (IP_IS_V4(ip_addr)) {
        /* 8.8.8.8 google */
        ip_addr_t ipv4_dns_addr = IPADDR4_INIT(0x08080808);
        dns_setserver(0, &ipv4_dns_addr);
    }
}

/** \brief  Low level init of the MAC and PHY.
 *
 *  \param[in]      netif  Pointer to LWIP netif structure
 */

static err_t rda59xx_low_level_init(struct netif *netif)
{
    static int init_flag = 0;
    if(init_flag == 0){
        //wland_reg_func();
        maclib_init();
        init_flag = 1;
    }
    return ERR_OK;
}

/**
 * This function is the ethernet packet send function. It calls
 * etharp_output after checking link status.
 *
 * \param[in] netif the lwip network interface structure for this enetif
 * \param[in] q Pointer to pbug to send
 * \param[in] ipaddr IP address
 * \return ERR_OK or error code
 */
static err_t rda59xx_etharp_output(struct netif *netif, struct pbuf *q, const ip_addr_t *ipaddr)
{
  /* Only send packet is link is up */
  if (netif->flags & NETIF_FLAG_LINK_UP)
    return etharp_output(netif, q, ipaddr);

  return ERR_CONN;
}

/** \brief  Low level output of a packet. Never call this from an
 *          interrupt context, as it may block until TX descriptors
 *          become available.
 *
 *  \param[in] netif the lwip network interface structure for this netif
 *  \param[in] p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 *  \return ERR_OK if the packet could be sent or an err_t value if the packet couldn't be sent
 */
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


/** \brief  Allocates a pbuf and returns the data from the incoming packet.
 *
 *  \param[in] netif the lwip network interface structure
 *  \param[in] idx   index of packet to be read
 *  \return a pbuf filled with the received packet (including MAC header)
 */
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


/** \brief  Attempt to read a packet from the EMAC interface.
 *
 *  \param[in] netif the lwip network interface structure
 *  \param[in] idx   index of packet to be read
 */
void rda59xx_netif_input(struct netif *netif, u8_t *data, u32_t len)
{
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  LWIP_DEBUGF(NETIF_DEBUG, ("rda59xx_netif_input\r\n"));
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
      LWIP_DEBUGF(NETIF_DEBUG, ("rda5991h_enetif_input: IP input ok\n"));
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
        netifapi_netif_set_link_down(&lwip_sta_netif);
    else
        netifapi_netif_set_link_down(&lwip_ap_netif);
}

void rda59xx_netif_link_up(u32_t netif)
{
    if(netif == 0)
        netifapi_netif_set_link_up(&lwip_sta_netif);
    else
        netifapi_netif_set_link_up(&lwip_ap_netif);
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
    err_t err;
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    netif->name[0] = 's';
    netif->name[1] = 't';
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    if(netif->hostname == NULL)
#ifdef DELETE_HFILOP_CODE
        netif->hostname = "rda59xx_sta";
#else
    {
        extern char *hfilop_layer_get_hostname(void);
	 netif->hostname = hfilop_layer_get_hostname();
    }
#endif
#endif /* LWIP_NETIF_HOSTNAME */
    rda59xx_get_macaddr((u8_t *)(netif->hwaddr), 0);

    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    // TODOETH: check if the flags are correct below
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    err = rda59xx_low_level_init(netif);
    if (err != ERR_OK)
        return err;
    netif->output = rda59xx_etharp_output;
    netif->linkoutput = rda59xx_low_level_output;

    netif_set_link_callback(&lwip_sta_netif, rda59xx_sta_netif_link_irq);
    netif_set_status_callback(&lwip_sta_netif, rda59xx_sta_netif_status_irq);
    netifapi_netif_set_default(&lwip_sta_netif);
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
    err_t err;
    LWIP_ASSERT("netif != NULL", (netif != NULL));

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
    err = rda59xx_low_level_init(netif);
    if (err != ERR_OK)
        return err;
    netif->output = rda59xx_etharp_output;
    netif->linkoutput = rda59xx_low_level_output;

    netif_set_link_callback(&lwip_ap_netif, rda59xx_ap_netif_link_irq);
    if(netif_default == NULL)
        netifapi_netif_set_default(&lwip_ap_netif);

    return ERR_OK;
}

void rda59xx_netif_down(int netif)
{
    if(netif == 0)
        netifapi_netif_set_down(&lwip_sta_netif);
    else
        netifapi_netif_set_down(&lwip_ap_netif);
}
void rda59xx_netif_up(int netif)
{
    if(netif == 0)
        netifapi_netif_set_up(&lwip_sta_netif);
    else
        netifapi_netif_set_up(&lwip_ap_netif);
}

