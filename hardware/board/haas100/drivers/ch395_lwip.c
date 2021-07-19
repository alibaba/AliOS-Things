/*
 * Copyright (C) 2020-2022 Alibaba Group Holding Limited
 */

#include <string.h>
#include "k_api.h"
#include "aos/kernel.h"

#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "netif/etharp.h"

#include "uservice/uservice.h"
#include "uservice/eventid.h"
#include "ulog/ulog.h"

#include "ch395_spi.h"
#include "ch395_cmd.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} tcpip_ip_info_t;

/* Private define ------------------------------------------------------------*/
/* Stack size of the interface thread */
#define CH395_TASK_PRIO 20
#define CH395_TASK_SIZE (6 * 1024)
#define CH395_MAX_DATA_SIZE 1514   /*max size is 1514 */
#define CH395_MIN_DATA_SZIE 60     /*min size is 64 include 4 bytes crc*/

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define TAG "ch395_lwip"

static struct netif eth_lwip_netif;
static tcpip_ip_info_t eth_ip_info = {0};
static aos_task_t gst_ch395_lwip_int_task = {0};
static aos_task_t gst_ch395_input_task = {0};
static aos_sem_t gst_ch395_recv_sem = {0};
static st_ch395_info_t gst_lwipch395info = {0};

static void ch395_lwip_inter_proc(void);
static int ch395_eth_sock_macraw(void);

static void dump_ch395_packet(char *buf, int len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        printf("0x%02x ", buf[i]);
        if ((i + 1) % 32 == 0) {
            printf("\r\n");
        }
    }
    printf("\r\n");
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    uint8_t *data = NULL;
    uint32_t datalen = 0;
    int32_t ret = 0;
    uint32_t first_copy = 0;
    uint32_t copylen = 0;
    void *src_buf = NULL;

    data = aos_malloc(CH395_MAX_DATA_SIZE);
    memset(data, 0, CH395_MAX_DATA_SIZE);

    for (q = p; q != NULL; q = q->next) {
        src_buf = q->payload;
        copylen = q->len;
        if (first_copy == 0) {
            src_buf = (char *)src_buf + ETH_PAD_SIZE;
            copylen = copylen - ETH_PAD_SIZE;
            first_copy = 1;
        }

        if (datalen + copylen >= CH395_MAX_DATA_SIZE) {
            aos_free(data);
            return ERR_BUF;
        }
        memcpy(&data[datalen], src_buf, copylen);
        datalen = datalen + copylen;
    }

    if (datalen < CH395_MIN_DATA_SZIE) {
        datalen = CH395_MIN_DATA_SZIE;
    }
    /*for 4-byte alignment， 14 bytes ethernet header will cost 16 bytes;
      so just we jump over the first two bytes */
    ret = ch395_socket_data_send(0, datalen, data);
    aos_free(data);
    if (ret) {
        printf("ch395 lwip low level output len %d fail", datalen);
        return ERR_IF;
    }

    return ERR_OK;
}

/**
  * @brief Should allocate a pbuf and transfer the bytes of the incoming
  * packet from the interface into the pbuf.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return a pbuf filled with the received packet (including MAC header)
  *         NULL on memory error
  */
static struct pbuf *low_level_input(struct netif *netif, uint8_t *data, uint32_t datalen)
{
    struct pbuf *p = NULL, *q = NULL;
    uint8_t *buffer = data;
    uint32_t bufferoffset = 0;
    uint32_t copylen = 0;
    uint32_t leftlen = datalen;
    uint32_t first_copy = 0;
    uint32_t pbuf_len = 0;
    void *dest_buf = NULL;

    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, datalen + ETH_PAD_SIZE, PBUF_POOL);
    if (NULL == p) {
        return NULL;
    }

    for (q = p; q != NULL; q = q->next) {
        dest_buf = q->payload;
        pbuf_len = q->len;

        if (first_copy == 0) {
            dest_buf = (char *)dest_buf + ETH_PAD_SIZE;
            pbuf_len = pbuf_len - ETH_PAD_SIZE;
            first_copy = 1;
        }

        if (q->len > leftlen) {
            copylen = leftlen;
        } else {
            copylen = pbuf_len;
        }

        memcpy(dest_buf, &buffer[bufferoffset], copylen);
        bufferoffset = bufferoffset + copylen;
        leftlen = leftlen - copylen;
    }

    return p;
}

