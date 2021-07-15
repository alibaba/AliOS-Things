/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iot_import.h"
#include "iotx_log.h"
#include "awss_reset_statis.h"

#ifdef AWSS_SUPPORT_DEV_RESET_STATIS
static struct awss_statis_dev_rst_t g_dev_rst_statis;

#define RST_CNT    g_dev_rst_statis.dev_rst_cnt
#define RST_SUC    g_dev_rst_statis.dev_rst_suc
#define RST_TMEAN  g_dev_rst_statis.dev_rst_time_mean
#define RST_TMIN   g_dev_rst_statis.dev_rst_time_min
#define RST_TMAX   g_dev_rst_statis.dev_rst_time_max
#define RST_START  g_dev_rst_statis.dev_rst_start
#define RST_END    g_dev_rst_statis.dev_rst_end

void awss_rst_update_statis(int type)
{
    uint32_t time = HAL_UptimeMs();
    switch (type) {
        case AWSS_RST_STATIS_START:
            RST_CNT ++;
            RST_START = time;
            break;
        case AWSS_RST_STATIS_SUC:
            RST_END = time;
            RST_SUC ++;
            time = (uint32_t)(RST_END - RST_START);
            if (RST_SUC > 0) {
                RST_TMEAN = (RST_TMEAN + time) / RST_SUC;
            } else {
                RST_SUC = 1;
                RST_TMEAN = time;
            }
            if (RST_TMIN == 0 || RST_TMIN > time)
                RST_TMIN = time;
            if (RST_TMAX == 0 || RST_TMAX < time)
                RST_TMAX = time;
            break;
        default:
            break;
    }
}

void awss_rst_disp_statis()
{
    log_debug("[RST]", "-------------------------DEV RESET STATIS----------------------------");
    log_debug("[RST]", "name\t\tmax\tmin\tmean\tcnt\tsuc");
    log_debug("[RST]", "Sync Reset     \t%u\t%u\t%u\t%u\t%u\t",
            RST_TMAX, RST_TMIN, RST_TMEAN, RST_CNT, RST_SUC);
    log_debug("[RST]", "---------------------------------------------------------------------");
}
#endif
