/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h" 
#include "rtl8195a_i2s.h"
#include "hal_i2s.h"

extern void *
_memset( void *s, int c, SIZE_T n );

RTK_STATUS
HalI2SInitRtl8195a_Patch(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
    //u8  I2SEn;
    u8  I2SMaster;
    u8  I2SWordLen;
    u8  I2SChNum;
    u8  I2SPageNum;
    u16 I2SPageSize;
    u16 I2SRate;
    u32 I2STxIntrMSK;
    u32 I2SRxIntrMSK;
    u8  I2STRxAct;
    u8  *I2STxData;
    u8  *I2SRxData;

    u32 Tmp;

    I2SIdx         = pHalI2SInitData->I2SIdx;
    //I2SEn          = pHalI2SInitData->I2SEn;
    I2SMaster      = pHalI2SInitData->I2SMaster;
    I2SWordLen     = pHalI2SInitData->I2SWordLen;
    I2SChNum       = pHalI2SInitData->I2SChNum;
    I2SPageNum     = pHalI2SInitData->I2SPageNum;
    I2SPageSize    = pHalI2SInitData->I2SPageSize;
    I2SRate        = pHalI2SInitData->I2SRate;
    I2STRxAct      = pHalI2SInitData->I2STRxAct;
    I2STxData      = pHalI2SInitData->I2STxData;
    I2SRxData      = pHalI2SInitData->I2SRxData;


    /* Disable the I2S first, and reset to default */
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, BIT_CTRL_CTLX_I2S_EN(0) | 
                                         BIT_CTRL_CTLX_I2S_SW_RSTN(1));
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, BIT_CTRL_CTLX_I2S_EN(0) | 
                                         BIT_CTRL_CTLX_I2S_SW_RSTN(0));
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, BIT_CTRL_CTLX_I2S_EN(0) | 
                                         BIT_CTRL_CTLX_I2S_SW_RSTN(1));

    Tmp = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL);
    Tmp |= BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1);

    if (I2SRate&0x10) 
    {
        Tmp |= BIT_CTRL_CTLX_I2S_CLK_SRC(1);
    }

    Tmp |= (BIT_CTRL_CTLX_I2S_WL(I2SWordLen) | BIT_CTRL_CTLX_I2S_CH_NUM(I2SChNum) |
            BIT_CTRL_CTLX_I2S_SLAVE_MODE(I2SMaster) | BIT_CTRL_CTLX_I2S_TRX_ACT(I2STRxAct));
    /* set 44.1khz clock source, word length, channel number, master or slave, trx act */
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, Tmp);

    Tmp = BIT_CTRL_SETTING_I2S_PAGE_SZ(I2SPageSize) | BIT_CTRL_SETTING_I2S_PAGE_NUM(I2SPageNum) |
          BIT_CTRL_SETTING_I2S_SAMPLE_RATE(I2SRate);
    /* set page size, page number, sample rate */
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_SETTING, Tmp);

    /* need tx rx buffer? need rx page own bit */
    if (I2STxData != NULL) {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE_PTR, (u32)I2STxData);
    }

    if (I2SRxData != NULL) {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE_PTR, (u32)I2SRxData);
    }

	pHalI2SInitData->I2STxIdx = 0;
	pHalI2SInitData->I2SRxIdx = 0;
    pHalI2SInitData->I2SHWTxIdx = 0;
    pHalI2SInitData->I2SHWRxIdx = 0;
    /* I2S Clear all interrupts first */
    HalI2SClrAllIntrRtl8195a(pHalI2SInitData);

    /* I2S Disable all interrupts first */
    I2STxIntrMSK   = pHalI2SInitData->I2STxIntrMSK;
    I2SRxIntrMSK   = pHalI2SInitData->I2SRxIntrMSK;
    pHalI2SInitData->I2STxIntrMSK = 0;
    pHalI2SInitData->I2SRxIntrMSK = 0;
    HalI2SIntrCtrlRtl8195a(pHalI2SInitData);
    pHalI2SInitData->I2STxIntrMSK = I2STxIntrMSK;
    pHalI2SInitData->I2SRxIntrMSK = I2SRxIntrMSK;

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetRateRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8 I2SIdx;
    u32 reg_value;

    I2SIdx = pHalI2SInitData->I2SIdx;

    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL);    
    reg_value &= ~(BIT_MASK_CTLX_I2S_CLK_SRC << BIT_SHIFT_CTLX_I2S_CLK_SRC);
    if (pHalI2SInitData->I2SRate&0x10) 
    {
        reg_value |= BIT_CTRL_CTLX_I2S_CLK_SRC(1);
    }
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, reg_value);
    
    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_SETTING);
    reg_value &= ~(BIT_MASK_SETTING_I2S_SAMPLE_RATE << BIT_SHIFT_SETTING_I2S_SAMPLE_RATE);
    reg_value |= BIT_CTRL_SETTING_I2S_SAMPLE_RATE(pHalI2SInitData->I2SRate);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_SETTING, reg_value);
    
    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetWordLenRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8 I2SIdx;
    u32 reg_value;

    I2SIdx = pHalI2SInitData->I2SIdx;
    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL);
    reg_value &= ~(BIT_MASK_CTLX_I2S_WL << BIT_SHIFT_CTLX_I2S_WL);
    reg_value |= BIT_CTRL_CTLX_I2S_WL(pHalI2SInitData->I2SWordLen);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, reg_value);

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetChNumRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8 I2SIdx;
    u32 reg_value;

    I2SIdx = pHalI2SInitData->I2SIdx;
    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL);
    reg_value &= ~(BIT_MASK_CTLX_I2S_CH_NUM << BIT_SHIFT_CTLX_I2S_CH_NUM);
    reg_value |= BIT_CTRL_CTLX_I2S_CH_NUM(pHalI2SInitData->I2SChNum);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, reg_value);

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetPageNumRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8 I2SIdx;
    u32 reg_value;

    I2SIdx = pHalI2SInitData->I2SIdx;

    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_SETTING);
    reg_value &= ~(BIT_MASK_SETTING_I2S_PAGE_NUM << BIT_SHIFT_SETTING_I2S_PAGE_NUM);
    reg_value |= BIT_CTRL_SETTING_I2S_PAGE_NUM(pHalI2SInitData->I2SPageNum);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_SETTING, reg_value);

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetPageSizeRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8 I2SIdx;
    u32 reg_value;

    I2SIdx = pHalI2SInitData->I2SIdx;

    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_SETTING);
    reg_value &= ~(BIT_MASK_SETTING_I2S_PAGE_SZ << BIT_SHIFT_SETTING_I2S_PAGE_SZ);
    reg_value |= BIT_CTRL_SETTING_I2S_PAGE_SZ(pHalI2SInitData->I2SPageSize);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_SETTING, reg_value);

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetDirectionRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8 I2SIdx;
    u32 reg_value;

    I2SIdx = pHalI2SInitData->I2SIdx;

    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL);
    reg_value &= ~(BIT_MASK_CTLX_I2S_TRX_ACT << BIT_SHIFT_CTLX_I2S_TRX_ACT);
    reg_value |= BIT_CTRL_CTLX_I2S_TRX_ACT(pHalI2SInitData->I2STRxAct);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, reg_value);

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetDMABufRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8 I2SIdx;
    u32 reg_value;
    u32 page_num;

    I2SIdx = pHalI2SInitData->I2SIdx;

    reg_value = HAL_I2S_READ32(I2SIdx, REG_I2S_SETTING);
    reg_value &= ~(BIT_MASK_SETTING_I2S_PAGE_SZ << BIT_SHIFT_SETTING_I2S_PAGE_SZ);
    reg_value &= ~(BIT_MASK_SETTING_I2S_PAGE_NUM << BIT_SHIFT_SETTING_I2S_PAGE_NUM);
    reg_value |= BIT_CTRL_SETTING_I2S_PAGE_SZ(pHalI2SInitData->I2SPageSize);
    reg_value |= BIT_CTRL_SETTING_I2S_PAGE_NUM(pHalI2SInitData->I2SPageNum);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_SETTING, reg_value);

    page_num = pHalI2SInitData->I2SPageNum + 1;
    if (pHalI2SInitData->I2STxData) {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE_PTR, (uint32_t)pHalI2SInitData->I2STxData);
        pHalI2SInitData->I2STxIntrMSK  = (1<<page_num) - 1;
    } else {
        pHalI2SInitData->I2STxIntrMSK = 0;
    }

    if (pHalI2SInitData->I2SRxData) {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE_PTR, (uint32_t)pHalI2SInitData->I2SRxData);
        pHalI2SInitData->I2SRxIntrMSK  = (1<<page_num) - 1;
    } else {
        pHalI2SInitData->I2SRxIntrMSK  = 0;
        
    }

    // According to the page number to modify the ISR mask
    HalI2SIntrCtrlRtl8195a(pHalI2SInitData);

    return _EXIT_SUCCESS;
}

