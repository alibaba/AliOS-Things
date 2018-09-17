#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define OS_MAX_IRQ_NUM               IRQ_XM510_END
#define INTERRUPT_NAME_MAX	32
typedef void (*irq_hdlr_t)(int irq, void *param);

typedef struct os_irq_desc
{
    irq_hdlr_t    irq_handler;
    void          *param;
    char          name[INTERRUPT_NAME_MAX];
    uint32_t      counter;
}os_irq_desc;

/*
 * Interrupt interfaces
 */
void os_hw_interrupt_init(void);
irq_hdlr_t os_hw_interrupt_create(int irq, irq_hdlr_t irq_handler,void *param, char *name);
void os_hw_interrupt_enable(int irq);
void os_hw_interrupt_disable(int irq);



#endif /* end of include guard: __INTERRUPT_H__ */

