/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <sal_arch.h>
#include <sal_ipaddr.h>
#include <sal.h>
#include "sal_config_module.h"

#define TAG "esp8266_wifi_module"

char net_ipaddr[16], net_mask[16], net_gw[16];

static uint8_t inited = 0;
static link_t g_link[NETM_SOCKET_NUM];
static netconn_data_input_cb_t g_netconn_data_input_cb;
static k_task_handle_t tid;

static char *strlwr(char *str)
{
    char *orig = str;

    for (; *str != '\0'; str++) {
        *str = tolower(*str);
    }

    return orig;
}

#if 0
static int fd_to_linkid(int fd)
{
    int link_id;

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    for (link_id = 0; link_id < NETM_SOCKET_NUM; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    csi_kernel_mutex_unlock(g_link_mutex);

    return link_id;
}
#endif
static void set_link_status(netm_status_e status, uint8_t flag)
{

    if (flag == 0) {
        if (g_netm_status == NETM_STATUS_CONFIGING) {
            if ((status == NETM_STATUS_LINKDOWN) || (status == NETM_STATUS_SOCKET_DISCONNECT)) {
                return;
            }
        } else if (g_netm_status == NETM_STATUS_LINKDOWN) {
            if (status == NETM_STATUS_SOCKET_DISCONNECT) {
                return;
            }
        } else if (g_netm_status == NETM_STATUS_LINKUP) {
            if (status == NETM_STATUS_LINKDOWN) {
                return;
            }
        }
    }

    g_netm_status = status;
    LOGI(TAG, "netm status change to %d", status);
}

static k_timer_cb_t at_send_timeout_callback(uint32_t arg)
{
    static uint8_t retry = 0;

    retry++;

    if (retry > 10) {
        //Do: reset
    }

    netm_resp_msg.result = -1;
    g_cmd_inprogress = 0;
    csi_kernel_msgq_put(g_netm2app_queue,  &netm_resp_msg, BACK, NO_WAIT);

    LOGD(TAG, "at_send_timeout_callback");
    return NULL;
}

static void uartx_event_cb_fun(int32_t idx, usart_event_e event)
{
    netm_dev_t *udev = &g_netm_dev;
    usart_handle_t handle = udev->usart_handle;
    netm_msg_t netm_read_msg;

    switch (event) {
        case USART_EVENT_SEND_COMPLETE:
            csi_kernel_sem_post(udev->ksem_write);

            break;

        case USART_EVENT_RECEIVED: {
            uint8_t data[16];
            int32_t ret;

            memset(data, 0, 16);

            if (udev->rb == NULL) {
                break;
            }

#if 0

            /* flow ctrl */
            if (udev->flowctrl) {
                ret = ringbuffer_available_write_space(udev->rb);

                if (ret <= 64) {
                    csi_usart_set_interrupt(handle, USART_INTR_READ, 0);
                    break;
                }
            }

#endif
            ret = csi_usart_receive_query(handle, data, 16);

            ringbuffer_write(udev->rb, (uint8_t *)data, ret);

            if (csi_kernel_msgq_get_count(g_netm_queue) == 0) {
                netm_read_msg.msg_src = SRC_DRIVER;
                csi_kernel_msgq_put(g_netm_queue,  &netm_read_msg, BACK, NO_WAIT);
            }
        }
        break;

        case USART_EVENT_RX_OVERFLOW:
            LOGE(TAG, "uart fifo rx overflow!!");
            break;

        default:
            break;
    }
}

static int netm_uart_init(void)
{
    usart_handle_t      usart_handle;

    usart_handle = csi_usart_initialize(2, uartx_event_cb_fun);

    if (usart_handle == NULL) {
        LOGE(TAG, "uart_init error");
        return -1;
    }

    memset(&g_netm_dev, 0x00, sizeof(g_netm_dev));
    g_netm_dev.block = 1;
    g_netm_dev.ksem_write = csi_kernel_sem_new(1, 0);
    g_netm_dev.ksem_read = csi_kernel_sem_new(1, 0);
    g_netm_dev.usart_handle = usart_handle; // will be used in callback
    g_netm_dev.rb = ringbuffer_create(CONFIG_NETM_RDBUFSIZE);

    if (g_netm_dev.rb == NULL) {
        LOGE(TAG, "netm_uart rb_create error\n");
        return -1;
    }

    csi_usart_config(usart_handle, CONFIG_NETM_BAUD, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1,
                     USART_DATA_BITS_8);

#ifdef CONFIG_NETM_FLOW_CONTROL
    ret = csi_usart_config_flowctrl(usart_handle, USART_FLOWCTRL_CTS_RTS);

    if (ret == 0) {

        g_netm_dev.flowctrl = 1;
        LOGD(TAG, "set flowctrl success");

    } else if (ret < 0) {
        LOGE(TAG, "set flowctrl error %x", ret);
        return -1;
    }

#endif

    return 0;
}

static int netm_uart_write(const char *buf, uint32_t nbytes)
{
    int ret;

    csi_usart_send(g_netm_dev.usart_handle, buf, nbytes);

    //wait transimit done
    ret = csi_kernel_sem_wait(g_netm_dev.ksem_write, -1);

    if (ret < 0) {
        return ret;
    }

    return nbytes;
}

int netm_softreset(void)
{
    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_RST;
    cmd.timeout = 5000;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
        }

        if (resp.result == 1) { //response data parse
            ret = 0;
        } else {
            ret = -1;
            LOGE(TAG, "rst error");
        }

    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;
}

