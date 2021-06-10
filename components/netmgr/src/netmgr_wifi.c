/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include "netmgr.h"
#include "netmgr_wifi.h"
#include "netmgr_conn.h"
#include "string_convert.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "uservice/uservice.h"
#include "uservice/eventid.h"
#ifdef WITH_LWIP
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/dns.h"
#include "lwip/netifapi.h"
#endif
#include "sntp/sntp.h"
#include "ulog/ulog.h"
#include "vfsdev/wifi_dev.h"
#if (RHINO_CONFIG_USER_SPACE > 0)
#include "callback.h"
#include "res.h"
#endif /* RHINO_CONFIG_USER_SPACE > 0 */

#ifndef SNTP_RCV_TIMEOUT
#define SNTP_RCV_TIMEOUT                5000
#endif

#ifndef SNTP_PERSERV_RETRY_TIMES
#define SNTP_PERSERV_RETRY_TIMES         2
#endif

#define SNTP_RETRY_ROUND                 30
#define SNTP_RETRY_ROUND_INTERVAL_MS     400

#ifndef SNTP_PERIODIC_SYNC_INTERVAL_MS
#define SNTP_PERIODIC_SYNC_INTERVAL_MS   (24*3600*1000)
#endif

#define NETMGR_WIFI_CONF    "/data/wifi.conf"
#define NETMGR_WIFI_TEMP_PATH    "/data/temp"
#define NETMGR_WIFI_TEMP_CONF    "/data/temp/temp.conf"
#define NETMGR_WIFI_STATUS  "/data/wifi_status"

#define CONFIG_ELEMENT_NUM      6
#define CONFIG_ELEMENT_SSID     "ssid"
#define CONFIG_ELEMENT_PWD      "password"
#define CONFIG_ELEMENT_BSSID    "bssid"
#define CONFIG_ELEMENT_AP_POWER "ap_power"
#define CONFIG_ELEMENT_FORMAT   "format"
#define CONFIG_ELEMENT_CHANNEL  "channel"

#define TAG "WIFI_SERVICE"
#define NETMGR_WIFI_LOGE(level, ...)      printf(level, ##__VA_ARGS__)
#define NETMGR_WIFI_LOGI(level, ...)      printf(level, ##__VA_ARGS__)
#define NETMGR_WIFI_LOGD(level, ...)      printf(level, ##__VA_ARGS__)

#define WPA_STATE_COMPLETED       "COMPLETED"
#define WPA_STATE_DISCONNECTED    "DISCONNECTED"
#define WPA_STATE_SCANNING        "SCANNING"
#define WPA_STATE_AUTHENTICATION  "AUTHENTICATION"
#define WPA_STATE_ASSOCIATING     "ASSOCIATING"
#define WPA_STATE_ASSOCIATED      "ASSOCIATED"
#define WPA_STATE_4WAY_HANDSHAKE  "4WAY_HANDSHAKE"
#define WPA_STATE_GROUP_HANDSHAKE "GROUP_HANDSHAKE"

#define NETMGR_WIFI_QUEUE_MAX_NUM 50

#define NETMGR_WIFI_DEFAULT_CONNECT_TIMEOUT 28000

#define NETMGR_WIFI_RETRY_MAX_NUM    8

#define NETMGR_WIFI_SNTP_PRI        32
#define NETMGR_WIFI_RECONNECT_PRI   32
#define NETMGR_WIFI_LINK_CHECK_PRI  32
#define NETMGR_WIFI_RECVMSG_PRI     33

#define NETMGR_WIFI_HANDSHAKE_FAILED_MAX_RETRY   8

#define NETMGR_WIFI_MAX_SPECIFIED_SCAN_NUM       4

#ifdef AOS_COMP_ACTIVATION
#define NETMGR_WIFI_REPORT_PRI   25
#endif /* AOS_COMP_ACTIVATION */
#define SCAN_RESULT_WAIT_TIMEOUT (8*1000)
#define DISCONNECT_WAIT_TIMEOUT  (5*1000)
static int retry_backoff_time[NETMGR_WIFI_RETRY_MAX_NUM]= {1, 2, 1, 2, 1, 2, 1, 2};
const char* wificonfigsymbol[CONFIG_ELEMENT_NUM] = {
        CONFIG_ELEMENT_SSID,
        CONFIG_ELEMENT_PWD,
        CONFIG_ELEMENT_BSSID,
        CONFIG_ELEMENT_AP_POWER,
        CONFIG_ELEMENT_FORMAT,
        CONFIG_ELEMENT_CHANNEL,
};

static char *m_sntp_servaddr[SNTP_MAX_SERVERS] = {
        "cn.pool.ntp.org",
        "0.cn.pool.ntp.org",
        "1.cn.pool.ntp.org",
};

typedef struct netmgr_wifi_msg_cb_node {
    struct netmgr_wifi_msg_cb_node* next;
    int  (*msg_cb)(void* data);                        /*  msg callback */
#if (RHINO_CONFIG_USER_SPACE > 0)
    task_group_t* group;
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
} netmgr_wifi_msg_cb_node_t;

typedef struct netmgr_wifi_msg
{
    netmgr_hdl_t hdl;
    int event_id;
    void* params;
} netmgr_wifi_msg_t;

typedef enum NETMGR_WIFI_CONNECT_STATE
{
    NETMGR_WIFI_SCAN_START,
    NETMGR_WIFI_SCAN_SUCCESS,
    NETMGR_WIFI_SCAN_FAILED,
    NETMGR_WIFI_CONNECT_START,
    NETMGR_WIFI_CONNECT_SUCCESS,
    NETMGR_WIFI_CONNECT_FAILED
} NETMGR_WIFI_CONNECT_STATE;

typedef enum WIFI_CONFIG_READ_MODE {
    READ_ALL_CONFIG,
    READ_SPECIFIC_CONFIG
} WIFI_CONFIG_READ_MODE;

static netmgr_conn_t* g_wifi_conn_info = NULL;
static NETMGR_WIFI_CONNECT_STATE g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_FAILED;
static bool g_is_recvmsg_result_got = false;
static bool g_is_dhcp_got_ip = false;
static bool g_is_sntp_task_running = false;
static bool g_is_sntp_enabled = true;
// static bool g_is_link_check_task_running = false;
static aos_queue_t      g_msg_queue;
static bool g_is_got_scan_request_cmd = false;
static wifi_scan_result_t  g_scan_result = {0};
static aos_sem_t   g_scan_result_sem;
static aos_sem_t   g_connect_scan_sem;
static aos_sem_t   g_disconnect_wait_sem;

static bool g_is_auto_save_ap_config = false;
static int g_handshake_failed_retry = 0;

static int update_wifi_config(netmgr_conn_t* conn, const char *name);
static int get_wifi_config(const char *name, netmgr_wifi_ap_config_t* saved_ap_conf, WIFI_CONFIG_READ_MODE mode);

static void wifi_indicate_event_handle(void* hdl, int event, void* arg);
static void handle_wifi_event(netmgr_conn_t* conn, int event);
static void periodic_sntp_handle();
#ifdef AOS_COMP_ACTIVATION
static void wifi_activation_report_task(void);
#endif /* AOS_COMP_ACTIVATION */
#if (RHINO_CONFIG_USER_SPACE > 0)
static int  wifi_msg_cb(task_group_t* group, netmgr_msg_cb_t cb, netmgr_msg_t* msg);
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
static void wifi_status_change(netmgr_conn_t *conn, int event);

static int ssid_contain_chinese_check(const char *ssid);

static int wifi_queue_init()
{
    uint32_t size = sizeof(netmgr_wifi_msg_t) * NETMGR_WIFI_QUEUE_MAX_NUM;
    netmgr_wifi_msg_t * msg_buf = NULL;

    msg_buf = (netmgr_wifi_msg_t *)aos_malloc(size);
    if (!msg_buf) {
        NETMGR_WIFI_LOGE("uart send buf allocate %lu fail!\n", size);
        goto err;
    }

    if (aos_queue_new(&g_msg_queue, msg_buf, size,
           sizeof(netmgr_wifi_msg_t)) != 0) {
        NETMGR_WIFI_LOGE("uart send queue create fail!\n");
        goto err;
    }
    return 0;
err:
    aos_free(msg_buf);
    aos_queue_free(&g_msg_queue);
    return -1;
}

static int wifi_send_queue_deinit()
{
    netmgr_wifi_msg_t *msg_buf = NULL;

    if (!aos_queue_is_valid(&g_msg_queue)) {
        return -1;
    }

    msg_buf = (netmgr_wifi_msg_t *)aos_queue_buf_ptr(&g_msg_queue);
    aos_free(msg_buf);

    aos_queue_free(&g_msg_queue);

    return 0;
}

static void wifi_recvmsg_handle(void* arg)
{
    int              ret;
    unsigned int     size;
    netmgr_wifi_msg_t msg;

    (void) arg;
    NETMGR_WIFI_LOGI("%s:%d task start\n", __func__, __LINE__);

    while (true) {

        if(g_is_recvmsg_result_got == false) {
            NETMGR_WIFI_LOGE("%s:%d recvmsg task exit\n", __func__, __LINE__);
            goto exit;
        }

        if (!aos_queue_is_valid(&g_msg_queue)) {
            NETMGR_WIFI_LOGE("%s:%d Error msg queue invalid!\n", __func__, __LINE__);
            goto exit;
         }

         memset(&msg, 0, sizeof(netmgr_wifi_msg_t));
         ret = aos_queue_recv(&g_msg_queue, AOS_WAIT_FOREVER, &msg, &size);
         if (ret != 0) {
            NETMGR_WIFI_LOGE("%s:%d Error msg queue recv, error_no %d\n", __func__, __LINE__, ret);
            goto exit;

         }
         if (size != sizeof(netmgr_wifi_msg_t)) {
            NETMGR_WIFI_LOGE("%s:%d Error msg queue recv: msg size %lu is not valid\n", __func__, __LINE__, size);
            goto exit;
         }

         wifi_indicate_event_handle((void *)msg.hdl, msg.event_id, msg.params);
    }
exit:
    NETMGR_WIFI_LOGI("%s:%d recvmsg task exits!\n", __func__, __LINE__);
    aos_task_exit(0);
}

static int wifi_recvmsg_task()
{
    aos_task_t  recvmsg_task;

    if(g_is_recvmsg_result_got == false) {
        g_is_recvmsg_result_got = true;
        aos_task_new_ext(&recvmsg_task, "wifi_recvmsg_task", (task_entry_t)wifi_recvmsg_handle, NULL, 4*1024, NETMGR_WIFI_RECVMSG_PRI);
        //krhino_sched_policy_set(recvmsg_task.hdl, KSCHED_RR, NETMGR_WIFI_RECVMSG_PRI);
        return 0;
    }
    else {
        NETMGR_WIFI_LOGE("wifi recv msg task is already runing\n");
        return -1;
    }
}

static void wifi_connect_failed_cb(uint32_t event_id, const void *param, void *context)
{
    NETMGR_WIFI_LOGE("%s:%d err=%d\n", __func__, __LINE__, event_id);
    if(event_id == RET_WIFI_INVALID_PASSWORD) {
        g_handshake_failed_retry = NETMGR_WIFI_HANDSHAKE_FAILED_MAX_RETRY;
        //netmgr_stat_chg_event(m, NOTIFY_WIFI_HANDSHAKE_FAILED, param);
    } else if(event_id == RET_WIFI_CONN_FAIL) {
        g_netmgr_wifi_connect_state = NETMGR_WIFI_CONNECT_FAILED;
    } else if(event_id != RET_WIFI_OK) {
        //netmgr_stat_chg_cb(m, EVENT_WIFI_DISCONNECTED, param);
    }
}

static void wifi_stat_chg_cb(uint32_t event_id, const void *params, void *context)
{
    netmgr_wifi_msg_t msg;
    int error;

    msg.hdl = 0;
    msg.event_id = event_id;
    msg.params = (void *)params;

    if ((error = aos_queue_send(&g_msg_queue, &msg,
                         sizeof(msg))) != 0) {
        NETMGR_WIFI_LOGE("%s:%d aos queue send failed error=%d\n", __func__, __LINE__, error);
    }
    NETMGR_WIFI_LOGI("%s:%d event_id %d\n", __func__, __LINE__, event_id);
}

static void wifi_scan_completed_cb(uint32_t event_id, const void *param, void *context)
{
    netmgr_wifi_ap_config_t* saved_ap_conf;
    netmgr_conn_t* conn = g_wifi_conn_info;
    int i;
    int j;
    wifi_scan_result_t* result = (wifi_scan_result_t *)param;

    if(result == NULL){
        return ;
    }

    if(event_id != EVENT_WIFI_SCAN_DONE) {
        return ;
    }

    if(g_is_got_scan_request_cmd == true) {
        if((result->ap_num != 0) && (result->ap_list != NULL)) {
            g_scan_result.ap_list = (ap_list_t *)malloc(result->ap_num * sizeof(ap_list_t));
            if(g_scan_result.ap_list != NULL) {
                g_scan_result.ap_num = result->ap_num;
                memcpy(g_scan_result.ap_list, result->ap_list, result->ap_num *sizeof(ap_list_t));
                NETMGR_WIFI_LOGI("%s:%d result->ap_num=%d\n", __func__, __LINE__, result->ap_num);
                for(i = 0; i < result->ap_num; i++) {
                    NETMGR_WIFI_LOGI("%s:%d ssid=%s power=%d bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, __LINE__,
                        result->ap_list[i].ssid,
                        result->ap_list[i].ap_power,
                        result->ap_list[i].bssid[0] & 0xff,
                        result->ap_list[i].bssid[1] & 0xff,
                        result->ap_list[i].bssid[2] & 0xff,
                        result->ap_list[i].bssid[3] & 0xff,
                        result->ap_list[i].bssid[4] & 0xff,
                        result->ap_list[i].bssid[5] & 0xff
                        );
                }
            } else {
                g_scan_result.ap_num = 0;
            }
        }
        else {
            g_scan_result.ap_num = 0;
            NETMGR_WIFI_LOGE("%s:%d scan failed\n", __func__, __LINE__);
        }

        g_is_got_scan_request_cmd = false;
        if(aos_sem_is_valid(&g_scan_result_sem) == 1) {
            NETMGR_WIFI_LOGI("%s:%d sem signal\n", __func__, __LINE__);
        } else {
            NETMGR_WIFI_LOGE("%s:%d scan result sem invalid\n", __func__, __LINE__);
        }
        aos_sem_signal_all(&g_scan_result_sem);
        return ;
    }

    if(conn == NULL || result == NULL || result->ap_num <= 0 || result->ap_list == NULL) {
        NETMGR_WIFI_LOGE("%s:%d scan result ap_num is 0 or ap list is null\n", __func__, __LINE__);
        g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_FAILED;
        aos_sem_signal_all(&g_connect_scan_sem);
        return;
    }

    saved_ap_conf = (netmgr_wifi_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_FAILED;
        aos_sem_signal_all(&g_connect_scan_sem);
        return ;
    }

    NETMGR_WIFI_LOGD("%s:%d ap_num=%d\n", __func__, __LINE__, result->ap_num);
    /* Found the most power ap info */
    for(i = 0; i < result->ap_num; i++) {
       for(j = 0; j < saved_ap_conf->ap_num; j++) {
            NETMGR_WIFI_LOGI("%s:%d config ssid=%s result ssid=%s power=%d bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, __LINE__,
                    saved_ap_conf->config[j].ssid,
                    result->ap_list[i].ssid,
                    result->ap_list[i].ap_power,
                    result->ap_list[i].bssid[0] & 0xff,
                    result->ap_list[i].bssid[1] & 0xff,
                    result->ap_list[i].bssid[2] & 0xff,
                    result->ap_list[i].bssid[3] & 0xff,
                    result->ap_list[i].bssid[4] & 0xff,
                    result->ap_list[i].bssid[5] & 0xff
                    );
            if((strcmp(result->ap_list[i].ssid, saved_ap_conf->config[j].ssid) != 0)
               && (strcmp(result->ap_list[i].ssid, saved_ap_conf->config[j].gbk_ssid) != 0)) {
                continue;
            }

            if(((saved_ap_conf->config[j].contain_chinese == true)
               && strcmp(result->ap_list[i].ssid, saved_ap_conf->config[j].gbk_ssid) == 0)) {
                NETMGR_WIFI_LOGI("%s:%d ssid gbk\n", __func__, __LINE__);
                saved_ap_conf->config[j].ssid_format = NETMGR_WIFI_SSID_FORMAT_GBK;
            } else {
                NETMGR_WIFI_LOGI("%s:%d ssid utf8\n", __func__, __LINE__);
                saved_ap_conf->config[j].ssid_format = NETMGR_WIFI_SSID_FORMAT_UTF8;
            }

            if(saved_ap_conf->config[j].ap_power == 0) {
               saved_ap_conf->config[j].ap_power = result->ap_list[i].ap_power;
               saved_ap_conf->config[j].sec_type = result->ap_list[i].sec_type;
               memcpy(saved_ap_conf->config[j].bssid, result->ap_list[i].bssid,
                       sizeof(saved_ap_conf->config[j].bssid));
               NETMGR_WIFI_LOGI("%s:%d found i=%d j=%d power=%d\n", __func__, __LINE__, i, j, saved_ap_conf->config[j].ap_power);
           }
           else {
               if(result->ap_list[i].ap_power > saved_ap_conf->config[j].ap_power) {
                   saved_ap_conf->config[j].ap_power = result->ap_list[i].ap_power;
                   saved_ap_conf->config[j].sec_type = result->ap_list[i].sec_type;
                   memset(saved_ap_conf->config[j].bssid, 0, sizeof(saved_ap_conf->config[j].bssid));
                   memcpy(saved_ap_conf->config[j].bssid, result->ap_list[i].bssid,
                       sizeof(saved_ap_conf->config[j].bssid));
                   NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
               }
           }
       }
    }

    g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_SUCCESS;
    aos_sem_signal_all(&g_connect_scan_sem);
    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
}

