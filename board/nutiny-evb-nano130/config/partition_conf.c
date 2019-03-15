/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/flash.h"

#include "board.h"

/* Logic partition on flash devices */
#define DEF_APPLICATION_START_ADDR  FMC_APROM_BASE
#define DEF_APPLICATION_SIZE        (0x10000)                   //64k bytes
#define DEF_PARAMETER_1_START_ADDR  (DEF_APPLICATION_START_ADDR+DEF_APPLICATION_SIZE)
#define DEF_PARAMETER_1_SIZE        (FMC_FLASH_PAGE_SIZE*8)         //4k bytes
#define DEF_PARAMETER_2_START_ADDR  (DEF_PARAMETER_1_START_ADDR+DEF_PARAMETER_1_SIZE)
#define DEF_PARAMETER_2_SIZE        (FMC_FLASH_PAGE_SIZE*16)        //8k bytes
#define DEF_PARAMETER_3_START_ADDR  (DEF_PARAMETER_2_START_ADDR+DEF_PARAMETER_2_SIZE)
#define DEF_PARAMETER_3_SIZE        (FMC_FLASH_PAGE_SIZE*16)        //8k bytes
#define DEF_PARAMETER_4_START_ADDR  (DEF_PARAMETER_3_START_ADDR+DEF_PARAMETER_3_SIZE)
#define DEF_PARAMETER_4_SIZE        (FMC_FLASH_PAGE_SIZE*16)        //8k bytes
#define DEF_CUSTOM_1_START_ADDR     (DEF_PARAMETER_4_START_ADDR+DEF_PARAMETER_4_SIZE)
#define DEF_CUSTOM_1_SIZE           (FMC_FLASH_PAGE_SIZE*16)    //8k bytes
#define DEF_CUSTOM_2_START_ADDR     (DEF_CUSTOM_1_START_ADDR+DEF_CUSTOM_1_SIZE)
#define DEF_CUSTOM_2_SIZE           (FMC_FLASH_PAGE_SIZE*16)    //8k bytes

const hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX] = {
    
  [HAL_PARTITION_APPLICATION] = {
        .partition_owner        = HAL_FLASH_EMBEDDED,
        .partition_description  = "Application",
        .partition_start_addr   = DEF_APPLICATION_START_ADDR,
        .partition_length       = DEF_APPLICATION_SIZE,
        .partition_options      = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },

    [HAL_PARTITION_PARAMETER_1] = {
        .partition_owner        = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = DEF_PARAMETER_1_START_ADDR,
        .partition_length           = DEF_PARAMETER_1_SIZE,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },

  [HAL_PARTITION_PARAMETER_2] = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "PARAMETER2",
    .partition_start_addr       = DEF_PARAMETER_2_START_ADDR,
    .partition_length           = DEF_PARAMETER_2_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },
    
    [HAL_PARTITION_PARAMETER_3] =   {
        .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "PARAMETER3",
    .partition_start_addr       = DEF_PARAMETER_3_START_ADDR,
    .partition_length           = DEF_PARAMETER_3_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },

  [HAL_PARTITION_PARAMETER_4] = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "PARAMETER4",
    .partition_start_addr       = DEF_PARAMETER_4_START_ADDR,
    .partition_length           = DEF_PARAMETER_4_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },
    
  [HAL_PARTITION_CUSTOM_1]      = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "CUSTOM1",
    .partition_start_addr       = DEF_CUSTOM_1_START_ADDR,
    .partition_length           = DEF_CUSTOM_1_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    
  [HAL_PARTITION_CUSTOM_2]      = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "CUSTOM2",
    .partition_start_addr       = DEF_CUSTOM_2_START_ADDR,
    .partition_length           = DEF_CUSTOM_2_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    }
};