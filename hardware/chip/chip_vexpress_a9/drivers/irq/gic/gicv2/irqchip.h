/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef AOS_IRQ_CHIP_H
#define AOS_IRQ_CHIP_H

#include <aos/kernel.h>
#include <k_api.h>

//#define IRQ_SET_GROUP1S     1
#define IRQ_SET_GROUP1NS    2    //default
#define IRQ_SET_GROUP0      4

#define CIAR_ID_MASK	    (0x3FFu)
#define CEOIR_ID_MASK	    (0x3FFu)

#ifndef GIC_SPI_START
#define GIC_SPI_START       32
#endif

#ifndef GIC_SPI_NUM
#define GIC_SPI_NUM         225
#endif

#define NR_IRQ_LINE         (GIC_SPI_START * RHINO_CONFIG_CPU_NUM + GIC_SPI_NUM)
#define IRQ_NUM_START       (0)
#define IRQ_NUM_MAX         (GIC_SPI_START + GIC_SPI_NUM)

#ifndef GICD_BASE
#define GICD_BASE           (0x1E001000u)
#endif

#ifndef GICC_BASE
#define GICC_BASE           (0x1E000100u)
#endif

typedef volatile struct __attribute__((packed)) {
    uint32_t DCTLR;                 /* 0x0 Distributor Control register */
    const uint32_t DTYPER;          /* 0x4 Controller type register */
    const uint32_t DIIDR;           /* 0x8 Implementer identification register */
    uint32_t _reserved0[29];        /* 0xC - 0x80; reserved and implementation-defined */
    uint32_t DIGROUPR[32];          /* 0x80 - 0xFC Interrupt group registers */
    uint32_t DISENABLER[32];        /* 0x100 - 0x17C Interrupt set-enable registers */
    uint32_t DICENABLER[32];        /* 0x180 - 0x1FC Interrupt clear-enable registers */
    uint32_t DISPENDR[32];          /* 0x200 - 0x27C Interrupt set-pending registers */
    uint32_t DICPENDR[32];          /* 0x280 - 0x2FC Interrupt clear-pending registers */
    uint32_t DICDABR[32];           /* 0x300 - 0x3FC Active Bit Registers (GIC v1) */
    uint32_t _reserved1[32];        /* 0x380 - 0x3FC reserved on GIC v1 */
    uint32_t DIPRIORITY[255];       /* 0x400 - 0x7F8 Interrupt priority registers */
    uint32_t _reserved2;            /* 0x7FC reserved */
    const uint32_t DITARGETSRO[8];  /* 0x800 - 0x81C Interrupt CPU targets, RO */
    uint32_t DITARGETSR[246];       /* 0x820 - 0xBF8 Interrupt CPU targets */
    uint32_t _reserved3;            /* 0xBFC reserved */
    uint32_t DICFGR[64];            /* 0xC00 - 0xCFC Interrupt config registers */
    /* Some PPI, SPI status registers and identification registers beyond this.
       Don't care about them */
} gicd_irq_registers_t;

typedef volatile struct __attribute__((packed)) {
    uint32_t CCTLR;                 /* 0x0 CPU Interface control register */
    uint32_t CCPMR;                 /* 0x4 Interrupt priority mask register */
    uint32_t CBPR;                  /* 0x8 Binary point register */
    const uint32_t CIAR;            /* 0xC Interrupt acknowledge register */
    uint32_t CEOIR;                 /* 0x10 End of interrupt register */
    const uint32_t CRPR;            /* 0x14 Running priority register */
    const uint32_t CHPPIR;          /* 0x18 Higher priority pending interrupt register */
    uint32_t CABPR;                 /* 0x1C Aliased binary point register */
    const uint32_t CAIAR;           /* 0x20 Aliased interrupt acknowledge register */
    uint32_t CAEOIR;                /* 0x24 Aliased end of interrupt register */
    const uint32_t CAHPPIR;         /* 0x28 Aliased highest priority pending interrupt register */
} gicc_irq_registers_t;

void __irq_enable_svc(int irq);
void __irq_disable_svc(int irq);
int __irq_is_active_svc(int irq);
int __irq_set_priority_svc(int irq, unsigned int priority);
int __irq_set_pending_svc(int irq);
int __irq_clear_pending_svc(int irq);
int __irq_get_active_irq_svc();
int __irq_end_interrupt_svc(int irq, uint32_t  gp_flags);
int __irq_set_type_svc(int irq, uint32_t  edge_flags);
int __irq_set_nmi_svc(int irq);
int __irq_set_affinity_global(int irq, uint32_t  cpumask);
void __send_mask_ipi_svc(int irq, unsigned int cpumask, int flags);
int __register_irq_global(int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long irqflags,
                          const char *devname, void *dev_id);
int __unregister_irq_global(int irq, void *dev_id);
int __unregister_allirq_global(int irq);

unsigned int irq_to_desc_index(int irq);
int desc_index_to_irq(unsigned int index);
void irq_chip_init(irq_data_t *d, int irq);
int aos_irq_is_init(void);
int irq_setup_action(int irq, irq_desc_t *desc, irq_action_t *newaction, int usermode);
irq_action_t *irq_free_action(int irq, irq_desc_t *desc, void *dev_id, int usermode);
void irq_free_all_action(int irq, irq_desc_t *desc, int usermode);
int cpu_in_user_mode(void);
irq_desc_t *irq_to_desc(int irq);

void enable_irq(uint32_t irq);
void disable_irq(uint32_t irq) ;
void irq_set_eoi(uint32_t number, uint32_t flags);
void set_irq_priority(uint32_t irq, uint32_t priority);
void irq_send_sgi(unsigned long long cluster_id, unsigned short tlist, unsigned int irq, unsigned int flags);
int  irq_set_type(uint32_t irq, uint32_t type);
void irq_set_nmi(uint32_t irq);
void irq_set_affinity_global(uint32_t irq, uint32_t mask);
void set_pending_irq(uint32_t irq);
void irq_free_all_action(int irq, irq_desc_t *desc, int usermode);

#endif
