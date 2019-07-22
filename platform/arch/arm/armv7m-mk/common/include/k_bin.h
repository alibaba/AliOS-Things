/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __K_BIN_H
#define __k_BIN_H

#include "preamble.h"

#ifndef RHINO_CONFIG_MAX_APP_BINS
#define MAX_APP_BINS   (3) /**< by default, support 3 app bins */
#else
#if (RHINO_CONFIG_MAX_APP_BINS > 32)
#error "app bins count beyonds 32"
#endif
#define MAX_APP_BINS RHINO_CONFIG_MAX_APP_BINS
#endif

extern const preamble_t *g_app_info[];

extern const int g_max_app_cnt;

#endif /* __K_BIN_H */

