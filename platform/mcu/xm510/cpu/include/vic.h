#ifndef __VIC_H__
#define __VIC_H__

void xm_unmask_irq(unsigned int irq);
void xm_ack_irq(unsigned int irq);
void xm_mask_irq(unsigned int irq);
void xm510_init_irq(void);


#endif

