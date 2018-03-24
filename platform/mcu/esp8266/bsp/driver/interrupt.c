#include <stdint.h>
#include <stddef.h>

#include "frxt/xtensa_api.h"

typedef void (* _xt_isr_t)(void *arg);

void ResetCcountVal(uint32_t cnt_val)
{
    asm volatile("wsr a2, ccount");
}

// void _xt_isr_mask(uint32_t mask)
// {
//     xt_ints_off(mask);
// }

// void _xt_isr_unmask(uint32_t mask)
// {
//     xt_ints_on(mask);
// }

void _xt_isr_attach(uint8_t i, _xt_isr_t func, void *arg)
{
    xt_set_interrupt_handler(i, func, arg);
}
