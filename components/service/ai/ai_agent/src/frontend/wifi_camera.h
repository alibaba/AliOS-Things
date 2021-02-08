/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */
#ifndef _WIFI_CAMERA_H_
#define _WIFI_CAMERA_H_
#include <stdint.h>

int32_t wificamera_init(void);
int32_t wificamera_uninit(void);
int32_t wificamera_connect(char *url);
void wificamera_disconnect(char *url);
int32_t wificamera_get_stream(char *url);
int32_t wificamera_save_image(char *path);
int32_t wificamera_show_image(void);
#endif //_WIFI_CAMERA_H_