#include <k_api.h>
#include <freertos_to_aos.h>
#include <typedef.h>
#include <sysregs.h>
#include <arch_timer.h>



#define BIT(nr)			(1UL << (nr))
#define CNTTIDR_VIRT(n)	(BIT(1) << ((n) * 4))

#define CNTVCT_LO	0x08
#define CNTVCT_HI	0x0c
#define CNTFRQ		0x10
#define CNTP_TVAL	0x28
#define CNTP_CTL	0x2c
#define CNTV_TVAL	0x38
#define CNTV_CTL	0x3c

#define ARCH_CP15_TIMER	BIT(0)
#define ARCH_MEM_TIMER	BIT(1)

struct arch_timer_device {
	unsigned int access;
	unsigned int irq;
	unsigned long ms_internal;
	irq_handler_t event_handler;
};

struct arch_timer {
	void *base;
	struct arch_timer_device dev;
};


enum ppi_nr {
	PHYS_SECURE_PPI,
	PHYS_NONSECURE_PPI,
	VIRT_PPI,
	HYP_PPI,
	MAX_TIMER_PPI
};

static struct arch_timer arch_timer;
static struct arch_timer_device *arch_timer_dev;
static u32 arch_timer_rate;
static int arch_timer_use_virtual = 0;


//////////////////////////////////////////////////////////////////
static u32 time2tick_ms(u32 time_ms)
{
    return TIME_TO_TICK_MS(time_ms);
}


#ifdef __ARM64__

/*
 * These register accessors are marked inline so the compiler can
 * nicely work out which register we want, and chuck away the rest of
 * the code.
 */
static inline
void arch_timer_reg_write_cp15(int access, enum arch_timer_reg reg, u32 val)
{
	if (access == ARCH_TIMER_PHYS_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			__asm__ volatile("msr cntp_ctl_el0,  %0" : : "r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			__asm__ volatile("msr cntp_tval_el0, %0" : : "r" (val));
			break;
		}
	} else if (access == ARCH_TIMER_VIRT_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			__asm__ volatile("msr cntv_ctl_el0,  %0" : : "r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			__asm__ volatile("msr cntv_tval_el0, %0" : : "r" (val));
			break;
		}
	}

	isb();
}

static inline
u32 arch_timer_reg_read_cp15(int access, enum arch_timer_reg reg)
{
	u32 val = 0;

	if (access == ARCH_TIMER_PHYS_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			__asm__ volatile("mrs %0,  cntp_ctl_el0" : "=r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			__asm__ volatile("mrs %0, cntp_tval_el0" : "=r" (val));
			break;
		}
	} else if (access == ARCH_TIMER_VIRT_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			__asm__ volatile("mrs %0,  cntv_ctl_el0" : "=r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			__asm__ volatile("mrs %0, cntv_tval_el0" : "=r" (val));
			break;
		}
	}

	return val;
}

static inline u32 arch_timer_get_cntfrq(void)
{
	u32 val;
	__asm__ volatile("mrs %0,   cntfrq_el0" : "=r" (val));
	return val;
}

static inline u32 arch_timer_get_cntkctl(void)
{
	u32 cntkctl;
	__asm__ volatile("mrs	%0, cntkctl_el1" : "=r" (cntkctl));
	return cntkctl;
}

static inline void arch_timer_set_cntkctl(u32 cntkctl)
{
	__asm__ volatile("msr	cntkctl_el1, %0" : : "r" (cntkctl));
}

static inline u64 arch_counter_get_cntpct(void)
{
	/*
	 * AArch64 kernel and user space mandate the use of CNTVCT.
	 */
	//BUG();
	while (1);
	return 0;
}

static inline u64 arch_counter_get_cntvct(void)
{
	u64 cval;

	isb();
	__asm__ volatile("mrs %0, cntvct_el0" : "=r" (cval));

	return cval;
}

#else

/*
 * These register accessors are marked inline so the compiler can
 * nicely work out which register we want, and chuck away the rest of
 * the code. At least it does so with a recent GCC (4.6.3).
 */
static inline
void arch_timer_reg_write_cp15(int access, enum arch_timer_reg reg, u32 val)
{
	if (access == ARCH_TIMER_PHYS_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			asm volatile("mcr p15, 0, %0, c14, c2, 1" : : "r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			asm volatile("mcr p15, 0, %0, c14, c2, 0" : : "r" (val));
			break;
		}
	} else if (access == ARCH_TIMER_VIRT_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			asm volatile("mcr p15, 0, %0, c14, c3, 1" : : "r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			asm volatile("mcr p15, 0, %0, c14, c3, 0" : : "r" (val));
			break;
		}
	}

	isb();
}

