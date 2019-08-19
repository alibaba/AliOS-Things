/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef QXWZ_ENABLED

#include "ulog/ulog.h"
#include "qxwz_types.h"
#include "qxwz_status.h"
#include "qxwz_sdk.h"
#include "board.h"
#include "ulocation_qxwz_common.h"
#include "ulocation_qxwz_service.h"

static uint8_t auth_flag                  = 0;
static uint8_t s_gga[REPORT_GGA_DATA_SIZE]={0};

static int32_t receive_iprtcm(qxwz_void_t *rtcm, qxwz_u32_t len, qxwz_data_type_e type)
{
    int32_t ret = -1;

    LOGD("uLocation-qxwz", "got ip rtcm!");
    if (rtcm == NULL) {
        LOGD("uLocation-qxwz", "rtcm is null!");
        return ret;
    }
    if (type == RTCM_TYPE_RAW) {
        ret = qxwz_rtcm_send(rtcm, len);
    }

    return ret;
}

static void receive_status(qxwz_s32_t status)
{
    LOGD("uLocation-qxwz", "got rtcm status = %d!", status);
    if (status == QXWZ_STATUS_NTRIP_USER_IDENTIFY_SUCCESS) {
        auth_flag = 1;
    }
}

qxwz_data_response_t data_res = {
    receive_iprtcm,
    NULL
};

qxwz_status_response_t status_res = {
    receive_status
};

int ulocation_qianxun_service(ulocation_qxwz_usr_config_t *usr_config, ulocation_gga_info *location)
{
    int s_current_time = 0;
    uint32_t i         = 0;
    int ret            = 0;

    s_current_time = time(NULL);

    ret = ulocation_qxwz_init(GGA_DATA_PORT, GGA_DATA_BAUD);  /* need modified board logic port name */
    if (ret != 0) {
        LOGE("uLocation-qxwz", "ulocation qianxun init failed!");
        return ret;
    }

    ret = qxwz_setting(usr_config, QXWZ_FALSE);
    if (ret != 0) {
        LOGE("uLocation-qxwz", "ulocation qianxun user config failed!");
        return ret;
    }

    ret = qxwz_start(&data_res, &status_res);
    if (0 != ret) {
        LOGE("uLocation-qxwz", "ulocation qianxun service init failed!");
        return ret;
    }

    while(1) {
        ret = qxwz_tick(s_current_time);
        if (ret < 0) {
            LOGE("uLocation-qxwz", "qxwz tick is %d!", ret);
            return ret;
        }
        aos_msleep(GGA_DATA_PERIOD);
        s_current_time += 1;
        qxwz_soc_recv_handler();

        if (auth_flag == 1 ) {
            LOGD("uLocation-qxwz", "qxwz auth success!");
            ret = ulocation_qxwz_getgga(s_gga);
            if (ret < 0) {
                LOGE("uLocation-qxwz", "gga data get error!");
            }
            qxwz_send_data(s_gga, strlen(s_gga) + 1, UDATA_GGA);
            ret = ulocation_ggainfo_parse(s_gga, location);
            if (ret != 0) {
                return ret;
            }
            show_ggainfo(location);
        }
    }

    return ret;
}

#endif