int netm_close_echo(uint8_t isAsync)
{
    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_INIT;
    cmd.timeout = 1000;

    ret = csi_kernel_msgq_put(g_netm_queue,  &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, -1);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "close_echo timeout");
        }

        if (resp.result == 1) { //response data parse
            ret = 0;
        } else {
            ret = -1;
        }
    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;
}

int netm_set_mode(uint8_t mode)
{
    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_MODE;
    cmd.param[0] = mode;
    cmd.timeout = 2000;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGD(TAG, "set_mode timeout");
        }

        if (resp.result == 1) { //response data parse
            ret = 0;
        } else {
            ret = -1;
        }

    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;

}

int netm_set_mux_mode(uint8_t mode, uint8_t isAsync)
{
    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_MUX_MODE;
    cmd.timeout = 1000;
    cmd.param[0] = mode;

    ret = csi_kernel_msgq_put(g_netm_queue,  &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, -1);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "set mux timeout");
        }

        if (resp.result == 1) { //response data parse
            ret = 0;
        } else {
            ret = -1;
        }
    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;
}

int netm_connect(int id, netm_conn_e type, char *srvname, uint16_t port)
{
    /*
    AT+CIPSTART="TCP","192.168.103.121",6000
    CONNECT
    */

    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;
    connect_param_t param;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src  = SRC_APP;
    cmd.cmd      = NETM_CMD_CONN;
    param.id     = id;
    param.type   = type;
    param.hostip = srvname;
    param.port   = port;
    memcpy(cmd.param, &param, sizeof(param));
    cmd.timeout = 5000;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "connect timeout");
        }

        if (resp.result == 1) { //response data parse
            ret = 0;
        } else {
            ret = -1;
        }

    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;

}

int netm_mux_send(int id, const uint8_t *pdata, int len)
{
    int ret = -1;
    static uint8_t retry = 0;
    netm_msg_t cmd;
    netm_msg_t resp;
    mux_send_param_t param;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    if (id >= NETM_SOCKET_NUM) {
        LOGE(TAG, "id(%d) err", id);
        return -1;
    }

    if (len <= 0 || len > 1460) {
        LOGE(TAG, "len(%d) err");
        return -1;
    }

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_MUX_SEND;
    param.id = id;
    param.data = (uint8_t *)pdata;
    param.len = len;
    memcpy(cmd.param, &param, sizeof(param));
    cmd.timeout = 5000; //netm_sockets[id].send_timeout;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result > 0) { //response data parse
            ret = resp.result;
        } else {
            if (resp.result == -1) {
                ret = -1;
                LOGE(TAG, "send timeout");
            } else {
                ret = -1;
            }

            retry++;

            if (retry > 3) {
                //DO : reset
            }
        }
    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;
}

int netm_parse_domain(const char *servername, char *ip)
{
    /*
    AT+CIPSTART="TCP","192.168.103.121",6000
    CONNECT
    */

    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);
    memset(&resp, 0, sizeof(netm_msg_t));

    cmd.msg_src  = SRC_APP;
    cmd.cmd      = NETM_CMD_DOMAIN;
    strcpy(cmd.param, strlwr((char *)servername));
    cmd.timeout  = 5000;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "domain timeout");
        } else if (resp.result == 1) { //response data parse
            strcpy(ip, resp.param);
            ret = 0;

        } else {
            ret = -1;
        }

    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;

}

