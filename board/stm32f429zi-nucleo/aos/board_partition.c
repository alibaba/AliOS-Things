/**
 ******************************************************************************
 * @file    board_partition.c
 * @author  MCU China FAE team
 * @version 1.0
 * @date    26/12/2018
 * @brief   aos porting layer
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 ******************************************************************************
 */
#include "hal/soc/soc.h"
#include <aos/kernel.h>

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

void board_init(void) {
  hal_partitions[HAL_PARTITION_APPLICATION].partition_owner =
      HAL_FLASH_EMBEDDED;
  hal_partitions[HAL_PARTITION_APPLICATION].partition_description =
      "Application";
  hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr = 0x08000000;
  hal_partitions[HAL_PARTITION_APPLICATION].partition_length =
      0x40000; // 256K bytes. STM32F429 provide 1M bytes per bank, but it takes too long to erase for certification.
  hal_partitions[HAL_PARTITION_APPLICATION].partition_options =
      PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

  hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner =
      HAL_FLASH_EMBEDDED;
  hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description =
      "PARAMETER1";
  hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr = 0x080C0000;
  hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length =
      0x2000; // 8k bytes
  hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options =
      PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

  hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner =
      HAL_FLASH_EMBEDDED;
  hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description =
      "PARAMETER2";
  hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr = 0x080E0000;
  hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length =
      0x2000; // 8k bytes
  hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options =
      PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

  hal_partitions[HAL_PARTITION_OTA_TEMP].partition_owner = HAL_FLASH_EMBEDDED;
  hal_partitions[HAL_PARTITION_OTA_TEMP].partition_description = "OTA Storage";
  hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr = 0x08100000;
  hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length =
      0x40000; // 256k bytes
  hal_partitions[HAL_PARTITION_OTA_TEMP].partition_options =
      PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

  hal_partitions[HAL_PARTITION_PARAMETER_3].partition_owner =
      HAL_FLASH_EMBEDDED;
  hal_partitions[HAL_PARTITION_PARAMETER_3].partition_description =
      "PARAMETER3";
  hal_partitions[HAL_PARTITION_PARAMETER_3].partition_start_addr = 0x081C0000;
  hal_partitions[HAL_PARTITION_PARAMETER_3].partition_length =
      0x2000; // 8k bytes
  hal_partitions[HAL_PARTITION_PARAMETER_3].partition_options =
      PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

  hal_partitions[HAL_PARTITION_PARAMETER_4].partition_owner =
      HAL_FLASH_EMBEDDED;
  hal_partitions[HAL_PARTITION_PARAMETER_4].partition_description =
      "PARAMETER4";
  hal_partitions[HAL_PARTITION_PARAMETER_4].partition_start_addr = 0x081E0000;
  hal_partitions[HAL_PARTITION_PARAMETER_4].partition_length =
      0x2000; // 8k bytes
  hal_partitions[HAL_PARTITION_PARAMETER_4].partition_options =
      PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
}
