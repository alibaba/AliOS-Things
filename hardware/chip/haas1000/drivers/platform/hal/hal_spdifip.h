/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_SPDIFIP_H__
#define __HAL_SPDIFIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "reg_spdifip.h"

#define spdifip_read32(b,a) \
     (*(volatile uint32_t *)(b+a))

#define spdifip_write32(v,b,a) \
     ((*(volatile uint32_t *)(b+a)) = v)

static inline void spdifip_w_enable_spdifip(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;

    val = spdifip_read32(reg_base, SPDIFIP_ENABLE_REG_REG_OFFSET);
    if (v)
        val |= SPDIFIP_ENABLE_REG_SPDIF_ENABLE_MASK;
    else
        val &= ~SPDIFIP_ENABLE_REG_SPDIF_ENABLE_MASK;

    spdifip_write32(val, reg_base, SPDIFIP_ENABLE_REG_REG_OFFSET);
}
#if 0
static inline void spdifip_w_enable_clk_gen(uint32_t reg_base, uint32_t v)
{
    if (v)
        spdifip_write32(1, reg_base, SPDIFIP_CLK_GEN_ENABLE_REG_REG_OFFSET);
    else
        spdifip_write32(0, reg_base, SPDIFIP_CLK_GEN_ENABLE_REG_REG_OFFSET);
}
#endif
#if 0
static inline void spdifip_w_enable_rx_block(uint32_t reg_base, uint32_t v)
{
    if (v)
        spdifip_write32(1, reg_base, SPDIFIP_RX_BLOCK_ENABLE_REG_REG_OFFSET);
    else
        spdifip_write32(0, reg_base, SPDIFIP_RX_BLOCK_ENABLE_REG_REG_OFFSET);
}
#endif
static inline void spdifip_w_sample_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_RX_CFG_REG_OFFSET);

    if (v)
        val |= SPDIFIP_RX_CFG_SAMPLEEN_MASK;
    else
        val &= ~SPDIFIP_RX_CFG_SAMPLEEN_MASK;

    spdifip_write32(val, reg_base, SPDIFIP_RX_CFG_REG_OFFSET);
}
static inline void spdifip_w_enable_rx(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_RX_CFG_REG_OFFSET);

    if (v)
        val |= SPDIFIP_RX_CFG_RXEN_MASK;
    else
        val &= ~SPDIFIP_RX_CFG_RXEN_MASK;

    spdifip_write32(val, reg_base, SPDIFIP_RX_CFG_REG_OFFSET);
}
static inline void spdifip_w_rx_format_cfg_reg(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;

    val = spdifip_read32(reg_base, SPDIFIP_RX_CFG_REG_OFFSET);
    val &= ~SPDIFIP_RX_CFG_FORMAT_MASK;
    val |= v<<SPDIFIP_RX_CFG_FORMAT_SHIFT;

    spdifip_write32(val, reg_base, SPDIFIP_RX_CFG_REG_OFFSET);
}

