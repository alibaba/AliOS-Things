/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef OTA_PLAT_CTR_H
#define OTA_PLAT_CTR_H
#include "ota/ota_service.h"
#if defined(__ICCARM__)
#define OTA_WEAK                __weak
#else
#define OTA_WEAK                __attribute__((weak))
#endif

int ota_rollback(void);
int hal_reboot_bank(void);
int ota_is_download_mode(void);
int ota_int(ota_boot_param_t *param);
int ota_clear(ota_boot_param_t *param);
int ota_set_boot(ota_boot_param_t *param);
int ota_write(unsigned int *off, char *in_buf, unsigned int in_buf_len);
int ota_read(unsigned int *off, char *out_buf, unsigned int out_buf_len);
const char *ota_version_get(unsigned char dev_type, char* dn);
#endif /*OTA_PLAT_CTR_H*/
