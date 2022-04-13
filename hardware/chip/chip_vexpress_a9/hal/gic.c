#include "mt_gic_v3.h"

void os_fiq_handle(void)
{
}

void os_irq_handle(void)
{
	hw_isr_dispatch();
}