/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <aos/list.h>
#include <board.h>
#include <irq/irqdesc.h>
#include <irqchip.h>
#if 0
#include "callback.h"
#endif

#define GIC_SOFT_IRQ_GEN        (GICD_BASE + 0xf00) // 0xf00
#define DCTRL_ENABLE            (1u)
#define CCTRL_ENABLE            (1u)

#ifndef DRV_REG_WRITE32
#define DRV_REG_WRITE32(_reg, _val) (*(volatile uint32_t*)&_reg = _val)
#endif
/*
#ifndef DRV_REG_READ32
#define DRV_REG_READ32(_reg)             (*(volatile unsigned int *)(&_reg))
#endif
*/

#ifndef DRV_WriteReg32
#define DRV_WriteReg32(addr,data)   ((*(volatile unsigned int *)(addr)) = (unsigned int)(data))
#endif
#ifndef DRV_Reg32
#define DRV_Reg32(addr)             (*(volatile unsigned int *)(addr))
#endif

static gicd_irq_registers_t* g_gicd_regs = (gicd_irq_registers_t*)GICD_BASE;
static gicc_irq_registers_t* g_gicc_regs = (gicc_irq_registers_t*)GICC_BASE;

static void gic_dist_init(void)
{
    g_gicd_regs = (gicd_irq_registers_t *)GICD_BASE;
    DRV_REG_WRITE32(g_gicd_regs->DCTLR, DCTRL_ENABLE); /* Enable the interrupt distributor */
}

static void gic_interfce_init(void)
{
    g_gicc_regs = (gicc_irq_registers_t *)GICC_BASE;
    DRV_REG_WRITE32(g_gicc_regs->CCPMR, 0xFFFFu); /* Enable all interrupt priorities */
    DRV_REG_WRITE32(g_gicc_regs->CCTLR, CCTRL_ENABLE); /* Enable interrupt forwarding to this CPU */
}

void enable_irq(uint32_t number)
{
    /* Enable the interrupt */
    uint8_t reg = number / 32;
    uint8_t bit = number % 32;

    uint32_t reg_val = g_gicd_regs->DISENABLER[reg];
    reg_val |= (1u << bit);
    DRV_REG_WRITE32(g_gicd_regs->DISENABLER[reg], reg_val);

    /* Forward interrupt to CPU Interface 0 */
    reg = number / 4;
    bit = (number % 4) * 8; /* Can result in bit 0, 8, 16 or 24 */
    reg_val = g_gicd_regs->DITARGETSR[reg];
    reg_val |= (1u << bit);
    DRV_REG_WRITE32(g_gicd_regs->DITARGETSR[reg], reg_val);
}

void disable_irq(uint32_t number)
{
    /* Enable the interrupt */
    uint32_t reg_val = 0;
    uint8_t reg = number / 32;
    uint8_t bit = number % 32;

    reg_val |= (1u << bit);
    DRV_REG_WRITE32(g_gicd_regs->DICENABLER[reg], reg_val);
}

static uint32_t gic_acknowledge_interrupt(void) {
    return g_gicc_regs->CIAR & CIAR_ID_MASK;
}

static void gic_end_interrupt(uint32_t number) {
    DRV_REG_WRITE32(g_gicc_regs->CEOIR, (number & CEOIR_ID_MASK));
}

void irq_set_eoi(uint32_t number, uint32_t flags)
{
    (void)flags;
    gic_end_interrupt(number);
}

void set_pending_irq(uint32_t number) {
    uint32_t reg_val = 0;
    uint8_t reg = number / 32;
    uint8_t bit = number % 32;

    reg_val |= (1u << bit);
    DRV_REG_WRITE32(g_gicd_regs->DISPENDR[reg], reg_val);
}

void set_irq_priority(uint32_t irq, uint32_t priority)
{
    uint32_t reg_val;
    uint32_t reg = irq / 4;
    uint32_t shift = 8 * (irq % 4);
    uint32_t mask = 0xff << shift;

    reg_val = g_gicd_regs->DIPRIORITY[reg];
    reg_val = (reg_val & ~mask) | (priority << shift);
    DRV_REG_WRITE32(g_gicd_regs->DIPRIORITY[reg], reg_val);
}

void gic_send_sgi(unsigned int irqID, unsigned int target_list, unsigned int filter_list)
{
    uint32_t value;

    value = (filter_list << 24)
          | (target_list << 16)
          | (1 << 15)
          | (irqID & 0xf);

    __asm volatile ("dsb sy":::"memory");

    DRV_WriteReg32(GIC_SOFT_IRQ_GEN, value);
}

