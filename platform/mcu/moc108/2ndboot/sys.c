/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "sys.h"

void hal_sys_delay(unsigned int ms)
{
    volatile unsigned long i;
	volatile unsigned int loop = ms;

    while (loop--) {
        i = 0;
	    while (i < 29500) {
	       i++;
	    }
    }
}

void hal_sys_reboot(void)
{
    ICU_PERI_CLK_PWD_CLEAR(ICU_PERI_CLK_PWD_ARM_WDT_MASK);
    // Write WDT key: 0x5A firstly and 0xA5 secondly.
    REG_WDT_CONFIG = ((WDKEY_ENABLE1 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    REG_WDT_CONFIG = ((WDKEY_ENABLE2 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    for(;;);
}
