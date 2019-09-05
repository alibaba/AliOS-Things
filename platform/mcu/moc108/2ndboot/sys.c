/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "sys.h"

void sys_delayms(volatile int ms)
{
    volatile unsigned long i;
    while (ms--) {
        i = 0;
	while (i < 29500) {
	   i++;
	}
    }
}

void sys_reboot(void)
{
    ICU_PERI_CLK_PWD_CLEAR(ICU_PERI_CLK_PWD_ARM_WDT_MASK);
    // Write WDT key: 0x5A firstly and 0xA5 secondly.
    REG_WDT_CONFIG = ((WDKEY_ENABLE1 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    REG_WDT_CONFIG = ((WDKEY_ENABLE2 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    for(;;);
}
