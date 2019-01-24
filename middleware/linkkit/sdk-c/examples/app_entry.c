/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "app_entry.h"
#include "iot_export.h"
#include "iot_import.h"

#ifdef WIFI_PROVISION_ENABLED

#define AWSS_SSID_MAXLEN   (32)
#define AWSS_PASSWD_MAXLEN (64)
#define AWSS_BSSID_MAXLEN  (6)
#define KV_KEY_SSID        "awss_ssid"
#define KV_KEY_PASSWD      "awss_passwd"
#define KV_KEY_BSSID       "awss_bssid"

static void *awss_suc_semphore = NULL;
static void *awss_thread_handler = NULL;

void LITE_set_loglevel(int);

static void awss_save_router_info()
{
    int len = 0;

    char passwd[AWSS_PASSWD_MAXLEN + 1] = {0};
    char ssid[AWSS_SSID_MAXLEN + 1] = {0};
    char bssid[AWSS_BSSID_MAXLEN] = {0};

    HAL_Wifi_Get_Ap_Info(ssid, passwd, (uint8_t *)bssid);

    /*
     * save current router information into flash.
     */
    len = strlen(ssid);
    HAL_Kv_Set(KV_KEY_SSID, ssid, len > AWSS_SSID_MAXLEN ? AWSS_SSID_MAXLEN : len, 0);
    len = strlen(passwd);
    HAL_Kv_Set(KV_KEY_PASSWD, passwd, len > AWSS_PASSWD_MAXLEN ? AWSS_PASSWD_MAXLEN : len, 0);

    HAL_Kv_Set(KV_KEY_BSSID, bssid, AWSS_BSSID_MAXLEN, 0);
}

#if !defined(DEVICE_MODEL_ENABLED) && defined(DEV_BIND_ENABLED)
static void *awss_bind_thread_handler = NULL;
void *awss_bind_thread_task(void *param)
{
    void *handler = awss_bind_thread_handler;

    awss_report_cloud();

    awss_bind_thread_handler = NULL;

    HAL_ThreadDelete(handler);

    return NULL;
}

static void awss_bind_start_task()
{
    hal_os_thread_param_t param = {0};

    if (awss_bind_thread_handler != NULL)
        return NULL;

    param.stack_size = 4096;
    HAL_ThreadCreate(&awss_bind_thread_handler, awss_bind_thread_task, NULL, &param, NULL);
}
#endif
/*
 * Note:
 * the linkkit_event_monitor must not block and should run to complete fast
 * if user wants to do complex operation with much time,
 * user should post one task to do this, not implement complex operation in
 * linkkit_event_monitor
 */

static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: /* AWSS start without enbale, just supports device discover */
            /* operate led to indicate user */
            HAL_Printf("IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: /* AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled. */
            HAL_Printf("IOTX_AWSS_ENABLE");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_LOCK_CHAN: /* AWSS lock channel(Got AWSS sync packet) */
            HAL_Printf("IOTX_AWSS_LOCK_CHAN");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_PASSWD_ERR: /* AWSS decrypt passwd error */
            HAL_Printf("IOTX_AWSS_PASSWD_ERR");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            HAL_Printf("IOTX_AWSS_GOT_SSID_PASSWD");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_CONNECT_ADHA: /* AWSS try to connnect adha (device
                                      * discover, router solution) */
            HAL_Printf("IOTX_AWSS_CONNECT_ADHA");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: /* AWSS fails to connect adha */
            HAL_Printf("IOTX_AWSS_CONNECT_ADHA_FAIL");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_CONNECT_AHA: /* AWSS try to connect aha (AP solution) */
            HAL_Printf("IOTX_AWSS_CONNECT_AHA");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: /* AWSS fails to connect aha */
            HAL_Printf("IOTX_AWSS_CONNECT_AHA_FAIL");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_SETUP_NOTIFY: /* AWSS sends out device setup information
                                      * (AP and router solution) */
            HAL_Printf("IOTX_AWSS_SETUP_NOTIFY");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_CONNECT_ROUTER: /* AWSS try to connect destination router */
            HAL_Printf("IOTX_AWSS_CONNECT_ROUTER");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: /* AWSS fails to connect destination router. */
            HAL_Printf("IOTX_AWSS_CONNECT_ROUTER_FAIL");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_GOT_IP: /* AWSS connects destination successfully and got
                                * ip address */
            HAL_Printf("IOTX_AWSS_GOT_IP");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_SUC_NOTIFY: /* AWSS sends out success notify (AWSS sucess) */
            HAL_Printf("IOTX_AWSS_SUC_NOTIFY");
            /* operate led to indicate user */
            awss_save_router_info();
            break;
        case IOTX_AWSS_BIND_NOTIFY: /* AWSS sends out bind notify information to
                                     * support bind between user and device */
            HAL_Printf("IOTX_AWSS_BIND_NOTIFY");
            /* operate led to indicate user */
            break;
        case IOTX_AWSS_ENABLE_TIMEOUT: /* AWSS enable timeout
                                        * user needs to enable awss again to support get ssid & passwd of router */
            HAL_Printf("IOTX_AWSS_ENALBE_TIMEOUT");
            /* operate led to indicate user */

            /* it's just demo, manufacture should care about issue of security */
            awss_config_press();
            break;
        case IOTX_CONN_CLOUD: /* Device try to connect cloud */
            HAL_Printf("IOTX_CONN_CLOUD");
            /* operate led to indicate user */
            break;
        case IOTX_CONN_CLOUD_FAIL: /* Device fails to connect cloud, refer to
                                    * net_sockets.h for error code */
            HAL_Printf("IOTX_CONN_CLOUD_FAIL");
            /* operate led to indicate user */
            break;
        case IOTX_CONN_CLOUD_SUC: /* Device connects cloud successfully */
            HAL_Printf("IOTX_CONN_CLOUD_SUC");
            /* operate led to indicate user */
#if !defined(DEVICE_MODEL_ENABLED) && defined(DEV_BIND_ENABLED)
            awss_bind_start_task();
#endif
            break;
        case IOTX_RESET: /* Linkkit reset success (just got reset response from
                          * cloud without any other operation) */
            HAL_Printf("IOTX_RESET");
            /* operate led to indicate user */
            break;
        default:
            break;
    }
}