static int wifi_set_mac(uint8_t *mac)
{
    int ret;

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(0 != ( ret = ioctl(g_wifi_conn_info->hdl, WIFI_DEV_CMD_SET_MAC, mac))) {
        return -1;
    }
    return 0;
}

static int wifi_get_mac(netmgr_hdl_t hdl, uint8_t *mac)
{
    int ret;

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(0 != (ret = ioctl(hdl, WIFI_DEV_CMD_GET_MAC, mac))) {
        return -1;
    }
    return 0;
}

static int wifi_get_rssi(int *rssi)
{
    wifi_ap_record_t out;
    int ret;

    if(0 != (ret = ioctl(g_wifi_conn_info->hdl, WIFI_DEV_CMD_STA_GET_LINK_STATUS, &out))) {
        return -1;
    }
    memcpy(rssi, &out.rssi, sizeof(out.rssi));
    return 0;
}

static void wifi_sntp_handle(void* arg)
{
    (void) arg;

    g_is_sntp_task_running = true;

    NETMGR_WIFI_LOGI("%s:%d start sntp action\n", __func__, __LINE__);
    periodic_sntp_handle();

    g_is_sntp_task_running = false;

}

static void wifi_sntp_task(void)
{
    aos_task_t  sntp_task;

    if(g_is_sntp_task_running == false) {
        NETMGR_WIFI_LOGI("start sntp task.\n");
        aos_task_new_ext(&sntp_task, "sntp_task", (task_entry_t)wifi_sntp_handle, NULL, 4*1024, NETMGR_WIFI_SNTP_PRI);
    } else {
        NETMGR_WIFI_LOGE("sntp task is already running.\n");
    }
}
#ifdef WITH_LWIP
static void tcpip_dhcpc_cb(struct netif *net_if) {
    struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(net_if, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

    if (netif_is_up(net_if) && (dhcp->state == DHCP_STATE_BOUND) && !ip_addr_isany(&net_if->ip_addr)) {
        int ret;
        netmgr_wifi_ip_stat_t in;
        netmgr_conn_t * conn = g_wifi_conn_info;

        NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
        memset(&in, 0, sizeof(in));
        snprintf(in.ip, sizeof(in.ip), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->ip_addr.addr)[0],
               ((u8_t *)&net_if->ip_addr.addr)[1],
               ((u8_t *)&net_if->ip_addr.addr)[2],
               ((u8_t *)&net_if->ip_addr.addr)[3]);

        snprintf(in.mask, sizeof(in.mask), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->netmask.addr)[0],
               ((u8_t *)&net_if->netmask.addr)[1],
               ((u8_t *)&net_if->netmask.addr)[2],
               ((u8_t *)&net_if->netmask.addr)[3]);

        snprintf(in.gate, sizeof(in.gate), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->gw.addr)[0],
               ((u8_t *)&net_if->gw.addr)[1],
               ((u8_t *)&net_if->gw.addr)[2],
               ((u8_t *)&net_if->gw.addr)[3]);

        if(conn != NULL) {
            if(0 != (ret = ioctl(conn->hdl, WIFI_DEV_CMD_NOTIFY_IP_STATE2DRV , &in))) {
                NETMGR_WIFI_LOGE("%s:%d set ip stat failed\n", __func__, __LINE__);
            }
        }

        NETMGR_WIFI_LOGI("\r\n IPv4 Address     : %hhu.%hhu.%hhu.%hhu\r\n",
               ((u8_t *)&net_if->ip_addr.addr)[0],
               ((u8_t *)&net_if->ip_addr.addr)[1],
               ((u8_t *)&net_if->ip_addr.addr)[2],
               ((u8_t *)&net_if->ip_addr.addr)[3]);
        NETMGR_WIFI_LOGI(" IPv4 Subnet mask : %hhu.%hhu.%hhu.%hhu\r\n",
               ((u8_t *)&net_if->netmask.addr)[0],
               ((u8_t *)&net_if->netmask.addr)[1],
               ((u8_t *)&net_if->netmask.addr)[2],
               ((u8_t *)&net_if->netmask.addr)[3]);
        NETMGR_WIFI_LOGI(" IPv4 Gateway     : %hhu.%hhu.%hhu.%hhu\r\n\r\n",
               ((u8_t *)&net_if->gw.addr)[0],
               ((u8_t *)&net_if->gw.addr)[1],
               ((u8_t *)&net_if->gw.addr)[2],
               ((u8_t *)&net_if->gw.addr)[3]);
        event_publish(EVENT_DHCP_SUCCESS, NULL);
        g_is_dhcp_got_ip = true;
    }
    NETMGR_WIFI_LOGI("%s: dhcp state =%d", __func__, dhcp->state);
}

static void wifi_dhcp_start(void )
{
    struct netif *net_if = NULL;
    netmgr_conn_t* conn;
    // int event = EVENT_NETMGR_DHCP_START_FAILED;

    conn = g_wifi_conn_info;
    net_if = conn->netif;
    if(!net_if) {
        NETMGR_WIFI_LOGE("get net interface failed\n");
        goto end;
    }
    netifapi_netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    netif_set_status_callback(net_if, tcpip_dhcpc_cb);

    if (netif_is_up(net_if)) {
        NETMGR_WIFI_LOGI("%s:%d start dhcp\n", __func__, __LINE__);
        if(netifapi_dhcp_start(net_if) != 0) {
            NETMGR_WIFI_LOGE("%s:%d start dhcp failed.\n", __func__, __LINE__);
            goto end;
        }
    }
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    return ;
end:
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    //netmgr_stat_chg_event(m, event, NULL);
    return ;
}

static void reconnect_task_handle(void* arg)
{
    netmgr_wifi_ap_config_t* saved_ap_conf;
    netmgr_conn_t* conn = g_wifi_conn_info;

    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d connection is invalid\n", __func__, __LINE__);
        return ;
    }

    saved_ap_conf = (netmgr_wifi_ap_config_t*) conn->saved_config;
    if(saved_ap_conf == NULL) {
        NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return ;
    }

    memset(saved_ap_conf, 0, sizeof(netmgr_wifi_ap_config_t));
    if(-1 == get_wifi_config(NETMGR_WIFI_TEMP_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
        if(-1 == get_wifi_config(NETMGR_WIFI_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
            conn->reconnect_task_running = false;
            NETMGR_WIFI_LOGE("%s:%d no config found\n", __func__, __LINE__);
            if(g_handshake_failed_retry != 0) {
                //netmgr_connect_fail_event(m, RET_WIFI_INVALID_PASSWORD, NULL);
            }
            return ;
        }
    }

    while(1) {

        if(false == conn->auto_reconnect) {
            NETMGR_WIFI_LOGE("Not auto reconnect, break the loop\n");
            conn->reconnect_task_running = false;
            break;
        }

        NETMGR_WIFI_LOGI("%s:%d state=%d\n", __func__, __LINE__, conn->state);
        if(conn->state == CONN_STATE_DISCONNECTED) {
            netmgr_wifi_connect_params_t params;
            memset(&params, 0, sizeof(params));
            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
            //netmgr_stat_chg_event(m, NOTIFY_CONN_RECONNECT, NULL);
            if(0 != netmgr_wifi_connect(0, &params)) {
                if(ioctl(conn->hdl, WIFI_DEV_CMD_CANCEL_CONNECT, NULL) != 0) {
                    NETMGR_WIFI_LOGE("WIFI_DEV_CMD_CANCEL_CONNECT failed\n");
                }
                conn->reconnect_task_running = false;
                netmgr_conn_state_change(conn, CONN_STATE_FAILED);
                break;
            }
        }
        else if(conn->state == CONN_STATE_NETWORK_CONNECTED)
        {
            NETMGR_WIFI_LOGI("%s:%d connected\n", __func__, __LINE__);
            conn->reconnect_task_running = false;
            break;
        }
        sleep(2);
    }
    return ;
}

static int wifi_auto_reconnect_task()
{
    aos_task_t  reconnect_task;
    netmgr_conn_t* conn = g_wifi_conn_info;

    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    if(conn->reconnect_task_running == true) {
        NETMGR_WIFI_LOGE("reconnect task is already running\n");
        return 0;
    }

    conn->reconnect_task_running = true;
    aos_task_new_ext(&reconnect_task, "reconnect_task", (task_entry_t)reconnect_task_handle, NULL, 4*1024, NETMGR_WIFI_RECONNECT_PRI);
    return 0;
}

static void sntp_config_servaddr(void)
{
    int i = 0;
    NETMGR_WIFI_LOGI("sntp config servadd start.");
    for (i = 0; i < SNTP_MAX_SERVERS; i++) {
        if (0 != sntp_set_server(i, m_sntp_servaddr[i])) {
            NETMGR_WIFI_LOGE("set sntp server:%s failed\n", m_sntp_servaddr[i]);
        }
        else {
            NETMGR_WIFI_LOGI("set sntp server:%s successfully\n", m_sntp_servaddr[i]);
        }
    }
    NETMGR_WIFI_LOGI("sntp config servadd end.");
}


static int wifi_connected(netmgr_conn_t* conn)
{
    struct netif* net_if;

    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    net_if = conn->netif;
    if(!netif_is_up(net_if)) {
        NETMGR_WIFI_LOGI("%s:%d up netif\n", __func__, __LINE__);
        if(!netif_is_link_up(net_if)) {
            netifapi_netif_set_link_up(net_if);
        }
        if(!netif_is_up(net_if)) {
            netifapi_netif_set_up(net_if);
            netifapi_netif_set_default(net_if);
        }
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    return netmgr_conn_state_change(conn, CONN_STATE_OBTAINING_IP);
}

static int wifi_failed(netmgr_conn_t* conn)
{
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    NETMGR_WIFI_LOGI("%s:%d auto_reconnect=%d\n", __func__, __LINE__, conn->auto_reconnect);
    if(conn->auto_reconnect == true) {

        netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTING);
        /* connection state change */
        wifi_auto_reconnect_task();
    }
    else {
        netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
    }
    return 0;
}

static int wifi_obtaining_ip(netmgr_conn_t* conn)
{
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    if(conn->use_ip_mode == NETMGR_WIFI_IP_MODE_STATIC) {
        struct netif* net_if = conn->netif;

        if(net_if != NULL) {
            netmgr_wifi_ip_stat_t in;
            const ip_addr_t* dns;
            int ret;

            netifapi_netif_set_addr(net_if, &conn->static_ip, &conn->static_mask, &conn->static_gw);
            dns = dns_getserver(0);
            dns_setserver(0, &conn->static_dns);
            dns_setserver(1, dns);

            event_publish(EVENT_DHCP_SUCCESS, NULL);

            memset(&in, 0, sizeof(in));
            snprintf(in.ip, sizeof(in.ip), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->ip_addr.addr)[0],
               ((u8_t *)&net_if->ip_addr.addr)[1],
               ((u8_t *)&net_if->ip_addr.addr)[2],
               ((u8_t *)&net_if->ip_addr.addr)[3]);

            snprintf(in.mask, sizeof(in.mask), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->netmask.addr)[0],
               ((u8_t *)&net_if->netmask.addr)[1],
               ((u8_t *)&net_if->netmask.addr)[2],
               ((u8_t *)&net_if->netmask.addr)[3]);

            snprintf(in.gate, sizeof(in.gate), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->gw.addr)[0],
               ((u8_t *)&net_if->gw.addr)[1],
               ((u8_t *)&net_if->gw.addr)[2],
               ((u8_t *)&net_if->gw.addr)[3]);
            if(0 != (ret = ioctl(conn->hdl, WIFI_DEV_CMD_NOTIFY_IP_STATE2DRV , &in))) {
                NETMGR_WIFI_LOGE("%s:%d set ip stat failed, ret=%d\n", __func__, __LINE__, ret);
            }
            g_is_dhcp_got_ip = true;
        }
    }
    else {
        NETMGR_WIFI_LOGI("%s:%d start dhcp\n", __func__, __LINE__);
        wifi_dhcp_start();
    }

    return 0;
}

#ifdef AOS_OPTIMIZE_NETWORK
#include "lwip/tcp.h"
/*  0:small rto 1:large rto */
extern int lwip_rto_flags;
/*  0:small rcv wnd 1:large rcv wnd */
extern int lwip_rcv_wnd_flags;

static void wifi_link_check()
{
    int rssi;
    netmgr_conn_t* conn = g_wifi_conn_info;

    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d conn is null\n", __func__, __LINE__);
        return ;
    }

    if(conn->state != CONN_STATE_NETWORK_CONNECTED) {
        NETMGR_WIFI_LOGI("%s:%d conn state %d is not network connected,exit\n", __func__, __LINE__, conn->state);
        return ;
    }

    wifi_get_rssi(&rssi);

    NETMGR_WIFI_LOGI("%s:%d rssi=%d flags=%d\n", __func__, __LINE__, rssi, lwip_rcv_wnd_flags);
    if((rssi < -60) && (lwip_rcv_wnd_flags == WND_FLAGS_LARGE)) {
        NETMGR_WIFI_LOGI("%s:%d rssi=%d\n", __func__, __LINE__, rssi);
        NETMGR_WIFI_LOGI("%s:%d weak\n", __func__, __LINE__);
        lwip_rto_flags = RTO_FLAGS_LARGE;
        lwip_rcv_wnd_flags = WND_FLAGS_SMALL;
    }

    if((rssi > -50) && (lwip_rcv_wnd_flags == WND_FLAGS_SMALL)) {
        NETMGR_WIFI_LOGI("%s:%d rssi=%d\n", __func__, __LINE__, rssi);
        NETMGR_WIFI_LOGI("%s:%d strong\n", __func__, __LINE__);
        lwip_rto_flags = RTO_FLAGS_SMALL;
        lwip_rcv_wnd_flags = WND_FLAGS_LARGE;
    }
}
#endif /* AOS_OPTIMIZE_NETWORK */
#endif /* WITH_LWIP */

static int wifi_network_connected(netmgr_conn_t* conn)
{
#ifdef AOS_COMP_ACTIVATION
    wifi_activation_report_task();
#endif /* AOS_COMP_ACTIVATION */

#ifdef AOS_OPTIMIZE_NETWORK
    if(g_is_link_check_task_running == false) {
        g_is_link_check_task_running = true;
        wifi_link_check();
        g_is_link_check_task_running = false;
    }
#endif /* AOS_OPTIMIZE_NETWORK */
    return 0;
}

static int wifi_disconnecting(netmgr_conn_t* conn)
{
    struct netif* net_if;

    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    net_if = conn->netif;
    if(net_if != NULL) {
        NETMGR_WIFI_LOGI("%s:%d clear ip address\n", __func__, __LINE__);
        netifapi_netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    }

    netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
    return 0;
}

static int wifi_disconnected(netmgr_conn_t* conn)
{
    struct netif* net_if;
    int ret;

    if(g_is_dhcp_got_ip == true) {
        g_is_dhcp_got_ip = false;
    }

    net_if = conn->netif;
    if(net_if != NULL) {
        if(netif_is_link_up(net_if)) {
            netifapi_netif_set_link_down(net_if);
        }
        if(netif_is_up(net_if)) {
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
        }
    }
    if(0 != (ret = ioctl(conn->hdl, WIFI_DEV_CMD_DISCONNECT, NULL))) {
        NETMGR_WIFI_LOGI("%s:%d disconnect failed\n", __func__, __LINE__);
    }
    NETMGR_WIFI_LOGI("%s:%d do disconnect\n", __func__, __LINE__);
    return 0;
}

static int newline_terminated(const char* buf, size_t buflen)
{
    size_t len = strlen(buf);
    if (len == 0) {
        return 0;
    }
    if (len == buflen - 1 && buf[buflen - 1] != '\r' &&
        buf[len - 1] != '\n') {
        return 0;
    }
    return 1;
}

static void skip_line_end(int fd)
{
    char buf[100];
    int  read_len = 0;

    read_len = read(fd, buf, sizeof(buf) - 1);
    while (read_len > 0) {
        buf[read_len] = '\0';
        if (newline_terminated(buf, read_len))
            return;

        read_len = read(fd, buf, sizeof(buf) - 1);
    }
}

