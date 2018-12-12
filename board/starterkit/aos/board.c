#include "aos/hal/flash.h"
#include <aos/kernel.h>

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

static void board_partition_init()
{
    hal_partitions[HAL_PARTITION_APPLICATION].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_description      = "Application";
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr       = 0x08000000;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length           = 0x32000;    //200k bytes
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description      = "PARAMETER1";
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr       = 0x08032000;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length           = 0x1000; // 4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description      = "PARAMETER2";
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr       = 0x08033000;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length           = 0x2000; //8k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_description      = "PARAMETER4";
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_start_addr       = 0x08035000;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_length           = 0x1000; //4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_CUSTOM_1].partition_owner               = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_CUSTOM_1].partition_description         = "CUSTOM1";
    hal_partitions[HAL_PARTITION_CUSTOM_1].partition_start_addr          = 0x08036000;
    hal_partitions[HAL_PARTITION_CUSTOM_1].partition_length              = 0xA000; //40k bytes
    hal_partitions[HAL_PARTITION_CUSTOM_1].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
}

void board_init(void)
{
    board_partition_init();

    board_cli_init();

}