int netm_disconnect(int id)
{
    /*
    AT+CIPSTART="TCP","192.168.103.121",6000
    CONNECT
    */

    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src  = SRC_APP;
    cmd.cmd      = NETM_CMD_DISC;
    cmd.timeout  = 1000;
    cmd.param[0] = id;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "disconnect timeout");
        }

        if (resp.result == 1) { //response data parse
            ret = 0;
        } else {
            ret = 0;
        }
    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;

}

int netm_get_local_ipaddr(char *buf)
{
    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_INFO;
    cmd.timeout = 2000;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "ping timeout");
        }

        if (resp.result == 1) { //response data parse
            strcpy((char *)buf, resp.param);
            ret = 0;
        } else {
            ret = -1;
        }

    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;

}

int netm_get_link_status(void)
{
    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_STAT;
    cmd.timeout = 1000;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "link_status timeout");
        }

        if (resp.result == 1) { //response data parse
            ret = resp.param[0];
        } else {
            ret = -1;
        }

    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;
}

int netm_set_default_ap(const char *ssid, const char *psw)
{
    /*
    AT+CWJAP_DEF="TP-LINK_ZTW","pass.work"
    OK
    */

    int ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;
    ap_set_param_t param;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    csi_kernel_mutex_lock(g_cmd_mutex, -1);

    cmd.msg_src = SRC_APP;
    cmd.cmd = NETM_CMD_SET_AP;
    param.ssid = (char *)ssid;
    param.psw  = (char *)psw;
    memcpy(((ap_set_param_t *)cmd.param), &param, sizeof(param));
    cmd.timeout = 30000;

    ret = csi_kernel_msgq_put(g_netm_queue, &cmd, BACK, NO_WAIT);

    if (ret < 0) {
        csi_kernel_mutex_unlock(g_cmd_mutex);
        return ret;
    }

    ret = csi_kernel_msgq_get(g_netm2app_queue, &resp, WAIT_FOREVER);  //wait CMD finish

    if (ret == 0) {

        if (resp.result == -1) {
            LOGE(TAG, "set_default_ap timeout");
            ret = -1;
        } else if (resp.result == 1) { //response data parse
            ret = 0;
        } else {
            ret = -1;
        }
    }

    csi_kernel_mutex_unlock(g_cmd_mutex);

    return ret;
}

/***********************
* channel interface
************************/
static int read_esp8266_one_line(char *line, int nbytes)
{
    char ch = 0;
    int line_len = 0;
    char ch_pre = 0;
    int data_in_rb;
    int sleep_cnt = 0;

    if (line == NULL || nbytes <= 0) {
        return -1;
    }

    if (ringbuffer_available_read_space(g_netm_dev.rb) <= 0) {
        return -1;
    }

    while (1) {
        if ((data_in_rb = ringbuffer_read(g_netm_dev.rb, (uint8_t *)&ch, 1)) > 0) {
            sleep_cnt = 0;

            if ((ch_pre == '\r') && (ch == '\n')) {
                if (line_len <= 1) {
                    line_len -= 1;  // remove continued \r\n
                } else {
                    line_len -= 1;    //remove '\r'
                    line[line_len] = '\0';
                    break;
                }
            } else {
                line[line_len++] = ch;
                ch_pre = ch;

                if (line_len == nbytes) {
                    //dont find '\r\n' in nbytes, maybe data received
                    break;
                }
            }
        } else {
            aos_msleep(10);

            if (sleep_cnt++ >= 2) { // 128 bytes should be received in 20ms
                line[line_len] = '\0';
                break;
            }
        }
    }

    return line_len;
}

