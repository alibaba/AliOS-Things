/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal_memmap.h"
#include "tee_addr_map.h"
#include "tee_types.h"
#include "tee_dbg.h"
#include "tee_asc.h"

extern void    hal_interrupt_init(void);
extern int32_t hal_uart_init(uint32_t addr, uint32_t baudrate, uint32_t parity,
                             int32_t wordsize, int32_t stopbit);
extern void    hal_uart_putchar(uint8_t ch);

int hal_platform_init(void)
{
    // setting PLL to 24MHz
    *(volatile uint32_t *)0x4000202c = 0xc0206;
    *(volatile uint32_t *)0x40002004 = 0x8;
    hal_interrupt_init();
    hal_uart_init(PLATFORM_UART_ADDRBASE0, 115200, 0, 8, 1);
    return 0;
}

void putch(int8_t ch)
{
    hal_uart_putchar((uint8_t)ch);
}

extern int asc_set_perm(uint32_t paddr, uint32_t size, bool is_secure,
                        uint8_t ap_perm, uint8_t cd_perm);
int        platform_set_perm(void)
{
    int ret;

    /* !!! Don't modify !!! set secure RO region*/
    ret =
      asc_set_perm(TW_RO_ADDR, TW_RO_SIZE, true, REGION_READ_ONLY, REGION_BOTH);
    TEE_ASSERT(0 == ret);

    /* !!! Don't modify !!! set secure RW region*/
    ret = asc_set_perm(TW_RW_ADDR, TW_RW_SIZE, true, REGION_READ_WRITE,
                       REGION_BOTH);
    TEE_ASSERT(0 == ret);


    /* TODO: add platform defined permission */
    return 0;
}
