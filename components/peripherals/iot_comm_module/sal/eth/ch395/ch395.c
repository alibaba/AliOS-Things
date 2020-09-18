/*
 * Copyright (C) 2020-2022 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "aos/kernel.h"

#include "network/network.h"
#include "hal_sal.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"

#include "ch395_spi.h"
#include "ch395_cmd.h"

#define TAG "ch395_sal_module"

#define SPI_TIME_OUT          1000
#define SOCK_DEFAULT_TIMEOUT  3000
#define CH395_TASK_PRIO       20
#define CH395_TASK_SIZE       (6 * 1024)

#define MAX_SURPPORT_SOCK_NUM 4         /*default is 4, max is 8*/
#define TCP_MSS_MAX_VALUE     1460
#define TCP_MSS_MIN_VALUE     60
#define TCP_TTL_MAX_VALUE     128
#define MAX_BUF_NUM           48
#define CH395_BLK_SIZE        512

typedef struct {
    uint8_t ipaddr[4];
    uint8_t gateway[4];
    uint8_t ip_mask[4];
    uint8_t ip_dns1[4];
    uint8_t ip_dns2[4];
}ch395_int_t;

typedef struct {
    ch395_int_t ip_info;
    uint8_t     mac_addr[6];
    uint8_t     phystate;
    uint8_t     mac_filt;
    uint16_t    retran_count;
    uint16_t    retran_period;
    uint8_t     int_mode;
    uint16_t    unreach_port;
    uint8_t     unreach_ip[4];
}st_ch395_info_t;

typedef enum {
    SOCK_STATE_OPEN = 0,
    SOCK_STATE_CONNECT,
    SOCK_STATE_DISCONN,
    SOCK_STATE_CLOSE,
}en_sock_state;

typedef struct {
    int32_t fd;
    aos_sem_t st_send_sem;
    aos_sem_t st_conn_sem;
    aos_sem_t st_disconn_sem;
    uint32_t tcp_conn_result;
    uint32_t send_result;
    uint8_t  destip[4];
    uint8_t  proto_type;
    uint8_t  sock_state;
    uint8_t  send_blk_num;
    uint8_t  recv_blk_num;
    uint16_t dest_port;
    uint16_t local_port;
}st_ch395_sock_t;

static int8_t g_i8_dev_add_flag = 0;
static int8_t g_i8_dev_init_flag = 0;
static spi_dev_t g_st_spi_info = {0};
static aos_task_t g_st_ch395_int_task = {0};
static aos_mutex_t gst_spi_op_mutex = {0};
static aos_mutex_t g_st_sock_mutex = {0};
static st_ch395_info_t  g_st_ch395info = {0};
static st_ch395_sock_t g_st_ch395_sock[MAX_SURPPORT_SOCK_NUM] = {0};

static netconn_data_input_cb_t g_netconn_data_input_cb;

static int32_t ch395_str_to_ip4addr(int8_t *str, uint8_t *paddr)
{
    uint32_t i = 0;
    uint32_t len = 0;
    uint8_t  ipindex = 0;
    uint8_t  strindex = 0;
    uint8_t  strdata[4] = {0};
    uint32_t ipaddr = 0;

    len = strlen(str);

    if (NULL == str || NULL == paddr) {
        LOGE(TAG, "%s %d Invalid input", __func__, __LINE__);
        return -1;
    }

    ipaddr = ipaddr_addr(str);
    if (ipaddr == -1) {
        LOGE(TAG, "%s %d Invalid ip str %s", __func__, __LINE__, str);
        return -1;
    }
    ipaddr = ntohl(ipaddr);
    paddr[0] = (uint8_t)((ipaddr & 0xFF000000) >> 24);
    paddr[1] = (uint8_t)((ipaddr & 0x00FF0000) >> 16);
    paddr[2] = (uint8_t)((ipaddr & 0x0000FF00) >> 8);
    paddr[3] = (uint8_t)(ipaddr & 0x000000FF);

    return 0;
}

static int32_t fd_to_linkid(int fd)
{
    int link_id;

    if (aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < MAX_SURPPORT_SOCK_NUM; link_id++) {
        if (g_st_ch395_sock[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_st_sock_mutex);

    return link_id;
}

static int32_t ch395_spi_data_write(uint8_t *pdata, uint32_t len)
{
    int32_t ret = 0;

    if (NULL == pdata || 0 == len) {
        LOGE(TAG, "invalid input");
        return -1;
    }

    if (g_i8_dev_add_flag == 0) {
        LOGE(TAG, "ch395 spi bus haven't init yet");
        return -1;
    }

    aos_mutex_lock(&gst_spi_op_mutex, AOS_WAIT_FOREVER);
    ret = hal_spi_send_ch395(&g_st_spi_info, pdata, len, SPI_TIME_OUT);
    aos_mutex_unlock(&gst_spi_op_mutex);
    if (ret) {
        LOGE(TAG, "spi cmd write fail %d", ret);
        return -1;
    }

    return 0;
}

static int32_t ch395_spi_send_and_recv(uint8_t *txdata, uint32_t txlen, uint8_t *rxdata, uint32_t rxlen)
{
    int32_t ret = 0;
    uint32_t i = 0;

    if (NULL == txdata || 0 == txlen || NULL == rxdata || 0 == rxlen) {
        LOGE(TAG, "invalid input");
        return -1;
    }

    if (g_i8_dev_add_flag == 0) {
        LOGE(TAG, "ch395 spi bus haven't init yet");
        return -1;
    }

    aos_mutex_lock(&gst_spi_op_mutex, AOS_WAIT_FOREVER);
    ret = hal_spi_send_and_recv_ch395_normal(&g_st_spi_info, txdata, txlen, rxdata, rxlen, SPI_TIME_OUT);
    aos_mutex_unlock(&gst_spi_op_mutex);
    if (ret) {
        LOGE(TAG, "===hal spi send and recv 0x%x len %d fail ret 0x%x==\r\n", txdata[0], txlen, ret);
        return -1;
    }

    return 0;
}

static int32_t ch395_spi_cmd_query(uint8_t cmd, uint8_t *pvalue)
{
    int32_t ret = 0;
    uint8_t data = 0;

    if (NULL == pvalue) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    ret = ch395_spi_send_and_recv(&cmd, sizeof(cmd), &data, sizeof(data));
    if (ret) {
        LOGE(TAG, "spi send and recv fail %d, cmd %d", ret, cmd);
        return -1;
    }

    *pvalue = data;
    return 0;
}

static int32_t ch395_get_cmd_status(uint8_t* pstatus)
{
    int32_t ret = 0;
    uint8_t cmd = CMD01_GET_CMD_STATUS;
    uint8_t value = 0;

    if (NULL == pstatus) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    ret = ch395_spi_cmd_query(cmd, &value);
    if (ret) {
        LOGE(TAG, "get cmd status fail %d ret");
        return -1;
    }

    *pstatus = value;

    return 0;
}

int32_t ch395_chip_exist_check(void)
{
    uint8_t cmd[2] = {CMD11_CHECK_EXIST, 0x5A};
    uint8_t readdata = 0;
    int32_t ret = 0;

    if (g_i8_dev_add_flag == 0) {
        LOGE(TAG, "ch395 spi bus haven't init yet");
        return -1;
    }

    aos_mutex_lock(&gst_spi_op_mutex, AOS_WAIT_FOREVER);
    ret = hal_spi_send_and_recv_ch395_exist(&g_st_spi_info, cmd, sizeof(cmd), &readdata, sizeof(readdata), SPI_TIME_OUT);
    aos_mutex_unlock(&gst_spi_op_mutex);
    if (ret) {
        LOGE(TAG, "===hal spi send and recv 0x%x len %d fail ret 0x%x==\r\n", cmd[0], sizeof(cmd), ret);
        return -1;
    }

    //LOGI(TAG, "Get readdata 0x%x ====", readdata);

    if (cmd[1] != ~readdata) {
        LOGE(TAG, "testdata 0x%x, readdata 0x%x ,chip not exist", cmd[1], readdata);
        return -1;
    }

    return 0;
}

static int32_t ch395_chip_reset(void)
{
    int32_t ret = 0;
    uint8_t cmd = CMD00_RESET_ALL;

    ret = ch395_spi_data_write(&cmd, sizeof(cmd));
    if (ret) {
        LOGE(TAG, "send chip reset cmd fail ");
        return -1;
    }
    /*wait 50ms ,make sure reset operation can finish*/
    aos_msleep(50);

    return 0;
}

static int32_t ch395_enter_sleep(void)
{
    int32_t ret = 0;
    uint8_t cmd = CMD00_ENTER_SLEEP;

    ret = ch395_spi_data_write(&cmd, sizeof(cmd));
    if (ret) {
        LOGE(TAG, "send chip reset cmd fail ");
        return -1;
    }

    /*when exite sleep mode should cost several ms (less than 10ms)*/
    return 0;
}

int32_t ch395_get_version(uint8_t *pchip_ver, uint8_t *pfirm_ver)
{
    int32_t ret = 0;
    uint8_t ver = 0;

    if (NULL == pchip_ver || NULL == pfirm_ver) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    ret = ch395_spi_cmd_query(CMD01_GET_IC_VER, &ver);
    if (ret) {
        LOGE(TAG, "Fail to get version cmd 0x%x ret %d", CMD01_GET_IC_VER, ret);
        return -1;
    }

    *pfirm_ver = ver & 0x0F;
    *pchip_ver = (ver & 0xF0) >> 4;

    return 0;
}

static int32_t ch395_set_phy_state(uint8_t state)
{
    int32_t ret = 0;
    uint8_t data[2] = {0};

    if (state != PHY_DISCONN && state != PHY_10M_FLL && state != PHY_10M_HALF
        && state != PHY_100M_FLL && state != PHY_100M_HALF && state != PHY_AUTO) {
        LOGE(TAG, "Invalid input 0x%x ", state);
        return -1;
    }

    data[0] = CMD10_SET_PHY;
    data[1] = state;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "send phy state fail cmd 0x%x state 0x%x", data[0], data[1]);
        return -1;
    }

    return 0;
}

