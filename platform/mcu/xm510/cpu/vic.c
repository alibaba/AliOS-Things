#include <board.h>
#include "vic.h"
#include <aos/types.h>


/*vic handle*/

void xm_ack_irq(unsigned int irq)
{
	//writel(1<<irq, (INTC_INTENCLEAR));
}

void xm_mask_irq(unsigned int irq)
{
	writel(1<<irq, INTC_INTENCLEAR);
}

void xm_unmask_irq(unsigned int irq)
{
	uint32_t value = readl(INTC_INTENABLE);
	writel(value | ( 1 << irq), INTC_INTENABLE);
}

void  xm510_init_irq(void)
{
//全部使用普通中断，不使用软中断，以后可以改为使用fiq 及vector中断
	writel(~0, INTC_INTENCLEAR);
	writel(0, INTC_INTSELECT);
	writel(~0, INTC_SOFTINTCLEAR);
}

