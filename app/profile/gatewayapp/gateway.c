/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "aos/uData.h"

#ifdef DATA_TO_CLOUD

#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#endif/*end DATA_TO_CLOUD*/


#define GATEWAY_PRINT    printf

#define GATEWAY_SHOW_UINT_1(TYPE,TIME,DATA1) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%f)\n", (float)(DATA1)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%d)\n", (unsigned int)(TIME)); \
}while(0);

#define GATEWAY_SHOW_UINT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%d) (%d) (%d)\n", (int)(DATA1),(int)(DATA2),(int)(DATA3)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%d)\n", (unsigned int)(TIME)); \
}while(0);

#define GATEWAY_SHOW_FLOAT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%f) (%f) (%f)\n", (DATA1),(DATA2),(DATA3)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%d)\n", (unsigned int)(TIME)); \
}while(0);

#define GATEWAY_SHOW_UINT_7(TYPE,TIME,DATA1,DATA2,DATA3,DATA4,DATA5,DATA6,DATA7) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%d) (%d) (%d) (%d) (%d) (%d) (%d)\n", (DATA1),(DATA2),(DATA3),(DATA4),(DATA5),(DATA6),(DATA7)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%d)\n", (uint32_t)(TIME)); \
}while(0);


#ifdef DATA_TO_CLOUD
static int linkkit_started = 0;
static int awss_running = 0;
extern int linkkit_app(void);
static void reboot_system(void *parms);

int awss_success_notify();
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
        //clear_wifi_ssid();
        return;
    }

    if (awss_running) {
#ifdef AWSS_NEED_REBOOT
        aos_post_delayed_action(200, reboot_system, NULL);
#endif
        return;
    }
    if (!linkkit_started) {
#ifdef CONFIG_YWSS
        awss_success_notify();
#endif
        gw_app();
#ifdef CONFIG_YWSS
        awss_success_notify();
#endif
        linkkit_started = 1;
    }
}

static void start_netmgr(void *p)
{
#ifdef CONFIG_YWSS
    netmgr_start(true);
#else
    netmgr_start(false);
#endif
}

static void reboot_system(void *parms)
{
    LOG("reboot system");
    aos_reboot();
}

#ifdef CONFIG_YWSS
extern int awss_report_reset();

static void cloud_service_event(input_event_t *event, void *priv_data)
{
    static uint8_t awss_reported = 0;
    if (event->type != EV_YUNIO) {
        return;
    }

    LOG("cloud_service_event %d", event->code);

    if (event->code == CODE_YUNIO_ON_CONNECTED) {
        LOG("user sub and pub here");
        if (!awss_reported) {
            awss_report_cloud();
            awss_reported = 1;
        }
        return;
    }

    if (event->code == CODE_YUNIO_ON_DISCONNECTED) {
    }
}

void do_awss_active()
{
    LOG("do_awss_active %d\n", awss_running);
    awss_running = 1;
    awss_config_press();
}

static void do_awss_reset()
{
    if (linkkit_started) {
        aos_task_new("reset", awss_report_reset, NULL, 2048);
    }
    netmgr_clear_ap_config();
    LOG("SSID cleared. Please reboot the system.\n");
    aos_post_delayed_action(1000, reboot_system, NULL);
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

static struct cli_command resetcmd = {
    .name = "reset",
    .help = "factory reset",
    .function = handle_reset_cmd
};

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_active, NULL);
}

static struct cli_command ncmd = {
    .name = "active_awss",
    .help = "active_awss [start]",
    .function = handle_active_cmd
};
#endif
#endif
#endif


