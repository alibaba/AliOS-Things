#ifndef _REG_RC_H_
#define _REG_RC_H_

#include "co_int.h"
#include "_reg_rc.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#define REG_RC_COUNT 107

#define REG_RC_DECODING_MASK 0x000001FF

/**
 * @brief CNTL_STAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31         FORCE_ENABLE   0
 *     30            SPI_RESET   0
 *  18:16             RC_STATE   0x0
 *     12        CH0_SHDN_STAT   0
 *     08               CH0_LD   0
 *     03                RC_EN   0
 *     00               CH0_EN   0
 * </pre>
 */
#define RC_CNTL_STAT_ADDR   0x01050000
#define RC_CNTL_STAT_OFFSET 0x00000000
#define RC_CNTL_STAT_INDEX  0x00000000
#define RC_CNTL_STAT_RESET  0x00000000

__INLINE uint32_t rc_cntl_stat_get(void)
{
    return REG_PL_RD(RC_CNTL_STAT_ADDR);
}

__INLINE void rc_cntl_stat_set(uint32_t value)
{
    REG_PL_WR(RC_CNTL_STAT_ADDR, value);
}

// field definitions
#define RC_FORCE_ENABLE_BIT     ((uint32_t)0x80000000)
#define RC_FORCE_ENABLE_POS     31
#define RC_SPI_RESET_BIT        ((uint32_t)0x40000000)
#define RC_SPI_RESET_POS        30
#define RC_RC_STATE_MASK        ((uint32_t)0x00070000)
#define RC_RC_STATE_LSB         16
#define RC_RC_STATE_WIDTH       ((uint32_t)0x00000003)
#define RC_CH0_SHDN_STAT_BIT    ((uint32_t)0x00001000)
#define RC_CH0_SHDN_STAT_POS    12
#define RC_CH0_LD_BIT           ((uint32_t)0x00000100)
#define RC_CH0_LD_POS           8
#define RC_RC_EN_BIT            ((uint32_t)0x00000008)
#define RC_RC_EN_POS            3
#define RC_CH0_EN_BIT           ((uint32_t)0x00000001)
#define RC_CH0_EN_POS           0

#define RC_FORCE_ENABLE_RST     0x0
#define RC_SPI_RESET_RST        0x0
#define RC_RC_STATE_RST         0x0
#define RC_CH0_SHDN_STAT_RST    0x0
#define RC_CH0_LD_RST           0x0
#define RC_RC_EN_RST            0x0
#define RC_CH0_EN_RST           0x0

__INLINE void rc_cntl_stat_pack(uint8_t forceenable, uint8_t spireset, uint8_t ch0ld, uint8_t rcen, uint8_t ch0en)
{
    ASSERT_ERR((((uint32_t)forceenable << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)spireset << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)ch0ld << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)rcen << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)ch0en << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_CNTL_STAT_ADDR,  ((uint32_t)forceenable << 31) | ((uint32_t)spireset << 30) | ((uint32_t)ch0ld << 8) | ((uint32_t)rcen << 3) | ((uint32_t)ch0en << 0));
}

