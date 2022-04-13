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

//#include "err.h"
#include "mt_gic_v3.h"
#include "mt_reg_base.h"
#include <mt_printf.h>

#define ICC_IAR1_EL1	sys_reg(3, 0, 12, 12, 0)
#define ICC_EOIR1_EL1	sys_reg(3, 0, 12, 12, 1)
#define ICC_IAR0_EL1	sys_reg(3, 0, 12,  8, 0)
#define ICC_EOIR0_EL1	sys_reg(3, 0, 12,  8, 1)
#define ICC_HPPIR1_EL1	sys_reg(3, 0, 12, 12, 2)
#define GICD_CTLR_ENABLE_GRP0		(1 << 0)
#define GICD_CTLR_ENGRP1NS      	(1 << 1)
#define GICD_CTLR_ENGRP1S   		(1 << 2)
#define GICD_CTLR_ARE       		(1 << 4)
#define GICD_CTLR_ARE_NS                (1 << 5)
#define GICD_CTLR_DS			(1 << 6)
#define GICD_CTLR_E1NWF			(1 << 7)
#define GICD_CTLR_RWP       		(1 << 31)
#define GICR_WAKER_ProcessorSleep	(1 << 1)
#define GICR_WAKER_ChildrenAsleep	(1 << 2)
/* GICD_TYPER bit definitions */
#define IT_LINES_NO_MASK    (0x1f)
#define ENABLE_GRP0		(1 << 0)
#define ENABLE_GRP1		(1 << 1)
/* Mask for the priority field common to all GIC interfaces */
#define GIC_PRI_MASK    0xff
/* GICC_CTLR bit definitions */
#define EOI_MODE_NS		(1 << 10)
#define EOI_MODE_S		(1 << 9)
#define IRQ_BYP_DIS_GRP1	(1 << 8)
#define FIQ_BYP_DIS_GRP1	(1 << 7)
#define IRQ_BYP_DIS_GRP0	(1 << 6)
#define FIQ_BYP_DIS_GRP0	(1 << 5)
#define CBPR			(1 << 4)
#define FIQ_EN			(1 << 3)
#define ACK_CTL			(1 << 2)
/* GICv3 ICC_SRE register bit definitions*/
#define ICC_SRE_EN		(1 << 3)
#define ICC_SRE_SRE		(1 << 0)
/* GICC_IIDR bit masks and shifts */
#define GICC_IIDR_PID_SHIFT 20
#define GICC_IIDR_ARCH_SHIFT 16
#define GICC_IIDR_REV_SHIFT 12
#define GICC_IIDR_IMP_SHIFT	0
#define GICC_IIDR_PID_MASK	0xfff
#define GICC_IIDR_ARCH_MASK	0xf
#define GICC_IIDR_REV_MASK	0xf
#define GICC_IIDR_IMP_MASK	0xfff
#define SZ_64K			(0x00010000)
#define INT_POL_SECCTL_NUM	20
#define NR_INT_POL_CTL		(20)
/* main cpu regs */
#define GICC_CTLR               0x0000
#define GICC_PMR                0x0004
#define GICC_BPR                0x0008
#define GICC_IAR                0x000c
#define GICC_EOIR               0x0010
#define GICC_RPR                0x0014
#define GICC_HPPIR              0x0018
#define GICC_APBR               0x001c
#define GICC_AIAR               0x0020
#define GICC_AEOIR              0x0024
#define GICC_AHPPIR            	0x0028
#define GICC_APR                0x00d0
#define GICC_NSAPR              0x00e0
#define GICC_IIDR               0x00fc
#define GICC_DIR                0x1000
/* distribution regs */
#define GICD_CTLR               0x000
#define GICD_TYPER              0x004
#define GICD_IIDR               0x008
#define GICD_STATUSR      	0x010
#define GICD_SEIR		0x068
#define GICD_IGROUPR            0x080
#define GICD_ISENABLER		0x100
#define GICD_ICENABLER		0x180
#define GICD_ISPENDR		0x200
#define GICD_ICPENDR		0x280
#define GICD_ISACTIVER		0x300
#define GICD_ICACTIVER		0x380
#define GICD_IPRIORITYR		0x400
#define GICD_ITARGETSR		0x800
#define GICD_ICFGR		0xc00
#define GICD_IGRPMODR		0xd00
#define GICD_NSACR		0xe00
#define GICD_SGIR               0xf00
#define GICD_CPENDSGIR		0xf10
#define GICD_SPENDSGIR		0xf20
#define GICD_ROUTE              0x6100
#define GICD_PIDR2		0xFFE8

