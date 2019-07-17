/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef APP_MGR_H
#define APP_MGR_H

#ifdef USE_FREERTOS
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mbedtls/aes.h"
#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#else
#include "mbedtls/md5.h"
#endif

/*
    js应用程序包  apppack
    js应用程序包解密、校验、拆分处理
*/

/* complete = 0 正常, 1 校验正确(文件结束),  -1 md5校验失败 */
typedef int (*write_js_cb_t)(const char *filename, int32_t file_size,
                             int32_t type, int32_t offset, uint8_t *buf,
                             int32_t buf_len, int32_t complete);

/* 回调函数，写文件至spiffs分区 */
int write_app_pack(const char *filename, int32_t file_size, int32_t type,
                   int32_t offset, uint8_t *buf, int32_t buf_len,
                   int32_t complete);

/* js pack 文件处理 */
void apppack_init(write_js_cb_t cb);

int apppack_update(uint8_t *ptr, int size);

void apppack_final();

/* 通知IDE处理进度 */
void apppack_post_process_state();

/* 从网络地址URL下载app pack文件 */
typedef int (*download_js_cb_t)(uint8_t *buf, int32_t buf_len);
int apppack_download(char *url, download_js_cb_t func);

/* 从URL下载并升级app pack, 异步函数 */
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

/* flag: 启动时是否加载index.js */
#define BoneFlag "BoneFlag"

/* flag: 记录上云三要素 */
#define DeviceSpec "DeviceSpec"

void app_mgr_set_boneflag(int enable);
int app_mgr_get_boneflag();

void app_mgr_set_devicespec(char *jsonstr);
int app_mgr_get_devicespec(char *jsonstr, int jsonstrlen);

int app_mgr_open_file(const char *targetname);

#endif /* APP_MGR_H */