__INLINE void rc_cntl_stat_unpack(uint8_t *forceenable, uint8_t *spireset, uint8_t *rcstate, uint8_t *ch0shdnstat, uint8_t *ch0ld, uint8_t *rcen, uint8_t *ch0en)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);

    *forceenable = (localVal & ((uint32_t)0x80000000)) >> 31;
    *spireset = (localVal & ((uint32_t)0x40000000)) >> 30;
    *rcstate = (localVal & ((uint32_t)0x00070000)) >> 16;
    *ch0shdnstat = (localVal & ((uint32_t)0x00001000)) >> 12;
    *ch0ld = (localVal & ((uint32_t)0x00000100)) >> 8;
    *rcen = (localVal & ((uint32_t)0x00000008)) >> 3;
    *ch0en = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t rc_force_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void rc_force_enable_setf(uint8_t forceenable)
{
    ASSERT_ERR((((uint32_t)forceenable << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(RC_CNTL_STAT_ADDR, (REG_PL_RD(RC_CNTL_STAT_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)forceenable << 31));
}

__INLINE uint8_t rc_spi_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

__INLINE void rc_spi_reset_setf(uint8_t spireset)
{
    ASSERT_ERR((((uint32_t)spireset << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(RC_CNTL_STAT_ADDR, (REG_PL_RD(RC_CNTL_STAT_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)spireset << 30));
}

__INLINE uint8_t rc_rc_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

__INLINE uint8_t rc_ch0_shdn_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

__INLINE uint8_t rc_ch0_ld_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

__INLINE void rc_ch0_ld_setf(uint8_t ch0ld)
{
    ASSERT_ERR((((uint32_t)ch0ld << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(RC_CNTL_STAT_ADDR, (REG_PL_RD(RC_CNTL_STAT_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)ch0ld << 8));
}

__INLINE uint8_t rc_rc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void rc_rc_en_setf(uint8_t rcen)
{
    ASSERT_ERR((((uint32_t)rcen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(RC_CNTL_STAT_ADDR, (REG_PL_RD(RC_CNTL_STAT_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)rcen << 3));
}

__INLINE uint8_t rc_ch0_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CNTL_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void rc_ch0_en_setf(uint8_t ch0en)
{
    ASSERT_ERR((((uint32_t)ch0en << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_CNTL_STAT_ADDR, (REG_PL_RD(RC_CNTL_STAT_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)ch0en << 0));
}

/**
 * @brief BEKEN_SPI register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28            PRESCALER   0x0
 *  27:00         TRX_REG_STAT   0x0
 * </pre>
 */
#define RC_BEKEN_SPI_ADDR   0x01050004
#define RC_BEKEN_SPI_OFFSET 0x00000004
#define RC_BEKEN_SPI_INDEX  0x00000001
#define RC_BEKEN_SPI_RESET  0x00000000

__INLINE uint32_t rc_beken_spi_get(void)
{
    return REG_PL_RD(RC_BEKEN_SPI_ADDR);
}

__INLINE void rc_beken_spi_set(uint32_t value)
{
    REG_PL_WR(RC_BEKEN_SPI_ADDR, value);
}

// field definitions
#define RC_PRESCALER_MASK      ((uint32_t)0xF0000000)
#define RC_PRESCALER_LSB       28
#define RC_PRESCALER_WIDTH     ((uint32_t)0x00000004)
#define RC_TRX_REG_STAT_MASK   ((uint32_t)0x0FFFFFFF)
#define RC_TRX_REG_STAT_LSB    0
#define RC_TRX_REG_STAT_WIDTH  ((uint32_t)0x0000001C)

#define RC_PRESCALER_RST       0x0
#define RC_TRX_REG_STAT_RST    0x0

__INLINE void rc_beken_spi_unpack(uint8_t *prescaler, uint32_t *trxregstat)
{
    uint32_t localVal = REG_PL_RD(RC_BEKEN_SPI_ADDR);

    *prescaler = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *trxregstat = (localVal & ((uint32_t)0x0FFFFFFF)) >> 0;
}

__INLINE uint8_t rc_prescaler_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_BEKEN_SPI_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

__INLINE void rc_prescaler_setf(uint8_t prescaler)
{
    ASSERT_ERR((((uint32_t)prescaler << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(RC_BEKEN_SPI_ADDR, (uint32_t)prescaler << 28);
}

__INLINE uint32_t rc_trx_reg_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_BEKEN_SPI_ADDR);
    return ((localVal & ((uint32_t)0x0FFFFFFF)) >> 0);
}

/**
 * @brief CH0_OUTPOWER register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:00         CH0_OUTPOWER   0x0
 * </pre>
 */
#define RC_CH0_OUTPOWER_ADDR   0x01050014
#define RC_CH0_OUTPOWER_OFFSET 0x00000014
#define RC_CH0_OUTPOWER_INDEX  0x00000005
#define RC_CH0_OUTPOWER_RESET  0x00000000

__INLINE uint32_t rc_ch0_outpower_get(void)
{
    return REG_PL_RD(RC_CH0_OUTPOWER_ADDR);
}

__INLINE void rc_ch0_outpower_set(uint32_t value)
{
    REG_PL_WR(RC_CH0_OUTPOWER_ADDR, value);
}

// field definitions
#define RC_CH0_OUTPOWER_MASK   ((uint32_t)0x000000FF)
#define RC_CH0_OUTPOWER_LSB    0
#define RC_CH0_OUTPOWER_WIDTH  ((uint32_t)0x00000008)

#define RC_CH0_OUTPOWER_RST    0x0

__INLINE uint8_t rc_ch0_outpower_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_OUTPOWER_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief CH0_RX_ONOFF_DELAY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16     CH0_RX_OFF_DELAY   0x0
 *  07:00      CH0_RX_ON_DELAY   0x0
 * </pre>
 */
#define RC_CH0_RX_ONOFF_DELAY_ADDR   0x01050020
#define RC_CH0_RX_ONOFF_DELAY_OFFSET 0x00000020
#define RC_CH0_RX_ONOFF_DELAY_INDEX  0x00000008
#define RC_CH0_RX_ONOFF_DELAY_RESET  0x00000000

__INLINE uint32_t rc_ch0_rx_onoff_delay_get(void)
{
    return REG_PL_RD(RC_CH0_RX_ONOFF_DELAY_ADDR);
}

__INLINE void rc_ch0_rx_onoff_delay_set(uint32_t value)
{
    REG_PL_WR(RC_CH0_RX_ONOFF_DELAY_ADDR, value);
}

// field definitions
#define RC_CH0_RX_OFF_DELAY_MASK   ((uint32_t)0x00FF0000)
#define RC_CH0_RX_OFF_DELAY_LSB    16
#define RC_CH0_RX_OFF_DELAY_WIDTH  ((uint32_t)0x00000008)
#define RC_CH0_RX_ON_DELAY_MASK    ((uint32_t)0x000000FF)
#define RC_CH0_RX_ON_DELAY_LSB     0
#define RC_CH0_RX_ON_DELAY_WIDTH   ((uint32_t)0x00000008)

#define RC_CH0_RX_OFF_DELAY_RST    0x0
#define RC_CH0_RX_ON_DELAY_RST     0x0

__INLINE void rc_ch0_rx_onoff_delay_pack(uint8_t ch0rxoffdelay, uint8_t ch0rxondelay)
{
    ASSERT_ERR((((uint32_t)ch0rxoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)ch0rxondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_RX_ONOFF_DELAY_ADDR,  ((uint32_t)ch0rxoffdelay << 16) | ((uint32_t)ch0rxondelay << 0));
}

__INLINE void rc_ch0_rx_onoff_delay_unpack(uint8_t *ch0rxoffdelay, uint8_t *ch0rxondelay)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_RX_ONOFF_DELAY_ADDR);

    *ch0rxoffdelay = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ch0rxondelay = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

__INLINE uint8_t rc_ch0_rx_off_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_RX_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

__INLINE void rc_ch0_rx_off_delay_setf(uint8_t ch0rxoffdelay)
{
    ASSERT_ERR((((uint32_t)ch0rxoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(RC_CH0_RX_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_RX_ONOFF_DELAY_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)ch0rxoffdelay << 16));
}

__INLINE uint8_t rc_ch0_rx_on_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_RX_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

__INLINE void rc_ch0_rx_on_delay_setf(uint8_t ch0rxondelay)
{
    ASSERT_ERR((((uint32_t)ch0rxondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_RX_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_RX_ONOFF_DELAY_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)ch0rxondelay << 0));
}

/**
 * @brief CH0_TX_ONOFF_DELAY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16     CH0_TX_OFF_DELAY   0x0
 *  07:00      CH0_TX_ON_DELAY   0x0
 * </pre>
 */
#define RC_CH0_TX_ONOFF_DELAY_ADDR   0x0105002C
#define RC_CH0_TX_ONOFF_DELAY_OFFSET 0x0000002C
#define RC_CH0_TX_ONOFF_DELAY_INDEX  0x0000000B
#define RC_CH0_TX_ONOFF_DELAY_RESET  0x00000000

__INLINE uint32_t rc_ch0_tx_onoff_delay_get(void)
{
    return REG_PL_RD(RC_CH0_TX_ONOFF_DELAY_ADDR);
}

__INLINE void rc_ch0_tx_onoff_delay_set(uint32_t value)
{
    REG_PL_WR(RC_CH0_TX_ONOFF_DELAY_ADDR, value);
}

// field definitions
#define RC_CH0_TX_OFF_DELAY_MASK   ((uint32_t)0x00FF0000)
#define RC_CH0_TX_OFF_DELAY_LSB    16
#define RC_CH0_TX_OFF_DELAY_WIDTH  ((uint32_t)0x00000008)
#define RC_CH0_TX_ON_DELAY_MASK    ((uint32_t)0x000000FF)
#define RC_CH0_TX_ON_DELAY_LSB     0
#define RC_CH0_TX_ON_DELAY_WIDTH   ((uint32_t)0x00000008)

#define RC_CH0_TX_OFF_DELAY_RST    0x0
#define RC_CH0_TX_ON_DELAY_RST     0x0

__INLINE void rc_ch0_tx_onoff_delay_pack(uint8_t ch0txoffdelay, uint8_t ch0txondelay)
{
    ASSERT_ERR((((uint32_t)ch0txoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)ch0txondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_TX_ONOFF_DELAY_ADDR,  ((uint32_t)ch0txoffdelay << 16) | ((uint32_t)ch0txondelay << 0));
}

__INLINE void rc_ch0_tx_onoff_delay_unpack(uint8_t *ch0txoffdelay, uint8_t *ch0txondelay)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_TX_ONOFF_DELAY_ADDR);

    *ch0txoffdelay = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ch0txondelay = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

__INLINE uint8_t rc_ch0_tx_off_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_TX_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

__INLINE void rc_ch0_tx_off_delay_setf(uint8_t ch0txoffdelay)
{
    ASSERT_ERR((((uint32_t)ch0txoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(RC_CH0_TX_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_TX_ONOFF_DELAY_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)ch0txoffdelay << 16));
}

__INLINE uint8_t rc_ch0_tx_on_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_TX_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

__INLINE void rc_ch0_tx_on_delay_setf(uint8_t ch0txondelay)
{
    ASSERT_ERR((((uint32_t)ch0txondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_TX_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_TX_ONOFF_DELAY_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)ch0txondelay << 0));
}

/**
 * @brief CH0_PA_ONOFF_DELAY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16     CH0_PA_OFF_DELAY   0x0
 *  07:00      CH0_PA_ON_DELAY   0x0
 * </pre>
 */
#define RC_CH0_PA_ONOFF_DELAY_ADDR   0x01050038
#define RC_CH0_PA_ONOFF_DELAY_OFFSET 0x00000038
#define RC_CH0_PA_ONOFF_DELAY_INDEX  0x0000000E
#define RC_CH0_PA_ONOFF_DELAY_RESET  0x00000000

__INLINE uint32_t rc_ch0_pa_onoff_delay_get(void)
{
    return REG_PL_RD(RC_CH0_PA_ONOFF_DELAY_ADDR);
}

__INLINE void rc_ch0_pa_onoff_delay_set(uint32_t value)
{
    REG_PL_WR(RC_CH0_PA_ONOFF_DELAY_ADDR, value);
}

// field definitions
#define RC_CH0_PA_OFF_DELAY_MASK   ((uint32_t)0x00FF0000)
#define RC_CH0_PA_OFF_DELAY_LSB    16
#define RC_CH0_PA_OFF_DELAY_WIDTH  ((uint32_t)0x00000008)
#define RC_CH0_PA_ON_DELAY_MASK    ((uint32_t)0x000000FF)
#define RC_CH0_PA_ON_DELAY_LSB     0
#define RC_CH0_PA_ON_DELAY_WIDTH   ((uint32_t)0x00000008)

#define RC_CH0_PA_OFF_DELAY_RST    0x0
#define RC_CH0_PA_ON_DELAY_RST     0x0

__INLINE void rc_ch0_pa_onoff_delay_pack(uint8_t ch0paoffdelay, uint8_t ch0paondelay)
{
    ASSERT_ERR((((uint32_t)ch0paoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)ch0paondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_PA_ONOFF_DELAY_ADDR,  ((uint32_t)ch0paoffdelay << 16) | ((uint32_t)ch0paondelay << 0));
}

__INLINE void rc_ch0_pa_onoff_delay_unpack(uint8_t *ch0paoffdelay, uint8_t *ch0paondelay)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_PA_ONOFF_DELAY_ADDR);

    *ch0paoffdelay = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ch0paondelay = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

__INLINE uint8_t rc_ch0_pa_off_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_PA_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

__INLINE void rc_ch0_pa_off_delay_setf(uint8_t ch0paoffdelay)
{
    ASSERT_ERR((((uint32_t)ch0paoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(RC_CH0_PA_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_PA_ONOFF_DELAY_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)ch0paoffdelay << 16));
}

__INLINE uint8_t rc_ch0_pa_on_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_PA_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

__INLINE void rc_ch0_pa_on_delay_setf(uint8_t ch0paondelay)
{
    ASSERT_ERR((((uint32_t)ch0paondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_PA_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_PA_ONOFF_DELAY_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)ch0paondelay << 0));
}

/**
 * @brief CH0_SHDN_ONOFF_DELAY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16   CH0_SHDN_OFF_DELAY   0x0
 *  07:00    CH0_SHDN_ON_DELAY   0x0
 * </pre>
 */
#define RC_CH0_SHDN_ONOFF_DELAY_ADDR   0x01050044
#define RC_CH0_SHDN_ONOFF_DELAY_OFFSET 0x00000044
#define RC_CH0_SHDN_ONOFF_DELAY_INDEX  0x00000011
#define RC_CH0_SHDN_ONOFF_DELAY_RESET  0x00000000

__INLINE uint32_t rc_ch0_shdn_onoff_delay_get(void)
{
    return REG_PL_RD(RC_CH0_SHDN_ONOFF_DELAY_ADDR);
}

__INLINE void rc_ch0_shdn_onoff_delay_set(uint32_t value)
{
    REG_PL_WR(RC_CH0_SHDN_ONOFF_DELAY_ADDR, value);
}

// field definitions
#define RC_CH0_SHDN_OFF_DELAY_MASK   ((uint32_t)0x00FF0000)
#define RC_CH0_SHDN_OFF_DELAY_LSB    16
#define RC_CH0_SHDN_OFF_DELAY_WIDTH  ((uint32_t)0x00000008)
#define RC_CH0_SHDN_ON_DELAY_MASK    ((uint32_t)0x000000FF)
#define RC_CH0_SHDN_ON_DELAY_LSB     0
#define RC_CH0_SHDN_ON_DELAY_WIDTH   ((uint32_t)0x00000008)

#define RC_CH0_SHDN_OFF_DELAY_RST    0x0
#define RC_CH0_SHDN_ON_DELAY_RST     0x0

__INLINE void rc_ch0_shdn_onoff_delay_pack(uint8_t ch0shdnoffdelay, uint8_t ch0shdnondelay)
{
    ASSERT_ERR((((uint32_t)ch0shdnoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)ch0shdnondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_SHDN_ONOFF_DELAY_ADDR,  ((uint32_t)ch0shdnoffdelay << 16) | ((uint32_t)ch0shdnondelay << 0));
}

__INLINE void rc_ch0_shdn_onoff_delay_unpack(uint8_t *ch0shdnoffdelay, uint8_t *ch0shdnondelay)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_SHDN_ONOFF_DELAY_ADDR);

    *ch0shdnoffdelay = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ch0shdnondelay = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

__INLINE uint8_t rc_ch0_shdn_off_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_SHDN_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

__INLINE void rc_ch0_shdn_off_delay_setf(uint8_t ch0shdnoffdelay)
{
    ASSERT_ERR((((uint32_t)ch0shdnoffdelay << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(RC_CH0_SHDN_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_SHDN_ONOFF_DELAY_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)ch0shdnoffdelay << 16));
}

__INLINE uint8_t rc_ch0_shdn_on_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_SHDN_ONOFF_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

__INLINE void rc_ch0_shdn_on_delay_setf(uint8_t ch0shdnondelay)
{
    ASSERT_ERR((((uint32_t)ch0shdnondelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(RC_CH0_SHDN_ONOFF_DELAY_ADDR, (REG_PL_RD(RC_CH0_SHDN_ONOFF_DELAY_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)ch0shdnondelay << 0));
}

/**
 * @brief CH0_FORCE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     16             F_CH0_EN   0
 *  15:08              F_CH0_B   0x0
 *     03           F_CH0_RXHP   0
 *     02           F_CH0_TXEN   0
 *     01           F_CH0_RXEN   0
 *     00           F_CH0_SHDN   0
 * </pre>
 */
#define RC_CH0_FORCE_ADDR   0x01050064
#define RC_CH0_FORCE_OFFSET 0x00000064
#define RC_CH0_FORCE_INDEX  0x00000019
#define RC_CH0_FORCE_RESET  0x00000000

__INLINE uint32_t rc_ch0_force_get(void)
{
    return REG_PL_RD(RC_CH0_FORCE_ADDR);
}

__INLINE void rc_ch0_force_set(uint32_t value)
{
    REG_PL_WR(RC_CH0_FORCE_ADDR, value);
}

// field definitions
#define RC_F_CH0_EN_BIT      ((uint32_t)0x00010000)
#define RC_F_CH0_EN_POS      16
#define RC_F_CH0_B_MASK      ((uint32_t)0x0000FF00)
#define RC_F_CH0_B_LSB       8
#define RC_F_CH0_B_WIDTH     ((uint32_t)0x00000008)
#define RC_F_CH0_RXHP_BIT    ((uint32_t)0x00000008)
#define RC_F_CH0_RXHP_POS    3
#define RC_F_CH0_TXEN_BIT    ((uint32_t)0x00000004)
#define RC_F_CH0_TXEN_POS    2
#define RC_F_CH0_RXEN_BIT    ((uint32_t)0x00000002)
#define RC_F_CH0_RXEN_POS    1
#define RC_F_CH0_SHDN_BIT    ((uint32_t)0x00000001)
#define RC_F_CH0_SHDN_POS    0

#define RC_F_CH0_EN_RST      0x0
#define RC_F_CH0_B_RST       0x0
#define RC_F_CH0_RXHP_RST    0x0
#define RC_F_CH0_TXEN_RST    0x0
#define RC_F_CH0_RXEN_RST    0x0
#define RC_F_CH0_SHDN_RST    0x0

__INLINE void rc_ch0_force_pack(uint8_t fch0en, uint8_t fch0b, uint8_t fch0rxhp, uint8_t fch0txen, uint8_t fch0rxen, uint8_t fch0shdn)
{
    ASSERT_ERR((((uint32_t)fch0en << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)fch0b << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)fch0rxhp << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)fch0txen << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)fch0rxen << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)fch0shdn << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_CH0_FORCE_ADDR,  ((uint32_t)fch0en << 16) | ((uint32_t)fch0b << 8) | ((uint32_t)fch0rxhp << 3) | ((uint32_t)fch0txen << 2) | ((uint32_t)fch0rxen << 1) | ((uint32_t)fch0shdn << 0));
}

__INLINE void rc_ch0_force_unpack(uint8_t *fch0en, uint8_t *fch0b, uint8_t *fch0rxhp, uint8_t *fch0txen, uint8_t *fch0rxen, uint8_t *fch0shdn)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_FORCE_ADDR);

    *fch0en = (localVal & ((uint32_t)0x00010000)) >> 16;
    *fch0b = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *fch0rxhp = (localVal & ((uint32_t)0x00000008)) >> 3;
    *fch0txen = (localVal & ((uint32_t)0x00000004)) >> 2;
    *fch0rxen = (localVal & ((uint32_t)0x00000002)) >> 1;
    *fch0shdn = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t rc_f_ch0_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

__INLINE void rc_f_ch0_en_setf(uint8_t fch0en)
{
    ASSERT_ERR((((uint32_t)fch0en << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(RC_CH0_FORCE_ADDR, (REG_PL_RD(RC_CH0_FORCE_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)fch0en << 16));
}

__INLINE uint8_t rc_f_ch0_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

__INLINE void rc_f_ch0_b_setf(uint8_t fch0b)
{
    ASSERT_ERR((((uint32_t)fch0b << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(RC_CH0_FORCE_ADDR, (REG_PL_RD(RC_CH0_FORCE_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)fch0b << 8));
}

__INLINE uint8_t rc_f_ch0_rxhp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void rc_f_ch0_rxhp_setf(uint8_t fch0rxhp)
{
    ASSERT_ERR((((uint32_t)fch0rxhp << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(RC_CH0_FORCE_ADDR, (REG_PL_RD(RC_CH0_FORCE_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)fch0rxhp << 3));
}

__INLINE uint8_t rc_f_ch0_txen_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE void rc_f_ch0_txen_setf(uint8_t fch0txen)
{
    ASSERT_ERR((((uint32_t)fch0txen << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(RC_CH0_FORCE_ADDR, (REG_PL_RD(RC_CH0_FORCE_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)fch0txen << 2));
}

__INLINE uint8_t rc_f_ch0_rxen_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void rc_f_ch0_rxen_setf(uint8_t fch0rxen)
{
    ASSERT_ERR((((uint32_t)fch0rxen << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(RC_CH0_FORCE_ADDR, (REG_PL_RD(RC_CH0_FORCE_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)fch0rxen << 1));
}

__INLINE uint8_t rc_f_ch0_shdn_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_CH0_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void rc_f_ch0_shdn_setf(uint8_t fch0shdn)
{
    ASSERT_ERR((((uint32_t)fch0shdn << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_CH0_FORCE_ADDR, (REG_PL_RD(RC_CH0_FORCE_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)fch0shdn << 0));
}

/**
 * @brief MISC_FORCE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     01              F_TX_ON   0
 *     00              F_RX_ON   0
 * </pre>
 */
#define RC_MISC_FORCE_ADDR   0x01050070
#define RC_MISC_FORCE_OFFSET 0x00000070
#define RC_MISC_FORCE_INDEX  0x0000001C
#define RC_MISC_FORCE_RESET  0x00000000

__INLINE uint32_t rc_misc_force_get(void)
{
    return REG_PL_RD(RC_MISC_FORCE_ADDR);
}

__INLINE void rc_misc_force_set(uint32_t value)
{
    REG_PL_WR(RC_MISC_FORCE_ADDR, value);
}

// field definitions
#define RC_F_TX_ON_BIT    ((uint32_t)0x00000002)
#define RC_F_TX_ON_POS    1
#define RC_F_RX_ON_BIT    ((uint32_t)0x00000001)
#define RC_F_RX_ON_POS    0

#define RC_F_TX_ON_RST    0x0
#define RC_F_RX_ON_RST    0x0

__INLINE void rc_misc_force_pack(uint8_t ftxon, uint8_t frxon)
{
    ASSERT_ERR((((uint32_t)ftxon << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)frxon << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_MISC_FORCE_ADDR,  ((uint32_t)ftxon << 1) | ((uint32_t)frxon << 0));
}

__INLINE void rc_misc_force_unpack(uint8_t *ftxon, uint8_t *frxon)
{
    uint32_t localVal = REG_PL_RD(RC_MISC_FORCE_ADDR);

    *ftxon = (localVal & ((uint32_t)0x00000002)) >> 1;
    *frxon = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t rc_f_tx_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_MISC_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void rc_f_tx_on_setf(uint8_t ftxon)
{
    ASSERT_ERR((((uint32_t)ftxon << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(RC_MISC_FORCE_ADDR, (REG_PL_RD(RC_MISC_FORCE_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)ftxon << 1));
}

__INLINE uint8_t rc_f_rx_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_MISC_FORCE_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void rc_f_rx_on_setf(uint8_t frxon)
{
    ASSERT_ERR((((uint32_t)frxon << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_MISC_FORCE_ADDR, (REG_PL_RD(RC_MISC_FORCE_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)frxon << 0));
}

/**
 * @brief FE_RX_DEL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  11:00         FE_RX_ON_DEL   0x0
 * </pre>
 */
#define RC_FE_RX_DEL_ADDR   0x01050078
#define RC_FE_RX_DEL_OFFSET 0x00000078
#define RC_FE_RX_DEL_INDEX  0x0000001E
#define RC_FE_RX_DEL_RESET  0x00000000

__INLINE uint32_t rc_fe_rx_del_get(void)
{
    return REG_PL_RD(RC_FE_RX_DEL_ADDR);
}

__INLINE void rc_fe_rx_del_set(uint32_t value)
{
    REG_PL_WR(RC_FE_RX_DEL_ADDR, value);
}

// field definitions
#define RC_FE_RX_ON_DEL_MASK   ((uint32_t)0x00000FFF)
#define RC_FE_RX_ON_DEL_LSB    0
#define RC_FE_RX_ON_DEL_WIDTH  ((uint32_t)0x0000000C)

#define RC_FE_RX_ON_DEL_RST    0x0

__INLINE uint16_t rc_fe_rx_on_del_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_FE_RX_DEL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000FFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_fe_rx_on_del_setf(uint16_t ferxondel)
{
    ASSERT_ERR((((uint32_t)ferxondel << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(RC_FE_RX_DEL_ADDR, (uint32_t)ferxondel << 0);
}

/**
 * @brief TRX_REG0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG0   0x0
 * </pre>
 */
#define RC_TRX_REG0_ADDR   0x01050080
#define RC_TRX_REG0_OFFSET 0x00000080
#define RC_TRX_REG0_INDEX  0x00000020
#define RC_TRX_REG0_RESET  0x00000000

__INLINE uint32_t rc_trx_reg0_get(void)
{
    return REG_PL_RD(RC_TRX_REG0_ADDR);
}

__INLINE void rc_trx_reg0_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG0_ADDR, value);
}

// field definitions
#define RC_TRX_REG0_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG0_LSB    0
#define RC_TRX_REG0_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG0_RST    0x0

__INLINE uint32_t rc_trx_reg0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg0_setf(uint32_t trxreg0)
{
    ASSERT_ERR((((uint32_t)trxreg0 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG0_ADDR, (uint32_t)trxreg0 << 0);
}

/**
 * @brief TRX_REG1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG1   0x0
 * </pre>
 */
#define RC_TRX_REG1_ADDR   0x01050084
#define RC_TRX_REG1_OFFSET 0x00000084
#define RC_TRX_REG1_INDEX  0x00000021
#define RC_TRX_REG1_RESET  0x00000000

__INLINE uint32_t rc_trx_reg1_get(void)
{
    return REG_PL_RD(RC_TRX_REG1_ADDR);
}

__INLINE void rc_trx_reg1_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG1_ADDR, value);
}

// field definitions
#define RC_TRX_REG1_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG1_LSB    0
#define RC_TRX_REG1_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG1_RST    0x0

__INLINE uint32_t rc_trx_reg1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg1_setf(uint32_t trxreg1)
{
    ASSERT_ERR((((uint32_t)trxreg1 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG1_ADDR, (uint32_t)trxreg1 << 0);
}

/**
 * @brief TRX_REG2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG2   0x0
 * </pre>
 */
#define RC_TRX_REG2_ADDR   0x01050088
#define RC_TRX_REG2_OFFSET 0x00000088
#define RC_TRX_REG2_INDEX  0x00000022
#define RC_TRX_REG2_RESET  0x00000000

__INLINE uint32_t rc_trx_reg2_get(void)
{
    return REG_PL_RD(RC_TRX_REG2_ADDR);
}

__INLINE void rc_trx_reg2_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG2_ADDR, value);
}

// field definitions
#define RC_TRX_REG2_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG2_LSB    0
#define RC_TRX_REG2_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG2_RST    0x0

__INLINE uint32_t rc_trx_reg2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg2_setf(uint32_t trxreg2)
{
    ASSERT_ERR((((uint32_t)trxreg2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG2_ADDR, (uint32_t)trxreg2 << 0);
}

/**
 * @brief TRX_REG3 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG3   0x0
 * </pre>
 */
#define RC_TRX_REG3_ADDR   0x0105008C
#define RC_TRX_REG3_OFFSET 0x0000008C
#define RC_TRX_REG3_INDEX  0x00000023
#define RC_TRX_REG3_RESET  0x00000000

__INLINE uint32_t rc_trx_reg3_get(void)
{
    return REG_PL_RD(RC_TRX_REG3_ADDR);
}

__INLINE void rc_trx_reg3_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG3_ADDR, value);
}

// field definitions
#define RC_TRX_REG3_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG3_LSB    0
#define RC_TRX_REG3_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG3_RST    0x0

__INLINE uint32_t rc_trx_reg3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg3_setf(uint32_t trxreg3)
{
    ASSERT_ERR((((uint32_t)trxreg3 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG3_ADDR, (uint32_t)trxreg3 << 0);
}

/**
 * @brief TRX_REG4 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG4   0x0
 * </pre>
 */
#define RC_TRX_REG4_ADDR   0x01050090
#define RC_TRX_REG4_OFFSET 0x00000090
#define RC_TRX_REG4_INDEX  0x00000024
#define RC_TRX_REG4_RESET  0x00000000

__INLINE uint32_t rc_trx_reg4_get(void)
{
    return REG_PL_RD(RC_TRX_REG4_ADDR);
}

__INLINE void rc_trx_reg4_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG4_ADDR, value);
}

// field definitions
#define RC_TRX_REG4_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG4_LSB    0
#define RC_TRX_REG4_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG4_RST    0x0

__INLINE uint32_t rc_trx_reg4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG4_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg4_setf(uint32_t trxreg4)
{
    ASSERT_ERR((((uint32_t)trxreg4 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG4_ADDR, (uint32_t)trxreg4 << 0);
}

/**
 * @brief TRX_REG5 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG5   0x0
 * </pre>
 */
#define RC_TRX_REG5_ADDR   0x01050094
#define RC_TRX_REG5_OFFSET 0x00000094
#define RC_TRX_REG5_INDEX  0x00000025
#define RC_TRX_REG5_RESET  0x00000000

__INLINE uint32_t rc_trx_reg5_get(void)
{
    return REG_PL_RD(RC_TRX_REG5_ADDR);
}

__INLINE void rc_trx_reg5_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG5_ADDR, value);
}

// field definitions
#define RC_TRX_REG5_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG5_LSB    0
#define RC_TRX_REG5_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG5_RST    0x0

__INLINE uint32_t rc_trx_reg5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG5_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg5_setf(uint32_t trxreg5)
{
    ASSERT_ERR((((uint32_t)trxreg5 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG5_ADDR, (uint32_t)trxreg5 << 0);
}

/**
 * @brief TRX_REG6 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG6   0x0
 * </pre>
 */
#define RC_TRX_REG6_ADDR   0x01050098
#define RC_TRX_REG6_OFFSET 0x00000098
#define RC_TRX_REG6_INDEX  0x00000026
#define RC_TRX_REG6_RESET  0x00000000

__INLINE uint32_t rc_trx_reg6_get(void)
{
    return REG_PL_RD(RC_TRX_REG6_ADDR);
}

__INLINE void rc_trx_reg6_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG6_ADDR, value);
}

// field definitions
#define RC_TRX_REG6_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG6_LSB    0
#define RC_TRX_REG6_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG6_RST    0x0

__INLINE uint32_t rc_trx_reg6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG6_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg6_setf(uint32_t trxreg6)
{
    ASSERT_ERR((((uint32_t)trxreg6 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG6_ADDR, (uint32_t)trxreg6 << 0);
}

/**
 * @brief TRX_REG7 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG7   0x0
 * </pre>
 */
#define RC_TRX_REG7_ADDR   0x0105009C
#define RC_TRX_REG7_OFFSET 0x0000009C
#define RC_TRX_REG7_INDEX  0x00000027
#define RC_TRX_REG7_RESET  0x00000000

__INLINE uint32_t rc_trx_reg7_get(void)
{
    return REG_PL_RD(RC_TRX_REG7_ADDR);
}

__INLINE void rc_trx_reg7_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG7_ADDR, value);
}

// field definitions
#define RC_TRX_REG7_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG7_LSB    0
#define RC_TRX_REG7_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG7_RST    0x0

__INLINE uint32_t rc_trx_reg7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG7_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg7_setf(uint32_t trxreg7)
{
    ASSERT_ERR((((uint32_t)trxreg7 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG7_ADDR, (uint32_t)trxreg7 << 0);
}

/**
 * @brief TRX_REG8 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG8   0x0
 * </pre>
 */
#define RC_TRX_REG8_ADDR   0x010500A0
#define RC_TRX_REG8_OFFSET 0x000000A0
#define RC_TRX_REG8_INDEX  0x00000028
#define RC_TRX_REG8_RESET  0x00000000

__INLINE uint32_t rc_trx_reg8_get(void)
{
    return REG_PL_RD(RC_TRX_REG8_ADDR);
}

__INLINE void rc_trx_reg8_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG8_ADDR, value);
}

// field definitions
#define RC_TRX_REG8_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG8_LSB    0
#define RC_TRX_REG8_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG8_RST    0x0

__INLINE uint32_t rc_trx_reg8_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG8_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg8_setf(uint32_t trxreg8)
{
    ASSERT_ERR((((uint32_t)trxreg8 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG8_ADDR, (uint32_t)trxreg8 << 0);
}

/**
 * @brief TRX_REG9 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TRX_REG9   0x0
 * </pre>
 */
#define RC_TRX_REG9_ADDR   0x010500A4
#define RC_TRX_REG9_OFFSET 0x000000A4
#define RC_TRX_REG9_INDEX  0x00000029
#define RC_TRX_REG9_RESET  0x00000000

__INLINE uint32_t rc_trx_reg9_get(void)
{
    return REG_PL_RD(RC_TRX_REG9_ADDR);
}

__INLINE void rc_trx_reg9_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG9_ADDR, value);
}

// field definitions
#define RC_TRX_REG9_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG9_LSB    0
#define RC_TRX_REG9_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG9_RST    0x0

__INLINE uint32_t rc_trx_reg9_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG9_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg9_setf(uint32_t trxreg9)
{
    ASSERT_ERR((((uint32_t)trxreg9 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG9_ADDR, (uint32_t)trxreg9 << 0);
}

/**
 * @brief TRX_REG10 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG10   0x0
 * </pre>
 */
#define RC_TRX_REG10_ADDR   0x010500A8
#define RC_TRX_REG10_OFFSET 0x000000A8
#define RC_TRX_REG10_INDEX  0x0000002A
#define RC_TRX_REG10_RESET  0x00000000

__INLINE uint32_t rc_trx_reg10_get(void)
{
    return REG_PL_RD(RC_TRX_REG10_ADDR);
}

__INLINE void rc_trx_reg10_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG10_ADDR, value);
}

// field definitions
#define RC_TRX_REG10_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG10_LSB    0
#define RC_TRX_REG10_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG10_RST    0x0

__INLINE uint32_t rc_trx_reg10_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG10_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg10_setf(uint32_t trxreg10)
{
    ASSERT_ERR((((uint32_t)trxreg10 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG10_ADDR, (uint32_t)trxreg10 << 0);
}

/**
 * @brief TRX_REG11 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG11   0x0
 * </pre>
 */
#define RC_TRX_REG11_ADDR   0x010500AC
#define RC_TRX_REG11_OFFSET 0x000000AC
#define RC_TRX_REG11_INDEX  0x0000002B
#define RC_TRX_REG11_RESET  0x00000000

__INLINE uint32_t rc_trx_reg11_get(void)
{
    return REG_PL_RD(RC_TRX_REG11_ADDR);
}

__INLINE void rc_trx_reg11_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG11_ADDR, value);
}

// field definitions
#define RC_TRX_REG11_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG11_LSB    0
#define RC_TRX_REG11_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG11_RST    0x0

__INLINE uint32_t rc_trx_reg11_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG11_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg11_setf(uint32_t trxreg11)
{
    ASSERT_ERR((((uint32_t)trxreg11 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG11_ADDR, (uint32_t)trxreg11 << 0);
}

/**
 * @brief TRX_REG12 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG12   0x0
 * </pre>
 */
#define RC_TRX_REG12_ADDR   0x010500B0
#define RC_TRX_REG12_OFFSET 0x000000B0
#define RC_TRX_REG12_INDEX  0x0000002C
#define RC_TRX_REG12_RESET  0x00000000

__INLINE uint32_t rc_trx_reg12_get(void)
{
    return REG_PL_RD(RC_TRX_REG12_ADDR);
}

__INLINE void rc_trx_reg12_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG12_ADDR, value);
}

// field definitions
#define RC_TRX_REG12_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG12_LSB    0
#define RC_TRX_REG12_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG12_RST    0x0

__INLINE uint32_t rc_trx_reg12_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG12_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg12_setf(uint32_t trxreg12)
{
    ASSERT_ERR((((uint32_t)trxreg12 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG12_ADDR, (uint32_t)trxreg12 << 0);
}

/**
 * @brief TRX_REG13 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG13   0x0
 * </pre>
 */
#define RC_TRX_REG13_ADDR   0x010500B4
#define RC_TRX_REG13_OFFSET 0x000000B4
#define RC_TRX_REG13_INDEX  0x0000002D
#define RC_TRX_REG13_RESET  0x00000000

__INLINE uint32_t rc_trx_reg13_get(void)
{
    return REG_PL_RD(RC_TRX_REG13_ADDR);
}

__INLINE void rc_trx_reg13_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG13_ADDR, value);
}

// field definitions
#define RC_TRX_REG13_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG13_LSB    0
#define RC_TRX_REG13_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG13_RST    0x0

__INLINE uint32_t rc_trx_reg13_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG13_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg13_setf(uint32_t trxreg13)
{
    ASSERT_ERR((((uint32_t)trxreg13 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG13_ADDR, (uint32_t)trxreg13 << 0);
}

/**
 * @brief TRX_REG14 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG14   0x0
 * </pre>
 */
#define RC_TRX_REG14_ADDR   0x010500B8
#define RC_TRX_REG14_OFFSET 0x000000B8
#define RC_TRX_REG14_INDEX  0x0000002E
#define RC_TRX_REG14_RESET  0x00000000

__INLINE uint32_t rc_trx_reg14_get(void)
{
    return REG_PL_RD(RC_TRX_REG14_ADDR);
}

__INLINE void rc_trx_reg14_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG14_ADDR, value);
}

// field definitions
#define RC_TRX_REG14_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG14_LSB    0
#define RC_TRX_REG14_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG14_RST    0x0

__INLINE uint32_t rc_trx_reg14_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG14_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg14_setf(uint32_t trxreg14)
{
    ASSERT_ERR((((uint32_t)trxreg14 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG14_ADDR, (uint32_t)trxreg14 << 0);
}

/**
 * @brief TRX_REG15 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG15   0x0
 * </pre>
 */
#define RC_TRX_REG15_ADDR   0x010500BC
#define RC_TRX_REG15_OFFSET 0x000000BC
#define RC_TRX_REG15_INDEX  0x0000002F
#define RC_TRX_REG15_RESET  0x00000000

__INLINE uint32_t rc_trx_reg15_get(void)
{
    return REG_PL_RD(RC_TRX_REG15_ADDR);
}

__INLINE void rc_trx_reg15_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG15_ADDR, value);
}

// field definitions
#define RC_TRX_REG15_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG15_LSB    0
#define RC_TRX_REG15_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG15_RST    0x0

__INLINE uint32_t rc_trx_reg15_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG15_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg15_setf(uint32_t trxreg15)
{
    ASSERT_ERR((((uint32_t)trxreg15 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG15_ADDR, (uint32_t)trxreg15 << 0);
}

/**
 * @brief TRX_REG16 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG16   0x0
 * </pre>
 */
#define RC_TRX_REG16_ADDR   0x010500C0
#define RC_TRX_REG16_OFFSET 0x000000C0
#define RC_TRX_REG16_INDEX  0x00000030
#define RC_TRX_REG16_RESET  0x00000000

__INLINE uint32_t rc_trx_reg16_get(void)
{
    return REG_PL_RD(RC_TRX_REG16_ADDR);
}

__INLINE void rc_trx_reg16_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG16_ADDR, value);
}

// field definitions
#define RC_TRX_REG16_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG16_LSB    0
#define RC_TRX_REG16_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG16_RST    0x0

__INLINE uint32_t rc_trx_reg16_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG16_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg16_setf(uint32_t trxreg16)
{
    ASSERT_ERR((((uint32_t)trxreg16 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG16_ADDR, (uint32_t)trxreg16 << 0);
}

/**
 * @brief TRX_REG17 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG17   0x0
 * </pre>
 */
#define RC_TRX_REG17_ADDR   0x010500C4
#define RC_TRX_REG17_OFFSET 0x000000C4
#define RC_TRX_REG17_INDEX  0x00000031
#define RC_TRX_REG17_RESET  0x00000000

__INLINE uint32_t rc_trx_reg17_get(void)
{
    return REG_PL_RD(RC_TRX_REG17_ADDR);
}

__INLINE void rc_trx_reg17_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG17_ADDR, value);
}

// field definitions
#define RC_TRX_REG17_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG17_LSB    0
#define RC_TRX_REG17_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG17_RST    0x0

__INLINE uint32_t rc_trx_reg17_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG17_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg17_setf(uint32_t trxreg17)
{
    ASSERT_ERR((((uint32_t)trxreg17 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG17_ADDR, (uint32_t)trxreg17 << 0);
}

/**
 * @brief TRX_REG18 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG18   0x0
 * </pre>
 */
#define RC_TRX_REG18_ADDR   0x010500C8
#define RC_TRX_REG18_OFFSET 0x000000C8
#define RC_TRX_REG18_INDEX  0x00000032
#define RC_TRX_REG18_RESET  0x00000000

__INLINE uint32_t rc_trx_reg18_get(void)
{
    return REG_PL_RD(RC_TRX_REG18_ADDR);
}

__INLINE void rc_trx_reg18_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG18_ADDR, value);
}

// field definitions
#define RC_TRX_REG18_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG18_LSB    0
#define RC_TRX_REG18_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG18_RST    0x0

__INLINE uint32_t rc_trx_reg18_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG18_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg18_setf(uint32_t trxreg18)
{
    ASSERT_ERR((((uint32_t)trxreg18 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG18_ADDR, (uint32_t)trxreg18 << 0);
}

/**
 * @brief TRX_REG19 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG19   0x0
 * </pre>
 */
#define RC_TRX_REG19_ADDR   0x010500CC
#define RC_TRX_REG19_OFFSET 0x000000CC
#define RC_TRX_REG19_INDEX  0x00000033
#define RC_TRX_REG19_RESET  0x00000000

__INLINE uint32_t rc_trx_reg19_get(void)
{
    return REG_PL_RD(RC_TRX_REG19_ADDR);
}

__INLINE void rc_trx_reg19_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG19_ADDR, value);
}

// field definitions
#define RC_TRX_REG19_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG19_LSB    0
#define RC_TRX_REG19_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG19_RST    0x0

__INLINE uint32_t rc_trx_reg19_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG19_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg19_setf(uint32_t trxreg19)
{
    ASSERT_ERR((((uint32_t)trxreg19 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG19_ADDR, (uint32_t)trxreg19 << 0);
}

/**
 * @brief TRX_REG20 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG20   0x0
 * </pre>
 */
#define RC_TRX_REG20_ADDR   0x010500D0
#define RC_TRX_REG20_OFFSET 0x000000D0
#define RC_TRX_REG20_INDEX  0x00000034
#define RC_TRX_REG20_RESET  0x00000000

__INLINE uint32_t rc_trx_reg20_get(void)
{
    return REG_PL_RD(RC_TRX_REG20_ADDR);
}

__INLINE void rc_trx_reg20_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG20_ADDR, value);
}

// field definitions
#define RC_TRX_REG20_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG20_LSB    0
#define RC_TRX_REG20_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG20_RST    0x0

__INLINE uint32_t rc_trx_reg20_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG20_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg20_setf(uint32_t trxreg20)
{
    ASSERT_ERR((((uint32_t)trxreg20 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG20_ADDR, (uint32_t)trxreg20 << 0);
}

/**
 * @brief TRX_REG21 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG21   0x0
 * </pre>
 */
#define RC_TRX_REG21_ADDR   0x010500D4
#define RC_TRX_REG21_OFFSET 0x000000D4
#define RC_TRX_REG21_INDEX  0x00000035
#define RC_TRX_REG21_RESET  0x00000000

__INLINE uint32_t rc_trx_reg21_get(void)
{
    return REG_PL_RD(RC_TRX_REG21_ADDR);
}

__INLINE void rc_trx_reg21_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG21_ADDR, value);
}

// field definitions
#define RC_TRX_REG21_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG21_LSB    0
#define RC_TRX_REG21_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG21_RST    0x0

__INLINE uint32_t rc_trx_reg21_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG21_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg21_setf(uint32_t trxreg21)
{
    ASSERT_ERR((((uint32_t)trxreg21 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG21_ADDR, (uint32_t)trxreg21 << 0);
}

/**
 * @brief TRX_REG22 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG22   0x0
 * </pre>
 */
#define RC_TRX_REG22_ADDR   0x010500D8
#define RC_TRX_REG22_OFFSET 0x000000D8
#define RC_TRX_REG22_INDEX  0x00000036
#define RC_TRX_REG22_RESET  0x00000000

__INLINE uint32_t rc_trx_reg22_get(void)
{
    return REG_PL_RD(RC_TRX_REG22_ADDR);
}

__INLINE void rc_trx_reg22_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG22_ADDR, value);
}

// field definitions
#define RC_TRX_REG22_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG22_LSB    0
#define RC_TRX_REG22_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG22_RST    0x0

__INLINE uint32_t rc_trx_reg22_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG22_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg22_setf(uint32_t trxreg22)
{
    ASSERT_ERR((((uint32_t)trxreg22 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG22_ADDR, (uint32_t)trxreg22 << 0);
}

/**
 * @brief TRX_REG23 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG23   0x0
 * </pre>
 */
#define RC_TRX_REG23_ADDR   0x010500DC
#define RC_TRX_REG23_OFFSET 0x000000DC
#define RC_TRX_REG23_INDEX  0x00000037
#define RC_TRX_REG23_RESET  0x00000000

__INLINE uint32_t rc_trx_reg23_get(void)
{
    return REG_PL_RD(RC_TRX_REG23_ADDR);
}

__INLINE void rc_trx_reg23_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG23_ADDR, value);
}

// field definitions
#define RC_TRX_REG23_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG23_LSB    0
#define RC_TRX_REG23_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG23_RST    0x0

__INLINE uint32_t rc_trx_reg23_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG23_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg23_setf(uint32_t trxreg23)
{
    ASSERT_ERR((((uint32_t)trxreg23 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG23_ADDR, (uint32_t)trxreg23 << 0);
}

/**
 * @brief TRX_REG24 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG24   0x0
 * </pre>
 */
#define RC_TRX_REG24_ADDR   0x010500E0
#define RC_TRX_REG24_OFFSET 0x000000E0
#define RC_TRX_REG24_INDEX  0x00000038
#define RC_TRX_REG24_RESET  0x00000000

__INLINE uint32_t rc_trx_reg24_get(void)
{
    return REG_PL_RD(RC_TRX_REG24_ADDR);
}

__INLINE void rc_trx_reg24_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG24_ADDR, value);
}

// field definitions
#define RC_TRX_REG24_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG24_LSB    0
#define RC_TRX_REG24_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG24_RST    0x0

__INLINE uint32_t rc_trx_reg24_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG24_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg24_setf(uint32_t trxreg24)
{
    ASSERT_ERR((((uint32_t)trxreg24 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG24_ADDR, (uint32_t)trxreg24 << 0);
}

/**
 * @brief TRX_REG25 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG25   0x0
 * </pre>
 */
#define RC_TRX_REG25_ADDR   0x010500E4
#define RC_TRX_REG25_OFFSET 0x000000E4
#define RC_TRX_REG25_INDEX  0x00000039
#define RC_TRX_REG25_RESET  0x00000000

__INLINE uint32_t rc_trx_reg25_get(void)
{
    return REG_PL_RD(RC_TRX_REG25_ADDR);
}

__INLINE void rc_trx_reg25_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG25_ADDR, value);
}

// field definitions
#define RC_TRX_REG25_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG25_LSB    0
#define RC_TRX_REG25_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG25_RST    0x0

__INLINE uint32_t rc_trx_reg25_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG25_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg25_setf(uint32_t trxreg25)
{
    ASSERT_ERR((((uint32_t)trxreg25 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG25_ADDR, (uint32_t)trxreg25 << 0);
}

/**
 * @brief TRX_REG26 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG26   0x0
 * </pre>
 */
#define RC_TRX_REG26_ADDR   0x010500E8
#define RC_TRX_REG26_OFFSET 0x000000E8
#define RC_TRX_REG26_INDEX  0x0000003A
#define RC_TRX_REG26_RESET  0x00000000

__INLINE uint32_t rc_trx_reg26_get(void)
{
    return REG_PL_RD(RC_TRX_REG26_ADDR);
}

__INLINE void rc_trx_reg26_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG26_ADDR, value);
}

// field definitions
#define RC_TRX_REG26_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG26_LSB    0
#define RC_TRX_REG26_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG26_RST    0x0

__INLINE uint32_t rc_trx_reg26_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG26_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg26_setf(uint32_t trxreg26)
{
    ASSERT_ERR((((uint32_t)trxreg26 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG26_ADDR, (uint32_t)trxreg26 << 0);
}

/**
 * @brief TRX_REG27 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            TRX_REG27   0x0
 * </pre>
 */
#define RC_TRX_REG27_ADDR   0x010500EC
#define RC_TRX_REG27_OFFSET 0x000000EC
#define RC_TRX_REG27_INDEX  0x0000003B
#define RC_TRX_REG27_RESET  0x00000000

__INLINE uint32_t rc_trx_reg27_get(void)
{
    return REG_PL_RD(RC_TRX_REG27_ADDR);
}

__INLINE void rc_trx_reg27_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_REG27_ADDR, value);
}

// field definitions
#define RC_TRX_REG27_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_TRX_REG27_LSB    0
#define RC_TRX_REG27_WIDTH  ((uint32_t)0x00000020)

#define RC_TRX_REG27_RST    0x0

__INLINE uint32_t rc_trx_reg27_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_REG27_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_trx_reg27_setf(uint32_t trxreg27)
{
    ASSERT_ERR((((uint32_t)trxreg27 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_TRX_REG27_ADDR, (uint32_t)trxreg27 << 0);
}

/**
 * @brief RX_AVG_CALC register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31         RX_DC_CAL_EN   0
 *     30          RX_AVG_MODE   0
 *  23:12          RX_AVG_I_RD   0x0
 *  11:00          RX_AVG_Q_RD   0x0
 * </pre>
 */
#define RC_RX_AVG_CALC_ADDR   0x010500F0
#define RC_RX_AVG_CALC_OFFSET 0x000000F0
#define RC_RX_AVG_CALC_INDEX  0x0000003C
#define RC_RX_AVG_CALC_RESET  0x00000000

__INLINE uint32_t rc_rx_avg_calc_get(void)
{
    return REG_PL_RD(RC_RX_AVG_CALC_ADDR);
}

__INLINE void rc_rx_avg_calc_set(uint32_t value)
{
    REG_PL_WR(RC_RX_AVG_CALC_ADDR, value);
}

// field definitions
#define RC_RX_DC_CAL_EN_BIT    ((uint32_t)0x80000000)
#define RC_RX_DC_CAL_EN_POS    31
#define RC_RX_AVG_MODE_BIT     ((uint32_t)0x40000000)
#define RC_RX_AVG_MODE_POS     30
#define RC_RX_AVG_I_RD_MASK    ((uint32_t)0x00FFF000)
#define RC_RX_AVG_I_RD_LSB     12
#define RC_RX_AVG_I_RD_WIDTH   ((uint32_t)0x0000000C)
#define RC_RX_AVG_Q_RD_MASK    ((uint32_t)0x00000FFF)
#define RC_RX_AVG_Q_RD_LSB     0
#define RC_RX_AVG_Q_RD_WIDTH   ((uint32_t)0x0000000C)

#define RC_RX_DC_CAL_EN_RST    0x0
#define RC_RX_AVG_MODE_RST     0x0
#define RC_RX_AVG_I_RD_RST     0x0
#define RC_RX_AVG_Q_RD_RST     0x0

__INLINE void rc_rx_avg_calc_pack(uint8_t rxdccalen, uint8_t rxavgmode, uint16_t rxavgird, uint16_t rxavgqrd)
{
    ASSERT_ERR((((uint32_t)rxdccalen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)rxavgmode << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)rxavgird << 12) & ~((uint32_t)0x00FFF000)) == 0);
    ASSERT_ERR((((uint32_t)rxavgqrd << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(RC_RX_AVG_CALC_ADDR,  ((uint32_t)rxdccalen << 31) | ((uint32_t)rxavgmode << 30) | ((uint32_t)rxavgird << 12) | ((uint32_t)rxavgqrd << 0));
}

__INLINE void rc_rx_avg_calc_unpack(uint8_t *rxdccalen, uint8_t *rxavgmode, uint16_t *rxavgird, uint16_t *rxavgqrd)
{
    uint32_t localVal = REG_PL_RD(RC_RX_AVG_CALC_ADDR);

    *rxdccalen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *rxavgmode = (localVal & ((uint32_t)0x40000000)) >> 30;
    *rxavgird = (localVal & ((uint32_t)0x00FFF000)) >> 12;
    *rxavgqrd = (localVal & ((uint32_t)0x00000FFF)) >> 0;
}

__INLINE uint8_t rc_rx_dc_cal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_AVG_CALC_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void rc_rx_dc_cal_en_setf(uint8_t rxdccalen)
{
    ASSERT_ERR((((uint32_t)rxdccalen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(RC_RX_AVG_CALC_ADDR, (REG_PL_RD(RC_RX_AVG_CALC_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)rxdccalen << 31));
}

__INLINE uint8_t rc_rx_avg_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_AVG_CALC_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

__INLINE void rc_rx_avg_mode_setf(uint8_t rxavgmode)
{
    ASSERT_ERR((((uint32_t)rxavgmode << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(RC_RX_AVG_CALC_ADDR, (REG_PL_RD(RC_RX_AVG_CALC_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)rxavgmode << 30));
}

__INLINE uint16_t rc_rx_avg_i_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_AVG_CALC_ADDR);
    return ((localVal & ((uint32_t)0x00FFF000)) >> 12);
}

__INLINE void rc_rx_avg_i_rd_setf(uint16_t rxavgird)
{
    ASSERT_ERR((((uint32_t)rxavgird << 12) & ~((uint32_t)0x00FFF000)) == 0);
    REG_PL_WR(RC_RX_AVG_CALC_ADDR, (REG_PL_RD(RC_RX_AVG_CALC_ADDR) & ~((uint32_t)0x00FFF000)) | ((uint32_t)rxavgird << 12));
}

__INLINE uint16_t rc_rx_avg_q_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_AVG_CALC_ADDR);
    return ((localVal & ((uint32_t)0x00000FFF)) >> 0);
}

__INLINE void rc_rx_avg_q_rd_setf(uint16_t rxavgqrd)
{
    ASSERT_ERR((((uint32_t)rxavgqrd << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(RC_RX_AVG_CALC_ADDR, (REG_PL_RD(RC_RX_AVG_CALC_ADDR) & ~((uint32_t)0x00000FFF)) | ((uint32_t)rxavgqrd << 0));
}

/**
 * @brief RX_CALIB_EN register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31            RX_CAL_EN   0
 *     30           RX_COMP_EN   0
 *  23:12           RX_DC_I_RD   0x0
 *  11:00           RX_DC_Q_RD   0x0
 * </pre>
 */
#define RC_RX_CALIB_EN_ADDR   0x010500F8
#define RC_RX_CALIB_EN_OFFSET 0x000000F8
#define RC_RX_CALIB_EN_INDEX  0x0000003E
#define RC_RX_CALIB_EN_RESET  0x00000000

__INLINE uint32_t rc_rx_calib_en_get(void)
{
    return REG_PL_RD(RC_RX_CALIB_EN_ADDR);
}

__INLINE void rc_rx_calib_en_set(uint32_t value)
{
    REG_PL_WR(RC_RX_CALIB_EN_ADDR, value);
}

// field definitions
#define RC_RX_CAL_EN_BIT     ((uint32_t)0x80000000)
#define RC_RX_CAL_EN_POS     31
#define RC_RX_COMP_EN_BIT    ((uint32_t)0x40000000)
#define RC_RX_COMP_EN_POS    30
#define RC_RX_DC_I_RD_MASK   ((uint32_t)0x00FFF000)
#define RC_RX_DC_I_RD_LSB    12
#define RC_RX_DC_I_RD_WIDTH  ((uint32_t)0x0000000C)
#define RC_RX_DC_Q_RD_MASK   ((uint32_t)0x00000FFF)
#define RC_RX_DC_Q_RD_LSB    0
#define RC_RX_DC_Q_RD_WIDTH  ((uint32_t)0x0000000C)

#define RC_RX_CAL_EN_RST     0x0
#define RC_RX_COMP_EN_RST    0x0
#define RC_RX_DC_I_RD_RST    0x0
#define RC_RX_DC_Q_RD_RST    0x0

__INLINE void rc_rx_calib_en_pack(uint8_t rxcalen, uint8_t rxcompen, uint16_t rxdcird, uint16_t rxdcqrd)
{
    ASSERT_ERR((((uint32_t)rxcalen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)rxcompen << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)rxdcird << 12) & ~((uint32_t)0x00FFF000)) == 0);
    ASSERT_ERR((((uint32_t)rxdcqrd << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(RC_RX_CALIB_EN_ADDR,  ((uint32_t)rxcalen << 31) | ((uint32_t)rxcompen << 30) | ((uint32_t)rxdcird << 12) | ((uint32_t)rxdcqrd << 0));
}

__INLINE void rc_rx_calib_en_unpack(uint8_t *rxcalen, uint8_t *rxcompen, uint16_t *rxdcird, uint16_t *rxdcqrd)
{
    uint32_t localVal = REG_PL_RD(RC_RX_CALIB_EN_ADDR);

    *rxcalen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *rxcompen = (localVal & ((uint32_t)0x40000000)) >> 30;
    *rxdcird = (localVal & ((uint32_t)0x00FFF000)) >> 12;
    *rxdcqrd = (localVal & ((uint32_t)0x00000FFF)) >> 0;
}

__INLINE uint8_t rc_rx_cal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_CALIB_EN_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void rc_rx_cal_en_setf(uint8_t rxcalen)
{
    ASSERT_ERR((((uint32_t)rxcalen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(RC_RX_CALIB_EN_ADDR, (REG_PL_RD(RC_RX_CALIB_EN_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)rxcalen << 31));
}

__INLINE uint8_t rc_rx_comp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_CALIB_EN_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

__INLINE void rc_rx_comp_en_setf(uint8_t rxcompen)
{
    ASSERT_ERR((((uint32_t)rxcompen << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(RC_RX_CALIB_EN_ADDR, (REG_PL_RD(RC_RX_CALIB_EN_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)rxcompen << 30));
}

__INLINE uint16_t rc_rx_dc_i_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_CALIB_EN_ADDR);
    return ((localVal & ((uint32_t)0x00FFF000)) >> 12);
}

__INLINE void rc_rx_dc_i_rd_setf(uint16_t rxdcird)
{
    ASSERT_ERR((((uint32_t)rxdcird << 12) & ~((uint32_t)0x00FFF000)) == 0);
    REG_PL_WR(RC_RX_CALIB_EN_ADDR, (REG_PL_RD(RC_RX_CALIB_EN_ADDR) & ~((uint32_t)0x00FFF000)) | ((uint32_t)rxdcird << 12));
}

__INLINE uint16_t rc_rx_dc_q_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_CALIB_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000FFF)) >> 0);
}

__INLINE void rc_rx_dc_q_rd_setf(uint16_t rxdcqrd)
{
    ASSERT_ERR((((uint32_t)rxdcqrd << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(RC_RX_CALIB_EN_ADDR, (REG_PL_RD(RC_RX_CALIB_EN_ADDR) & ~((uint32_t)0x00000FFF)) | ((uint32_t)rxdcqrd << 0));
}

/**
 * @brief RX_ERROR_RD register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:16        RX_AMP_ERR_RD   0x0
 *  09:00      RX_PHASE_ERR_RD   0x0
 * </pre>
 */
#define RC_RX_ERROR_RD_ADDR   0x010500FC
#define RC_RX_ERROR_RD_OFFSET 0x000000FC
#define RC_RX_ERROR_RD_INDEX  0x0000003F
#define RC_RX_ERROR_RD_RESET  0x00000000

__INLINE uint32_t rc_rx_error_rd_get(void)
{
    return REG_PL_RD(RC_RX_ERROR_RD_ADDR);
}

// field definitions
#define RC_RX_AMP_ERR_RD_MASK     ((uint32_t)0x03FF0000)
#define RC_RX_AMP_ERR_RD_LSB      16
#define RC_RX_AMP_ERR_RD_WIDTH    ((uint32_t)0x0000000A)
#define RC_RX_PHASE_ERR_RD_MASK   ((uint32_t)0x000003FF)
#define RC_RX_PHASE_ERR_RD_LSB    0
#define RC_RX_PHASE_ERR_RD_WIDTH  ((uint32_t)0x0000000A)

#define RC_RX_AMP_ERR_RD_RST      0x0
#define RC_RX_PHASE_ERR_RD_RST    0x0

__INLINE void rc_rx_error_rd_unpack(uint16_t *rxamperrrd, uint16_t *rxphaseerrrd)
{
    uint32_t localVal = REG_PL_RD(RC_RX_ERROR_RD_ADDR);

    *rxamperrrd = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *rxphaseerrrd = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint16_t rc_rx_amp_err_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_ERROR_RD_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

__INLINE uint16_t rc_rx_phase_err_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_ERROR_RD_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief RX_TY2_RD register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  09:00            RX_TY2_RD   0x0
 * </pre>
 */
#define RC_RX_TY2_RD_ADDR   0x01050100
#define RC_RX_TY2_RD_OFFSET 0x00000100
#define RC_RX_TY2_RD_INDEX  0x00000040
#define RC_RX_TY2_RD_RESET  0x00000000

__INLINE uint32_t rc_rx_ty2_rd_get(void)
{
    return REG_PL_RD(RC_RX_TY2_RD_ADDR);
}

// field definitions
#define RC_RX_TY2_RD_MASK   ((uint32_t)0x000003FF)
#define RC_RX_TY2_RD_LSB    0
#define RC_RX_TY2_RD_WIDTH  ((uint32_t)0x0000000A)

#define RC_RX_TY2_RD_RST    0x0

__INLINE uint16_t rc_rx_ty2_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_TY2_RD_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief RX_DC_WR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:12           RX_DC_I_WR   0x800
 *  11:00           RX_DC_Q_WR   0x0
 * </pre>
 */
#define RC_RX_DC_WR_ADDR   0x01050104
#define RC_RX_DC_WR_OFFSET 0x00000104
#define RC_RX_DC_WR_INDEX  0x00000041
#define RC_RX_DC_WR_RESET  0x00800000

__INLINE uint32_t rc_rx_dc_wr_get(void)
{
    return REG_PL_RD(RC_RX_DC_WR_ADDR);
}

__INLINE void rc_rx_dc_wr_set(uint32_t value)
{
    REG_PL_WR(RC_RX_DC_WR_ADDR, value);
}

// field definitions
#define RC_RX_DC_I_WR_MASK   ((uint32_t)0x00FFF000)
#define RC_RX_DC_I_WR_LSB    12
#define RC_RX_DC_I_WR_WIDTH  ((uint32_t)0x0000000C)
#define RC_RX_DC_Q_WR_MASK   ((uint32_t)0x00000FFF)
#define RC_RX_DC_Q_WR_LSB    0
#define RC_RX_DC_Q_WR_WIDTH  ((uint32_t)0x0000000C)

#define RC_RX_DC_I_WR_RST    0x800
#define RC_RX_DC_Q_WR_RST    0x0

__INLINE void rc_rx_dc_wr_pack(uint16_t rxdciwr, uint16_t rxdcqwr)
{
    ASSERT_ERR((((uint32_t)rxdciwr << 12) & ~((uint32_t)0x00FFF000)) == 0);
    ASSERT_ERR((((uint32_t)rxdcqwr << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(RC_RX_DC_WR_ADDR,  ((uint32_t)rxdciwr << 12) | ((uint32_t)rxdcqwr << 0));
}

__INLINE void rc_rx_dc_wr_unpack(uint16_t *rxdciwr, uint16_t *rxdcqwr)
{
    uint32_t localVal = REG_PL_RD(RC_RX_DC_WR_ADDR);

    *rxdciwr = (localVal & ((uint32_t)0x00FFF000)) >> 12;
    *rxdcqwr = (localVal & ((uint32_t)0x00000FFF)) >> 0;
}

__INLINE uint16_t rc_rx_dc_i_wr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_DC_WR_ADDR);
    return ((localVal & ((uint32_t)0x00FFF000)) >> 12);
}

__INLINE void rc_rx_dc_i_wr_setf(uint16_t rxdciwr)
{
    ASSERT_ERR((((uint32_t)rxdciwr << 12) & ~((uint32_t)0x00FFF000)) == 0);
    REG_PL_WR(RC_RX_DC_WR_ADDR, (REG_PL_RD(RC_RX_DC_WR_ADDR) & ~((uint32_t)0x00FFF000)) | ((uint32_t)rxdciwr << 12));
}

__INLINE uint16_t rc_rx_dc_q_wr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_DC_WR_ADDR);
    return ((localVal & ((uint32_t)0x00000FFF)) >> 0);
}

__INLINE void rc_rx_dc_q_wr_setf(uint16_t rxdcqwr)
{
    ASSERT_ERR((((uint32_t)rxdcqwr << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(RC_RX_DC_WR_ADDR, (REG_PL_RD(RC_RX_DC_WR_ADDR) & ~((uint32_t)0x00000FFF)) | ((uint32_t)rxdcqwr << 0));
}

/**
 * @brief RX_ERROR_WR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:16        RX_AMP_ERR_WR   0x200
 *  09:00      RX_PHASE_ERR_WR   0x200
 * </pre>
 */
#define RC_RX_ERROR_WR_ADDR   0x01050108
#define RC_RX_ERROR_WR_OFFSET 0x00000108
#define RC_RX_ERROR_WR_INDEX  0x00000042
#define RC_RX_ERROR_WR_RESET  0x02000200

__INLINE uint32_t rc_rx_error_wr_get(void)
{
    return REG_PL_RD(RC_RX_ERROR_WR_ADDR);
}

__INLINE void rc_rx_error_wr_set(uint32_t value)
{
    REG_PL_WR(RC_RX_ERROR_WR_ADDR, value);
}

// field definitions
#define RC_RX_AMP_ERR_WR_MASK     ((uint32_t)0x03FF0000)
#define RC_RX_AMP_ERR_WR_LSB      16
#define RC_RX_AMP_ERR_WR_WIDTH    ((uint32_t)0x0000000A)
#define RC_RX_PHASE_ERR_WR_MASK   ((uint32_t)0x000003FF)
#define RC_RX_PHASE_ERR_WR_LSB    0
#define RC_RX_PHASE_ERR_WR_WIDTH  ((uint32_t)0x0000000A)

#define RC_RX_AMP_ERR_WR_RST      0x200
#define RC_RX_PHASE_ERR_WR_RST    0x200

__INLINE void rc_rx_error_wr_pack(uint16_t rxamperrwr, uint16_t rxphaseerrwr)
{
    ASSERT_ERR((((uint32_t)rxamperrwr << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxphaseerrwr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_RX_ERROR_WR_ADDR,  ((uint32_t)rxamperrwr << 16) | ((uint32_t)rxphaseerrwr << 0));
}

__INLINE void rc_rx_error_wr_unpack(uint16_t *rxamperrwr, uint16_t *rxphaseerrwr)
{
    uint32_t localVal = REG_PL_RD(RC_RX_ERROR_WR_ADDR);

    *rxamperrwr = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *rxphaseerrwr = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint16_t rc_rx_amp_err_wr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_ERROR_WR_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

__INLINE void rc_rx_amp_err_wr_setf(uint16_t rxamperrwr)
{
    ASSERT_ERR((((uint32_t)rxamperrwr << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_RX_ERROR_WR_ADDR, (REG_PL_RD(RC_RX_ERROR_WR_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)rxamperrwr << 16));
}

__INLINE uint16_t rc_rx_phase_err_wr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_RX_ERROR_WR_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

__INLINE void rc_rx_phase_err_wr_setf(uint16_t rxphaseerrwr)
{
    ASSERT_ERR((((uint32_t)rxphaseerrwr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_RX_ERROR_WR_ADDR, (REG_PL_RD(RC_RX_ERROR_WR_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)rxphaseerrwr << 0));
}

/**
 * @brief TX_MODE_CFG register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:30         TEST_PATTERN   0x0
 *  25:16              I_CONST   0x200
 *  09:00              Q_CONST   0x200
 * </pre>
 */
#define RC_TX_MODE_CFG_ADDR   0x01050130
#define RC_TX_MODE_CFG_OFFSET 0x00000130
#define RC_TX_MODE_CFG_INDEX  0x0000004C
#define RC_TX_MODE_CFG_RESET  0x02000200

__INLINE uint32_t rc_tx_mode_cfg_get(void)
{
    return REG_PL_RD(RC_TX_MODE_CFG_ADDR);
}

__INLINE void rc_tx_mode_cfg_set(uint32_t value)
{
    REG_PL_WR(RC_TX_MODE_CFG_ADDR, value);
}

// field definitions
#define RC_TEST_PATTERN_MASK   ((uint32_t)0xC0000000)
#define RC_TEST_PATTERN_LSB    30
#define RC_TEST_PATTERN_WIDTH  ((uint32_t)0x00000002)
#define RC_I_CONST_MASK        ((uint32_t)0x03FF0000)
#define RC_I_CONST_LSB         16
#define RC_I_CONST_WIDTH       ((uint32_t)0x0000000A)
#define RC_Q_CONST_MASK        ((uint32_t)0x000003FF)
#define RC_Q_CONST_LSB         0
#define RC_Q_CONST_WIDTH       ((uint32_t)0x0000000A)

#define RC_TEST_PATTERN_RST    0x0
#define RC_I_CONST_RST         0x200
#define RC_Q_CONST_RST         0x200

__INLINE void rc_tx_mode_cfg_pack(uint8_t testpattern, uint16_t iconst, uint16_t qconst)
{
    ASSERT_ERR((((uint32_t)testpattern << 30) & ~((uint32_t)0xC0000000)) == 0);
    ASSERT_ERR((((uint32_t)iconst << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)qconst << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_MODE_CFG_ADDR,  ((uint32_t)testpattern << 30) | ((uint32_t)iconst << 16) | ((uint32_t)qconst << 0));
}

__INLINE void rc_tx_mode_cfg_unpack(uint8_t *testpattern, uint16_t *iconst, uint16_t *qconst)
{
    uint32_t localVal = REG_PL_RD(RC_TX_MODE_CFG_ADDR);

    *testpattern = (localVal & ((uint32_t)0xC0000000)) >> 30;
    *iconst = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *qconst = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint8_t rc_test_pattern_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_MODE_CFG_ADDR);
    return ((localVal & ((uint32_t)0xC0000000)) >> 30);
}

__INLINE void rc_test_pattern_setf(uint8_t testpattern)
{
    ASSERT_ERR((((uint32_t)testpattern << 30) & ~((uint32_t)0xC0000000)) == 0);
    REG_PL_WR(RC_TX_MODE_CFG_ADDR, (REG_PL_RD(RC_TX_MODE_CFG_ADDR) & ~((uint32_t)0xC0000000)) | ((uint32_t)testpattern << 30));
}

__INLINE uint16_t rc_i_const_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_MODE_CFG_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

__INLINE void rc_i_const_setf(uint16_t iconst)
{
    ASSERT_ERR((((uint32_t)iconst << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_TX_MODE_CFG_ADDR, (REG_PL_RD(RC_TX_MODE_CFG_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)iconst << 16));
}

__INLINE uint16_t rc_q_const_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_MODE_CFG_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

__INLINE void rc_q_const_setf(uint16_t qconst)
{
    ASSERT_ERR((((uint32_t)qconst << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_MODE_CFG_ADDR, (REG_PL_RD(RC_TX_MODE_CFG_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)qconst << 0));
}

/**
 * @brief TX_SIN_CFG register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:22             TX_SIN_F   0x0
 *  17:16          TX_SIN_MODE   0x0
 *  15:12           TX_SIN_AMP   0x0
 * </pre>
 */
#define RC_TX_SIN_CFG_ADDR   0x01050134
#define RC_TX_SIN_CFG_OFFSET 0x00000134
#define RC_TX_SIN_CFG_INDEX  0x0000004D
#define RC_TX_SIN_CFG_RESET  0x00000000

__INLINE uint32_t rc_tx_sin_cfg_get(void)
{
    return REG_PL_RD(RC_TX_SIN_CFG_ADDR);
}

__INLINE void rc_tx_sin_cfg_set(uint32_t value)
{
    REG_PL_WR(RC_TX_SIN_CFG_ADDR, value);
}

// field definitions
#define RC_TX_SIN_F_MASK      ((uint32_t)0xFFC00000)
#define RC_TX_SIN_F_LSB       22
#define RC_TX_SIN_F_WIDTH     ((uint32_t)0x0000000A)
#define RC_TX_SIN_MODE_MASK   ((uint32_t)0x00030000)
#define RC_TX_SIN_MODE_LSB    16
#define RC_TX_SIN_MODE_WIDTH  ((uint32_t)0x00000002)
#define RC_TX_SIN_AMP_MASK    ((uint32_t)0x0000F000)
#define RC_TX_SIN_AMP_LSB     12
#define RC_TX_SIN_AMP_WIDTH   ((uint32_t)0x00000004)

#define RC_TX_SIN_F_RST       0x0
#define RC_TX_SIN_MODE_RST    0x0
#define RC_TX_SIN_AMP_RST     0x0

__INLINE void rc_tx_sin_cfg_pack(uint16_t txsinf, uint8_t txsinmode, uint8_t txsinamp)
{
    ASSERT_ERR((((uint32_t)txsinf << 22) & ~((uint32_t)0xFFC00000)) == 0);
    ASSERT_ERR((((uint32_t)txsinmode << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)txsinamp << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(RC_TX_SIN_CFG_ADDR,  ((uint32_t)txsinf << 22) | ((uint32_t)txsinmode << 16) | ((uint32_t)txsinamp << 12));
}

__INLINE void rc_tx_sin_cfg_unpack(uint16_t *txsinf, uint8_t *txsinmode, uint8_t *txsinamp)
{
    uint32_t localVal = REG_PL_RD(RC_TX_SIN_CFG_ADDR);

    *txsinf = (localVal & ((uint32_t)0xFFC00000)) >> 22;
    *txsinmode = (localVal & ((uint32_t)0x00030000)) >> 16;
    *txsinamp = (localVal & ((uint32_t)0x0000F000)) >> 12;
}

__INLINE uint16_t rc_tx_sin_f_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_SIN_CFG_ADDR);
    return ((localVal & ((uint32_t)0xFFC00000)) >> 22);
}

__INLINE void rc_tx_sin_f_setf(uint16_t txsinf)
{
    ASSERT_ERR((((uint32_t)txsinf << 22) & ~((uint32_t)0xFFC00000)) == 0);
    REG_PL_WR(RC_TX_SIN_CFG_ADDR, (REG_PL_RD(RC_TX_SIN_CFG_ADDR) & ~((uint32_t)0xFFC00000)) | ((uint32_t)txsinf << 22));
}

__INLINE uint8_t rc_tx_sin_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_SIN_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

__INLINE void rc_tx_sin_mode_setf(uint8_t txsinmode)
{
    ASSERT_ERR((((uint32_t)txsinmode << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(RC_TX_SIN_CFG_ADDR, (REG_PL_RD(RC_TX_SIN_CFG_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)txsinmode << 16));
}

__INLINE uint8_t rc_tx_sin_amp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_SIN_CFG_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

__INLINE void rc_tx_sin_amp_setf(uint8_t txsinamp)
{
    ASSERT_ERR((((uint32_t)txsinamp << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(RC_TX_SIN_CFG_ADDR, (REG_PL_RD(RC_TX_SIN_CFG_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)txsinamp << 12));
}

/**
 * @brief TX_DC_COMP register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:16         TX_I_DC_COMP   0x200
 *  09:00         TX_Q_DC_COMP   0x200
 * </pre>
 */
#define RC_TX_DC_COMP_ADDR   0x0105013C
#define RC_TX_DC_COMP_OFFSET 0x0000013C
#define RC_TX_DC_COMP_INDEX  0x0000004F
#define RC_TX_DC_COMP_RESET  0x02000200

__INLINE uint32_t rc_tx_dc_comp_get(void)
{
    return REG_PL_RD(RC_TX_DC_COMP_ADDR);
}

__INLINE void rc_tx_dc_comp_set(uint32_t value)
{
    REG_PL_WR(RC_TX_DC_COMP_ADDR, value);
}

// field definitions
#define RC_TX_I_DC_COMP_MASK   ((uint32_t)0x03FF0000)
#define RC_TX_I_DC_COMP_LSB    16
#define RC_TX_I_DC_COMP_WIDTH  ((uint32_t)0x0000000A)
#define RC_TX_Q_DC_COMP_MASK   ((uint32_t)0x000003FF)
#define RC_TX_Q_DC_COMP_LSB    0
#define RC_TX_Q_DC_COMP_WIDTH  ((uint32_t)0x0000000A)

#define RC_TX_I_DC_COMP_RST    0x200
#define RC_TX_Q_DC_COMP_RST    0x200

__INLINE void rc_tx_dc_comp_pack(uint16_t txidccomp, uint16_t txqdccomp)
{
    ASSERT_ERR((((uint32_t)txidccomp << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)txqdccomp << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_DC_COMP_ADDR,  ((uint32_t)txidccomp << 16) | ((uint32_t)txqdccomp << 0));
}

__INLINE void rc_tx_dc_comp_unpack(uint16_t *txidccomp, uint16_t *txqdccomp)
{
    uint32_t localVal = REG_PL_RD(RC_TX_DC_COMP_ADDR);

    *txidccomp = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *txqdccomp = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint16_t rc_tx_i_dc_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_DC_COMP_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

__INLINE void rc_tx_i_dc_comp_setf(uint16_t txidccomp)
{
    ASSERT_ERR((((uint32_t)txidccomp << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_TX_DC_COMP_ADDR, (REG_PL_RD(RC_TX_DC_COMP_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)txidccomp << 16));
}

__INLINE uint16_t rc_tx_q_dc_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_DC_COMP_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

__INLINE void rc_tx_q_dc_comp_setf(uint16_t txqdccomp)
{
    ASSERT_ERR((((uint32_t)txqdccomp << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_DC_COMP_ADDR, (REG_PL_RD(RC_TX_DC_COMP_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)txqdccomp << 0));
}

/**
 * @brief TX_GAIN_COMP register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:16       TX_I_GAIN_COMP   0x3FF
 *  09:00       TX_Q_GAIN_COMP   0x3FF
 * </pre>
 */
#define RC_TX_GAIN_COMP_ADDR   0x01050140
#define RC_TX_GAIN_COMP_OFFSET 0x00000140
#define RC_TX_GAIN_COMP_INDEX  0x00000050
#define RC_TX_GAIN_COMP_RESET  0x03FF03FF

__INLINE uint32_t rc_tx_gain_comp_get(void)
{
    return REG_PL_RD(RC_TX_GAIN_COMP_ADDR);
}

__INLINE void rc_tx_gain_comp_set(uint32_t value)
{
    REG_PL_WR(RC_TX_GAIN_COMP_ADDR, value);
}

// field definitions
#define RC_TX_I_GAIN_COMP_MASK   ((uint32_t)0x03FF0000)
#define RC_TX_I_GAIN_COMP_LSB    16
#define RC_TX_I_GAIN_COMP_WIDTH  ((uint32_t)0x0000000A)
#define RC_TX_Q_GAIN_COMP_MASK   ((uint32_t)0x000003FF)
#define RC_TX_Q_GAIN_COMP_LSB    0
#define RC_TX_Q_GAIN_COMP_WIDTH  ((uint32_t)0x0000000A)

#define RC_TX_I_GAIN_COMP_RST    0x3FF
#define RC_TX_Q_GAIN_COMP_RST    0x3FF

__INLINE void rc_tx_gain_comp_pack(uint16_t txigaincomp, uint16_t txqgaincomp)
{
    ASSERT_ERR((((uint32_t)txigaincomp << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)txqgaincomp << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_GAIN_COMP_ADDR,  ((uint32_t)txigaincomp << 16) | ((uint32_t)txqgaincomp << 0));
}

__INLINE void rc_tx_gain_comp_unpack(uint16_t *txigaincomp, uint16_t *txqgaincomp)
{
    uint32_t localVal = REG_PL_RD(RC_TX_GAIN_COMP_ADDR);

    *txigaincomp = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *txqgaincomp = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint16_t rc_tx_i_gain_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_GAIN_COMP_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

__INLINE void rc_tx_i_gain_comp_setf(uint16_t txigaincomp)
{
    ASSERT_ERR((((uint32_t)txigaincomp << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_TX_GAIN_COMP_ADDR, (REG_PL_RD(RC_TX_GAIN_COMP_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)txigaincomp << 16));
}

__INLINE uint16_t rc_tx_q_gain_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_GAIN_COMP_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

__INLINE void rc_tx_q_gain_comp_setf(uint16_t txqgaincomp)
{
    ASSERT_ERR((((uint32_t)txqgaincomp << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_GAIN_COMP_ADDR, (REG_PL_RD(RC_TX_GAIN_COMP_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)txqgaincomp << 0));
}

/**
 * @brief TX_PHASE_TY2_COMP register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:16        TX_PHASE_COMP   0x200
 *  09:00          TX_TY2_COMP   0x200
 * </pre>
 */
#define RC_TX_PHASE_TY2_COMP_ADDR   0x01050144
#define RC_TX_PHASE_TY2_COMP_OFFSET 0x00000144
#define RC_TX_PHASE_TY2_COMP_INDEX  0x00000051
#define RC_TX_PHASE_TY2_COMP_RESET  0x02000200

__INLINE uint32_t rc_tx_phase_ty2_comp_get(void)
{
    return REG_PL_RD(RC_TX_PHASE_TY2_COMP_ADDR);
}

__INLINE void rc_tx_phase_ty2_comp_set(uint32_t value)
{
    REG_PL_WR(RC_TX_PHASE_TY2_COMP_ADDR, value);
}

// field definitions
#define RC_TX_PHASE_COMP_MASK   ((uint32_t)0x03FF0000)
#define RC_TX_PHASE_COMP_LSB    16
#define RC_TX_PHASE_COMP_WIDTH  ((uint32_t)0x0000000A)
#define RC_TX_TY2_COMP_MASK     ((uint32_t)0x000003FF)
#define RC_TX_TY2_COMP_LSB      0
#define RC_TX_TY2_COMP_WIDTH    ((uint32_t)0x0000000A)

#define RC_TX_PHASE_COMP_RST    0x200
#define RC_TX_TY2_COMP_RST      0x200

__INLINE void rc_tx_phase_ty2_comp_pack(uint16_t txphasecomp, uint16_t txty2comp)
{
    ASSERT_ERR((((uint32_t)txphasecomp << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)txty2comp << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_PHASE_TY2_COMP_ADDR,  ((uint32_t)txphasecomp << 16) | ((uint32_t)txty2comp << 0));
}

__INLINE void rc_tx_phase_ty2_comp_unpack(uint16_t *txphasecomp, uint16_t *txty2comp)
{
    uint32_t localVal = REG_PL_RD(RC_TX_PHASE_TY2_COMP_ADDR);

    *txphasecomp = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *txty2comp = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint16_t rc_tx_phase_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_PHASE_TY2_COMP_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

__INLINE void rc_tx_phase_comp_setf(uint16_t txphasecomp)
{
    ASSERT_ERR((((uint32_t)txphasecomp << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(RC_TX_PHASE_TY2_COMP_ADDR, (REG_PL_RD(RC_TX_PHASE_TY2_COMP_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)txphasecomp << 16));
}

__INLINE uint16_t rc_tx_ty2_comp_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_PHASE_TY2_COMP_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

__INLINE void rc_tx_ty2_comp_setf(uint16_t txty2comp)
{
    ASSERT_ERR((((uint32_t)txty2comp << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TX_PHASE_TY2_COMP_ADDR, (REG_PL_RD(RC_TX_PHASE_TY2_COMP_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)txty2comp << 0));
}

/**
 * @brief TX_OTHER_CFG register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31           TX_IQ_SWAP   0
 *  30:21     IQ_CONSTANT_POUT   0x0
 *  19:16          TX_PRE_GAIN   0x0
 *  15:06   IQ_CONSTANT_IQCAL_P   0x0
 * </pre>
 */
#define RC_TX_OTHER_CFG_ADDR   0x01050148
#define RC_TX_OTHER_CFG_OFFSET 0x00000148
#define RC_TX_OTHER_CFG_INDEX  0x00000052
#define RC_TX_OTHER_CFG_RESET  0x00000000

__INLINE uint32_t rc_tx_other_cfg_get(void)
{
    return REG_PL_RD(RC_TX_OTHER_CFG_ADDR);
}

__INLINE void rc_tx_other_cfg_set(uint32_t value)
{
    REG_PL_WR(RC_TX_OTHER_CFG_ADDR, value);
}

// field definitions
#define RC_TX_IQ_SWAP_BIT             ((uint32_t)0x80000000)
#define RC_TX_IQ_SWAP_POS             31
#define RC_IQ_CONSTANT_POUT_MASK      ((uint32_t)0x7FE00000)
#define RC_IQ_CONSTANT_POUT_LSB       21
#define RC_IQ_CONSTANT_POUT_WIDTH     ((uint32_t)0x0000000A)
#define RC_TX_PRE_GAIN_MASK           ((uint32_t)0x000F0000)
#define RC_TX_PRE_GAIN_LSB            16
#define RC_TX_PRE_GAIN_WIDTH          ((uint32_t)0x00000004)
#define RC_IQ_CONSTANT_IQCAL_P_MASK   ((uint32_t)0x0000FFC0)
#define RC_IQ_CONSTANT_IQCAL_P_LSB    6
#define RC_IQ_CONSTANT_IQCAL_P_WIDTH  ((uint32_t)0x0000000A)

#define RC_TX_IQ_SWAP_RST             0x0
#define RC_IQ_CONSTANT_POUT_RST       0x0
#define RC_TX_PRE_GAIN_RST            0x0
#define RC_IQ_CONSTANT_IQCAL_P_RST    0x0

__INLINE void rc_tx_other_cfg_pack(uint8_t txiqswap, uint16_t iqconstantpout, uint8_t txpregain, uint16_t iqconstantiqcalp)
{
    ASSERT_ERR((((uint32_t)txiqswap << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)iqconstantpout << 21) & ~((uint32_t)0x7FE00000)) == 0);
    ASSERT_ERR((((uint32_t)txpregain << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)iqconstantiqcalp << 6) & ~((uint32_t)0x0000FFC0)) == 0);
    REG_PL_WR(RC_TX_OTHER_CFG_ADDR,  ((uint32_t)txiqswap << 31) | ((uint32_t)iqconstantpout << 21) | ((uint32_t)txpregain << 16) | ((uint32_t)iqconstantiqcalp << 6));
}

__INLINE void rc_tx_other_cfg_unpack(uint8_t *txiqswap, uint16_t *iqconstantpout, uint8_t *txpregain, uint16_t *iqconstantiqcalp)
{
    uint32_t localVal = REG_PL_RD(RC_TX_OTHER_CFG_ADDR);

    *txiqswap = (localVal & ((uint32_t)0x80000000)) >> 31;
    *iqconstantpout = (localVal & ((uint32_t)0x7FE00000)) >> 21;
    *txpregain = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *iqconstantiqcalp = (localVal & ((uint32_t)0x0000FFC0)) >> 6;
}

__INLINE uint8_t rc_tx_iq_swap_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_OTHER_CFG_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void rc_tx_iq_swap_setf(uint8_t txiqswap)
{
    ASSERT_ERR((((uint32_t)txiqswap << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(RC_TX_OTHER_CFG_ADDR, (REG_PL_RD(RC_TX_OTHER_CFG_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)txiqswap << 31));
}

__INLINE uint16_t rc_iq_constant_pout_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_OTHER_CFG_ADDR);
    return ((localVal & ((uint32_t)0x7FE00000)) >> 21);
}

__INLINE void rc_iq_constant_pout_setf(uint16_t iqconstantpout)
{
    ASSERT_ERR((((uint32_t)iqconstantpout << 21) & ~((uint32_t)0x7FE00000)) == 0);
    REG_PL_WR(RC_TX_OTHER_CFG_ADDR, (REG_PL_RD(RC_TX_OTHER_CFG_ADDR) & ~((uint32_t)0x7FE00000)) | ((uint32_t)iqconstantpout << 21));
}

__INLINE uint8_t rc_tx_pre_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_OTHER_CFG_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

__INLINE void rc_tx_pre_gain_setf(uint8_t txpregain)
{
    ASSERT_ERR((((uint32_t)txpregain << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(RC_TX_OTHER_CFG_ADDR, (REG_PL_RD(RC_TX_OTHER_CFG_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)txpregain << 16));
}

__INLINE uint16_t rc_iq_constant_iqcal_p_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TX_OTHER_CFG_ADDR);
    return ((localVal & ((uint32_t)0x0000FFC0)) >> 6);
}

__INLINE void rc_iq_constant_iqcal_p_setf(uint16_t iqconstantiqcalp)
{
    ASSERT_ERR((((uint32_t)iqconstantiqcalp << 6) & ~((uint32_t)0x0000FFC0)) == 0);
    REG_PL_WR(RC_TX_OTHER_CFG_ADDR, (REG_PL_RD(RC_TX_OTHER_CFG_ADDR) & ~((uint32_t)0x0000FFC0)) | ((uint32_t)iqconstantiqcalp << 6));
}

/**
 * @brief AGC_CFG register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31         AGC_MAP_MODE   0
 *  30:29           AGC_LNA_RD   0x0
 *     28           AGC_BUF_RD   0
 *  27:24           AGC_PGA_RD   0x0
 *  23:16              TSSI_RD   0x0
 *  15:14           ST_SAR_ADC   0x1
 *  13:12            ST_RX_ADC   0x1
 *  11:10              TX_DC_N   0x1
 *     09              DSEL_VA   0
 *  08:01         TSSI_POUT_TH   0x40
 *     00         ST_RX_ADC_IQ   0
 * </pre>
 */
#define RC_AGC_CFG_ADDR   0x01050150
#define RC_AGC_CFG_OFFSET 0x00000150
#define RC_AGC_CFG_INDEX  0x00000054
#define RC_AGC_CFG_RESET  0x00005480

__INLINE uint32_t rc_agc_cfg_get(void)
{
    return REG_PL_RD(RC_AGC_CFG_ADDR);
}

__INLINE void rc_agc_cfg_set(uint32_t value)
{
    REG_PL_WR(RC_AGC_CFG_ADDR, value);
}

// field definitions
#define RC_AGC_MAP_MODE_BIT    ((uint32_t)0x80000000)
#define RC_AGC_MAP_MODE_POS    31
#define RC_AGC_LNA_RD_MASK     ((uint32_t)0x60000000)
#define RC_AGC_LNA_RD_LSB      29
#define RC_AGC_LNA_RD_WIDTH    ((uint32_t)0x00000002)
#define RC_AGC_BUF_RD_BIT      ((uint32_t)0x10000000)
#define RC_AGC_BUF_RD_POS      28
#define RC_AGC_PGA_RD_MASK     ((uint32_t)0x0F000000)
#define RC_AGC_PGA_RD_LSB      24
#define RC_AGC_PGA_RD_WIDTH    ((uint32_t)0x00000004)
#define RC_TSSI_RD_MASK        ((uint32_t)0x00FF0000)
#define RC_TSSI_RD_LSB         16
#define RC_TSSI_RD_WIDTH       ((uint32_t)0x00000008)
#define RC_ST_SAR_ADC_MASK     ((uint32_t)0x0000C000)
#define RC_ST_SAR_ADC_LSB      14
#define RC_ST_SAR_ADC_WIDTH    ((uint32_t)0x00000002)
#define RC_ST_RX_ADC_MASK      ((uint32_t)0x00003000)
#define RC_ST_RX_ADC_LSB       12
#define RC_ST_RX_ADC_WIDTH     ((uint32_t)0x00000002)
#define RC_TX_DC_N_MASK        ((uint32_t)0x00000C00)
#define RC_TX_DC_N_LSB         10
#define RC_TX_DC_N_WIDTH       ((uint32_t)0x00000002)
#define RC_DSEL_VA_BIT         ((uint32_t)0x00000200)
#define RC_DSEL_VA_POS         9
#define RC_TSSI_POUT_TH_MASK   ((uint32_t)0x000001FE)
#define RC_TSSI_POUT_TH_LSB    1
#define RC_TSSI_POUT_TH_WIDTH  ((uint32_t)0x00000008)
#define RC_ST_RX_ADC_IQ_BIT    ((uint32_t)0x00000001)
#define RC_ST_RX_ADC_IQ_POS    0

#define RC_AGC_MAP_MODE_RST    0x0
#define RC_AGC_LNA_RD_RST      0x0
#define RC_AGC_BUF_RD_RST      0x0
#define RC_AGC_PGA_RD_RST      0x0
#define RC_TSSI_RD_RST         0x0
#define RC_ST_SAR_ADC_RST      0x1
#define RC_ST_RX_ADC_RST       0x1
#define RC_TX_DC_N_RST         0x1
#define RC_DSEL_VA_RST         0x0
#define RC_TSSI_POUT_TH_RST    0x40
#define RC_ST_RX_ADC_IQ_RST    0x0

__INLINE void rc_agc_cfg_pack(uint8_t agcmapmode, uint8_t agclnard, uint8_t agcbufrd, uint8_t agcpgard, uint8_t tssird, uint8_t stsaradc, uint8_t strxadc, uint8_t txdcn, uint8_t dselva, uint8_t tssipoutth, uint8_t strxadciq)
{
    ASSERT_ERR((((uint32_t)agcmapmode << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)agclnard << 29) & ~((uint32_t)0x60000000)) == 0);
    ASSERT_ERR((((uint32_t)agcbufrd << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)agcpgard << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)tssird << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)stsaradc << 14) & ~((uint32_t)0x0000C000)) == 0);
    ASSERT_ERR((((uint32_t)strxadc << 12) & ~((uint32_t)0x00003000)) == 0);
    ASSERT_ERR((((uint32_t)txdcn << 10) & ~((uint32_t)0x00000C00)) == 0);
    ASSERT_ERR((((uint32_t)dselva << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)tssipoutth << 1) & ~((uint32_t)0x000001FE)) == 0);
    ASSERT_ERR((((uint32_t)strxadciq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR,  ((uint32_t)agcmapmode << 31) | ((uint32_t)agclnard << 29) | ((uint32_t)agcbufrd << 28) | ((uint32_t)agcpgard << 24) | ((uint32_t)tssird << 16) | ((uint32_t)stsaradc << 14) | ((uint32_t)strxadc << 12) | ((uint32_t)txdcn << 10) | ((uint32_t)dselva << 9) | ((uint32_t)tssipoutth << 1) | ((uint32_t)strxadciq << 0));
}

__INLINE void rc_agc_cfg_unpack(uint8_t *agcmapmode, uint8_t *agclnard, uint8_t *agcbufrd, uint8_t *agcpgard, uint8_t *tssird, uint8_t *stsaradc, uint8_t *strxadc, uint8_t *txdcn, uint8_t *dselva, uint8_t *tssipoutth, uint8_t *strxadciq)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);

    *agcmapmode = (localVal & ((uint32_t)0x80000000)) >> 31;
    *agclnard = (localVal & ((uint32_t)0x60000000)) >> 29;
    *agcbufrd = (localVal & ((uint32_t)0x10000000)) >> 28;
    *agcpgard = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *tssird = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *stsaradc = (localVal & ((uint32_t)0x0000C000)) >> 14;
    *strxadc = (localVal & ((uint32_t)0x00003000)) >> 12;
    *txdcn = (localVal & ((uint32_t)0x00000C00)) >> 10;
    *dselva = (localVal & ((uint32_t)0x00000200)) >> 9;
    *tssipoutth = (localVal & ((uint32_t)0x000001FE)) >> 1;
    *strxadciq = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t rc_agc_map_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void rc_agc_map_mode_setf(uint8_t agcmapmode)
{
    ASSERT_ERR((((uint32_t)agcmapmode << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)agcmapmode << 31));
}

__INLINE uint8_t rc_agc_lna_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x60000000)) >> 29);
}

__INLINE void rc_agc_lna_rd_setf(uint8_t agclnard)
{
    ASSERT_ERR((((uint32_t)agclnard << 29) & ~((uint32_t)0x60000000)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x60000000)) | ((uint32_t)agclnard << 29));
}

__INLINE uint8_t rc_agc_buf_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

__INLINE void rc_agc_buf_rd_setf(uint8_t agcbufrd)
{
    ASSERT_ERR((((uint32_t)agcbufrd << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)agcbufrd << 28));
}

__INLINE uint8_t rc_agc_pga_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

__INLINE void rc_agc_pga_rd_setf(uint8_t agcpgard)
{
    ASSERT_ERR((((uint32_t)agcpgard << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)agcpgard << 24));
}

__INLINE uint8_t rc_tssi_rd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

__INLINE void rc_tssi_rd_setf(uint8_t tssird)
{
    ASSERT_ERR((((uint32_t)tssird << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)tssird << 16));
}

__INLINE uint8_t rc_st_sar_adc_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x0000C000)) >> 14);
}

__INLINE void rc_st_sar_adc_setf(uint8_t stsaradc)
{
    ASSERT_ERR((((uint32_t)stsaradc << 14) & ~((uint32_t)0x0000C000)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x0000C000)) | ((uint32_t)stsaradc << 14));
}

__INLINE uint8_t rc_st_rx_adc_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

__INLINE void rc_st_rx_adc_setf(uint8_t strxadc)
{
    ASSERT_ERR((((uint32_t)strxadc << 12) & ~((uint32_t)0x00003000)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x00003000)) | ((uint32_t)strxadc << 12));
}

__INLINE uint8_t rc_tx_dc_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000C00)) >> 10);
}

__INLINE void rc_tx_dc_n_setf(uint8_t txdcn)
{
    ASSERT_ERR((((uint32_t)txdcn << 10) & ~((uint32_t)0x00000C00)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x00000C00)) | ((uint32_t)txdcn << 10));
}

__INLINE uint8_t rc_dsel_va_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

__INLINE void rc_dsel_va_setf(uint8_t dselva)
{
    ASSERT_ERR((((uint32_t)dselva << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)dselva << 9));
}

__INLINE uint8_t rc_tssi_pout_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x000001FE)) >> 1);
}

__INLINE void rc_tssi_pout_th_setf(uint8_t tssipoutth)
{
    ASSERT_ERR((((uint32_t)tssipoutth << 1) & ~((uint32_t)0x000001FE)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x000001FE)) | ((uint32_t)tssipoutth << 1));
}

__INLINE uint8_t rc_st_rx_adc_iq_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_AGC_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void rc_st_rx_adc_iq_setf(uint8_t strxadciq)
{
    ASSERT_ERR((((uint32_t)strxadciq << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(RC_AGC_CFG_ADDR, (REG_PL_RD(RC_AGC_CFG_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)strxadciq << 0));
}

/**
 * @brief TRX_SPI_INTLV register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31         TRX_BANK_RPT   0
 *  09:00        TRX_SPI_INTLV   0x40
 * </pre>
 */
#define RC_TRX_SPI_INTLV_ADDR   0x01050170
#define RC_TRX_SPI_INTLV_OFFSET 0x00000170
#define RC_TRX_SPI_INTLV_INDEX  0x0000005C
#define RC_TRX_SPI_INTLV_RESET  0x00000040

__INLINE uint32_t rc_trx_spi_intlv_get(void)
{
    return REG_PL_RD(RC_TRX_SPI_INTLV_ADDR);
}

__INLINE void rc_trx_spi_intlv_set(uint32_t value)
{
    REG_PL_WR(RC_TRX_SPI_INTLV_ADDR, value);
}

// field definitions
#define RC_TRX_BANK_RPT_BIT     ((uint32_t)0x80000000)
#define RC_TRX_BANK_RPT_POS     31
#define RC_TRX_SPI_INTLV_MASK   ((uint32_t)0x000003FF)
#define RC_TRX_SPI_INTLV_LSB    0
#define RC_TRX_SPI_INTLV_WIDTH  ((uint32_t)0x0000000A)

#define RC_TRX_BANK_RPT_RST     0x0
#define RC_TRX_SPI_INTLV_RST    0x40

__INLINE void rc_trx_spi_intlv_unpack(uint8_t *trxbankrpt, uint16_t *trxspiintlv)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_SPI_INTLV_ADDR);

    *trxbankrpt = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trxspiintlv = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint8_t rc_trx_bank_rpt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_SPI_INTLV_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE uint16_t rc_trx_spi_intlv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_TRX_SPI_INTLV_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

__INLINE void rc_trx_spi_intlv_setf(uint16_t trxspiintlv)
{
    ASSERT_ERR((((uint32_t)trxspiintlv << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(RC_TRX_SPI_INTLV_ADDR, (uint32_t)trxspiintlv << 0);
}

/**
 * @brief ADDA_REG0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            ADDA_REG0   0x0
 * </pre>
 */
#define RC_ADDA_REG0_ADDR   0x01050190
#define RC_ADDA_REG0_OFFSET 0x00000190
#define RC_ADDA_REG0_INDEX  0x00000064
#define RC_ADDA_REG0_RESET  0x00000000

__INLINE uint32_t rc_adda_reg0_get(void)
{
    return REG_PL_RD(RC_ADDA_REG0_ADDR);
}

__INLINE void rc_adda_reg0_set(uint32_t value)
{
    REG_PL_WR(RC_ADDA_REG0_ADDR, value);
}

// field definitions
#define RC_ADDA_REG0_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_ADDA_REG0_LSB    0
#define RC_ADDA_REG0_WIDTH  ((uint32_t)0x00000020)

#define RC_ADDA_REG0_RST    0x0

__INLINE uint32_t rc_adda_reg0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_ADDA_REG0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_adda_reg0_setf(uint32_t addareg0)
{
    ASSERT_ERR((((uint32_t)addareg0 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_ADDA_REG0_ADDR, (uint32_t)addareg0 << 0);
}

/**
 * @brief ADDA_REG1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            ADDA_REG1   0x0
 * </pre>
 */
#define RC_ADDA_REG1_ADDR   0x01050194
#define RC_ADDA_REG1_OFFSET 0x00000194
#define RC_ADDA_REG1_INDEX  0x00000065
#define RC_ADDA_REG1_RESET  0x00000000

__INLINE uint32_t rc_adda_reg1_get(void)
{
    return REG_PL_RD(RC_ADDA_REG1_ADDR);
}

__INLINE void rc_adda_reg1_set(uint32_t value)
{
    REG_PL_WR(RC_ADDA_REG1_ADDR, value);
}

// field definitions
#define RC_ADDA_REG1_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_ADDA_REG1_LSB    0
#define RC_ADDA_REG1_WIDTH  ((uint32_t)0x00000020)

#define RC_ADDA_REG1_RST    0x0

__INLINE uint32_t rc_adda_reg1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_ADDA_REG1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_adda_reg1_setf(uint32_t addareg1)
{
    ASSERT_ERR((((uint32_t)addareg1 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_ADDA_REG1_ADDR, (uint32_t)addareg1 << 0);
}

/**
 * @brief ADDA_REG2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            ADDA_REG2   0x0
 * </pre>
 */
#define RC_ADDA_REG2_ADDR   0x01050198
#define RC_ADDA_REG2_OFFSET 0x00000198
#define RC_ADDA_REG2_INDEX  0x00000066
#define RC_ADDA_REG2_RESET  0x00000000

__INLINE uint32_t rc_adda_reg2_get(void)
{
    return REG_PL_RD(RC_ADDA_REG2_ADDR);
}

__INLINE void rc_adda_reg2_set(uint32_t value)
{
    REG_PL_WR(RC_ADDA_REG2_ADDR, value);
}

// field definitions
#define RC_ADDA_REG2_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_ADDA_REG2_LSB    0
#define RC_ADDA_REG2_WIDTH  ((uint32_t)0x00000020)

#define RC_ADDA_REG2_RST    0x0

__INLINE uint32_t rc_adda_reg2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_ADDA_REG2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_adda_reg2_setf(uint32_t addareg2)
{
    ASSERT_ERR((((uint32_t)addareg2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_ADDA_REG2_ADDR, (uint32_t)addareg2 << 0);
}

/**
 * @brief ADDA_REG3 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            ADDA_REG3   0x0
 * </pre>
 */
#define RC_ADDA_REG3_ADDR   0x0105019C
#define RC_ADDA_REG3_OFFSET 0x0000019C
#define RC_ADDA_REG3_INDEX  0x00000067
#define RC_ADDA_REG3_RESET  0x00000000

__INLINE uint32_t rc_adda_reg3_get(void)
{
    return REG_PL_RD(RC_ADDA_REG3_ADDR);
}

__INLINE void rc_adda_reg3_set(uint32_t value)
{
    REG_PL_WR(RC_ADDA_REG3_ADDR, value);
}

// field definitions
#define RC_ADDA_REG3_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_ADDA_REG3_LSB    0
#define RC_ADDA_REG3_WIDTH  ((uint32_t)0x00000020)

#define RC_ADDA_REG3_RST    0x0

__INLINE uint32_t rc_adda_reg3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_ADDA_REG3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_adda_reg3_setf(uint32_t addareg3)
{
    ASSERT_ERR((((uint32_t)addareg3 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_ADDA_REG3_ADDR, (uint32_t)addareg3 << 0);
}

/**
 * @brief ADDA_REG4 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            ADDA_REG4   0x0
 * </pre>
 */
#define RC_ADDA_REG4_ADDR   0x010501A0
#define RC_ADDA_REG4_OFFSET 0x000001A0
#define RC_ADDA_REG4_INDEX  0x00000068
#define RC_ADDA_REG4_RESET  0x00000000

__INLINE uint32_t rc_adda_reg4_get(void)
{
    return REG_PL_RD(RC_ADDA_REG4_ADDR);
}

__INLINE void rc_adda_reg4_set(uint32_t value)
{
    REG_PL_WR(RC_ADDA_REG4_ADDR, value);
}

// field definitions
#define RC_ADDA_REG4_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_ADDA_REG4_LSB    0
#define RC_ADDA_REG4_WIDTH  ((uint32_t)0x00000020)

#define RC_ADDA_REG4_RST    0x0

__INLINE uint32_t rc_adda_reg4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_ADDA_REG4_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_adda_reg4_setf(uint32_t addareg4)
{
    ASSERT_ERR((((uint32_t)addareg4 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_ADDA_REG4_ADDR, (uint32_t)addareg4 << 0);
}

/**
 * @brief ADDA_REG5 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            ADDA_REG5   0x0
 * </pre>
 */
#define RC_ADDA_REG5_ADDR   0x010501A4
#define RC_ADDA_REG5_OFFSET 0x000001A4
#define RC_ADDA_REG5_INDEX  0x00000069
#define RC_ADDA_REG5_RESET  0x00000000

__INLINE uint32_t rc_adda_reg5_get(void)
{
    return REG_PL_RD(RC_ADDA_REG5_ADDR);
}

__INLINE void rc_adda_reg5_set(uint32_t value)
{
    REG_PL_WR(RC_ADDA_REG5_ADDR, value);
}

// field definitions
#define RC_ADDA_REG5_MASK   ((uint32_t)0xFFFFFFFF)
#define RC_ADDA_REG5_LSB    0
#define RC_ADDA_REG5_WIDTH  ((uint32_t)0x00000020)

#define RC_ADDA_REG5_RST    0x0

__INLINE uint32_t rc_adda_reg5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_ADDA_REG5_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void rc_adda_reg5_setf(uint32_t addareg5)
{
    ASSERT_ERR((((uint32_t)addareg5 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(RC_ADDA_REG5_ADDR, (uint32_t)addareg5 << 0);
}

/**
 * @brief ADDA_REG_STAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  05:00        ADDA_REG_STAT   0x0
 * </pre>
 */
#define RC_ADDA_REG_STAT_ADDR   0x010501A8
#define RC_ADDA_REG_STAT_OFFSET 0x000001A8
#define RC_ADDA_REG_STAT_INDEX  0x0000006A
#define RC_ADDA_REG_STAT_RESET  0x00000000

__INLINE uint32_t rc_adda_reg_stat_get(void)
{
    return REG_PL_RD(RC_ADDA_REG_STAT_ADDR);
}

__INLINE void rc_adda_reg_stat_set(uint32_t value)
{
    REG_PL_WR(RC_ADDA_REG_STAT_ADDR, value);
}

// field definitions
#define RC_ADDA_REG_STAT_MASK   ((uint32_t)0x0000003F)
#define RC_ADDA_REG_STAT_LSB    0
#define RC_ADDA_REG_STAT_WIDTH  ((uint32_t)0x00000006)

#define RC_ADDA_REG_STAT_RST    0x0

__INLINE uint8_t rc_adda_reg_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC_ADDA_REG_STAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000003F)) == 0);
    return (localVal >> 0);
}


#endif // _REG_RC_H_

