#ifndef GIC_H
#define GIC_H

#include <stdint.h>
#include "cpu.h"

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
} gic_distributor_registers;

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
} gic_cpu_interface_registers;

void gic_init(void);
void gic_enable_interrupt(uint16_t number);
void gic_disable_interrupt(uint16_t number);
uint16_t gic_acknowledge_interrupt();
void gic_end_interrupt(uint16_t number);

#define GIC_DIST_BASE   (PERIPHEAL_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET)
#define GIC_IFACE_BASE  (PERIPHEAL_BASE_ADDR + GIC_IFACE_OFFSET)

#define DCTRL_ENABLE    (1u)
#define CCTRL_ENABLE    (1u)

#define CIAR_ID_MASK	(0x3FFu)
#define CEOIR_ID_MASK	(0x3FFu)

#endif
