/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "aos/uData.h"

#ifdef DATA_TO_CLOUD
#include "netmgr.h"
#include "iot_export.h"
#include "app_entry.h"

#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#include <k_api.h>

static char linkkit_started = 0;
static char awss_running    = 0;

void set_iotx_info();
void do_awss_active();

#ifdef CONFIG_PRINT_HEAP
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        // clear_wifi_ssid();
        return;
    }

    if (!linkkit_started) {
#ifdef CONFIG_PRINT_HEAP
        print_heap();
#endif
#ifdef MQTT_DIRECT
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 6);
#else
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 8);
#endif
        linkkit_started = 1;
    }
}


static void cloud_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_YUNIO) {
        return;
    }

    LOG("cloud_service_event %d", event->code);

    if (event->code == CODE_YUNIO_ON_CONNECTED) {
        LOG("user sub and pub here");
        return;
    }

    if (event->code == CODE_YUNIO_ON_DISCONNECTED) {
    }
}

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
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device
                              // discover
            // operate led to indicate user
            LOG("IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable
            LOG("IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;
        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            LOG("IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;
        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            LOG("IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            LOG("IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
                                     // discover, router solution)
            LOG("IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            LOG("IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            LOG("IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            LOG("IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
                                     // (AP and router solution)
            LOG("IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            LOG("IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
                                            // router.
            LOG("IOTX_AWSS_CONNECT_ROUTER_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
                               // ip address
            LOG("IOTX_AWSS_GOT_IP");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
                                   // sucess)
            LOG("IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
                                    // support bind between user and device
            LOG("IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD: // Device try to connect cloud
            LOG("IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
                                   // net_sockets.h for error code
            LOG("IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            LOG("IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
                         // cloud without any other operation)
            LOG("IOTX_RESET");
            // operate led to indicate user
            break;
        default:
            break;
    }
}

static void start_netmgr(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
}

void do_awss_active()
{
    LOG("do_awss_active %d\n", awss_running);
    awss_running = 1;
    #ifdef WIFI_AWSS_ENABLED
    extern int awss_config_press();
    awss_config_press();
    #endif
}

static void linkkit_reset(void *p)
{
    netmgr_clear_ap_config();
    HAL_Sys_reboot();
}


extern int  awss_report_reset();
static void do_awss_reset()
{
#ifdef WIFI_AWSS_ENABLED
    aos_task_new("reset", (void (*)(void *))awss_report_reset, NULL, 2048);
#endif
    aos_post_delayed_action(2000, linkkit_reset, NULL);
}


void linkkit_key_process(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_KEY) {
        return;
    }
    LOG("awss config press %d\n", eventinfo->value);

    if (eventinfo->code == CODE_BOOT) {
        if (eventinfo->value == VALUE_KEY_CLICK) {

            do_awss_active();
        } else if (eventinfo->value == VALUE_KEY_LTCLICK) {
            do_awss_reset();
        }
    }
}

#ifdef CONFIG_AOS_CLI
static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_reset, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_active, NULL);
}

static struct cli_command resetcmd = { .name     = "reset",
                                       .help     = "factory reset",
                                       .function = handle_reset_cmd };

static struct cli_command ncmd = { .name     = "active_awss",
                                   .help     = "active_awss [start]",
                                   .function = handle_active_cmd };
#endif

#ifdef CONFIG_PRINT_HEAP
static void duration_work(void *p)
{
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
}
#endif

#endif


#define UDATA_PRINT    printf

#define UDATA_SHOW_UINT_1(TYPE,TIME,DATA1) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%d)\n", (int)(DATA1)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%d)\n", (unsigned int)(TIME)); \
}while(0);

#define UDATA_SHOW_UINT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%d) (%d) (%d)\n", (int)(DATA1),(int)(DATA2),(int)(DATA3)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%d)\n", (unsigned int)(TIME)); \
}while(0);

#define UDATA_SHOW_FLOAT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%f) (%f) (%f)\n", (DATA1),(DATA2),(DATA3)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%d)\n", (unsigned int)(TIME)); \
}while(0);

#ifdef UDATA_YLOOP

void uData_report_demo(input_event_t *event, void *priv_data)
{
    udata_pkg_t buf;
    if ((event == NULL) || (event->type != EV_UDATA)) {
        return;
    }

    if (event->code == CODE_UDATA_REPORT_PUBLISH) {
        int ret = 0;
        ret = uData_report_publish(event, &buf);
        if (ret != 0) {
            return;
        }
        switch (buf.type) {

            case UDATA_SERVICE_ACC: {
                accel_data_t *acc = (accel_data_t *)buf.payload;
                UDATA_SHOW_UINT_3(buf.type, (uint32_t)acc->timestamp, acc->data[0], acc->data[1], acc->data[2]);
                break;
            }


            case UDATA_SERVICE_MAG: {
                mag_data_t *mag = (mag_data_t *)buf.payload;
                UDATA_SHOW_UINT_3(buf.type, (uint32_t)mag->timestamp, mag->data[0], mag->data[1], mag->data[2]);
                break;
            }

            case UDATA_SERVICE_GYRO: {
                gyro_data_t *gyro = (gyro_data_t *)buf.payload;
                UDATA_SHOW_UINT_3(buf.type, (uint32_t)gyro->timestamp, gyro->data[0], gyro->data[1], gyro->data[2]);
                break;
            }

            case UDATA_SERVICE_ALS: {
                als_data_t *als = (als_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, als->timestamp, als->lux);
                break;
            }

            case UDATA_SERVICE_PS: {
                proximity_data_t *ps = (proximity_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, ps->timestamp, ps->present);
                break;
            }

            case UDATA_SERVICE_BARO: {
                barometer_data_t *baro = (barometer_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, baro->timestamp, baro->p);
                break;
            }

            case UDATA_SERVICE_TEMP: {
                temperature_data_t *temp = (temperature_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, temp->timestamp, temp->t);
                break;
            }

            case UDATA_SERVICE_UV: {
                uv_data_t *uv = (uv_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, uv->timestamp, uv->uvi);
                break;
            }

            case UDATA_SERVICE_HUMI: {
                humidity_data_t *humi = (humidity_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, humi->timestamp, humi->h);
                break;
            }

            case UDATA_SERVICE_HALL: {
                hall_data_t *hall = (hall_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, hall->timestamp, hall->hall_level);
                break;
            }
            case UDATA_SERVICE_HR: {
                heart_rate_data_t *heart = (heart_rate_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, heart->timestamp, heart->hear_rate);
                break;
            }

            case UDATA_SERVICE_GPS: {
                gps_data_t *gps = (gps_data_t *)buf.payload;
                UDATA_SHOW_FLOAT_3(buf.type, (uint32_t)gps->timestamp, gps->lat, gps->lon, gps->elv);
                break;
            }


            default:
                break;

        }

    }
}


#else

void uData_report_demo(sensor_msg_pkg_t *msg)
{
    udata_pkg_t buf;
    if ((msg == NULL)) {
        return;
    }

    if (msg->cmd == UDATA_MSG_REPORT_PUBLISH) {
        int ret = 0;
        ret = uData_report_publish(msg, &buf);
        if (ret != 0) {
            return;
        }
        switch (buf.type) {

            case UDATA_SERVICE_ACC: {
                accel_data_t *acc = (accel_data_t *)buf.payload;
                UDATA_SHOW_UINT_3(buf.type, (uint32_t)acc->timestamp, acc->data[0], acc->data[1], acc->data[2]);
                break;
            }


            case UDATA_SERVICE_MAG: {
                mag_data_t *mag = (mag_data_t *)buf.payload;
                UDATA_SHOW_UINT_3(buf.type, (uint32_t)mag->timestamp, mag->data[0], mag->data[1], mag->data[2]);
                break;
            }

            case UDATA_SERVICE_GYRO: {
                gyro_data_t *gyro = (gyro_data_t *)buf.payload;
                UDATA_SHOW_UINT_3(buf.type, (uint32_t)gyro->timestamp, gyro->data[0], gyro->data[1], gyro->data[2]);
                break;
            }

            case UDATA_SERVICE_ALS: {
                als_data_t *als = (als_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, als->timestamp, als->lux);
                break;
            }

            case UDATA_SERVICE_PS: {
                proximity_data_t *ps = (proximity_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, ps->timestamp, ps->present);
                break;
            }

            case UDATA_SERVICE_BARO: {
                barometer_data_t *baro = (barometer_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, baro->timestamp, baro->p);
                break;
            }

            case UDATA_SERVICE_TEMP: {
                temperature_data_t *temp = (temperature_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, temp->timestamp, temp->t);
                break;
            }

            case UDATA_SERVICE_UV: {
                uv_data_t *uv = (uv_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, uv->timestamp, uv->uvi);
                break;
            }

            case UDATA_SERVICE_HUMI: {
                humidity_data_t *humi = (humidity_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, humi->timestamp, humi->h);
                break;
            }

            case UDATA_SERVICE_HALL: {
                hall_data_t *hall = (hall_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, hall->timestamp, hall->hall_level);
                break;
            }
            case UDATA_SERVICE_HR: {
                heart_rate_data_t *heart = (heart_rate_data_t *)buf.payload;
                UDATA_SHOW_UINT_1(buf.type, heart->timestamp, heart->hear_rate);
                break;
            }

            case UDATA_SERVICE_GPS: {
                gps_data_t *gps = (gps_data_t *)buf.payload;
                UDATA_SHOW_FLOAT_3(buf.type, (uint32_t)gps->timestamp, gps->lat, gps->lon, gps->elv);
                break;
            }


            default:
                break;

        }

    }
}


#endif
int udata_sample(void)
{
    int ret = 0;
#ifdef UDATA_YLOOP

    aos_register_event_filter(EV_UDATA, uData_report_demo, NULL);
#else
    ret = uData_register_msg_handler(uData_report_demo);
    LOG("uData_queue_registerslot service_dtc_handle ret=%d\n", ret);
    if (ret == -1) {
        LOG("error occur reg uData_report_demo \n");
        return ret;
    }
	        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);

#endif
    ret = uData_subscribe(UDATA_SERVICE_TEMP);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);

    return 0;
}

int application_start(int argc, char **argv)
{

#ifdef DATA_TO_CLOUD
#ifdef CONFIG_PRINT_HEAP
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
#endif

#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);
#endif

#ifdef WITH_SAL
    sal_init();
#endif


    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();
    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);

#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&ncmd);
#endif
    set_iotx_info();
    aos_task_new("netmgr", start_netmgr, NULL, 4096);
#endif

    udata_sample();

#ifdef LITTLEVGL_DISPLAY
    sensor_display_init();
#endif

    aos_loop_run();

    return 0;
}
