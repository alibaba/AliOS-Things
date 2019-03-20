/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/flash.h"

#include "board.h"

/* Logic partition on flash devices */
#if defined(USE_OTA_SPIM_FLASH)
#define DEF_APPLICATION_START_ADDR  FMC_APROM_BASE
#define DEF_APPLICATION_SIZE        (0x80000-FMC_FLASH_PAGE_SIZE*2-FMC_FLASH_PAGE_SIZE*2-FMC_FLASH_PAGE_SIZE*4) // 480k bytes
#define DEF_PARAMETER_1_START_ADDR  (DEF_APPLICATION_START_ADDR+DEF_APPLICATION_SIZE)
#define DEF_PARAMETER_1_SIZE        (FMC_FLASH_PAGE_SIZE*2)     //8k bytes
#define DEF_PARAMETER_2_START_ADDR  (DEF_PARAMETER_1_START_ADDR+DEF_PARAMETER_1_SIZE)
#define DEF_PARAMETER_2_SIZE        (FMC_FLASH_PAGE_SIZE*2)     //8k bytes
#else
#define DEF_APPLICATION_START_ADDR  FMC_APROM_BASE
#define DEF_APPLICATION_SIZE        (0x3E000)           //248k bytes
#define DEF_PARAMETER_1_START_ADDR  (DEF_APPLICATION_START_ADDR+DEF_APPLICATION_SIZE)
#define DEF_PARAMETER_1_SIZE        (FMC_FLASH_PAGE_SIZE*2)             //8k bytes
#define DEF_OTATEMP_START_ADDR      (DEF_PARAMETER_1_START_ADDR+DEF_PARAMETER_1_SIZE)
#define DEF_OTATEMP_SIZE            (DEF_APPLICATION_SIZE)
#define DEF_PARAMETER_2_START_ADDR  (DEF_OTATEMP_START_ADDR+DEF_OTATEMP_SIZE)
#define DEF_PARAMETER_2_SIZE        (FMC_FLASH_PAGE_SIZE*2)     //8k bytes
#endif

const hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX] = {
    
    [HAL_PARTITION_APPLICATION] = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Application",
        .partition_start_addr       = DEF_APPLICATION_START_ADDR,
        .partition_length           = DEF_APPLICATION_SIZE,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },
    [HAL_PARTITION_PARAMETER_1] = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = DEF_PARAMETER_1_START_ADDR,
        .partition_length           = DEF_PARAMETER_1_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },
    [HAL_PARTITION_OTA_TEMP] = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "OTA",
        .partition_start_addr       = DEF_OTATEMP_START_ADDR,
        .partition_length           = DEF_OTATEMP_SIZE,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },
    [HAL_PARTITION_PARAMETER_2] = {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = DEF_PARAMETER_2_START_ADDR,
        .partition_length           = DEF_PARAMETER_2_SIZE,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
    },
};