int32_t ch395_get_phy_status(uint8_t *pstate)
{
    int32_t ret = 0;

    if (NULL == pstate) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    ret = ch395_spi_cmd_query(CMD01_GET_PHY_STATUS, pstate);
    if (ret) {
        LOGE(TAG, "Fail to get phy status %d", ret);
        return -1;
    }

    return 0;
}

int32_t ch395_set_mac_addr(uint8_t mac[6])
{
    int32_t ret = 0;
    uint8_t data[7] = {0};

    data[0] = CMD60_SET_MAC_ADDR;
    memcpy(&data[1], mac, 6);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write cmd 0x%x and mac fail ,ret %d", CMD60_SET_MAC_ADDR, ret);
        return -1;
    }

    /*it will save mac info at eerpom , it will take 100 ms*/
    aos_msleep(100);

    return 0;
}

int32_t ch395_get_mac_addr(uint8_t mac[6])
{
    int32_t ret = 0;
    uint8_t cmd = CMD06_GET_MAC_ADDR;

    ret = ch395_spi_send_and_recv(&cmd, sizeof(cmd), mac, 6);
    if (ret) {
        LOGE(TAG, "spi send and recv fail %d, recv data len %d", ret, 6);
        return -1;
    }

    return 0;
}

static int32_t ch395_set_mac_filt(uint8_t filt_type, uint32_t hash0, uint32_t hash1)
{
    int32_t ret = 0;
    uint8_t data[10] = {0};

    data[0] = CMD90_SET_MAC_FILT;
    data[1] = filt_type;
    data[2] = (uint8_t)hash0;
    data[3] = (uint8_t)(hash0 >> 8);
    data[4] = (uint8_t)(hash0 >> 16);
    data[5] = (uint8_t)(hash0 >> 24);
    data[6] = (uint8_t)hash1;
    data[7] = (uint8_t)(hash1 >> 8);
    data[8] = (uint8_t)(hash1 >> 16);
    data[9] = (uint8_t)(hash1 >> 24);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "fail to send mac filt data");
        return -1;
    }

    return 0;
}

static int32_t ch395_set_ip_addr(uint8_t ip[4])
{
    int32_t ret = 0;
    uint8_t data[5] = {0};

    data[0] = CMD40_SET_IP_ADDR;
    memcpy(&data[1], ip, 4);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write ip addr ret %d", ret);
        return -1;
    }

    return 0;
}

static int32_t ch395_set_gw_ip_addr(uint8_t ip[4])
{
    int32_t ret = 0;
    uint8_t data[5] = {0};

    data[0] = CMD40_SET_GWIP_ADDR;
    memcpy(&data[1], ip, 4);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write gateway ip addr ret %d", ret);
        return -1;
    }

    return 0;
}

static int32_t ch395_set_ip_mask_addr(uint8_t ip[4])
{
    int32_t ret = 0;
    uint8_t data[5] = {0};

    data[0] = CMD40_SET_MASK_ADDR;
    memcpy(&data[1], ip, 4);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write mask ip addr ret %d", ret);
        return -1;
    }

    return 0;
}

static int32_t ch395_get_global_int_status(uint8_t *pstatus)
{
    int32_t ret = 0;

    if (NULL == pstatus) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    ret = ch395_spi_cmd_query(CMD01_GET_GLOB_INT_STATUS, pstatus);
    if (ret) {
        LOGE(TAG, "Fail to get interrup status");
        return -1;
    }

    return 0;
}

static int32_t ch395_get_global_all_int_status(uint16_t *pstatus)
{
    int32_t ret = 0;
    uint8_t recvdata[2] = {0};
    uint8_t cmd = CMD02_GET_GLOB_INT_STATUS_ALL;

    if (NULL == pstatus) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    ret = ch395_spi_send_and_recv(&cmd, sizeof(cmd), (uint8_t *)recvdata, sizeof(recvdata));
    if (ret) {
        LOGE(TAG, "spi send and recv fail %d", ret);
        return -1;
    }

    *pstatus = (uint16_t)(recvdata[1] << 8) + recvdata[0];

    return 0;
}

