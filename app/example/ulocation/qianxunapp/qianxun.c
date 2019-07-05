/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <time.h>
#include <unistd.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "board.h"

#include "netmgr.h"
#include "qxwz_types.h"
#include "qxwz_sdk.h"
#include "qxwz_common.h"

#define GGA_DATA_SIZE        128
#define GGA_DATA_FREQ        3

#define QXWZ_APPKEY          "770302"
#define QXWZ_APPSECRET       "99094b805092d2e7e08f63bcde97a2a7a70bded81a1e0b0f6728dede7fc01902"
#define QXWZ_DEVICE_ID       "AliOS-Things2.1.1"
#define QXWZ_DEVICE_TYPE     "uLocation_qianxun"

static uint8_t s_gga[GGA_DATA_SIZE]={0};
static qxwz_started = 0;

static const qxwz_usr_config_t s_config = {
    QXWZ_APPKEY,     /* appKey */
    QXWZ_APPSECRET,  /* appSecret */
    QXWZ_DEVICE_ID,  /* device_ID */
    QXWZ_DEVICE_TYPE /* device_Type */
};

static int32_t receive_iprtcm(qxwz_void_t *rtcm, qxwz_u32_t len, qxwz_data_type_e type)
{
    int32_t ret = -1;
    LOGD("uLocation-qxwz", "got ip rtcm!\n");
    if (rtcm == NULL){
        LOGD("uLocation-qxwz", "rtcm is null!\n");
        return ret;
    }
    if (type == RTCM_TYPE_RAW){
        ret = qxwz_rtcm_send(rtcm, len);
    }
    return ret;
}

static void receive_status(qxwz_s32_t status)
{
    LOGD("uLocation-qxwz", "got rtcm status = %d!\n", status);
}

qxwz_data_response_t data_res = {
    receive_iprtcm,
    NULL
};

qxwz_status_response_t status_res = {
    receive_status
};

int32_t qianxun_service_sample()
{
    static int s_current_time = 0;
    static uint32_t i = 0;
    int32_t ret = 0;
    s_current_time = time(NULL);

    qxwz_setting(&s_config, QXWZ_FALSE);
    ret = qxwz_start(&data_res, &status_res);
    if(0 != ret){
        return ret;
    }

    while(1){
        ret = qxwz_tick(s_current_time);
        aos_msleep(GGA_DATA_PERIOD);
        s_current_time += 1;
        if(i > GGA_DATA_FREQ){
            qxwz_soc_recv_handler();
            qxwz_gga_get(s_gga, GGA_DATA_SIZE);
            ret = qxwz_send_data(s_gga, strlen(s_gga) + 1, UDATA_GGA);
            i = 0;
        }
        i++;
    }

    return ret;
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    int ret;
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
        return;
    }

    if (!qxwz_started){
        qianxun_service_sample();
        qxwz_started = 1;
    }
}


int application_start(int argc, char *argv[])
{
    int ret = -1;
    LOGD("uLocation-qxwz", "ulocation qianxun app!\n");
    ret = qxwz_gga_init(GGA_DATA_PORT, GGA_DATA_BAUD);  /* need modified board logic port name */
    if (ret != 0){
        LOGE("uLocation-qxwz", "ulocation qianxun app gga init failed!\n");
        return ret;
    }

#ifdef WITH_SAL
    sal_init();
#endif

    netmgr_init();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    netmgr_start(0);

    aos_loop_run();
    return 0;
}