static int process_at_resp_event(char *line, int line_len, rdata_cb recv_handle_cb)
{
    int data_len = 0;
    int prefix_len = 0;
    int remain_len;
    int data_off = 0;
    int id = -1;
    int got_frame = 1;
    int sleep_cnt = 0;
    char *recvdata = NULL;

    if (strstr(line, "+IPD")) {
        char *str1 = line;
        char *str2;

        prefix_len += strlen("+IPD,");
        str1 += prefix_len;
        str2 = strtok(str1, ",");
        prefix_len += strlen(str2);
        prefix_len += 1;    // ','
        id = atoi(str2);

        if (id >= NETM_SOCKET_NUM) {
            LOGE(TAG, "socket_id %d error", id);
            return -1;
        }

        str2 = strtok(NULL, ":");
        prefix_len += strlen(str2);
        prefix_len += 1;    // ':'
        data_len = atoi(str2);

        if (data_len == 0) {
            LOGE(TAG, "socket[%d] recv %d data", id, data_len);
            return -1;
        }

        //memcpy data to app_buf
        //LOGD(TAG, "id = %d, prefix =%d, data_len =%d", id, prefix_len, data_len);
        LOGD(TAG, "+IPD,%d,%d:\n", id, data_len);

        recvdata = (char *)aos_malloc(data_len + 1);

        if (!recvdata) {
            LOGE(TAG, "Error: %s %d out of memory.", __func__, __LINE__);
            return -1;
        }

        memset(recvdata, 0, data_len + 1);

        /* write head to socket buf |len|data */
        memcpy(recvdata, (line + prefix_len), line_len - prefix_len);
        data_off = line_len - prefix_len;

        remain_len = data_len - (line_len - prefix_len);

        if ((remain_len > 0) && (line_len != LINE_LEN)) {
            line[0] = '\r';
            line[1] = '\n';

            memcpy(recvdata + data_off, (uint8_t *)line, 2);
            data_off += 2;

            remain_len -= 2;
        }

        while (remain_len > ringbuffer_available_read_space(g_netm_dev.rb)) {
            csi_kernel_delay_ms(10);

            if (sleep_cnt++ > 10) {
                break;
            }
        }

        // read remained data, try to reduce memory size by reading repeatly
        while (remain_len > 0) {
            int read_len;

            if (remain_len >= LINE_LEN) {
                read_len = LINE_LEN;
            } else {
                read_len = remain_len;
            }

            if (read_len == ringbuffer_read(g_netm_dev.rb, (uint8_t *)line, read_len)) {
                memcpy(recvdata + data_off, (uint8_t *)line, read_len);
                data_off += read_len;
                remain_len -= read_len;
            } else {
                got_frame = 0;
                LOGE(TAG, "don't got frame !!!!");
                break;
            }

        }

        if (got_frame == 1) {
            recv_handle_cb(id, recvdata, data_len, got_frame);
        }

        aos_free(recvdata);
    }

    return 0;

}