/*only works in tcp mode, biggest value is 20 */
static int32_t ch395_tcp_retran_count_set(uint8_t count)
{
    int32_t ret = 0;
    uint8_t data[2] = {0};

    data[1] = count;
    if (data[1] > 20) {
        LOGE(TAG, "Invalid input %d , set it to 20", data[1]);
        data[1] = 20;
    }

    data[0] = CMD10_SET_RETRAN_COUNT;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to set retran data");
        return -1;
    }

    return 0;

}

/*period time is ms */
static int32_t ch395_tcp_retry_period_set(uint16_t time)
{
    int32_t ret = 0;
    uint8_t data[3] = {0};

    if (time > 1000) {
        LOGE(TAG, "Invalid input %d ", time);
        return -1;
    }

    data[0] = CMD20_SET_RETRAN_PERIOD;
    data[1] = (uint8_t)time;
    data[2] = (uint8_t)(time >> 8);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to set retran data");
        return -1;
    }

    return 0;

}

static int32_t ch395_get_remote_ip_port(uint8_t sock, uint8_t ip[4], uint16_t *pport)
{
    int32_t ret = 0;
    uint8_t send_data[2] = {0};
    uint8_t recv_data[6] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM || NULL == pport) {
        LOGE(TAG, "Invalid input sock %d pport %p ", sock, pport);
        return -1;
    }

    send_data[0] = CMD06_GET_REMOT_IPP_SN;
    send_data[1] = sock;

    ret = ch395_spi_send_and_recv(send_data, sizeof(send_data), recv_data, sizeof(recv_data));
    if (ret) {
        LOGE(TAG, "Fail to get sock %d remote port and ip", sock);
        return -1;
    }

    memcpy(ip, recv_data, 4);

    *pport = recv_data[4] + (uint16_t)(recv_data[5] << 8);

    return 0;
}

static int32_t ch395_clear_sock_recv_buff(uint8_t sock)
{
    int32_t ret = 0;
    uint8_t data[2] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD10_CLEAR_RECV_BUF_SN;
    data[1] = sock;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    return 0;
}

static int32_t ch395_get_sock_tcp_status(uint8_t sock, uint8_t *psock_status, uint8_t *ptcp_status)
{
    int32_t ret = 0;
    uint8_t data[2] = {0};
    uint8_t recv_data[2] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM || NULL == psock_status || NULL == ptcp_status) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD12_GET_SOCKET_STATUS_SN;
    data[1] = sock;

    ret = ch395_spi_send_and_recv(data, sizeof(data), recv_data, sizeof(recv_data));
    if (ret) {
        LOGE(TAG, "Fail to get sock %d tcp status", sock);
        return -1;
    }

    *psock_status = recv_data[0];
    *ptcp_status = recv_data[1];

    return 0;
}

static int32_t ch395_get_sock_int_status(uint8_t sock, uint8_t *pinterrupt)
{
    int32_t ret = 0;
    uint8_t send_data[2] = {0};
    uint8_t data = 0;

    if(sock >= MAX_SURPPORT_SOCK_NUM || NULL == pinterrupt) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    send_data[0] = CMD11_GET_INT_STATUS_SN;
    send_data[1] = sock;

    ret = ch395_spi_send_and_recv(send_data, sizeof(send_data), &data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to get sock %d interupt status", sock);
        return -1;
    }

    *pinterrupt = data;

    return 0;
}

static int32_t ch395_set_sock_dest_ip(uint8_t sock, uint8_t *pdst_ip)
{
    int32_t ret = 0;
    uint8_t data[6] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdst_ip) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD50_SET_IP_ADDR_SN;
    data[1] = sock;
    memcpy(&data[2], pdst_ip, 4);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    memcpy(g_st_ch395_sock[sock].destip, pdst_ip, 4);
    aos_mutex_unlock(&g_st_sock_mutex);

    return 0;
}

static int32_t ch395_set_sock_dest_port(uint8_t sock, uint16_t dest_port)
{
    int32_t ret = 0;
    uint8_t data[4] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD30_SET_DES_PORT_SN;
    data[1] = sock;
    data[2] = (uint8_t)dest_port;
    data[3] = (uint8_t)(dest_port >> 8);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    g_st_ch395_sock[sock].dest_port = dest_port;
    aos_mutex_unlock(&g_st_sock_mutex);

    return 0;
}

static int32_t ch395_set_sock_src_port(uint8_t sock, uint16_t src_port)
{
    int32_t ret = 0;
    uint8_t data[4] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD30_SET_SOUR_PORT_SN;
    data[1] = sock;
    data[2] = (uint8_t)src_port;
    data[3] = (uint8_t)(src_port >> 8);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    g_st_ch395_sock[sock].local_port = src_port;
    aos_mutex_unlock(&g_st_sock_mutex);

    return 0;
}

static int32_t ch395_set_sock_proto_type(uint8_t sock, uint8_t proto_type)
{
    int32_t ret = 0;
    uint8_t data[3] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM || proto_type > PROTO_TYPE_TCP) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD20_SET_PROTO_TYPE_SN;
    data[1] = sock;
    data[2] = proto_type;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    g_st_ch395_sock[sock].proto_type = proto_type;
    aos_mutex_unlock(&g_st_sock_mutex);

    return 0;
}

static int32_t ch395_socket_open(uint8_t sock)
{
    int32_t ret = 0;
    int32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD1W_OPEN_SOCKET_SN;
    data[1] = sock;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    /*check cmd status GET_CMD_STATUS, CMD_ERR_SUCCESS means successed*/
    do {
        retry++;
        status = 0;
        /*need to wait data free interrupt*/
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 &&(status == CMD_ERR_SUCCESS)) {
            break;
        }
        aos_msleep(2);
    }while(retry < 20);

    if (retry <= 20) {
        return 0;
    }

    return -1;
}

static int32_t ch395_socket_close(uint8_t sock)
{
    int32_t ret = 0;
    int32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD1W_CLOSE_SOCKET_SN;
    data[1] = sock;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    /*need to check cmd status*/
    do {
        retry++;
        status = 0;
        /*need to wait data free interrupt*/
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 && (status == CMD_ERR_SUCCESS)) {
            break;
        }
        aos_msleep(2);
    }while(retry < 20);

    if (retry <= 20) {
        return 0;
    }

    return -1;
}

static int32_t ch395_socket_tcp_connect(uint8_t sock)
{
    int32_t ret = 0;
    int32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD1W_TCP_CONNECT_SN;
    data[1] = sock;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    /*need to check cmd status*/
    do {
        retry++;
        status = 0;
        /*need to wait data free interrupt*/
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 && (status == CMD_ERR_SUCCESS)) {
            break;
        }
        aos_msleep(2);
    }while(retry < 20);

    if (retry >= 20) {
        LOGE(TAG, "Fail to execute cmd tcp connect");
        return -1;
    }

    /* start to wait for the connect sem4 */
    ret = aos_sem_wait(&g_st_ch395_sock[sock].st_conn_sem, SOCK_DEFAULT_TIMEOUT);
    if (ret) {
        LOGE(TAG, "Wait sock %d tcp connect sem fail", sock);
        return -1;
    }

    if (g_st_ch395_sock[sock].tcp_conn_result != 0) {
        LOGE(TAG, "sock %d tcp connect fail ");
        return -1;
    }

    return 0;
}

