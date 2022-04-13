#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

typedef void (*isr_ptr)(int , void*);

#define ISR_COUNT   (1024)
#define MAX_ISR     (ISR_COUNT - 1)


typedef enum {
    IRQ_OK = 0,
    IRQ_INVALID_IRQ_ID,
    IRQ_ALREADY_REGISTERED
} irq_error;

irq_error irq_register_isr(uint16_t irq_number, isr_ptr callback);

#endif
