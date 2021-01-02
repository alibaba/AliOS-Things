/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

//#include <network/network.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wifi_service.h"
#include "netmgr.h"
#include "netmgr_conn.h"
#include "string_convert.h"
#include "aos/cli.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/netifapi.h"
#include "sntp/sntp.h"
#include "network/hal/wifi.h"
#include "posix/semaphore.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

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

#define WIFI_SERVICE_CONF    "/data/wifi.conf"
#define WIFI_SERVICE_TEMP_PATH    "/data/temp"
#define WIFI_SERVICE_TEMP_CONF    "/data/temp/temp.conf"
#define WIFI_SERVICE_STATUS  "/data/wifi_status"

#define CONFIG_ELEMENT_NUM      5
#define CONFIG_ELEMENT_SSID     "ssid"
#define CONFIG_ELEMENT_PWD      "password"
#define CONFIG_ELEMENT_BSSID    "bssid"
#define CONFIG_ELEMENT_FORMAT   "format"
#define CONFIG_ELEMENT_CHANNEL  "channel"

#ifndef IW_EXT_STR_RECONNECTION_FAIL
#define IW_EXT_STR_RECONNECTION_FAIL  "RECONNECTION FAILURE"
#endif

#define TAG "WIFI_SERVICE"
#define WIFI_SERVICE_LOGE(level, ...)      LOGE(TAG, level, ##__VA_ARGS__)
#define WIFI_SERVICE_LOGI(level, ...)      LOGI(TAG, level, ##__VA_ARGS__)
#define WIFI_SERVICE_LOGD(level, ...)      LOGD(TAG, level, ##__VA_ARGS__)

#define WPA_STATE_COMPLETED       "COMPLETED"
#define WPA_STATE_DISCONNECTED    "DISCONNECTED"
#define WPA_STATE_SCANNING        "SCANNING"
#define WPA_STATE_AUTHENTICATION  "AUTHENTICATION"
#define WPA_STATE_ASSOCIATING     "ASSOCIATING"
#define WPA_STATE_ASSOCIATED      "ASSOCIATED"
#define WPA_STATE_4WAY_HANDSHAKE  "4WAY_HANDSHAKE"
#define WPA_STATE_GROUP_HANDSHAKE "GROUP_HANDSHAKE"

#define WIFI_SERVICE_QUEUE_MAX_NUM 50

#define WIFI_SERVICE_DEFAULT_CONNECT_TIMEOUT 28000

#define WIFI_SERVICE_RETRY_MAX_NUM    8

#define WIFI_SERVICE_SNTP_PRI        32
#define WIFI_SERVICE_RECONNECT_PRI   32

#define WIFI_SERVICE_HANDSHAKE_FAILED_MAX_RETRY   4

#ifdef AOS_ACTIVATION_REPORT
#define WIFI_SERVICE_REPORT_PRI   25
#endif /* AOS_ACTIVATION_REPORT */
static int retry_backoff_time[WIFI_SERVICE_RETRY_MAX_NUM]= {2, 2, 2, 4, 4, 4, 8, 8};
const char* wificonfigsymbol[CONFIG_ELEMENT_NUM] = {
        CONFIG_ELEMENT_SSID,
        CONFIG_ELEMENT_PWD,
        CONFIG_ELEMENT_BSSID,
        CONFIG_ELEMENT_FORMAT,
        CONFIG_ELEMENT_CHANNEL,
};

static char *m_sntp_servaddr[SNTP_MAX_SERVERS] = {
        "cn.pool.ntp.org",
        "0.cn.pool.ntp.org",
        "1.cn.pool.ntp.org",
};

typedef struct wifi_service_msg_t
{
    hal_wifi_module_t* m;
    hal_wifi_event_t stat;
    void* arg;
} wifi_service_msg_t;

typedef enum WIFI_SERVICE_CONNECT_STATE
{
    WIFI_SERVICE_SCAN_START,
    WIFI_SERVICE_SCAN_SUCCESS,
    WIFI_SERVICE_SCAN_FAILED,
    WIFI_SERVICE_CONNECT_START,
    WIFI_SERVICE_CONNECT_SUCCESS,
    WIFI_SERVICE_CONNECT_FAILED
} WIFI_SERVICE_CONNECT_STATE;

typedef enum WIFI_CONFIG_READ_MODE {
    READ_ALL_CONFIG,
    READ_SPECIFIC_CONFIG
} WIFI_CONFIG_READ_MODE;

static netmgr_conn_t* g_wifi_conn_info = NULL;
static WIFI_SERVICE_CONNECT_STATE g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
static bool g_is_recvmsg_result_got = false;
static bool g_is_dhcp_got_ip = false;
static bool g_is_sntp_task_running = false;
static aos_queue_t      g_msg_queue;
static bool g_is_got_scan_request_cmd = false;

static int g_handshake_failed_retry = 0;

static int update_wifi_config(netmgr_conn_t* conn, const char *file_name);
static int get_wifi_config(const char *file_name, wifi_service_ap_config_t* saved_ap_conf, WIFI_CONFIG_READ_MODE mode);

static void wifi_indicate_event_handle(hal_wifi_module_t* m, hal_wifi_event_t event_cmd, void* arg);
static void wifi_service_event(netmgr_conn_t* conn, hal_wifi_event_t event, uint8_t* reason_code);
static void periodic_sntp_handle();
static void wifi_sntp_task(void);
static void wifi_dhcp_start(void);
#ifdef AOS_ACTIVATION_REPORT
static void wifi_activation_report_task(void);
#endif /* AOS_ACTIVATION_REPORT */
static void netmgr_stat_chg_event(hal_wifi_module_t *m, hal_wifi_event_t stat,
                                  void* arg);

static int wifi_service_queue_init()
{
    uint32_t size = sizeof(wifi_service_msg_t) * WIFI_SERVICE_QUEUE_MAX_NUM;
    wifi_service_msg_t * msg_buf = NULL;

    msg_buf = (wifi_service_msg_t *)aos_malloc(size);
    if (!msg_buf) {
        WIFI_SERVICE_LOGE("uart send buf allocate %lu fail!\n", size);
        goto err;
    }

    if (aos_queue_new(&g_msg_queue, msg_buf, size,
           sizeof(wifi_service_msg_t)) != 0) {
        WIFI_SERVICE_LOGE("uart send queue create fail!\n");
        goto err;
    }
    return 0;
err:
    aos_free(msg_buf);
    aos_queue_free(&g_msg_queue);
    return -1;
}

static int wifi_service_send_queue_deinit()
{
    wifi_service_msg_t *msg_buf = NULL;

    if (!aos_queue_is_valid(&g_msg_queue)) {
        return -1;
    }

    msg_buf = (wifi_service_msg_t *)aos_queue_buf_ptr(&g_msg_queue);
    aos_free(msg_buf);

    aos_queue_free(&g_msg_queue);

    return 0;
}

static void wifi_recvmsg_handle(void* arg)
{
    int              ret;
    unsigned int     size;
    wifi_service_msg_t msg;
    uint8_t          reason_code;

    (void) arg;
    WIFI_SERVICE_LOGI("%s:%d task start\n", __func__, __LINE__);

    while (true) {

        if(g_is_recvmsg_result_got == false) {
            WIFI_SERVICE_LOGE("%s:%d recvmsg task exit\n", __func__, __LINE__);
            goto exit;
        }

        if (!aos_queue_is_valid(&g_msg_queue)) {
            WIFI_SERVICE_LOGE("%s:%d Error msg queue invalid!\n", __func__, __LINE__);
            goto exit;
         }

         memset(&msg, 0, sizeof(wifi_service_msg_t));
         ret = aos_queue_recv(&g_msg_queue, AOS_WAIT_FOREVER, &msg, &size);
         if (ret != 0) {
            WIFI_SERVICE_LOGE("%s:%d Error msg queue recv, error_no %d\n", __func__, __LINE__, ret);
            goto exit;

         }

         if (size != sizeof(wifi_service_msg_t)) {
            WIFI_SERVICE_LOGE("%s:%d Error msg queue recv: msg size %lu is not valid\n", __func__, __LINE__, size);
            goto exit;
         }

         if(msg.arg != NULL) {
             reason_code = *((uint8_t*)msg.arg);
             WIFI_SERVICE_LOGI("%s:%d reason_code %d\n", __func__, __LINE__, reason_code);
             free(msg.arg);
             wifi_indicate_event_handle(msg.m, msg.stat, (void*)&reason_code);
         } else {
             wifi_indicate_event_handle(msg.m, msg.stat, NULL);
         }

    }
exit:
    WIFI_SERVICE_LOGI("%s:%d recvmsg task exits!\n", __func__, __LINE__);
    aos_task_exit(0);
}

static int wifi_recvmsg_task()
{
    if(g_is_recvmsg_result_got == false) {
        g_is_recvmsg_result_got = true;
        aos_task_new("wifi_recvmsg_task", wifi_recvmsg_handle, NULL, 4*1024);
        return 0;
    }
    else {
        WIFI_SERVICE_LOGE("wifi recv msg task is already runing\n");
        return -1;
    }
}

static void netmgr_ip_got_event(hal_wifi_module_t* m, hal_wifi_ip_stat_t* pnet,
                                void* arg)
{
    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, (unsigned long)pnet->ip);
}

static void netmgr_connect_fail_event(hal_wifi_module_t* m, int err, void* arg)
{
    WIFI_SERVICE_LOGE("%s:%d err=%d\n", __func__, __LINE__, err);
    if(err == RET_WIFI_INVALID_PASSWORD) {
        g_handshake_failed_retry = WIFI_SERVICE_HANDSHAKE_FAILED_MAX_RETRY;
        netmgr_stat_chg_event(m, NOTIFY_WIFI_HANDSHAKE_FAILED, arg);
    } else if(err == RET_WIFI_CONN_FAIL) {
        g_wifi_service_connect_state = WIFI_SERVICE_CONNECT_FAILED;
    } else if(err != RET_WIFI_OK) {
        netmgr_stat_chg_event(m, NOTIFY_WIFI_DISCONNECTED, arg);
    }
}

static void netmgr_stat_chg_event(hal_wifi_module_t* m, hal_wifi_event_t stat,
                                  void* arg)
{
    wifi_service_msg_t msg;
    int error;

    msg.m = m;
    msg.stat = stat;
    if(arg != NULL) {
        uint8_t* reason_code = (uint8_t *)malloc(sizeof(uint8_t));
        if(reason_code != NULL) {
            memcpy(reason_code, arg, sizeof(uint8_t));
            WIFI_SERVICE_LOGI("%s:%d reason_code %d\n", __func__, __LINE__, *reason_code);
            msg.arg = reason_code;
        }
        else {
            msg.arg = NULL;
        }
    } else {
        msg.arg = NULL;
    }

    if ((error = aos_queue_send(&g_msg_queue, &msg,
                         sizeof(msg))) != 0) {
        WIFI_SERVICE_LOGE("%s:%d aos queue send failed error=%d\n", __func__, __LINE__, error);
    }
    WIFI_SERVICE_LOGI("%s:%d stat %d\n", __func__, __LINE__, stat);
}