static int32_t ch395_socket_tcp_disconnect(uint8_t sock)
{
    int32_t ret = 0;
    uint32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    data[0] = CMD1W_TCP_CONNECT_SN;
    data[1] = sock;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    /*need to check cmd status*/
    do {
        retry++;
        status = 0;
        /*need to wait data free interrupt*/
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 && (status == CMD_ERR_SUCCESS)) {
            break;
        }
        aos_msleep(2);
    }while(retry < 20);

    if (retry >= 20) {
        LOGE(TAG, "Fail to execute cmd tcp disconnect");
        return -1;
    }

    /* wait for the disconnect sem4 */
    ret = aos_sem_wait(&g_st_ch395_sock[sock].st_disconn_sem, SOCK_DEFAULT_TIMEOUT);
    if (ret) {
        LOGE(TAG, "Wait sock %d tcp disconnect sem fail, nothing to do", sock);
        return -1;
    }

    return 0;
}

static int32_t ch395_socket_data_send(uint8_t sock, uint16_t datalen, uint8_t *pdata)
{
    int32_t ret = 0;
    uint8_t *psend_data = NULL;
    uint32_t sendlen = 0;
    int32_t retry = 0;

    if(sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdata) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    sendlen = datalen + 4;
    psend_data = malloc(sendlen);
    if (NULL == psend_data) {
        LOGE(TAG, "%s %d fail to malloc %d data", __FILE__, __LINE__, sendlen);
        return -1;
    }
    memset(psend_data, 0, sendlen);

    psend_data[0] = CMD30_WRITE_SEND_BUF_SN;
    psend_data[1] = sock;
    psend_data[2] = (uint8_t) (datalen);
    psend_data[3] = (uint8_t) (datalen >> 8);
    memcpy(&psend_data[4], pdata, datalen);
    //LOGI(TAG, "%s %d sock %d send data %s len %d", __FILE__, __LINE__, sock, pdata, datalen);
    
    aos_mutex_lock(&gst_spi_op_mutex, AOS_WAIT_FOREVER);
    ret = hal_spi_send_ch395_sockdata(&g_st_spi_info, psend_data, sendlen, SPI_TIME_OUT);
    aos_mutex_unlock(&gst_spi_op_mutex);
    free(psend_data);
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sendlen);
        return -1;
    }

    /* wait for the disconnect sem4 */
    ret = aos_sem_wait(&g_st_ch395_sock[sock].st_send_sem, SOCK_DEFAULT_TIMEOUT);
    if (ret) {
        LOGE(TAG, "Wait sock %d data send sem fail", sock);
        return -1;
    }

    if (g_st_ch395_sock[sock].send_result != 0) {
        LOGE(TAG, "%s %d sock %d send data fail ", __FILE__, __LINE__, sock);
        return -1;
    }

    return 0;
}

static int32_t ch395_socket_recv_data_len(uint8_t sock, uint16_t *pdatalen)
{
    int32_t ret = 0;
    uint8_t send_data[2] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdatalen) {
        LOGE(TAG, "Invalid input %d", sock);
        return -1;
    }

    send_data[0] = CMD12_GET_RECV_LEN_SN;
    send_data[1] = sock;

    ret = ch395_spi_send_and_recv(send_data, sizeof(send_data), pdatalen, sizeof(uint16_t));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    return 0;
}

static int32_t ch395_socket_recv_data(uint8_t sock, uint16_t datalen, uint8_t *pdata)
{
    int32_t ret = 0;
    uint8_t send_data[4] = {0};

    if(sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdata || datalen == 0) {
        LOGE(TAG, "Invalid input %d datalen %d", sock, datalen);
        return -1;
    }

    send_data[0] = CMD30_READ_RECV_BUF_SN;
    send_data[1] = sock;
    send_data[2] = (uint8_t)datalen;
    send_data[3] = (uint8_t)(datalen >> 8);

    ret = ch395_spi_send_and_recv(send_data, sizeof(send_data), pdata, datalen);
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sock);
        return -1;
    }

    return 0;
}

/*
 * Input          : enable : 1  enable PING
                           : 0  disable PING
 */
int32_t ch395_ping_enable(uint8_t enable)
{
    int32_t ret = 0;
    uint8_t cmd_data[2] = {0};

    if (enable !=0 && enable != 1) {
        LOGE(TAG, "Invalid inpute %d", enable);
        return -1;
    }

    cmd_data[0] = CMD01_PING_ENABLE;
    cmd_data[1] = enable;

    ret = ch395_spi_data_write(cmd_data, sizeof(cmd_data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", enable);
        return -1;
    }

    return 0;
}

/*
 * Input          : enable : 1  enable dhcp
                           : 0  disable dhcp
 */
int32_t ch395_dhcp_enable(uint8_t enable)
{
    int32_t ret = 0;
    uint8_t status = 0;
    uint8_t i = 0;
    uint8_t cmd_data[2] = {0};
    /*should do it after chip is enabled*/

    if (enable !=0 && enable != 1) {
        LOGE(TAG, "Invalid inpute %d", enable);
        return -1;
    }

    cmd_data[0] = CMD10_DHCP_ENABLE;
    cmd_data[1] = enable;

    ret = ch395_spi_data_write(cmd_data, sizeof(cmd_data));
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", enable);
        return -1;
    }
    aos_msleep(20);
    /*check cmd status*/
    while(1) {
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 && (status == CMD_ERR_SUCCESS)) {
            break;
        }
        if(i++ > 200) {
            return -1;
        }
        /* we should wait more than 2 ms to get cmd status*/
    }
    LOGI(TAG, "dhcp enable %d execute successed ", enable);

    return 0;
}

int32_t ch395_dhcp_get_status(uint8_t *pstatus)
{
    int32_t ret = 0;

    if (NULL == pstatus) {
        LOGE(TAG, "Invalid inpute ");
        return -1;
    }

    ret = ch395_spi_cmd_query(CMD01_GET_DHCP_STATUS, pstatus);
    if (ret) {
        LOGE(TAG, "Fail to get dhcp status ret %d", ret);
        return -1;
    }

    return 0;
}

int32_t ch395_get_ip_interface(ch395_int_t *pst_inf)
{
    int32_t ret = 0;
    uint8_t cmd = CMD014_GET_IP_INF;

    if (NULL == pst_inf) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    ret = ch395_spi_send_and_recv(&cmd, sizeof(cmd), pst_inf, sizeof(ch395_int_t));
    if (ret) {
        LOGE(TAG, "Fail to read interface info data");
        return -1;
    }

    return 0;
}

/*should be set before ch395 init*/
static int32_t ch395_set_tcp_mss(uint16_t mss)
{
    int32_t ret = 0;
    uint8_t data[3] = {0};

    if (mss > TCP_MSS_MAX_VALUE || mss < TCP_MSS_MIN_VALUE) {
        LOGE(TAG, "Invalid Input %d", mss);
        return -1;
    }

    data[0] = CMD20_SET_TCP_MSS;
    data[1] = (uint8_t)mss;
    data[1] = (uint8_t)(mss >> 8);
    memcpy(&data[1], &mss, sizeof(mss));

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write mss value %d", mss);
        return -1;
    }

    return 0;
}

