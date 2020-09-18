/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_BTPCMIP_H__
#define __HAL_BTPCMIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "reg_btpcmip.h"

#define btpcmip_read32(b,a) \
     (*(volatile uint32_t *)(b+a))

#define btpcmip_write32(v,b,a) \
     ((*(volatile uint32_t *)(b+a)) = v)

static inline void btpcmip_w_enable_btpcmip(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;

    val = btpcmip_read32(reg_base, BTPCMIP_ENABLE_REG_REG_OFFSET);
    if (v)
        val |= BTPCMIP_ENABLE_REG_BTPCM_ENABLE_MASK;
    else
        val &= ~BTPCMIP_ENABLE_REG_BTPCM_ENABLE_MASK;

    btpcmip_write32(val, reg_base, BTPCMIP_ENABLE_REG_REG_OFFSET);
}

static inline void btpcmip_flush_rx_fifo(uint32_t reg_base)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_RX_FIFO_FLUSH_REG_OFFSET);

    val |= BTPCMIP_RX_FIFO_FLUSH_MASK;

    val &= ~BTPCMIP_RX_FIFO_FLUSH_MASK;

    btpcmip_write32(val, reg_base, BTPCMIP_RX_FIFO_FLUSH_REG_OFFSET);
}

static inline void btpcmip_flush_tx_fifo(uint32_t reg_base)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_TX_FIFO_FLUSH_REG_OFFSET);

    val |= BTPCMIP_TX_FIFO_FLUSH_MASK;

    val &= ~BTPCMIP_TX_FIFO_FLUSH_MASK;

    btpcmip_write32(val, reg_base, BTPCMIP_TX_FIFO_FLUSH_REG_OFFSET);
}

static inline void btpcmip_w_tx_fifo(uint32_t reg_base, uint32_t v)
{
    btpcmip_write32(v, reg_base, BTPCMIP_TX_BUFF_REG_OFFSET);
}

////config btpcm parameter
static inline void btpcmip_w_shortsync(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_CR_REG_OFFSET);
    if (v)
        val |= BTPCMIP_CR_SYNCSHORT_MASK;
    else
        val &= ~BTPCMIP_CR_SYNCSHORT_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_CR_REG_OFFSET);
}

static inline void btpcmip_w_length(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_CR_REG_OFFSET);
    val &= ~BTPCMIP_CR_LENTH_MASK;
    val |= (BTPCMIP_CR_LENTH_MASK & (v << BTPCMIP_CR_LENTH_SHIFT));
    btpcmip_write32(val, reg_base, BTPCMIP_CR_REG_OFFSET);
}

static inline void btpcmip_w_slot_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_CR_REG_OFFSET);
    val &= ~BTPCMIP_CR_SLOTSEL_MASK;
    val |= v;
    btpcmip_write32(val, reg_base, BTPCMIP_CR_REG_OFFSET);
}

static inline void btpcmip_pcm_clk_open_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_CR_REG_OFFSET);
    val &= ~BTPCMIP_CR_PCM_CLK_OPEN_EN_MASK;
    val |= v<<BTPCMIP_CR_PCM_CLK_OPEN_EN_SHIFT;
    btpcmip_write32(val, reg_base, BTPCMIP_CR_REG_OFFSET);
}

static inline void btpcmip_w_mask1mask2(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_CR_REG_OFFSET);
    val &= ~BTPCMIP_CR_MASK1_MASK;
    val &= ~BTPCMIP_CR_MASK2_MASK;
    val |= BTPCMIP_CR_MASK1_MASK;
    val |= BTPCMIP_CR_MASK2_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_CR_REG_OFFSET);
}

////config rx parameter
static inline void btpcmip_w_signextin(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_RCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_RCR0_SIGNEXTIN_MASK;
    else
        val &= ~BTPCMIP_RCR0_SIGNEXTIN_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_RCR0_REG_OFFSET);
}