static int read_config_value(char** pos, const char* obj,int* line, netmgr_wifi_ap_info_t* config)
{
    char* pos_start = NULL;
    char* pos_end = NULL;
    int len = 0;
    char t_str[18] = {'\0'};

    NETMGR_WIFI_LOGI("%s:%d read ap config to ap info:%p\n", __func__, __LINE__, config);
    if((pos_start=strstr(*pos,obj)) != NULL) {
        pos_start = strchr(pos_start, '"');
        if(NULL == pos_start) {
            NETMGR_WIFI_LOGE("Line %d:Invalid value line '%s'.", *line,pos_start);
            return -1;
        }
        pos_start ++;
        pos_end = strchr(pos_start, '"');
        if(pos_end != NULL) {
            len = (pos_end - pos_start);
            pos_end += 2;
            if (*pos_end != '}') {
                pos_end++;
            }
            *pos = pos_end;
            NETMGR_WIFI_LOGI("%s:%d len :%d %s ", __func__, __LINE__, len, pos_start);
            if(strncmp(obj,"ssid", 4) == 0) {
                int contain_chinese = ssid_contain_chinese_check(config->ssid);
                strncpy(config->ssid, pos_start, len);
                config->ssid[len]='\0';
                NETMGR_WIFI_LOGI("%s:%d ssid=%s\n", __func__, __LINE__, config->ssid);
                if((1 == contain_chinese) && (0 != string_convert((uint8_t *)config->gbk_ssid, MAX_SSID_SIZE, (uint8_t *)config->ssid, strlen(config->ssid), UTF8_TO_GBK))) {
                   config->contain_chinese = true;
                } else {
                   config->contain_chinese = false;
                }
            } else if(strncmp(obj,"password", 8) == 0) {
                memcpy(config->pwd, pos_start, len);
                config->pwd[len]='\0';
                NETMGR_WIFI_LOGI("%s:%d len=%d pwd=%s\n", __func__, __LINE__, len, config->pwd);
            }else if(strncmp(obj,"format", 6) == 0) {
                strncpy(t_str, pos_start, len);
                t_str[len]='\0';
                if(strncmp(t_str, "gbk", 3) == 0) {
                    config->ssid_format = NETMGR_WIFI_SSID_FORMAT_GBK;
                } else {
                    config->ssid_format = NETMGR_WIFI_SSID_FORMAT_UTF8;
                }
                NETMGR_WIFI_LOGI("%s:%d format=%s\n", __func__, __LINE__, t_str);
            } else if(strncmp(obj,"bssid",5) == 0) {
                strncpy((char*)t_str, pos_start, len);
                t_str[len]='\0';
                NETMGR_WIFI_LOGI("%s:%d len=%d pwd=%s pwd_len=%d t_str=%s\n", __func__, __LINE__, len, config->pwd, ETH_ALEN, t_str);
                sscanf((char*)t_str,"%02x:%02x:%02x:%02x:%02x:%02x",
                    &config->bssid[0],
                    &config->bssid[1],
                    &config->bssid[2],
                    &config->bssid[3],
                    &config->bssid[4],
                    &config->bssid[5]);
                NETMGR_WIFI_LOGI("%s:%d pwd=%s t_str=%s\n", __func__, __LINE__, config->pwd, t_str);
            } else if(strncmp(obj,"ap_power",8) == 0) {
                strncpy(t_str,pos_start,len);
                t_str[len]='\0';
                config->ap_power = atoi((char*)t_str);
            } else if(strncmp(obj,"channel",7) == 0) {
                strncpy(t_str,pos_start,len);
                t_str[len]='\0';
                config->channel = atoi((char*)t_str);
            }
        }else {
            NETMGR_WIFI_LOGE("%s:%d Line %d:invalid quotation '%s'.", __func__, __LINE__, *line, pos_start);
            return -1;
        }
    }
    return 0;
}

static char* get_config_line(char* s, int size, int fd, int* line,
    char** _pos)
{
    char *pos, *end, *sstart;
    int  read_len = 0;

    read_len = read(fd, s, size);
    while (read_len > 0) {
        (*line)++;
        s[read_len] = '\0';
        if (!newline_terminated(s, read_len)) {
        /*
         * The line was truncated - skip rest of it to avoid
         * confusing error messages.
         */
            NETMGR_WIFI_LOGI("Long line in configuration file truncated");
            skip_line_end(fd);
         }
         pos = s;

         /* Skip white space from the beginning of line. */
         while (*pos == ' ' || *pos == '\t' || *pos == '\r') {
             pos++;
         }

         /* Skip comment lines and empty lines */
         if (*pos == '#' || *pos == '\n' || *pos == '\0') {
             read_len = read(fd, s, size);
             continue;
         }

         /*
          * Remove # comments unless they are within a double quoted
          * string.
          */
         sstart = strchr(pos, '"');
         if (sstart)
            sstart = strrchr(sstart + 1, '"');
         if (!sstart)
            sstart = pos;
         end = strchr(sstart, '#');
         if (end)
            *end-- = '\0';
         else
            end = pos + strlen(pos) - 1;

         /* Remove trailing white space. */
         while (end > pos &&
             (*end == '\n' || *end == ' ' || *end == '\t' ||
              *end == '\r')) {
             *end-- = '\0';
         }
         if (*pos == '\0') {
             read_len = read(fd, s, size);
             continue;
         }
         if (_pos) {
             *_pos = pos;
         }
         return pos;
    }
    if (_pos) {
        *_pos = NULL;
    }
    return NULL;
}

static int get_wifi_config(const char *name, netmgr_wifi_ap_config_t* saved_ap_conf, WIFI_CONFIG_READ_MODE mode)
{
    int fd;
    int i = 0;
    int j = 0;
    int line = 0;
    char *pos = NULL;
    char buf[256];
    int len;

    NETMGR_WIFI_LOGD("%s:%d saved_ap_conf %p", __func__, __LINE__, saved_ap_conf);
    if(saved_ap_conf == NULL) {
        NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return -1;
    }

    if((saved_ap_conf->ap_num > MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->ap_num < 0)) {
        NETMGR_WIFI_LOGE("%s:%d error used ap idx: %d ap_num: %d max config num: %d\n", __func__, __LINE__, saved_ap_conf->used_ap, saved_ap_conf->ap_num,  MAX_AP_CONFIG_NUM);
        return -1;
    }

    fd = open(name, O_RDONLY);
    if(fd < 0) {
        NETMGR_WIFI_LOGE("%s:%d open %s failed:%s\n", __func__, __LINE__, name, strerror(errno));
        return -1;
    }

    i = saved_ap_conf->ap_num;

    if (get_config_line(buf, sizeof(buf) - 1, fd, &line, &pos)) {
        netmgr_wifi_ap_info_t ap_info;

        memset(&ap_info, 0, sizeof(netmgr_wifi_ap_info_t));
        if(strstr(pos, "network={") != NULL) {

            pos += (strlen("network={") + 2);
            while (pos) {
                if(strncmp(pos,"}", 1) == 0) {
                    break;
                }
                for(j = 0; j < CONFIG_ELEMENT_NUM; j++) {
                    len = strlen(wificonfigsymbol[j]);
                    if(strncmp(pos, wificonfigsymbol[j], len) != 0 ||
                        pos[len] != '='){
                        continue;
                    }

                    if(read_config_value(&pos, wificonfigsymbol[j], &line,
                       &ap_info)) {
                        NETMGR_WIFI_LOGI("%s:%d\n failed to line:%d parse: %s", __func__, __LINE__, line, pos);
                        continue;
                    }
                }
            }
            NETMGR_WIFI_LOGD("%s:%d ssid=%s\n", __func__, __LINE__, ap_info.ssid);

            if(saved_ap_conf->ap_num != 0) {
                int k;
                for(k = 0; k < saved_ap_conf->ap_num; k++) {
                    NETMGR_WIFI_LOGD("%s:%d k=%d ssid=%s ap info ssid=%s\n", __func__, __LINE__, k, saved_ap_conf->config[k].ssid, ap_info.ssid);
                    if(0 == strcmp(saved_ap_conf->config[k].ssid, ap_info.ssid)) {
                        if(saved_ap_conf->config[k].channel == 0) {
                            NETMGR_WIFI_LOGI("%s:%d update config[%d] with cfg channel=%d\n", __func__, __LINE__, k, ap_info.channel);
                            saved_ap_conf->config[k].channel = ap_info.channel;
                        }
                        break;
                    }
                }

                if((mode == READ_ALL_CONFIG) && (k == saved_ap_conf->ap_num)){
                    if(i < MAX_AP_CONFIG_NUM) {
                        memcpy(&saved_ap_conf->config[i], &ap_info, sizeof(netmgr_wifi_ap_info_t));
                        i++;
                        NETMGR_WIFI_LOGI("%s:%d increase ap_num=%d\n", __func__, __LINE__, i);
                    }
                    else {
                        NETMGR_WIFI_LOGI("%s:%d ap num reached max %d\n", __func__, __LINE__, i);
                    }
                }
                else {
                    NETMGR_WIFI_LOGD("%s:%d k=%d ap_num=%d\n", __func__, __LINE__, k, saved_ap_conf->ap_num);
                }
            }
            else {
                if(i < MAX_AP_CONFIG_NUM) {
                    memcpy(&saved_ap_conf->config[i], &ap_info, sizeof(netmgr_wifi_ap_info_t));
                    i++;
                    NETMGR_WIFI_LOGI("%s:%d increase ap_num=%d\n", __func__, __LINE__, i);
                }
                else {
                    NETMGR_WIFI_LOGI("%s:%d ap num reached max %d\n", __func__, __LINE__, i);
                }
            }

            NETMGR_WIFI_LOGD("%s:%d index:%d\n", __func__, __LINE__, i);
            if(i >= MAX_AP_CONFIG_NUM) {
                NETMGR_WIFI_LOGI("%s:%d ap num reached max %d\n", __func__, __LINE__,  i);
            }
        }
    }

    saved_ap_conf->ap_num = i;
    NETMGR_WIFI_LOGI("%s:%d ap_num=%d\n", __func__, __LINE__, saved_ap_conf->ap_num);
    close(fd);
    return 0;
}

#ifdef AOS_COMP_ACTIVATION
#include "activation.h"
static void wifi_activation_report_handle(void* arg)
{
    (void) arg;

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    activation_report();

}

static void wifi_activation_report_task()
{
    NETMGR_WIFI_LOGI("start report device info.");
    aos_task_new("activation_report_task", (task_entry_t)wifi_activation_report_handle, NULL, 4*1024);
}
#endif /* AOS_COMP_ACTIVATION */

#define NETMGR_WIFI_CONFIG_LEN 512
static int update_wifi_config(netmgr_conn_t* conn, const char *name)
{
    int i = 0;
    char buf[NETMGR_WIFI_CONFIG_LEN];
    int len = 0;
    int channel = 0;
    char *ssid = NULL;
    char *format = NULL;
    int fd;
    int ret;
    netmgr_wifi_ap_config_t* saved_ap_conf;

    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    if(0 != (ret = (ioctl(conn->hdl, WIFI_DEV_CMD_GET_CHANNEL, &channel)))) {
       NETMGR_WIFI_LOGE("%s:%d get channel failed\n", __func__, __LINE__);
        return -1;
    }

    saved_ap_conf = (netmgr_wifi_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return -1;
    }

    /* check gbk and utf8 when ssid contain chinese */
    if((saved_ap_conf->used_ap >= MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->used_ap < 0)
        ||(saved_ap_conf->ap_num > MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->ap_num <= 0)) {
        NETMGR_WIFI_LOGE("%s:%d error used ap idx: %d ap_num: %d max config num: %d\n", __func__, __LINE__, saved_ap_conf->used_ap, saved_ap_conf->ap_num,  MAX_AP_CONFIG_NUM);
        return -1;
    }

    i = saved_ap_conf->used_ap;
    NETMGR_WIFI_LOGI("%s:%d i=%d ssid=%s ssid_format=%d\n", __func__, __LINE__, i,
    saved_ap_conf->config[i].ssid, saved_ap_conf->config[i].ssid_format);

    if(i != 0) {
        /* move used ap info to the head of the array */
        memset(&saved_ap_conf->config[0], 0, sizeof(saved_ap_conf->config[0]));
        memcpy(&saved_ap_conf->config[0], &saved_ap_conf->config[i], sizeof(saved_ap_conf->config[0]));
    }

    saved_ap_conf->used_ap = 0;
    i = saved_ap_conf->used_ap;

    saved_ap_conf->config[i].channel = channel;

#if (MAX_AP_CONFIG_NUM > 1)
    memset(&saved_ap_conf->config[1], 0, sizeof(saved_ap_conf->config[1]) * (saved_ap_conf->ap_num - 1));
#endif
    saved_ap_conf->ap_num = 1;

    if(-1 == get_wifi_config(name, saved_ap_conf, READ_ALL_CONFIG)) {
        if(-1 == get_wifi_config(NETMGR_WIFI_TEMP_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
            NETMGR_WIFI_LOGE("%s:%d no temp config found\n", __func__, __LINE__);
        } else {
            NETMGR_WIFI_LOGE("%s:%d no config found\n", __func__, __LINE__);
        }
    }

    if((0 != mkdir(NETMGR_WIFI_TEMP_PATH, 0644)) && (errno != EEXIST && errno != 0)) {
        NETMGR_WIFI_LOGE("mkdir %s failed:%s\n", NETMGR_WIFI_TEMP_PATH, strerror(errno));
        return -1;
    }

    fd = open(NETMGR_WIFI_TEMP_CONF, O_RDWR | O_CREAT);
    if(fd < 0) {
        NETMGR_WIFI_LOGE("open %s failed:%s\n", NETMGR_WIFI_TEMP_CONF, strerror(errno));
        return -1;
    }

    NETMGR_WIFI_LOGI("%s:%d ap_num=%d\n", __func__, __LINE__, saved_ap_conf->ap_num);
    for(i= 0; i < saved_ap_conf->ap_num; i++) {
        ssid = saved_ap_conf->config[i].ssid;
        if(NETMGR_SSID_FORMAT_UTF8 == saved_ap_conf->config[i].ssid_format) {
            format = "utf8";
        } else {
            format = "gbk";
        }

        snprintf(buf,NETMGR_WIFI_CONFIG_LEN,"network={\n \
ssid=\"%s\"\n \
password=\"%s\"\n \
bssid=\"%02x:%02x:%02x:%02x:%02x:%02x\"\n \
ap_power=\"%d\"\n \
format=\"%s\"\n \
channel=\"%d\"\n}\n\n",
              ssid,
              saved_ap_conf->config[i].pwd,
              saved_ap_conf->config[i].bssid[0] & 0xff,
              saved_ap_conf->config[i].bssid[1] & 0xff,
              saved_ap_conf->config[i].bssid[2] & 0xff,
              saved_ap_conf->config[i].bssid[3] & 0xff,
              saved_ap_conf->config[i].bssid[4] & 0xff,
              saved_ap_conf->config[i].bssid[5] & 0xff,
              saved_ap_conf->config[i].ap_power,
              format,
              saved_ap_conf->config[i].channel);

        len = strlen(buf);
        NETMGR_WIFI_LOGI("saved_ap_conf=%p wifi config buff len :%d\n",saved_ap_conf, len);
        NETMGR_WIFI_LOGI("saving wifi config file:%s\n",buf);
        if(0 > write(fd, buf,len)) {
            NETMGR_WIFI_LOGE("write:%s\n",strerror(errno));
        }
    }
    close(fd);

    if(rename(NETMGR_WIFI_TEMP_CONF, NETMGR_WIFI_CONF) != 0) {
        NETMGR_WIFI_LOGE("rename:%s\n",strerror(errno));
        return -1;
    }

    return 0;
}

/* Support 2G bands only */
static int channel_to_frequency(int chan)
{
    /*  see 802.11 17.3.8.3.2 and Annex
     *  there are overlapping channel numbers
     *  in 5GHz and 2GHz bands */
#if 0
    if (band == IEEE80211_BAND_5GHZ) {
        if (chan >= 182 && chan <= 196) {
            return 4000 + chan * 5;
        }
        else {
            return 5000 + chan * 5;
        }
    } else
#endif
    { /*  IEEE80211_BAND_2GHZ */
        if (chan == 14) {
            return 2484;
        }
        else if (chan > 0 && chan < 14) {
            return 2407 + chan * 5;
        }
    }

    return 0;
}

static int update_wifi_status(netmgr_conn_t* conn, const char *name, char* wpa_status)
{
    int i = 0;
    char buf[NETMGR_WIFI_CONFIG_LEN];
    int len = 0;
    int channel = 0;
    int freq = 0;
    int fd;
    netmgr_wifi_ap_config_t* saved_ap_conf;
    struct netif* net_if;
    uint8_t mac[6];
    char *ssid = NULL;
    int ret;

    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }
    if(0 != (ret = (ioctl(conn->hdl, WIFI_DEV_CMD_GET_CHANNEL, &channel)))) {
        NETMGR_WIFI_LOGE("%s:%d get channel failed\n", __func__, __LINE__);
        return -1;
    }

    unlink(name);

    fd = open(name, O_RDWR | O_CREAT);
    if(fd < 0) {
        NETMGR_WIFI_LOGE("open %s failed:%s\n", name, strerror(errno));
        return -1;
    }


    freq = channel_to_frequency(channel);

    wifi_get_mac(conn->hdl, mac);

    saved_ap_conf = (netmgr_wifi_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        close(fd);
        NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return -1;
    }

    if((saved_ap_conf->used_ap >= MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->used_ap < 0)) {
        close(fd);
        NETMGR_WIFI_LOGE("%s:%d invalid used ap %d\n", __func__, __LINE__, saved_ap_conf->used_ap);
        return -1;
    }

    i = saved_ap_conf->used_ap;
    if(i == -1) {
        close(fd);
        NETMGR_WIFI_LOGE("%s:%d invalid used_ap=%d\n", __func__, __LINE__, i);
        return -1;
    }
    NETMGR_WIFI_LOGI("%s:%d used_ap=%d\n", __func__, __LINE__, i);
    ssid = saved_ap_conf->config[i].ssid;

    net_if = conn->netif;
    if((!net_if) || (!netif_is_up(net_if))) {
        NETMGR_WIFI_LOGI("%s:%d no ip config use 0.0.0.0\n", __func__, __LINE__);
        snprintf(buf,NETMGR_WIFI_CONFIG_LEN,"\
bssid=%02x:%02x:%02x:%02x:%02x:%02x\n\
freq=%d\n\
ssid=%s\n\
ip_address=%hu.%hu.%hu.%hu\n\
address=%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx\n\
wpa_state=%s\n",
              saved_ap_conf->config[i].bssid[0] & 0xff,
              saved_ap_conf->config[i].bssid[1] & 0xff,
              saved_ap_conf->config[i].bssid[2] & 0xff,
              saved_ap_conf->config[i].bssid[3] & 0xff,
              saved_ap_conf->config[i].bssid[4] & 0xff,
              saved_ap_conf->config[i].bssid[5] & 0xff,
              freq,
              ssid,
              0,
              0,
              0,
              0,
              mac[0] & 0xff,
              mac[1] & 0xff,
              mac[2] & 0xff,
              mac[3] & 0xff,
              mac[4] & 0xff,
              mac[5] & 0xff,
              wpa_status
              );
    }
    else {
        snprintf(buf,NETMGR_WIFI_CONFIG_LEN,"\
bssid=%02x:%02x:%02x:%02x:%02x:%02x\n\
freq=%d\n\
ssid=%s\n\
ip_address=%hu.%hu.%hu.%hu\n\
address=%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx\n\
wpa_state=%s\n",
              saved_ap_conf->config[i].bssid[0] & 0xff,
              saved_ap_conf->config[i].bssid[1] & 0xff,
              saved_ap_conf->config[i].bssid[2] & 0xff,
              saved_ap_conf->config[i].bssid[3] & 0xff,
              saved_ap_conf->config[i].bssid[4] & 0xff,
              saved_ap_conf->config[i].bssid[5] & 0xff,
              freq,
              ssid,
              ((u8_t *)&net_if->ip_addr.addr)[0],
              ((u8_t *)&net_if->ip_addr.addr)[1],
              ((u8_t *)&net_if->ip_addr.addr)[2],
              ((u8_t *)&net_if->ip_addr.addr)[3],
              mac[0] & 0xff,
              mac[1] & 0xff,
              mac[2] & 0xff,
              mac[3] & 0xff,
              mac[4] & 0xff,
              mac[5] & 0xff,
              wpa_status
              );
    }
    len = strlen(buf);
    NETMGR_WIFI_LOGI("saved_ap_conf=%p wifi config buff len :%d\n",saved_ap_conf, len);
    NETMGR_WIFI_LOGI("saving file:%s\n",buf);
    if(0 > write(fd, buf,len)) {
        NETMGR_WIFI_LOGE("write:%s\n",strerror(errno));
    }

    close(fd);
    return 0;

}

