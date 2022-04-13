#include <stddef.h>
#include "irq.h"
#include "gic.h"

static isr_ptr callbacks[1024] = { NULL };

static isr_ptr callback(uint16_t number);

void irq_handler(void) {
    uint16_t irq = gic_acknowledge_interrupt();
    isr_ptr isr = callback(irq);
    if (isr != NULL) {
        isr();
    }
    gic_end_interrupt(irq);
}

irq_error irq_register_isr(uint16_t irq_number, isr_ptr callback) {
    if (irq_number > MAX_ISR) {
        return IRQ_INVALID_IRQ_ID;
    } else if (callbacks[irq_number] != NULL) {
        return IRQ_ALREADY_REGISTERED;
    } else {
        callbacks[irq_number] = callback;
    }
    return IRQ_OK;
}

static isr_ptr callback(uint16_t number) {
    if (number > MAX_ISR) {
        return NULL;
    }
    return callbacks[number];
}