static int32_t ch395_set_func_param(uint32_t param)
{
    int32_t ret = 0;
    uint8_t data[5] = {0};

    data[0] = CMD40_SET_FUN_PARA;
    data[1] = (uint8_t)param;
    data[2] = (uint8_t)(param >> 8);
    data[2] = (uint8_t)(param >> 16);
    data[3] = (uint8_t)(param >> 24);

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to set func param value %d", param);
        return -1;
    }

    return 0;

}

static int32_t ch395_set_sock_ttl(uint8_t sock, uint8_t ttl)
{
    int32_t ret = 0;
    uint8_t data[3] = {0};

    /*should be called after sock is opened*/
    if (ttl > TCP_TTL_MAX_VALUE || sock >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "Invalid Input ttl %d, sock %d", ttl, sock);
        return -1;
    }

    data[0] = CMD20_SET_TTL;
    data[1] = sock;
    data[2] = ttl;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write sock %d ttl %d", sock, ttl);
        return -1;
    }

    return 0;
}

/* There is 48 blocks in ch395. Each Block size is 512 bytes.
 * The default config is :
 *     sock   0   recv buf     0      8
 *                send buf     8      4
 *     sock   1   recv buf     12     8
 *                send buf     20     4
 *     sock   2   recv buf     24     8
 *                send buf     32     4
 *     sock   3   recv buf     36     8
 *                send buf     44     4
 *     other      recv buf     NULL   0
 *                send buf     NULL   0
 */

static int32_t ch395_sock_set_recv_buf(uint8_t sock, uint8_t startblk, uint8_t blknum)
{
    int32_t ret = 0;
    uint8_t data[4] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || startblk >= MAX_BUF_NUM || blknum >= MAX_BUF_NUM) {
        LOGE(TAG, "Invalid input sock %d, startblk %d, blknum %d", sock, startblk, blknum);
        return -1;
    }

    data[0] = CMD30_SET_RECV_BUF;
    data[1] = sock;
    data[2] = startblk;
    data[3] = blknum;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write sock %d startblk %d blknum %d", sock, startblk, blknum);
        return -1;
    }
    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    g_st_ch395_sock[sock].recv_blk_num = blknum;
    aos_mutex_unlock(&g_st_sock_mutex);

    return 0;
}

static int32_t ch395_sock_set_send_buf(uint8_t sock, uint8_t startblk, uint8_t blknum)
{
    int32_t ret = 0;
    uint8_t data[4] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || startblk >= MAX_BUF_NUM || blknum >= MAX_BUF_NUM) {
        LOGE(TAG, "Invalid input sock %d, startblk %d, blknum %d", sock, startblk, blknum);
        return -1;
    }

    data[0] = CMD30_SET_SEND_BUF;
    data[1] = sock;
    data[2] = startblk;
    data[3] = blknum;

    ret = ch395_spi_data_write(data, sizeof(data));
    if (ret) {
        LOGE(TAG, "Fail to write sock %d startblk %d blknum %d", sock, startblk, blknum);
        return -1;
    }

    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    g_st_ch395_sock[sock].send_blk_num = blknum;
    aos_mutex_unlock(&g_st_sock_mutex);
    return 0;

}

void ch395_sock_interrupt_proc(uint8_t sockindex)
{
    uint8_t  sock_int_socket = 0;
    uint16_t recv_len = 0;
    uint8_t  *precv_data = NULL;
    uint8_t  remote_ip[16] = {0};
    int32_t  ret = 0;

    /* get sock interrupt status */
    ret = ch395_get_sock_int_status(sockindex, &sock_int_socket);
    //LOGI(TAG, "proc sock %d interrup 0x%x", sockindex, sock_int_socket);
    if(sock_int_socket & SINT_STAT_SENBUF_FREE)                      /* tcp send proc */
    {
        //LOGI(TAG, "sock %d send data done ", sockindex);
        /*it means send ok */
        if (aos_sem_is_valid(&g_st_ch395_sock[sockindex].st_send_sem)) {
            g_st_ch395_sock[sockindex].send_result = 0;
            aos_sem_signal(&g_st_ch395_sock[sockindex].st_send_sem);
        }
    }

    if(sock_int_socket & SINT_STAT_SEND_OK)                          /*  */
    {
        /*only one buf is ok, so do nothing for now*/
    }

   if(sock_int_socket & SINT_STAT_CONNECT)                            /**/
   {
        /*it means connect ok */
        //LOGI(TAG, "sock %d connect ok ", sockindex);
        if (g_st_ch395_sock[sockindex].proto_type == PROTO_TYPE_TCP
            && aos_sem_is_valid(&g_st_ch395_sock[sockindex].st_conn_sem))
        {
            g_st_ch395_sock[sockindex].tcp_conn_result = 0;
            aos_sem_signal(&g_st_ch395_sock[sockindex].st_conn_sem);
        }
   }

   if(sock_int_socket & SINT_STAT_DISCONNECT)                        /*  */
   {
        //LOGI(TAG, "sock %d disconnect ", sockindex);
       /*it means connect ok */
        if (g_st_ch395_sock[sockindex].proto_type == PROTO_TYPE_TCP
            && aos_sem_is_valid(&g_st_ch395_sock[sockindex].st_disconn_sem))
        {
            aos_sem_signal(&g_st_ch395_sock[sockindex].st_disconn_sem);
        }
   }

   if(sock_int_socket & SINT_STAT_TIM_OUT)                           /* */
   {
        //LOGI(TAG, "sock %d connect timeout ", sockindex);
         /*it means connect ok */
        if (g_st_ch395_sock[sockindex].proto_type == PROTO_TYPE_TCP
            && aos_sem_is_valid(&g_st_ch395_sock[sockindex].st_conn_sem))
        {
            g_st_ch395_sock[sockindex].tcp_conn_result = -1;
            aos_sem_signal(&g_st_ch395_sock[sockindex].st_conn_sem);
        }

        if (g_st_ch395_sock[sockindex].proto_type == PROTO_TYPE_UDP
            && aos_sem_is_valid(&g_st_ch395_sock[sockindex].st_send_sem))
        {
            g_st_ch395_sock[sockindex].send_result = -1;
            aos_sem_signal(&g_st_ch395_sock[sockindex].st_send_sem);
        }
    }

    if(sock_int_socket & SINT_STAT_RECV)                             /*  */
    {
        //LOGI(TAG, "sock %d recv data ", sockindex);
        /*get recv data length*/
        ret = ch395_socket_recv_data_len(sockindex, &recv_len);
        if (ret) {
            LOGE(TAG, "Fail to get sock %d recv length", sockindex);
            return ;
        }

        //LOGI(TAG, "sock %d recv data len %d", sockindex, recv_len);

        if(recv_len == 0)
        {
            LOGE(TAG, "sock %d no data need to recv ", sockindex);
            return ;
        }
        /* then we need to recv the data */
        precv_data = aos_malloc(recv_len);
        if (NULL == precv_data)
        {
            LOGE(TAG, "Fail to malloc %d ", recv_len);
            return ;
        }
        memset(precv_data, 0, recv_len);

        ret = ch395_socket_recv_data(sockindex, recv_len, precv_data);
        if (ret)
        {
            LOGE(TAG, "sock %d recv data fail len %d", sockindex, recv_len);
            aos_free(precv_data);
            return ;
        }

        sprintf(remote_ip, "%d.%d.%d.%d", g_st_ch395_sock[sockindex].destip);
        if (g_netconn_data_input_cb && (g_st_ch395_sock[sockindex].fd >= 0)) {
            if (g_netconn_data_input_cb(g_st_ch395_sock[sockindex].fd, precv_data, recv_len, remote_ip, g_st_ch395_sock[sockindex].dest_port)) {
                LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                    __func__, g_st_ch395_sock[sockindex].fd, recv_len);
            }
        }
        aos_free(precv_data);
   }
}

