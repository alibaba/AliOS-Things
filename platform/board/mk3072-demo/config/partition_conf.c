#include "aos/hal/flash.h"
#include <aos/kernel.h>
#include "lega_flash.h"

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];
extern const lega_logic_partition_t lega_partitions[];

void flash_partition_init(void)
{
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_owner            = lega_partitions[PARTITION_BOOTLOADER].partition_owner;
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_description      = lega_partitions[PARTITION_BOOTLOADER].partition_description;
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_start_addr       = lega_partitions[PARTITION_BOOTLOADER].partition_start_addr;
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_length           = lega_partitions[PARTITION_BOOTLOADER].partition_length;
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_options          = lega_partitions[PARTITION_BOOTLOADER].partition_options ;

    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner            = lega_partitions[PARTITION_PARAMETER_1].partition_owner;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description      = lega_partitions[PARTITION_PARAMETER_1].partition_description;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr       = lega_partitions[PARTITION_PARAMETER_1].partition_start_addr;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length           = lega_partitions[PARTITION_PARAMETER_1].partition_length;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options          = lega_partitions[PARTITION_PARAMETER_1].partition_options ;

    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner            = lega_partitions[PARTITION_PARAMETER_2].partition_owner ;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description      = lega_partitions[PARTITION_PARAMETER_2].partition_description;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr       = lega_partitions[PARTITION_PARAMETER_2].partition_start_addr;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length           = lega_partitions[PARTITION_PARAMETER_2].partition_length;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options          = lega_partitions[PARTITION_PARAMETER_2].partition_options;

    hal_partitions[HAL_PARTITION_APPLICATION].partition_owner            = lega_partitions[PARTITION_APPLICATION].partition_owner;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_description      = lega_partitions[PARTITION_APPLICATION].partition_description;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr       = lega_partitions[PARTITION_APPLICATION].partition_start_addr;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length           = lega_partitions[PARTITION_APPLICATION].partition_length;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options          = lega_partitions[PARTITION_APPLICATION].partition_options;

    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_owner               = lega_partitions[PARTITION_OTA_TEMP].partition_owner;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_description         = lega_partitions[PARTITION_OTA_TEMP].partition_description;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr          = lega_partitions[PARTITION_OTA_TEMP].partition_start_addr;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length              = lega_partitions[PARTITION_OTA_TEMP].partition_length;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_options             = lega_partitions[PARTITION_OTA_TEMP].partition_options;
}

