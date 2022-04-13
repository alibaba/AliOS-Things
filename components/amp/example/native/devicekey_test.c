/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "amp_fs.h"
#include "infra_defs.h"

#define DEVICE_SECRET_LEN   32
#define DEVICESECRET_BACKUP "device_secret"
#define DEVICE_SECRET       "p92hx5ygEnZDdhCivTzLQDSSX87TXfQr"


int main()
{
    char buffer[IOTX_DEVICE_SECRET_LEN] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN] = {0};
    int buffer_len = IOTX_DEVICE_SECRET_LEN;

    printf("Please input your device secret");
    fflush(stdin);
    while (fgets(buffer, sizeof(buffer), stdin)) {
        printf("the input length is: %d", strlen(buffer));
        if (strlen(buffer) != DEVICE_SECRET_LEN + 1) {
            printf("invalid device secret length");
            break;
        }
        HAL_Kv_Set(DEVICESECRET_BACKUP, buffer, strlen(buffer) - 1, 1);
        HAL_Kv_Get(DEVICESECRET_BACKUP, device_secret, &buffer_len);
        printf("device secret store %s", device_secret);
        break;
    }

    return 0;
}