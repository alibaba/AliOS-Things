#include "aos/hal/flash.h"
#include <aos/kernel.h>

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

void board_init(void)
{
    hal_partitions[HAL_PARTITION_APPLICATION].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_description      = "Application";
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr       = 0x08000000;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length           = 0x100000;    //1024k bytes
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description      = "PARAMETER1";
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr       = 0x08100000;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length           = 0x4000; // 16k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description      = "PARAMETER2";
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr       = 0x08104000;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length           = 0x8000; //32k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_description      = "PARAMETER4";
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_start_addr       = 0x0810C000;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_length           = 0x4000; //16k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_description      = "PARAMETER3";
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_start_addr       = 0x08110000;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_length           = 0x10000; //64k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_SPIFFS].partition_owner                 = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_SPIFFS].partition_description           = "spiffs";
    hal_partitions[HAL_PARTITION_SPIFFS].partition_start_addr            = 0x08120000;
    hal_partitions[HAL_PARTITION_SPIFFS].partition_length                = 0x40000; //256K bytes
    hal_partitions[HAL_PARTITION_SPIFFS].partition_options               = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

}