static void ch395_inter_proc(void)
{
    uint16_t ch395_int_status;
    uint8_t  dhcp_status = 0;
    uint8_t  phy_status = 0;
    uint32_t retry = 0;
    int32_t  ret = 0;

    while(1) {
        /*every 100 ms proc the chip interrupt*/
        aos_msleep(100);

        /*if dev haven't init , then nothing to do continue*/
        if (g_i8_dev_add_flag == 0 || g_i8_dev_init_flag == 0) {
            continue;
        }

        ch395_int_status = 0;
        ret = ch395_get_global_all_int_status(&ch395_int_status);
        //LOGI(TAG, "proc global interrup 0x%x ", ch395_int_status);
        if(ch395_int_status & GINT_STAT_UNREACH)
        {
            /* nothing to do for now*/
        }

        if(ch395_int_status & GINT_STAT_IP_CONFLI)
        {

        }

        if(ch395_int_status & GINT_STAT_PHY_CHANGE)
        {
            /*get phy status*/
            ret = ch395_get_phy_status(&phy_status);
            if (ret != 0) {
                LOGE(TAG, "Fail to get phy status");
                continue;
            }

            g_st_ch395info.phystate = phy_status;

            if(phy_status == PHY_DISCONN){
                LOGI(TAG, "eth link down");
            } else {
                /*start up to dhcp*/
                LOGI(TAG, "eth link up");
                ret = ch395_dhcp_enable(1);
                if (ret) {
                    LOGE(TAG, "Fail to enable dhcp");
                }
            }
        }

        if(ch395_int_status & GINT_STAT_DHCP)                            /* dhcp/pppoe interrup proc */
        {
            ret = ch395_dhcp_get_status(&dhcp_status);
            if (ret) {
                LOGE(TAG, "Fail to dhcp result");
                continue;
            }
            
            if (dhcp_status == 0) {
                /*try to get ip interface */
                do {
                    memset(&g_st_ch395info.ip_info, 0, sizeof(g_st_ch395info.ip_info));
                    ret = ch395_get_ip_interface(&g_st_ch395info.ip_info);
                    if (ret) {
                        LOGE(TAG, "Fail to get eth interface ip info");
                        continue;
                    }
                    if (g_st_ch395info.ip_info.ipaddr[0] != 0 && g_st_ch395info.ip_info.ipaddr[1] != 0) {
                        /* Post got ip event */
                        LOGI(TAG, "get ip info %d.%d.%d.%d gateway %d.%d.%d.%d mask %d.%d.%d.%d dns1 %d.%d.%d.%d dns2 %d.%d.%d.%d\r\n",
                            g_st_ch395info.ip_info.ipaddr[0], g_st_ch395info.ip_info.ipaddr[1], g_st_ch395info.ip_info.ipaddr[2], g_st_ch395info.ip_info.ipaddr[3],
                            g_st_ch395info.ip_info.gateway[0], g_st_ch395info.ip_info.gateway[1], g_st_ch395info.ip_info.gateway[2], g_st_ch395info.ip_info.gateway[3],
                            g_st_ch395info.ip_info.ip_mask[0], g_st_ch395info.ip_info.ip_mask[1], g_st_ch395info.ip_info.ip_mask[2], g_st_ch395info.ip_info.ip_mask[3],
                            g_st_ch395info.ip_info.ip_dns1[0], g_st_ch395info.ip_info.ip_dns1[1], g_st_ch395info.ip_info.ip_dns1[2], g_st_ch395info.ip_info.ip_dns1[3],
                            g_st_ch395info.ip_info.ip_dns2[0], g_st_ch395info.ip_info.ip_dns2[1], g_st_ch395info.ip_info.ip_dns2[2], g_st_ch395info.ip_info.ip_dns2[3]);

                        aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0u);
                        break;
                    }
                    aos_msleep(1000);
                    retry++;
                } while(retry < 10);
                

            } else {
                //LOGD(TAG, "dhcp time out, cannot get ip addr, it will go on dhcp after 16 second");
            }
        }
        if(ch395_int_status & GINT_STAT_SOCK0)
        {
            ch395_sock_interrupt_proc(0);                                     /* sock 0 interrupt proc */
        }
        if(ch395_int_status & GINT_STAT_SOCK1)
        {
            ch395_sock_interrupt_proc(1);                                     /* sock 1 interrupt proc*/
        }
        if(ch395_int_status & GINT_STAT_SOCK2)
        {
            ch395_sock_interrupt_proc(2);                                     /* sock 2 interrupt proc*/
        }
        if(ch395_int_status & GINT_STAT_SOCK3)
        {
            ch395_sock_interrupt_proc(3);                                     /* sock 3 interrupt proc*/
        }
        if(ch395_int_status & GINT_STAT_SOCK4)
        {
            //ch395_sock_interrupt_proc(4);                                     /* sock 4 interrupt proc*/
        }
        if(ch395_int_status & GINT_STAT_SOCK5)
        {
            //ch395_sock_interrupt_proc(5);                                     /* sock 5 interrupt proc*/
        }
        if(ch395_int_status & GINT_STAT_SOCK6)
        {
            //ch395_sock_interrupt_proc(6);                                     /* sock 6 interrupt proc*/
        }
        if(ch395_int_status & GINT_STAT_SOCK7)
        {
            //ch395_sock_interrupt_proc(7);                                     /* sock 7 interrupt proc*/
        }
    }
}

static void ch395_interface_info_init()
{
    memset(&g_st_ch395info, 0, sizeof(g_st_ch395info));
    g_st_ch395info.phystate = PHY_DISCONN;

    return ;
}

static void ch395_sock_info_init()
{
    int8_t i = 0;

    for (i = 0; i < MAX_SURPPORT_SOCK_NUM; i++) {
        memset(&g_st_ch395_sock[i], -1, sizeof(st_ch395_sock_t));

        if (i < 4)
        {
            g_st_ch395_sock[i].send_blk_num = 4;
            g_st_ch395_sock[i].recv_blk_num = 8;
        }
        aos_sem_new(&g_st_ch395_sock[i].st_send_sem, 0);
        aos_sem_new(&g_st_ch395_sock[i].st_conn_sem, 0);
        aos_sem_new(&g_st_ch395_sock[i].st_disconn_sem, 0);
        g_st_ch395_sock[i].sock_state = SINT_STAT_DISCONNECT;
    }
}

