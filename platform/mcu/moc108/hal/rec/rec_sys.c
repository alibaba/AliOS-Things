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

void rec_hal_flashmap_init()
{
}
void rec_err_print(void *errinfo)
{
    int *regs = (int *)errinfo;
    int i;

    /* exception print, same with "_recovery_error" */
    printf("Exception %p:\r\n", regs);
    printf("=======================================================\r\n");
    printf("Exception context:\r\n");
    printf("R0: 0x%x, R1: 0x%x, R2: 0x%x, R3: 0x%x\r\n",
            regs[0], regs[1], regs[2], regs[3]);
    printf("R4: 0x%x, R5: 0x%x, R6: 0x%x, R7: 0x%x\r\n",
            regs[4], regs[5], regs[6], regs[7]);
    printf("R8: 0x%x, R9: 0x%x, R10: 0x%x, R11: 0x%x\r\n",
            regs[8], regs[9], regs[10], regs[11]);
    printf("R12: 0x%x, SP: 0x%x, LR: 0x%x, PC: 0x%x\r\n",
            regs[12], regs[13], regs[14]&0xFFFFFFFE, regs[15]);
    printf("=======================================================\r\n");
    printf("Exception stack:\r\n");
    for ( i = 16 ; i < 256 && &regs[i] < 0x440000; i+=4 )
    {
        printf("0x%p: 0x%x 0x%x 0x%x 0x%x\r\n",
                &regs[i], regs[i], regs[i+1], regs[i+2], regs[i+3]);
    }
    printf("=======================================================\r\n");
}

void rec_reboot(void)
{
    //rec_uart_deinit();

    ICU_PERI_CLK_PWD_CLEAR(ICU_PERI_CLK_PWD_ARM_WDT_MASK);
    // Write WDT key: 0x5A firstly and 0xA5 secondly.
    REG_WDT_CONFIG = ((WDKEY_ENABLE1 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    REG_WDT_CONFIG = ((WDKEY_ENABLE2 << WDT_CONFIG_WDKEY_POSI)
                    | (1     << WDT_CONFIG_PERIOD_POSI));
    for(;;);
}
unsigned int rec_get_boot_addr()
{
    return 0;
}

unsigned int rec_get_ota_mode()
{
    return 1;
}

void rec_upgrade_reboot()
{
}