static void wifi_status_change(netmgr_conn_t *conn, int event)
{
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }
    NETMGR_WIFI_LOGI("%s:%d event=%d conn->state=%d\n", __func__, __LINE__, event, conn->state);

    if(event == EVENT_NETMGR_WIFI_SCAN_STARTED) {
        //update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_SCANNING);
    }
    else if(event == EVENT_NETMGR_WIFI_AUTHENTICATING) {
        //update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_AUTHENTICATION);
    }
    else if(event == EVENT_NETMGR_WIFI_ASSOCIATING) {
        //update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_ASSOCIATING);
    }
    else if(event == EVENT_NETMGR_WIFI_ASSOCIATED) {
        //update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_ASSOCIATED);
    }
    else if(event == EVENT_NETMGR_WIFI_4WAY_HANDSHAKE) {
        //update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_4WAY_HANDSHAKE);
    }
    else if(event == EVENT_NETMGR_WIFI_4WAY_HANDSHAKE_DONE) {
        g_handshake_failed_retry = 0;
    }
    else if(event == EVENT_NETMGR_WIFI_GROUP_HANDSHAKE) {
        //update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_GROUP_HANDSHAKE);
    }
    else if(event == EVENT_NETMGR_WIFI_GROUP_HANDSHAKE_DONE) {
    }
    else if(event == EVENT_NETMGR_WIFI_CONNECTED) {
        NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
        g_netmgr_wifi_connect_state = NETMGR_WIFI_CONNECT_SUCCESS;
        netmgr_conn_state_change(conn, CONN_STATE_CONNECTED);
        if(g_is_auto_save_ap_config == true) {
            update_wifi_config(conn, NETMGR_WIFI_CONF);
        }
    }
    else if(event == EVENT_NETMGR_DHCP_SUCCESS) {
        if(g_is_sntp_enabled == true) {
            wifi_sntp_task();
        }
        update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_COMPLETED);
        netmgr_conn_state_change(conn, CONN_STATE_NETWORK_CONNECTED);

//        extern void start_net_deamon(void);
//        start_net_deamon();

        NETMGR_WIFI_LOGD("%s:%d network connected\n", __func__, __LINE__);
    }
    else if(event == EVENT_NETMGR_SNTP_SUCCESS) {
        NETMGR_WIFI_LOGD("%s:%d SNTP success\n", __func__, __LINE__);
    }
    else if(event == EVENT_NETMGR_SNTP_FAILED) {
        NETMGR_WIFI_LOGI("%s:%d SNTP FAILED\n", __func__, __LINE__);
    }
    else if(event == EVENT_NETMGR_WIFI_HANDSHAKE_FAILED) {
        g_handshake_failed_retry ++;
        if(g_handshake_failed_retry >= NETMGR_WIFI_HANDSHAKE_FAILED_MAX_RETRY) {
            NETMGR_WIFI_LOGI("%s:%d handshake failed %d times\n", __func__, __LINE__, g_handshake_failed_retry);
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        } else {
            NETMGR_WIFI_LOGI("%s:%d handshake failed %d times\n", __func__, __LINE__, g_handshake_failed_retry);
            g_netmgr_wifi_connect_state = NETMGR_WIFI_CONNECT_FAILED;
        }
    }
    else {
        if(event == EVENT_NETMGR_WIFI_DISCONNECTED) {
            if(1 == aos_sem_is_valid(&g_disconnect_wait_sem)) {
                NETMGR_WIFI_LOGI("%s:%d signal disconnected event\n", __func__, __LINE__);
                aos_sem_signal_all(&g_disconnect_wait_sem);
            }

            if(conn->state != CONN_STATE_CONNECTING) {
                NETMGR_WIFI_LOGI("%s:%d update wifi status\n", __func__, __LINE__);
                update_wifi_status(conn, NETMGR_WIFI_STATUS, WPA_STATE_DISCONNECTED);
            }
        }

        NETMGR_WIFI_LOGD("%s:%d event=%d stat=%d\n", __func__, __LINE__, event, conn->state);
        if(conn->state == CONN_STATE_CONNECTING) {
            if((event == EVENT_NETMGR_WIFI_DISCONNECTED)
                && (g_netmgr_wifi_connect_state == NETMGR_WIFI_SCAN_START)) {
                NETMGR_WIFI_LOGI("%s:%d Ingore DISCONNECTED event in scan start state\n", __func__, __LINE__);
            } else if((event == EVENT_NETMGR_WIFI_SCAN_FAILED)
                || (event == EVENT_NETMGR_WIFI_NETWORK_NOT_FOUND)) {
                g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_FAILED;
            } else if(event == EVENT_NETMGR_WIFI_CONN_TIMEOUT) {
                NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
                netmgr_conn_state_change(conn, CONN_STATE_FAILED);
            } else {
                g_netmgr_wifi_connect_state = NETMGR_WIFI_CONNECT_FAILED;
            }
        } else if((conn->state != CONN_STATE_DISCONNECTING)
            && (conn->state != CONN_STATE_DISCONNECTED)) {
            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        }
    }

    if((event != EVENT_NETMGR_WIFI_HANDSHAKE_FAILED)
        || (g_handshake_failed_retry >= NETMGR_WIFI_HANDSHAKE_FAILED_MAX_RETRY)) {
        NETMGR_WIFI_LOGI("%s:%d wifi service event\n", __func__, __LINE__);
        if(event == EVENT_NETMGR_WIFI_HANDSHAKE_FAILED) {
            g_handshake_failed_retry = 0;
        }
        handle_wifi_event(conn, event);
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
}

static void handle_wifi_event(netmgr_conn_t* conn, int event)
{
    netmgr_msg_t msg;
    int i;

    NETMGR_WIFI_LOGD("%s:%d event=%d\n", __func__, __LINE__, event);
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }

    if(event >= EVENT_NETMGR_WIFI_DISCONNECTED && event < EVENT_NETMGR_MAX) {
        netmgr_wifi_msg_cb_node_t* node = conn->msg_cb_list;
        NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
		event_publish(event, NULL);
        while(node != NULL) {
            memset(&msg, 0, sizeof(msg));
            msg.id = NETMGR_MSGID_WIFI_STATUS_FROM_IMPL;
            msg.data.status = event;
#if (RHINO_CONFIG_USER_SPACE > 0)
            NETMGR_WIFI_LOGI("%s:%d node->msg_cb=%p node->group=%p pid=%d event=%d status=%d\n", __func__, __LINE__, node->msg_cb, node->group, node->group->pid, event, msg.data.status);
            wifi_msg_cb(node->group, (netmgr_msg_cb_t)node->msg_cb, &msg);
#else
            node->msg_cb(&msg);
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
            NETMGR_WIFI_LOGI("%s:%d event=%d\n", __func__, __LINE__, event);

            if((event == EVENT_NETMGR_WIFI_DISCONNECTED) ||
                (event == EVENT_NETMGR_DHCP_SUCCESS)) {
                netmgr_wifi_network_status_change_t* change;
                netmgr_wifi_ap_config_t* saved_ap_conf;
                char *wifi_status;

                NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
                saved_ap_conf = (netmgr_wifi_ap_config_t* )conn->saved_config;
                if(saved_ap_conf == NULL) {
                    NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
                    return ;
                }

                if((saved_ap_conf->used_ap >= MAX_AP_CONFIG_NUM)
                    ||(saved_ap_conf->used_ap < 0)) {
                    NETMGR_WIFI_LOGE("%s:%d invalid used ap %d\n", __func__, __LINE__, saved_ap_conf->used_ap);
                    return ;
                }

                i = saved_ap_conf->used_ap;
                if(i == -1) {
                    NETMGR_WIFI_LOGI("%s:%d invalid used_ap=%d\n", __func__, __LINE__, i);
                    return ;
                }

#if (RHINO_CONFIG_USER_SPACE > 0)
                change = (netmgr_wifi_network_status_change_t*)res_malloc(node->group->pid, sizeof(netmgr_wifi_network_status_change_t));
#else
                change = (netmgr_wifi_network_status_change_t*)malloc(sizeof(netmgr_wifi_network_status_change_t));
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
                if(change == NULL) {
#if (RHINO_CONFIG_USER_SPACE > 0)
                    NETMGR_WIFI_LOGE("%s:%d res_malloc failed\n", __func__, __LINE__);
#else
                    NETMGR_WIFI_LOGE("%s:%d malloc failed\n", __func__, __LINE__);
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
                    return ;
                }

                memset(change, 0, sizeof(netmgr_wifi_network_status_change_t));

                wifi_get_rssi(&change->quantity);
                NETMGR_WIFI_LOGI("%s:%d quantity=%d event=%d change=%p\n", __func__, __LINE__, change->quantity, event, change);
                if(event == EVENT_NETMGR_WIFI_DISCONNECTED) {
                    wifi_status = "disconnect";
                }
                else {
                    wifi_status = "connect";
                }
                NETMGR_WIFI_LOGD("%s:%d saved_ap_conf=%p\n", __func__, __LINE__, saved_ap_conf);
                if (strlen(wifi_status) <= NETMGR_WIFI_STATUS_MAX_LENGTH) {
                    strncpy(change->status, wifi_status, strlen(wifi_status) + 1);
                } else {
                    strncpy(change->status, wifi_status, NETMGR_WIFI_STATUS_MAX_LENGTH);
                }
                NETMGR_WIFI_LOGI("%s:%d saved_ap_conf->ssid=%s\n", __func__, __LINE__, saved_ap_conf->config[i].ssid);
                NETMGR_WIFI_LOGI("%s:%d change->status=%s\n", __func__, __LINE__, change->status);
                strncpy(change->ssid, saved_ap_conf->config[i].ssid, strlen(saved_ap_conf->config[i].ssid));
                NETMGR_WIFI_LOGD("%s:%d change->ssid=%s\n", __func__, __LINE__, change->ssid);
                NETMGR_WIFI_LOGD("%s:%d saved_ap_conf->pwd=%s\n", __func__, __LINE__, saved_ap_conf->config[i].pwd);
                memcpy(change->password, saved_ap_conf->config[i].pwd, sizeof(saved_ap_conf->config[i].pwd));

                msg.id = NETMGR_MSGID_NETWORK_STATUS;
                msg.data.network_status_change = change;
                NETMGR_WIFI_LOGD("%s:%d status=%s ssid=%s password=%s\n", __func__, __LINE__, change->status, change->ssid, change->password);
#if (RHINO_CONFIG_USER_SPACE > 0)
                wifi_msg_cb(node->group, (netmgr_msg_cb_t)node->msg_cb, &msg);
                res_free(node->group->pid, change);
#else
                event_publish(event, &msg);
                node->msg_cb(&msg);
                free(change);
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
            }
            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
            node = node->next;
        }
    }
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
}

static void wifi_indicate_event_handle(void* hdl, int wifi_event, void* arg)
{
    int event;
    netmgr_conn_t* conn = g_wifi_conn_info;


    NETMGR_WIFI_LOGD("%s:%d wifi_event=%d\n", __func__, __LINE__, wifi_event);
    switch(wifi_event)
    {
        case EVENT_WIFI_DISCONNECTED:
            event = EVENT_NETMGR_WIFI_DISCONNECTED;
            NETMGR_WIFI_LOGD("Disconnection indication received");
            break;
        case EVENT_WIFI_CONNECTED:
            event = EVENT_NETMGR_WIFI_CONNECTED;
            conn->netif = (struct netif*)arg;
		    NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_CONNECTED");
            break;
        case EVENT_WIFI_4WAY_HANDSHAKE_DONE:
            event = EVENT_NETMGR_WIFI_4WAY_HANDSHAKE_DONE;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_FOURWAY_HANDSHAKE_DONE");
            break;
        case EVENT_WIFI_NETWORK_NOT_FOUND:
            event = EVENT_NETMGR_WIFI_NETWORK_NOT_FOUND;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_NO_NETWORK");
            break;
        case EVENT_WIFI_SCAN_STARTED:
            event = EVENT_NETMGR_WIFI_SCAN_STARTED;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_SCAN_START");
            break;
        case EVENT_WIFI_SCAN_FAILED:
            event = EVENT_NETMGR_WIFI_SCAN_FAILED;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_SCAN_FAILED");
            break;
        case EVENT_WIFI_AUTHENTICATING:
            event = EVENT_NETMGR_WIFI_AUTHENTICATING;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_AUTHENTICATION");
            break;
        case EVENT_WIFI_AUTH_REJECT:
            event = EVENT_NETMGR_WIFI_AUTH_REJECT;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_AUTH_REJECT");
            break;
        case EVENT_WIFI_DEAUTH:
            event = EVENT_NETMGR_WIFI_DEAUTH;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_DEAUTH");
            break;
        case EVENT_WIFI_AUTH_TIMEOUT:
            event = EVENT_NETMGR_WIFI_AUTH_TIMEOUT;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_AUTH_TIMEOUT");
            break;
        case EVENT_WIFI_ASSOCIATING:
            event = EVENT_NETMGR_WIFI_ASSOCIATING;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_ASSOCIATING");
            break;
        case EVENT_WIFI_ASSOCIATED:
            event = EVENT_NETMGR_WIFI_ASSOCIATED;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_ASSOCIATED");
            break;
        case EVENT_WIFI_ASSOC_REJECT:
            event = EVENT_NETMGR_WIFI_ASSOC_REJECT;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_ASSOC_REJECT");
            break;
        case EVENT_WIFI_ASSOC_TIMEOUT:
            event = EVENT_NETMGR_WIFI_ASSOC_TIMEOUT;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_ASSOC_TIMEOUT");
            break;
        case EVENT_WIFI_HANDSHAKE_FAILED:
            event = EVENT_NETMGR_WIFI_HANDSHAKE_FAILED;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_HANDSHAKE_FAILED");
            break;
        case EVENT_WIFI_4WAY_HANDSHAKE:
            event = EVENT_NETMGR_WIFI_4WAY_HANDSHAKE;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_4WAY_HANDSHAKE");
            break;
        case EVENT_WIFI_GROUP_HANDSHAKE:
            event = EVENT_NETMGR_WIFI_GROUP_HANDSHAKE;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_GROUP_HANDSHAKE");
            break;
        case EVENT_WIFI_GROUP_HANDSHAKE_DONE:
            event = EVENT_NETMGR_WIFI_GROUP_HANDSHAKE_DONE;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_EVENT_GROUP_HANDSHAKE_DONE");
            break;
        case EVENT_WIFI_CONN_TIMEOUT:
            event = EVENT_NETMGR_WIFI_CONN_TIMEOUT;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_CONN_TIMEOUT");
            break;
        case EVENT_DHCP_SUCCESS:
            event = EVENT_NETMGR_DHCP_SUCCESS;
            NETMGR_WIFI_LOGD("NETMGR_DHCP_SUCCESS");
            break;
        case EVENT_SNTP_SUCCESS:
            event = EVENT_NETMGR_SNTP_SUCCESS;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_SNTP_SUCCESS");
            break;
        case EVENT_CONN_RECONNECT:
            event = EVENT_NETMGR_CONN_RECONNECT;
            NETMGR_WIFI_LOGD("NETMGR_WIFI_CONN_RECONNECT");
            break;
        default:
            NETMGR_WIFI_LOGD("not support event=%d\n", wifi_event);
            return ;
    }

    if(event >= EVENT_NETMGR_WIFI_DISCONNECTED && (event <= EVENT_NETMGR_MAX)) {
        if(conn == NULL) {
            NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
            return;
        }

        NETMGR_WIFI_LOGI("%s:%d event=%d\n", __func__, __LINE__, event);
        wifi_status_change(conn, event);

    }
    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
}

