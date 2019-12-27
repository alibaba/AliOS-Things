/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/hardware.h>

#ifndef DAS_PLATFORM_AOS
#error "3080 only supported by AliOS Things"
#else

#include <aos/kernel.h>

#include "hal/wifi.h"

#define DEV_ID_LEN    6

size_t das_hal_firmware_version(char *buf, size_t size)
{
    char *version = NULL;
    size_t real_len = 0;

    version = SYSINFO_APP_VERSION;
    real_len = strlen(version);
    real_len += 1;
    if (size < real_len) {
        DAS_LOG("short buffer %d : %d\n", size, real_len);
        return 0;
    }

    memcpy(buf, version, real_len);
    return real_len;
}

size_t das_hal_device_id(char *buf, size_t size)
{
    int ret;
    char device_id[DEV_ID_LEN];

    if (size <= 2 * DEV_ID_LEN) {
        DAS_LOG("short buffer %d : %d\n", size, 2 * DEV_ID_LEN);
        return 0;
    }

    ret = hal_wifi_get_mac_addr(NULL, device_id);
    if (ret) {
        DAS_LOG("get mac addr fail\n");
        return 0;
    }

    das_hex2string(buf, size, device_id, DEV_ID_LEN);

    return (2 * DEV_ID_LEN);
}

#ifdef DAS_SERVICE_ROM_ENABLED

extern uint32_t __flash_text_start__;
extern uint32_t __flash_text_end__;

int das_hal_rom_info(das_rom_bank_t banks[DAS_ROM_BANK_NUMBER])
{

    banks[0].address = &__flash_text_start__;
    banks[0].size = &__flash_text_end__ - &__flash_text_start__;

    return DAS_ROM_BANK_NUMBER;
}

#endif /* DAS_SERVICE_ROM_ENABLED */

#endif