#if 1
static inline void spdifip_w_enable_rx_channel0(uint32_t reg_base, uint32_t v)
{
    if (v)
        spdifip_write32(1, reg_base, SPDIFIP_RX_ENABLE_REG_OFFSET);
    else
        spdifip_write32(0, reg_base, SPDIFIP_RX_ENABLE_REG_OFFSET);
}
#endif
#if 0
static inline void spdifip_w_enable_tx_block(uint32_t reg_base, uint32_t v)
{
    if (v)
        spdifip_write32(1, reg_base, SPDIFIP_TX_BLOCK_ENABLE_REG_REG_OFFSET);
    else
        spdifip_write32(0, reg_base, SPDIFIP_TX_BLOCK_ENABLE_REG_REG_OFFSET);
}
#endif
static inline void spdifip_w_enable_tx(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_TX_CFG_REG_OFFSET);

    if (v)
        val |= SPDIFIP_TX_CFG_TXEN_MASK;
    else
        val &= ~SPDIFIP_TX_CFG_TXEN_MASK;

    spdifip_write32(val, reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
}
static inline void spdifip_w_tx_valid(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_TX_CFG_REG_OFFSET);

    if (v)
        val |= SPDIFIP_TX_CFG_TXVALID_MASK;
    else
        val &= ~SPDIFIP_TX_CFG_TXVALID_MASK;

    spdifip_write32(val, reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
}
#if 1
static inline void spdifip_w_tx_ratio(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_TX_CFG_REG_OFFSET);

    val &= ~SPDIFIP_TX_CFG_RATIO_MASK;

    val |= v<<SPDIFIP_TX_CFG_RATIO_SHIFT;;

    spdifip_write32(val, reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
}
static inline void spdifip_w_tx_chnst_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_TX_CFG_REG_OFFSET);

    val &= ~SPDIFIP_TX_CFG_CHSTEN_MASK;

    val |= v<<SPDIFIP_TX_CFG_CHSTEN_SHIFT;;

    spdifip_write32(val, reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
}
static inline void spdifip_w_tx_userdata_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_TX_CFG_REG_OFFSET);

    val &= ~SPDIFIP_TX_CFG_USRDATAEN_MASK;

    val |= v<<SPDIFIP_TX_CFG_USRDATAEN_SHIFT;;

    spdifip_write32(val, reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
}
static inline void spdifip_w_tx_format_cfg_reg(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;

    val = spdifip_read32(reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
    val &= ~SPDIFIP_TX_CFG_FORMAT_MASK;
    val |= v<<SPDIFIP_TX_CFG_FORMAT_SHIFT;;

    spdifip_write32(val, reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
}

static inline void spdifip_w_enable_tx_channel0(uint32_t reg_base, uint32_t v)
{
    if (v)
        spdifip_write32(1, reg_base, SPDIFIP_TX_ENABLE_REG_OFFSET);
    else
        spdifip_write32(0, reg_base, SPDIFIP_TX_ENABLE_REG_OFFSET);
}
#endif
#if 0
static inline void spdifip_w_tx_resolution(uint32_t reg_base, uint32_t v)
{
    spdifip_write32(v<<SPDIFIP_TX_CFG_WLEN_SHIFT, reg_base, SPDIFIP_TX_CFG_REG_OFFSET);
}
static inline void spdifip_w_rx_resolution(uint32_t reg_base, uint32_t v)
{
    spdifip_write32(v<<SPDIFIP_RX_CFG_WLEN_SHIFT, reg_base, SPDIFIP_RX_CFG_REG_OFFSET);
}
#endif
static inline void spdifip_w_clk_cfg_reg(uint32_t reg_base, uint32_t v)
{
    spdifip_write32(v, reg_base, SPDIFIP_CLK_CFG_REG_OFFSET);
}
static inline void spdifip_w_tx_left_fifo(uint32_t reg_base, uint32_t v)
{
    spdifip_write32(v, reg_base, SPDIFIP_LEFT_TX_BUFF_REG_OFFSET);
}
static inline void spdifip_w_tx_right_fifo(uint32_t reg_base, uint32_t v)
{
    spdifip_write32(v, reg_base, SPDIFIP_RIGHT_TX_BUFF_REG_OFFSET);
}
static inline void spdifip_w_tx_fifo_threshold(uint32_t reg_base, uint32_t v)
{
    spdifip_write32(v<<SPDIFIP_TX_FIFO_CFG_LEVEL_SHIFT, reg_base, SPDIFIP_TX_FIFO_CFG_REG_OFFSET);
}
static inline void spdifip_w_rx_fifo_threshold(uint32_t reg_base, uint32_t v)
{
    spdifip_write32(v<<SPDIFIP_RX_FIFO_CFG_LEVEL_SHIFT, reg_base, SPDIFIP_RX_FIFO_CFG_REG_OFFSET);
}
static inline void spdifip_w_tx_fifo_reset(uint32_t reg_base)
{
    spdifip_write32(SPDIFIP_TX_FIFO_FLUSH_MASK, reg_base, SPDIFIP_TX_FIFO_FLUSH_REG_OFFSET);
}
static inline void spdifip_w_rx_fifo_reset(uint32_t reg_base)
{
    spdifip_write32(SPDIFIP_RX_FIFO_FLUSH_MASK, reg_base, SPDIFIP_RX_FIFO_FLUSH_REG_OFFSET);
}
static inline uint32_t spdifip_r_int_status(uint32_t reg_base)
{
    return spdifip_read32(reg_base, SPDIFIP_INT_STATUS_REG_OFFSET);
}
static inline void spdifip_w_enable_tx_dma(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_DMA_CTRL_REG_OFFSET);
    if (v)
        val |= SPDIFIP_DMA_CTRL_TX_ENABLE_MASK;
    else
        val &= ~SPDIFIP_DMA_CTRL_TX_ENABLE_MASK;

    spdifip_write32(val, reg_base, SPDIFIP_DMA_CTRL_REG_OFFSET);
}
static inline void spdifip_w_enable_rx_dma(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = spdifip_read32(reg_base, SPDIFIP_DMA_CTRL_REG_OFFSET);
    if (v)
        val |= SPDIFIP_DMA_CTRL_RX_ENABLE_MASK;
    else
        val &= ~SPDIFIP_DMA_CTRL_RX_ENABLE_MASK;

    spdifip_write32(val, reg_base, SPDIFIP_DMA_CTRL_REG_OFFSET);
}

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SPDIFIP_H__ */
