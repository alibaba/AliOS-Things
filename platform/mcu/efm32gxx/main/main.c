/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <k_api.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/hal.h>
#include "board.h"
#include "debug.h"

#ifdef CONFIG_LINKWAN
#include "radio.h"
#endif

#include "port_mcu.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "em_cmu.h"
#include "em_emu.h"

extern uart_dev_t uart_1;

void platform_init(void)
{
    CMU_HFRCOBandSet(cmuHFRCOBand_14MHz);                 
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);  
    
    CMU_ClockEnable(cmuClock_HFPER, true); 
    
    CMU_ClockEnable(cmuClock_GPIO, true); 

    GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0); //PC0 TXD
    GPIO_PinModeSet(gpioPortC, 1, gpioModeInputPull, 0); //PC1 RXD

    CMU_OscillatorEnable(cmuOsc_LFXO,true,false);

    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_CORELE, true);

    CMU_ClockEnable(cmuClock_USART0, true);

    /* lowpower uart */
    /* CMU_ClockEnable(cmuClock_LEUART0, true); */
    CMU_ClockEnable(cmuClock_USART1, true);
}

#define LORA_TASK_STACKSIZE 256
#define LORA_TASK_PRIORITY 20

static ktask_t lora_task_obj;
cpu_stack_t lora_task_buf[LORA_TASK_STACKSIZE];

extern int application_start(void);

void aos_app_entry(void *args)
{
    uint32_t cpu_frequency;

    platform_init();

    hal_uart_init(&uart_1);

    cpu_frequency = SystemCoreClockGet();
    SysTick_Config(cpu_frequency / RHINO_CONFIG_TICKS_PER_SECOND);

    EFM32_EFM32_FLASH_Init();

    HW_Init();

    application_start();
}

int main(void)
{
    krhino_init();
    krhino_task_create(&lora_task_obj, "loratask", 0, LORA_TASK_PRIORITY,
        50, lora_task_buf, LORA_TASK_STACKSIZE, (task_entry_t)aos_app_entry, 1);
    krhino_start();
}
