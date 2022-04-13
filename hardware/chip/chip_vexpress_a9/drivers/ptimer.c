#include <stdbool.h>
#include "ptimer.h"
#include "irq.h"

static private_timer_registers* regs;
static const uint32_t refclock = 100000000u; /* 24 MHz */
static volatile uint32_t systime;

/* The private timer's period is:
 *
 * (Prescaler + 1) * (Load value + 1)
 * ----------------------------------
 *             Refclock
 *
 *
 * Simplified, if prescaler == 0, then:
 *   Load value = (period * refclock) - 1
 */

static bool validate_config(uint16_t millisecs) {
    /* Very simplified - assume we leave prescaler at 0
     * Then the largest possible numerator is just UINT32_MAX */
    uint32_t max_period = UINT32_MAX / refclock;
    uint32_t max_millis = max_period * 1000u;

    return millisecs < max_millis;
}

static uint32_t millisecs_to_timer_value(uint16_t millisecs) {
    double period = millisecs * 0.001;
    uint32_t value = (period * refclock) - 1;

    return value;
}

ptimer_error ptimer_init(uint16_t millisecs) {
    regs = (private_timer_registers*)PTIMER_BASE;
    if (!validate_config(millisecs)) {
        return PTIMER_INVALID_PERIOD;
    }
    uint32_t load_val = millisecs_to_timer_value(millisecs);
    
    WRITE32(regs->LR, load_val); /* Load the initial timer value */

    /* Register the interrupt */
    (void)irq_register_isr(PTIMER_INTERRUPT, ptimer_isr);

    uint32_t ctrl = CTRL_AUTORELOAD | CTRL_IRQ_ENABLE | CTRL_ENABLE;
    WRITE32(regs->CTRL, ctrl); /* Configure and start the timer */
    gic_enable_interrupt(PTIMER_INTERRUPT);

    return PTIMER_OK;
}

void ptimer_isr(void) {
    WRITE32(regs->ISR, ISR_CLEAR); /* Clear the interrupt */
    systime++;
}

uint32_t get_ptime_count()
{
    return systime;
}
