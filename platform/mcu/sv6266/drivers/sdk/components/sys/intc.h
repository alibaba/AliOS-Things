#ifndef _INTC_H_
#define _INTC_H_

#include <stdint.h>
#include <stddef.h>

typedef void (*isr_func) (uint32_t irq_num);

void intc_init (void);

void intc_register_isr (uint32_t irq_num, isr_func isr);
void intc_irq_clean (uint32_t irq_num);
void intc_irq_clean_all (void);

void intc_irq_set_priority (uint32_t irq_num, uint32_t irq_pri);
void intc_irq_set_priority_all (uint32_t irq_pri1, uint32_t irq_pri2);

void intc_irq_enable (uint32_t irq_num);
void intc_irq_disable (uint32_t irq_num);
void intc_irq_disable_all (void);
uint32_t intc_irq_status (void);

void intc_group02_irq_enable (uint32_t group02_irq_num, isr_func isr);
void intc_group02_irq_disable (uint32_t group02_irq_num);
uint32_t intc_group02_irq_status (void);

void intc_group15_irq_enable (uint32_t group15_irq_num, isr_func isr);
void intc_group15_irq_disable (uint32_t group15_irq_num);
uint32_t intc_group15_irq_status (void);

void intc_group31_irq_enable (uint32_t group31_irq_num, isr_func isr);
void intc_group31_irq_disable (uint32_t group31_irq_num);
uint32_t intc_group31_irq_status (void);

#endif // #ifndef  _INTC_H_