static inline
u32 arch_timer_reg_read_cp15(int access, enum arch_timer_reg reg)
{
	u32 val = 0;

	if (access == ARCH_TIMER_PHYS_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			asm volatile("mrc p15, 0, %0, c14, c2, 1" : "=r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			asm volatile("mrc p15, 0, %0, c14, c2, 0" : "=r" (val));
			break;
		}
	} else if (access == ARCH_TIMER_VIRT_ACCESS) {
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			asm volatile("mrc p15, 0, %0, c14, c3, 1" : "=r" (val));
			break;
		case ARCH_TIMER_REG_TVAL:
			asm volatile("mrc p15, 0, %0, c14, c3, 0" : "=r" (val));
			break;
		}
	}

	return val;
}

static inline u32 arch_timer_get_cntfrq(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r" (val));
	return val;
}

static inline u64 arch_counter_get_cntpct(void)
{
	u64 cval;

	isb();
	asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (cval));
	return cval;
}

static inline u64 arch_counter_get_cntvct(void)
{
	u64 cval;

	isb();
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (cval));
	return cval;
}

static inline u32 arch_timer_get_cntkctl(void)
{
	u32 cntkctl;
	asm volatile("mrc p15, 0, %0, c14, c1, 0" : "=r" (cntkctl));
	return cntkctl;
}

static inline void arch_timer_set_cntkctl(u32 cntkctl)
{
	asm volatile("mcr p15, 0, %0, c14, c1, 0" : : "r" (cntkctl));
}

#endif


/*
 * Architected system timer support.
 */

static
void arch_timer_reg_write(int access, enum arch_timer_reg reg, u32 val,
			  struct arch_timer_device *clk)
{
	if (access == ARCH_TIMER_MEM_PHYS_ACCESS) {
		struct arch_timer *timer = &arch_timer;
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			writel(val, timer->base + CNTP_CTL);
			break;
		case ARCH_TIMER_REG_TVAL:
			writel(val, timer->base + CNTP_TVAL);
			break;
		}
	} else if (access == ARCH_TIMER_MEM_VIRT_ACCESS) {
		struct arch_timer *timer = &arch_timer;
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			writel(val, timer->base + CNTV_CTL);
			break;
		case ARCH_TIMER_REG_TVAL:
			writel(val, timer->base + CNTV_TVAL);
			break;
		}
	} else {
		arch_timer_reg_write_cp15(access, reg, val);
	}
}

static
u32 arch_timer_reg_read(int access, enum arch_timer_reg reg,
			struct arch_timer_device *clk)
{
	u32 val;

	if (access == ARCH_TIMER_MEM_PHYS_ACCESS) {
		struct arch_timer *timer = &arch_timer;
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			val = readl(timer->base + CNTP_CTL);
			break;
		case ARCH_TIMER_REG_TVAL:
			val = readl(timer->base + CNTP_TVAL);
			break;
		}
	} else if (access == ARCH_TIMER_MEM_VIRT_ACCESS) {
		struct arch_timer *timer = &arch_timer;
		switch (reg) {
		case ARCH_TIMER_REG_CTRL:
			val = readl(timer->base + CNTV_CTL);
			break;
		case ARCH_TIMER_REG_TVAL:
			val = readl(timer->base + CNTV_TVAL);
			break;
		}
	} else {
		val = arch_timer_reg_read_cp15(access, reg);
	}

	return val;
}

static void set_next_event(const int access, unsigned long evt,
					   struct arch_timer_device *clk)
{
	unsigned long ctrl;
	ctrl = arch_timer_reg_read(access, ARCH_TIMER_REG_CTRL, clk);
	ctrl |= ARCH_TIMER_CTRL_ENABLE;
	ctrl &= ~ARCH_TIMER_CTRL_IT_MASK;
	arch_timer_reg_write(access, ARCH_TIMER_REG_TVAL, evt, clk);
	arch_timer_reg_write(access, ARCH_TIMER_REG_CTRL, ctrl, clk);
}

