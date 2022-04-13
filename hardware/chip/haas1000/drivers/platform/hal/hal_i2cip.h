/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_I2CIP_H__
#define __HAL_I2CIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "reg_i2cip.h"

#define i2cip_read32(b,a) \
     (*(volatile uint32_t *)(b+a))

#define i2cip_write32(v,b,a) \
     ((*(volatile uint32_t *)(b+a)) = v)

static inline uint8_t i2cip_w_enable(uint32_t reg_base, uint8_t enable)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_ENABLE_REG_OFFSET);
    if (enable)
        val |= I2CIP_ENABLE_MASK;
    else
        val &= ~I2CIP_ENABLE_MASK;
    i2cip_write32(val, reg_base, I2CIP_ENABLE_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_clear_ctrl(uint32_t reg_base)
{
    i2cip_write32(0, reg_base, I2CIP_CTRL_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_10bit_master(uint32_t reg_base, uint8_t enable)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_CTRL_REG_OFFSET);
    if (enable)
        val |= I2CIP_10BITADDR_MASTER_MASK;
    else
        val &= ~I2CIP_10BITADDR_MASTER_MASK;
    i2cip_write32(val, reg_base, I2CIP_CTRL_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_r_ctrl_reg(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CTRL_REG_OFFSET);
}
static inline uint8_t i2cip_w_10bit_slave(uint32_t reg_base, uint8_t enable)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_CTRL_REG_OFFSET);
    if (enable)
        val |= I2CIP_10BITADDR_SLAVE_MASK;
    else
        val &= ~I2CIP_10BITADDR_SLAVE_MASK;
    i2cip_write32(val, reg_base, I2CIP_CTRL_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_restart(uint32_t reg_base, uint8_t restart)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_CTRL_REG_OFFSET);
    if (restart)
        val |= I2CIP_RESTART_ENABLE_MASK;
    else
        val &= ~I2CIP_RESTART_ENABLE_MASK;
    i2cip_write32(val, reg_base, I2CIP_CTRL_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_speed(uint32_t reg_base, uint32_t speed)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_CTRL_REG_OFFSET);
    val &= ~I2CIP_SPEED_MASK;
    val |= speed;
    i2cip_write32(val, reg_base, I2CIP_CTRL_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_standard_speed_hcnt(uint32_t reg_base, uint32_t hcnt)
{
    uint32_t val = 0;
    val |= hcnt<<I2CIP_SS_SCL_HCNT_SHIFT;
    i2cip_write32(val, reg_base, I2CIP_SS_SCL_HCNT_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_fast_speed_hcnt(uint32_t reg_base, uint32_t hcnt)
{
    uint32_t val = 0;
    val |= hcnt<<I2CIP_FS_SCL_HCNT_SHIFT;
    i2cip_write32(val, reg_base, I2CIP_FS_SCL_HCNT_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_high_speed_hcnt(uint32_t reg_base, uint32_t hcnt)
{
    uint32_t val = 0;
    val |= hcnt<<I2CIP_HS_SCL_HCNT_SHIFT;
    i2cip_write32(val, reg_base, I2CIP_HS_SCL_HCNT_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_standard_speed_lcnt(uint32_t reg_base, uint32_t lcnt)
{
    uint32_t val = 0;
    val |= lcnt<<I2CIP_SS_SCL_LCNT_SHIFT;
    i2cip_write32(val, reg_base, I2CIP_SS_SCL_LCNT_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_fast_speed_lcnt(uint32_t reg_base, uint32_t lcnt)
{
    uint32_t val = 0;
    val |= lcnt<<I2CIP_FS_SCL_LCNT_SHIFT;
    i2cip_write32(val, reg_base, I2CIP_FS_SCL_LCNT_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_high_speed_lcnt(uint32_t reg_base, uint32_t lcnt)
{
    uint32_t val = 0;
    val |= lcnt<<I2CIP_HS_SCL_LCNT_SHIFT;
    i2cip_write32(val, reg_base, I2CIP_HS_SCL_LCNT_REG_OFFSET);
    return 0;
}
static inline uint32_t i2cip_r_target_address_reg(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
}
static inline void i2cip_w_target_address_reg(uint32_t reg_base, uint32_t val)
{
    i2cip_write32(val, reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
}
static inline uint8_t i2cip_w_target_address(uint32_t reg_base, uint32_t addr)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
    val &= ~I2CIP_TARGET_ADDRESS_MASK;
    val |= (addr<<I2CIP_TARGET_ADDRESS_SHIFT) & I2CIP_TARGET_ADDRESS_MASK;
    if (addr & HAL_I2C_10BITADDR_MASK) {
        val |= I2CIP_TARGET_ADDRESS_IC_10BITADDR_MASTER_MASK;
    } else {
        val &= ~I2CIP_TARGET_ADDRESS_IC_10BITADDR_MASTER_MASK;
    }
    i2cip_write32(val, reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_gc_or_start_bit(uint32_t reg_base, uint8_t set)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
    if (set)
        val |= I2CIP_TARGET_ADDRESS_GC_OR_START_MASK;
    else
        val &= ~I2CIP_TARGET_ADDRESS_GC_OR_START_MASK;
    i2cip_write32(val, reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_special_bit(uint32_t reg_base, uint8_t set)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
    if (set)
        val |= I2CIP_TARGET_ADDRESS_SPECIAL_BIT_MASK;
    else
        val &= ~I2CIP_TARGET_ADDRESS_SPECIAL_BIT_MASK;
    i2cip_write32(val, reg_base, I2CIP_TARGET_ADDRESS_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_address_as_slave(uint32_t reg_base, uint32_t addr)
{
    uint32_t val = 0;
    val |= addr<<I2CIP_ADDRESS_AS_SLAVE_SHIFT;
    i2cip_write32(val, reg_base, I2CIP_ADDRESS_AS_SLAVE_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_init_int_mask(uint32_t reg_base, uint32_t mask)
{
    uint32_t val = 0;
    val = mask;
    i2cip_write32(val, reg_base, I2CIP_INT_MASK_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_set_int_mask(uint32_t reg_base, uint32_t mask)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_INT_MASK_REG_OFFSET);
    val |= mask;
    i2cip_write32(val, reg_base, I2CIP_INT_MASK_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_clear_int_mask(uint32_t reg_base, uint32_t mask)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_INT_MASK_REG_OFFSET);
    val &= ~mask;
    i2cip_write32(val, reg_base, I2CIP_INT_MASK_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_disable_slave(uint32_t reg_base, uint8_t disable)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_CTRL_REG_OFFSET);
    if (disable)
        val |= I2CIP_SLAVE_DISABLE_MASK;
    else
        val &= ~I2CIP_SLAVE_DISABLE_MASK;
    i2cip_write32(val, reg_base, I2CIP_CTRL_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_master_mode(uint32_t reg_base, uint8_t master_mode)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_CTRL_REG_OFFSET);
    if (master_mode)
        val |= I2CIP_MASTER_MODE_MASK;
    else
        val &= ~I2CIP_MASTER_MODE_MASK;
    i2cip_write32(val, reg_base, I2CIP_CTRL_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_rx_threshold(uint32_t reg_base, uint32_t th)
{
    i2cip_write32(th, reg_base, I2CIP_RX_THRESHOLD_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_tx_threshold(uint32_t reg_base, uint32_t th)
{
    i2cip_write32(th, reg_base, I2CIP_TX_THRESHOLD_REG_OFFSET);
    return 0;
}
static inline uint8_t i2cip_w_cmd_data(uint32_t reg_base, uint32_t cmd_data)
{
    i2cip_write32(cmd_data, reg_base, I2CIP_CMD_DATA_REG_OFFSET);
    return 0;
}
static inline uint32_t i2cip_r_clr_tx_over(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_TX_OVER_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_rx_over(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_RX_OVER_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_rx_under(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_RX_UNDER_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_stop_det(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_STOP_DET_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_start_det(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_START_DET_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_tx_abrt(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_TX_ABRT_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_activity(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_ACTIVITY_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_all_intr(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_ALL_INTR_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_rd_req(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_RD_REQ_REG_OFFSET);
}
static inline uint32_t i2cip_r_clr_rx_done(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CLR_RX_DONE_REG_OFFSET);
}
static inline uint32_t i2cip_r_cmd_data(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_CMD_DATA_REG_OFFSET);
}
static inline uint32_t i2cip_r_status(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_STATUS_REG_OFFSET);
}
static inline uint32_t i2cip_r_int_status(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_INT_STATUS_REG_OFFSET);
}
static inline uint32_t i2cip_r_raw_int_status(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_RAW_INT_STATUS_REG_OFFSET);
}
static inline uint32_t i2cip_r_enable_status(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_ENABLE_STATUS_REG_OFFSET);
}
static inline uint32_t i2cip_r_tx_fifo_level(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_TX_FIFO_LEVEL_REG_OFFSET);
}
static inline uint32_t i2cip_r_rx_fifo_level(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_RX_FIFO_LEVEL_REG_OFFSET);
}
static inline uint32_t i2cip_r_tx_abrt_source(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_TX_ABRT_SOURCE_REG_OFFSET);
}
static inline uint32_t i2cip_w_tx_dma_enable(uint32_t reg_base, uint8_t enable)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_DMA_CR_REG_OFFSET);
    if (enable)
        val |= I2CIP_DMA_CR_TDMAE_MASK;
    else
        val &= ~I2CIP_DMA_CR_TDMAE_MASK;
    i2cip_write32(val, reg_base, I2CIP_DMA_CR_REG_OFFSET);
    return 0;
}
static inline uint32_t i2cip_w_rx_dma_enable(uint32_t reg_base, uint8_t enable)
{
    uint32_t val = 0;
    val = i2cip_read32(reg_base, I2CIP_DMA_CR_REG_OFFSET);
    if (enable)
        val |= I2CIP_DMA_CR_RDMAE_MASK;
    else
        val &= ~I2CIP_DMA_CR_RDMAE_MASK;
    i2cip_write32(val, reg_base, I2CIP_DMA_CR_REG_OFFSET);
    return 0;
}
static inline uint32_t i2cip_w_tx_dma_tl(uint32_t reg_base, uint32_t val)
{
    return i2cip_write32(val, reg_base, I2CIP_DMA_TX_TL_REG_OFFSET);
}
static inline uint32_t i2cip_w_rx_dma_tl(uint32_t reg_base, uint32_t val)
{
    return i2cip_write32(val, reg_base, I2CIP_DMA_RX_TL_REG_OFFSET);
}
static inline uint32_t i2cip_w_sda_hold_time(uint32_t reg_base, uint32_t val)
{
    return i2cip_write32(val, reg_base, I2CIP_SDA_HOLD_REG_OFFSET);
}
static inline uint32_t i2cip_r_ic_enable_status(uint32_t reg_base)
{
    return i2cip_read32(reg_base, I2CIP_IC_ENABLE_STATUS_REG_OFFSET);
}
static inline uint32_t i2cip_w_fs_spklen(uint32_t reg_base, uint32_t val)
{
    return i2cip_write32(val, reg_base, I2CIP_IC_FS_SPKLEN_REG_OFFSET);
}
static inline uint32_t i2cip_w_hs_spklen(uint32_t reg_base, uint32_t val)
{
    return i2cip_write32(val, reg_base, I2CIP_IC_HS_SPKLEN_REG_OFFSET);
}
static inline uint32_t i2cip_w_data_to_vad(uint32_t reg_base, uint32_t val)
{
    i2cip_write32(val ? 1 : 0, reg_base, I2CIP_IC_VAD_PATH_REG_OFFSET);
    return 0;
}
#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2CIP_H__ */