static int awss_valid_bssid(const char *bssid)
{
    char zero[AWSS_BSSID_MAXLEN] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    char bcast[AWSS_BSSID_MAXLEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    if (bssid == NULL)
        return 0;

    if (memcmp(bssid, zero, sizeof(zero)) == 0)
        return 0;

    if (memcmp(bssid, bcast, sizeof(bcast)) == 0)
        return 0;

    return 1;
}

static int awss_valid_passwd(const char *passwd)
{
    if (passwd == NULL || strlen(passwd) > AWSS_PASSWD_MAXLEN)
        return 0;

    return 1;
}

static int awss_valid_ssid(const char *ssid)
{
    if (ssid == NULL || strlen(ssid) > AWSS_SSID_MAXLEN || strlen(ssid) == 0)
        return 0;

    return 1;
}

static int awss_demo()
{
    char pk[PRODUCT_KEY_MAXLEN + 1] = {0};
    char dn[DEVICE_NAME_MAXLEN + 1] = {0};

    char passwd[AWSS_PASSWD_MAXLEN + 1] = {0};
    char ssid[AWSS_SSID_MAXLEN + 1] = {0};
    char bssid[AWSS_BSSID_MAXLEN] = {0};

    int res = -1;
    int len = 0;

    HAL_GetProductKey(pk);
    HAL_GetDeviceName(dn);

    if (strlen(pk) == 0 || strlen(pk) >= PRODUCT_KEY_MAXLEN) {
        HAL_Printf("[ERR]: Invalid Proudct Key\n");
        return -1;
    }

    if (strlen(dn) == 0 || strlen(dn) >= DEVICE_NAME_MAXLEN) {
        HAL_Printf("[ERR]: Invalid Device Name\n");
        return -1;
    }

    if (HAL_Awss_Get_Conn_Encrypt_Type() == 3) {
        /*
         * Secret Per Product
         */
        char ps[PRODUCT_SECRET_MAXLEN + 1] = {0};
        HAL_GetProductSecret(ps);
        if (strlen(ps) == 0 || strlen(ps) >= PRODUCT_SECRET_MAXLEN) {
            HAL_Printf("[ERR]: Invalid Product Secret\n");
            return -1;
        }
    } else if (HAL_Awss_Get_Conn_Encrypt_Type() == 4) {
        /*
         * Secret Per Device
         */
        char ds[DEVICE_SECRET_MAXLEN + 1] = {0};
        HAL_GetDeviceSecret(ds);
        if (strlen(ds) == 0 || strlen(ds) >= DEVICE_SECRET_MAXLEN) {
            HAL_Printf("[ERR]: Invalid Device Secret\n");
            return -1;
        }
    }

    len = sizeof(ssid);
    HAL_Kv_Get(KV_KEY_SSID, ssid, &len);
    len = sizeof(passwd);
    HAL_Kv_Get(KV_KEY_PASSWD, passwd, &len);
    len = sizeof(bssid);
    HAL_Kv_Get(KV_KEY_BSSID, bssid, &len);

    if (awss_valid_ssid(ssid) && awss_valid_passwd(passwd) &&
        awss_valid_bssid(bssid)) {
        res = HAL_Awss_Connect_Ap(30 * 1000, ssid, passwd, 0, 0, (uint8_t *)bssid, 0);
        HAL_Printf("[%s] connect router %s\b", res == 0 ? "INFO" : "ERR",
                res == 0 ? "success" : "fail");
    }

    if (res != 0) {
        /*
         * invalid router information or connect router failed
         */
        iotx_event_regist_cb(linkkit_event_monitor);
        /* it's just demo, manufacture should care about issue of security */
        awss_config_press();
        awss_start();
    } else {
        /*
         * connect router successfully
         */
    }
    return 0;
}

void *awss_thread_task(void *param)
{
    awss_demo();

    if (awss_suc_semphore)
        HAL_SemaphorePost(awss_suc_semphore);

    return NULL;
}
#endif

int main(int argc, char **argv)
{
    app_main_paras_t paras;
    paras.argc = argc;
    paras.argv = argv;

#ifdef WIFI_PROVISION_ENABLED
    /* set PK/PS/DN/DS */
#if 0
    HAL_SetProductKey(pk);
    HAL_SetProductSecret(ps);
    HAL_SetDeviceName(dn);
    HAL_SetDeviceSecret(ds);
#endif

    /* set DEBUG LOG LEVEL */
    LITE_set_loglevel(5);

    awss_suc_semphore = HAL_SemaphoreCreate();
    if (awss_suc_semphore == NULL) {
        HAL_Printf("[ERR]: alloc semphore fail\n");
        return -1;
    }

    do {
        hal_os_thread_param_t param = {0};
        /*
         * stack size for awss_thread_task should been larger than 4K
         */
        param.stack_size = 6144;
        HAL_ThreadCreate(&awss_thread_handler, awss_thread_task, NULL, &param, NULL);
    } while (0);

    /*
     * wait for AWSS success
     */
    HAL_SemaphoreWait(awss_suc_semphore, PLATFORM_WAIT_INFINITE);
    HAL_SemaphoreDestroy(awss_suc_semphore);
    awss_suc_semphore = NULL;
#endif
    linkkit_main((void *)&paras);
    return 0;
}