static bool sntp_start(void)
{
    struct timeval ntp_time;
    int round = 0;
    sntp_arg m_sntp_arg = {
            NULL,
            SNTP_RCV_TIMEOUT,
            SNTP_PERSERV_RETRY_TIMES};

    while(1) {
        NETMGR_WIFI_LOGI("sntp getting time.");
        sntp_config_servaddr();
        if (0 == sntp_get_time(&m_sntp_arg, &ntp_time)) {
            NETMGR_WIFI_LOGI("[sntp] OK: sec %lld usec %ld\n", ntp_time.tv_sec, ntp_time.tv_usec);
            return true;
        } else if((g_wifi_conn_info->state != CONN_STATE_CONNECTED)
        && (g_wifi_conn_info->state != CONN_STATE_OBTAINING_IP)
        && (g_wifi_conn_info->state != CONN_STATE_NETWORK_CONNECTED)) {
            NETMGR_WIFI_LOGE("sntp task quit round for network is disconnected\n");
            return false;
        }
        else {
            int retry_time = retry_backoff_time[round];
            NETMGR_WIFI_LOGI("[sntp] wait for sntp done...e\n");
            if(round < NETMGR_WIFI_RETRY_MAX_NUM - 1) {
                round ++;
            }
            aos_msleep(retry_time * 1000);
        }
    }
}

static void periodic_sntp_handle()
{

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);

    if((g_is_sntp_enabled == true) && (sntp_start() == true)) {
        //netmgr_stat_chg_event(m, NOTIFY_SNTP_SUCCESS, NULL);
        event_publish(EVENT_SNTP_SUCCESS, NULL);
        NETMGR_WIFI_LOGI("%s:%d post sntp action\n", __func__, __LINE__);
        //aos_post_delayed_action(SNTP_PERIODIC_SYNC_INTERVAL_MS, periodic_sntp_handle, NULL);
    }
    else {
        //netmgr_stat_chg_event(m, NOTIFY_SNTP_FAILED, NULL);
        NETMGR_WIFI_LOGI("%s:%d cancel sntp action\n", __func__, __LINE__);
        //aos_cancel_delayed_action(SNTP_PERIODIC_SYNC_INTERVAL_MS, periodic_sntp_handle, NULL);
    }

}

int netmgr_wifi_init(netmgr_hdl_t hdl)
{
    int err;
    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);

    if(g_wifi_conn_info != NULL) {
        NETMGR_WIFI_LOGE("wifi service has already initialized.");
        return 0;
    }

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    /* init connection info */
    g_wifi_conn_info = netmgr_conn_init(NETMGR_TYPE_WIFI);
    if(g_wifi_conn_info == NULL) {
        NETMGR_WIFI_LOGE("wifi service init failed.");
        return -1;
    }

    g_wifi_conn_info->auto_reconnect = false;
    g_wifi_conn_info->use_ip_mode = NETMGR_WIFI_IP_MODE_AUTO;

    g_wifi_conn_info->connected_cb = wifi_connected;
    g_wifi_conn_info->failed_cb = wifi_failed;
    g_wifi_conn_info->obtaining_ip_cb = wifi_obtaining_ip;
    g_wifi_conn_info->network_connected_cb = wifi_network_connected;
    g_wifi_conn_info->disconnecting_cb = wifi_disconnecting;
    g_wifi_conn_info->disconnected_cb = wifi_disconnected;

    g_wifi_conn_info->saved_config = (netmgr_wifi_ap_config_t*)malloc(sizeof(netmgr_wifi_ap_config_t));
    g_wifi_conn_info->hdl = hdl;
    if(g_wifi_conn_info->saved_config == NULL) {
        NETMGR_WIFI_LOGE("malloc conn info saved config failed\n");
        return -1;
    } else {
        netmgr_wifi_ap_config_t* saved_ap_conf = g_wifi_conn_info->saved_config;
        if(saved_ap_conf == NULL) {
            NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
            return -1;
        }
        memset(saved_ap_conf, 0, sizeof(netmgr_wifi_ap_config_t));
        saved_ap_conf->used_ap = -1;
        NETMGR_WIFI_LOGI("%s:%d memset ap config\n", __func__, __LINE__);
    }
    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);

    if(wifi_queue_init() != 0) {
        NETMGR_WIFI_LOGE("wifi service queue init failed\n");
        return -1;
    }

    if(wifi_recvmsg_task() != 0) {
        NETMGR_WIFI_LOGE("wifi recv msg task create failed\n");
        return -1;
    }

    err = aos_sem_new(&g_scan_result_sem, 0);
    if(err != 0) {
        NETMGR_WIFI_LOGE("%s:%d sem new failed, err=%d\n", __func__, __LINE__, err);
        return -1;
    }

    err = aos_sem_new(&g_connect_scan_sem, 0);
    if(err != 0) {
        NETMGR_WIFI_LOGE("%s:%d sem new failed, err=%d\n", __func__, __LINE__, err);
        return -1;
    }

    err = aos_sem_new(&g_disconnect_wait_sem, 0);
    if(err != 0) {
        NETMGR_WIFI_LOGE("%s:%d sem new failed, err=%d\n", __func__, __LINE__, err);
        return -1;
    }

    event_subscribe(EVENT_WIFI_SCAN_DONE, wifi_scan_completed_cb, NULL);

    event_subscribe(EVENT_WIFI_DISCONNECTED, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_SCAN_STARTED, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_SCAN_FAILED,  wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_NETWORK_NOT_FOUND, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_AUTHENTICATING, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_AUTH_REJECT, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_AUTH_TIMEOUT, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_ASSOCIATING, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_ASSOC_REJECT, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_ASSOC_TIMEOUT, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_ASSOCIATED, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_4WAY_HANDSHAKE, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_HANDSHAKE_FAILED, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_4WAY_HANDSHAKE_DONE, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_GROUP_HANDSHAKE, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_GROUP_HANDSHAKE_DONE, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_CONNECTED, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_CONN_TIMEOUT, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_WIFI_DEAUTH, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_DHCP_SUCCESS, wifi_stat_chg_cb, NULL);
    event_subscribe(EVENT_SNTP_SUCCESS, wifi_stat_chg_cb, NULL);

    event_subscribe(EVENT_WIFI_DISCONNECTED, wifi_connect_failed_cb, NULL);

    return 0;
}

void netmgr_wifi_deinit(netmgr_hdl_t hdl)
{
    struct netif* net_if;
    static netmgr_conn_t* conn;
    int ret;
    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);

    free(g_wifi_conn_info->saved_config);

    conn = g_wifi_conn_info;
    if(0 != (ret = ioctl(hdl, WIFI_DEV_CMD_DISCONNECT, NULL))) {
        NETMGR_WIFI_LOGE("%s:%d WIFI_DEV_CMD_DISCONNECT Failed\n", __func__, __LINE__);
    }

    netmgr_conn_deinit(g_wifi_conn_info);
    g_wifi_conn_info = NULL;

    if(g_is_recvmsg_result_got == true) {
        NETMGR_WIFI_LOGI("%s:%d stop recvmsg task\n", __func__, __LINE__);
        g_is_recvmsg_result_got = false;
    }

    if(0 != wifi_send_queue_deinit()) {
        NETMGR_WIFI_LOGE("%s:%d send queue deinit failed\n", __func__, __LINE__);
        return;
    }

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    net_if = conn->netif;
    if((net_if != NULL) && (netif_is_up(net_if))) {
        if(netif_is_link_up(net_if)) {
            netifapi_netif_set_link_down(net_if);
        }
        if(netif_is_up(net_if)) {
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
        }
    }
    aos_sem_free(&g_scan_result_sem);
    aos_sem_free(&g_connect_scan_sem);
    aos_sem_free(&g_disconnect_wait_sem);

    return;
}

void* netmgr_wifi_get_netif(netmgr_hdl_t hdl)
{
    netmgr_conn_t *conn = g_wifi_conn_info;
    return conn->netif;
}

