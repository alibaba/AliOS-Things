/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "port.h"
#include <k_api.h>

#define  ICU_BASE_ADDR                            0x87000u
#define  ICU_REG_IR027_SWINT                     (*(volatile uint8_t *)(ICU_BASE_ADDR + 0x01Bu))
#define  ICU_REG_IER03_SWINT                     (*(volatile uint8_t *)(ICU_BASE_ADDR + 0x203u))
#define  ICU_REG_SWINTR                          (*(volatile uint8_t *)(ICU_BASE_ADDR + 0x2E0u))
#define  ICU_REG_IPR003_SWINT                    (*(volatile uint8_t *)(ICU_BASE_ADDR + 0x303u))
#define  ICU_SWINTR_BIT_SWINT                     0x1u
#define  ICU_IER03_BIT_SWINT                      0x8u


uint32_t interpt_bound = IPL_BOUNDARY << 24u;

void os_pre_init(void)
{
    ICU_REG_IER03_SWINT  = ICU_IER03_BIT_SWINT;
    ICU_REG_IR027_SWINT  = 0u;
    ICU_REG_IPR003_SWINT = 1u;

    interpt_bound = IPL_BOUNDARY << 24u;
}

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;
    uint32_t temp = (uint32_t)(stack_base + stack_size);

    /* stack aligned by 8 byte */
    temp &= 0xfffffff8; 
    stk = (cpu_stack_t *)temp;

    *--stk = (cpu_stack_t)0x00030000u;
    *--stk = (cpu_stack_t)entry;
    *--stk = 0x15151515u; 
    *--stk = 0x14141414u;
    *--stk = 0x13131313u;
    *--stk = 0x12121212u;
    *--stk = 0x11111111u;
    *--stk = 0x10101010u;
    *--stk = 0x09090909u;
    *--stk = 0x08080808u;
    *--stk = 0x07070707u;
    *--stk = 0x06060606u;
    *--stk = 0x05050505u;
    *--stk = 0x04040404u;
    *--stk = 0x03030303u;
    *--stk = 0x02020202u;
    *--stk = (cpu_stack_t)arg;
#if __RX600
    *--stk = 0x00000100u;
#endif
    *--stk = 0x00009ABCu;
    *--stk = 0x12345678u;
    return stk;
}


void   cpu_intrpt_switch(void)
{
    ICU_REG_SWINTR = ICU_SWINTR_BIT_SWINT;                      /* Trigger the pended interrupt request                 */
}
void   cpu_task_switch(void)
{
    ICU_REG_SWINTR = ICU_SWINTR_BIT_SWINT;                      /* Trigger the pended interrupt request                 */
}
