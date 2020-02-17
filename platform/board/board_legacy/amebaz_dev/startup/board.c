/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/kernel.h>
#include "aos/kernel.h"
#include <aos/hal/flash.h>
#include "ameba_soc.h"

#ifdef AOS_COMP_DEBUG
typedef void (*HAL_VECTOR_FUN) (void );
extern HAL_VECTOR_FUN  NewVectorTable[];
extern void HardFault_Handler(void);
#endif

extern uint32_t SystemCoreClock;

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

void board_init(void)
{
}