static int wifi_start_scan(netmgr_conn_t* conn, netmgr_wifi_ap_config_t* saved_ap_conf)
{
    static wifi_scan_result_t scan_result;
    ap_list_t ap_list[MAX_AP_CONFIG_NUM * 2];
    int ap_num;
    int i;
    int ret;

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    if((saved_ap_conf->ap_num > MAX_AP_CONFIG_NUM) ||
        (saved_ap_conf->ap_num < 0)) {
        NETMGR_WIFI_LOGE("%s:%d invalid ap num %d\n", __func__, __LINE__, saved_ap_conf->ap_num);
        return -1;
    }

    for(i = 0, ap_num = 0; (i < saved_ap_conf->ap_num)&&(ap_num < MAX_AP_CONFIG_NUM * 2); i++, ap_num++) {
        memset(&ap_list[ap_num], 0, sizeof(ap_list[ap_num]));
        NETMGR_WIFI_LOGI("%s:%d ssid=%s channel=%d\n", __func__, __LINE__, saved_ap_conf->config[i].ssid, saved_ap_conf->config[i].channel);
        /* reset ap_power*/
        saved_ap_conf->config[i].ap_power = 0;
        strncpy(ap_list[ap_num].ssid, saved_ap_conf->config[i].ssid, strlen(saved_ap_conf->config[i].ssid));
        ap_list[ap_num].channel = saved_ap_conf->config[i].channel;
        if(saved_ap_conf->config[i].contain_chinese == true) {
            strncpy(ap_list[ap_num].ssid, saved_ap_conf->config[i].gbk_ssid, strlen(saved_ap_conf->config[i].gbk_ssid));
            ap_list[ap_num].channel = saved_ap_conf->config[i].channel;
        }
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(aos_sem_is_valid(&g_connect_scan_sem) == 0) {
        NETMGR_WIFI_LOGI("%s:%d sem is invalid\n", __func__, __LINE__);

        ret = aos_sem_new(&g_connect_scan_sem, 0);
        if(ret != 0) {
            NETMGR_WIFI_LOGE("%s:%d sem new failed, ret=%d\n", __func__, __LINE__, ret);
            return -1;
        }
        NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    scan_result.ap_num = ap_num;
    scan_result.ap_list = ap_list;
    if(0 != (ret = ioctl(conn->hdl, WIFI_DEV_CMD_START_SPECIFIED_SCAN, &scan_result)))
    {
        NETMGR_WIFI_LOGE("%s:%d specified scan failed, ret =%d, use full scan\n", __func__, __LINE__, ret);
        if(0 != (ret = ioctl(conn->hdl, WIFI_DEV_CMD_START_SCAN, NULL))) {
            NETMGR_WIFI_LOGE("%s:%d full scan failed, ret = %d\n", __func__, __LINE__, ret);
            return -1;
        }
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    ret = aos_sem_wait(&g_connect_scan_sem, SCAN_RESULT_WAIT_TIMEOUT);
    if(ret != 0) {
        NETMGR_WIFI_LOGE("%s:%d sem wait %d seconds timeout\n",
                            __func__, __LINE__, SCAN_RESULT_WAIT_TIMEOUT/1000);
        return -1;
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    return 0;
}

static int wifi_start_connect(netmgr_hdl_t hdl, netmgr_conn_t* conn, const char* ssid, const uint8_t* password, const uint8_t* bssid, const wifi_sec_type_t sec_type, int time_ms)
{
    wifi_config_t type;

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    memset(&type, 0, sizeof(type));
    type.mode = WIFI_MODE_STA;

    strncpy(type.ssid, ssid, sizeof(type.ssid) - 1);
    memcpy(type.password, password, sizeof(type.password));
    NETMGR_WIFI_LOGI("%s:%d ssid=%s password=%s, bssid=%02x:%02x:%02x:%02x:%02x:%02x \n",
           __func__, __LINE__, ssid, password, bssid[0] & 0xff, bssid[1] & 0xff,
           bssid[2] & 0xff, bssid[3] & 0xff, bssid[4] & 0xff, bssid[5] & 0xff);
    NETMGR_WIFI_LOGI("%s:%d state=%d\n", __func__, __LINE__, conn->state);
    if((conn->state == CONN_STATE_DISCONNECTED) ||
       (conn->state == CONN_STATE_CONNECTING)) {
        int ret = 0;
        NETMGR_WIFI_LOGE("%s:%d start wifi\n", __func__, __LINE__);
        ret = ioctl(hdl, WIFI_DEV_CMD_CONNECT, &type);
        if(ret != 0) {
            NETMGR_WIFI_LOGE("%s:%d wifi connect failed=%d\n", __func__, __LINE__, ret);
            return -1;
        }
        return 0;
    }

    return 0;
}

/*  check if the ssid valid or contain chinese */
static int ssid_contain_chinese_check(const char *ssid) {
    int i = 0;
    int contain_chinese = 0;

    if(!ssid || !ssid[0]) {
        NETMGR_WIFI_LOGE("%s:%d Error: app ssid is NULL!\n", __func__, __LINE__);
        return -1;
    }

    while(ssid[i] != '\0') {
        /*  ascii code */
        if((unsigned char)ssid[i] > 0x7f) {
            contain_chinese = 1;
        }

        i++;

        if(i >= MAX_SSID_LEN) {
            NETMGR_WIFI_LOGE("%s:%d Error: utf-8 ssid len is too large\n", __func__, __LINE__);
            return -1;
        }
    }

    return contain_chinese;
}

int netmgr_wifi_connect(netmgr_hdl_t hdl, netmgr_wifi_connect_params_t *params)
{
    struct netif* net_if;
    uint8_t gbk_ssid[MAX_SSID_LEN];
    int ret;
    int expire_time = 0;
    netmgr_conn_t* conn = g_wifi_conn_info;
    netmgr_wifi_ap_config_t* saved_ap_conf;
    int retry = -1;
    char* ssid;
    char* pwd;
    // char* bssid;
    int time_ms;

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    if(params == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: params is invalid\n", __func__, __LINE__);
        return -1;
    }

    ssid    = params->ssid;
    pwd     = params->pwd;
    //bssid   = params->bssid;
    time_ms = params->timeout;

    saved_ap_conf = (netmgr_wifi_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        NETMGR_WIFI_LOGE("%s:%d connectiong ap info is null\n", __func__, __LINE__);
        if(ssid != NULL) {
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        }
        return -1;
    }

    if(strlen(ssid) != 0) {
        int contain_chinese = ssid_contain_chinese_check(ssid);

        if(contain_chinese == -1) {
            NETMGR_WIFI_LOGI("%s:%d invalid ssid=%s\n", __func__, __LINE__, ssid);
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
            return -1;
        }

        if((conn->state == CONN_STATE_CONNECTED)
            ||(conn->state == CONN_STATE_OBTAINING_IP)
            ||(conn->state == CONN_STATE_NETWORK_CONNECTED)) {
            NETMGR_WIFI_LOGI("%s:%d do wifi disconnect when wifi is connected\n", __func__, __LINE__);
            netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
        }

        memset(saved_ap_conf, 0, sizeof(netmgr_wifi_ap_config_t));
        NETMGR_WIFI_LOGI("%s:%d ssid=%s pwd=%s\n", __func__, __LINE__, ssid, pwd);

        /* saved to saved_ap_conf */
        saved_ap_conf->ap_num = 1;
        strncpy(saved_ap_conf->config[0].ssid, ssid, sizeof(saved_ap_conf->config[0].ssid) - 1);
        memcpy(saved_ap_conf->config[0].pwd, pwd, sizeof(saved_ap_conf->config[0].pwd));

        /* has chinese code */
        if((0 != contain_chinese) && (0 != string_convert(gbk_ssid, MAX_SSID_SIZE, (uint8_t*)ssid, strlen(ssid), UTF8_TO_GBK))) {
            const char* p_ssid = NULL;

            p_ssid = (char *)gbk_ssid;
            NETMGR_WIFI_LOGI("%s:%d gbk_ssid=%s ssid=%s len=%d\n", __func__, __LINE__, gbk_ssid, ssid, strlen(p_ssid));

            /* saved to saved_ap_conf */
            saved_ap_conf->config[0].contain_chinese = true;
            NETMGR_WIFI_LOGI("%s:%d len=%d\n", __func__, __LINE__, sizeof(saved_ap_conf->config[0].ssid) - 1);
            strncpy(saved_ap_conf->config[0].gbk_ssid, p_ssid, sizeof(saved_ap_conf->config[0].gbk_ssid) - 1);
        }

        /* get wifi config */
        if(-1 == get_wifi_config(NETMGR_WIFI_TEMP_CONF, saved_ap_conf, READ_SPECIFIC_CONFIG)) {
            if(-1 == get_wifi_config(NETMGR_WIFI_CONF, saved_ap_conf, READ_SPECIFIC_CONFIG)) {
                NETMGR_WIFI_LOGE("%s:%d no temp config found\n", __func__, __LINE__);
            } else {
                NETMGR_WIFI_LOGE("%s:%d no config found\n", __func__, __LINE__);
            }
        }
    }

    NETMGR_WIFI_LOGI("%s:%d time_ms=%d\n", __func__, __LINE__, time_ms);
    if(time_ms != 0) {
        expire_time = (int) aos_now_ms() + time_ms;
        NETMGR_WIFI_LOGI("%s:%d expire_time=%d\n", __func__, __LINE__, expire_time);
    }
    /* Setting default connect state */
    g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_FAILED;
    netmgr_conn_state_change(conn, CONN_STATE_CONNECTING);
    /* waiting scan result */
    while(1) {

        NETMGR_WIFI_LOGI("%s:%d state=%d\n", __func__, __LINE__, conn->state);
        NETMGR_WIFI_LOGI("%s:%d connect state=%d\n", __func__, __LINE__, g_netmgr_wifi_connect_state);

        if(conn->state == CONN_STATE_NETWORK_CONNECTED) {
            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
            ret = 0;
            /* update wifi config when reconnect successfully */
            if(strlen(ssid) == 0) {
                int channel;
                int i;

                saved_ap_conf = (netmgr_wifi_ap_config_t* )conn->saved_config;
                if(saved_ap_conf == NULL) {
                    NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
                    return -1;
                }
                i = saved_ap_conf->used_ap;
                if(0 != (ret = (ioctl(hdl, WIFI_DEV_CMD_GET_CHANNEL, &channel)))) {
                    NETMGR_WIFI_LOGE("%s:%d get channel failed, ret=%d\n", __func__, __LINE__, ret);
                } else if((i != -1) && (channel != saved_ap_conf->config[i].channel)) {
                    NETMGR_WIFI_LOGI("%s:%d new channel:%d old channel:%d\n",
                                          __func__, __LINE__, channel, saved_ap_conf->config[i].channel);
                    /* update channel information */
                    saved_ap_conf->config[i].channel = channel;
                }
                update_wifi_config(conn, NETMGR_WIFI_CONF);
            }
            break;
        }
        else if(conn->state == CONN_STATE_DISCONNECTED) {
            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
            net_if = conn->netif;
            if(net_if) {
               netifapi_dhcp_stop(net_if);
               netifapi_netif_set_down(net_if);
            }
            ret = -1;
            break;
        }
        else if((retry >= NETMGR_WIFI_RETRY_MAX_NUM)
                || ((time_ms != 0) &&((int)aos_now_ms() > expire_time))) {
            /* Timeout Occured */
            NETMGR_WIFI_LOGI("%s:%d timeout\n", __func__, __LINE__);
            if(g_netmgr_wifi_connect_state == NETMGR_WIFI_CONNECT_SUCCESS) {
                NETMGR_WIFI_LOGI("%s:%d stop dhcp\n", __func__, __LINE__);
                net_if = conn->netif;
                if(net_if) {
                    netifapi_dhcp_stop(net_if);
                    netifapi_netif_set_down(net_if);
                }
            }

            g_netmgr_wifi_connect_state = NETMGR_WIFI_CONNECT_FAILED;
            if(g_handshake_failed_retry != 0) {
                g_handshake_failed_retry = NETMGR_WIFI_HANDSHAKE_FAILED_MAX_RETRY;
                //netmgr_stat_chg_event(m, NOTIFY_WIFI_HANDSHAKE_FAILED, NULL);
            } else {
                //netmgr_stat_chg_event(m, NOTIFY_WIFI_CONN_TIMEOUT, NULL);
            }
            ret = -1;
            break;
        }
        else if((g_netmgr_wifi_connect_state == NETMGR_WIFI_SCAN_FAILED)
              || (g_netmgr_wifi_connect_state == NETMGR_WIFI_CONNECT_FAILED)) {
            NETMGR_WIFI_LOGI("%s:%d start scan\n", __func__, __LINE__);

            net_if = conn->netif;
            if((retry != -1) && net_if) {
                NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);

                if(!net_if) {
                    NETMGR_WIFI_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
                    return -1;
                } else {
                    if(netif_is_link_up(net_if)) {
                        netifapi_netif_set_link_down(net_if);
                    }
                    if(netif_is_up(net_if)) {
                        netifapi_dhcp_stop(net_if);
                        netifapi_netif_set_down(net_if);
                    }
                }

                aos_msleep(retry_backoff_time[retry] * 1000);

                if(!netif_is_link_up(net_if)) {
                    netifapi_netif_set_link_up(net_if);
                }
                if(!netif_is_up(net_if)) {
                    netifapi_netif_set_up(net_if);
                }
            }

            retry ++;

            g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_START;
            if(0 != wifi_start_scan(conn, saved_ap_conf)) {
                NETMGR_WIFI_LOGE("%s:%d\n", __func__, __LINE__);
                g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_FAILED;
                ret = -1;
                /* Wait 1s for avoid scan conflict */
                aos_msleep(1000);
                continue;
            }
        }
        else if(g_netmgr_wifi_connect_state == NETMGR_WIFI_SCAN_SUCCESS){
            int i, j;

            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);

            for(i = 0, j = -1; i < saved_ap_conf->ap_num; i++) {
                NETMGR_WIFI_LOGI("%s:%d [%d] power=%d\n", __func__, __LINE__, i, saved_ap_conf->config[i].ap_power);
                if((i == 0) && (saved_ap_conf->config[i].ap_power > -75)
                    && (saved_ap_conf->config[i].ap_power != 0)) {
                    j = 0;
                    break;
                }
                else if(saved_ap_conf->config[i].ap_power != 0) {
                    if(j == -1) {
                        j = i;
                    }
                    else if(saved_ap_conf->config[j].ap_power < saved_ap_conf->config[i].ap_power) {
                        j = i;
                    }
                }
            }

            if(j == -1) {
                NETMGR_WIFI_LOGE("%s:%d no ap found\n", __func__, __LINE__);
                g_netmgr_wifi_connect_state = NETMGR_WIFI_SCAN_FAILED;
                ret = -1;
                continue;
            }

            i = j;

            g_netmgr_wifi_connect_state = NETMGR_WIFI_CONNECT_START;

            saved_ap_conf->used_ap = i;
            NETMGR_WIFI_LOGI("%s:%d ap_num=%d i=%d\n", __func__, __LINE__, saved_ap_conf->ap_num, i);
            /* Reset retry to 0 */
            retry = 0;
            if(saved_ap_conf->config[i].ssid_format == NETMGR_SSID_FORMAT_UTF8) {
                ret = wifi_start_connect(hdl, conn,
                                     saved_ap_conf->config[i].ssid,
                                     saved_ap_conf->config[i].pwd,
                                     saved_ap_conf->config[i].bssid,
                                     saved_ap_conf->config[i].sec_type,
                                     time_ms);
            } else {
                ret = wifi_start_connect(hdl, conn, saved_ap_conf->config[i].gbk_ssid,
                                         saved_ap_conf->config[i].pwd,
                                         saved_ap_conf->config[i].bssid,
                                         saved_ap_conf->config[i].sec_type,
                                         time_ms);
            }
        }
        else {
            int retry_time = retry_backoff_time[retry];
            if(g_netmgr_wifi_connect_state == NETMGR_WIFI_CONNECT_SUCCESS) {
                retry ++;
            }
            NETMGR_WIFI_LOGI("%s:%d sleep aos_now_ms=%d retry=%d\n", __func__, __LINE__, (int)aos_now_ms(), retry);
            aos_msleep(retry_time * 1000);
        }

        if(time_ms != 0) {
            NETMGR_WIFI_LOGI("%s:%d timeout time:%d aos_now_ms=%d\n", __func__, __LINE__, (int)expire_time,(int)aos_now_ms());
        }
    }
    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    if(ret != 0) {
        /* If ssid is not null trigger reconnect here
         * If ssid is null tigger reconnect by reconnect task */
        if(g_handshake_failed_retry < NETMGR_WIFI_HANDSHAKE_FAILED_MAX_RETRY) {
            NETMGR_WIFI_LOGI("%s:%d tigger reconnect\n", __func__, __LINE__);
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        }
    }
    return ret;
}

int netmgr_wifi_disconnect(netmgr_hdl_t hdl)
{
    netmgr_conn_t *conn = g_wifi_conn_info;
    struct netif* net_if;
    int ret;

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(conn != NULL) {
        NETMGR_WIFI_LOGE("%s:%d state=%d\n", __func__, __LINE__, conn->state);
        netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
    } else {
        return -1;
    }

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    net_if = conn->netif;
    if(net_if != NULL) {
        if(netif_is_link_up(net_if)) {
            netifapi_netif_set_link_down(net_if);
        }
        if(netif_is_up(net_if)) {
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
        }
    }
    if(0 != (ret = ioctl(hdl, WIFI_DEV_CMD_DISCONNECT, NULL))) {
    }

    NETMGR_WIFI_LOGI("%s:%d sem wait %d seconds\n",
                      __func__, __LINE__, DISCONNECT_WAIT_TIMEOUT/1000);
    if(0 != aos_sem_wait(&g_disconnect_wait_sem, DISCONNECT_WAIT_TIMEOUT)) {
        NETMGR_WIFI_LOGE("%s:%d sem wait %d seconds timeout\n",
                      __func__, __LINE__, DISCONNECT_WAIT_TIMEOUT/1000);
        return -1;
    }
    NETMGR_WIFI_LOGI("%s:%d sem wait %d seconds end\n",
                      __func__, __LINE__, DISCONNECT_WAIT_TIMEOUT/1000);

	return 0;
}

void netmgr_wifi_auto_reconnect(int enable)
{
    netmgr_conn_t *conn = g_wifi_conn_info;
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }

    NETMGR_WIFI_LOGI("%s:%d enable=%d\n", __func__, __LINE__, enable);
    conn->auto_reconnect = enable;

    if((conn->auto_reconnect == true)
       && (conn->state == CONN_STATE_DISCONNECTED)) {
        wifi_auto_reconnect_task();
    }
}

#if (RHINO_CONFIG_USER_SPACE > 0)
static int  wifi_msg_cb(task_group_t* group, netmgr_msg_cb_t cb, netmgr_msg_t* msg)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;
    kbuf_queue_t *send_queue   = NULL;
    kbuf_queue_t *recv_queue   = NULL;

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    if(group == NULL) {
        NETMGR_WIFI_LOGE("%s:%d invalid group\n", __func__, __LINE__);
        return -1;
    }

    if(msg == NULL) {
        NETMGR_WIFI_LOGE("%s:%d invalid msg\n", __func__, __LINE__);
        return -2;
    }

    send_queue = group->cb_call_buf_queue;
    recv_queue = group->cb_ret_buf_queue;

    call_msg.arg_value[0] = (netmgr_msg_t *) res_malloc(group->pid, sizeof(netmgr_msg_t));
    if(call_msg.arg_value[0] == NULL) {
        NETMGR_WIFI_LOGE("%s:%d res_malloc failed\n", __func__, __LINE__);
        return -3;
    }

    memset(call_msg.arg_value[0], 0, sizeof(netmgr_msg_t));
    memcpy(call_msg.arg_value[0], msg, sizeof(netmgr_msg_t));
    call_msg.func_ptr     = cb;
    call_msg.arg_cnt      = 1;
    call_msg.has_ret      = 1;

    NETMGR_WIFI_LOGI("%s:%d wlan_status=%d\n", __func__, __LINE__, msg->data.status);
    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        res_free(group->pid, call_msg.arg_value[0]);
        NETMGR_WIFI_LOGE("%s:%d buf queue push failed\n", __func__, __LINE__);
        return -4;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        res_free(group->pid, call_msg.arg_value[0]);
        NETMGR_WIFI_LOGE("%s:%d buf queue pop failed\n", __func__, __LINE__);
        return -5;
    }

    res_free(group->pid, call_msg.arg_value[0]);

    return 0;
}
#endif /* RHINO_CONFIG_USER_SPACE > 0 */

int netmgr_wifi_set_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{
    netmgr_conn_t *conn = g_wifi_conn_info;
    netmgr_wifi_msg_cb_node_t * node;
#if (RHINO_CONFIG_USER_SPACE > 0)
    ktask_t *cur = NULL;
    task_group_t *group;
#endif /* RHINO_CONFIG_USER_SPACE > 0 */

    NETMGR_WIFI_LOGI("%s:%d msg_cb=%p\n", __func__, __LINE__, cb);
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

#if (RHINO_CONFIG_USER_SPACE > 0)
    cur = krhino_cur_task_get();
    group = cur->task_group;
    NETMGR_WIFI_LOGI("%s:%d group=%p pid=%d\n", __func__, __LINE__, group, group->pid);
#endif /* RHINO_CONFIG_USER_SPACE > 0 */

    node = (netmgr_wifi_msg_cb_node_t *) conn->msg_cb_list;

    if(node == NULL) {
        conn->msg_cb_list = (netmgr_wifi_msg_cb_node_t *)malloc(sizeof(netmgr_wifi_msg_cb_node_t));
        if(conn->msg_cb_list == NULL) {
            NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
            return -1;
        }
        node = (netmgr_wifi_msg_cb_node_t *) conn->msg_cb_list;
        node->next = NULL;
        node->msg_cb = (void*) cb;
#if (RHINO_CONFIG_USER_SPACE > 0)
        node->group = group;
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
    } else {
        while(node->next != NULL) {
            node = node->next;
        }

        node->next = (netmgr_wifi_msg_cb_node_t *)malloc(sizeof(netmgr_wifi_msg_cb_node_t));
        if(node->next == NULL) {
            NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
            return -1;
        }
        node->next->next = NULL;
        node->next->msg_cb = (void*) cb;
#if (RHINO_CONFIG_USER_SPACE > 0)
        node->next->group = group;
#endif /* RHINO_CONFIG_USER_SPACE > 0 */
    }

    return 0;
}

