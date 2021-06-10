/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_BOOT_CMD_H
#define AMP_BOOT_CMD_H

enum {
    AMP_BOOT_CMD_NULL,
    AMP_BOOT_CMD_EXIT,
    AMP_BOOT_CMD_QUERY_IMEI,
    AMP_BOOT_CMD_QUERY_SEC,
    AMP_BOOT_CMD_FLASH_SEC,
    AMP_BOOT_CMD_FLASH_JS,
    AMP_BOOT_CMD_FLASH_KV,
    AMP_BOOT_CMD_MAX,
    AMP_BOOT_CMD_ERROR = 127,
};

typedef struct {
    char cmd_str[32];
} amp_boot_cmd_t;

int amp_boot_get_cmd(int timeout_ms);
void amp_boot_cmd_begin(int cmd);
void amp_boot_cmd_end(int cmd);

#endif