/*
 * Re-Distributor registers, offsets from RD_base
 */
#define GICR_V3_CTLR                       0x0000
#define GICR_V3_IIDR                       0x0004
#define GICR_V3_TYPER                      0x0008
#define GICR_V3_STATUSR                    0x0010
#define GICR_V3_WAKER                      0x0014
#define GICR_V3_SETLPIR                    0x0040
#define GICR_V3_CLRLPIR                    0x0048
#define GICR_V3_PROPBASER                  0x0070
#define GICR_V3_PENDBASER                  0x0078
#define GICE_V3_IGROUP0			   0x0080
#define GICR_V3_INVLPIR                    0x00A0
#define GICR_V3_INVALLR                    0x00B0
#define GICR_V3_SYNCR                      0x00C0
#define GICR_V3_MOVLPIR                    0x0100
#define GICR_V3_MOVALLR                    0x0110
#define GICE_V3_IGRPMOD0		   0x0d00
#define GIC_V3_PIDR2_ARCH_MASK             0xf0
#define GIC_V3_PIDR2_ARCH_GICv3            0x30
#define GIC_V3_PIDR2_ARCH_GICv4            0x40
#define GICR_V3_WAKER_ProcessorSleep       (1U << 1)
#define GICR_V3_WAKER_ChildrenAsleep       (1U << 2)
/*******************************************************************************
 * GICv3 CPU interface registers & constants
 ******************************************************************************/
/* SCR bit definitions */
#define SCR_NS_BIT                         (1U << 0)
/* ICC_SRE bit definitions*/
#define ICC_SRE_EN_BIT                     (1 << 3)
#define ICC_SRE_DIB_BIT                    (1 << 2)
#define ICC_SRE_DFB_BIT                    (1 << 1)
#define ICC_SRE_SRE_BIT                    (1 << 0)

#define LTRACEF(s) printf(s)

#define configSECURE_WORLD 1

static struct irq_desc_t irq_desc[NR_IRQ_LINE];

#define READ_REGISTER_UINT32(reg) \
    (*(volatile unsigned int * const)(reg))

#define WRITE_REGISTER_UINT32(reg, val) \
    (*(volatile unsigned int * const)(reg)) = (val)
#define INREG32(x)          READ_REGISTER_UINT32((unsigned int *)(x))
#define OUTREG32(x, y)      WRITE_REGISTER_UINT32((unsigned int *)(x), (unsigned int )(y))
#define DRV_Reg32(addr)             INREG32(addr)
#define DRV_WriteReg32(addr, data)  OUTREG32(addr, data)

#define dsb()   \
        do {    \
            __asm__ __volatile__ ("dsb" : : : "memory"); \
        } while (0)

static void mt_gic_icc_primask_write(uint32_t reg)
{
	__asm__ volatile("MCR p15, 0, %0, c4, c6, 0" :: "r" (reg)); //ICC_PMR
	dsb();
}

uint32_t mt_gic_icc_primask_read(void)
{
	uint32_t reg;
	__asm__ volatile("MRC p15, 0, %0, c4, c6, 0" : "=r" (reg));
	return reg;
}

static void mt_gic_icc_igrpen1_write(uint32_t reg)
{
	__asm__ volatile("MCR p15, 0, %0, c12, c12, 7" :: "r" (reg));
}