int netmgr_wifi_del_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{
    netmgr_conn_t *conn = g_wifi_conn_info;
    netmgr_wifi_msg_cb_node_t *node;
    netmgr_wifi_msg_cb_node_t *prev_node;

    NETMGR_WIFI_LOGI("%s:%d msg_cb=%p\n", __func__, __LINE__, cb);
    if(conn == NULL) {
        NETMGR_WIFI_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    node = (netmgr_wifi_msg_cb_node_t *) conn->msg_cb_list;
    prev_node = node;

    while(node != NULL) {
        if(node->msg_cb == (void *)cb) {
            NETMGR_WIFI_LOGI("%s:%d cb:%p found\n", __func__, __LINE__, cb);
            if(prev_node == node) {
                free(node);
                conn->msg_cb_list = NULL;
            }
            else {
                prev_node->next = node->next;
                free(node);
            }
            return 0;
        }

        prev_node = node;
        node = node->next;
    }

    NETMGR_WIFI_LOGE("%s:%d cb:%p is not found\n", __func__, __LINE__, cb);
    return -1;
}

int netmgr_wifi_set_ip_mode(netmgr_wifi_ip_mode_t mode)
{
    struct netif* net_if;
    netmgr_conn_t *conn = g_wifi_conn_info;

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    net_if = conn->netif;
    if(!net_if) {
        NETMGR_WIFI_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    if(conn->use_ip_mode != mode) {
        const ip_addr_t *dns = NULL;
        conn->use_ip_mode = mode;
        NETMGR_WIFI_LOGI("%s:%d set ip mode %d\n", __func__, __LINE__, mode);
        if(mode == NETMGR_WIFI_IP_MODE_AUTO) {
            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
            netifapi_netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
            dns_setserver(0, IP4_ADDR_ANY);
            dns_setserver(1, IP4_ADDR_ANY);
            //netmgr_stat_chg_event(m, CONN_STATE_OBTAINING_IP, NULL);
        } else {
            NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
            netifapi_netif_set_up(net_if);
            netifapi_netif_set_addr(net_if, &conn->static_ip, &conn->static_mask, &conn->static_gw);
            dns = dns_getserver(0);
            dns_setserver(0, &conn->static_dns);
            dns_setserver(1, dns);
        }
    }
    return 0;
}

netmgr_wifi_ip_mode_t netmgr_wifi_get_ip_mode(void)
{
    netmgr_conn_t *conn = g_wifi_conn_info;
    return conn->use_ip_mode;
}

int netmgr_wifi_get_ip_addr(char* ip_address)
{
    struct netif* net_if;
    static netmgr_conn_t* conn;

    conn = g_wifi_conn_info;
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    net_if = conn->netif;
    if(!net_if) {
        NETMGR_WIFI_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    sprintf(ip_address, "%d.%d.%d.%d",
                     ((u8_t *)&net_if->ip_addr.addr)[0],
                     ((u8_t *)&net_if->ip_addr.addr)[1],
                     ((u8_t *)&net_if->ip_addr.addr)[2],
                     ((u8_t *)&net_if->ip_addr.addr)[3]);

    return 0;
}

int netmgr_wifi_set_static_ip_stat(const char* ip_addr, const char* mask, const char* gw, const char* dns_server)
{
    struct netif* net_if;
    const ip_addr_t* dns;
    netmgr_conn_t *conn = g_wifi_conn_info;

    if((ip_addr == NULL) || (mask == NULL) || (gw == NULL) || (dns_server == NULL)) {
        NETMGR_WIFI_LOGE("%s:%d invalid params\n", __func__, __LINE__);
        return -1;
    }

    NETMGR_WIFI_LOGI("%s:%d ip_addr=%s mask=%s gw=%s dns_server=%s\n",
                       __func__, __LINE__, ip_addr, mask, gw, dns_server);
    net_if = conn->netif;
    if(!net_if) {
        NETMGR_WIFI_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    sscanf(ip_addr, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_ip.addr)[0],
                     &((u8_t *)&conn->static_ip.addr)[1],
                     &((u8_t *)&conn->static_ip.addr)[2],
                     &((u8_t *)&conn->static_ip.addr)[3]);
    NETMGR_WIFI_LOGI("%s:%d set ip address %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_ip.addr)[0],
                     ((u8_t *)&conn->static_ip.addr)[1],
                     ((u8_t *)&conn->static_ip.addr)[2],
                     ((u8_t *)&conn->static_ip.addr)[3]);

    sscanf(mask, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_mask.addr)[0],
                     &((u8_t *)&conn->static_mask.addr)[1],
                     &((u8_t *)&conn->static_mask.addr)[2],
                     &((u8_t *)&conn->static_mask.addr)[3]);
    NETMGR_WIFI_LOGI("%s:%d set mask %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_mask.addr)[0],
                     ((u8_t *)&conn->static_mask.addr)[1],
                     ((u8_t *)&conn->static_mask.addr)[2],
                     ((u8_t *)&conn->static_mask.addr)[3]);

    sscanf(gw, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_gw.addr)[0],
                     &((u8_t *)&conn->static_gw.addr)[1],
                     &((u8_t *)&conn->static_gw.addr)[2],
                     &((u8_t *)&conn->static_gw.addr)[3]);
    NETMGR_WIFI_LOGI("%s:%d set gw %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_gw.addr)[0],
                     ((u8_t *)&conn->static_gw.addr)[1],
                     ((u8_t *)&conn->static_gw.addr)[2],
                     ((u8_t *)&conn->static_gw.addr)[3]);

    sscanf(dns_server, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_dns.addr)[0],
                     &((u8_t *)&conn->static_dns.addr)[1],
                     &((u8_t *)&conn->static_dns.addr)[2],
                     &((u8_t *)&conn->static_dns.addr)[3]);
    NETMGR_WIFI_LOGI("%s:%d set dns server %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_dns.addr)[0],
                     ((u8_t *)&conn->static_dns.addr)[1],
                     ((u8_t *)&conn->static_dns.addr)[2],
                     ((u8_t *)&conn->static_dns.addr)[3]);

    netifapi_dhcp_stop(net_if);
    netifapi_netif_set_down(net_if);
    netifapi_netif_set_up(net_if);
    netifapi_netif_set_addr(net_if, &conn->static_ip, &conn->static_mask, &conn->static_gw);
    dns = dns_getserver(0);
    dns_setserver(0, &conn->static_dns);
    dns_setserver(1, dns);

    return 0;
}

int netmgr_wifi_get_ip_stat(char* ip_addr, char* mask, char* gw, char* dns_server)
{
    struct netif* net_if;
    const ip_addr_t* dns;
    static netmgr_conn_t* conn;

    conn = g_wifi_conn_info;
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    net_if = conn->netif;
    if(!net_if) {
        NETMGR_WIFI_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    sprintf(ip_addr, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&net_if->ip_addr.addr)[0],
                     ((u8_t *)&net_if->ip_addr.addr)[1],
                     ((u8_t *)&net_if->ip_addr.addr)[2],
                     ((u8_t *)&net_if->ip_addr.addr)[3]);

    sprintf(mask, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&net_if->netmask.addr)[0],
                     ((u8_t *)&net_if->netmask.addr)[1],
                     ((u8_t *)&net_if->netmask.addr)[2],
                     ((u8_t *)&net_if->netmask.addr)[3]);

    sprintf(gw, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&net_if->gw.addr)[0],
                     ((u8_t *)&net_if->gw.addr)[1],
                     ((u8_t *)&net_if->gw.addr)[2],
                     ((u8_t *)&net_if->gw.addr)[3]);

    dns = dns_getserver(0);
    sprintf(dns_server, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&dns->addr)[0],
                     ((u8_t *)&dns->addr)[1],
                     ((u8_t *)&dns->addr)[2],
                     ((u8_t *)&dns->addr)[3]);
    NETMGR_WIFI_LOGI("%s:%d ip_addr=%s mask=%s gw=%s dns=%s\n", __func__, __LINE__,
                        ip_addr, mask, gw, dns_server);
    return 0;

}

int netmgr_wifi_save_config(netmgr_hdl_t hdl)
{
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    return update_wifi_config(g_wifi_conn_info, NETMGR_WIFI_CONF);
}

int netmgr_wifi_get_config(netmgr_hdl_t hdl, netmgr_wifi_config_t* config)
{
    if(config == NULL) {
        NETMGR_WIFI_LOGE("%s:%d invalid config\n", __func__, __LINE__);
        return -1;
    }
    memset(config, 0, sizeof(netmgr_wifi_ap_config_t));
    if(-1 == get_wifi_config(NETMGR_WIFI_TEMP_CONF, config, READ_ALL_CONFIG)) {
        if(-1 == get_wifi_config(NETMGR_WIFI_CONF, config, READ_ALL_CONFIG)) {
            NETMGR_WIFI_LOGE("%s:%d no config found\n", __func__, __LINE__);
            return -1;
        }
    }
    return 0;
}