u8
HalI2SGetTxPageRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
	u16 I2STxIdx = pHalI2SInitData->I2STxIdx;
    u32 reg;
    
    I2SIdx = pHalI2SInitData->I2SIdx;
    
	reg = HAL_I2S_READ32(I2SIdx, REG_I2S_TX_PAGE0_OWN+(I2STxIdx<<2));
    if ((reg & (1<<31)) == 0) {
        return I2STxIdx;
    } else {
        return 0xFF;
    }
}

u8
HalI2SGetRxPageRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
	u16 I2SRxIdx = pHalI2SInitData->I2SRxIdx;
    u32 reg;
    
    I2SIdx = pHalI2SInitData->I2SIdx;
    
	reg = HAL_I2S_READ32(I2SIdx, REG_I2S_RX_PAGE0_OWN+(I2SRxIdx << 2));
    if ((reg & (1<<31)) == 0) {
        return I2SRxIdx;
    } else {
        return 0xFF;
    }
}

RTK_STATUS
HalI2SPageSendRtl8195a(
    IN  VOID    *Data,
	IN  u8      PageIdx
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
	u16 I2STxIdx = pHalI2SInitData->I2STxIdx;
	u8  I2SPageNum = pHalI2SInitData->I2SPageNum;
    u8  I2SIdx;

    if (I2STxIdx != PageIdx) {
        DBG_I2S_ERR("HalI2SPageSendRtl8195a: UnExpected Page Index. TxPage=%d, Expected:%d\r\n", 
            PageIdx, I2STxIdx);
    }
    
    I2SIdx = pHalI2SInitData->I2SIdx;
    
	HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE0_OWN+4*PageIdx, 1<<31);
    I2STxIdx = PageIdx+1;
    if (I2STxIdx > I2SPageNum) {
        I2STxIdx = 0;
    }
    pHalI2SInitData->I2STxIdx = I2STxIdx;

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SPageRecvRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
	u16 I2SRxIdx = pHalI2SInitData->I2SRxIdx;
	u8  I2SPageNum = pHalI2SInitData->I2SPageNum;
    u32 reg;
    u8  I2SIdx;

    I2SIdx = pHalI2SInitData->I2SIdx;
	reg = HAL_I2S_READ32(I2SIdx, REG_I2S_RX_PAGE0_OWN+(I2SRxIdx << 2));
    if ((reg & (1<<31)) != 0) {
        DBG_I2S_ERR("HalI2SPageRecvRtl8195a: No Idle Rx Page\r\n");
        return _EXIT_FAILURE;
    }

	HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE0_OWN+(I2SRxIdx<<2), 1<<31);
    I2SRxIdx += 1;
    if (I2SRxIdx > I2SPageNum) {
        I2SRxIdx = 0;
    }
    pHalI2SInitData->I2SRxIdx = I2SRxIdx;

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SClearAllOwnBitRtl8195a(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8  I2SIdx;
    u32 i;
    
    I2SIdx = pHalI2SInitData->I2SIdx;

    for (i=0;i<4;i++) {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE0_OWN+(i<<2), 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE0_OWN+(i<<2), 0);
    }

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SDMACtrlRtl8195a(
    IN  VOID    *Data
)
{

    return _EXIT_SUCCESS;
}