uint32_t mt_gic_icc_igrpen1_read(void)
{
	uint32_t reg;
	__asm__ volatile("MRC p15, 0, %0, c12, c12, 7" : "=r" (reg));
	return reg;
}

uint32_t mt_gic_icc_iar1_read(void)
{
	uint32_t reg;
	__asm__ volatile("MRC p15, 0, %0, c12, c12, 0" : "=r" (reg));
	return reg;
}

static void mt_gic_icc_msre_write(void)
{
	uint32_t reg;
#define MON_MODE    "#22"
#define SVC_MODE    "#19"
	/*
	 * switch to monitor mode and mark ICC_MSRE.
	 */
	__asm__ volatile("CPS " MON_MODE "\n"
	                 "MRC p15, 6, %0, c12, c12, 5\n"
	                 "ORR %0, %0, #9\n"
	                 "MCR p15, 6, %0, c12, c12, 5\n"
	                 "CPS " SVC_MODE "\n" : "=r" (reg));
	dsb();
}

static void mt_gic_icc_sre_write(uint32_t reg)
{
	__asm__ volatile("MCR p15, 0, %0, c12, c12, 5" :: "r" (reg));  //ICC_SRE
	dsb();
}

uint32_t mt_gic_icc_sre_read(void)
{
	uint32_t reg;
	__asm__ volatile("MRC p15, 0, %0, c12, c12, 5" : "=r" (reg));
	return reg;
}

uint32_t mt_gic_icc_bpr1_read(void)
{
	uint32_t reg;
	__asm__ volatile("MRC p15, 0, %0, c12, c12, 3" : "=r" (reg));
	return reg;
}

static inline void gic_write_eoir(unsigned int irq)
{
	asm volatile("MCR p15, 0, %0, c12, c12, 1" : : "r" (irq));
	dsb();
}

/** @ingroup type_group_scp_intc_ext_API
 * @par Description
 *     Enable irq.
 * @param[in]
 *     irq: irq interrupt number.
 * @return
 *     None.
 * @par Boundary case and Limitation
 *     irq < NR_IRQ_LINE.
 * @par Error case and Error handling
 *     None.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
uint32_t enable_irq(uint32_t irq)
{
	uint32_t mask = 1 << (irq % 32);

	if (irq >= NR_IRQ_LINE || irq_desc[irq].handler == NULL) {
		printf("ERROR Interrupt ID %lu or handler\n\r", irq);
		return -1;
	}

	//PRINTF_D("\tmask is: 0x%x\n", mask);
	if (irq < 32) {
		//PRINTF_D("\t0x0c090100: 0x%x\n", INREG32(GICR_BASE + SZ_64K + GICD_ISENABLER));
		DRV_WriteReg32(GICR_BASE + SZ_64K + GICD_ISENABLER, mask);
		//PRINTF_D("\t0x0c090100: 0x%x\n", INREG32(GICR_BASE + SZ_64K + GICD_ISENABLER));
	}
	else
		DRV_WriteReg32(GICD_BASE + GICD_ISENABLER + irq / 32 * 4, mask);

	return 0;
}

/** @ingroup type_group_scp_intc_ext_API
 * @par Description
 *     Disable irq.
 * @param[in]
 *     irq: irq interrupt number.
 * @return
 *     None.
 * @par Boundary case and Limitation
 *     irq < IRQ_MAX_CHANNEL.
 * @par Error case and Error handling
 *     None.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
uint32_t disable_irq(uint32_t irq)
{
	uint32_t mask = 1 << (irq % 32);

	if (irq >= NR_IRQ_LINE) {
		printf("ERROR IRQ ID %lu\n\r", irq);
		return -1;
	}

	if (irq < 32)
		DRV_WriteReg32(GICR_BASE + SZ_64K + GICD_ICENABLER, mask);
	else
		DRV_WriteReg32(GICD_BASE + GICD_ICENABLER + irq / 32 * 4, mask);

	return 0;
}

static void gic_ack_irq(unsigned int irq)
{
	gic_write_eoir(irq);
}

void set_irq_priority(uint32_t irq, uint32_t priority)
{
	uint32_t reg = irq / 4;
	uint32_t shift = 8 * (irq % 4);
	uint32_t mask = 0xff << shift;
	uint32_t regval;

	regval = DRV_Reg32(GICD_BASE + GICD_IPRIORITYR + reg);
	regval = (regval & ~mask) | (priority << shift);
	DRV_WriteReg32(GICD_BASE + GICD_IPRIORITYR + reg * 4, regval);
}

uint32_t get_irq_priority(uint32_t irq)
{
	uint32_t reg = irq / 4;
	uint32_t shift = 8 * (irq % 4);
	uint32_t priority;

	priority = (DRV_Reg32(GICD_BASE + GICD_IPRIORITYR + reg * 4) >> shift) & 0xff;

	return priority;
}


static void set_affinity_local(uint32_t irq)
{
	uintptr_t addr = GICD_BASE + GICD_ITARGETSR + (irq & ~3);
	int cpu_idx = get_cpu_idx();
	uint32_t mask = 1 << cpu_idx;
	uint32_t shift = (irq & 3) << 3;
	uint32_t reg_val;

	reg_val = DRV_Reg32(addr);
	reg_val &= ~(0xff << shift);
	reg_val |= mask << shift;

	DRV_WriteReg32(addr, reg_val);
}

/*
 * irq_set_sensitivity: Set IRQ sensitivity.
 * @irq: IRQ number
 * @type: IRQ type (IRQ_TYPE_xxx)
 */
