/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include "k_api.h"

#include <hal/wifi.h>
#include "aos/kernel.h"
#include <sdkconfig.h>
#include <soc/dport_reg.h>
#include <soc/cpu.h>
#include <esp_panic.h>
#include <cache.h>
#include "rtc.h"
#include <esp_intr.h>
//#include "smp_sync.h"

extern int _init_start;
extern void os_crosscore_int_init();
extern void cpu_first_task_start(void);

extern void esp_cache_err_int_init();
extern void dport_access_init_core_aos();
extern int  ets_printf(const char *fmt, ...);
extern void ets_set_appcpu_boot_addr(uint32_t start);
extern void ets_install_uart_printf(void);
extern void uartAttach(void);
extern void uart_tx_switch(uint8_t uart_no);

/*void start_cpu1_default(void) IRAM_ATTR;*/

void os_start_cpu1_default(void)
{
#if CONFIG_ESP32_TRAX_TWOBANKS
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif
#if CONFIG_ESP32_APPTRACE_ENABLE
    esp_err_t err = esp_apptrace_init();
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to init apptrace module on CPU1 (%d)!", err);
    }
#endif

    //Take care putting stuff here: if asked, os will happily tell you the scheduler
    //has started, but it isn't active *on this CPU* yet.
    //esp_cache_err_int_init();
    os_crosscore_int_init();
    dport_access_init_core_aos();

    ets_printf("Starting scheduler on APP CPU.\r\n");

    cpu_first_task_start();
}


static void wdt_reset_cpu1_info_enable(void)
{
    DPORT_REG_SET_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_PDEBUG_ENABLE | DPORT_APP_CPU_RECORD_ENABLE);
    DPORT_REG_CLR_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_RECORD_ENABLE);
}


void os_start_cpu1(void)
{

    asm volatile (\
                  "wsr    %0, vecbase\n" \
                  ::"r"(&_init_start));

    ets_set_appcpu_boot_addr(0); 
    cpu_configure_region_protection();

#if CONFIG_CONSOLE_UART_NONE
    ets_install_putc1(NULL);
    ets_install_putc2(NULL);
#else // CONFIG_CONSOLE_UART_NONE
    uartAttach();
    ets_install_uart_printf();
    uart_tx_switch(CONFIG_CONSOLE_UART_NUM);
#endif

    wdt_reset_cpu1_info_enable();
    ets_printf( "App cpu up.\r\n");
    os_start_cpu1_default();

}

void os_load_slavecpu(void)
{
    RESET_REASON rst_reas;

    rst_reas = rtc_get_reset_reason(1);
    if(rst_reas == RTCWDT_SYS_RESET || rst_reas== TG0WDT_SYS_RESET) {
        esp_panic_wdt_stop();
    }

    ets_printf("Starting app cpu, entry point is %p\r\n", os_start_cpu1);

    //Flush and enable icache for APP CPU
    Cache_Flush(1);
    Cache_Read_Enable(1);
    esp_cpu_unstall(1);
    // Enable clock and reset APP CPU. Note that OpenOCD may have already
    // enabled clock and taken APP CPU out of reset. In this case don't reset
    // APP CPU again, as that will clear the breakpoints which may have already
    // been set.
    if (!DPORT_GET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN)) {
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_C_REG, DPORT_APPCPU_RUNSTALL);
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
    }

    ets_set_appcpu_boot_addr((uint32_t)os_start_cpu1);

    os_wait_allcore();
    
}

