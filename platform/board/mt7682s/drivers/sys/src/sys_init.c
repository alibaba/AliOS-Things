/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <string.h>
#ifdef USE_FREERTOS_ENABLE
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#else
#include "freertos_to_aos.h"
#endif
/* Hal includes. */
#include "hal.h"
#include "hal_dcxo.h"

#include "nvdm.h"
#include "syslog.h"
#include "io_def.h"
#include "bsp_gpio_ept_config.h"

#include "memory_map.h"
#include "memory_attribute.h"


extern uint32_t chip_hardware_code(void);
extern uint32_t chip_eco_version(void);

#include <connsys_driver.h>
#include "connsys_profile.h"

#if defined(MTK_MINICLI_ENABLE)
#include "cli_def.h"
#endif

void user_check_default_value(void);

#ifdef MTK_WIFI_ROM_ENABLE
void move_iot_rom_data_to_ram();
#endif


#ifndef MTK_DEBUG_LEVEL_NONE

log_create_module(main, PRINT_LEVEL_ERROR);

LOG_CONTROL_BLOCK_DECLARE(main);
LOG_CONTROL_BLOCK_DECLARE(common);
LOG_CONTROL_BLOCK_DECLARE(hal);
LOG_CONTROL_BLOCK_DECLARE(lwip);
LOG_CONTROL_BLOCK_DECLARE(fw_interface);
LOG_CONTROL_BLOCK_DECLARE(inband);
LOG_CONTROL_BLOCK_DECLARE(wifi);


log_control_block_t *syslog_control_blocks[] = {
    &LOG_CONTROL_BLOCK_SYMBOL(main),
    &LOG_CONTROL_BLOCK_SYMBOL(common),
    &LOG_CONTROL_BLOCK_SYMBOL(hal),
    &LOG_CONTROL_BLOCK_SYMBOL(lwip),
    &LOG_CONTROL_BLOCK_SYMBOL(fw_interface),
    &LOG_CONTROL_BLOCK_SYMBOL(inband),
    &LOG_CONTROL_BLOCK_SYMBOL(wifi),
    NULL
};
#endif

static void SystemClock_Config(void)
{
    hal_clock_init();
}


/**
* @brief       This function is to initialize cache controller.
* @param[in]   None.
* @return      None.
*/
static void cache_init(void)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 16 */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* cacheable address, cacheable size(both MUST be 4k bytes aligned) */
        /* UBIN length */
#if ((PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682))
#ifdef CM4_UBIN_LENGTH
        {CM4_BASE, CM4_LENGTH + CM4_UBIN_LENGTH},
#else
        {CM4_BASE, CM4_LENGTH},
#endif
#endif
        /* virtual sysram */
        {VSYSRAM_BASE, VSYSRAM_LENGTH},
#if (PRODUCT_VERSION == 7686)
        /* virtual memory */
        {VRAM_BASE, VRAM_LENGTH}
#endif
    };

    region_number = (hal_cache_region_t)(sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));

    hal_cache_init();
    hal_cache_set_size(HAL_CACHE_SIZE_32KB);
    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        hal_cache_region_config(region, &region_cfg_tbl[region]);
        hal_cache_region_enable(region);
    }
    for (; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable(region);
    }
    hal_cache_enable();
}

/**
* @brief       caculate actual bit value of region size.
* @param[in]   region_size: actual region size.
* @return      corresponding bit value of region size for MPU setting.
*/
static uint32_t caculate_mpu_region_size(uint32_t region_size)
{
    uint32_t count;

    if (region_size < 32) {
        return 0;
    }
    for (count = 0; ((region_size  & 0x80000000) == 0); count++, region_size  <<= 1);
    return 30 - count;
}