static inline void btpcmip_w_msbin(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_RCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_RCR0_MSBIN_MASK;
    else
        val &= ~BTPCMIP_RCR0_MSBIN_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_RCR0_REG_OFFSET);
}

static inline void btpcmip_w_signin(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_RCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_RCR0_SIGNIN_MASK;
    else
        val &= ~BTPCMIP_RCR0_SIGNIN_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_RCR0_REG_OFFSET);
}

static inline void btpcmip_w_2sin(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_RCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_RCR0_2SIN_MASK;
    else
        val &= ~BTPCMIP_RCR0_2SIN_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_RCR0_REG_OFFSET);
}

static inline void btpcmip_w_1sin(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_RCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_RCR0_1SIN_MASK;
    else
        val &= ~BTPCMIP_RCR0_1SIN_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_RCR0_REG_OFFSET);
}

////config tx parameter
static inline void btpcmip_w_signexto(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_TCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_TCR0_SIGNEXTO_MASK;
    else
        val &= ~BTPCMIP_TCR0_SIGNEXTO_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_TCR0_REG_OFFSET);
}

static inline void btpcmip_w_msbo(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_TCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_TCR0_MSBO_MASK;
    else
        val &= ~BTPCMIP_TCR0_MSBO_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_TCR0_REG_OFFSET);
}

static inline void btpcmip_w_signo(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_TCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_TCR0_SIGNO_MASK;
    else
        val &= ~BTPCMIP_TCR0_SIGNO_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_TCR0_REG_OFFSET);
}

static inline void btpcmip_w_2so(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_TCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_TCR0_2SO_MASK;
    else
        val &= ~BTPCMIP_TCR0_2SO_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_TCR0_REG_OFFSET);
}

static inline void btpcmip_w_1so(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_TCR0_REG_OFFSET);
    if (v)
        val |= BTPCMIP_TCR0_1SO_MASK;
    else
        val &= ~BTPCMIP_TCR0_1SO_MASK;
    btpcmip_write32(val, reg_base, BTPCMIP_TCR0_REG_OFFSET);
}

////
static inline uint32_t btpcmip_r_int_status(uint32_t reg_base)
{
    return btpcmip_read32(reg_base, BTPCMIP_INT_STATUS_REG_OFFSET);
}

////
static inline void btpcmip_w_tx_fifo_threshold(uint32_t reg_base, uint32_t v)
{
    btpcmip_write32(v<<BTPCMIP_TX_FIFO_CFG_LEVEL_SHIFT, reg_base, BTPCMIP_TX_FIFO_CFG_REG_OFFSET);
}
static inline void btpcmip_w_rx_fifo_threshold(uint32_t reg_base, uint32_t v)
{
    btpcmip_write32(v<<BTPCMIP_RX_FIFO_CFG_LEVEL_SHIFT, reg_base, BTPCMIP_RX_FIFO_CFG_REG_OFFSET);
}

static inline void btpcmip_w_enable_tx_dma(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_DMA_CTRL_REG_OFFSET);
    if (v)
        val |= BTPCMIP_DMA_CTRL_TX_ENABLE_MASK;
    else
        val &= ~BTPCMIP_DMA_CTRL_TX_ENABLE_MASK;

    btpcmip_write32(val, reg_base, BTPCMIP_DMA_CTRL_REG_OFFSET);
}
static inline void btpcmip_w_enable_rx_dma(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = btpcmip_read32(reg_base, BTPCMIP_DMA_CTRL_REG_OFFSET);
    if (v)
        val |= BTPCMIP_DMA_CTRL_RX_ENABLE_MASK;
    else
        val &= ~BTPCMIP_DMA_CTRL_RX_ENABLE_MASK;

    btpcmip_write32(val, reg_base, BTPCMIP_DMA_CTRL_REG_OFFSET);
}

#ifdef __cplusplus
}
#endif

#endif /* __HAL_BTPCMIP_H__ */
