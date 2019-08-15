/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_OTA_DOWNLOAD_H
#define BE_OTA_DOWNLOAD_H
#include <stdint.h>

typedef enum {
    OTA_REBOOT_FAILED     = -6,
    OTA_UPGRADE_FAILED    = -5,
    OTA_CHECK_FAILED      = -4,
    OTA_DECOMPRESS_FAILED = -3,
    OTA_DOWNLOAD_FAILED   = -2,
    OTA_INIT_FAILED       = -1,
    OTA_INIT              = 0,
    OTA_DOWNLOAD          = 1,
    OTA_DECOMPRESS        = 2,
    OTA_CHECK             = 3,
    OTA_UPGRADE           = 4,
    OTA_REBOOT            = 5,
    OTA_REBOOT_SUCCESS    = 6,
    OTA_CANCEL            = 7,
    OTA_MAX
} OTA_STATUS_T;

typedef enum {
    OTA_DOWNLOAD_RECV_FAIL   = -6,
    OTA_DOWNLOAD_SEND_FAIL   = -5,
    OTA_DOWNLOAD_SOCKET_FAIL = -4,
    OTA_DOWNLOAD_IP_FAIL     = -3,
    OTA_DOWNLOAD_URL_FAIL    = -2,
    OTA_DOWNLOAD_FAIL        = -1,
    OTA_DOWNLOAD_CONTINUE    = 0,
    OTA_DOWNLOAD_CANCEL      = 1,
    OTA_DOWNLOAD_FINISH      = 2
} OTA_DOWNLOAD_T;

typedef int (*write_flash_cb_t)(int32_t writed_size, uint8_t *buf,
                                int32_t buf_len, int type);

int ota_download(char *url, write_flash_cb_t func, char *md5);
int check_md5(const char *buffer, const int32_t len);

/*
uint32_t ota_get_update_breakpoint(void);
void save_state(uint32_t breakpoint, MD5_CTX *pMD5);
int ota_set_update_breakpoint(uint32_t offset);
int ota_get_last_MD5_context(MD5_CTX *ctx);
int ota_set_cur_MD5_context(MD5_CTX *ctx);
int ota_get_last_MD5(char *value);
int ota_set_cur_MD5(char *value); */
#endif /* BE_OTA_DOWNLOAD_H */