static void ch395_sock_info_deinit()
{
    uint8_t i = 0;
    for (i = 0; i < MAX_SURPPORT_SOCK_NUM; i++) {
        aos_sem_free(&g_st_ch395_sock[i].st_send_sem);
        aos_sem_free(&g_st_ch395_sock[i].st_conn_sem);
        aos_sem_free(&g_st_ch395_sock[i].st_disconn_sem);
        memset(&g_st_ch395_sock[i], -1, sizeof(st_ch395_sock_t));
    }
}

int32_t ch395_dev_init(void)
{
    int32_t ret = 0;
    uint8_t status = CH395_ERR_UNKNOW;
    uint8_t cmd = CMD0W_INIT_CH395;
    uint8_t i = 0;

    ret = ch395_spi_data_write(&cmd, 1);
    if (ret) {
        LOGE(TAG, "write init cmd fail %d ", ret);
        return -1;
    }
    /*should wait 350ms after init cmd*/
    aos_msleep(200);
    /*check cmd excute status*/
    while(1) {
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 && status == CMD_ERR_SUCCESS) {
            break;
        }
        if(i++ > 200) {
            return -1;
        }
        /* we should wait more than 2 ms to get cmd status*/
        aos_msleep(2);
    }

    return 0;
}

static int32_t HAL_SAL_Init(void)
{
    int32_t ret = 0;

    if (g_i8_dev_add_flag == 0) {
        LOGE(TAG, "%s device haven't add yet", TAG);
        return -1;
    }

    if (g_i8_dev_init_flag) {
        LOGE(TAG, "ch395 have already inited %d ", g_i8_dev_init_flag);
        return 0;
    }

    /*need to check wether need to set mac ip addr, gateway, mask etc*/
    ret = aos_mutex_new(&g_st_sock_mutex);
    if (ret) {
        LOGE(TAG, "Fail to new mutex 0x%x", ret);
        return -1;
    }

    ret = aos_task_new_ext(&g_st_ch395_int_task, TAG, ch395_inter_proc, NULL, CH395_TASK_SIZE, CH395_TASK_PRIO);
    if (ret) {
        LOGE(TAG, "Fail to start chip interrupt proc task 0x%x", ret);
        aos_mutex_free(&g_st_sock_mutex);
        return -1;
    }

    ch395_interface_info_init();

    ch395_sock_info_init();

    g_i8_dev_init_flag = 1;

    ret = ch395_set_func_param(SOCK_CTRL_FLAG_SOCKET_CLOSE);
    if (ret) {
        g_i8_dev_init_flag = 0;
        aos_mutex_free(&g_st_sock_mutex);
        aos_task_delete(TAG);
        ch395_sock_info_deinit();
        LOGE(TAG, "sal init fail : ch395 set func param fail %d", ret);
        return -1;
    }
    
    ret = ch395_dev_init();
    if (ret) {
        g_i8_dev_init_flag = 0;
        aos_mutex_free(&g_st_sock_mutex);
        aos_task_delete(TAG);
        ch395_sock_info_deinit();
        LOGE(TAG, "ch395 init fail %d", ret);
        return -1;
    }

    ret = ch395_ping_enable(1);
    if (ret) {
        LOGE(TAG, "ch395 ping enable fail %d", ret);
    }

    return 0;
}

static int32_t HAL_SAL_Deinit(void)
{
    if (g_i8_dev_init_flag == 0) {
        LOGE(TAG, "ch395 have not init yet ");
        return 0;
    }
    g_i8_dev_init_flag = 0;
    aos_mutex_free(&g_st_sock_mutex);
    aos_task_delete(TAG);
    ch395_sock_info_deinit();

    return 0;
}

static int32_t ch395_sock_client_open(uint8_t sock, uint8_t destip[4], uint16_t rport, uint16_t lport, uint8_t proto_type)
{
    int32_t ret = 0;

    ret = ch395_set_sock_proto_type(sock, proto_type);
    if (ret) {
        LOGE(TAG, "Fail to set sock %d proto type 0x%x", sock, proto_type);
        return -1;
    }

    ret = ch395_set_sock_dest_ip(sock, destip);
    if (ret) {
        LOGE(TAG, "Fail to set sock %d dest ip 0x%x", sock, destip);
        return -1;
    }
    
    ret = ch395_set_sock_dest_port(sock, rport);
    if (ret) {
        LOGE(TAG, "Fail to set sock %d remote port %d", sock, rport);
        return -1;
    }

    if (-1 != lport) {
        ret = ch395_set_sock_src_port(sock, lport);
        if (ret) {
            LOGE(TAG, "Fail to set sock %d local port %d", sock, lport);
            return -1;
        }
    }

    ret = ch395_socket_open(sock);
    if (ret) {
        LOGE(TAG, "Fail to open sock %d, proto 0x%x ip 0x%x dport %d lport %d",
            sock, proto_type, destip, rport, lport);
        return -1;
    }

    return 0;
}

static int32_t HAL_SAL_Start(sal_conn_t *conn)
{
    int32_t ret = 0;
    uint8_t linkid = 0;
    uint8_t remote_ip[4]  = {0};
    static int32_t test_count  = 0;

    if (!g_i8_dev_init_flag) {
        LOGE(TAG, "ch395 module haven't init yet ");
        return -1;
    }

    if (!conn || !conn->addr) {
        LOGE(TAG, "invalid input");
        return -1;
    }

    if (ch395_str_to_ip4addr(conn->addr, remote_ip) != 0) {
        /*input addr is domain , need to translated into binary */
        /*for now return -1*/
        LOGE(TAG, "%s %d Invalid ip str %s", __func__, __LINE__, conn->addr);
        return -1;
    }

    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    for (linkid = 0; linkid < MAX_SURPPORT_SOCK_NUM; linkid++) {
        if (g_st_ch395_sock[linkid].fd >= 0) {
            continue;
        }
        g_st_ch395_sock[linkid].fd = conn->fd;
        break;
    }
    aos_mutex_unlock(&g_st_sock_mutex);

    if (linkid >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "No sock available for now sock %d.", linkid);
        return -1;
    }

    switch (conn->type) {
        case TCP_SERVER:
            LOGE(TAG, "tcp server not supported for now");
            goto err;
            break;
        case TCP_CLIENT:
            ret = ch395_sock_client_open(linkid, remote_ip, conn->r_port, conn->l_port, PROTO_TYPE_TCP);
            if (ret) {
                LOGE(TAG, "Fail to open sock %d", linkid);
                goto err;
            }

            ret = ch395_socket_tcp_connect(linkid);
            if (ret) {
                ret = ch395_socket_close(linkid);
                if (ret) {
                    LOGE(TAG, "Fatal error ! socket close fail !");
                }
                goto err;
            }

            break;
        case UDP_UNICAST:
            ret = ch395_sock_client_open(linkid, remote_ip, conn->r_port, conn->l_port, PROTO_TYPE_UDP);
            if (ret) {
                LOGE(TAG, "Fail to open sock %d", linkid);
                goto err;
            }
            break;
        case SSL_CLIENT:
        case UDP_BROADCAST:
        default:
            LOGE(TAG, "ch395 module connect type %d not support ", conn->type);
            /*release the linkinfo*/
            goto err;
    }

    return 0;

