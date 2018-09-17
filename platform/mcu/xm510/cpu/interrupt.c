#include <board.h>
#include <stdio.h>
#include <string.h>
#include "vic.h"
#include <aos/types.h>
#include "interrupt.h"

/* exception and interrupt handler table */
os_irq_desc g_interrupt_handlers[OS_MAX_IRQ_NUM];

static void os_hw_interrupt_handle(int irq, void *param)
{
    printf("UnInited interrupt %d occured,defaule handler!!!\n", irq);
}

/**
 * hardware interrupt initialized
 */
void os_hw_interrupt_init(void)
{
    register uint32_t idx;

    /* init software exceptions table */
    memset(g_interrupt_handlers, 0x00, sizeof(g_interrupt_handlers));
    for (idx = 0; idx < OS_MAX_IRQ_NUM; idx++)
    {
        g_interrupt_handlers[idx].irq_handler = os_hw_interrupt_handle;
    }
	xm510_init_irq();

}


/**
* create a irq handler; set the software int vector
 */
irq_hdlr_t os_hw_interrupt_create(int irq, irq_hdlr_t irq_handler,void *param, char *name)
{
    irq_hdlr_t old_handler = NULL;

    if (irq < OS_MAX_IRQ_NUM)
    {
        old_handler = g_interrupt_handlers[irq].irq_handler;

        if (irq_handler != NULL)
        {
            g_interrupt_handlers[irq].irq_handler = irq_handler;
            g_interrupt_handlers[irq].param = param;
            strncpy(g_interrupt_handlers[irq].name, name, INTERRUPT_NAME_MAX);
        }
    }

    return old_handler;
}


/**
 * interrupt disable
 */
void os_hw_interrupt_disable(int irq)
{
    xm_mask_irq( irq);
}

/**
 * interrupt enable
 */
void os_hw_interrupt_enable(int irq)
{
	xm_unmask_irq(irq);
}


void os_interrupt_counter_show(char *pwbuf, int blen, int argc, char **argv)
{
	int irq = 0;
	for(irq = 0;irq < OS_MAX_IRQ_NUM;irq++)
	{
		if(strlen(g_interrupt_handlers[irq].name))
		{
			printf("%-2d:  %-16s  %-4ld\n", irq, g_interrupt_handlers[irq].name, g_interrupt_handlers[irq].counter);
		}
	}
}