void irq_send_sgi(unsigned long long cluster_id, unsigned short tlist, unsigned int irq, unsigned int flags)
{
    (void)flags;
    gic_send_sgi(irq, tlist, cluster_id);
    return;
}

int irq_set_type(uint32_t irq, uint32_t type)
{
    (void)irq;
    (void)type;
    return 0;
}

void irq_set_nmi(uint32_t irq_no)
{
    (void)irq_no;
    return;
}

static void set_affinity_global(uint32_t irq, uint32_t mask)
{
    uint32_t reg = (irq & ~3);
    uint32_t shift = (irq & 3) << 3;
    uint32_t reg_val = g_gicd_regs->DITARGETSR[reg];
    reg_val &= ~(0xff << shift);
    reg_val |= mask << shift;

    DRV_REG_WRITE32(g_gicd_regs->DITARGETSR[reg], reg_val);
}

void irq_set_affinity_global(uint32_t irq, uint32_t mask)
{
    set_affinity_global(irq, mask);
}

void irq_set_vector(int irq, void* addr)
{
    return;
}

int irq_set_vector_before_init(int irq, void* handler)
{
    /* gic init next to aos_init before board_init */
    return false;
}

unsigned int irq_to_desc_index(int irq)
{
    uint8_t cpu = cpu_cur_get();

    if(irq < GIC_SPI_START && cpu){
        return irq + IRQ_NUM_MAX + (cpu - 1)* GIC_SPI_START;
    }else{
        return irq;
    }
}

int desc_index_to_irq(unsigned int index)
{
    unsigned int irq;
    if(index >= IRQ_NUM_MAX){
        irq = (index - IRQ_NUM_MAX) % GIC_SPI_START;
    }else{
        irq = index;
    }
    return irq;
}

int irq_get_cur_cpsr(void)
{
    int   CPSR = 0;

#if defined(__CC_ARM)
    register int temp __asm("cpsr");
    CPSR = temp;
#elif defined(__ICCARM__)
    asm volatile("mrs %0, CPSR\n" : "=r"(CPSR));
#elif defined(__GNUC__)
    __asm__ volatile("mrs %0, CPSR\n" : "=r"(CPSR));
#endif

    return CPSR;
}

int cpu_in_user_mode(void)
{
#if 0
    int   CPSR = irq_get_cur_cpsr();
    CPSR &= CPSR_PROCESS_MODE_MASK;

    if(CPSR == CPSR_Mode_USR){
        return true;
    }
#endif
    return false;
}

void os_fiq_handle(void)
{
/*
    int irq;
    irq_desc_t *irqdesc;
    uint32_t iar = gic_icc_iar0_read();
    unsigned int vector = iar & 0x3ff;
    if(vector >= NR_IRQ_LINE){
        return;
    }
    irq = desc_index_to_irq(vector);
    //krhino_intrpt_enter();
    irqdesc = irq_to_desc(irq);
    if(irqdesc->handle_irq){
        irqdesc->handle_irq(irqdesc);
        gic_write_eoi0r(vector);
    }else{
        printk("[Error]os_irq_handle unregister irq:%d\r\n",irq);
        __irq_disable_svc(irq);
    }
    //krhino_intrpt_exit();
    */
}

void os_irq_handle(void)
{
    int irq;
    irq_desc_t *irqdesc;
    uint32_t iar = gic_acknowledge_interrupt();
    unsigned int vector = iar & 0x3ff;
    if(vector >= NR_IRQ_LINE){
        return;
    }
    irq = vector;
    //krhino_intrpt_enter();
    irqdesc = irq_to_desc(irq);
    if(irqdesc->handle_irq){
        irqdesc->handle_irq(irqdesc);
        gic_end_interrupt(vector);
    }else{
        // printk("[Error]os_irq_handle unregister irq:%d\r\n",irq);
        __irq_disable_svc(irq);
    }
    //krhino_intrpt_exit();
}

static inline uint32_t gic_irq(irq_data_t *d)
{
    return d->irq;
}

static void gic_mask_irq(irq_data_t *d)
{
    uint32_t irq;
    if(!d){
        return;
    }
    irq = gic_irq(d);
    __irq_disable_svc(irq);
    return;
}

static void gic_unmask_irq(irq_data_t *d)
{
    uint32_t irq;
    if(!d){
        return;
    }
    irq = gic_irq(d);
    __irq_enable_svc(irq);
    return;
}