static void ethernetif_input(void const *argument)
{
    struct pbuf *p;
    int ret = 0;
    uint8_t sock = 0;
    uint16_t recv_len = 0;
    uint8_t *precv_data = NULL;
    struct netif *netif = (struct netif *)argument;

    /* then we need to recv the data */
    precv_data = aos_malloc(CH395_MAX_DATA_SIZE);
    if (NULL == precv_data) {
        LOGE(TAG, "Fail to malloc %d ", recv_len);
        return;
    }

    while (1) {
        ret = aos_sem_wait(&gst_ch395_recv_sem, AOS_WAIT_FOREVER);
        if (ret) {
            LOGE(TAG, "Fail to wait socket %d recv sem4 ", sock);
            continue;
        }

        ret = ch395_socket_recv_data_len(sock, &recv_len);
        if (ret) {
            LOGE(TAG, "Fail to get sock %d recv length", sock);
            continue;
        }

        if (recv_len == 0) {
            LOGE(TAG, "sock %d no data need to recv ", sock);
            continue;
        }

        memset(precv_data, 0, CH395_MAX_DATA_SIZE);

        ret = ch395_socket_recv_data(sock, recv_len, precv_data);
        if (ret) {
            LOGE(TAG, "sock %d recv data fail len %d", sock, recv_len);
            continue;
        }

        p = low_level_input(netif, precv_data, recv_len);
        if (p != NULL) {
            if (netif->input(p, netif) != ERR_OK) {
                pbuf_free(p);
            }
        }
    }

    /*shoudn't reach here incase memory leak */
    aos_free(precv_data);
}

static int low_level_init(struct netif *netif)
{
    unsigned char macaddress[6] = {0};
    int ret = 0;

    ret = ch395_get_mac_addr(macaddress);
    if (ret) {
        printf("eth fail to get mac addr ret %d \r\n", ret);
        return -1;
    }

    /* set netif MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set netif MAC hardware address */
    netif->hwaddr[0] = macaddress[0];
    netif->hwaddr[1] = macaddress[1];
    netif->hwaddr[2] = macaddress[2];
    netif->hwaddr[3] = macaddress[3];
    netif->hwaddr[4] = macaddress[4];
    netif->hwaddr[5] = macaddress[5];

    /* set netif maximum transfer unit */
    netif->mtu = 1500;

    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    ret = aos_sem_new(&gst_ch395_recv_sem, 0);
    if (ret) {
        printf("creat to new ch395 lwip send sem4 0x%x\r\n", ret);
        return -1;
    }

    ret = aos_task_new_ext(&gst_ch395_input_task, "ch395_input",
                           ethernetif_input, netif, CH395_TASK_SIZE, CH395_TASK_PRIO);
    if (ret) {
        printf("fail to start ch395 lwip input task 0x%x\r\n", ret);
        aos_sem_free(&gst_ch395_recv_sem);
        return -1;
    }

    ret = aos_task_new_ext(&gst_ch395_lwip_int_task, "ch395_irq",
                           ch395_lwip_inter_proc, NULL, CH395_TASK_SIZE, CH395_TASK_PRIO - 4);
    if (ret) {
        LOGE(TAG, "Fail to start chip interrupt proc task 0x%x", ret);
        aos_task_delete(&gst_ch395_input_task);
        aos_sem_free(&gst_ch395_recv_sem);
        return -1;
    }

    return 0;
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
static err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "haas_aos";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* set netif maximum transfer unit */
    netif->mtu = 1500;

    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    if (low_level_init(netif) != 0) {
        printf("ch395 lwip low level init fail\r\n");
        return ERR_IF;
    }

    return ERR_OK;
}

void post_ip_addr(tcpip_ip_info_t ip)
{
    /* post ip, mask and gateway in dhcp mode */
    printf("************************************************** \r\n");
    printf("DHCP Enable \r\n");
    printf("ip = %s \r\n", ip4addr_ntoa(&eth_ip_info.ip));
    printf("mask = %s \r\n", ip4addr_ntoa(&eth_ip_info.netmask));
    printf("gateway = %s \r\n", ip4addr_ntoa(&eth_ip_info.gw));
    printf("************************************************** \r\n");
}

