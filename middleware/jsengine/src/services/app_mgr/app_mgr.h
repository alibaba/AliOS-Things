/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef APP_MGR_H
#define APP_MGR_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* complete = 0 success, 1 check success(EOF),  -1 check failed */
typedef int (*write_js_cb_t)(const char *filename, int32_t file_size,
                             int32_t type, int32_t offset, uint8_t *buf,
                             int32_t buf_len, int32_t complete);

/* write js file into filesystem, callback function */
int write_app_pack(const char *filename, int32_t file_size, int32_t type,
                   int32_t offset, uint8_t *buf, int32_t buf_len,
                   int32_t complete);

/* js pack parser */
void apppack_init(write_js_cb_t cb);

int apppack_update(uint8_t *ptr, int size);

void apppack_final();

/* report process state to webIDE */
void apppack_post_process_state();

/* get apppack from URL */
typedef int (*download_js_cb_t)(uint8_t *buf, int32_t buf_len);
int apppack_download(char *url, download_js_cb_t func);

/* get apppack and upgrade from URL */
int apppack_upgrade(char *url);

#ifdef LINUXOSX
typedef struct {
    char *url;
    int file_size;
    char md5[32 + 1];
    int type;
} upgrade_image_param_t;
int simulator_upgrade(upgrade_image_param_t *p_info);
#endif

/* flag: load index.js */
#define BoneFlag "BoneFlag"

/* flag: device certificate */
#define DeviceSpec "DeviceSpec"

void app_mgr_set_boneflag(int enable);
int app_mgr_get_boneflag();

void app_mgr_set_devicespec(char *jsonstr);
int app_mgr_get_devicespec(char *jsonstr, int jsonstrlen);

int app_mgr_open_file(const char *targetname);

#endif /* APP_MGR_H */
