/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_DOWNLOAD_H_
#define OTA_DOWNLOAD_H_
#include <stdint.h>
#include "ota_transport.h"
#include "ota_update_manifest.h"
#include "md5.h"

int ota_download(char *url, write_flash_cb_t func, char *md5);
int check_md5(const char *buffer, const int32_t len);

uint32_t ota_get_update_breakpoint(void);
void save_state(uint32_t breakpoint, MD5_CTX *pMD5);
int ota_set_update_breakpoint(uint32_t offset);
int ota_get_last_MD5_context(MD5_CTX *ctx);
int ota_set_cur_MD5_context(MD5_CTX *ctx);
int ota_get_last_MD5(char *value);
int ota_set_cur_MD5(char *value);
#endif /* OTA_DOWNLOAD_H_ */
