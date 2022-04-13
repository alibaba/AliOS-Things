/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_PSRAMIP_V1_H__
#define __HAL_PSRAMIP_V1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "reg_psramip_v1.h"

#define PSRIP_KEY (0x55000000)

#define psramip_read8(reg_base,a) \
     (*(volatile unsigned char*)((reg_base)+(a)))

#define psramip_read32(reg_base,a) \
     (*(volatile unsigned int *)((reg_base)+(a)))

#define psramip_write32(v,reg_base,a) \
     ((*(volatile unsigned int *)((reg_base)+(a))) = v)

	
#define	PSRAM_STAT_BUSY  (0x01)
#define	PSRAM_STAT_TXFIFO_EMPTY  	(0x02)
#define	PSRAM_STAT_TXFIFO_FULL 	(0x04)
#define	PSRAM_STAT_RXFIFO_EMPTY 	(0x08)
#define    PSRAM_STAT_RXFIFO_COUNT (0x1f << 4)

/* ip ops */
#if 1
inline static void psramip_w_cmd_addr(uint32_t reg_base, uint32_t cmd, uint32_t addr)
{
    psramip_write32(cmd<<PSRIP_CMD_ADDR_CMD_SHIFT | addr<<PSRIP_CMD_ADDR_ADDR_SHIFT, reg_base, PSRIP_CMD_ADDR_REG_OFFSET);
}
#endif
inline static uint32_t psramip_r_busy(uint32_t reg_base)
{
    return (psramip_read32(reg_base, PSRIP_STAT_REG_OFFSET) & PSRIP_STAT_BUSY_MASK);
}

inline static uint32_t psramip_r_sleep_wakeup_state(uint32_t reg_base)
{
    return (psramip_read32(reg_base, PSRIP_SPWKUPCTRL1_REG_OFFSET) & PSRIP_SPWKUPCTRL1_SLP_WKUP_MASK);
}

inline static uint32_t psramip_r_exit_sleep_onprocess(uint32_t reg_base)
{
    return (psramip_read32(reg_base, PSRIP_SPWKUPCTRL1_REG_OFFSET) & PSRIP_SPWKUPCTRL1_ONPROCESS_MASK);
}

inline static uint32_t psramip_r_calibst(uint32_t reg_base)
{
    return (psramip_read32(reg_base, PSRIP_MODECALIBR_REG_OFFSET) & PSRIP_MODECALIBR_CALIBST_MASK);
}

inline static uint32_t psramip_w_exit_sleep(uint32_t reg_base)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_SPWKUPCTRL1_REG_OFFSET);
    val |= (PSRIP_SPWKUPCTRL1_ONPROCESS_MASK);
    psramip_write32(val, reg_base, PSRIP_SPWKUPCTRL1_REG_OFFSET);
    psramip_r_busy(reg_base);
    return 0;
}

inline static uint32_t psramip_w_wrap_mode_enable(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);
    if (v)
        val |= (PSRIP_SPWKUPCTRL2_WRAP_MODE_ENABLE_MASK);
    else
        val &= ~(PSRIP_SPWKUPCTRL2_WRAP_MODE_ENABLE_MASK);

    val |= (PSRIP_KEY);
    psramip_write32(val, reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);
    return 0;
}

inline static uint32_t psramip_w_1kwrap_mode(uint32_t reg_base)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);

    val |= (PSRIP_SPWKUPCTRL2_1KWRAPTYPE_MODE_MASK);

    val |= (PSRIP_KEY);
    psramip_write32(val, reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);
    return 0;
}
inline static uint32_t psramip_w_32bytewrap_mode(uint32_t reg_base)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);

    val &= ~(PSRIP_SPWKUPCTRL2_32BYTEWRAPTYPE_MODE_MASK);

    val |= (PSRIP_KEY);
    psramip_write32(val, reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);
    return 0;
}