err:
    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    g_st_ch395_sock[linkid].fd = -1;
    g_st_ch395_sock[linkid].dest_port = 0;
    g_st_ch395_sock[linkid].local_port = 0;
    memset(g_st_ch395_sock[linkid].destip, 0, 4);
    g_st_ch395_sock[linkid].proto_type = 0;
    g_st_ch395_sock[linkid].sock_state = 0;
    aos_mutex_unlock(&g_st_sock_mutex);
    return -1;
}

static int32_t HAL_SAL_Close(int fd, int32_t remote_port)
{
    int32_t ret = 0;
    int32_t linkid = 0;

    if (!g_i8_dev_init_flag) {
        LOGE(TAG, "ch395 module haven't init yet ");
        return -1;
    }

    linkid = fd_to_linkid(fd);

    if (linkid >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "No connection found for fd (%d)", fd);
        return -1;
    }

    if (g_st_ch395_sock[linkid].proto_type == PROTO_TYPE_TCP) {
        ret = ch395_socket_tcp_disconnect(linkid);
        if (ret) {
            LOGE(TAG, "Fail to disconnect %d tcp link and nothing to do", linkid);
        }
    }

    ret = ch395_clear_sock_recv_buff(linkid);
    if (ret) {
        LOGE(TAG, "sock %d fail to clear recv buff", linkid);
    }

    ret = ch395_socket_close(linkid);
    if (ret) {
        LOGE(TAG, "Fail to close sock %d ret %d", linkid, ret);
    }

    aos_mutex_lock(&g_st_sock_mutex, AOS_WAIT_FOREVER);
    g_st_ch395_sock[linkid].fd = -1;
    g_st_ch395_sock[linkid].dest_port = 0;
    g_st_ch395_sock[linkid].local_port = 0;
    memset(g_st_ch395_sock[linkid].destip, 0, 4);
    g_st_ch395_sock[linkid].proto_type = 0;
    g_st_ch395_sock[linkid].sock_state = 0;
    aos_mutex_unlock(&g_st_sock_mutex);

    return 0;

}

static int32_t HAL_SAL_Send(int32_t fd, uint8_t *data, uint32_t len,
                 char remote_ip[16], int32_t remote_port, int32_t timeout)
{
    int32_t ret = 0;
    int32_t linkid = 0;
    uint8_t destip[4] = {0};
    
    if (NULL == data || 0 == len) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    if (!g_i8_dev_init_flag) {
        LOGE(TAG, "ch395 module haven't init yet ");
        return -1;
    }

    linkid = fd_to_linkid(fd);

    if (linkid >= MAX_SURPPORT_SOCK_NUM) {
        LOGE(TAG, "No connection found for fd (%d)", fd);
        return -1;
    }
    
    if (len >= (g_st_ch395_sock[linkid].send_blk_num * CH395_BLK_SIZE)) {
        LOGE(TAG, "send data len %d over limite %d ", len, g_st_ch395_sock[linkid].send_blk_num * CH395_BLK_SIZE);
        return -1;
    }
#if 0
    if (NULL != remote_ip) {
        if (ch395_str_to_ip4addr(remote_ip, destip) != 0) {
            /*input addr is domain , need to translated into binary */
            /*for now return -1*/
            LOGE(TAG, "%s %d Invalid ip str %s", __func__, __LINE__, remote_ip);
            return -1;
        }

        if (g_st_ch395_sock[linkid].proto_type == PROTO_TYPE_TCP)
        {
            /*we should check dest ip and port is the same or not*/
            if (memcmp(destip, g_st_ch395_sock[linkid].destip, sizeof(destip)) != 0
                || g_st_ch395_sock[linkid].dest_port != remote_port) {
                LOGE(TAG, "dest ip is %d.%d.%d.%d , dest port is %d; sock connect ip %d.%d.%d.%d, port is %d",
                    destip[0], destip[1], destip[2], destip[3], remote_port, g_st_ch395_sock[linkid].destip[0],
                    g_st_ch395_sock[linkid].destip[1], g_st_ch395_sock[linkid].destip[2],
                    g_st_ch395_sock[linkid].destip[3], g_st_ch395_sock[linkid].dest_port);
                return -1;
            }
        }
    }
#endif    

    ret = ch395_socket_data_send(linkid, len, data);
    if (ret)
    {
        LOGE(TAG, "sock %d send data fail, len %d", linkid, len);
        return -1;
    }

    return 0;
}

int32_t ch395_sal_add_dev(void* data)
{
    int32_t ret = 0;
    uint8_t chip_ver = 0;
    uint8_t soft_ver = 0;

    if (NULL == data) {
        LOGE(TAG, "Invalid input");
        return -1;
    }

    if (g_i8_dev_add_flag) {
        LOGE(TAG, "ch395 device have already added flag :%d", g_i8_dev_add_flag);
        return 0;
    }

    memset(&g_st_spi_info, 0, sizeof(g_st_spi_info));
    memcpy(&g_st_spi_info, data, sizeof(g_st_spi_info));

    ret = hal_ch395_spi_init(&g_st_spi_info);
    if (ret) {
        LOGE(TAG, "spi init fail 0x%x, port %d, spi role %d, firstbit %d, work_mode %d, freq %d",
            ret, g_st_spi_info.port, g_st_spi_info.config.role, g_st_spi_info.config.firstbit,
            g_st_spi_info.config.mode, g_st_spi_info.config.freq);
        return -1;
    }

    ret = aos_mutex_new(&gst_spi_op_mutex);
    if (ret) {
        LOGE(TAG, "Failed to new a mutex 0x%x", ret);
        return -1;
    }

    g_i8_dev_add_flag = 1;

    ret = ch395_get_version(&chip_ver, &soft_ver);
    if (ret || chip_ver != 0x4 ) {
        LOGE(TAG, "Fail to get chip ver: 0x%x soft ver 0x%x ret : 0x%x", chip_ver, soft_ver, ret);
        g_i8_dev_add_flag = 0;
        return -1;
    }
#if 0
    /*check ch395 exist*/
    ret = ch395_chip_exist_check();
    if (ret) {
        LOGE(TAG, "Fail to check ch395 exist");
        g_i8_dev_add_flag = 0;
        return -1;
    }
#endif
    return 0;

}

static int32_t HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    /* add your code here */
    return -1;
}

static int32_t HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}

static sal_op_t sal_op = {
    .next = NULL,
    .version = "1.0.0",
    .name = "ch395",
    .add_dev = ch395_sal_add_dev,
    .init = HAL_SAL_Init,
    .start = HAL_SAL_Start,
    .send_data = HAL_SAL_Send,
    .domain_to_ip = HAL_SAL_DomainToIp,
    .finish = HAL_SAL_Close,
    .deinit = HAL_SAL_Deinit,
    .register_netconn_data_input_cb = HAL_SAL_RegisterNetconnDataInputCb,
};

/* Generated by aos tools, do NOT modify! */
#ifndef __SAL_DEV_NAME
#error "__SAL_DEV_NAME C mocro not defined, please define it in the SAL device driver."
#endif

int __SAL_DEV_NAME_sal_device_init(void)
{
    return sal_module_register(&sal_op);
}