static void gic_eoi_irq(irq_data_t *d)
{
    uint32_t irq;
    if(!d){
        return;
    }

    irq = gic_irq(d);
#if 0
    if(cpu_in_user_mode()){
        PRINTF_W("irq:%d,gic_eoi_irq not support in usermode!\r\n", irq);
        return;
    }
#endif
    __irq_end_interrupt_svc(irq, d->flags);
    //gic_end_interrupt(irq);

    return;
}

/*
static void ipi_send_single(irq_data_t *data, unsigned int cpu)
{
    uint32_t irq;
    if(!d){
        return;
    }
    irq = gic_irq(d);
    if(IRQ_SET_GROUP0 & d->flags){
        gic_send_sgi0(0, 1ul << cpu, irq);
    }else{
        gic_send_sgi(0, 1ul << cpu, irq);
    }

    return;
}*/

void irq_set_user_active(irq_data_t *d)
{
    return;
}

void irq_clear_user_active(irq_data_t *d)
{
    return;
}

static void ipi_send_mask(irq_data_t *data, unsigned int cpumask)
{
    __send_mask_ipi_svc(data->irq, cpumask, data->flags);

    return;
}

static int gic_set_type(irq_data_t *data, unsigned int flow_type)
{
    uint32_t irq;
    if(!data){
        return -1;
    }
    irq = gic_irq(data);
    return __irq_set_type_svc(irq, flow_type);
    //return irq_set_type(irq, flow_type);
}

static void gic_set_nmi(irq_data_t *data)
{
    uint32_t irq;
    if(!data){
        return ;
    }

#if 0
    if(cpu_in_user_mode()){
        PRINTF_W("irq:%d,gic_set_nmi not support in usermode!\r\n", gic_irq(data));
        return;
    }
#endif

    data->flags |= IRQ_SET_GROUP0;
    irq = gic_irq(data);
    __irq_set_nmi_svc(irq);
    //irq_set_nmi(irq);
    return;
}

static int gic_set_affinity(irq_data_t *data, unsigned int cpumask, bool force)
{
    uint32_t irq;
    if(!data){
        return -1;
    }
    (void)force;
    irq = gic_irq(data);

    __irq_set_affinity_global(irq, cpumask);
    //set_affinity_global(irq, cpumask);
    return 0;
}

static int gic_set_pending(irq_data_t *data)
{
    uint32_t irq;
    if(!data){
        return -1;
    }
    irq = gic_irq(data);
    return __irq_set_pending_svc(irq);
}

static irq_chip_t gic_chip = {
    .name           = "GICv2",
    .irq_enable     = gic_unmask_irq,
    .irq_disable    = gic_mask_irq,
    .irq_eoi        = gic_eoi_irq,
    //.ipi_send_single = ipi_send_single,
    .ipi_send_mask  = ipi_send_mask,
    .irq_set_type   = gic_set_type,
    .irq_set_nmi    = gic_set_nmi,
    .irq_set_affinity   = gic_set_affinity,
    .irq_set_pending = gic_set_pending,
};

void irq_chip_init(irq_data_t *d, int irq)
{
    d->chip = &gic_chip;
    d->irq = irq;
    d->flags = IRQ_SET_GROUP1NS;
}

extern void irq_desc_init(void);
extern void irq_desc_uninit(void);
#ifdef AOS_COMPILE_APP
/* PL0 cannot read CPUID ,and can be switch between cores in userspace*/
static int g_os_int_init_flag = 0;
#define G_SYS_INT_INIT_FLAG    g_os_int_init_flag
#else
static int g_os_int_init_flag[RHINO_CONFIG_CPU_NUM] = {0};
#define G_SYS_INT_INIT_FLAG    g_os_int_init_flag[cpu_cur_get()]
#endif

#define SYS_OS_INT_INIT_MAGIC   0X11AA33CC

int aos_irq_system_init(void)
{
    if(G_SYS_INT_INIT_FLAG == SYS_OS_INT_INIT_MAGIC){
       return 0;
    }

#if 0
    if(cpu_in_user_mode()){
        irq_desc_init();
        G_SYS_INT_INIT_FLAG = SYS_OS_INT_INIT_MAGIC;
        return 0;
    }
#endif

    if(0 == cpu_cur_get()){
        gic_dist_init();
        irq_desc_init();
    }

    gic_interfce_init();
    G_SYS_INT_INIT_FLAG = SYS_OS_INT_INIT_MAGIC;
    return 0;
}

int aos_irq_is_init(void)
{
    if(G_SYS_INT_INIT_FLAG == SYS_OS_INT_INIT_MAGIC){
        return 1;
    }
    return 0;
}

int aos_irq_system_deinit(void)
{
    G_SYS_INT_INIT_FLAG = 0;
    irq_desc_uninit();
    return 0;
}
