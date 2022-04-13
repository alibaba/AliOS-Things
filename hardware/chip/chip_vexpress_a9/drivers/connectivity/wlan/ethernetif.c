#include <stdio.h>
#include <stdlib.h>
#include "aos/kernel.h"
#if 0
#include "aos/yloop.h"
#endif
#include <k_api.h>
#include "network/network.h"
#include <lwip/tcpip.h>
#include "netif/etharp.h"
#include "ethernetif.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* The time to block waiting for input. */
#define TIME_WAITING_FOR_INPUT                 ( AOS_WAIT_FOREVER )
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE            ( 1024 )
#define INTERFACE_THREAD__PRIO                 ( 5 )


/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 '0'



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Semaphore to signal incoming packets */
aos_sem_t s_xSemaphore;

struct netif lwip_netif;

ip_info_t eth_ip_info = {0};

/* Private function prototypes -----------------------------------------------*/
static void ethernetif_input( void const * argument );

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Ethernet Rx Transfer completed callback
  * @param  heth: ETH handle
  * @retval None
  */
void HAL_ETH_RxCpltCallback(void *dev)
{
  aos_sem_signal(&s_xSemaphore);
}

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH) 
*******************************************************************************/
/**
  * @brief In this function, the hardware should be initialized.
  * Called from ethernetif_init().
  *
  * @param netif the already initialized lwip network interface structure
  *        for this ethernetif
  */
static void low_level_init(struct netif *netif)
{
  uint8_t macaddress[6]= { 0x00, 0x0c, 0x29, 0xcd, 0x94, 0x38 };
  if (smc911x_emac_init(macaddress) == 0)
  {
    /* Set netif link flag */
    netif->flags |= NETIF_FLAG_LINK_UP;
  }
  
  
  /* set netif MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set netif MAC hardware address */
  netif->hwaddr[0] =  macaddress[0];
  netif->hwaddr[1] =  macaddress[1];
  netif->hwaddr[2] =  macaddress[2];
  netif->hwaddr[3] =  macaddress[3];
  netif->hwaddr[4] =  macaddress[4];
  netif->hwaddr[5] =  macaddress[5];

  /* set netif maximum transfer unit */
  netif->mtu = 1500;

  /* Accept broadcast address and ARP traffic */
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  aos_sem_new(&s_xSemaphore, 0);

  /* create the task that handles the ETH_MAC */
  sys_thread_new("netif", ethernetif_input, &lwip_netif, INTERFACE_THREAD_STACK_SIZE, INTERFACE_THREAD__PRIO);

}


/**
  * @brief This function should do the actual transmission of the packet. The packet is
  * contained in the pbuf that is passed to the function. This pbuf
  * might be chained.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
  * @return ERR_OK if the packet could be sent
  *         an err_t value if the packet couldn't be sent
  *
  * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
  *       strange results. You might consider waiting for space in the DMA queue
  *       to become available since the stack doesn't retry to send a packet
  *       dropped because of memory failure (except for the TCP timers).
  */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    return smc911x_emac_tx(p);
}

/**
  * @brief Should allocate a pbuf and transfer the bytes of the incoming
  * packet from the interface into the pbuf.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return a pbuf filled with the received packet (including MAC header)
  *         NULL on memory error
  */
struct pbuf * low_level_input(struct netif *netif)
{
    extern struct pbuf *smc911x_emac_rx(void);
    struct pbuf *p = smc911x_emac_rx();
    return p;
}

/**
  * @brief This function is the ethernetif_input task, it is processed when a packet 
  * is ready to be read from the interface. It uses the function low_level_input() 
  * that should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
void ethernetif_input( void const * argument )
{
  struct pbuf *p;
  struct netif *netif = (struct netif *) argument;
  
  for( ;; )
  {
    if (aos_sem_wait( &s_xSemaphore, TIME_WAITING_FOR_INPUT) == 0)
    {
      do
      {
        p = low_level_input( netif );
        if (p != NULL)
        {
          if (netif->input( p, netif) != ERR_OK )
          {
            pbuf_free(p);
          }
        }
      }while(p!=NULL);
    }
  }
}

/**
  * @brief Should be called at the beginning of the program to set up the
  * network interface. It calls the function low_level_init() to do the
  * actual setup of the hardware.
  *
  * This function should be passed as a parameter to netif_add().
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return ERR_OK if the loopif is initialized
  *         ERR_MEM if private data couldn't be allocated
  *         any other err_t on error
  */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;

  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

void post_ip_addr(ip_info_t ip)
{
    /* post ip, mask and gateway in dhcp mode */
    printf("************************************************** \r\n");
    printf("DHCP Enable \r\n");
    printf("ip = %s \r\n", ip4addr_ntoa(&eth_ip_info.ip));
    printf("mask = %s \r\n", ip4addr_ntoa(&eth_ip_info.netmask));
    printf("gateway = %s \r\n", ip4addr_ntoa(&eth_ip_info.gw));
    printf("************************************************** \r\n");
#ifndef FEATURE_NETWORK_USPACE_VEXPRESS_TMP
    telnetserver_start();
#endif
}


static void tcpip_dhcpc_cb(struct netif *pstnetif)
{
    long long ts = aos_now();
    srand((unsigned int)ts);
    tcp_init();
    udp_init();

    if (!ip4_addr_cmp(ip_2_ip4(&pstnetif->ip_addr), IP4_ADDR_ANY4)) {
        //check whether IP is changed
        if (!ip4_addr_cmp(ip_2_ip4(&pstnetif->ip_addr), &eth_ip_info.ip) ||
                !ip4_addr_cmp(ip_2_ip4(&pstnetif->netmask), &eth_ip_info.netmask) ||
                !ip4_addr_cmp(ip_2_ip4(&pstnetif->gw), &eth_ip_info.gw)) {
            ip4_addr_set(&eth_ip_info.ip, ip_2_ip4(&pstnetif->ip_addr));
            ip4_addr_set(&eth_ip_info.netmask, ip_2_ip4(&pstnetif->netmask));
            ip4_addr_set(&eth_ip_info.gw, ip_2_ip4(&pstnetif->gw));

            /* post the dhcp ip address */
            post_ip_addr(eth_ip_info);

#ifdef AOS_LOOP
#ifndef FEATURE_NETWORK_USPACE_VEXPRESS_TMP
            aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
#endif
#endif
        }
    }
    return;
}


err_t tcpip_dhcpc_start(struct netif *pstnetif)
{
    if (NULL == pstnetif){
        printf("input netif is NULL\n");
        return -1;
    }

    if (netif_is_up(pstnetif)) {
        if (dhcp_start(pstnetif) != ERR_OK) {
            printf("dhcp client start failed\n");
            return -1;
        }
    }
    netif_set_status_callback(pstnetif, tcpip_dhcpc_cb);
    return 0;
}

static void tcpip_init_done(void *arg)
{
    ip4_addr_t ipaddr, netmask, gw;
    memset(&ipaddr, 0, sizeof(ipaddr));
    memset(&netmask, 0, sizeof(netmask));
    memset(&gw, 0, sizeof(gw));

    netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);

    netif_set_default(&lwip_netif);
    netif_set_up(&lwip_netif);
    tcpip_dhcpc_start(&lwip_netif);
}

int lwip_tcpip_init(void)
{
    tcpip_init(tcpip_init_done, NULL);

    return 0;
}
