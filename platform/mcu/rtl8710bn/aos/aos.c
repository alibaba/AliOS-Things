/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <k_api.h>
#include <debug_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/init.h"
#include "aos/kernel.h"
#include "aos/hal/wdg.h"
#include "aos/hal/gpio.h"
#include "diag.h"
#include "platform_stdlib.h"
#include "rtl8710b.h"
#include "core_cm4.h"

#include "wifi_conf.h"
#include "wlan_intf.h"
#include "wifi_constants.h"

#include "osdep_service.h"
#include "rtl8710b_ota.h"

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

static kinit_t kinit;

extern int application_start(int argc, char **argv);
extern void board_init(void);

extern uint32_t SystemCoreClock;

static void hal_init()
{
}

extern void hw_start_hal(void);

static void board_cli_init(void)
{
     kinit.argc = 0;
     kinit.argv = NULL;
     kinit.cli_enable = 1;
}


void dump_mem_info(){}
static void hal_wlan_init()
{
	if ( rtl_cryptoEngine_init() != 0 ) {
		DiagPrintf("crypto engine init failed\r\n");
	}

	/* Initialize log uart and at command service */
	//ReRegisterPlatformLogUart();

#if CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif

#if CONFIG_WLAN
//wifi_on(RTW_MODE_STA);
//wifi_disable_powersave();
    hal_wifi_init();
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_set_autoreconnect(RTW_AUTORECONNECT_INFINITE);
#endif
	printf("\n\r%s(%d), Available heap 0x%x", __FUNCTION__, __LINE__, rtw_getFreeHeapSize());	
#endif
}

void start_ate(void)
{
  u32 img2_addr = 0x080D0000;
  IMAGE_HEADER *img2_hdr = (IMAGE_HEADER *)img2_addr;
  IMAGE_HEADER *ram_img2_hdr = (IMAGE_HEADER *)(img2_addr + IMAGE_HEADER_LEN + img2_hdr->image_size);
  u8 *ram_img2_data = (u8*)(ram_img2_hdr + 1);
  u8 *ram_img2_valid_code = ram_img2_data + 8;
  if (_strcmp((const char *)ram_img2_valid_code, (const char *)"RTKWin")) {
    DBG_8195A("IMG_BOOTUSER SIGN Invalid\n");
    while(1);
  }
  /* load ram image2 data into RAM */
  _memcpy((void *)ram_img2_hdr->image_addr, ram_img2_data, ram_img2_hdr->image_size);
  PRAM_START_FUNCTION img2_entry_fun = (PRAM_START_FUNCTION)ram_img2_data;
  img2_entry_fun->RamStartFun();
}

/* For QC test */
static void board_mode_check(void)
{
    #define KEY_ELINK  12
    #define KEY_BOOT   9

    gpio_dev_t gpio_key_boot;
    gpio_key_boot.port = KEY_BOOT;
    gpio_key_boot.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_key_boot);
    uint32_t boot;
    hal_gpio_input_get(&gpio_key_boot, &boot);
    
    gpio_dev_t gpio_key_elink;
    gpio_key_elink.port = KEY_ELINK;
    gpio_key_elink.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_key_elink);
    uint32_t elink;

    hal_gpio_input_get(&gpio_key_elink, &elink);
    printf("--------------------------------> built at "__DATE__" "__TIME__"\r\n");
    hal_gpio_input_get(&gpio_key_boot, &boot);
    printf("--------------------------------> boot %d, elink %d \r\n", boot, elink);



    if(boot == 0)
    {
        if(elink == 0)
            start_ate();
        else
            qc_test(0);
    }
    if(elink == 0){
        if(OTA_INDEX_1 == ota_get_cur_index()) {
            OTA_Change(OTA_INDEX_2);
            printf("-----change OTA 2 \r\n");
        } else {
            OTA_Change(OTA_INDEX_1);
            printf("-----change OTA 1 \r\n");
        }
        aos_msleep(1000);
        hal_reboot();
    }

    board_init();
}

void sys_init_func(void)
{
#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    pmu_set_sysactive_time();
#endif
    hal_init();

    hw_start_hal();

    hal_wlan_init();

        
    board_cli_init();

#ifdef USE_MX1290
    board_mode_check();
#endif

    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif

    krhino_task_dyn_del(NULL);
}

#if (DEBUG_CONFIG_PANIC == 1)
typedef void (*HAL_VECTOR_FUN) (void );
extern HAL_VECTOR_FUN  NewVectorTable[];
extern void HardFault_Handler(void);
#endif

void main(void)
{
    hal_wdg_finalize(0);

    aos_init();

#if (DEBUG_CONFIG_PANIC == 1)
    /* AliOS-Things taking over the exception */
    /* replace HardFault Vector */
    NewVectorTable[3] = HardFault_Handler;
    /* replace MemManage Vector */
    NewVectorTable[4] = HardFault_Handler;
    /* replace BusFault Vector */
    NewVectorTable[5] = HardFault_Handler;
    /* replace UsageFault Vector */
    NewVectorTable[6] = HardFault_Handler;
#endif

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI , 0, AOS_START_STACK, (task_entry_t)sys_init_func, 1);
    
    SysTick_Config(SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND);

    aos_start();

    return;
}
