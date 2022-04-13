#ifndef __ARM_ARCH_TIMER_H
#define __ARM_ARCH_TIMER_H

#include "mt_irq_define.h"
#include "mt_gic_v3.h"

#define ARCH_TIMER_CTRL_ENABLE		(1 << 0)
#define ARCH_TIMER_CTRL_IT_MASK		(1 << 1)
#define ARCH_TIMER_CTRL_IT_STAT		(1 << 2)

enum arch_timer_reg {
	ARCH_TIMER_REG_CTRL,
	ARCH_TIMER_REG_TVAL,
};

#define ARCH_TIMER_PHYS_ACCESS		0
#define ARCH_TIMER_VIRT_ACCESS		1
#define ARCH_TIMER_MEM_PHYS_ACCESS	2
#define ARCH_TIMER_MEM_VIRT_ACCESS	3

#define ARCH_TIMER_USR_PCT_ACCESS_EN	(1 << 0) /* physical counter */
#define ARCH_TIMER_USR_VCT_ACCESS_EN	(1 << 1) /* virtual counter */
#define ARCH_TIMER_VIRT_EVT_EN		(1 << 2)
#define ARCH_TIMER_EVT_TRIGGER_SHIFT	(4)
#define ARCH_TIMER_EVT_TRIGGER_MASK	(0xF << ARCH_TIMER_EVT_TRIGGER_SHIFT)
#define ARCH_TIMER_USR_VT_ACCESS_EN	(1 << 8) /* virtual timer registers */
#define ARCH_TIMER_USR_PT_ACCESS_EN	(1 << 9) /* physical timer registers */

/*
 * Frequency shoulde be read from register cntfrq_el0,
 * Not fixed with hard code.
 */
#define MT_ARCH_TIMER_FREQUENCY ((unsigned int)13000000)
#define DELAY_TIMER_1US_TICK    ((unsigned int)13)    // (13MHz)
#define DELAY_TIMER_1MS_TICK    ((unsigned int)13000) // (13MHz)

#define TIME_TO_TICK_US(us)     ((us)*DELAY_TIMER_1US_TICK)
#define TIME_TO_TICK_MS(ms)     ((ms)*DELAY_TIMER_1MS_TICK)

extern int platform_set_periodic_timer(irq_handler_t handler, void *arg,
                                unsigned long msec_interval);
extern void vConfigureTickInterrupt(void);
extern void udelay(unsigned long usec);
extern void mdelay(unsigned long msec);
extern void ndelay(unsigned long nsec);
#endif  /* __ARM_ARCH_TIMER_H */