void gateway_report_demo(sensor_msg_pkg_t *msg)
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
                GATEWAY_SHOW_FLOAT_3(buf.type, (uint32_t)acc->timestamp, ((float)acc->data[0]/1000), ((float)acc->data[1]/1000), ((float)acc->data[2]/1000));
                break;
            }


            case UDATA_SERVICE_MAG: {
                mag_data_t *mag = (mag_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_3(buf.type, (uint32_t)mag->timestamp, mag->data[0], mag->data[1], mag->data[2]);
                break;
            }

            case UDATA_SERVICE_GYRO: {
                gyro_data_t *gyro = (gyro_data_t *)buf.payload;
                GATEWAY_SHOW_FLOAT_3(buf.type, (uint32_t)gyro->timestamp, ((float)gyro->data[0]/1000000), ((float)gyro->data[1]/1000000), ((float)gyro->data[2]/1000000));
                break;
            }

            case UDATA_SERVICE_ALS: {
                als_data_t *als = (als_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, als->timestamp, als->lux);
                break;
            }

            case UDATA_SERVICE_PS: {
                proximity_data_t *ps = (proximity_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, ps->timestamp, ps->present);
                break;
            }

            case UDATA_SERVICE_BARO: {
                barometer_data_t *baro = (barometer_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, baro->timestamp, baro->p);
                break;
            }

            case UDATA_SERVICE_TEMP: {
                temperature_data_t *temp = (temperature_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, temp->timestamp, ((float)temp->t/10));
                break;
            }

            case UDATA_SERVICE_UV: {
                uv_data_t *uv = (uv_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, uv->timestamp, uv->uvi);
                break;
            }

            case UDATA_SERVICE_HUMI: {
                humidity_data_t *humi = (humidity_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, humi->timestamp, ((float)humi->h/10));
                break;
            }

            case UDATA_SERVICE_HALL: {
                hall_data_t *hall = (hall_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, hall->timestamp, hall->hall_level);
                break;
            }
            case UDATA_SERVICE_HR: {
                heart_rate_data_t *heart = (heart_rate_data_t *)buf.payload;
                GATEWAY_SHOW_UINT_1(buf.type, heart->timestamp, heart->hear_rate);
                break;
            }

            case UDATA_SERVICE_GPS: {
                gps_data_t *gps = (gps_data_t *)buf.payload;
                GATEWAY_SHOW_FLOAT_3(buf.type, (uint32_t)gps->timestamp, gps->lat, gps->lon, gps->elv);
                break;
            }

             case UDATA_SERVICE_RTC: {
	            rtc_data_t *rtc = (rtc_data_t *)buf.payload;
	            GATEWAY_SHOW_UINT_7(buf.type, (uint32_t)rtc->timestamp, rtc->year, rtc->month, rtc->date, rtc->day, rtc->hours, rtc->minutes, rtc->seconds);
	            break;
            }

            default:
                break;

        }

    }
}


int gateway_sample(void)
{
    int ret = 0;

    ret = uData_register_msg_handler(gateway_report_demo);
    LOG("uData_queue_registerslot service_dtc_handle ret=%d\n", ret);
    if (ret == -1) {
        LOG("error occur reg uData_report_demo \n");
        return ret;
    }

    ret = uData_subscribe(UDATA_SERVICE_ACC);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = uData_subscribe(UDATA_SERVICE_GYRO);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = uData_subscribe(UDATA_SERVICE_RTC);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = uData_subscribe(UDATA_SERVICE_TEMP);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = uData_subscribe(UDATA_SERVICE_HUMI);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    return 0;
}

int application_start(int argc, char **argv)
{
    int ret;
    ret = uData_main();
    if (unlikely(ret)) {
        return -1;
    }

#ifdef DATA_TO_CLOUD
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_SAL
    sal_init();
#endif
    aos_set_log_level(AOS_LL_DEBUG);

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    netmgr_init();
    
#ifdef CONFIG_YWSS
    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);
#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&ncmd);
#endif
#endif
        
#ifdef CONFIG_NET_LWIP
    lwip_tcpip_init();

    /* Initialize webserver */
    http_server_netconn_init();
#endif

    aos_task_new("netmgr", start_netmgr, NULL, 4096);

#endif
    gateway_sample();

    aos_loop_run();

    return 0;
}
