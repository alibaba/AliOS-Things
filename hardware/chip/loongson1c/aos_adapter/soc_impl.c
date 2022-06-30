/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <ls1c_uart.h>

extern void plat_irq_dispatch(void);

void cpu_interrupt_handler(void)
{
    krhino_intrpt_enter();
    plat_irq_dispatch();
    krhino_intrpt_exit();
}

#define WDT_BASE        0xBFE5C060
#define WDT_EN          (WDT_BASE + 0x00)
#define WDT_SET         (WDT_BASE + 0x04)
#define WDT_TIMER       (WDT_BASE + 0x08)

void hal_reboot(void)
{
    *(volatile uint32_t *)WDT_EN = 0x01;
    *(volatile uint32_t *)WDT_TIMER = 0x01;
    *(volatile uint32_t *)WDT_SET = 0x01;
    while (1);
}

void soc_err_proc(kstat_t err)
{
    while (1);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

/* for printk */
int alios_debug_print(const char *buf, int size)
{
    for (int i = 0; i < size; i++) {
        char str[2];

        str[0] = buf[i];
        str[1] = '\0';
        uart2_print(str);
    }

    return 0;
}
