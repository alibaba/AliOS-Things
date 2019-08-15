/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_bin.h"

/* if more app should be supported, define them in kernel linker script */
extern char app1_info_addr;
extern char app2_info_addr;
extern char app3_info_addr;

const preamble_t *g_app_info[MAX_APP_BINS] =  {
    (preamble_t*) &app1_info_addr,
    (preamble_t*) &app2_info_addr,
    (preamble_t*) &app3_info_addr,
};

const int g_max_app_cnt = MAX_APP_BINS;

