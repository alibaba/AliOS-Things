/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_types.h"

#define SBY_CRPM_BASEADDR (0x20140000)
#define AP_CRPM_BASEADDR (0x22005000)
#define AP_WDT_BASEADDR (0x22004000)
#define SRC_CLK_19M2 (19200000)
#define SRC_CLK_32K (32768)

#define SBY_CRM_BASE_ADDR (SBY_CRPM_BASEADDR)
#define CK_WDT_BASE (AP_WDT_BASEADDR)
#define CK_CRM_BASE_ADDR (AP_CRPM_BASEADDR)

#define WDT_RESET_TYPE (SBY_CRM_BASE_ADDR + 0x4)

#define WDT_CRM_REG (CK_CRM_BASE_ADDR + 0x70)
#define WDT_RESET_REG (CK_CRM_BASE_ADDR + 0x74)
#define WDT_CRM_INT_CLEAR (CK_CRM_BASE_ADDR + 0x88)


#define BIT(n) (1UL << (n))
#define BIT_MASK(n) (BIT(n) - 1)

#define WDT_TIMEOUT 419

typedef struct
{
    uint32_t version;    /**< offset:0x0,      R/W  */
    uint32_t config;     /**< offset:0x4,      R/W  */
    uint32_t value_load; /**< offset:0x8,      R/W  */
    uint32_t count;      /**< offset:0xc,      R/W  */
    uint32_t status;     /**< offset:0x10,     R/W  */
    uint32_t value_int;  /**< offset:0x14,     R/W  */
    uint32_t set_en;     /**< offset:0x18,     R/W  */
    uint32_t start;      /**< offset:0x1c,     R/W  */
} zx29_wdt_reg_t;

static inline uint32_t getreg32(volatile uint32_t *addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void putreg32(uint32_t val, volatile uint32_t *addr)
{
    *(volatile uint32_t *)addr = val;
}

static inline void __write_bits(int width, int nr, volatile uint32_t *addr,
                                int val)
{
    uint32_t temp = *addr;

    temp &= ~(BIT_MASK(width) << nr);
    temp |= (val << nr);

    *addr = temp;
}

void watchdog_start(void)
{
    volatile zx29_wdt_reg_t *wdt_reg = (zx29_wdt_reg_t *)AP_WDT_BASEADDR;
    // reset
    putreg32(0x0, (uint32_t *)WDT_RESET_REG);
    putreg32(0x3, (uint32_t *)WDT_RESET_REG);
    __write_bits(2, 4, (uint32_t *)WDT_RESET_TYPE, 0x3);
    // clk gen
    putreg32(0x7, (uint32_t *)WDT_CRM_REG);

    wdt_reg->config = (0x1234 << 16) | (31 << 8);

    wdt_reg->value_int = (0x1234 << 16) | (0 & 0xffff);

    wdt_reg->value_load = (0x1234 << 16) | (WDT_TIMEOUT & 0xffff);

    // refresh
    if (wdt_reg->set_en == 0x0015) {
        wdt_reg->set_en = 0x1234002A;
    } else {
        wdt_reg->set_en = 0x12340015;
    }

    while ((wdt_reg->status & 0x2) == 0x0)
        ;
    // start
    wdt_reg->start |= 0x12340001;
}