static void tcpip_dhcpc_cb(struct netif *pstnetif)
{
    uint8_t ip_addr[4] = {0};
    uint8_t mask_addr[4] = {0};
    uint8_t gw_addr[4] = {0};
    int32_t ret = 0;

    if (!ip4_addr_cmp(ip_2_ip4(&pstnetif->ip_addr), IP4_ADDR_ANY4)) {
        // check whether IP is changed
        if (!ip4_addr_cmp(ip_2_ip4(&pstnetif->ip_addr), &eth_ip_info.ip) ||
            !ip4_addr_cmp(ip_2_ip4(&pstnetif->netmask), &eth_ip_info.netmask) ||
            !ip4_addr_cmp(ip_2_ip4(&pstnetif->gw), &eth_ip_info.gw)) {
            ip4_addr_set(&eth_ip_info.ip, ip_2_ip4(&pstnetif->ip_addr));
            ip4_addr_set(&eth_ip_info.netmask, ip_2_ip4(&pstnetif->netmask));
            ip4_addr_set(&eth_ip_info.gw, ip_2_ip4(&pstnetif->gw));

            /* post the dhcp ip address */
            post_ip_addr(eth_ip_info);
            /*set ip addr for ch395*/
            memcpy(ip_addr, &eth_ip_info.ip.addr, sizeof(ip_addr));
            memcpy(mask_addr, &eth_ip_info.netmask.addr, sizeof(ip_addr));
            memcpy(gw_addr, &eth_ip_info.gw.addr, sizeof(ip_addr));
            printf("\r\nip: %d:%d:%d:%d mask: %d:%d:%d:%d gw:%d:%d:%d:%d \r\n",
                ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3],
                mask_addr[0], mask_addr[1], mask_addr[2], mask_addr[3],
                gw_addr[0], gw_addr[1], gw_addr[2], gw_addr[3]);
            ret = ch395_set_ip_addr(ip_addr);
            if (ret) {
                printf("set ip addr fail \r\n");
            }
            ret = ch395_set_gw_ip_addr(gw_addr);
            if (ret) {
                printf("set gateway ip addr fail \r\n");
            }
            ch395_set_ip_mask_addr(mask_addr);
            if (ret) {
                printf("set ip mask addr fail \r\n");
            }
        }
        event_publish(EVENT_NETMGR_DHCP_SUCCESS, NULL);
    }

    return;
}

err_t tcpip_dhcpc_start(struct netif *pstnetif)
{
    int ret = 0;
    /*at first try to enable dhcp*/
    if (NULL == pstnetif) {
        printf("input netif is NULL \r\n");
        return -1;
    }

    if (netif_is_up(pstnetif)) {
        if (dhcp_start(pstnetif) != ERR_OK) {
            LOG("dhcp client start failed");
            return -1;
        }
    }

    netif_set_status_callback(pstnetif, tcpip_dhcpc_cb);
    return 0;
}

