#ifndef CPU_A9_H
#define CPU_A9_H

#include <stdint.h>

#define WRITE32(_reg, _val) (*(volatile uint32_t*)&_reg = _val)

#define PERIPHEAL_BASE_ADDR     (0x1E000000u)
#define GIC_IFACE_OFFSET        (0x100u)
#define GIC_DISTRIBUTOR_OFFSET  (0x1000u)
#define PTIMER_OFFSET		    (0x600u)

static inline uint32_t cpu_get_periphbase(void);
static inline void cpu_enable_interrupts(void);

static inline uint32_t cpu_get_periphbase(void) {
    uint32_t result;
    asm ("mrc p15, #4, %0, c15, c0, #0" : "=r" (result));
    return result;
}

static inline void cpu_enable_interrupts(void) {
    asm ("cpsie if");
}

#endif
