/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "linklora.h"

void linklora_test_cli_cb(uint8_t cmd)
{
    switch (cmd) {
        case '?':
            LOG_PRINTF("1: tx 51 bytes in SF12\r\n");
            break;
        case 1:
            break;
        default:
           LOG_PRINTF("press ? for help\r\n");
           break;
    }
}
