#include "aos/hal/flash.h"
#include <stddef.h>

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] = {
    [HAL_PARTITION_BOOTLOADER] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "boot1",
            .partition_start_addr  = 0x0,
            .partition_length      = 0x10000, // 64k bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_PARAMETER_3] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "boot_info",
            .partition_start_addr  = 0x10000, // boot information need protect
            .partition_length      = 0x2000,  // 8k bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_BOOT1] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "boot2A",
            .partition_start_addr  = 0x12000,
            .partition_length      = 0x18000, // 64k bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_APPLICATION] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "RTOSA",
            .partition_start_addr  = 0x2A000,
            .partition_length      = 0x578000, // 5.5M bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_BOOT1_REDUND] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "boot2B",
            .partition_start_addr  = 0x5A2000,
            .partition_length      = 0x18000, // 64k bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_OTA_TEMP] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "RTOSB",
            .partition_start_addr  = 0x5BA000,
            .partition_length      = 0x578000, // 5.5M bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_LITTLEFS] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "littleFS", // for little fs module
            .partition_start_addr  = 0xB32000,
            .partition_length      = 0x4AE000,
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_PARAMETER_4] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "boot1_sec",
            .partition_start_addr  = 0xFE0000,
            .partition_length      = 0x10000, // 64k bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_PARAMETER_1] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "boot2_info",
            .partition_start_addr  = 0xFF0000,
            .partition_length      = 0x1000, // 4k bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_PARAMETER_2] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "KV", // for KV module
            .partition_start_addr  = 0xFF1000,
            .partition_length      = 0xD000, // K bytes
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_ENV] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "factory",
            .partition_start_addr  = 0xFFE000,
            .partition_length      = 0x2000,
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_ENV_REDUND] = {
            .partition_owner       = HAL_FLASH_EMBEDDED,
            .partition_description = "factory_backup",
            .partition_start_addr  = 0xFFE000,
            .partition_length      = 0x2000,
            .partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
};

/*  Declare a constant to indicate the defined partitions amount */
const size_t hal_partitions_amount =
    (sizeof(hal_partitions) / sizeof(hal_logic_partition_t));