static int process_at_resp_cmd(char *line, int line_len)
{
    int ret = -1;
    int end = 0;

    switch (g_cmd_inprogress) {

        case NETM_CMD_RST :

            if (strcmp(line, "ready") == 0) {
                set_link_status(NETM_STATUS_READY, 0);
                netm_resp_msg.result = 1;
                end = 1;
                LOGI(TAG, "wifi %s", line);
            }

            break;

        case NETM_CMD_INFO :
            if (strstr(line, "+CIPSTA:ip:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTA:ip:");

                strcpy(netm_resp_msg.param, str1);
                strncpy(net_ipaddr, str1 + 1, (strlen(str1) - 2));
            }

            if (strstr(line, "+CIPSTA:gateway:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTA:gateway:");

                strncpy(net_gw, str1 + 1, (strlen(str1) - 2));
            }

            if (strstr(line, "+CIPSTA:netmask:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTA:netmask:");

                strncpy(net_mask, str1 + 1, (strlen(str1) - 2));

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_INFO_MAC :

            if (strstr(line, "+CIPSTAMAC:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTAMAC:");

                strcpy(netm_resp_msg.param, str1);

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_INFO_LINK :

            if (strstr(line, "+CWJAP:")) {
                char *str1 = line;

                str1 += strlen("+CWJAP:");

                strcpy(netm_resp_msg.param, str1);

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_SET_AP :
            if (strstr(line, "WIFI CONNECTED")) {
                set_link_status(NETM_STATUS_LINKUP, 0);
            } else if (strstr(line, "WIFI GOT IP")) {
                set_link_status(NETM_STATUS_GOTIP, 0);
            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "FAIL") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_AP_DISC :

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;

            } else  if (strstr(line, "WIFI DISCONNECT")) {
                set_link_status(NETM_STATUS_LINKDOWN, 0);
                end = 1;
            }

            break;

        case NETM_CMD_MODE :
            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            }

            break;

        case NETM_CMD_RESTORE :
            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
            } else if (strcmp(line, "ready") == 0) {
                set_link_status(NETM_STATUS_READY, 0);
                end = 1;
            }

            break;

        case NETM_CMD_UART_CONFIG :
            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            }

            break;

        case NETM_CMD_INIT :

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            }

            break;

        case NETM_CMD_CONN :

            if (strstr(line, "CONNECT")) {

                LOGD(TAG, "connected\n");
                char *str1 = line;
                char *str2;

                str2 = strtok(str1, ",");
                netm_resp_msg.param[0] = atoi(str2);

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_MUX_SEND :

            if (strstr(line, ">")) {
                if (g_mux_send_frame.len > 0 && g_mux_send_frame.len <= 1460) {
                    ret = netm_uart_write(g_mux_send_frame.data, g_mux_send_frame.len);
                } else {
                    ret = -1;
                }

            } else if (strstr(line, "SEND OK")) {
                netm_resp_msg.result = g_mux_send_frame.len;         //wait sendok
                end = 1;
            } else if (strstr(line, "SEND FAIL")) {
                netm_resp_msg.result = 0;                           //send fail
                end = 1;
            } else if (strstr(line, "link is not valid")) {
                netm_resp_msg.result = 0;                           //send fail
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_DISC:

            if (strstr(line, "CLOSED")) {
                char *str1 = line;
                char *str2;

                str2 = strtok(str1, ",");
                netm_resp_msg.param[0] = atoi(str2); //id
            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            }

            break;

        case NETM_CMD_MUX_MODE :

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_STAT :

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            } else if (strstr(line, "STATUS:")) {
                char *str1 = line;
                int status;

                str1 += strlen("STATUS:");
                status = atoi(str1);

                if (status == 2) {
                    netm_resp_msg.param[0] = NETM_STATUS_GOTIP;
                    set_link_status(NETM_STATUS_GOTIP, 0);
                } else if (status == 3) {
                    netm_resp_msg.param[0] = NETM_STATUS_SOCKET_CONNECTED;
                    set_link_status(NETM_STATUS_SOCKET_CONNECTED, 0);
                } else if (status == 4) {
                    netm_resp_msg.param[0] = NETM_STATUS_SOCKET_DISCONNECT;
                    set_link_status(NETM_STATUS_SOCKET_DISCONNECT, 0);
                } else if (status == 5) {
                    netm_resp_msg.param[0] = NETM_STATUS_LINKDOWN;
                    set_link_status(NETM_STATUS_LINKDOWN, 0);
                }

            }

            break;

        case NETM_CMD_DOMAIN :

            if (strcmp(line, "OK") == 0) {
                end = 1;

            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            } else if (strstr(line, "+CIPDOMAIN:")) {

                char *str1 = line;

                str1 += strlen("+CIPDOMAIN:");

                memset(netm_resp_msg.param, 0, sizeof(netm_resp_msg.param));
                memcpy(netm_resp_msg.param, str1, strlen(str1));
                netm_resp_msg.result = 1;
            }

            break;

        case NETM_CMD_SHUT:

            if (strstr(line, "SHUT OK")) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_PING:

            if ((line[0] == '+') && (strlen(line) >= 2)) {
                netm_resp_msg.param[0] = atoi(&line[1]);

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            }

            break;

        case NETM_CMD_RAW :

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end = 1;
            } else {
                strcpy(netm_resp_msg.param, line);
            }

            break;

        default:
            break;
    }

    if (strstr(line, "WIFI DISCONNECT") != NULL) {
        set_link_status(NETM_STATUS_LINKDOWN, 0);
    }

    if (strstr(line, "WIFI CONNECTED") != NULL) {
        set_link_status(NETM_STATUS_LINKUP, 0);
    }

    if (strstr(line, "WIFI GOT IP") != NULL) {
        set_link_status(NETM_STATUS_GOTIP, 0);
    }

    if (strstr(line, "Smart get wifi info") != NULL) {
        set_link_status(NETM_STATUS_CONFIGING, 0);
    }

    if (strstr(line, "smartconfig connected wifi") != NULL) {
        set_link_status(NETM_STATUS_GOTIP, 0);
    }

    if ((end != 1) && strcmp(line, "ready") == 0) {
        set_link_status(NETM_STATUS_READY, 0);
        LOGI(TAG, "%s", line);
    }

    if ((g_netm_status > NETM_STATUS_LINKUP) && (end != 1) && (strstr(line, "CLOSED") != NULL)) {
        char *str1 = line;
        char *str2;

        str2 = strtok(str1, ",");
        LOGI(TAG, "server closed socket[%d]", atoi(str2));
    }

    if (end) {
        csi_kernel_timer_stop(timer_atmsg);   //timer_del

        g_cmd_inprogress = 0;
        ret = csi_kernel_msgq_put(g_netm2app_queue,  &netm_resp_msg, BACK, NO_WAIT);

        if (ret < 0) {
            LOGE(TAG, "driver msgq_put error");
            return ret;
        }
    }

    return 0;

}