static void netmgr_scan_completed_event(hal_wifi_module_t*      m,
                                        hal_wifi_scan_result_t* result,
                                        void*                  arg)
{
    netmgr_conn_t* conn = g_wifi_conn_info;
    wifi_service_ap_config_t* saved_ap_conf;
    int i;
    int j;

    if(g_is_got_scan_request_cmd == true) {
        if((result != NULL)
          && (result->ap_num != 0)
          && (result->ap_list != NULL)) {
            for(i = 0; i < result->ap_num; i++) {
                WIFI_SERVICE_LOGD("%s:%d ssid=%s power=%d bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, __LINE__,
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
        }
        else {
            WIFI_SERVICE_LOGE("%s:%d scan failed\n", __func__, __LINE__);
        }
        g_is_got_scan_request_cmd = false;
        return ;
    }

    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
        return;
    }

    saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
    if(result == NULL) {
        WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
        g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
        return ;
    }

    if((result->ap_num <= 0) || (result->ap_list == NULL)) {
        WIFI_SERVICE_LOGE("%s:%d scan result ap_num is 0 or ap list is null\n", __func__, __LINE__);
        g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
        return ;
    }

    WIFI_SERVICE_LOGD("%s:%d ap_num=%d\n", __func__, __LINE__, result->ap_num);
    /* Found the most power ap info */
    for(i = 0; i < result->ap_num; i++) {
       for(j = 0; j < saved_ap_conf->ap_num; j++) {
            WIFI_SERVICE_LOGI("%s:%d config ssid=%s result ssid=%s power=%d bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, __LINE__,
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
            if(strcmp(result->ap_list[i].ssid, saved_ap_conf->config[j].ssid) != 0) {
               continue;
            }

            if(saved_ap_conf->config[j].ap_power == 0) {
               saved_ap_conf->config[j].ap_power = result->ap_list[i].ap_power;
               saved_ap_conf->config[j].sec_type = result->ap_list[i].sec_type;
               memcpy(saved_ap_conf->config[j].bssid, result->ap_list[i].bssid,
                       sizeof(saved_ap_conf->config[j].bssid));
               WIFI_SERVICE_LOGE("%s:%d found i=%d j=%d power=%d\n", __func__, __LINE__, i, j, saved_ap_conf->config[j].ap_power);
           }
           else {
               if(result->ap_list[i].ap_power > saved_ap_conf->config[j].ap_power) {
                   saved_ap_conf->config[j].ap_power = result->ap_list[i].ap_power;
                   saved_ap_conf->config[j].sec_type = result->ap_list[i].sec_type;
                   memset(saved_ap_conf->config[j].bssid, 0, sizeof(saved_ap_conf->config[j].bssid));
                   memcpy(saved_ap_conf->config[j].bssid, result->ap_list[i].bssid,
                       sizeof(saved_ap_conf->config[j].bssid));
                   WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
               }
           }
       }
    }

    g_wifi_service_connect_state = WIFI_SERVICE_SCAN_SUCCESS;
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);

}

static void netmgr_scan_adv_completed_event(hal_wifi_module_t*          m,
                                            hal_wifi_scan_result_adv_t* result,
                                            void*                       arg)
{
}

static void netmgr_para_chg_event(hal_wifi_module_t*      m,
                                  hal_wifi_ap_info_adv_t* ap_info, char* key,
                                  int key_len, void* arg)
{
    wifi_service_ap_config_t* saved_ap_conf;
    netmgr_conn_t* conn = g_wifi_conn_info;
    int i;

    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }

    if (!ap_info) {
        WIFI_SERVICE_LOGE("netmgr %s invalid ap_info pointer.", __func__);
        return;
    }

    WIFI_SERVICE_LOGI("netmgr %s %d, bssid: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
         __func__, __LINE__,
         (uint8_t)(ap_info->bssid[0]), (uint8_t)(ap_info->bssid[1]), (uint8_t)(ap_info->bssid[2]),
         (uint8_t)(ap_info->bssid[3]), (uint8_t)(ap_info->bssid[4]), (uint8_t)(ap_info->bssid[5]));

    saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        WIFI_SERVICE_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return ;
    }

    if((saved_ap_conf->used_ap > MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->used_ap < 0)) {
        WIFI_SERVICE_LOGE("%s:%d invalid used ap %d\n", __func__, __LINE__, saved_ap_conf->used_ap);
        return ;
    }

    i = saved_ap_conf->used_ap;

    WIFI_SERVICE_LOGI("%s:%d saved_ap_conf=%p bssid=%02x:%02x:%02x:%02x:%02x:%02x before\n",
            __func__, __LINE__, saved_ap_conf,
            saved_ap_conf->config[i].bssid[0] & 0xff,
            saved_ap_conf->config[i].bssid[1] & 0xff,
            saved_ap_conf->config[i].bssid[2] & 0xff,
            saved_ap_conf->config[i].bssid[3] & 0xff,
            saved_ap_conf->config[i].bssid[4] & 0xff,
            saved_ap_conf->config[i].bssid[5] & 0xff
            );
    /* Update bssid information here */
    memcpy(saved_ap_conf->config[i].bssid, ap_info->bssid,
           sizeof(saved_ap_conf->config[i].bssid));
    WIFI_SERVICE_LOGI("%s:%d saved_ap_conf=%p bssid=%02x:%02x:%02x:%02x:%02x:%02x before\n",
            __func__, __LINE__, saved_ap_conf,
            saved_ap_conf->config[i].bssid[0] & 0xff,
            saved_ap_conf->config[i].bssid[1] & 0xff,
            saved_ap_conf->config[i].bssid[2] & 0xff,
            saved_ap_conf->config[i].bssid[3] & 0xff,
            saved_ap_conf->config[i].bssid[4] & 0xff,
            saved_ap_conf->config[i].bssid[5]
            );
    update_wifi_config(conn, WIFI_SERVICE_CONF);
}