void irq_set_sensitivity(uint32_t irq, uint32_t type)
{
	unsigned int config;

	/* set sensitivity */
	if (type & (IRQ_TYPE_EDGE_BOTH)) {
		config = DRV_Reg32(GICD_BASE + GICD_ICFGR + (irq / 16) * 4);
		config |= (0x2 << (irq % 16) * 2);
		DRV_WriteReg32(GICD_BASE + GICD_ICFGR + (irq / 16) * 4, config);
	} else {
		config = DRV_Reg32(GICD_BASE + GICD_ICFGR + (irq / 16) * 4);
		config &= ~(0x2 << (irq % 16) * 2);
		DRV_WriteReg32(GICD_BASE + GICD_ICFGR + (irq / 16) * 4, config);
	}
}

/*
 * irq_set_polarity: Set IRQ polarity.
 * @irq: IRQ number
 * @type: IRQ type (IRQ_TYPE_xxx)
 */
void irq_set_polarity(uint32_t irq, uint32_t type)
{
	unsigned int offset;
	unsigned int reg_index;
	unsigned int value;

	reg_index = (irq - GIC_SPI_START) >> 5;
	offset = (irq - GIC_SPI_START) & 0x1f;
	if (type & (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_LEVEL_LOW)) {
		value = DRV_Reg32(INT_POL_CTL0 + (reg_index * 4));
		value |= (1 << offset);
		DRV_WriteReg32(INT_POL_CTL0 + (reg_index * 4), value);
	} else {
		value = DRV_Reg32(INT_POL_CTL0 + (reg_index * 4));
		value &= (0xFFFFFFFF ^ (0x1 << offset));
		DRV_WriteReg32(INT_POL_CTL0 + (reg_index * 4), value);
	}
}

/*
 * irq_set_type: Set IRQ type (sensitivity and polarity).
 * @irq: IRQ number
 * @type: IRQ type (IRQ_TYPE_xxx)
 * Return 0 if success; Return negative values if failure.
 */
int irq_set_type(uint32_t irq, uint32_t type)
{
	if (irq < 32) {
		//printf("irq < 32, No need set trigger type\n", irq);
		return -1;
	}

	irq_set_sensitivity(irq, type);
	irq_set_polarity(irq, type);
	return 0;
}

