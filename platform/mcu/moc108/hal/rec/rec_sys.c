/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "rec_sys.h"

extern void rec_uart_init();
extern void rec_uart_deinit();
extern void rec_flash_init();

void rec_delayms(volatile int timesMS)
{
	volatile unsigned long i;

    while (timesMS --)
	{
		i = 0;
		while (i < 29500)
		{
			i++;
		}
	}
}

void rec_hal_init()
{
    rec_uart_init();
    rec_flash_init();
}

void rec_err_print(void *errinfo)
{
   printf("rec Exception.\r\n");
}
void rec_reboot(void)
{
    ICU_PERI_CLK_PWD_CLEAR(ICU_PERI_CLK_PWD_ARM_WDT_MASK);
    // Write WDT key: 0x5A firstly and 0xA5 secondly.
    REG_WDT_CONFIG = ((WDKEY_ENABLE1 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    REG_WDT_CONFIG = ((WDKEY_ENABLE2 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    for(;;);
}

unsigned int rec_get_ota_mode()
{
    return 1;
}
