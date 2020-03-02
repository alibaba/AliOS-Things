/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <k_api.h>
#include "aos/yloop.h"
#include "aos/hal/gpio.h"
#include "aos/kernel.h"
#include "aos/hal/wdg.h"
#include "aos/hal/gpio.h"
#include "aos/hal/uart.h"
#include "board.h"
#include "network/hal/wifi.h"
#include "aos/init.h"
#include "ameba_soc.h"
#include "diag.h"
#include "platform_stdlib.h"
#include "rtl8710b.h"
#include "core_cm4.h"

#include "wifi_conf.h"
#include "wlan_intf.h"
#include "wifi_constants.h"

#include "osdep_service.h"
#include "rtl8710b_ota.h"


#define KEY_AWSS   12

extern uint32_t SystemCoreClock;
extern hal_wifi_module_t rtl8710bn_wifi_module;
extern uart_dev_t uart_0;

static uint64_t   awss_time = 0;
static gpio_dev_t gpio_key_awss;

static void key_poll_func(void *arg)
{
    uint32_t level;
    uint64_t diff;

    hal_gpio_input_get(&gpio_key_awss, &level);

    if (level == 0) {
        aos_post_delayed_action(10, key_poll_func, NULL);
    } else {
        diff = aos_now_ms() - awss_time;
        if (diff > 6000) { /*long long press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_CLICK);
        } else {
            aos_post_delayed_action(10, key_poll_func, NULL);
        }
    }
}

static void key_proc_work(void *arg)
{
    aos_schedule_call(key_poll_func, NULL);
}

static void handle_awss_key(void *arg)
{
    uint32_t gpio_value;

    hal_gpio_input_get(&gpio_key_awss, &gpio_value);
    if (gpio_value == 0 && awss_time == 0) {
        awss_time = aos_now_ms();
        aos_loop_schedule_work(0, key_proc_work, NULL, NULL, NULL);
    }
}

void board_awss_key_init(void)
{
    gpio_key_awss.port = KEY_AWSS;
    gpio_key_awss.config = INPUT_PULL_UP;

    hal_gpio_init(&gpio_key_awss);
    hal_gpio_enable_irq(&gpio_key_awss, IRQ_TRIGGER_FALLING_EDGE, handle_awss_key, NULL);
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

    board_awss_key_init();
}



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


#ifdef AOS_COMP_DEBUG
typedef void (*HAL_VECTOR_FUN) (void );
extern HAL_VECTOR_FUN  NewVectorTable[];
extern void HardFault_Handler(void);
#endif

void board_basic_init(void)
{
    hal_wdg_finalize(0);

#ifdef AOS_COMP_DEBUG
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
}

void board_tick_init(void)
{
    SysTick_Config(SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND);
}

void board_network_init(void)
{
#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    pmu_set_sysactive_time();
#endif

    DBG_8195A("start hal-----------\n");
    hal_wifi_register_module(&rtl8710bn_wifi_module);
#ifdef CONFIG_AOS_MESH
    hal_umesh_register_wifi(&rtl8710bn_wifi_module);
#endif

    hal_wlan_init();

#ifdef USE_MX1290
    board_mode_check();
#endif

}

void board_stduart_init(void)
{
    uart_0.port                = MICO_UART_1;
    uart_0.config.baud_rate    = 115200;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
}

void board_dma_init(void)
{
    return;
}

void board_gpio_init()
{
    return;
}

void board_flash_init(void)
{
    return;
}

void board_kinit_init(kinit_t* init_args)
{
    return;
}