static int handle_at_cmd(netm_msg_t *msg)
{
    char cmd[64];

    memset(cmd, 0, sizeof(cmd));

    g_cmd_inprogress = msg->cmd;

    switch (msg->cmd) {
        case NETM_CMD_INIT:
            sprintf(cmd, "ATE0\r\n");
            break;

        case NETM_CMD_RST:
            sprintf(cmd, "AT+RST\r\n");
            break;

        case NETM_CMD_INFO_LINK:
            sprintf(cmd, "AT+CWJAP?\r\n");
            break;

        case NETM_CMD_INFO_MAC:
            sprintf(cmd, "AT+CIPSTAMAC?\r\n");
            break;

        case NETM_CMD_INFO :
            sprintf(cmd, "AT+CIPSTA?\r\n");
            break;

        case NETM_CMD_SET_AP : {

            ap_set_param_t param;
            memcpy(&param, msg->param, sizeof(param));
            sprintf(cmd, "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", param.ssid, param.psw);
        }
        break;

        case NETM_CMD_AP_DISC :
            sprintf(cmd, "AT+CWQAP\r\n");
            break;

        case NETM_CMD_MODE :
            sprintf(cmd, "AT+CWMODE_DEF=%d\r\n", msg->param[0]);
            break;

        case NETM_CMD_RESTORE :
            sprintf(cmd, "AT+RESTORE\r\n");
            break;

        case NETM_CMD_UART_CONFIG : {

            uart_config_param_t param;
            memcpy(&param, msg->param, sizeof(param));

            if (param.writetoflash == 0) {
                sprintf(cmd, "AT+UART_CUR=%d,%d,%d,%d,%d\r\n", param.baud, param.databits, param.stopbits, param.parity,
                        param.flow_control);
            } else {
                sprintf(cmd, "AT+UART_DEF=%d,%d,%d,%d,%d\r\n", param.baud, param.databits, param.stopbits, param.parity,
                        param.flow_control);
            }
        }
        break;

        case NETM_CMD_CONN: {
            connect_param_t param;
            memcpy(&param, msg->param, sizeof(param));

            if (param.type == NETM_CONN_TCP) {
                sprintf(cmd, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d\r\n", param.id, param.hostip, param.port);
            } else {
                sprintf(cmd, "AT+CIPSTART=%d,\"UDP\",\"%s\",%d\r\n", param.id, param.hostip, param.port);
            }

            break;
        }

        case NETM_CMD_MUX_SEND: {
            memcpy(&g_mux_send_frame, msg->param, sizeof(mux_send_param_t));
            sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", g_mux_send_frame.id, g_mux_send_frame.len);
            break;
        }

        case NETM_CMD_DISC: {
            sprintf(cmd, "AT+CIPCLOSE=%d\r\n", msg->param[0]);
            break;
        }

        case NETM_CMD_STAT:
            sprintf(cmd, "AT+CIPSTATUS\r\n");
            break;

        case NETM_CMD_PING:
            sprintf(cmd, "AT+PING=\"%s\"\r\n", msg->param);
            break;

        case NETM_CMD_DOMAIN :
            sprintf(cmd, "AT+CIPDOMAIN=\"%s\"\r\n", msg->param);
            break;

        case NETM_CMD_SHUT:
            sprintf(cmd, "AT+CIPSHUT\r\n");
            break;

        case NETM_CMD_MUX_MODE:

            sprintf(cmd, "AT+CIPMUX=%d\r\n", msg->param[0]);
            break;

        case NETM_CMD_RAW:
            sprintf(cmd, "%s\r\n", msg->param);
            break;

        default:
            break;
    }

    LOGD(TAG, "write_cmd {%s}", cmd);

    if (netm_uart_write(cmd, strlen(cmd)) > 0) {
        csi_kernel_timer_start(timer_atmsg, csi_kernel_ms2tick(msg->timeout));  //start timer

        return 0;
    }

    LOGE(TAG, "handle_at_cmd error");

    return -1;

}

static int handle_at_response(netm_msg_t *msg)
{
    char line[LINE_LEN];
    int line_len;

    memset(line, 0, LINE_LEN);

    while ((line_len = read_esp8266_one_line(line, sizeof(line))) > 0) {
        if (strstr(line, "+IPD") || strstr(line, "+RECEIVE")) {
            process_at_resp_event(line, line_len, esp8266_wifi_module_socket_data_handle);
        } else {
            LOGD(TAG, "read lines {%s}\n", line);
            process_at_resp_cmd(line, line_len);
        }
    }

    return 0;
}

static void netm_task(void *arg)
{
    netm_msg_t msgbuf;
    int  ret = 0;

    while (1) {

        memset(&msgbuf, 0, sizeof(msgbuf));
        ret = csi_kernel_msgq_get(g_netm_queue, (void *)&msgbuf, -1);

        if (ret < 0) {
            LOGE(TAG, "recv msg fail,error:%x", ret);
            continue;
        }

        if (msgbuf.msg_src == SRC_APP) {
            handle_at_response(&msgbuf);
            handle_at_cmd(&msgbuf);
        } else if (msgbuf.msg_src == SRC_DRIVER) {
            handle_at_response(&msgbuf);
        }

    }/* while */
}

static int esp8266_uart_init(void)
{
    int ret = 0;

    netm_softreset();

    if (ret < 0) {
        return ret;
    }

    netm_close_echo(0);

    if (ret < 0) {
        return ret;
    }

    ret = netm_set_mode(1);

    if (ret < 0) {
        return ret;
    }

    ret = netm_set_mux_mode(1, 0);

    if (ret < 0) {
        return ret;
    }

    return 0;
}

static void esp8266_get_ip_delayed_action(void *arg)
{
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
}


int esp8266_wifi_got_ip(const char *ssid, const char *psw)
{
    char rsp[LINE_LEN] = {0};

    int ret = netm_set_default_ap(ssid, psw);

    if (ret < 0) {
        LOGE(TAG, "WIFI config error try again");
        return ret;
    }

    netm_get_local_ipaddr(rsp);

    LOGD(TAG, "esp8266 got ip %s \r\n", rsp);
    /*delay 5 seconds to post got ip event*/
    aos_post_delayed_action(100, esp8266_get_ip_delayed_action, NULL);
    return 0;
}

static void esp8266_wifi_module_socket_data_handle(int fd, void *data, size_t len, bool enable)
{
    /*
    +IPD,0,44,106.15.213.199,5684:
    */

    if (g_netconn_data_input_cb && (enable == 1)) {
        if (g_netconn_data_input_cb(fd, data, len, NULL, 0)) {
            LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                 __func__, fd, len);
        }
    }

    return;
}

