/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_boot.h"

static amp_boot_cmd_t g_amp_boot_cmd[] = {
    [AMP_BOOT_CMD_EXIT] = {
        "cmd_exit"
    },
    [AMP_BOOT_CMD_QUERY_IMEI] = {
        "cmd_query_imei"
    },
    [AMP_BOOT_CMD_QUERY_SEC] = {
        "cmd_query_sec"
    },
    [AMP_BOOT_CMD_FLASH_SEC] = {
        "cmd_flash_sec"
    },
    [AMP_BOOT_CMD_FLASH_JS] = {
        "cmd_flash_js"
    },
    [AMP_BOOT_CMD_FLASH_KV] = {
        "cmd_flash_kv"
    }
};

int amp_boot_get_cmd(int timeout_ms)
{
    int32_t cmd_id    = 0;
    int32_t recv_len  = 0;
    char cmd_buff[32] = {0};

    recv_len = amp_boot_uart_recv_line(cmd_buff, 32, timeout_ms);
    if (recv_len <= 0) {
        return AMP_BOOT_CMD_NULL;
    }
    for (cmd_id = 0; cmd_id < AMP_BOOT_CMD_MAX; cmd_id++) {
        if(recv_len != strlen(g_amp_boot_cmd[cmd_id].cmd_str)) {
            continue;
        }
        if (strncmp(cmd_buff, g_amp_boot_cmd[cmd_id].cmd_str, recv_len) == 0) {
            return cmd_id;
        }
    }
    return AMP_BOOT_CMD_ERROR;
}

void amp_boot_cmd_begin(int cmd)
{
    amp_boot_uart_send_str(">>>>");
    amp_boot_uart_send_str(g_amp_boot_cmd[cmd].cmd_str);
}

void amp_boot_cmd_end(int cmd)
{
    amp_boot_uart_send_str("<<<<");
    amp_boot_uart_send_str(g_amp_boot_cmd[cmd].cmd_str);
}