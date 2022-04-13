/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_DIP_H__
#define __APP_DIP_H__

#ifdef __cplusplus
extern "C" {
#endif

//the mobile connect type
typedef enum {
    MOBILE_CONNECT_IDLE,
    MOBILE_CONNECT_IOS,
    MOBILE_CONNECT_ANDROID,
} MOBILE_CONN_TYPE_E;

void app_dip_init(void);

#ifdef __cplusplus
}
#endif

#endif