int netmgr_wifi_del_config(netmgr_hdl_t hdl, const char* ssid)
{
    netmgr_wifi_ap_config_t* saved_ap_conf;
    netmgr_conn_t* conn = g_wifi_conn_info;
    int err;
    int i;
    int j;

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(ssid == NULL) {
        NETMGR_WIFI_LOGI("%s:%d ssid is null\n", __func__, __LINE__);
        return -1;
    }

    if(conn == NULL) {
        NETMGR_WIFI_LOGI("%s:%d conn is null\n", __func__, __LINE__);
        return -1;
    }

    saved_ap_conf = (netmgr_wifi_ap_config_t*) conn->saved_config;
    if(saved_ap_conf == NULL) {
        NETMGR_WIFI_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return -1;
    }
    memset(saved_ap_conf, 0, sizeof(netmgr_wifi_ap_config_t));
    if(-1 == get_wifi_config(NETMGR_WIFI_TEMP_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
        if(-1 == get_wifi_config(NETMGR_WIFI_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
            NETMGR_WIFI_LOGE("%s:%d no config found\n", __func__, __LINE__);
            return -1;
        }
    }

    for(i = 0; i < saved_ap_conf->ap_num; i++) {
        if(strncmp(saved_ap_conf->config[i].ssid, ssid, strlen(ssid)) == 0) {
            break;
        }
    }

    if(i == saved_ap_conf->ap_num) {
        NETMGR_WIFI_LOGE("%s:%d ssid %s is not found in wifi config\n", __func__, __LINE__, ssid);
        return -1;
    }

    err = remove(NETMGR_WIFI_CONF);
    if( err < 0 ){
        NETMGR_WIFI_LOGE("%s:%d %s\n", __func__, __LINE__, strerror(errno));
    }
    err = remove(NETMGR_WIFI_TEMP_CONF);
    if( err < 0 ){
        NETMGR_WIFI_LOGE("%s:%d %s\n", __func__, __LINE__, strerror(errno));
    }

    memset(&saved_ap_conf->config[i], 0, sizeof(netmgr_wifi_ap_info_t));
    j = i + 1;
    while(j < saved_ap_conf->ap_num) {
        memcpy(&saved_ap_conf->config[i], &saved_ap_conf->config[j], sizeof(netmgr_wifi_ap_info_t));
        i = j;
        j ++;
    }

    saved_ap_conf->ap_num --;
    NETMGR_WIFI_LOGI("%s:%d update config and left ap num:%d\n", __func__, __LINE__, saved_ap_conf->ap_num);
    update_wifi_config(conn, NETMGR_WIFI_CONF);

    return 0;
}

int  netmgr_wifi_scan_result(netmgr_wifi_ap_list_t* ap_info, int num, netmgr_wifi_scan_type_t type)
{
    int ret;
    int i, j, min_idx;
    int8_t min_power;
    ap_list_t *ap_check;
    ap_list_t ap_list[NETMGR_WIFI_MAX_SPECIFIED_SCAN_NUM];

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(aos_sem_is_valid(&g_scan_result_sem) == 0) {
        NETMGR_WIFI_LOGI("%s:%d sem is invalid\n", __func__, __LINE__);
        ret = aos_sem_new(&g_scan_result_sem, 0);
        if(ret != 0) {
            NETMGR_WIFI_LOGE("%s:%d sem new failed, ret=%d\n", __func__, __LINE__, ret);
            return -1;
        }
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    g_scan_result.ap_num = 0;
    g_is_got_scan_request_cmd = true;
    if(type == NETMGR_WIFI_SCAN_TYPE_FULL || type == NETMGR_WIFI_SCAN_TYPE_FULL_MERGE) {
        if(0 != (ret = ioctl(g_wifi_conn_info->hdl, WIFI_DEV_CMD_START_SCAN, NULL))) {
            g_is_got_scan_request_cmd = false;
            NETMGR_WIFI_LOGE("%s:%d full scan failed, ret = %d\n", __func__, __LINE__, ret);
            return -1;
        }
    } else {
        for(i = 0; (i < num)&&(i < NETMGR_WIFI_MAX_SPECIFIED_SCAN_NUM); i++) {
            memset(&ap_list[i], 0, sizeof(ap_list[i]));
            NETMGR_WIFI_LOGI("%s:%d ssid=%s\n", __func__, __LINE__, ap_info[i].ssid);
            strncpy(ap_list[i].ssid, ap_info[i].ssid, strlen(ap_info[i].ssid));
        }
#ifdef  HTTP_DEMO_DEBUG
        scan_ap_list_t scan;

        scan.ap_list = ap_list;
        scan.ap_num  = num;
        if(0 != (ret = ioctl(g_wifi_conn_info->hdl, WIFI_DEV_CMD_START_SPECIFIED_SCAN, &scan))) {
#else
        if(0 != (ret = ioctl(g_wifi_conn_info->hdl, WIFI_DEV_CMD_START_SPECIFIED_SCAN, ap_list))) {
#endif
            g_is_got_scan_request_cmd = false;
            NETMGR_WIFI_LOGE("%s:%d specified scan failed\n", __func__, __LINE__);
            return -1;
        }
    }

    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    ret = aos_sem_wait(&g_scan_result_sem, SCAN_RESULT_WAIT_TIMEOUT);
    if(ret != 0) {
        g_is_got_scan_request_cmd = false;
        NETMGR_WIFI_LOGE("%s:%d sem wait %d seconds timeout\n",
                            __func__, __LINE__, SCAN_RESULT_WAIT_TIMEOUT/1000);
        return -1;
    }

    if(g_scan_result.ap_num == 0) {
        return 0;
    }

    if(type == NETMGR_WIFI_SCAN_TYPE_FULL_MERGE) {
        memset(ap_info, 0, num * sizeof(netmgr_wifi_ap_list_t));
        for(j = 0; j < num; j++) {
            ap_info[j].ap_power= -110;
        }

        /* find max power SSIDs */
        for(i = 0; i < g_scan_result.ap_num; i++) {
            ap_check = &g_scan_result.ap_list[i];

            /* anonymous ssid, go to next */
            if(ap_check->ssid[0] == 0) continue;

            /* ssid repeat check */
            for(j = 0; j < num; j++) {
                /* ssid repeat */
                if (strcmp(ap_info[j].ssid, ap_check->ssid) == 0) {
                    if (ap_info[j].ap_power < ap_check->ap_power) {
                        ap_info[j] = *(netmgr_wifi_ap_list_t *)ap_check;
                    }
                    break;
                }
            }
            if (j != num) {
                /* find ssid repeat */
                continue;
            }

            /* find mini power ap_info */
            min_power = 0;
            min_idx = -1;
            for(j = 0; j < num; j++) {
                if (min_power > ap_info[j].ap_power) {
                    min_idx = j;
                    min_power = ap_info[j].ap_power;
                }
            }

            /* compare and update */
            if (min_idx != -1 && min_power < ap_check->ap_power) {
                ap_info[min_idx] = *(netmgr_wifi_ap_list_t *)ap_check;
            }
        }
    } else {
        NETMGR_WIFI_LOGI("%s:%d num=%d ap_num=%d\n", __func__, __LINE__, num, g_scan_result.ap_num);
        num = num >= g_scan_result.ap_num ? g_scan_result.ap_num : num;
        memcpy(ap_info, g_scan_result.ap_list, num * sizeof(ap_list_t));
    }
    free(g_scan_result.ap_list);
    g_scan_result.ap_list = NULL;

    NETMGR_WIFI_LOGD("%s:%d\n", __func__, __LINE__);
    return g_scan_result.ap_num;
}

netmgr_conn_state_t netmgr_wifi_get_wifi_state() {
    netmgr_conn_t* conn = g_wifi_conn_info;

    if(conn) {
        NETMGR_WIFI_LOGD("%s:%d state=%d\n", __func__, __LINE__, conn->state);
        return conn->state;
    }
    NETMGR_WIFI_LOGE("%s:%d get wifi state failed\n", __func__, __LINE__);
    return CONN_STATE_UNKNOWN;
}

void netmgr_wifi_start_monitor() {
    netmgr_conn_t* conn = g_wifi_conn_info;
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(conn != NULL) {
        if(ioctl(conn->hdl, WIFI_DEV_CMD_START_MONITOR, NULL) != 0) {
           NETMGR_WIFI_LOGE("WIFI_DEV_CMD_START_MONITOR failed\n");
        }
    }
}

void netmgr_wifi_stop_monitor() {
    netmgr_conn_t* conn = g_wifi_conn_info;
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(conn != NULL) {
        if(ioctl(conn->hdl, WIFI_DEV_CMD_STOP_MONITOR, NULL) != 0) {
           NETMGR_WIFI_LOGE("WIFI_DEV_CMD_STOP_MONITOR failed\n");
        }
    }
}

void netmgr_wifi_start_mgnt_monitor() {
    netmgr_conn_t* conn = g_wifi_conn_info;
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(conn != NULL) {
        if(ioctl(conn->hdl, WIFI_DEV_CMD_START_MGNT_MONITOR, NULL) != 0) {
           NETMGR_WIFI_LOGE("WIFI_DEV_CMD_START_MGNT_MONITOR failed\n");
        }
    }
}

void netmgr_wifi_stop_mgnt_monitor() {
    netmgr_conn_t* conn = g_wifi_conn_info;
    NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
    if(conn != NULL) {
        if(ioctl(conn->hdl, WIFI_DEV_CMD_STOP_MGNT_MONITOR, NULL) != 0) {
           NETMGR_WIFI_LOGE("WIFI_DEV_CMD_STOP_MGNT_MONITOR failed\n");
        }
    }
}

// static monitor_data_cb_t g_mgnt_monitor_data_cb = NULL;
#if (RHINO_CONFIG_USER_SPACE > 0)
static task_group_t* g_mgnt_monitor_task_group = NULL;

static void  wifi_mgnt_monitor_data_cb(uint8_t *data, int len, wifi_link_info_t *info)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;
    kbuf_queue_t *send_queue   = NULL;
    kbuf_queue_t *recv_queue   = NULL;
    monitor_data_cb_t mgnt_monitor_cb = g_mgnt_monitor_data_cb;
    task_group_t* group = g_mgnt_monitor_task_group;

    if(mgnt_monitor_cb == NULL) {
        NETMGR_WIFI_LOGE("%s:%d invalid mgnt data callback\n", __func__, __LINE__);
        return ;
    }

    if(group == NULL) {
        NETMGR_WIFI_LOGE("%s:%d invalid mgnt monitor task group\n", __func__, __LINE__);
        return ;
    }

    if((len <= 0) || (data == NULL) || (info == NULL)) {
        NETMGR_WIFI_LOGE("%s:%d invalid params\n", __func__, __LINE__);
        return ;
    }

    send_queue = group->cb_call_buf_queue;
    recv_queue = group->cb_ret_buf_queue;

    call_msg.arg_value[0] = (netmgr_msg_t *) res_malloc(group->pid, len);
    if(call_msg.arg_value[0] == NULL) {
        NETMGR_WIFI_LOGE("%s:%d res_malloc failed\n", __func__, __LINE__);
        return ;
    }

    memset(call_msg.arg_value[0], 0, len);
    memcpy(call_msg.arg_value[0], data, len);

    call_msg.arg_value[1] = (void* )len;

    call_msg.arg_value[2] = (netmgr_msg_t *) res_malloc(group->pid, sizeof(wifi_link_info_t));
    if(call_msg.arg_value[2] == NULL) {
        res_free(group->pid, call_msg.arg_value[0]);
        NETMGR_WIFI_LOGE("%s:%d res_malloc failed\n", __func__, __LINE__);
        return ;
    }

    memset(call_msg.arg_value[2], 0, sizeof(wifi_link_info_t));
    memcpy(call_msg.arg_value[2], info, sizeof(wifi_link_info_t));

    call_msg.func_ptr     = g_mgnt_monitor_data_cb;
    call_msg.arg_cnt      = 3;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        res_free(group->pid, call_msg.arg_value[2]);
        res_free(group->pid, call_msg.arg_value[0]);
        NETMGR_WIFI_LOGE("%s:%d buf queue push failed\n", __func__, __LINE__);
        return ;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        res_free(group->pid, call_msg.arg_value[2]);
        res_free(group->pid, call_msg.arg_value[0]);
        NETMGR_WIFI_LOGE("%s:%d buf queue pop failed\n", __func__, __LINE__);
        return ;
    }

    res_free(group->pid, call_msg.arg_value[2]);
    res_free(group->pid, call_msg.arg_value[0]);

    return ;
}
#endif /* RHINO_CONFIG_USER_SPACE > 0 */

void netmgr_wifi_register_mgnt_monitor_cb(monitor_data_cb_t fn) {
    netmgr_conn_t* conn = g_wifi_conn_info;
    if(conn != NULL) {
#if (RHINO_CONFIG_USER_SPACE > 0)
        ktask_t *cur;

        cur = krhino_cur_task_get();
        g_mgnt_monitor_task_group = cur->task_group;
        NETMGR_WIFI_LOGI("%s:%d group=%p pid=%d\n", __func__, __LINE__, g_mgnt_monitor_task_group, g_mgnt_monitor_task_group->pid);
        g_mgnt_monitor_data_cb = fn;
        if(ioctl(conn->hdl, WIFI_DEV_CMD_REGISTER_MGNT_MONITOR_CB, wifi_mgnt_monitor_data_cb) != 0) {
            NETMGR_WIFI_LOGE("WIFI_DEV_CMD_REGISTER_MGNT_MONITOR_CB\n");
        }
#else
        if(ioctl(conn->hdl, WIFI_DEV_CMD_REGISTER_MGNT_MONITOR_CB, fn) != 0) {
            NETMGR_WIFI_LOGE("WIFI_DEV_CMD_REGISTER_MGNT_MONITOR_CB\n");
        }
#endif
    }
}

int netmgr_wifi_send_80211_raw_frame(uint8_t *buf, int len) {
    netmgr_conn_t* conn = g_wifi_conn_info;
    if(conn != NULL) {
        static raw_frame_t frame;
        frame.data = buf;
        frame.len = len;
        NETMGR_WIFI_LOGI("%s:%d\n", __func__, __LINE__);
        return ioctl(conn->hdl, WIFI_DEV_CMD_SEND_80211_RAW_FRAME, &frame);
    }

    return -1;
}

static void handle_netmgr_wifi_help_cmd()
{
    NETMGR_WIFI_LOGI("Usage: netgmr -t wifi [options]\n");
    NETMGR_WIFI_LOGI("       netmgr -t wifi [-h]\n");
    NETMGR_WIFI_LOGI("  -i,        Init wifi service\n");
    NETMGR_WIFI_LOGI("  -k,        Deinit wifi service\n");
    NETMGR_WIFI_LOGI("  -a,        Set auto reconnect wifi flag\n");
    NETMGR_WIFI_LOGI("  -b,        Set auto save ap info flag. 1, enable; 0, disable\n");
    NETMGR_WIFI_LOGI("  -c,        Connect wifi\n");
    NETMGR_WIFI_LOGI("  -e,        Disconnect wifi\n");
    NETMGR_WIFI_LOGI("  -m,        Set mac address\n");
    NETMGR_WIFI_LOGI("  -g,        Get mac address\n");
    NETMGR_WIFI_LOGI("  -s,        Show scan result\n");
    NETMGR_WIFI_LOGI("  -p,        Print wifi status\n");
    NETMGR_WIFI_LOGI("  -r,        Read wifi config\n");
    NETMGR_WIFI_LOGI("  -w,        Write wifi config\n");
    NETMGR_WIFI_LOGI("  -d,        Delete wifi config\n");
    NETMGR_WIFI_LOGI("  -n,        1, sntp enable 2, sntp disable no args show sntp enable/disable state\n");
    NETMGR_WIFI_LOGI("  -h,        Show netmgr wifi help commands\n");
    NETMGR_WIFI_LOGI("Example:\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -i\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -a 1\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -c bssid password\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -s\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -p\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -r\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -w network={\\nssid=\"apple\"\\npassword=\"aos123456\"\\nchannel=\"1\"\\n}\\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -d\n");
    NETMGR_WIFI_LOGI("netmgr -t wifi -n 0\n");
}

static void wifi_connect_handle(char * args)
{
    netmgr_wifi_connect_params_t* params = (netmgr_wifi_connect_params_t*) args;

    netmgr_wifi_connect(g_wifi_conn_info->hdl, params);
}

static void wifi_set_mac_handle(char * args)
{
    uint8_t* mac = (uint8_t*) args;
    wifi_set_mac(mac);
}

static void wifi_handle_cmd(int argc, char **argv)
{
    const char *rtype = argc > 0 ? argv[0] : "";

    if (strcmp(rtype, "-i") == 0) {
        NETMGR_WIFI_LOGI("%s:%d netmgr_service_init\n", __func__, __LINE__);
        netmgr_service_init(NULL);
    }
    else if (strcmp(rtype, "-k") == 0) {
        NETMGR_WIFI_LOGI("%s:%d netmgr_service_deinit\n", __func__, __LINE__);
        netmgr_service_deinit();
    }
    else if (strcmp(rtype, "-a") == 0) {
        if((argc > 1) && ((strcmp(argv[1], "0") == 0) || (strcmp(argv[1], "1") == 0))) {
            int enable = atoi(argv[1]);
            netmgr_wifi_auto_reconnect(enable);
        } else {
            NETMGR_WIFI_LOGI("%s:%d netmgr -a invalid params\n", __func__, __LINE__);
        }
    }
    else if (strcmp(rtype, "-b") == 0) {
        if((argc > 1) && ((strcmp(argv[1], "0") == 0) || (strcmp(argv[1], "1") == 0))) {
            int enable = atoi(argv[1]);
            if(enable) {
                netmgr_wifi_set_auto_save_ap(true);
            } else {
                netmgr_wifi_set_auto_save_ap(false);
            }
        } else {
            NETMGR_WIFI_LOGI("%s:%d invalid params\n", __func__, __LINE__);
        }
    }
    else if (strcmp(rtype, "-c") == 0) {
        if((argc > 1)) {
            netmgr_wifi_connect_params_t *params;

            params = (netmgr_wifi_connect_params_t*) malloc(sizeof(netmgr_wifi_connect_params_t));
            if(params == NULL) {
                NETMGR_WIFI_LOGE("%s:%d malloc failed\n", __func__, __LINE__);
                return ;
            }
            memset(params, 0, sizeof(netmgr_wifi_connect_params_t));
            strncpy(params->ssid, argv[1], sizeof(params->ssid)-1);
            params->timeout = 18000;

            if(argc > 2) {
                strncpy((char* )params->pwd, argv[2], sizeof(params->pwd)-1);
            }

            if(argc > 3) {
                strncpy((char* )params->bssid, argv[3], sizeof(params->bssid)-1);
            }

            if(argc > 4) {
                params->timeout = atoi(argv[4]);
            }

            aos_task_new("wifi_connect_task",(task_entry_t)wifi_connect_handle, params, 4*1024);

        } else {
            NETMGR_WIFI_LOGE("%s:%d netmgr -c invalid params\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-e") == 0) {
        netmgr_conn_t* conn = g_wifi_conn_info;
        if(conn != NULL) {
            netmgr_wifi_disconnect(conn->hdl);
        }
    } else if (strcmp(rtype, "-m") == 0) {
        uint8_t mac[6] = {0};
        if(argc > 1) {
            sscanf((char*)argv[1],"%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
                    &mac[0],
                    &mac[1],
                    &mac[2],
                    &mac[3],
                    &mac[4],
                    &mac[5]);
            aos_task_new("wifi_set_mac_task", (task_entry_t)wifi_set_mac_handle, mac, 4*1024);
        } else {
            NETMGR_WIFI_LOGE("%s:%d WIFI_SET_MAC invalid params\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-g") == 0) {
        uint8_t mac[6];
        if(-1 != wifi_get_mac(g_wifi_conn_info->hdl, mac)) {
            NETMGR_WIFI_LOGI("mac address=%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx\n",
              mac[0] & 0xff,
              mac[1] & 0xff,
              mac[2] & 0xff,
              mac[3] & 0xff,
              mac[4] & 0xff,
              mac[5] & 0xff);
        } else {
            NETMGR_WIFI_LOGE("%s:%d WIFI_GET_MAC failed\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-s") == 0) {
        netmgr_wifi_ap_list_t ap_info[32];
        int ap_num;

        memset(ap_info, 0, sizeof(ap_info));
        ap_num = netmgr_wifi_scan_result(ap_info, 32, NETMGR_WIFI_SCAN_TYPE_FULL);
        NETMGR_WIFI_LOGI("%s:%d ap_num=%d\n", __func__, __LINE__, ap_num);
        if((ap_num != -1) && (ap_num < 32)) {
            int i;
            for(i = 0; i < ap_num; i++ ) {
                NETMGR_WIFI_LOGI("%s:%d ssid=%s power=%d bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, __LINE__,
                    ap_info[i].ssid,
                    ap_info[i].ap_power,
                    ap_info[i].bssid[0] & 0xff,
                    ap_info[i].bssid[1] & 0xff,
                    ap_info[i].bssid[2] & 0xff,
                    ap_info[i].bssid[3] & 0xff,
                    ap_info[i].bssid[4] & 0xff,
                    ap_info[i].bssid[5] & 0xff
                    );
            }
        }
    } else if (strcmp(rtype, "-p") == 0) {
        netmgr_wifi_ap_config_t* ap_conf = (netmgr_wifi_ap_config_t* )g_wifi_conn_info->saved_config;
        char * ssid = NULL;

        if((ap_conf != NULL) && (ap_conf->used_ap != -1) &&(ap_conf->used_ap < ap_conf->ap_num)) {
            netmgr_wifi_ap_info_t * ap_info = NULL;
            int used_ap = ap_conf->used_ap;

            ap_info = &ap_conf->config[used_ap];

            ssid = ap_info->ssid;
            NETMGR_WIFI_LOGI("%s:%d ssid:%s\n", __func__, __LINE__, ssid);
        }

        if(g_wifi_conn_info->state == CONN_STATE_DISCONNECTING) {
            NETMGR_WIFI_LOGI("%s:%d DISCONNECTING\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_DISCONNECTED) {
            NETMGR_WIFI_LOGI("%s:%d DISCONNECTED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_CONNECTING) {
            NETMGR_WIFI_LOGI("%s:%d CONNECTING\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_CONNECTED) {
            NETMGR_WIFI_LOGI("%s:%d WIFI CONNECTED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_OBTAINING_IP) {
            NETMGR_WIFI_LOGI("%s:%d OBTAINING IP\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_NETWORK_CONNECTED) {
            NETMGR_WIFI_LOGI("%s:%d NETWORK_CONNECTED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_FAILED) {
            NETMGR_WIFI_LOGI("%s:%d FAILED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_UNKNOWN) {
            NETMGR_WIFI_LOGI("%s:%d UNKNOWN\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-r") == 0) {
        int fd;
        char buf[100];
        int read_len;

        fd = open(NETMGR_WIFI_CONF, O_RDONLY);
        if(fd < 0) {
            NETMGR_WIFI_LOGE("%s:%d open %s failed:%s\n", __func__, __LINE__, NETMGR_WIFI_CONF,strerror(errno));
            return ;
        }

        if (lseek(fd, 0, SEEK_SET) < 0) {
            close(fd);
            NETMGR_WIFI_LOGE("%s:%d lseek %s failed:%s\n", __func__, __LINE__, NETMGR_WIFI_CONF,strerror(errno));
            return;
        }

        read_len = read(fd, buf, sizeof(buf) - 1);
        while(read_len > 0) {
            buf[read_len] = '\0';
            NETMGR_WIFI_LOGD("%s", buf);
            read_len = read(fd, buf, sizeof(buf) - 1);
        }
        close(fd);
    } else if (strcmp(rtype, "-d") == 0) {
        int err;
        if((err = remove(NETMGR_WIFI_CONF)) == 0 &&
           (err = remove(NETMGR_WIFI_STATUS)) == 0 &&
           (err = remove(NETMGR_WIFI_TEMP_PATH)) == 0) {
            NETMGR_WIFI_LOGI("%s:%d remove success\n", __func__, __LINE__);
        } else {
            NETMGR_WIFI_LOGE("%s:%d remove failed errno=%d %s\n", __func__, __LINE__, errno, strerror(errno));
        }
    } else if (strcmp(rtype, "-w") == 0) {
        if(argc > 1) {
            int fd;
            const char* p = argv[1];
            bool is_backslash = false;

            fd = open(NETMGR_WIFI_CONF, O_RDWR | O_CREAT);
            if(fd < 0) {
                NETMGR_WIFI_LOGE("%s:%d open %s failed:%s\n", __func__, __LINE__, NETMGR_WIFI_CONF,strerror(errno));
                return ;
            }

            while(*p != '\0') {
                if(*p == '\\') {
                    if(is_backslash == true) {
                        write(fd, "\\", 1);
                    }
                    is_backslash = true;
                }
                else if((*p == 'n') && (is_backslash == true)) {
                    write(fd, "\n", 1);
                    is_backslash = false;
                }
                else {
                    if(is_backslash == true) {
                        write(fd, "\\", 1);
                        is_backslash = false;
                    }
                    write(fd, p, 1);
                }
                p ++;
            }
            write(fd, "\0", 1);
            NETMGR_WIFI_LOGI("%s:%d Write Config Success\n", __func__, __LINE__);

            if (lseek(fd, 0L, SEEK_SET) < 0) {
                NETMGR_WIFI_LOGE("%s:%d lseek %s failed:%s\n", __func__, __LINE__, NETMGR_WIFI_CONF,strerror(errno));
                close(fd);
                return;
            }
            close(fd);
        }
        else {
            NETMGR_WIFI_LOGE("%s:%d Invalid params\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-n") == 0) {
        if(argc == 1) {
            if(g_is_sntp_enabled == true) {
                NETMGR_WIFI_LOGI("%s:%d sntp is enabled\n", __func__, __LINE__);
            } else {
                NETMGR_WIFI_LOGI("%s:%d sntp is disabled\n", __func__, __LINE__);
            }
        }
        if((argc > 1) && ((strcmp(argv[1], "0") == 0) || (strcmp(argv[1], "1") == 0))) {
            if(atoi(argv[1]) == 1) {
                NETMGR_WIFI_LOGI("%s:%d enable sntp\n", __func__, __LINE__);
                if(g_is_sntp_enabled == false) {
                    wifi_sntp_task();
                }
                g_is_sntp_enabled = true;
            } else {
                NETMGR_WIFI_LOGI("%s:%d disable sntp\n", __func__, __LINE__);
                g_is_sntp_enabled = false;
            }
        }
    } else {
        handle_netmgr_wifi_help_cmd();
    }
}

int netmgr_wifi_set_auto_save_ap(bool auto_save_ap)
{
    g_is_auto_save_ap_config = auto_save_ap;

    return 0;
}

#if AOS_COMP_CLI
static void handle_netmgr_wifi_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";

    if (strcmp(rtype, "-t") == 0) {
        const char *type = argc > 2 ? argv[2] : "";

        if(type == NULL) {
            NETMGR_WIFI_LOGE("type is null\n");
        } else if(strcmp(type, "wifi") == 0) {
            wifi_handle_cmd(argc-3, &argv[3]);
        }
        else {
            NETMGR_WIFI_LOGE("type = %s not support\n", type);
        }
    }
}

static struct cli_command ncmd = {
    .name     = "netmgr",
    .help     = "netmgr [-t] wifi [-c|-e|-m|-g|-s|-p|-r|-d|-w]",
    .function = handle_netmgr_wifi_cmd,
};

void netmgr_wifi_cli_register(void)
{
    aos_cli_register_command(&ncmd);
}
#endif
