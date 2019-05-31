#include "lega_cm4.h"
#include "lega_flash.h"

/* Logic partition on flash devices */
const lega_logic_partition_t lega_partitions[] =
{
    [PARTITION_BOOTLOADER] =
    {
        .partition_owner            = FLASH_EMBEDDED,
        .partition_description      = "Bootloader",
        .partition_start_addr       = BOOTLOADER_FLASH_START_ADDR,
        .partition_length           = BOOTLOADER_MAX_SIZE,    //64k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [PARTITION_PARAMETER_1] =
    {
        .partition_owner            = FLASH_EMBEDDED,
        .partition_description      = "Info",
        .partition_start_addr       = INFO_FLASH_START_ADDR,
        .partition_length           = INFO_MAX_SIZE, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PARTITION_PARAMETER_2] =
    {
        .partition_owner            = FLASH_EMBEDDED,
        .partition_description      = "KV",
        .partition_start_addr       = KV_FLASH_START_ADDR,
        .partition_length           = KV_MAX_SIZE, //128k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PARTITION_APPLICATION] =
    {
        .partition_owner            = FLASH_EMBEDDED,
        .partition_description      = "Application",
        .partition_start_addr       = APP_FLASH_START_ADDR,
        .partition_length           = APP_MAX_SIZE, //768k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PARTITION_OTA_TEMP] =
    {
        .partition_owner           = FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = OTA_FLASH_START_ADDR,
        .partition_length          = OTA_MAX_SIZE, //768k bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    }
};