/**
* @brief       This function is to initialize MPU.
* @param[in]   None.
* @return      None.
*/
ATTR_TEXT_IN_SYSRAM void mpu_init(void)
{
    hal_mpu_region_t region, region_number;
    hal_mpu_region_config_t region_config;
    typedef struct {
        uint32_t mpu_region_base_address;			/**< MPU region start address */
        uint32_t mpu_region_end_address;			/**< MPU region end address */
        hal_mpu_access_permission_t mpu_region_access_permission; /**< MPU region access permission */
        uint8_t mpu_subregion_mask; 			/**< MPU sub region mask*/
        bool mpu_xn;					/**< XN attribute of MPU, if set TRUE, execution of an instruction fetched from the corresponding region is not permitted */
    } mpu_region_information_t;

#if defined (__GNUC__) || defined (__CC_ARM)

    //TCM: VECTOR TABLE + CODE+RO DATA
    extern uint32_t Image$$TCM$$RO$$Base;
    extern uint32_t Image$$TCM$$RO$$Limit;
    //SYSRAM: CODE+RO DATA
    extern uint32_t Image$$CACHED_SYSRAM_TEXT$$Base;
    extern uint32_t Image$$CACHED_SYSRAM_TEXT$$Limit;
    //STACK END
    extern unsigned int Image$$STACK$$ZI$$Base[];

#if (PRODUCT_VERSION == 7686)
    //RAM: CODE+RO DATA
    extern uint32_t Image$$CACHED_RAM_TEXT$$Base;
    extern uint32_t Image$$CACHED_RAM_TEXT$$Limit;
#endif
    /* MAX region number is 8 */
    mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t) &Image$$TCM$$RO$$Base, (uint32_t) &Image$$TCM$$RO$$Limit, HAL_MPU_READONLY, 0x0, FALSE},//Vector table+TCM code+TCM rodata
        {(uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Base, (uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Limit, HAL_MPU_READONLY, 0x0, FALSE}, //SYSRAM code+SYSRAM rodata
        {(uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Base - VRAM_BASE, (uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Limit - VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t) &Image$$STACK$$ZI$$Base, (uint32_t) &Image$$STACK$$ZI$$Base + 32, HAL_MPU_READONLY, 0x0, TRUE}, //Stack end check for stack overflow
#if (PRODUCT_VERSION == 7686)
        {(uint32_t) &Image$$CACHED_RAM_TEXT$$Base, (uint32_t) &Image$$CACHED_RAM_TEXT$$Limit, HAL_MPU_READONLY, 0x0, FALSE}, //RAM code+RAM rodata
        {(uint32_t) &Image$$CACHED_RAM_TEXT$$Base - VRAM_BASE, (uint32_t) &Image$$CACHED_RAM_TEXT$$Limit - VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
#else
        {(uint32_t) 0, (uint32_t) 0x400000, HAL_MPU_NO_ACCESS, 0x0, TRUE} //Set EMI address range as no access
#endif
    };

#elif defined (__ICCARM__)

#pragma section = ".intvec"
#pragma section = ".tcm_rwdata"
#pragma section = ".sysram_code"
#pragma section = ".sysram_rodata"
#pragma section = "CSTACK"
#if (PRODUCT_VERSION == 7686)
#pragma section = ".ram_code"
#pragma section = ".ram_rodata"
#endif

    /* MAX region number is 8 */
    mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t)__section_begin(".intvec"), (uint32_t)__section_begin(".tcm_rwdata"), HAL_MPU_READONLY, 0x0, FALSE},//Vector table+TCM code+TCM rodata
        {(uint32_t)__section_begin(".sysram_code"), (uint32_t)__section_end(".sysram_code") + (uint32_t)__section_end(".sysram_rodata") - (uint32_t)__section_begin(".sysram_rodata"), HAL_MPU_READONLY, 0x0, FALSE},//SYSRAM code+SYSRAM rodata
        {(uint32_t)__section_begin(".sysram_code") - VRAM_BASE, (uint32_t)__section_end(".sysram_code") + (uint32_t)__section_end(".sysram_rodata") - (uint32_t)__section_begin(".sysram_rodata") - VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t)__section_begin("CSTACK"), (uint32_t)__section_begin("CSTACK") + 32, HAL_MPU_READONLY, 0x0, TRUE}, //Stack end check for stack overflow
#if (PRODUCT_VERSION == 7686)
        {(uint32_t)__section_begin(".ram_code"), (uint32_t)__section_end(".ram_code") + (uint32_t)__section_end(".ram_rodata") - (uint32_t)__section_begin(".ram_rodata"), HAL_MPU_READONLY, 0x0, FALSE},//RAM code+RAM rodata
        {(uint32_t)__section_begin(".ram_code") - VRAM_BASE, (uint32_t)__section_end(".ram_code") + (uint32_t)__section_end(".ram_rodata") - (uint32_t)__section_begin(".ram_rodata") - VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
#else
        {(uint32_t) 0, (uint32_t) 0x400000, HAL_MPU_NO_ACCESS, 0x0, TRUE} //Set EMI address range as no access
#endif
    };

