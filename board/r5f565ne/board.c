#include "platform.h"
#include "flash.h"
#include "board.h"
/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "bootloader",
	    .partition_start_addr       = 0xFFE00000,
	    .partition_length           = 0x40000,    //256k bytes FLASH_CF_BLOCK_62~FLASH_CF_BLOCK_69
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},

	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0xFFE80000,
	    .partition_length           = 0x78000,    //512k bytes FLASH_CF_BLOCK_46~FLASH_CF_BLOCK_61
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x00100000,
        .partition_length           = 0x1000, // 4k bytes	FLASH_DF_BLOCK_0
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x00101000,
        .partition_length           = 0x1000, //4k bytes	FLASH_DF_BLOCK_128
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0xFFE00000,
        .partition_length          = 0x100000, //512k bytes	FLASH_CF_BLOCK_22~FLASH_CF_BLOCK_37
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x00102000,
        .partition_length           = 0x1000, //4k bytes	FLASH_DF_BLOCK_256
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x00103000,
        .partition_length           = 0x1000, //4k bytes	FLASH_DF_BLOCK_384
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

void board_init(void)
{
}
