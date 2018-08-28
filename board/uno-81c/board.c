#include "hal/soc/soc.h"
#include <aos/kernel.h>

#define HAL_PARTITION_SYS_DATA HAL_PARTITION_MAX

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
#if 0
    [HAL_PARTITION_BOOTLOADER] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Bootloader",
        .partition_start_addr       = 0x18001000,
        .partition_length           = 0xF000,    //60k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Application",
        .partition_start_addr       = 0x18004000,
        .partition_length           = 0x1f4000,//2M bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#if 1
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "OTA Storage",
        .partition_start_addr       = 0x181f8000, //0x18100000,
        .partition_length           = 0x1f3000, // 2M bytes //0x100000, 1M bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
    [HAL_PARTITION_SYS_DATA] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "SYS Data",
        .partition_start_addr       = 0x183eb000,
        .partition_length           = 0x1000,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x183ec000,//0x180C0000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x183ed000,//0x180C1000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x183ef000,//0x180C3000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x183F0000, //0x180C4000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

void board_init(void)
{
}