inline static uint32_t psramip_w_dqs_wr_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_MODECALIBR_REG_OFFSET);
    val &= ~(PSRIP_MODECALIBR_DQS_WR_SEL_MASK);
    val |= (PSRIP_KEY | (v<<PSRIP_MODECALIBR_DQS_WR_SEL_SHIFT));
    psramip_write32(val, reg_base, PSRIP_MODECALIBR_REG_OFFSET);
    return 0;
}

inline static uint32_t psramip_w_dqs_rd_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_MODECALIBR_REG_OFFSET);
    val &= ~(PSRIP_MODECALIBR_DQS_RD_SEL_MASK);
    val |= (PSRIP_KEY | (v<<PSRIP_MODECALIBR_DQS_RD_SEL_SHIFT));
    psramip_write32(val, reg_base, PSRIP_MODECALIBR_REG_OFFSET);
    return 0;
}

inline static void psramip_w_enable_and_trigger_calib(uint32_t reg_base)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_MODECALIBR_REG_OFFSET);
    val |= (PSRIP_KEY | PSRIP_MODECALIBR_ENABLE_CALIB_MASK | PSRIP_MODECALIBR_TRIGGER_MASK);
    psramip_write32(val, reg_base, PSRIP_MODECALIBR_REG_OFFSET);
}

#if 0
inline static void psramip_w_enable_calib(uint32_t reg_base)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_MODECALIBR_REG_OFFSET);
    val |= (PSRIP_KEY | PSRIP_MODECALIBR_ENABLE_CALIB_MASK);
    psramip_write32(val, reg_base, PSRIP_MODECALIBR_REG_OFFSET);
}

inline static void psramip_w_trigger_calib(uint32_t reg_base)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_MODECALIBR_REG_OFFSET);
    val |= (PSRIP_KEY | PSRIP_MODECALIBR_TRIGGER_MASK);
    psramip_write32(val, reg_base, PSRIP_MODECALIBR_REG_OFFSET);
}
#endif

#if 0
/* removed */
inline static void psramip_w_phy_enable(uint32_t reg_base, uint32_t v)
{
    if (v)
        psramip_write32(1, reg_base, PSRIP_PHYOCTR_REG_OFFSET);
    else
        psramip_write32(0, reg_base, PSRIP_PHYOCTR_REG_OFFSET);
}

#endif
inline static void psramip_w_tx_fifo(uint32_t reg_base, uint32_t v)
{
    psramip_write32(v, reg_base, PSRIP_TX_DATA_REG_OFFSET);
}

inline static void psramip_w_acc_size(uint32_t reg_base, uint32_t v)
{
    psramip_write32(v<<PSRIP_ACCSIZE_SIZE_SHIFT, reg_base, PSRIP_ACCSIZE_REG_OFFSET);
}

inline static uint32_t psramip_r_rx_fifo(uint32_t reg_base)
{
    return psramip_read32(reg_base, PSRIP_RX_DATA_REG_OFFSET);
}

inline static uint32_t psramip_r_status(uint32_t reg_base)
{
    return psramip_read32(reg_base, PSRIP_STAT_REG_OFFSET);
}

inline static void psramip_clear_fifo(uint32_t reg_base)
{
    psramip_write32(((1<<PSRIP_FIFOCLR_CLR_TX_SHIFT) | (1<<PSRIP_FIFOCLR_CLR_RX_SHIFT)), reg_base, PSRIP_FIFOCLR_REG_OFFSET);
    while((psramip_r_status(reg_base)& PSRAM_STAT_BUSY));
}

inline static void psramip_w_high_speed_enable(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = psramip_read32(reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);
    if (v)
        val |= PSRIP_SPWKUPCTRL2_HS_MODE_MASK;
    else
        val &= ~PSRIP_SPWKUPCTRL2_HS_MODE_MASK;

    psramip_write32(val, reg_base, PSRIP_SPWKUPCTRL2_REG_OFFSET);
}

#ifdef __cplusplus
}
#endif

#endif /* PSRAMIP_HAL_H */
