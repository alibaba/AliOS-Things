/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include <aos/hal/uart.h>
#include <aos/hal/timer.h>
#include <network/hal/base.h>
#include <network/hal/wifi.h>
#include "board.h"

#include "hal_sys.h"
#include "syslog.h"

extern uint32_t chip_hardware_code(void);
extern uint32_t chip_eco_version(void);
	

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

uart_dev_t uart_0;
uart_dev_t uart_1;


extern hal_wifi_module_t sim_aos_wifi_mico;

void hal_reboot(void)
{
    hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
}


static void platform_uart_init()
{
    uart_0.port                = 0;
    uart_0.config.baud_rate    = 115200;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    //uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
}


static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    platform_uart_init();
    mtk_hal_flash_init();
    hal_nvic_init();
}

extern void log_sys_init();
extern void ota_module_register();

void platform_init(void)
{
    hal_clock_init();
    SystemCoreClockUpdate();
	bsp_ept_gpio_setting_init();
	prvSetupHardware();

	cache_func_init();
#ifdef MTK_WIFI_ROM_ENABLE
    move_iot_rom_data_to_ram();
#endif
	
	/* Connsys init will download FW, MT5932 need download N9 FW before any task created, to init ucHeap early */
	connsys_init(NULL);
	log_sys_init();

#ifdef MTK_NVDM_ENABLE
	nvdm_default_config_init();
#endif
	systick_init();

	    /* Call this function to indicate the system initialize done. */
    //SysInitStatus_Set();
	//LOG_I(common, "platform:MT%04x-E%d.\r\n", chip_hardware_code(), chip_eco_version());
    LOG_I(common, "system initialize done.\r\n");
	
}




