/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * WiFi debugger.
 */
#ifndef __WIFI_DBG_H__
#define __WIFI_DBG_H__

#define WIFI_MAX_BOARD_NAME_LEN (33)

typedef struct wifidbg_config {
    wifi_mode_t mode;
    char board_name[WIFI_MAX_BOARD_NAME_LEN + 1];
} wifidbg_config_t;

int wifidbg_init(wifidbg_config_t *config);
void wifidbg_dispatch();

#endif /* __WIFIDBG_H__ */
