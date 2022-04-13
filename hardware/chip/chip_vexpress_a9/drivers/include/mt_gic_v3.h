/*
 * Copyright (c) 2018 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __MT_GIC_V3_H
#define __MT_GIC_V3_H
//#include <printf-stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <irq.h>

#ifndef NULL
#define NULL		0U
#endif

/*
 * Define IRQ code.
 */
#define GIC_SPI_START	32
#define MT_NR_SPI	225
#define NR_IRQ_LINE  (MT_NR_SPI + GIC_SPI_START)

#define	IRQ_TYPE_EDGE_RISING	0x00000001
#define	IRQ_TYPE_EDGE_FALLING	0x00000002
#define	IRQ_TYPE_EDGE_BOTH	(IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define	IRQ_TYPE_LEVEL_HIGH	0x00000004
#define	IRQ_TYPE_LEVEL_LOW	0x00000008

enum {
	/* Ignore cpu_mask and forward interrupt to all CPUs other than the current cpu */
	ARM_GIC_SGI_FLAG_TARGET_FILTER_NOT_SENDER = 0x1,
	/* Ignore cpu_mask and forward interrupt to current CPU only */
	ARM_GIC_SGI_FLAG_TARGET_FILTER_SENDER = 0x2,
	ARM_GIC_SGI_FLAG_TARGET_FILTER_MASK = 0x3,
	/* Only forward the interrupt to CPUs that has the interrupt configured as group 1 (non-secure) */
	ARM_GIC_SGI_FLAG_NS = 0x4,
};

typedef void (*irq_handler_t) (int, void *);

/** @ingroup type_group_scp_intc_ext_struct
 * @brief INTC irq event structure\n
 */
struct irq_desc_t {
/** interrupt event handler */
	irq_handler_t handler;
/** interrupt event device */
	void *dev;
/** interrupt event name */
	const char *name;
/** interrupt event count */
	uint32_t irq_count;
/** interrupt wakeup count */
	uint32_t wakeup_count;
/** initialization times of an interrupt */
	uint32_t init_count;
/** last event handled enter time */
	uint64_t last_enter;
/** last event handled exit time */
	uint64_t last_exit;
/** interrupt event duraiton  */
	uint64_t max_duration;
};

#define ICC_SGI1R_TARGET_LIST_SHIFT	0
#define ICC_SGI1R_AFFINITY_1_SHIFT	16
#define ICC_SGI1R_SGI_ID_SHIFT		24

static inline void gic_write_sgi1r(unsigned long long val)
{
	//write_sysreg(val, ICC_SGI1R);
	__asm__ volatile("mcrr p15, 0, %Q0, %R0, c12" : : "r" (val));
}


/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
#if 0
int request_irq(uint32_t irq, irq_handler_t handler, uint32_t type, const char *name, void *dev);
void free_irq(uint32_t irq);
uint32_t enable_irq(uint32_t irq);
uint32_t disable_irq(uint32_t irq);
#endif

int irq_set_type(uint32_t irq, uint32_t type);
uint32_t mt_gic_icc_primask_read(void);
uint32_t mt_gic_icc_igrpen1_read(void);
uint32_t mt_gic_icc_iar1_read(void);
uint32_t mt_gic_icc_sre_read(void);
uint32_t mt_gic_icc_bpr1_read(void);
void *platform_init_interrupts(void);
void *gic_gicd_get_address(void);
void hw_isr_dispatch(void);
void gic_send_sgi(unsigned long long cluster_id, unsigned short tlist, unsigned int irq);
irq_error irq_register_isr(uint16_t irq_number, isr_ptr callback);
void gic_init(void);
void gic_enable_interrupt(uint16_t number);
void gic_disable_interrupt(uint16_t number) ;
void gic_end_interrupt(uint32_t number);

#endif