static void reconnect_task_handle(void* arg)
{
    wifi_service_ap_config_t* saved_ap_conf;
    netmgr_conn_t* conn = g_wifi_conn_info;

    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        conn->reconnect_task_running = false;
        return ;
    }

    saved_ap_conf = (wifi_service_ap_config_t*) conn->saved_config;
    if(saved_ap_conf == NULL) {
        WIFI_SERVICE_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return ;
    }
    memset(saved_ap_conf, 0, sizeof(wifi_service_ap_config_t));
    WIFI_SERVICE_LOGI("%s:%d used_ap=%d\n", __func__, __LINE__, saved_ap_conf->used_ap);
    if(-1 == get_wifi_config(WIFI_SERVICE_TEMP_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
        if(-1 == get_wifi_config(WIFI_SERVICE_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
            conn->reconnect_task_running = false;
            WIFI_SERVICE_LOGE("%s:%d no config found\n", __func__, __LINE__);
            return ;
        }
    }

    while(1) {

        WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);

        if(false == conn->auto_reconnect) {
            WIFI_SERVICE_LOGE("Not auto reconnect, break the loop\n");
            conn->reconnect_task_running = false;
            break;
        }

        WIFI_SERVICE_LOGI("%s:%d state=%d\n", __func__, __LINE__, conn->state);
        if(conn->state == CONN_STATE_DISCONNECTED) {
            WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
            if(0 != wifi_service_wifi_connect(NULL, NULL, NULL, 0)) {
                hal_wifi_module_t* m;
                WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
                m = hal_wifi_get_default_module();
                if(m == NULL) {
                    conn->reconnect_task_running = false;
                    WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
                    break;
                }

                hal_wifi_cancel(m);
                conn->reconnect_task_running = false;
                netmgr_conn_state_change(conn, CONN_STATE_FAILED);
                break;
            }
        }
        else if(conn->state == CONN_STATE_NETWORK_CONNECTED)
        {
            WIFI_SERVICE_LOGE("%s:%d connected\n", __func__, __LINE__);
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
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    if(conn->reconnect_task_running == true) {
        WIFI_SERVICE_LOGE("reconnect task is already running\n");
        return 0;
    }

    conn->reconnect_task_running = true;
    aos_task_new_ext(&reconnect_task, "reconnect_task", reconnect_task_handle, NULL, 4*1024, WIFI_SERVICE_RECONNECT_PRI);
    return 0;
}

static void netmgr_fatal_err_event(hal_wifi_module_t* m, void* arg) {
}

static const hal_wifi_event_cb_t g_wifi_conn_hal_event = {
    .connect_fail        = netmgr_connect_fail_event,
    .ip_got              = netmgr_ip_got_event,
    .stat_chg            = netmgr_stat_chg_event,
    .scan_compeleted     = netmgr_scan_completed_event,
    .scan_adv_compeleted = netmgr_scan_adv_completed_event,
    .para_chg            = netmgr_para_chg_event,
    .fatal_err           = netmgr_fatal_err_event,
};

static void sntp_config_servaddr(void)
{
    int i = 0;
    WIFI_SERVICE_LOGI("sntp config servadd start.");
    for (i = 0; i < SNTP_MAX_SERVERS; i++) {
        if (0 != sntp_set_server(i, m_sntp_servaddr[i])) {
            WIFI_SERVICE_LOGE("set sntp server:%s failed\n", m_sntp_servaddr[i]);
        }
        else {
            WIFI_SERVICE_LOGI("set sntp server:%s successfully\n", m_sntp_servaddr[i]);
        }
    }
    WIFI_SERVICE_LOGI("sntp config servadd end.");
}


static int wifi_connected(netmgr_conn_t* conn)
{
    hal_wifi_module_t* m;
    struct netif* net_if;

    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    m = hal_wifi_get_default_module();
    if(!m) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if(!net_if) {
        WIFI_SERVICE_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    if(!netif_is_up(net_if)) {
        WIFI_SERVICE_LOGE("%s:%d up netif\n", __func__, __LINE__);
        if(!netif_is_link_up(net_if)) {
            netifapi_netif_set_link_up(net_if);
        }
        if(!netif_is_up(net_if)) {
            netifapi_netif_set_up(net_if);
            netifapi_netif_set_default(net_if);
        }
    }

    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    return netmgr_conn_state_change(conn, CONN_STATE_OBTAINING_IP);
}

static int wifi_failed(netmgr_conn_t* conn)
{
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    WIFI_SERVICE_LOGI("%s:%d auto_reconnect=%d\n", __func__, __LINE__, conn->auto_reconnect);
    if(conn->auto_reconnect == true) {

        netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTING);
        /* connection state change */
        wifi_auto_reconnect_task();
    }
    else {
        netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
    }
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    return 0;
}

static int wifi_obtaining_ip(netmgr_conn_t* conn)
{
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    WIFI_SERVICE_LOGI("%s:%d start dhcp\n", __func__, __LINE__);
    wifi_dhcp_start();

    return 0;
}

#ifdef AOS_OPTIMIZE_NETWORK
#include "lwip/tcp.h"
/*  0:small rto 1:large rto */
extern int lwip_rto_flags;
/*  0:small rcv wnd 1:large rcv wnd */
extern int lwip_rcv_wnd_flags;

void wifi_link_check()
{
    int rssi;
    netmgr_conn_t* conn = g_wifi_conn_info;

    if(conn == NULL) {
        WIFI_SERVICE_LOGI("%s:%d conn is null\n", __func__, __LINE__);
        return ;
    }

    if(conn->state != CONN_STATE_NETWORK_CONNECTED) {
        WIFI_SERVICE_LOGI("%s:%d conn state %d is not network connected,exit\n", __func__, __LINE__, conn->state);
        return ;
    }

    wifi_service_get_rssi(&rssi);

    WIFI_SERVICE_LOGI("%s:%d rssi=%d flags=%d\n", __func__, __LINE__, rssi, lwip_rcv_wnd_flags);
    if((rssi < -60) && (lwip_rcv_wnd_flags == WND_FLAGS_LARGE)) {
        WIFI_SERVICE_LOGI("%s:%d rssi=%d\n", __func__, __LINE__, rssi);
        WIFI_SERVICE_LOGI("%s:%d weak\n", __func__, __LINE__);
        lwip_rto_flags = RTO_FLAGS_LARGE;
        lwip_rcv_wnd_flags = WND_FLAGS_SMALL;
    }

    if((rssi > -50) && (lwip_rcv_wnd_flags == WND_FLAGS_SMALL)) {
        WIFI_SERVICE_LOGI("%s:%d rssi=%d\n", __func__, __LINE__, rssi);
        WIFI_SERVICE_LOGI("%s:%d strong\n", __func__, __LINE__);
        lwip_rto_flags = RTO_FLAGS_SMALL;
        lwip_rcv_wnd_flags = WND_FLAGS_LARGE;
    }
}
#endif /* AOS_OPTIMIZE_NETWORK */

static int wifi_network_connected(netmgr_conn_t* conn)
{
#ifdef AOS_ACTIVATION_REPORT
    wifi_activation_report_task();
#endif /* AOS_ACTIVATION_REPORT */

#ifdef AOS_OPTIMIZE_NETWORK
    wifi_link_check();
#endif /* AOS_OPTIMIZE_NETWORK */
    return 0;
}

static int wifi_disconnecting(netmgr_conn_t* conn)
{
    hal_wifi_module_t* m;
    struct netif* net_if;

    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if(net_if != NULL) {
        WIFI_SERVICE_LOGI("%s:%d clear ip address\n", __func__, __LINE__);
        netifapi_netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    }

    netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
    return 0;
}

static int wifi_disconnected(netmgr_conn_t* conn)
{
    hal_wifi_module_t* m;
    struct netif* net_if;

    if(g_is_dhcp_got_ip == true) {
        g_is_dhcp_got_ip = false;
    }

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if(net_if != NULL) {
        if(netif_is_link_up(net_if)) {
            netifapi_netif_set_link_down(net_if);
        }
        if(netif_is_up(net_if)) {
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
        }
    }

    if((0 != hal_wifi_disconnect(m))
        && (0 != hal_wifi_suspend(m))) {
        WIFI_SERVICE_LOGI("%s:%d disconnect failed\n", __func__, __LINE__);
    }

    WIFI_SERVICE_LOGI("%s:%d do disconnect\n", __func__, __LINE__);
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

static void skip_line_end(FILE* stream)
{
    char buf[100];
    while (fgets(buf, sizeof(buf), stream)) {
        buf[sizeof(buf) - 1] = '\0';
        if (newline_terminated(buf, sizeof(buf)))
            return;
    }
}

static int read_config_value(const char* pos,
		const char* obj,int* line, wifi_service_ap_info_t* config)
{
    char* pos_start = NULL;
    char* pos_end = NULL;
    int len = 0;
    char t_str[18]={'\0'};

    WIFI_SERVICE_LOGI("%s:%d read ap config to ap info:%p\n", __func__, __LINE__, config);
    if((pos_start=strstr(pos,obj))!=NULL) {
        pos_start = strchr(pos_start,'"');
        if(NULL == pos_start) {
            WIFI_SERVICE_LOGE("Line %d:Invalid value line '%s'.",*line,pos_start);
            return -1;
        }
        pos_start++;
        pos_end = strchr(pos_start,'"');
        if(pos_end != NULL) {
            len = (pos_end - pos_start);
            WIFI_SERVICE_LOGI("%s:%d len :%d %s ", __func__, __LINE__, len, pos_start);
            if(strncmp(obj, "ssid", 4) == 0) {
                strncpy(config->ssid,pos_start,len);
	            config->ssid[len]='\0';
                WIFI_SERVICE_LOGI("%s:%d ssid=%s\n", __func__, __LINE__, config->ssid);
	        } else if(strncmp(obj, "password", 8) == 0) {
                strncpy(config->pwd, pos_start, len);
                config->pwd[len]='\0';
                WIFI_SERVICE_LOGI("%s:%d len=%d pwd=%s\n", __func__, __LINE__, len, config->pwd);
	        } else if(strncmp(obj,"bssid", 5) == 0) {
                memcpy(config->bssid, pos_start, len);
                config->pwd[len]='\0';
                WIFI_SERVICE_LOGI("%s:%d len=%d bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, __LINE__, len, config->bssid[0], config->bssid[1], config->bssid[2], config->bssid[3], config->bssid[4], config->bssid[5]);
	        } else if(strncmp(obj,"format", 6) == 0) {
                strncpy(t_str, pos_start, len);
                t_str[len]='\0';
                if(strncmp(t_str, "gbk", 3) == 0) {
                    strncpy(config->utf8_ssid, config->ssid, sizeof(config->utf8_ssid));
                    memset(config->ssid, 0, sizeof(config->ssid));
                    if((0 != string_convert((uint8_t *)config->ssid, MAX_SSID_SIZE, (uint8_t *)config->utf8_ssid, strlen(config->utf8_ssid), UTF8_TO_GBK))) {
                        WIFI_SERVICE_LOGI("%s:%d gbk ssid=%s\n", __func__, __LINE__);
                        config->is_ssid_gbk = 1;
                    }
                }
                WIFI_SERVICE_LOGI("%s:%d format=%s\n", __func__, __LINE__, t_str);
            } else if(strncmp(obj, "bssid", 5) == 0) {
                strncpy((char*)t_str, pos_start, len);
                t_str[len]='\0';
                WIFI_SERVICE_LOGI("%s:%d len=%d pwd=%s pwd_len=%d t_str=%s\n", __func__, __LINE__, len, config->pwd, ETH_ALEN, t_str);
                sscanf((char*)t_str,"%02s:%02s:%02s:%02s:%02s:%02s",
                    &config->bssid[0],
                    &config->bssid[1],
                    &config->bssid[2],
                    &config->bssid[3],
                    &config->bssid[4],
                    &config->bssid[5]);
                WIFI_SERVICE_LOGI("%s:%d pwd=%s t_str=%s\n", __func__, __LINE__, config->pwd, t_str);
            } else if(strncmp(obj,"channel",7) == 0) {
				strncpy(t_str,pos_start,len);
				t_str[len]='\0';
				config->channel = atoi((char*)t_str);
            }
        }else {
            WIFI_SERVICE_LOGE("%s:%d Line %d:invalid quotation '%s'.", __func__, __LINE__, *line, pos_start);
            return -1;
        }
    }
    return 0;
}

static char* get_config_line(char* s, int size, FILE* stream, int* line,
    char** _pos)
{
    char *pos, *end, *sstart;

    while (fgets(s, size, stream)) {
        (*line)++;
        s[size - 1] = '\0';
        if (!newline_terminated(s, size)) {
        /*
         * The line was truncated - skip rest of it to avoid
         * confusing error messages.
         */
            WIFI_SERVICE_LOGI("Long line in configuration file truncated");
            skip_line_end(stream);
         }
         pos = s;

	 /* Skip white space from the beginning of line. */
	 while (*pos == ' ' || *pos == '\t' || *pos == '\r') {
	    pos++;
         }

	 /* Skip comment lines and empty lines */
	 if (*pos == '#' || *pos == '\n' || *pos == '\0')
	    continue;

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

static int get_wifi_config(const char *file_name, wifi_service_ap_config_t* saved_ap_conf, WIFI_CONFIG_READ_MODE mode)
{
    FILE *fp = NULL;
    int i = 0;
    int j=0;
    int line = 0;
    char *pos = NULL;
    char buf[256];
    int len;

    WIFI_SERVICE_LOGD("%s:%d saved_ap_conf %p", __func__, __LINE__, saved_ap_conf);
    if(saved_ap_conf == NULL) {
        WIFI_SERVICE_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return -1;
    }

    if((saved_ap_conf->ap_num > MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->ap_num < 0)) {
        WIFI_SERVICE_LOGE("%s:%d error used ap idx: %d ap_num: %d max config num: %d\n", __func__, __LINE__, saved_ap_conf->used_ap, saved_ap_conf->ap_num,  MAX_AP_CONFIG_NUM);
        return -1;
    }

    fp = fopen(file_name,"r");
    if(NULL == fp) {
        WIFI_SERVICE_LOGE("%s:%d open %s failed:%s\n", __func__, __LINE__, file_name,strerror(errno));
        return -1;
    }

    i = saved_ap_conf->ap_num;
    while(get_config_line(buf, sizeof(buf), fp, &line, &pos)) {
        wifi_service_ap_info_t ap_info;

        memset(&ap_info, 0, sizeof(wifi_service_ap_info_t));
        WIFI_SERVICE_LOGD("%s:%d pos: %s", __func__, __LINE__, pos);
        if(strstr(pos, "network={") != NULL) {
            while(get_config_line(buf, sizeof(buf), fp, &line, &pos)) {

                WIFI_SERVICE_LOGD("%s:%d pos:%s", __func__, __LINE__, pos);
                if(strncmp(pos,"}", 1) == 0) {
                    break;
                }
                for(j = 0; j < CONFIG_ELEMENT_NUM; j++) {
                    len = strlen(wificonfigsymbol[j]);
                    WIFI_SERVICE_LOGD("%s:%d %s, %d", __func__, __LINE__, wificonfigsymbol[j], len);
                    if(strncmp(pos, wificonfigsymbol[j], len) != 0 ||
                       pos[len] != '='){
                        continue;
                    }

                    if(read_config_value(pos, wificonfigsymbol[j], &line,
                       &ap_info)) {
                        WIFI_SERVICE_LOGE("%s:%d\n failed to line:%d parse: %s", __func__, __LINE__, line, pos);
                        continue;
                    }
                    WIFI_SERVICE_LOGD("%s:%d symbol[%d]=%s\n", __func__, __LINE__, j, wificonfigsymbol[j]);
                }
            }
            WIFI_SERVICE_LOGD("%s:%d ssid=%s\n", __func__, __LINE__, ap_info.ssid);

            if(saved_ap_conf->ap_num != 0) {
                int k;
                for(k = 0; k < saved_ap_conf->ap_num; k++) {
                    WIFI_SERVICE_LOGD("%s:%d k=%d ssid=%s ap info ssid=%s\n", __func__, __LINE__, k, saved_ap_conf->config[k].ssid, ap_info.ssid);
                    if(0 == strcmp(saved_ap_conf->config[k].ssid, ap_info.ssid)) {
                        if(saved_ap_conf->config[k].channel == 0) {
                            WIFI_SERVICE_LOGI("%s:%d update config[%d] with cfg channel=%d\n", __func__, __LINE__, k, ap_info.channel);
                            saved_ap_conf->config[k].channel = ap_info.channel;
                        }
                        break;
                    }
                }

                if((mode == READ_ALL_CONFIG) && (k == saved_ap_conf->ap_num)){
                    if(i < MAX_AP_CONFIG_NUM) {
                        memcpy(&saved_ap_conf->config[i], &ap_info, sizeof(wifi_service_ap_info_t));
                        i++;
                        WIFI_SERVICE_LOGI("%s:%d increase ap_num=%d\n", __func__, __LINE__, i);
                    }
                    else {
                        WIFI_SERVICE_LOGI("%s:%d ap num reached max %d\n", __func__, __LINE__, i);
                    }
                }
                else {
                    WIFI_SERVICE_LOGD("%s:%d k=%d ap_num=%d\n", __func__, __LINE__, k, saved_ap_conf->ap_num);
                }
            }
            else {
                if(i < MAX_AP_CONFIG_NUM) {
                    memcpy(&saved_ap_conf->config[i], &ap_info, sizeof(wifi_service_ap_info_t));
                    i++;
                    WIFI_SERVICE_LOGI("%s:%d increase ap_num=%d\n", __func__, __LINE__, i);
                }
                else {
                    WIFI_SERVICE_LOGI("%s:%d ap num reached max %d\n", __func__, __LINE__, i);
                }
            }

            WIFI_SERVICE_LOGD("%s:%d index:%d\n", __func__, __LINE__, i);
            if(i >= MAX_AP_CONFIG_NUM) {
                WIFI_SERVICE_LOGI("%s:%d ap num reached max %d\n", __func__, __LINE__,  i);
                break;
            }
        }
    }
    saved_ap_conf->ap_num = i;
    WIFI_SERVICE_LOGI("%s:%d ap_num=%d\n", __func__, __LINE__, saved_ap_conf->ap_num);
    fclose(fp);
    return 0;
}

#ifdef AOS_ACTIVATION_REPORT
#include "activation.h"
static void wifi_activation_report_handle(void* arg)
{
    (void) arg;

    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    activation_report();

}

static void wifi_activation_report_task()
{
    WIFI_SERVICE_LOGI("start report device info.");
    aos_task_new("activation_report_task", wifi_activation_report_handle, NULL, 4*1024);
}
#endif /* AOS_ACTIVATION_REPORT */

#define WIFI_SERVICE_CONFIG_LEN 512
static int update_wifi_config(netmgr_conn_t* conn, const char *file_name)
{
    int i = 0;
    char buf[WIFI_SERVICE_CONFIG_LEN];
    int len = 0;
    int channel = 0;
    char *ssid = NULL;
    char *format = NULL;
    FILE *temp_fp = NULL;
    wifi_service_ap_config_t* saved_ap_conf;
    hal_wifi_module_t *  m;

    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    if(-1 == (channel = hal_wifi_get_channel(m))) {
        WIFI_SERVICE_LOGE("%s:%d get channel failed\n", __func__, __LINE__);
        return -1;
    }

    saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        WIFI_SERVICE_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return -1;
    }

    /* check gbk and utf8 when ssid contain chinese */
    if((saved_ap_conf->used_ap > MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->used_ap < 0)
        ||(saved_ap_conf->ap_num > MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->ap_num <= 0)) {
        WIFI_SERVICE_LOGE("%s:%d error used ap idx: %d ap_num: %d max config num: %d\n", __func__, __LINE__, saved_ap_conf->used_ap, saved_ap_conf->ap_num,  MAX_AP_CONFIG_NUM);
        return -1;
    }

    i = saved_ap_conf->used_ap;
    WIFI_SERVICE_LOGE("%s:%d used_ap=%d\n", __func__, __LINE__, i);

    if(i != 0) {
       /* move used ap info to the head of the array */
            memset(&saved_ap_conf->config[0], 0, sizeof(saved_ap_conf->config[0]));
            memcpy(&saved_ap_conf->config[0], &saved_ap_conf->config[i], sizeof(saved_ap_conf->config[0]));
        }

        saved_ap_conf->used_ap = 0;
    i = saved_ap_conf->used_ap;
    saved_ap_conf->config[i].channel = channel;

#if (MAX_AP_CONFIG_NUM > 1)
        memset(&saved_ap_conf->config[1], 0, sizeof(saved_ap_conf->config[1])* (saved_ap_conf->ap_num - 1));
#endif
    saved_ap_conf->ap_num = 1;

    if(-1 == get_wifi_config(file_name, saved_ap_conf, READ_ALL_CONFIG)) {
        if(-1 == get_wifi_config(WIFI_SERVICE_TEMP_CONF, saved_ap_conf, READ_ALL_CONFIG)) {
            WIFI_SERVICE_LOGE("%s:%d no temp config found\n", __func__, __LINE__);
        } else {
            WIFI_SERVICE_LOGE("%s:%d no config found\n", __func__, __LINE__);
        }
    }

    if((0 != mkdir(WIFI_SERVICE_TEMP_PATH, 0644)) && (errno != EEXIST)) {
        WIFI_SERVICE_LOGE("mkdir %s failed:%s\n", WIFI_SERVICE_TEMP_PATH, strerror(errno));
        return -1;
    }

    temp_fp = fopen(WIFI_SERVICE_TEMP_CONF,"a");
    if(NULL == temp_fp) {
        WIFI_SERVICE_LOGE("open %s failed:%s\n", WIFI_SERVICE_TEMP_CONF, strerror(errno));
        return -1;
    }

    WIFI_SERVICE_LOGI("%s:%d ap_num=%d\n", __func__, __LINE__, saved_ap_conf->ap_num);
    for(i= 0; i < saved_ap_conf->ap_num; i++) {
        if(0 == saved_ap_conf->config[i].is_ssid_gbk) {
            ssid = saved_ap_conf->config[i].ssid;
            format = "utf8";
        } else {
            ssid = saved_ap_conf->config[i].utf8_ssid;
            format = "gbk";
        }

        snprintf(buf,WIFI_SERVICE_CONFIG_LEN,"network={\n \
ssid=\"%s\"\n \
password=\"%s\"\n \
bssid=%02x:%02x:%02x:%02x:%02x:%02x\n\
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
              format,
              saved_ap_conf->config[i].channel);

        len = strlen(buf);
        WIFI_SERVICE_LOGI("saved_ap_conf=%p wifi config buff len :%d\n",saved_ap_conf, len);
        WIFI_SERVICE_LOGI("saving wifi config file:%s\n",buf);
        if(1 != fwrite(buf,len,1,temp_fp)) {
            WIFI_SERVICE_LOGE("fwrite:%s\n",strerror(errno));
        }
    }
    fclose(temp_fp);

    if(rename(WIFI_SERVICE_TEMP_CONF, WIFI_SERVICE_CONF) == 0) {
        WIFI_SERVICE_LOGE("rename:%s\n",strerror(errno));
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

static int update_wifi_status(netmgr_conn_t* conn, const char *file_name, char* wpa_status)
{
    int i = 0;
    char buf[WIFI_SERVICE_CONFIG_LEN];
    int len = 0;
    int channel = 0;
    int freq = 0;
    FILE *fp = NULL;
    wifi_service_ap_config_t* saved_ap_conf;
    hal_wifi_module_t *  m;
    struct netif* net_if;
    uint8_t mac[6];
    char *ssid = NULL;

    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    if(-1 == (channel = hal_wifi_get_channel(m))) {
        WIFI_SERVICE_LOGE("%s:%d get channel failed\n", __func__, __LINE__);
        return -1;
    }

    unlink(file_name);

    fp = fopen(file_name,"a");
    if(NULL == fp) {
        WIFI_SERVICE_LOGE("open %s failed:%s\n", file_name, strerror(errno));
        return -1;
    }

    freq = channel_to_frequency(channel);

    wifi_service_get_mac(mac);

    saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        fclose(fp);
        WIFI_SERVICE_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
        return -1;
    }

    if((saved_ap_conf->used_ap > MAX_AP_CONFIG_NUM)
        ||(saved_ap_conf->used_ap < 0)) {
        fclose(fp);
        WIFI_SERVICE_LOGE("%s:%d invalid used ap %d\n", __func__, __LINE__, saved_ap_conf->used_ap);
        return -1;
    }

    i = saved_ap_conf->used_ap;
    if(i == -1) {
        fclose(fp);
        WIFI_SERVICE_LOGE("%s:%d invalid used_ap=%d\n", __func__, __LINE__, i);
        return -1;
    }
    if(0 == saved_ap_conf->config[i].is_ssid_gbk) {
        ssid = saved_ap_conf->config[i].ssid;
    } else {
        ssid = saved_ap_conf->config[i].utf8_ssid;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if((!net_if) || (!netif_is_up(net_if))) {
        WIFI_SERVICE_LOGI("%s:%d no ip config use 0.0.0.0\n", __func__, __LINE__);
        snprintf(buf,WIFI_SERVICE_CONFIG_LEN,"\
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
        snprintf(buf,WIFI_SERVICE_CONFIG_LEN,"\
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
    WIFI_SERVICE_LOGI("saved_ap_conf=%p wifi config buff len :%d\n",saved_ap_conf, len);
    WIFI_SERVICE_LOGI("saving file:%s\n",buf);
    if(1 != fwrite(buf,len,1,fp)) {
        WIFI_SERVICE_LOGE("fwrite:%s\n",strerror(errno));
    }

    fclose(fp);
    return 0;

}

static void wifi_status_change(netmgr_conn_t *conn, hal_wifi_event_t event, uint8_t* reason_code)
{
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }

    WIFI_SERVICE_LOGI("%s:%d event=%d conn->state=%d\n", __func__, __LINE__, event, conn->state);
    if(event == NOTIFY_WIFI_SCAN_STARTED) {
        //update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_SCANNING);
    }
    else if(event == NOTIFY_WIFI_AUTHENTICATING) {
        //update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_AUTHENTICATION);
    }
    else if(event == NOTIFY_WIFI_ASSOCIATING) {
        //update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_ASSOCIATING);
    }
    else if(event == NOTIFY_WIFI_ASSOCIATED) {
        //update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_ASSOCIATED);
    }
    else if(event == NOTIFY_WIFI_4WAY_HANDSHAKE) {
        //update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_4WAY_HANDSHAKE);
    }
    else if(event == NOTIFY_WIFI_4WAY_HANDSHAKE_DONE) {
        g_handshake_failed_retry = 0;
    }
    else if(event == NOTIFY_WIFI_GROUP_HANDSHAKE) {
        //update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_GROUP_HANDSHAKE);
    }
    else if(event == NOTIFY_WIFI_GROUP_HANDSHAKE_DONE) {
    }
    else if(event == NOTIFY_WIFI_CONNECTED) {
        WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
        g_wifi_service_connect_state = WIFI_SERVICE_CONNECT_SUCCESS;
        netmgr_conn_state_change(conn, CONN_STATE_CONNECTED);
    }
    else if(event == NOTIFY_DHCP_SUCCESS) {
        wifi_sntp_task();
        update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_COMPLETED);
        netmgr_conn_state_change(conn, CONN_STATE_NETWORK_CONNECTED);
        WIFI_SERVICE_LOGD("%s:%d network connected\n", __func__, __LINE__);
    }
    else if(event == NOTIFY_SNTP_SUCCESS) {
        WIFI_SERVICE_LOGD("%s:%d SNTP success\n", __func__, __LINE__);
    }
    else if(event == NOTIFY_SNTP_FAILED) {
        WIFI_SERVICE_LOGI("%s:%d SNTP FAILED\n", __func__, __LINE__);
    }
    else if(event == NOTIFY_WIFI_HANDSHAKE_FAILED) {
        g_handshake_failed_retry ++;
        if(g_handshake_failed_retry >= WIFI_SERVICE_HANDSHAKE_FAILED_MAX_RETRY) {
            WIFI_SERVICE_LOGD("%s:%d handshake failed %d times\n", __func__, __LINE__, g_handshake_failed_retry);
            /* wrong password just return, don't reconnect */
            netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTING);
        } else {
            WIFI_SERVICE_LOGD("%s:%d handshake failed %d times\n", __func__, __LINE__, g_handshake_failed_retry);
            g_wifi_service_connect_state = WIFI_SERVICE_CONNECT_FAILED;
        }
    }
    else {
        if((event == NOTIFY_WIFI_DISCONNECTED)
                && (conn->state != CONN_STATE_CONNECTING)) {
            update_wifi_status(conn, WIFI_SERVICE_STATUS, WPA_STATE_DISCONNECTED);
        }

        WIFI_SERVICE_LOGD("%s:%d event=%d stat=%d\n", __func__, __LINE__, event, conn->state);
        if(conn->state == CONN_STATE_CONNECTING) {
            if(event == NOTIFY_WIFI_SCAN_FAILED) {
                g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
            }
            else {
                g_wifi_service_connect_state = WIFI_SERVICE_CONNECT_FAILED;
            }
        }
        else if((conn->state != CONN_STATE_DISCONNECTING)
         && (conn->state != CONN_STATE_DISCONNECTED)) {
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
            WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
        }
    }
    if((event != NOTIFY_WIFI_HANDSHAKE_FAILED)
        || (g_handshake_failed_retry >= WIFI_SERVICE_HANDSHAKE_FAILED_MAX_RETRY)) {
        wifi_service_event(conn, event, reason_code);
    }
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
}

static void wifi_service_event(netmgr_conn_t* conn, hal_wifi_event_t event, uint8_t* reason_code)
{
    wifi_service_data_t msg_data = {
		.id = WIFI_SERVICE_MSG_ID_WIFI_STATUS_FROM_IMPL,
    };
    int i;

    WIFI_SERVICE_LOGD("%s:%d event=%d reason_code=%d\n", __func__, __LINE__, event, *reason_code);
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }

    if(event >= NOTIFY_WIFI_DISCONNECTED && event < NOTIFY_MAX) {
        msg_data.data.wlan_status = event;

        if(conn->msg_cb) {
            WIFI_SERVICE_LOGI("%s:%d msg_cb event=%d\n", __func__, __LINE__, event);
            conn->msg_cb(&msg_data);

            if((event == NOTIFY_WIFI_DISCONNECTED) ||
               (event == NOTIFY_DHCP_SUCCESS)) {
                wifi_service_network_status_change_t change = {0};
                char *wifi_status;
                wifi_service_ap_config_t* saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
                i = saved_ap_conf->used_ap;

                wifi_service_get_rssi(&change.quantity);
                WIFI_SERVICE_LOGI("%s:%d quantity=%d event=%d\n", __func__, __LINE__, change.quantity, event);
                if(event == NOTIFY_WIFI_DISCONNECTED) {
                    wifi_status = "disconnect";
                }
                else {
                    wifi_status = "connect";
                }
                WIFI_SERVICE_LOGD("%s:%d saved_ap_conf=%p\n", __func__, __LINE__, saved_ap_conf);
                strncpy(change.status, wifi_status, strlen(wifi_status));
                WIFI_SERVICE_LOGD("%s:%d saved_ap_conf->ssid=%s\n", __func__, __LINE__, saved_ap_conf->config[i].ssid);
                if(saved_ap_conf->config[i].is_ssid_gbk == 0) {
                    strncpy(change.ssid, saved_ap_conf->config[i].ssid, strlen(saved_ap_conf->config[i].ssid));
                } else {
                    strncpy(change.ssid, saved_ap_conf->config[i].utf8_ssid, strlen(saved_ap_conf->config[i].utf8_ssid));
                }
                WIFI_SERVICE_LOGD("%s:%d change.ssid=%s\n", __func__, __LINE__, change.ssid);
                WIFI_SERVICE_LOGD("%s:%d saved_ap_conf->pwd=%s\n", __func__, __LINE__, saved_ap_conf->config[i].pwd);
                strncpy(change.password, saved_ap_conf->config[i].pwd, strlen(saved_ap_conf->config[i].pwd));
                if(reason_code != NULL) {
                    change.reason_code = *reason_code;
                } else {
                    change.reason_code = 0;
                }
                WIFI_SERVICE_LOGI("%s:%d reason_code %d\n", __func__, __LINE__, change.reason_code);
                msg_data.id = WIFI_SERVICE_MSG_ID_NETWORK_STATUS;
                msg_data.data.network_status_change = &change;
                WIFI_SERVICE_LOGD("%s:%d status=%s ssid=%s password=%s\n", __func__, __LINE__, change.status, change.ssid, change.password);
                conn->msg_cb(&msg_data);

            }
        }
    }
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
}

static void wifi_indicate_event_handle(hal_wifi_module_t* m, hal_wifi_event_t event_cmd, void* arg)
{
    WIFI_SERVICE_LOGD("%s:%d event_cmd=%d\n", __func__, __LINE__, event_cmd);
    switch(event_cmd)
    {
        case NOTIFY_WIFI_DISCONNECTED:
            WIFI_SERVICE_LOGD("Disconnection indication received");
            break;
        case NOTIFY_WIFI_CONNECTED:
		    WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_CONNECTED");
            break;
        case NOTIFY_WIFI_4WAY_HANDSHAKE_DONE:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_FOURWAY_HANDSHAKE_DONE");
            break;
        case NOTIFY_WIFI_NETWORK_NOT_FOUND:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_NO_NETWORK");
            break;
        case NOTIFY_WIFI_SCAN_STARTED:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_SCAN_START");
            break;
#if 0
        case WIFI_SERVICE_EVENT_SCAN_DONE:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_SCAN_DONE");
            break;
#endif
        case NOTIFY_WIFI_SCAN_FAILED:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_SCAN_FAILED");
            break;
        case NOTIFY_WIFI_AUTHENTICATING:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_AUTHENTICATION");
            break;
        case NOTIFY_WIFI_AUTH_REJECT:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_AUTH_REJECT");
            break;
        case NOTIFY_WIFI_DEAUTH:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_DEAUTH");
            break;
        case NOTIFY_WIFI_AUTH_TIMEOUT:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_AUTH_TIMEOUT");
            break;
        case NOTIFY_WIFI_ASSOCIATING:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_ASSOCIATING");
            break;
        case NOTIFY_WIFI_ASSOCIATED:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_ASSOCIATED");
            break;
        case NOTIFY_WIFI_ASSOC_REJECT:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_ASSOC_REJECT");
            break;
        case NOTIFY_WIFI_ASSOC_TIMEOUT:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_ASSOC_TIMEOUT");
            break;
#if 0
        case WIFI_SERVICE_EVENT_CHALLENGE_FAIL:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_CHALLENGE_FAIL");
            break;
#endif
        case NOTIFY_WIFI_HANDSHAKE_FAILED:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_HANDSHAKE_FAILED");
            break;
        case NOTIFY_WIFI_4WAY_HANDSHAKE:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_4WAY_HANDSHAKE");
            break;
        case NOTIFY_WIFI_GROUP_HANDSHAKE:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_GROUP_HANDSHAKE");
            break;
        case NOTIFY_WIFI_GROUP_HANDSHAKE_DONE:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_EVENT_GROUP_HANDSHAKE_DONE");
            break;
        case NOTIFY_WIFI_CONN_TIMEOUT:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_CONN_TIMEOUT");
            break;
        case NOTIFY_DHCP_SUCCESS:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_DHCP_SUCCESS");
            break;
        case NOTIFY_SNTP_SUCCESS:
            WIFI_SERVICE_LOGD("WIFI_SERVICE_SNTP_SUCCESS");
            break;
        default:
            WIFI_SERVICE_LOGD("not support event=%d\n", event_cmd);
            break;
    }

    if(event_cmd >= NOTIFY_WIFI_DISCONNECTED && (event_cmd < NOTIFY_MAX)) {
        netmgr_conn_t* conn = g_wifi_conn_info;
#if 0
        if(event_cmd == NOTIFY_WIFI_HANDSHAKE_FAILED ||
            event_cmd == NOTIFY_WIFI_NETWORK_NOT_FOUND) {
            WIFI_SERVICE_LOGE("%s:%d event_cmd=%d\n", __func__, __LINE__, event_cmd);
            wifi_service_event(event_cmd, NULL);
            msg_cb_enable = false;
        }else {
            WIFI_SERVICE_LOGE("%s:%d event_cmd=%d\n", __func__, __LINE__, event_cmd);
            if(msg_cb_enable && event_cmd != NOTIFY_MAX) {
            WIFI_SERVICE_LOGE("%s:%d event_cmd=%d\n", __func__, __LINE__, event_cmd);
                wifi_service_event(event_cmd, NULL);
            }
        }
#endif
        if(conn == NULL) {
            WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
            return;
        }

        WIFI_SERVICE_LOGI("%s:%d event_cmd=%d\n", __func__, __LINE__, event_cmd);
        wifi_status_change(conn, event_cmd, (uint8_t*)arg);

    }
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
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
        WIFI_SERVICE_LOGI("sntp getting time.");
        sntp_config_servaddr();
        if (0 == sntp_get_time(&m_sntp_arg, &ntp_time)) {
            WIFI_SERVICE_LOGI("[sntp] OK: sec %ld usec %ld\n", ntp_time.tv_sec, ntp_time.tv_usec);
            return true;
        } else if((g_wifi_conn_info->state != CONN_STATE_CONNECTED)
        && (g_wifi_conn_info->state != CONN_STATE_OBTAINING_IP)
        && (g_wifi_conn_info->state != CONN_STATE_NETWORK_CONNECTED)) {
            WIFI_SERVICE_LOGE("sntp task quit round for network is disconnected\n");
            return false;
        }
        else {
            int retry_time = retry_backoff_time[round];
            WIFI_SERVICE_LOGI("[sntp] wait for sntp done...e\n");
            if(round < WIFI_SERVICE_RETRY_MAX_NUM - 1) {
            round ++;
            }
            aos_msleep(retry_time * 1000);
        }
    }
}

static void tcpip_dhcpc_cb(struct netif *net_if) {
    struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(net_if, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

    if (netif_is_up(net_if) && (dhcp->state == DHCP_STATE_BOUND) && !ip_addr_isany(&net_if->ip_addr)) {
        int ret;
        hal_wifi_ip_stat_t in;
        hal_wifi_module_t*  m;

        m = hal_wifi_get_default_module();

        WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
        if( m == NULL ) {
            WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
            return ;
        }
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

        ret = hal_wifi_set_ip_stat(m, &in, STATION);
        if(ret != 0) {
            WIFI_SERVICE_LOGE("%s:%d set ip stat failed\n", __func__, __LINE__);
        }

        WIFI_SERVICE_LOGI("\r\n IPv4 Address     : %hhu.%hhu.%hhu.%hhu\r\n",
               ((u8_t *)&net_if->ip_addr.addr)[0],
               ((u8_t *)&net_if->ip_addr.addr)[1],
               ((u8_t *)&net_if->ip_addr.addr)[2],
               ((u8_t *)&net_if->ip_addr.addr)[3]);
        WIFI_SERVICE_LOGI(" IPv4 Subnet mask : %hhu.%hhu.%hhu.%hhu\r\n",
               ((u8_t *)&net_if->netmask.addr)[0],
               ((u8_t *)&net_if->netmask.addr)[1],
               ((u8_t *)&net_if->netmask.addr)[2],
               ((u8_t *)&net_if->netmask.addr)[3]);
        WIFI_SERVICE_LOGI(" IPv4 Gateway     : %hhu.%hhu.%hhu.%hhu\r\n\r\n",
               ((u8_t *)&net_if->gw.addr)[0],
               ((u8_t *)&net_if->gw.addr)[1],
               ((u8_t *)&net_if->gw.addr)[2],
               ((u8_t *)&net_if->gw.addr)[3]);
        netmgr_stat_chg_event(m, NOTIFY_DHCP_SUCCESS, NULL);
	    if (m != NULL && m->ev_cb != NULL && m->ev_cb->ip_got != NULL) {
		    hal_wifi_ip_stat_t net = {0};
            memcpy(net.ip, &(net_if->ip_addr.addr), sizeof(net_if->ip_addr.addr));
		    m->ev_cb->ip_got(m, &in, NULL);
		}
        g_is_dhcp_got_ip = true;
    }
    WIFI_SERVICE_LOGI("%s: dhcp state =%d", __func__, dhcp->state);
}

static void periodic_sntp_handle()
{

    hal_wifi_module_t*  m;

    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    m = hal_wifi_get_default_module();
    if( m == NULL ) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return ;
    }
    if(sntp_start() == true) {
        netmgr_stat_chg_event(m, NOTIFY_SNTP_SUCCESS, NULL);
        WIFI_SERVICE_LOGI("%s:%d post sntp action\n", __func__, __LINE__);
        aos_post_delayed_action(SNTP_PERIODIC_SYNC_INTERVAL_MS, periodic_sntp_handle, NULL);
    }
    else {
        netmgr_stat_chg_event(m, NOTIFY_SNTP_FAILED, NULL);
        WIFI_SERVICE_LOGI("%s:%d cancel sntp action\n", __func__, __LINE__);
        aos_cancel_delayed_action(SNTP_PERIODIC_SYNC_INTERVAL_MS, periodic_sntp_handle, NULL);
    }

}

static void wifi_sntp_handle(void* arg)
{
    (void) arg;

    g_is_sntp_task_running = true;

    WIFI_SERVICE_LOGI("%s:%d start sntp action\n", __func__, __LINE__);
    periodic_sntp_handle();

    g_is_sntp_task_running = false;

}

static void wifi_sntp_task(void)
{
    aos_task_t  sntp_task;

    if(g_is_sntp_task_running == false) {
        WIFI_SERVICE_LOGI("start sntp task.\n");
        aos_task_new_ext(&sntp_task, "sntp_task", wifi_sntp_handle, NULL, 4*1024, WIFI_SERVICE_SNTP_PRI);
    } else {
        WIFI_SERVICE_LOGE("sntp task is already running.\n");
    }
}

static void wifi_dhcp_start(void )
{
    struct netif *net_if = NULL;
    hal_wifi_event_t event = NOTIFY_DHCP_START_FAILED;
    hal_wifi_module_t *  m;

    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        goto end;
    }

    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    net_if = hal_wifi_get_netif(m, STATION);
    if(!net_if) {
        WIFI_SERVICE_LOGE("get net interface failed\n");
        goto end;
    }
    netifapi_netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);

    if (netif_is_up(net_if)) {
        WIFI_SERVICE_LOGI("%s:%d start dhcp\n", __func__, __LINE__);
        if(netifapi_dhcp_start(net_if) != 0) {
            WIFI_SERVICE_LOGE("%s:%d start dhcp failed.\n", __func__, __LINE__);
            goto end;
        }
        netif_set_status_callback(net_if, tcpip_dhcpc_cb);
    }
    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    return ;
end:
    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    netmgr_stat_chg_event(m, event, NULL);
    return ;
}

int wifi_service_init(void)
{
    int err;
    hal_wifi_module_t *module;
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    /* init wifi hal */
    err = hal_wifi_init();
    if( err != 0) {
        WIFI_SERVICE_LOGE("wifi service init failed.");
        return err;
    }

    if(g_wifi_conn_info != NULL) {
        WIFI_SERVICE_LOGE("wifi service has already initialized.");
        return 0;
    }
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    /* init connection info */
    g_wifi_conn_info = netmgr_conn_init(INTERFACE_WIFI);
    if(g_wifi_conn_info == NULL) {
        WIFI_SERVICE_LOGE("wifi service init failed.");
        return -1;
    }

    g_wifi_conn_info->auto_reconnect = false;

    g_wifi_conn_info->connected_cb = wifi_connected;
    g_wifi_conn_info->failed_cb = wifi_failed;
    g_wifi_conn_info->obtaining_ip_cb = wifi_obtaining_ip;
    g_wifi_conn_info->network_connected_cb = wifi_network_connected;
    g_wifi_conn_info->disconnecting_cb = wifi_disconnecting;
    g_wifi_conn_info->disconnected_cb = wifi_disconnected;

    g_wifi_conn_info->saved_config = (wifi_service_ap_config_t*)malloc(sizeof(wifi_service_ap_config_t));

    if(g_wifi_conn_info->saved_config == NULL) {
        WIFI_SERVICE_LOGE("malloc conn info saved config failed\n");
        return -1;
    } else {
        wifi_service_ap_config_t* saved_ap_conf = g_wifi_conn_info->saved_config;
        if(saved_ap_conf == NULL) {
            WIFI_SERVICE_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
            return -1;
        }
        memset(saved_ap_conf, 0, sizeof(wifi_service_ap_config_t));
        saved_ap_conf->used_ap = -1;
        WIFI_SERVICE_LOGI("%s:%d memset ap config\n", __func__, __LINE__);
    }

    module = hal_wifi_get_default_module();
    if(module == NULL) {
       WIFI_SERVICE_LOGE("wifi get default module failed.");
       return -1;
    }
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);

    if(wifi_service_queue_init() != 0) {
        WIFI_SERVICE_LOGE("wifi service queue init failed\n");
        return -1;
    }

    if(wifi_recvmsg_task() != 0) {
        WIFI_SERVICE_LOGE("wifi recv msg task create failed\n");
        return -1;
    }

    hal_wifi_install_event(module, &g_wifi_conn_hal_event);
    return 0;
}

int wifi_service_deinit(void)
{
    hal_wifi_module_t *m;
    struct netif* net_if;
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);

    free(g_wifi_conn_info->saved_config);

    netmgr_conn_deinit(g_wifi_conn_info);
    g_wifi_conn_info = NULL;

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    if(g_is_recvmsg_result_got == true) {
        WIFI_SERVICE_LOGI("%s:%d stop recvmsg task\n", __func__, __LINE__);
        g_is_recvmsg_result_got = false;
    }

    if(0 != wifi_service_send_queue_deinit()) {
        WIFI_SERVICE_LOGE("%s:%d send queue deinit failed\n", __func__, __LINE__);
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if((net_if != NULL) && (netif_is_up(net_if))) {
        if(netif_is_link_up(net_if)) {
            netifapi_netif_set_link_down(net_if);
        }
        if(netif_is_up(net_if)) {
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
        }
    }

    if(0 != hal_wifi_disconnect(m)) {
        return hal_wifi_suspend(m);
    }
    return 0;
}

int wifi_service_on(void)
{
    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    return 0;
}

int wifi_service_off(void)
{
    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    return 0;
}

static int wifi_start_scan(netmgr_conn_t* conn, wifi_service_ap_config_t* saved_ap_conf)
{
    hal_wifi_module_t *  m;
    ap_list_t ap_list[MAX_AP_CONFIG_NUM];
    int ap_num;
    int i;

    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    ap_num = saved_ap_conf->ap_num;
    for(i = 0; i < ap_num; i++) {
        memset(&ap_list[i], 0, sizeof(ap_list[i]));
        WIFI_SERVICE_LOGI("%s:%d ssid=%s channel=%d\n", __func__, __LINE__, saved_ap_conf->config[i].ssid, saved_ap_conf->config[i].channel);
        /* reset ap_power*/
        saved_ap_conf->config[i].ap_power = 0;
        strncpy(ap_list[i].ssid, saved_ap_conf->config[i].ssid, strlen(saved_ap_conf->config[i].ssid));
        ap_list[i].channel = saved_ap_conf->config[i].channel;
    }

    if(0 != hal_wifi_start_specified_scan(m, ap_list, ap_num))
    {
        WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
	    if(0 != hal_wifi_start_scan(m)) {
            WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
            return -1;
        }
    }

    return 0;
}

static int wifi_start_connect(netmgr_conn_t* conn, const char* ssid, const uint8_t* password, const uint8_t* bssid, const hal_wifi_sec_type sec_type, int time_ms)
{
    hal_wifi_module_t *  m;
    hal_wifi_init_type_t type;

    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    memset(&type, 0, sizeof(type));
    type.wifi_mode = STATION;
    type.wifi_retry_interval = 1;

    /* Disable dhcp */
    type.dhcp_mode = DHCP_DISABLE;
    strncpy(type.wifi_ssid, ssid, sizeof(type.wifi_ssid) - 1);
    memcpy(type.wifi_bssid, bssid, sizeof(type.wifi_bssid));
    memcpy(type.wifi_key, password, sizeof(type.wifi_key));
    type.sec_type = sec_type;
    WIFI_SERVICE_LOGI("%s:%d ssid=%s password=%s, bssid=%02x:%02x:%02x:%02x:%02x:%02x \n",
           __func__, __LINE__, ssid, password, bssid[0] & 0xff, bssid[1] & 0xff,
           bssid[2] & 0xff, bssid[3] & 0xff, bssid[4] & 0xff, bssid[5] & 0xff);
    WIFI_SERVICE_LOGI("%s:%d state=%d\n", __func__, __LINE__, conn->state);
    if((conn->state == CONN_STATE_DISCONNECTED) ||
       (conn->state == CONN_STATE_CONNECTING)) {
        WIFI_SERVICE_LOGE("%s:%d start wifi\n", __func__, __LINE__);
        if(0 != hal_wifi_connect(m, &type)) {
            return hal_wifi_start(m, &type);
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
        WIFI_SERVICE_LOGE("%s:%d Error: app ssid is NULL!\n", __func__, __LINE__);
        return -1;
    }

    while(ssid[i] != '\0') {
        /*  ascii code */
        if((unsigned char)ssid[i] > 0x7f) {
            contain_chinese = 1;
        }

        i++;

        if(i >= MAX_SSID_LEN) {
            WIFI_SERVICE_LOGE("%s:%d Error: utf-8 ssid len is too large\n", __func__, __LINE__);
            return -1;
        }
    }

    return contain_chinese;
}

int wifi_service_wifi_connect(const char *ssid, const char *password,
		const char *bssid, int time_ms)
{
    hal_wifi_module_t *m;
    struct netif* net_if;
    uint8_t gbk_ssid[MAX_SSID_LEN];
    int ret;
    int expire_time = 0;
    netmgr_conn_t * conn = g_wifi_conn_info;
    wifi_service_ap_config_t* saved_ap_conf;
    int retry = -1;

    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    m = hal_wifi_get_default_module();
    if(!m) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        if(ssid != NULL) {
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        }
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if(!net_if) {
        WIFI_SERVICE_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    if(!netif_is_up(net_if)) {
        WIFI_SERVICE_LOGE("%s:%d up netif\n", __func__, __LINE__);
        if(!netif_is_link_up(net_if)) {
            netifapi_netif_set_link_up(net_if);
        }
        if(!netif_is_up(net_if)) {
            netifapi_netif_set_up(net_if);
            netifapi_netif_set_default(net_if);
        }
    }

    saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
    if(saved_ap_conf == NULL) {
        WIFI_SERVICE_LOGE("%s:%d connectiong ap info is null\n", __func__, __LINE__);
        if(ssid != NULL) {
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        }
        return -1;
    }

    if(ssid != NULL) {
        int contain_chinese = ssid_contain_chinese_check(ssid);

        if(contain_chinese == -1) {
            WIFI_SERVICE_LOGI("%s:%d invalid ssid=%s\n", __func__, __LINE__, ssid);
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
            return -1;
        }

        if((conn->state == CONN_STATE_CONNECTED)
            ||(conn->state == CONN_STATE_OBTAINING_IP)
            ||(conn->state == CONN_STATE_NETWORK_CONNECTED)) {
            WIFI_SERVICE_LOGI("%s:%d do wifi disconnect when wifi is connected\n", __func__, __LINE__);
            netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
        }

        memset(saved_ap_conf, 0, sizeof(wifi_service_ap_config_t));
        WIFI_SERVICE_LOGI("%s:%d ssid=%s pwd=%s\n", __func__, __LINE__, ssid, password);

        /* saved to saved_ap_conf */
        saved_ap_conf->ap_num = 1;
        strncpy(saved_ap_conf->config[0].ssid, ssid, sizeof(saved_ap_conf->config[0].ssid) - 1);
        memcpy(saved_ap_conf->config[0].pwd, password, sizeof(saved_ap_conf->config[0].pwd));

        /* has chinese code */
        if((0 != contain_chinese) && (0 != string_convert(gbk_ssid, MAX_SSID_SIZE, (uint8_t*)ssid, strlen(ssid), UTF8_TO_GBK))) {
            const char* p_ssid = NULL;

            p_ssid = (char *)gbk_ssid;
            WIFI_SERVICE_LOGI("%s:%d gbk_ssid=%s ssid=%s len=%d\n", __func__, __LINE__, gbk_ssid, ssid, strlen(p_ssid));

            /* saved to saved_ap_conf */
            saved_ap_conf->ap_num = 2;
            saved_ap_conf->config[1].is_ssid_gbk = 1;
            WIFI_SERVICE_LOGI("%s:%d len=%d\n", __func__, __LINE__, sizeof(saved_ap_conf->config[1].ssid) - 1);
            strncpy(saved_ap_conf->config[1].ssid, p_ssid, sizeof(saved_ap_conf->config[1].ssid) - 1);
            strncpy(saved_ap_conf->config[1].utf8_ssid, ssid, sizeof(saved_ap_conf->config[1].utf8_ssid) - 1);
            strncpy(saved_ap_conf->config[1].pwd, saved_ap_conf->config[0].pwd, sizeof(saved_ap_conf->config[1].pwd) - 1);
        }

        /* get wifi config */
        if(-1 == get_wifi_config(WIFI_SERVICE_TEMP_CONF, saved_ap_conf, READ_SPECIFIC_CONFIG)) {
            if(-1 == get_wifi_config(WIFI_SERVICE_CONF, saved_ap_conf, READ_SPECIFIC_CONFIG)) {
                WIFI_SERVICE_LOGE("%s:%d no temp config found\n", __func__, __LINE__);
            } else {
                WIFI_SERVICE_LOGE("%s:%d no config found\n", __func__, __LINE__);
            }
        }
    }

    WIFI_SERVICE_LOGI("%s:%d time_ms=%d\n", __func__, __LINE__, time_ms);
    if(time_ms != 0) {
        expire_time = (int) aos_now_ms() + time_ms;
        WIFI_SERVICE_LOGI("%s:%d expire_time=%d\n", __func__, __LINE__, expire_time);
    }
    /* Setting default connect state */
    g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
    netmgr_conn_state_change(conn, CONN_STATE_CONNECTING);
    /* waiting scan result */
    while(1) {

        WIFI_SERVICE_LOGI("%s:%d state=%d\n", __func__, __LINE__, conn->state);
        WIFI_SERVICE_LOGI("%s:%d connect state=%d\n", __func__, __LINE__, g_wifi_service_connect_state);

        if(conn->state == CONN_STATE_NETWORK_CONNECTED) {
            WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
            ret = 0;
            /* check channel in reconnect state */
            if(ssid == NULL) {
                int channel;
                int i;

                saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
                if(saved_ap_conf == NULL) {
                    WIFI_SERVICE_LOGE("%s:%d saved ap config is null\n", __func__, __LINE__);
                    return -1;
                }
                i = saved_ap_conf->used_ap;

                if(-1 == (channel = hal_wifi_get_channel(m))) {
                    WIFI_SERVICE_LOGE("%s:%d get channel failed\n", __func__, __LINE__);
                } else if((i != -1) && (channel != saved_ap_conf->config[i].channel)) {
                    WIFI_SERVICE_LOGI("%s:%d new channel:%d old channel:%d\n",
                                          __func__, __LINE__, channel, saved_ap_conf->config[i].channel);
                    /* update channel information */
                    saved_ap_conf->config[i].channel = channel;
                }
            }
            update_wifi_config(conn, WIFI_SERVICE_CONF);
            break;
        }
        else if(conn->state == CONN_STATE_DISCONNECTED) {
            WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
            ret = -1;
            break;
        }
        else if((retry >= WIFI_SERVICE_RETRY_MAX_NUM)
                || ((time_ms != 0) &&((int)aos_now_ms() > expire_time))) {
            /* Timeout Occured */
            WIFI_SERVICE_LOGE("%s:%d timeout\n", __func__, __LINE__);
            if(g_wifi_service_connect_state == WIFI_SERVICE_CONNECT_SUCCESS) {
                WIFI_SERVICE_LOGE("%s:%d stop dhcp\n", __func__, __LINE__);
                netifapi_dhcp_stop(net_if);
                netifapi_netif_set_down(net_if);
            }
            g_wifi_service_connect_state = WIFI_SERVICE_CONNECT_FAILED;
            ret = -1;
            break;
        }
        else if((g_wifi_service_connect_state == WIFI_SERVICE_SCAN_FAILED)
              || (g_wifi_service_connect_state == WIFI_SERVICE_CONNECT_FAILED)) {
            WIFI_SERVICE_LOGI("%s:%d start scan\n", __func__, __LINE__);

            if(retry != -1) {
                hal_wifi_module_t *m;
                struct netif* net_if;

                WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
                m = hal_wifi_get_default_module();
                if( m == NULL ) {
                    WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
                    return -1;
                }

                net_if = hal_wifi_get_netif(m, STATION);
                if(!net_if) {
                    WIFI_SERVICE_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
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

                if(0 != hal_wifi_disconnect(m)) {
                    if(0 != hal_wifi_suspend(m)) {
                        WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
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

            g_wifi_service_connect_state = WIFI_SERVICE_SCAN_START;
            if(0 != wifi_start_scan(conn, saved_ap_conf)) {
                WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
                g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
                ret = -1;
                /* Wait 1s for avoid scan conflict */
                aos_msleep(1000);
                continue;
            }
        }
        else if(g_wifi_service_connect_state == WIFI_SERVICE_SCAN_SUCCESS){
                int i, j;

                WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);

                for(i = 0, j = -1; i < saved_ap_conf->ap_num; i++) {
                        WIFI_SERVICE_LOGI("%s:%d [%d] power=%d\n", __func__, __LINE__, i, saved_ap_conf->config[i].ap_power);
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
                    WIFI_SERVICE_LOGE("%s:%d no ap found\n", __func__, __LINE__);
                    g_wifi_service_connect_state = WIFI_SERVICE_SCAN_FAILED;
                    ret = -1;
                    continue;
                }

                i = j;

                g_wifi_service_connect_state = WIFI_SERVICE_CONNECT_START;

                saved_ap_conf->used_ap = i;
                WIFI_SERVICE_LOGI("%s:%d ap_num=%d i=%d\n", __func__, __LINE__, saved_ap_conf->ap_num, i);
                /* Reset retry to 0 */
                retry = 0;
                ret = wifi_start_connect(conn, saved_ap_conf->config[i].ssid, saved_ap_conf->config[i].pwd, saved_ap_conf->config[i].bssid, saved_ap_conf->config[i].sec_type, time_ms);
        }
        else {
            int retry_time = retry_backoff_time[retry];
            if(g_wifi_service_connect_state == WIFI_SERVICE_CONNECT_SUCCESS) {
                retry ++;
            }
            WIFI_SERVICE_LOGI("%s:%d sleep aos_now_ms=%d retry=%d\n", __func__, __LINE__, (int)aos_now_ms(), retry);
            aos_msleep(retry_time * 1000);
        }

        if(time_ms != 0) {
            WIFI_SERVICE_LOGI("%s:%d timeout time:%d aos_now_ms=%d\n", __func__, __LINE__, (int)expire_time,(int)aos_now_ms());
        }
    }
    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    if(ret != 0) {
        /* If ssid is not null trigger reconnect here
         * If ssid is null tigger reconnect by reconnect task */
        if(ssid != NULL) {
            netmgr_conn_state_change(conn, CONN_STATE_FAILED);
        }
    }
    return ret;
}

int wifi_service_disconnect(void)
{
    hal_wifi_module_t *m;
    netmgr_conn_t *conn = g_wifi_conn_info;
    struct netif* net_if;

    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    if(conn != NULL) {
        WIFI_SERVICE_LOGE("%s:%d state=%d\n", __func__, __LINE__, conn->state);
        netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
    }

    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    m = hal_wifi_get_default_module();
    if( m == NULL ) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if(net_if != NULL) {
        if(netif_is_link_up(net_if)) {
            netifapi_netif_set_link_down(net_if);
        }
        if(netif_is_up(net_if)) {
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
        }
    }

    if(0 != hal_wifi_disconnect(m)) {
        return hal_wifi_suspend(m);
    }
	return 0;
}

void wifi_service_auto_reconnect(int enable)
{
    netmgr_conn_t *conn = g_wifi_conn_info;
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }

    WIFI_SERVICE_LOGI("%s:%d enable=%d\n", __func__, __LINE__, enable);
    conn->auto_reconnect = enable;

    if((conn->auto_reconnect == true)
       && (conn->state == CONN_STATE_DISCONNECTED)) {
        wifi_auto_reconnect_task();
    }
}

void wifi_service_set_wifi_msg_cb(wifi_service_msg_cb_t cb)
{
    netmgr_conn_t *conn = g_wifi_conn_info;

    WIFI_SERVICE_LOGD("%s:%d\n", __func__, __LINE__);
    if(conn == NULL) {
        WIFI_SERVICE_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return;
    }

    conn->msg_cb = (void*)cb;
}


int wifi_service_get_rssi(int *rssi)
{
    hal_wifi_link_stat_t out;
    hal_wifi_module_t *m;

    m = hal_wifi_get_default_module();

    if( m == NULL ) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    hal_wifi_get_link_stat(m, &out);
    memcpy(rssi, &out.rssi, sizeof(out.rssi));
    return 0;
}

int wifi_service_get_ip_addr(char* ip_address)
{
    hal_wifi_module_t *m;
    struct netif* net_if;

    m = hal_wifi_get_default_module();

    if( m == NULL ) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if(!net_if) {
        WIFI_SERVICE_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    sprintf(ip_address, "%d.%d.%d.%d",
                     ((u8_t *)&net_if->ip_addr.addr)[0],
                     ((u8_t *)&net_if->ip_addr.addr)[1],
                     ((u8_t *)&net_if->ip_addr.addr)[2],
                     ((u8_t *)&net_if->ip_addr.addr)[3]);

    return 0;
}

int wifi_service_get_mac(char *mac)
{
    hal_wifi_module_t *m;

    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    m = hal_wifi_get_default_module();
    if( m == NULL ) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }
    return hal_wifi_get_mac_addr(m, (uint8_t*)mac);
}

int wifi_service_save_config(void)
{
    WIFI_SERVICE_LOGI("%s:%d\n", __func__, __LINE__);
    return update_wifi_config(g_wifi_conn_info, WIFI_SERVICE_CONF);
}

static void handle_wifi_service_help_cmd()
{
    WIFI_SERVICE_LOGI("Usage: netgmr -t wifi [options]\n");
    WIFI_SERVICE_LOGI("       netmgr -t wifi [-h]\n");
    WIFI_SERVICE_LOGI("  -i,        Init wifi service\n");
    WIFI_SERVICE_LOGI("  -a,        Set auto reconnect wifi flag\n");
    WIFI_SERVICE_LOGI("  -c,        Connect wifi\n");
    WIFI_SERVICE_LOGI("  -e,        Disconnect wifi\n");
    WIFI_SERVICE_LOGI("  -g,        Get mac address\n");
    WIFI_SERVICE_LOGI("  -s,        Show scan result\n");
    WIFI_SERVICE_LOGI("  -p,        Print wifi status\n");
    WIFI_SERVICE_LOGI("  -r,        Read wifi config\n");
    WIFI_SERVICE_LOGI("  -w,        Write wifi config\n");
    WIFI_SERVICE_LOGI("  -d,        Delete wifi config\n");
    WIFI_SERVICE_LOGI("  -h,        Show netmgr wifi help commands\n");
    WIFI_SERVICE_LOGI("Example:\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -i\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -a 1\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -c bssid password\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -s\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -p\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -r\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -w network={\\nssid=\"apple\"\\npassword=\"aos123456\"\\nchannel=\"1\"\\n}\\n");
    WIFI_SERVICE_LOGI("netmgr -t wifi -d\n");
}

typedef struct netmgr_connect_params {
   char ssid[MAX_SSID_LEN];
   uint8_t pwd[MAX_PWD_LEN];
   uint8_t bssid[ETH_ALEN];
   int  timeout;
} netmgr_connect_params;

static void wifi_connect_handle(char * args)
{
    netmgr_connect_params* params = (netmgr_connect_params*) args;

    if(params == NULL) {
        WIFI_SERVICE_LOGE("%s:%d malloc failed\n", __func__, __LINE__);
        return ;
    }

    wifi_service_on();
    wifi_service_wifi_connect(params->ssid, params->pwd, params->bssid, params->timeout);

    free(params);
}

void wifi_service_handle_cmd(int argc, char **argv)
{
    const char *rtype = argc > 0 ? argv[0] : "";
    if (strcmp(rtype, "-i") == 0) {
        WIFI_SERVICE_LOGI("%s:%d WIFI_SERVICE_INIT\n", __func__, __LINE__);
        wifi_service_init();
    }
    else if (strcmp(rtype, "-a") == 0) {
        if((argc > 1) && ((strcmp(argv[1], "0") == 0) || (strcmp(argv[1], "1") == 0))) {
            int enable = atoi(argv[1]);
            wifi_service_auto_reconnect(enable);
        } else {
            WIFI_SERVICE_LOGI("%s:%d WIFI_SERVICE_ON invalid params\n", __func__, __LINE__);
        }
    }
    else if (strcmp(rtype, "-c") == 0) {
        if((argc > 1)) {
            netmgr_connect_params *params;

            params = (netmgr_connect_params*) malloc(sizeof(netmgr_connect_params));
            if(params == NULL) {
                WIFI_SERVICE_LOGE("%s:%d malloc failed\n", __func__, __LINE__);
                return ;
            }
            memset(params, 0, sizeof(netmgr_connect_params));
            strncpy(params->ssid, argv[1], sizeof(params->ssid)-1);

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
            WIFI_SERVICE_LOGE("%s:%d WIFI_SERVICE_CONNECT invalid params\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-e") == 0) {
        wifi_service_disconnect();
    } else if (strcmp(rtype, "-g") == 0) {
        uint8_t mac[6];
        if(-1 != wifi_service_get_mac(mac)) {
            WIFI_SERVICE_LOGI("mac address=%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx\n",
              mac[0] & 0xff,
              mac[1] & 0xff,
              mac[2] & 0xff,
              mac[3] & 0xff,
              mac[4] & 0xff,
              mac[5] & 0xff);
        } else {
            WIFI_SERVICE_LOGE("%s:%d WIFI_SERVICE_GET_MAC failed\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-s") == 0) {
        hal_wifi_module_t *  m;
        m = hal_wifi_get_default_module();

        if(m == NULL) {
            WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
            return ;
        }
        g_is_got_scan_request_cmd = true;

     	if(0 != hal_wifi_start_scan(m)) {
            g_is_got_scan_request_cmd = false;
            WIFI_SERVICE_LOGE("%s:%d\n", __func__, __LINE__);
            return ;
        }

    } else if (strcmp(rtype, "-p") == 0) {
        wifi_service_ap_config_t* ap_conf = (wifi_service_ap_config_t* )g_wifi_conn_info->saved_config;
        char * ssid = NULL;

        if((ap_conf != NULL) && (ap_conf->used_ap != -1) &&(ap_conf->used_ap < ap_conf->ap_num)) {
            wifi_service_ap_info_t * ap_info = NULL;
            int used_ap = ap_conf->used_ap;

            ap_info = &ap_conf->config[used_ap];

            if(ap_info->is_ssid_gbk == 0) {
                ssid = ap_info->ssid;
            } else {
                ssid = ap_info->utf8_ssid;
            }
            WIFI_SERVICE_LOGI("%s:%d ssid:%s\n", __func__, __LINE__, ssid);
        }

        if(g_wifi_conn_info->state == CONN_STATE_DISCONNECTING) {
            WIFI_SERVICE_LOGI("%s:%d DISCONNECTING\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_DISCONNECTED) {
            WIFI_SERVICE_LOGI("%s:%d DISCONNECTED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_CONNECTING) {
            WIFI_SERVICE_LOGI("%s:%d CONNECTING\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_CONNECTED) {
            WIFI_SERVICE_LOGI("%s:%d WIFI CONNECTED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_OBTAINING_IP) {
            WIFI_SERVICE_LOGI("%s:%d OBTAINING IP\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_NETWORK_CONNECTED) {
            WIFI_SERVICE_LOGI("%s:%d NETWORK_CONNECTED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_FAILED) {
            WIFI_SERVICE_LOGI("%s:%d FAILED\n", __func__, __LINE__);
        } else if(g_wifi_conn_info->state == CONN_STATE_UNKNOWN) {
            WIFI_SERVICE_LOGI("%s:%d UNKNOWN\n", __func__, __LINE__);
        }
    } else if (strcmp(rtype, "-r") == 0) {
        FILE *fp = NULL;
        char buf[100];

        fp = fopen(WIFI_SERVICE_CONF, "r");
        if(NULL == fp) {
            WIFI_SERVICE_LOGE("%s:%d open %s failed:%s\n", __func__, __LINE__, WIFI_SERVICE_CONF,strerror(errno));
            return ;
        }

        fseek(fp, 0, SEEK_SET);

        while(fgets(buf, sizeof(buf), fp)) {
            buf[sizeof(buf) - 1] = '\0';
            WIFI_SERVICE_LOGD("%s", buf);
        }
        fclose(fp);
    } else if (strcmp(rtype, "-d") == 0) {
        int err;
        if((err = remove(WIFI_SERVICE_CONF)) == 0) {
            WIFI_SERVICE_LOGI("%s:%d remove success\n", __func__, __LINE__);
        } else {
            WIFI_SERVICE_LOGE("%s:%d remove failed errno=%d %s\n", __func__, __LINE__, errno, strerror(errno));
        }
    } else if (strcmp(rtype, "-w") == 0) {
        if(argc > 1) {
            FILE *fp = NULL;
            const char* p = argv[1];
            bool is_backslash = false;

            fp = fopen(WIFI_SERVICE_CONF,"a");
            if(NULL == fp) {
                WIFI_SERVICE_LOGE("%s:%d open %s failed:%s\n", __func__, __LINE__, WIFI_SERVICE_CONF,strerror(errno));
                return ;
            }

            while(*p != '\0') {
                if(*p == '\\') {
                    if(is_backslash == true) {
                        fputc('\\',fp);
                    }
                    is_backslash = true;
                }
                else if((*p == 'n') && (is_backslash == true)) {
                    fputc('\n',fp);
                    is_backslash = false;
                }
                else {
                    if(is_backslash == true) {
                        fputc('\\',fp);
                        is_backslash = false;
                    }
                    fputc(*p,fp);
                }
                p ++;
            }
            fputc('\0',fp);
            WIFI_SERVICE_LOGI("%s:%d Write Config Success\n", __func__, __LINE__);

            fseek(fp, 0L, SEEK_SET);
            fclose(fp);
        }
        else {
            WIFI_SERVICE_LOGE("%s:%d Invalid params\n", __func__, __LINE__);
        }
    } else {
        handle_wifi_service_help_cmd();
    }
}

#ifdef AOS_COMP_CLI
static void handle_netmgr_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";

    if (strcmp(rtype, "-t") == 0) {
        const char *type = argc > 2 ? argv[2] : "";

        if(type == NULL) {
            LOGE(TAG, "type is null\n");
        } else if(strcmp(type, "wifi") == 0) {
            wifi_service_handle_cmd(argc-3, &argv[3]);
        }
        else {
                LOGE(TAG, "type = %s not support\n", type);
        }
    }
}

static struct cli_command ncmd = {
    .name     = "netmgr",
    .help     = "netmgr [-t] wifi [-c|-e|-g|-s|-p|-r|-d|-w]",
    .function = handle_netmgr_cmd,
};
#endif

int32_t netmgr_init(void) {
#ifdef AOS_COMP_CLI
    aos_cli_register_command(&ncmd);
#endif
    return wifi_service_init();
}

void netmgr_deinit(void) {
    wifi_service_deinit();
}

int32_t netmgr_start(bool autoconfig) {
    wifi_service_auto_reconnect(autoconfig);
    return 0;
}

int32_t netmgr_stats(int32_t interface, netmgr_stats_t *stats) {
    int ret;
    if(interface == INTERFACE_WIFI) {
    ret = wifi_service_get_ip_addr(stats->ip);
        if(ret != 0) {
        WIFI_SERVICE_LOGE("%s:%d get ip failed\n", __func__, __LINE__);
        stats->ip_available = false;
            return -1;
    }
    stats->ip_available = g_is_dhcp_got_ip;
    }
    else {
        WIFI_SERVICE_LOGE("%s:%d invalid params interface:%d\n", __func__, __LINE__, interface);
        stats->ip_available = false;
    }
	return 0;
}

int32_t netmgr_connect(const char *ssid, const uint8_t *password, uint32_t timeout) {
    return wifi_service_wifi_connect(ssid, (char *)password, NULL, timeout);
}

bool netmgr_wifi_get_ip_state()
{
    netmgr_conn_t* conn = g_wifi_conn_info;

    if((conn != NULL)
        &&(conn->state == CONN_STATE_NETWORK_CONNECTED)) {
        return true;
    }
    return false;
}

void netmgr_reconnect_wifi()
{
    wifi_service_auto_reconnect(true);
}

int netmgr_set_ap_config(netmgr_ap_config_t *config)
{
    FILE *temp_fp = NULL;
    char buf[WIFI_SERVICE_CONFIG_LEN];
    int len = 0;
    int channel = 0;
    hal_wifi_module_t *  m;
    char *format = "utf8";

    m = hal_wifi_get_default_module();
    if(m == NULL) {
        WIFI_SERVICE_LOGE("%s:%d get module failed\n", __func__, __LINE__);
        return -1;
    }

    if(-1 == (channel = hal_wifi_get_channel(m))) {
        WIFI_SERVICE_LOGE("%s:%d get channel failed\n", __func__, __LINE__);
        return -1;
    }

    if((0 != mkdir(WIFI_SERVICE_TEMP_PATH, 0644)) && (errno != EEXIST)) {
        WIFI_SERVICE_LOGE("mkdir %s failed:%s\n", WIFI_SERVICE_TEMP_PATH, strerror(errno));
        return -1;
    }

    temp_fp = fopen(WIFI_SERVICE_TEMP_CONF,"a");
    if(NULL == temp_fp) {
        WIFI_SERVICE_LOGE("open %s failed:%s\n", WIFI_SERVICE_TEMP_CONF, strerror(errno));
        return -1;
    }

    snprintf(buf,WIFI_SERVICE_CONFIG_LEN,"network={\n \
ssid=\"%s\"\n \
password=\"%s\"\n \
bssid=%02x:%02x:%02x:%02x:%02x:%02x\n \
format=\"%s\"\n \
channel=\"%d\"\n}\n\n",
    config->ssid,
    config->pwd,
    config->bssid[0] & 0xff,
    config->bssid[1] & 0xff,
    config->bssid[2] & 0xff,
    config->bssid[3] & 0xff,
    config->bssid[4] & 0xff,
    config->bssid[5] & 0xff,
    format,
    channel
    );

    len = strlen(buf);
    WIFI_SERVICE_LOGI("%s:%d config buff len :%d\n", __func__, __LINE__, len);
    WIFI_SERVICE_LOGI("saving wifi config file:%s\n",buf);
    if(1 != fwrite(buf,len,1,temp_fp)) {
        WIFI_SERVICE_LOGE("fwrite:%s\n",strerror(errno));
    }
}

void netmgr_clear_ap_config(void)
{
    if(remove(WIFI_SERVICE_CONF) == 0) {
        WIFI_SERVICE_LOGI("%s:%d remove config success\n", __func__, __LINE__);
    } else {
        WIFI_SERVICE_LOGE("%s:%d remove config failed errno=%d\n", __func__, __LINE__, errno);
    }

    if(remove(WIFI_SERVICE_TEMP_CONF) == 0) {
        WIFI_SERVICE_LOGI("%s:%d remove temp config success\n", __func__, __LINE__);
    } else {
        WIFI_SERVICE_LOGE("%s:%d remove temp config failed errno=%d\n", __func__, __LINE__, errno);
    }

}

void wifi_get_ip(char ips[16])
{
    wifi_service_get_ip_addr(ips);
}

int netmgr_get_ap_config(netmgr_ap_config_t *config)
{
    netmgr_conn_t* conn = g_wifi_conn_info;
    if(config == NULL) {
        return -1;
    }

    if((conn != NULL)
        &&(conn->state == CONN_STATE_NETWORK_CONNECTED)) {
        wifi_service_ap_config_t* saved_ap_conf;
        int i;

        saved_ap_conf = (wifi_service_ap_config_t* )conn->saved_config;
        if(saved_ap_conf == NULL) {
            return -1;
        }

        i = saved_ap_conf->used_ap;
        if((i >= saved_ap_conf->ap_num)
            || (i < 0)) {
            return -1;
        }

        memcpy(config->ssid, saved_ap_conf->config[i].ssid, sizeof(config->ssid));
        memcpy(config->pwd, saved_ap_conf->config[i].pwd, sizeof(config->pwd));
        memcpy(config->ssid, saved_ap_conf->config[i].bssid, sizeof(config->bssid));
        return 0;
    }
    return -1;
}