#endif
	//printf("Image$$TCM$$RO$$Base=%x,Image$$TCM$$RO$$Limit=%x,Image$$CACHED_SYSRAM_TEXT$$Base=%x,Image$$CACHED_SYSRAM_TEXT$$Limit=%x",Image$$TCM$$RO$$Base,Image$$TCM$$RO$$Limit,Image$$CACHED_SYSRAM_TEXT$$Base,Image$$CACHED_SYSRAM_TEXT$$Limit);
    hal_mpu_config_t mpu_config = {
        /* PRIVDEFENA, HFNMIENA */
        TRUE, TRUE
    };

    region_number = (hal_mpu_region_t)(sizeof(region_information) / sizeof(region_information[0]));

    hal_mpu_init(&mpu_config);
    for (region = HAL_MPU_REGION_0; region < region_number; region++) {
        /* Updata region information to be configured */
        region_config.mpu_region_address = region_information[region].mpu_region_base_address;
        region_config.mpu_region_size = (hal_mpu_region_size_t) caculate_mpu_region_size(region_information[region].mpu_region_end_address - region_information[region].mpu_region_base_address);
        region_config.mpu_region_access_permission = region_information[region].mpu_region_access_permission;
        region_config.mpu_subregion_mask = region_information[region].mpu_subregion_mask;
        region_config.mpu_xn = region_information[region].mpu_xn;

        hal_mpu_region_configure(region, &region_config);
        hal_mpu_region_enable(region);
    }
    /* make sure unused regions are disabled */
    for (; region < HAL_MPU_REGION_MAX; region++) {
        hal_mpu_region_disable(region);
    }
    hal_mpu_enable();
}

static void prvSetupHardware(void)
{
    bsp_io_def_uart_init();

    /* sleep manager init*/
    hal_sleep_manager_init();

    cache_init();
    mpu_init();
    mtk_hal_flash_init();
    hal_nvic_init();
#ifdef HAL_PMU_MODULE_ENABLED
    hal_pmu_init();
#endif
    hal_dcxo_init();
    mtk_hal_rtc_init();
}

#if !defined(MTK_DEBUG_LEVEL_NONE) && defined(MTK_NVDM_ENABLE)
static void syslog_config_save(const syslog_config_t *config)
{
    char *syslog_filter_buf;

    syslog_filter_buf = (char *)pvPortMalloc(SYSLOG_FILTER_LEN);
    configASSERT(syslog_filter_buf != NULL);
    syslog_convert_filter_val2str((const log_control_block_t **)config->filters, syslog_filter_buf);
    nvdm_write_data_item("common", "syslog_filters", \
                         NVDM_DATA_ITEM_TYPE_STRING, (const uint8_t *)syslog_filter_buf, strlen(syslog_filter_buf));
    vPortFree(syslog_filter_buf);
}

static uint32_t syslog_config_load(syslog_config_t *config)
{
    uint32_t sz = SYSLOG_FILTER_LEN;
    char *syslog_filter_buf;

    syslog_filter_buf = (char *)pvPortMalloc(SYSLOG_FILTER_LEN);
    configASSERT(syslog_filter_buf != NULL);
    nvdm_read_data_item("common", "syslog_filters", (uint8_t *)syslog_filter_buf, &sz);
    syslog_convert_filter_str2val(config->filters, syslog_filter_buf);
    vPortFree(syslog_filter_buf);

    return 0;
}
#endif
void log_sys_init()
{
#ifndef MTK_DEBUG_LEVEL_NONE
#ifdef MTK_NVDM_ENABLE
    log_init(syslog_config_save, syslog_config_load, syslog_control_blocks);
#else
    log_init(NULL, NULL, syslog_control_blocks);
#endif
#endif
}
void cache_func_init()
{
    /* sleep manager init*/
    hal_sleep_manager_init();

	cache_init();
#ifdef HAL_PMU_MODULE_ENABLED
    hal_pmu_init();
#endif
	hal_dcxo_init();
	mtk_hal_rtc_init();

}
void nvdm_default_config_init()
{
	#ifdef MTK_NVDM_ENABLE
    /* nvdm init */
    nvdm_init();
    user_check_default_value();
	#endif
}
void system_init(void)
{
    /* SystemClock Config */
    SystemClock_Config();
    SystemCoreClockUpdate();

    /* bsp_ept_gpio_setting_init() under driver/board/mt76x7_hdk/ept will initialize the GPIO settings
     * generated by easy pinmux tool (ept). ept_*.c and ept*.h are the ept files and will be used by
     * bsp_ept_gpio_setting_init() for GPIO pinumux setup.
     */
    bsp_ept_gpio_setting_init();

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

#ifdef MTK_WIFI_ROM_ENABLE
    move_iot_rom_data_to_ram();
#endif

    /* Connsys init will download FW, MT5932 need download N9 FW before any task created, to init ucHeap early */
    connsys_init(NULL);

#ifdef MTK_NVDM_ENABLE
    /* nvdm init */
    nvdm_init();
    user_check_default_value();
#endif

#ifndef MTK_DEBUG_LEVEL_NONE
#ifdef MTK_NVDM_ENABLE
    log_init(syslog_config_save, syslog_config_load, syslog_control_blocks);
#else
    log_init(NULL, NULL, syslog_control_blocks);
#endif
#endif

   // LOG_I(common, "platform:MT%04x-E%d.\r\n", chip_hardware_code(), chip_eco_version());
    LOG_I(common, "system initialize done.\r\n");
}