/*****************************************
* esp8266_wifi_module interface
*****************************************/
static int esp8266_wifi_module_init(void)
{
    int ret = 0;
    int i;

    if (inited) {
        LOGI(TAG, "esp8266 wifi module have already inited \r\n");
        return 0;
    }

    ret = netm_uart_init();

    if (ret != 0) {
        return ret;
    }

    for (i = 0; i < NETM_SOCKET_NUM; i++) {
        memset(g_link, 0, sizeof(g_link));
        g_link[i].fd = -1;
    }

    g_cmd_mutex = csi_kernel_mutex_new();

    g_link_mutex = csi_kernel_mutex_new();

    if (NULL == g_cmd_mutex) {
        LOGE(TAG, "Creating cmd mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    if (NULL == g_link_mutex) {
        LOGE(TAG, "Creating cmd mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    g_netm_queue = csi_kernel_msgq_new(5, sizeof(netm_msg_t));

    if (NULL == g_netm_queue) {
        LOGE(TAG, "Creating netm queue failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    g_netm2app_queue = csi_kernel_msgq_new(5, sizeof(netm_msg_t));

    if (NULL == g_netm2app_queue) {
        LOGE(TAG, "Creating 2app queue failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    timer_atmsg = csi_kernel_timer_new((k_timer_cb_t)at_send_timeout_callback, KTIMER_TYPE_ONCE, NULL);

    if (NULL == timer_atmsg) {
        LOGE(TAG, "Creating msg_timer failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    ret = esp8266_uart_init();

    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    inited = 1;

    return 0;
err:

    if (g_cmd_mutex != NULL) {
        csi_kernel_mutex_del(g_cmd_mutex);
    }

    if (g_netm_queue != NULL) {
        csi_kernel_msgq_del(g_netm_queue);
    }

    if (g_netm2app_queue != NULL) {
        csi_kernel_msgq_del(g_netm2app_queue);
    }

    if (timer_atmsg != NULL) {
        csi_kernel_timer_del(timer_atmsg);
    }

    return -1;
}


static int esp8266_wifi_module_deinit()
{
    if (!inited) {
        return 0;
    }

    csi_kernel_mutex_del(g_cmd_mutex);

    csi_kernel_msgq_del(g_netm_queue);

    csi_kernel_msgq_del(g_netm2app_queue);

    csi_kernel_timer_del(timer_atmsg);

    inited = 0;
    return 0;
}

static int esp8266_wifi_module_domain_to_ip(char *domain, char ip[16])
{
    int ret = -1;

    if (!inited) {
        LOGE(TAG, "%s esp8266 wifi module haven't init yet \r\n", __func__);
        return -1;
    }

    if (NULL == domain || NULL == ip) {
        LOGE(TAG, "invalid input at %s \r\n", __func__);
        return -1;
    }

    ret = netm_parse_domain(domain, ip);

    if (ret < 0) {
        LOGE(TAG, "domain to ip fail %s \r\n", __func__);
        return -1;
    }

    return 0;
}

static int esp8266_wifi_module_conn_start(sal_conn_t *conn)
{
    int  linkid = 0;
    int ret = -1;
    netm_conn_e net_type = 0;

    if (!inited) {
        LOGE(TAG, "%s SAL wifi haven't init yet \r\n", __func__);
        return -1;
    }

    if (!conn || !conn->addr) {
        LOGE(TAG, "%s %d - invalid input \r\n", __func__, __LINE__);
        return -1;
    }

    csi_kernel_mutex_lock(g_link_mutex, -1);

    linkid = conn->fd ;//- SAL_SOCKET_OFFSET;

    if (linkid >= NETM_SOCKET_NUM) {
        LOGE(TAG, "No link available for now, %s failed. \r\n", __func__);
        return -1;
    }


    switch (conn->type) {
        case TCP_SERVER:
        case TCP_CLIENT:
            net_type = NETM_CONN_TCP;
            break;

        case UDP_UNICAST:
            net_type = NETM_CONN_UDP;
            break;

        case SSL_CLIENT:
        case UDP_BROADCAST:
        default:
            break;
    }

    ret = netm_connect(linkid, net_type, conn->addr, conn->r_port);

    if (ret < 0) {
        LOGE(TAG, "conn_start fail %s \r\n", __func__);
        //csi_kernel_mutex_lock(g_link_mutex, -1);
        //g_link[linkid].fd = -1;
        //csi_kernel_mutex_unlock(g_link_mutex);
        return -1;
    }

    return 0;

}

static int esp8266_wifi_module_conn_close(int fd, int32_t remote_port)
{
    int  linkid = 0;
    int  ret = 0;

    if (!inited) {
        LOGE(TAG, "%s esp8266 wifi module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd; //fd_to_linkid(fd);

    if (linkid >= NETM_SOCKET_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    ret = netm_disconnect(linkid);

    if (ret < 0) {
        LOGE(TAG, "conn_close fail %s \r\n", fd, __func__);
        return -1;
    }

    //csi_kernel_mutex_lock(g_link_mutex, -1);
    //g_link[linkid].fd = -1;
    //csi_kernel_mutex_unlock(g_link_mutex);

    return ret;
}

static int esp8266_wifi_module_send(int fd, uint8_t *data, uint32_t len,
                                    char remote_ip[16], int32_t remote_port, int32_t timeout)
{
    int  linkid;
    int  ret = -1;

    if (!inited) {
        LOGE(TAG, "%s esp8266 wifi module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd; //fd_to_linkid(fd);

    if (linkid >= NETM_SOCKET_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    ret = netm_mux_send(linkid, data, len);

    if (ret < 0) {
        LOGE(TAG, "%s esp8266 wifi module send fail\r\n", __func__);
        return ret;
    }

    return 0;
}

static int esp8266_wifi_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }

    return 0;
}

sal_op_t esp8266_sal_opt;

int esp8266_sal_init()
{
    k_status_t status = 0;

    esp8266_sal_opt.version = "1.0.0";
    esp8266_sal_opt.init = esp8266_wifi_module_init;
    esp8266_sal_opt.start = esp8266_wifi_module_conn_start;
    esp8266_sal_opt.send = esp8266_wifi_module_send;
    esp8266_sal_opt.domain_to_ip = esp8266_wifi_module_domain_to_ip;
    esp8266_sal_opt.close = esp8266_wifi_module_conn_close;
    esp8266_sal_opt.deinit = esp8266_wifi_module_deinit;
    esp8266_sal_opt.register_netconn_data_input_cb = esp8266_wifi_packet_input_cb_register;

    LOGD(TAG, "start netm task");
#ifndef AOS_NO_WIFI
    status = csi_kernel_task_new(netm_task, "netm", NULL, KPRIO_NORMAL_BELOW0, 0, NULL, 2048, &tid);
#endif

    if (status != 0) {
        LOGE(TAG, "WIFI SAL Task creation failed %d\r\n", status);
        return -1;
    }

    sal_module_register(&esp8266_sal_opt);

    return 0;
}

