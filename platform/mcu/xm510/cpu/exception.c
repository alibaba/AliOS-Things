#include <stdio.h>
#include "k_api.h"
#include "board.h"
#include "vic.h"
#include "interrupt.h"

extern os_irq_desc g_interrupt_handlers[];

void os_fiq_handle()
{
    printf("FIQ\n");
}

void os_irq_handle()
{
    int intno;
    irq_hdlr_t isr_func;
	int irq = 0;
	int irq_status;

    krhino_intrpt_enter();

	for(irq = 0;irq < OS_MAX_IRQ_NUM;irq++)
	{
		irq_status = readl(INTC_IRQSTATUS);
		if ((1<<irq)&irq_status)
		{
			intno = irq;
			g_interrupt_handlers[intno].counter++;
		    isr_func = g_interrupt_handlers[intno].irq_handler;
		    if (isr_func)
		    {
		        isr_func(intno, g_interrupt_handlers[intno].param);
		    }

			/* end of interrupt */
			xm_ack_irq(intno);
		}
	}

    krhino_intrpt_exit();
}

