/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "platform.h"
#include "plic_driver.h"

/*HIFIVE1 int including local int and global int
for local int now we only care about IRQ_M_TIMER
for global int all go through local int IRQ_M_EXT
local IRQ_M_EXT can extend to PLIC_NUM_INTERRUPTS global interrupts
hal int funs are designed for IRQ_M_EXT
*/

extern plic_instance_t g_plic;
extern int PLIC_int_init(void);
extern void PLIC_set_int_handle(int32_t vec, void* fun);

int32_t hal_interpt_init(void)
{
    return PLIC_int_init();
}


int32_t hal_interpt_mask(int32_t vec)
{
    if(vec >= PLIC_NUM_INTERRUPTS){
        return -1;
    }

    PLIC_disable_interrupt (&g_plic, vec);
    
    return 0;
}


int32_t hal_interpt_umask(int32_t vec)
{
    if(vec >= PLIC_NUM_INTERRUPTS){
        return -1;
    }

    PLIC_enable_interrupt (&g_plic, vec);
    
    return 0;
}


int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, void *para,  char *name)
{
    (void)name;
    (void)para;

    if(vec >= PLIC_NUM_INTERRUPTS){
        return -1;
    }
    PLIC_set_int_handle(vec,handler);
    //g_ext_interrupt_handlers[vec] = handler;

    PLIC_set_priority(&g_plic, vec, 2);
    
    return 0;
    
}


