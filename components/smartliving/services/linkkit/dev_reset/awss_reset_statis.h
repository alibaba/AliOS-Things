/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_RESET_STATIS_H__
#define __AWSS_RESET_STATIS_H__

#ifndef AWSS_SUPPORT_DEV_RESET_STATIS
#define AWSS_SUPPORT_DEV_RESET_STATIS
#endif

enum {
    AWSS_RST_STATIS_START,
    AWSS_RST_STATIS_SUC,
};

#ifdef AWSS_SUPPORT_DEV_RESET_STATIS
struct awss_statis_dev_rst_t {
    uint32_t dev_rst_cnt;        /* the count of reset sync */
    uint32_t dev_rst_suc;        /* the successful count of reset sync */
    uint32_t dev_rst_time_mean;  /* the mean time of reset sync */
    uint32_t dev_rst_time_max;   /* the max time of reset sync */
    uint32_t dev_rst_time_min;   /* the min time of reset sync */
    uint32_t dev_rst_start;      /* the start time to sync reset */
    uint32_t dev_rst_end;        /* the end time of reset sync */
};  /* statistics for reset sync */


void awss_rst_update_statis(int type);
void awss_rst_disp_statis();

#define AWSS_RST_UPDATE_STATIS(type) awss_rst_update_statis(type)
#define AWSS_RST_DISP_STATIS()       awss_rst_disp_statis()
#else
#define AWSS_RST_UPDATE_STATIS(type)
#define AWSS_RST_DISP_STATIS()
#endif
#endif
