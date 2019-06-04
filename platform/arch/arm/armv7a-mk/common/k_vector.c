/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "k_arch.h"

/* sdk interrupt entry */
//extern void os_fiq_handle(void);
//extern void os_irq_handle(void);

/* defined in vector_s.S, putted in ".vectors:" section */
extern uint32_t _vector_table[];

void cpu_interrupt_handler(unsigned int except_id)
{
    krhino_intrpt_enter();
    switch (except_id) {
        case ARM_EXCEPT_FIQ:
            /* bsp fiq entry */
//            os_fiq_handle();
            break;

        case ARM_EXCEPT_IRQ:
            /* bsp irq entry */
//            os_irq_handle();
            break;

        default:
            //__asm__ __volatile__("udf 0":::"memory");
            break;
   }
    krhino_intrpt_exit();
}

/* set vector table base address */
void *k_vectable_get()
{
    return (void *)&_vector_table[0];
}

/* set vector table base address */
void k_vectable_set()
{
    uint32_t reg;

    reg  = os_get_SCTLR();
    /*
    SCTLR.V, bit[13]   Vectors bit.
    0  Low exception vectors,
    1  High exception vectors (Hivecs), base address 0xFFFF0000.
    */
    reg &= ~0x2000;
    os_set_SCTLR(reg);
    OS_ISB();
    os_set_VBAR((uint32_t)&_vector_table[0]);
    OS_ISB();
}

