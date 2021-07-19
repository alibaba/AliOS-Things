/*
 * Copyright (C) 2020-2022 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include "sys/socket.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include "ulog/ulog.h"

#include "ch395_spi.h"
#include "ch395_cmd.h"

#define TAG "ch395_sal_module"

#define SPI_TIME_OUT          1000

#define TCP_MSS_MAX_VALUE     1460
#define TCP_MSS_MIN_VALUE     60
#define TCP_TTL_MAX_VALUE     128
#define MAX_BUF_NUM           48
#define IPV4_ADDR_STR_LEN     15

static int8_t guc_module_initflag = 0;
static spi_dev_t g_st_spi_info = {0};
static aos_mutex_t gst_spi_op_mutex = {0};

#define TAG "ch395_cmd"

int32_t ch395_str_to_ip4addr(int8_t *str, uint8_t *paddr)
{
    uint32_t i = 0;
    uint32_t len = 0;
    uint32_t ipaddr = 0;

    if (NULL == str || NULL == paddr) {
        LOGE(TAG, "%s %d Invalid input", __func__, __LINE__);
        return -1;
    }

    len = strlen(str);
    if (len > IPV4_ADDR_STR_LEN) {
        LOGE(TAG, "%s %d Invalid input %s ", __func__, __LINE__, str);
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

int32_t ch395_module_init(spi_dev_t *pstspiinfo)
{
    int32_t ret = 0;

    if (NULL == pstspiinfo) {
        LOGE(TAG, "%s %d invalid input \r\n", __FILE__, __LINE__);
        return -1;
    }

    if (guc_module_initflag) {
        return 0;
    }

    ret = hal_ch395_spi_init(pstspiinfo);
    if (ret) {
        LOGE(TAG, "spi init fail 0x%x, port %d, spi role %d, firstbit %d, work_mode %d, freq %d",
            ret, pstspiinfo->port, pstspiinfo->config.role, pstspiinfo->config.firstbit,
            pstspiinfo->config.mode, pstspiinfo->config.freq);
        return -1;
    }

    ret = aos_mutex_new(&gst_spi_op_mutex);
    if (ret) {
        LOGE(TAG, "Failed to new a mutex 0x%x", ret);
        return -1;
    }

    memset(&g_st_spi_info, 0, sizeof(g_st_spi_info));
    memcpy(&g_st_spi_info, pstspiinfo, sizeof(g_st_spi_info));

    guc_module_initflag = 1;
    return 0;
}

static int32_t ch395_spi_data_write(uint8_t *pdata, uint32_t len)
{
    int32_t ret = 0;

    if (NULL == pdata || 0 == len) {
        LOGE(TAG, "invalid input");
        return -1;
    }

    if (guc_module_initflag == 0) {
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

    if (guc_module_initflag == 0) {
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

static int32_t ch395_get_cmd_status(uint8_t *pstatus)
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

    if (guc_module_initflag == 0) {
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

    if (cmd[1] != ~readdata) {
        LOGE(TAG, "testdata 0x%x, readdata 0x%x ,chip not exist", cmd[1], readdata);
        return -1;
    }

    return 0;
}

int32_t ch395_chip_reset(void)
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

int32_t ch395_enter_sleep(void)
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

int32_t ch395_set_phy_state(uint8_t state)
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

int32_t ch395_set_mac_filt(uint8_t filt_type, uint32_t hash0, uint32_t hash1)
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

int32_t ch395_set_ip_addr(uint8_t ip[4])
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

int32_t ch395_set_gw_ip_addr(uint8_t ip[4])
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

int32_t ch395_set_ip_mask_addr(uint8_t ip[4])
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

int32_t ch395_get_global_int_status(uint8_t *pstatus)
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

int32_t ch395_get_global_all_int_status(uint16_t *pstatus)
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
int32_t ch395_tcp_retran_count_set(uint8_t count)
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
int32_t ch395_tcp_retry_period_set(uint16_t time)
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

int32_t ch395_get_remote_ip_port(uint8_t sock, uint8_t ip[4], uint16_t *pport)
{
    int32_t ret = 0;
    uint8_t send_data[2] = {0};
    uint8_t recv_data[6] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || NULL == pport) {
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

int32_t ch395_clear_sock_recv_buff(uint8_t sock)
{
    int32_t ret = 0;
    uint8_t data[2] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM) {
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

int32_t ch395_get_sock_tcp_status(uint8_t sock, uint8_t *psock_status, uint8_t *ptcp_status)
{
    int32_t ret = 0;
    uint8_t data[2] = {0};
    uint8_t recv_data[2] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || NULL == psock_status || NULL == ptcp_status) {
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

int32_t ch395_get_sock_int_status(uint8_t sock, uint8_t *pinterrupt)
{
    int32_t ret = 0;
    uint8_t send_data[2] = {0};
    uint8_t data = 0;

    if (sock >= MAX_SURPPORT_SOCK_NUM || NULL == pinterrupt) {
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

int32_t ch395_set_sock_dest_ip(uint8_t sock, uint8_t *pdst_ip)
{
    int32_t ret = 0;
    uint8_t data[6] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdst_ip) {
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

    return 0;
}

int32_t ch395_set_sock_dest_port(uint8_t sock, uint16_t dest_port)
{
    int32_t ret = 0;
    uint8_t data[4] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM) {
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

    return 0;
}

int32_t ch395_set_sock_src_port(uint8_t sock, uint16_t src_port)
{
    int32_t ret = 0;
    uint8_t data[4] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM) {
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

    return 0;
}

int32_t ch395_set_sock_proto_type(uint8_t sock, uint8_t proto_type)
{
    int32_t ret = 0;
    uint8_t data[3] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || proto_type > PROTO_TYPE_TCP) {
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

    return 0;
}

int32_t ch395_socket_open(uint8_t sock)
{
    int32_t ret = 0;
    int32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM) {
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
        if (ret == 0 && (status != CH395_ERR_BUSY)) {
            break;
        }
        aos_msleep(2);
    } while (retry < 20);

    if (retry <= 20) {
        return 0;
    }

    return -1;
}

int32_t ch395_socket_close(uint8_t sock)
{
    int32_t ret = 0;
    int32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM) {
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
        if (ret == 0 && (status != CH395_ERR_BUSY)) {
            break;
        }
        aos_msleep(2);
    } while (retry < 20);

    if (retry <= 20) {
        return 0;
    }

    return -1;
}

int32_t ch395_socket_tcp_connect(uint8_t sock)
{
    int32_t ret = 0;
    int32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM) {
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
        if (ret == 0 && (status != CMD_ERR_SUCCESS)) {
            break;
        }
        aos_msleep(2);
    } while (retry < 20);

    if (retry >= 20) {
        LOGE(TAG, "Fail to execute cmd tcp connect");
        return -1;
    }

    return 0;
}

int32_t ch395_socket_tcp_disconnect(uint8_t sock)
{
    int32_t ret = 0;
    uint32_t retry = 0;
    uint8_t status = 0;
    uint8_t data[2] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM) {
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
        if (ret == 0 && (status != CH395_ERR_BUSY)) {
            break;
        }
        aos_msleep(2);
    } while (retry < 20);

    if (retry >= 20) {
        LOGE(TAG, "Fail to execute cmd tcp disconnect");
        return -1;
    }

    return 0;
}

int32_t ch395_socket_data_send(uint8_t sock, uint16_t datalen, uint8_t *pdata)
{
    int32_t ret = 0;
    uint8_t *psend_data = NULL;
    uint32_t sendlen = 0;
    int32_t retry = 0;

    if (sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdata) {
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

    aos_mutex_lock(&gst_spi_op_mutex, AOS_WAIT_FOREVER);
    ret = hal_spi_send_ch395_sockdata(&g_st_spi_info, psend_data, sendlen, SPI_TIME_OUT);
    aos_mutex_unlock(&gst_spi_op_mutex);
    free(psend_data);
    if (ret) {
        LOGE(TAG, "Fail to send data 0x%x", sendlen);
        return -1;
    }

    return 0;
}

int32_t ch395_socket_recv_data_len(uint8_t sock, uint16_t *pdatalen)
{
    int32_t ret = 0;
    uint8_t send_data[2] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdatalen) {
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

int32_t ch395_socket_recv_data(uint8_t sock, uint16_t datalen, uint8_t *pdata)
{
    int32_t ret = 0;
    uint8_t send_data[4] = {0};

    if (sock >= MAX_SURPPORT_SOCK_NUM || NULL == pdata || datalen == 0) {
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

    if (enable != 0 && enable != 1) {
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

    if (enable != 0 && enable != 1) {
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
    while (1) {
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 && (status != CH395_ERR_BUSY)) {
            break;
        }
        if (i++ > 200) {
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
int32_t ch395_set_tcp_mss(uint16_t mss)
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

int32_t ch395_set_func_param(uint32_t param)
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

int32_t ch395_set_sock_ttl(uint8_t sock, uint8_t ttl)
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
int32_t ch395_sock_set_recv_buf(uint8_t sock, uint8_t startblk, uint8_t blknum)
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

    return 0;
}

int32_t ch395_sock_set_send_buf(uint8_t sock, uint8_t startblk, uint8_t blknum)
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

    return 0;

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
    while (1) {
        ret = ch395_get_cmd_status(&status);
        if (ret == 0 && status != CH395_ERR_BUSY) {
            break;
        }
        if (i++ > 200) {
            return -1;
        }
        /* we should wait more than 2 ms to get cmd status*/
        aos_msleep(2);
    }

    return 0;
}