static void tcpip_init_done(void *arg)
{
#if LWIP_IPV4
    ip4_addr_t ipaddr, netmask, gw;
    memset(&ipaddr, 0, sizeof(ipaddr));
    memset(&netmask, 0, sizeof(netmask));
    memset(&gw, 0, sizeof(gw));
    netif_add(&eth_lwip_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
#endif

    netif_set_default(&eth_lwip_netif);
    netif_set_up(&eth_lwip_netif);
}

/* should be called after dhcp is done */
static int ch395_eth_sock_macraw(void)
{
    int ret = 0;

    ret = ch395_set_sock_proto_type(0, PROTO_TYPE_MAC_RAW);
    if (ret) {
        LOGE(TAG, "Fail to set sock 0 macraw mode fail");
        return -1;
    }

    ret = ch395_socket_open(0);
    if (ret) {
        LOGE(TAG, "ch395 lwip fail to open socket 0 macraw mode");
        return -1;
    }

    return 0;
}

static void ch395_lwip_sock_interrupt_proc(uint8_t sockindex)
{
    uint8_t sock_int_socket = 0;
    uint16_t recv_len = 0;
    uint8_t *precv_data = NULL;
    int32_t ret = 0;

    /* get sock interrupt status */
    ret = ch395_get_sock_int_status(sockindex, &sock_int_socket);
    /* send done proc */
    if (sock_int_socket & SINT_STAT_SENBUF_FREE) {
        // LOGI(TAG, "sock %d send data done ", sockindex);
        /*it means send ok */
    }

    if (sock_int_socket & SINT_STAT_SEND_OK) {
        /*only one buf is ok, so do nothing for now*/
    }

    if (sock_int_socket & SINT_STAT_CONNECT) {
        /*the interrup only happened in tcp mode , in socket 0
        macraw mode nothing todo */
    }

    if (sock_int_socket & SINT_STAT_DISCONNECT) {
        /*the interrup only happened in tcp mode , in socket 0
        macraw mode nothing todo */
    }

    if (sock_int_socket & SINT_STAT_TIM_OUT) {
        /*the interrup only happened in tcp mode , in socket 0
        macraw mode nothing todo */
    }

    if (sock_int_socket & SINT_STAT_RECV) {
        // LOGI(TAG, "sock %d recv data ", sockindex);
        /*get recv data length*/
        /*signal recv sem4*/
        if (aos_sem_is_valid(&gst_ch395_recv_sem)) {
            aos_sem_signal(&gst_ch395_recv_sem);
        }
    }
}

static void ch395_lwip_inter_proc(void)
{
    uint16_t ch395_int_status;
    uint8_t dhcp_status = 0;
    uint8_t phy_status = 0;
    uint32_t retry = 0;
    int32_t ret = 0;
    ip4_addr_t ipaddr = {0};
    ip4_addr_t netmask = {0};
    ip4_addr_t gw = {0};

    while (1) {
        /*every 50 ms proc the chip interrupt*/
        aos_msleep(5);

        ch395_int_status = 0;
        ret = ch395_get_global_all_int_status(&ch395_int_status);

        if (ch395_int_status & GINT_STAT_UNREACH) {
            /* nothing to do for now*/
            LOGI(TAG, "recv unreach interrupt, nothing to do for now");
        }

        if (ch395_int_status & GINT_STAT_IP_CONFLI) {
            LOGI(TAG, "recv ip confict interrupt, nothing to do for now");
        }

        if (ch395_int_status & GINT_STAT_PHY_CHANGE) {
            /*get phy status*/
            ret = ch395_get_phy_status(&phy_status);
            if (ret != 0) {
                LOGE(TAG, "Fail to get phy status");
                continue;
            }
            gst_lwipch395info.phystate = phy_status;
            if (phy_status == PHY_DISCONN) {
                LOGI(TAG, "eth link down");
                netif_set_link_down(&eth_lwip_netif);
                /* remove IP address from interface  */
                netif_set_addr(&eth_lwip_netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
            } else {
                /*start up to dhcp*/
                LOGI(TAG, "eth link up");
                netif_set_link_up(&eth_lwip_netif);
                tcpip_dhcpc_start(&eth_lwip_netif);
            }
        }

        /* dhcp/pppoe interrup proc */
        if (ch395_int_status & GINT_STAT_DHCP) {
            ret = ch395_dhcp_get_status(&dhcp_status);
            if (ret) {
                LOGE(TAG, "Fail to dhcp result");
                continue;
            }

            if (dhcp_status == 0) {
                /*try to get ip interface */
                do {
                    memset(&gst_lwipch395info.ip_info, 0, sizeof(gst_lwipch395info.ip_info));
                    ret = ch395_get_ip_interface(&gst_lwipch395info.ip_info);
                    if (ret) {
                        LOGE(TAG, "Fail to get eth interface ip info");
                        continue;
                    }

                    if (gst_lwipch395info.ip_info.ipaddr[0] != 0 && gst_lwipch395info.ip_info.ipaddr[1] != 0) {
                        /* Post got ip event */
                        LOGI(TAG, "get ip info %d.%d.%d.%d gateway %d.%d.%d.%d mask %d.%d.%d.%d dns1 %d.%d.%d.%d dns2 %d.%d.%d.%d\r\n",
                             gst_lwipch395info.ip_info.ipaddr[0], gst_lwipch395info.ip_info.ipaddr[1], gst_lwipch395info.ip_info.ipaddr[2], gst_lwipch395info.ip_info.ipaddr[3],
                             gst_lwipch395info.ip_info.gateway[0], gst_lwipch395info.ip_info.gateway[1], gst_lwipch395info.ip_info.gateway[2], gst_lwipch395info.ip_info.gateway[3],
                             gst_lwipch395info.ip_info.ip_mask[0], gst_lwipch395info.ip_info.ip_mask[1], gst_lwipch395info.ip_info.ip_mask[2], gst_lwipch395info.ip_info.ip_mask[3],
                             gst_lwipch395info.ip_info.ip_dns1[0], gst_lwipch395info.ip_info.ip_dns1[1], gst_lwipch395info.ip_info.ip_dns1[2], gst_lwipch395info.ip_info.ip_dns1[3],
                             gst_lwipch395info.ip_info.ip_dns2[0], gst_lwipch395info.ip_info.ip_dns2[1], gst_lwipch395info.ip_info.ip_dns2[2], gst_lwipch395info.ip_info.ip_dns2[3]);
                        memcpy(&ipaddr.addr, gst_lwipch395info.ip_info.ipaddr, 4);
                        memcpy(&netmask.addr, gst_lwipch395info.ip_info.ip_mask, 4);
                        memcpy(&gw.addr, gst_lwipch395info.ip_info.gateway, 4);

                        netif_set_addr(&eth_lwip_netif, &ipaddr, &netmask, &gw);
                        tcpip_dhcpc_cb(&eth_lwip_netif);
                        break;
                    }
                    aos_msleep(1000);
                    retry++;
                } while (retry < 10);
            } else {
                // LOGD(TAG, "dhcp time out, cannot get ip addr, it will go on dhcp after 16 second");
            }
        }
        if (ch395_int_status & GINT_STAT_SOCK0) {
            ch395_lwip_sock_interrupt_proc(0);
        }
    }
}

int eth_lwip_tcpip_init(void)
{
    int ret = 0;
    unsigned char chip_ver = 0;
    unsigned char soft_ver = 0;
    spi_dev_t eth_spi_dev = {0};

    eth_spi_dev.port = 0;
    eth_spi_dev.config.data_size = SPI_DATA_SIZE_8BIT;
    eth_spi_dev.config.mode = SPI_WORK_MODE_3;
    eth_spi_dev.config.cs = SPI_CS_DIS;
    eth_spi_dev.config.freq = 2000000;
    eth_spi_dev.config.role = SPI_ROLE_MASTER;
    eth_spi_dev.config.firstbit = SPI_FIRSTBIT_MSB;
    eth_spi_dev.config.t_mode = SPI_TRANSFER_NORMAL;

    ret = ch395_module_init(&eth_spi_dev);
    if (ret) {
        printf("spi init fail 0x%x, port %d, spi role %d, firstbit %d, work_mode %d, freq %d",
               ret, eth_spi_dev.port, eth_spi_dev.config.role, eth_spi_dev.config.firstbit,
               eth_spi_dev.config.mode, eth_spi_dev.config.freq);
        return -1;
    }

    ret = ch395_get_version(&chip_ver, &soft_ver);
    if (ret || chip_ver != 0x4) {
        printf("Fail to get chip ver: 0x%x soft ver 0x%x ret : 0x%x", chip_ver, soft_ver, ret);
        return -1;
    }

    ret = ch395_set_func_param(SOCK_CTRL_FLAG_SOCKET_CLOSE);
    if (ret) {
        printf("eht init fail : ch395 set func param fail %d", ret);
        return -1;
    }

    ret = ch395_dev_init();
    if (ret) {
        printf("eht init fail : ch395 init fail %d", ret);
        return -1;
    }

    ret = ch395_eth_sock_macraw();
    if (ret) {
        printf("eth fail to set socket 0 macraw mode \r\n", ret);
        return -1;
    }

    ret = ch395_ping_enable(1);
    if (ret) {
        printf("eht init fail : ch395 ping enable fail %d", ret);
    }

    tcpip_init_done(NULL);

    return 0;
}