#if 0

/** @ingroup type_group_scp_intc_ext_API
 * @par Description
 *     Register a irq handler.
 * @param[in]
 *     irq: irq interrupt number.
 * @param[in]
 *     handler: irq handler.
 * @param[in]
 *     *name: interrupt name.
 * @param[in]
 *     *dev: interrupt device.
 * Return 0 if success; Return negative values if failure.
 * @par Boundary case and Limitation
 *     irq < IRQ_MAX_CHANNEL.
 * @par Error case and Error handling
 *     None.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int request_irq(uint32_t irq, irq_handler_t handler, uint32_t type, const char *name, void *dev)
{
	if (irq < NR_IRQ_LINE) {
		disable_irq(irq);
		irq_desc[irq].name = name;
		irq_desc[irq].handler = handler;
		irq_desc[irq].dev = dev;
		irq_desc[irq].irq_count = 0;
		irq_desc[irq].wakeup_count = 0;
		irq_desc[irq].init_count++;
		irq_desc[irq].last_enter = 0;
		irq_desc[irq].last_exit = 0;
		irq_desc[irq].max_duration = 0;
		irq_set_type(irq, type);
		enable_irq(irq);
        //printf("request_irq: IRQ ID %lu\n\r", irq);
		return 0;
	} else {
		printf("request_irq: ERR IRQ ID %lu\n\r", irq);
		return -1;
	}
}

/** @ingroup type_group_scp_intc_ext_API
 * @par Description
 *     Unregister an irq handler.
 * @param[in]
 *     irq: irq interrupt number.
 * @param[in]
 *     dev: device identity to free.
 * @return
 *     None.
 * @par Boundary case and Limitation
 *     irq < IRQ_MAX_CHANNEL.
 * @par Error case and Error handling
 *     None.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
void free_irq(uint32_t irq)
{
	if (irq < NR_IRQ_LINE) {
		irq_desc[irq].handler = NULL;
		disable_irq(irq);

		if (irq_desc[irq].last_exit < irq_desc[irq].last_enter)
			PRINTF_W("free_irq:IRQ ID %lu maybe processing\n\r", irq);
	} else {
		PRINTF_E("free_irq:ERR IRQ ID %lu\n\r", irq);
	}
}
#endif
/** @ingroup type_group_scp_intc_ext_API
 * @par Description
 *     IRQ data structure initialization.
 * @par Parameters
 *     None.
 * @return
 *     None.
 * @par Boundary case and Limitation
 *     None.
 * @par Error case and Error handling
 *     None.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void irq_init(void)
{
	uint32_t irq;

	for (irq = 0; irq < NR_IRQ_LINE; irq++) {
		irq_desc[irq].name = "";
		irq_desc[irq].handler = NULL;
		irq_desc[irq].dev = NULL;
		irq_desc[irq].irq_count = 0;
		irq_desc[irq].wakeup_count = 0;
		irq_desc[irq].init_count = 0;
		irq_desc[irq].last_enter = 0;
		irq_desc[irq].last_exit = 0;
		irq_desc[irq].max_duration = 0;
	}
}

void gic_send_sgi(unsigned long long cluster_id, unsigned short tlist, unsigned int irq)
{
	unsigned long long val;
#if 0
	val = (MPIDR_TO_SGI_AFFINITY(cluster_id, 3)	|
	       MPIDR_TO_SGI_AFFINITY(cluster_id, 2)	|
	       irq << ICC_SGI1R_SGI_ID_SHIFT		|
	       MPIDR_TO_SGI_AFFINITY(cluster_id, 1)	|
	       tlist << ICC_SGI1R_TARGET_LIST_SHIFT);
#else
	val = (irq << ICC_SGI1R_SGI_ID_SHIFT) | (((cluster_id >> 8) & 0xff) << ICC_SGI1R_AFFINITY_1_SHIFT) | tlist;
#endif

	PRINTF_D("CPU%d: ICC_SGI1R_EL1 %llx\n", tlist, val);
	gic_write_sgi1r(val);
}

/** @ingroup type_group_scp_intc_InFn
 * @par Description
 *     Low level interrupt service routine.
 * @par Parameters
 *     None.
 * @return
 *     None.
 * @par Boundary case and Limitation
 *     None.
 * @par Error case and Error handling
 *     None.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
#if 0
void hw_isr_dispatch(void)
{
	uint64_t duration;
	void *dev;
	// get the current vector
	uint32_t iar = mt_gic_icc_iar1_read();
	unsigned int vector = iar & 0x3ff;

	//printf("\t%s, vector id is %d\n", __func__, vector);

	if (vector >= NR_IRQ_LINE) {
		// spurious
		return;
	}

	if (vector < NR_IRQ_LINE) {
		if (irq_desc[vector].handler) {
			irq_desc[vector].irq_count++;
			dev = irq_desc[vector].dev;
			irq_desc[vector].handler(vector, dev);
			disable_irq(vector);
			gic_ack_irq(vector);

			/* record the last handled interrupt duration, unit: (ns) */
			duration = irq_desc[vector].last_exit -
				   irq_desc[vector].last_enter;
			/* handle the xgpt overflow case
			 * discard the duration time when exit time < enter time
			 */
			if (irq_desc[vector].last_exit >
			    irq_desc[vector].last_enter)
				irq_desc[vector].max_duration =
				    duration;

			enable_irq(vector);
		} else {
			disable_irq(vector);
			gic_ack_irq(vector);
			//enable_irq(vector); //Greta temp solution since 29 continues come in
			printf("IRQ ID %lu handler is null, disable it\n\r",
			       vector);

		}
	} else {
		printf("ERROR IRQ ID %lu\n\r", vector);
	}
}
#else
void hw_isr_dispatch(void)
{
    irq_handler();
}
#endif
static void mt_gic_cpu_init(void)
{
	mt_gic_icc_sre_write(0x01);
	mt_gic_icc_primask_write(0xF0);
	mt_gic_icc_igrpen1_write(0x01);
	dsb();
}
static void mt_gic_redist_init(void)
{
	//unsigned int i;
	unsigned int value;

	//for (i = 0; i < 32/*NR_IRQ_LINE*/; i += 32) {
#if (configSECURE_WORLD == 1)
		DRV_WriteReg32(GICR_BASE + SZ_64K + GICD_IGROUPR/* + i * 4 / 32*/, 0x0);
		DRV_WriteReg32(GICR_BASE + SZ_64K + GICD_IGRPMODR/* + i * 4 / 32*/, 0xFFFFFFFF);
#else
		DRV_WriteReg32(GICR_BASE + SZ_64K + GICD_IGROUPR/* + i * 4 / 32*/, 0xFFFFFFFF);
		DRV_WriteReg32(GICR_BASE + SZ_64K + GICD_IGRPMODR/* + i * 4 / 32*/, 0x0);
#endif
	//}
	/*
	 * Disable all interrupts < 32.
	 */
	//printf("\t0x%x: 0x%x\n", GICR_BASE + SZ_64K + GICD_ISENABLER, INREG32(GICR_BASE + SZ_64K + GICD_ISENABLER));
	DRV_WriteReg32(GICR_BASE + SZ_64K + GICD_ICENABLER, 0xFFFFFFFF);
	//printf("\t0x%x: 0x%x\n", GICR_BASE + SZ_64K + GICD_ISENABLER, INREG32(GICR_BASE + SZ_64K + GICD_ISENABLER));

	/* Wake up this CPU redistributor */
	value = DRV_Reg32(GICR_BASE + GICR_V3_WAKER);
	value &= ~GICR_WAKER_ProcessorSleep;
	DRV_WriteReg32(GICR_BASE + GICR_V3_WAKER, value);
	while (DRV_Reg32(GICR_BASE + GICR_V3_WAKER) & GICR_WAKER_ChildrenAsleep);
}
static void mt_git_dist_rwp(void)
{
	/*
	 * check GICD_CTLR.RWP for done check
	 */
	while (DRV_Reg32(GICD_BASE + GICD_CTLR) & GICD_CTLR_RWP) {
	}
}
static void mt_gic_dist_init(void)
{
	unsigned int i;
	uint64_t affinity;
	DRV_WriteReg32(GICD_BASE + GICD_CTLR, GICD_CTLR_ARE);
	mt_git_dist_rwp();

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for (i = GIC_SPI_START; i < NR_IRQ_LINE; i += 16) {
		DRV_WriteReg32(GICD_BASE + GICD_ICFGR + i * 4 / 16, 0);
	}

	/*
	 * Set all global interrupts to CPU0 only.
	 */
	for (i = GIC_SPI_START; i < NR_IRQ_LINE; i++) {
		DRV_WriteReg32(GICD_BASE + GICD_ROUTE + i * 8, 0x0);
	}

	/*
	 * Set all interrupts to G1S.  Leave the PPI and SGIs alone
	 * as they are set by redistributor registers.
	 */

	for (i = GIC_SPI_START; i < NR_IRQ_LINE; i += 32)
	{
#if (configSECURE_WORLD == 1) //8512
		DRV_WriteReg32(GICD_BASE + GICD_IGROUPR + i * 4 / 32, 0x0);
		DRV_WriteReg32(GICD_BASE + GICD_IGRPMODR + i * 4 / 32, 0xFFFFFFFF);
#else   //8133(8168)
		DRV_WriteReg32(GICD_BASE + GICD_IGROUPR + i * 4 / 32, 0xFFFFFFFF);
		DRV_WriteReg32(GICD_BASE + GICD_IGRPMODR + i * 4 / 32, 0x0);
#endif
	}

	/*
	 * Set priority on all interrupts.
	 */
	for (i = GIC_SPI_START; i < NR_IRQ_LINE; i += 4) {
		DRV_WriteReg32(GICD_BASE + GICD_IPRIORITYR + i * 4 / 4, 0xA0A0A0A0);
	}

	/*
	 * Disable all interrupts.
	 */
	for (i = GIC_SPI_START; i < NR_IRQ_LINE; i += 32) {
		DRV_WriteReg32(GICD_BASE + GICD_ICENABLER + i * 4 / 32, 0xFFFFFFFF);
	}

	/*
	 * Clear all active status
	 */
	for (i = GIC_SPI_START; i < NR_IRQ_LINE; i += 32) {
		DRV_WriteReg32(GICD_BASE + GICD_ICACTIVER + i * 4 / 32, 0xFFFFFFFF);
	}

	/*
	 * Clear all pending status
	 */
	for (i = GIC_SPI_START; i < NR_IRQ_LINE; i += 32) {
		DRV_WriteReg32(GICD_BASE + GICD_ICPENDR + i * 4 / 32, 0xFFFFFFFF);
	}
	dsb();

	mt_git_dist_rwp();
	DRV_WriteReg32(GICD_BASE + GICD_CTLR, GICD_CTLR_ENABLE_GRP0 | GICD_CTLR_ENGRP1NS |
			GICD_CTLR_ENGRP1S | GICD_CTLR_ARE | GICD_CTLR_ARE_NS);
	mt_git_dist_rwp();
}

void *platform_init_interrupts(void)
{
#if (configSECURE_WORLD == 1)
	mt_gic_icc_msre_write();
#endif
	mt_gic_dist_init();
	mt_gic_redist_init();
#if (configSECURE_WORLD == 1)
	mt_gic_cpu_init();
#endif
	irq_init();

	return (unsigned long)GICD_BASE;
}

void *gic_gicd_get_address(void)
{
	return (unsigned long)GICD_BASE;
}

