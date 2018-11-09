/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "rec_pub.h"

#define BIT0 1

#define PERIPHS_WDT_BASEADDR        0x60000900

#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr)   (addr)

#define READ_PERI_REG(addr)                             (*((volatile unsigned int *)ETS_UNCACHED_ADDR(addr)))
#define WRITE_PERI_REG(addr, val)                       (*((volatile unsigned int *)ETS_UNCACHED_ADDR(addr))) = (unsigned int)(val)
#define CLEAR_PERI_REG_MASK(reg, mask)                  WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~(mask))))
#define SET_PERI_REG_MASK(reg, mask)                    WRITE_PERI_REG((reg), (READ_PERI_REG(reg) | (mask)))
#define GET_PERI_REG_BITS(reg, hipos, lowpos)           ((READ_PERI_REG(reg) >> (lowpos)) & ((1 << ((hipos) - (lowpos) + 1)) - 1))
#define SET_PERI_REG_BITS(reg, bit_map, value, shift)   (WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~((bit_map) << (shift)))) | ((value) << (shift)) ))

#define WDT_REG_READ(reg)           READ_PERI_REG(PERIPHS_WDT_BASEADDR + reg)
#define WDT_REG_WRITE(reg, val)     WRITE_PERI_REG(PERIPHS_WDT_BASEADDR + reg, val)

#define CLEAR_WDT_REG_MASK(reg, mask)   \
    WDT_REG_WRITE(reg, (WDT_REG_READ(reg)&(~mask)))
#define SET_WDT_REG_MASK(reg, mask)     \
    WDT_REG_WRITE(reg, (WDT_REG_READ(reg)|(mask)))

#define WDT_CTL_ADDRESS             0x0
#define WDT_CTL_RSTLEN_MSB          5
#define WDT_CTL_RSTLEN_LSB          3
#define WDT_CTL_RSTLEN_MASK         0x38
#define WDT_CTL_RSPMOD_MSB          2
#define WDT_CTL_RSPMOD_LSB          1
#define WDT_CTL_RSPMOD_MASK         0x6
#define WDT_CTL_EN_MSB              0
#define WDT_CTL_EN_LSB              0
#define WDT_CTL_EN_MASK             0x1

#define WDT_OP_ADDRESS              0x4
#define WDT_OP_DATA_MSB             3
#define WDT_OP_DATA_LSB             0
#define WDT_OP_DATA_MASK            0xf

#define WDT_RST_ADDRESS             0x14

#define PERIPHS_DPORT_BASEADDR      0x3ff00000
#define EDGE_INT_ENABLE_REG         (PERIPHS_DPORT_BASEADDR + 0x04)

#define WDT_EDGE_INT_ENABLE()        SET_PERI_REG_MASK(EDGE_INT_ENABLE_REG, BIT0)
#define WDT_EDGE_INT_DISABLE()       CLEAR_PERI_REG_MASK(EDGE_INT_ENABLE_REG, BIT0)

extern unsigned char pp_soft_wdt_count;
extern unsigned short init_hw_wdt;
extern void pp_soft_wdt_init();

static unsigned int rec_log2(unsigned int x)
{
    unsigned int exp = 0;

    while(x>>=1) {
        exp++;
    }

    return exp;
}

void rec_wdt_init(unsigned int timeout_ms)
{
	unsigned int timeout_bit = 0;

	CLEAR_WDT_REG_MASK(WDT_CTL_ADDRESS, BIT0);

	WDT_EDGE_INT_ENABLE();

	timeout_bit = rec_log2(timeout_ms*10/8);

	/* (0.8ms x 2^n) timeout for interrupt */
	WDT_REG_WRITE(WDT_OP_ADDRESS, timeout_bit);
	/* (0.8ms x 2^n) timeout for reset after interrupt */
	WDT_REG_WRITE(WDT_OP_ADDRESS, timeout_bit);
	SET_PERI_REG_BITS(PERIPHS_WDT_BASEADDR + WDT_CTL_ADDRESS, WDT_CTL_RSTLEN_MASK, 7<<WDT_CTL_RSTLEN_LSB, 0);
	SET_PERI_REG_BITS(PERIPHS_WDT_BASEADDR + WDT_CTL_ADDRESS, WDT_CTL_RSPMOD_MASK, 0<<WDT_CTL_RSPMOD_LSB, 0);
	SET_PERI_REG_BITS(PERIPHS_WDT_BASEADDR + WDT_CTL_ADDRESS, WDT_CTL_EN_MASK, 1<<WDT_CTL_EN_LSB, 0);

    pp_soft_wdt_init();
}

void rec_wdt_start()
{

}

void rec_wdt_stop()
{
    pp_soft_wdt_count = 0;
    WDT_REG_WRITE(WDT_RST_ADDRESS, 0x73);
    CLEAR_WDT_REG_MASK(WDT_CTL_ADDRESS, BIT0);
    WDT_EDGE_INT_DISABLE();

}

void rec_wdt_feed()
{
	pp_soft_wdt_count = 0;
	WDT_REG_WRITE(WDT_RST_ADDRESS, 0x73);
}