static void timer_handler(const int access,
					struct arch_timer_device *dev)
{
	unsigned long ctrl;
	unsigned long evt;

	ctrl = arch_timer_reg_read(access, ARCH_TIMER_REG_CTRL, dev);
	if (ctrl & ARCH_TIMER_CTRL_IT_STAT) {
		ctrl |= ARCH_TIMER_CTRL_IT_MASK;
		arch_timer_reg_write(access, ARCH_TIMER_REG_CTRL, ctrl, dev);
		dev->event_handler();
	}

	evt = dev->ms_internal;
	set_next_event(access, evt, dev);
}

static void arch_timer_handler(void)
{
	struct arch_timer_device *dev = arch_timer_dev;

	return timer_handler(dev->access, dev);
}


static int timer_shutdown(const int access,
					  struct arch_timer_device *clk)
{
	unsigned long ctrl;

	ctrl = arch_timer_reg_read(access, ARCH_TIMER_REG_CTRL, clk);
	ctrl &= ~ARCH_TIMER_CTRL_ENABLE;
	arch_timer_reg_write(access, ARCH_TIMER_REG_CTRL, ctrl, clk);

	return 0;
}

int arch_timer_shutdown(void)
{
	struct arch_timer_device *dev;

	dev = arch_timer_dev;
	return timer_shutdown(dev->access, dev);
}

static void arch_counter_set_user_access(void)
{
	u32 cntkctl = arch_timer_get_cntkctl();
	int enable = 1;

	/* Disable user access to the timers and the physical counter */
	/* Also disable virtual event stream */
	cntkctl &= ~(ARCH_TIMER_USR_PT_ACCESS_EN
			| ARCH_TIMER_USR_VT_ACCESS_EN
			| ARCH_TIMER_VIRT_EVT_EN
			| ARCH_TIMER_USR_PCT_ACCESS_EN);

	/* Enable user access to the virtual counter */
	if (enable) //(IS_ENABLED(CONFIG_ARM_ARCH_TIMER_VCT_ACCESS))
		cntkctl |= ARCH_TIMER_USR_VCT_ACCESS_EN;
	else
		cntkctl &= ~ARCH_TIMER_USR_VCT_ACCESS_EN;

	arch_timer_set_cntkctl(cntkctl);
}

static void arch_timer_detect_rate(void)
{
	int hard_code_enabled = 1;

	/* Who has more than one independent system counter? */
	if (arch_timer_rate)
		return;

	/*
	 * Try to determine the frequency from the device tree or CNTFRQ,
	 * if ACPI is enabled, get the frequency from CNTFRQ ONLY.
	 */
	if (hard_code_enabled)
		arch_timer_rate = MT_ARCH_TIMER_FREQUENCY; //readl(cntbase + CNTFRQ);
	else
		arch_timer_rate = arch_timer_get_cntfrq();

	/* Check the timer frequency. */
	if (arch_timer_rate == 0)
		printf("Architected timer frequency not available\n");
}

unsigned int arch_timer_get_rate(void)
{
	return arch_timer_rate;
}

static void arch_timer_arch_init(struct arch_timer_device *dev)
{
	unsigned mask = ARCH_CP15_TIMER | ARCH_MEM_TIMER;

	arch_timer_detect_rate();
	arch_counter_set_user_access();

	/* Use physical timer for Free-OS */
	dev->access = ARCH_TIMER_PHYS_ACCESS;
	arch_timer_use_virtual = 0;

	dev->irq = GIC_PPI_TIMER_NS_EL1; /* arch-timer PPI IRQ number */

	printf("[arch-timer] frequency = %u\n", arch_timer_get_rate());
}


int platform_set_periodic_timer(irq_handler_t handler, void *arg,
                                unsigned long msec_interval)
{
	struct arch_timer_device *dev;

	if (msec_interval == 0) {
		printf("[arch-timer] msec_interval = %d\n", msec_interval);
		return -1;
	}

	arch_timer_dev = &arch_timer.dev;
	dev = arch_timer_dev;

	arch_timer_arch_init(dev);

	dev->event_handler = handler;
	/* calculate timeout interval for periodic schedule */
	dev->ms_internal = time2tick_ms(msec_interval);

	//request_irq(dev->irq, arch_timer_handler, IRQ_TYPE_LEVEL_LOW, "arch-timer", NULL);

	set_next_event(dev->access, dev->ms_internal, dev);

	return 0;
}

