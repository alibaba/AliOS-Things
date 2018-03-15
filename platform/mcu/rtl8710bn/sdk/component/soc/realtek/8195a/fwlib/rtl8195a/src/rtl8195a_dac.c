/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h" 
#include "rtl8195a_dac.h"
#include "hal_dac.h"


//---------------------------------------------------------------------------------------------------
//Function Name:
//		HalDACInit8195a
//
// Description:
//		To initialize DAC module by using the given data.
//
// Arguments:
//		[in] VOID    *Data -
//			The DAC parameter data struct.
//
// Return:
//		The status of the DeInit process.
//          _EXIT_SUCCESS if the initialization succeeded.
//          _EXIT_FAILURE if the initialization failed.
//
// Note:
//		None
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-15.
//
//---------------------------------------------------------------------------------------------------
RTK_STATUS
HalDACInit8195a(
    IN  VOID    *Data
){
    PHAL_DAC_INIT_DAT   pHalDacInitData = (PHAL_DAC_INIT_DAT)Data;
    u32 DacTempDat;
    u8  DacTempIdx      = pHalDacInitData->DACIdx;
    
    /* Enable DAC power cut */
    DacTempDat  = HAL_DAC_READ32(0, REG_DAC_PWR_CTRL);
    DacTempDat  |= BIT_DAC_PWR_AUTO;
    
    HAL_DAC_WRITE32(0, REG_DAC_PWR_CTRL, DacTempDat);

    /* Disable DAC module first */
    HAL_DAC_WRITE32(DacTempIdx, REG_DAC_CTRL, 0);

    /* Setup DAC module */
    DacTempDat  = 0;
    DacTempDat  |= (BIT_CTRL_DAC_SPEED(pHalDacInitData->DACDataRate)            |
                    BIT_CTRL_DAC_ENDIAN(pHalDacInitData->DACEndian)             |
                    BIT_CTRL_DAC_FILTER_SETTLE(pHalDacInitData->DACFilterSet)   |
                    BIT_CTRL_DAC_BURST_SIZE(pHalDacInitData->DACBurstSz)        |
                    BIT_CTRL_DAC_DBG_SEL(pHalDacInitData->DACDbgSel)            |
                    BIT_CTRL_DAC_DSC_DBG_SEL(pHalDacInitData->DACDscDbgSel)     |
                    BIT_CTRL_DAC_BYPASS_DSC(pHalDacInitData->DACBPDsc)          |
                    BIT_CTRL_DAC_DELTA_SIGMA(pHalDacInitData->DACDeltaSig));
                    
    HAL_DAC_WRITE32(DacTempIdx, REG_DAC_CTRL, DacTempDat);
    
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		HalI2CInit8195a
//
// Description:
//		To initialize I2C module by using the given data.
//
// Arguments:
//		[in] VOID    *Data -
//			The I2C parameter data struct.
//
// Return:
//		The status of the DeInit process.
//          _EXIT_SUCCESS if the initialization succeeded.
//          _EXIT_FAILURE if the initialization failed.
//
// Note:
//		None
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//---------------------------------------------------------------------------------------------------
RTK_STATUS
HalDACDeInit8195a(
    IN  VOID    *Data
){
    PHAL_DAC_INIT_DAT   pHalDacInitData = (PHAL_DAC_INIT_DAT)Data;
    u32 DacTempDat;

    DacTempDat = HAL_DAC_READ32(pHalDacInitData->DACIdx, REG_DAC_CTRL);
    DacTempDat &= (~BIT_DAC_FIFO_EN);
    HAL_DAC_WRITE32(pHalDacInitData->DACIdx, REG_DAC_CTRL ,DacTempDat);
    
    return _EXIT_SUCCESS;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		HalI2CIntrCtrl8195a
//
// Description:
//		Modify the I2C interrupt mask according to the given value
//
// Arguments:
//		[in] VOID    *Data -
//			The I2C parameter data struct.
//
// Return:
//		The status of the enable process.
//          _EXIT_SUCCESS if the de-initialization succeeded.
//          _EXIT_FAILURE if the de-initialization failed.
//
// Note:
//		None
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-02-18.
//
//---------------------------------------------------------------------------------------------------
RTK_STATUS
HalDACEnableRtl8195a(
    IN  VOID    *Data
){
    PHAL_DAC_INIT_DAT   pHalDacInitData = (PHAL_DAC_INIT_DAT)Data;
    u32 DacTempDat;
    u8  DacTempIdx      = pHalDacInitData->DACIdx;
    
    DacTempDat  = HAL_DAC_READ32(DacTempIdx, REG_DAC_CTRL);
    DacTempDat  &= (~BIT_DAC_FIFO_EN);

    DacTempDat  |= BIT_CTRL_DAC_FIFO_EN(pHalDacInitData->DACEn);
    HAL_DAC_WRITE32(DacTempIdx, REG_DAC_CTRL, DacTempDat);

    return _EXIT_SUCCESS;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		HalI2CIntrCtrl8195a
//
// Description:
//		Modify the I2C interrupt mask according to the given value
//
// Arguments:
//		[in] VOID    *Data -
//			The I2C parameter data struct.
//
// Return:
//		The status of the enable process.
//          _EXIT_SUCCESS if the de-initialization succeeded.
//          _EXIT_FAILURE if the de-initialization failed.
//
// Note:
//		None
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-02-18.
//
//---------------------------------------------------------------------------------------------------
RTK_STATUS
HalDACIntrCtrl8195a(
    IN  VOID    *Data
){
    PHAL_DAC_INIT_DAT   pHalDacInitData = (PHAL_DAC_INIT_DAT)Data;

    HAL_DAC_WRITE32(pHalDacInitData->DACIdx, REG_DAC_INTR_CTRL, pHalDacInitData->DACIntrMSK);
    
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		HalI2CReceiveRtl8195a
//
// Description:
//		Directly read one data byte a I2C data fifo.
//
// Arguments:
//		[in] VOID    *Data -
//			The I2C parameter data struct.
//
// Return:
//		The first data fifo content.
//
// Note:
//		None
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-02-18.
//
//---------------------------------------------------------------------------------------------------
u8
HalDACSendRtl8195a(
    IN  VOID    *Data
){

	Data = Data;
    return (0);
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		HalDACReadRegRtl8195a
//
// Description:
//		
//
// Arguments:
//		[in] VOID    *Data -
//			The DAC parameter data struct.
//		[in] I2CReg -
//			The DAC register offset.
//
// Return:
//		The DAC register content in u32 format.
//
// Note:
//		None
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-15.
//
//---------------------------------------------------------------------------------------------------
u32
HalDACReadRegRtl8195a(
        IN  VOID    *Data,
        IN  u8      I2CReg
){
    PHAL_DAC_INIT_DAT   pHalDacInitData = (PHAL_DAC_INIT_DAT)Data;

    //DBG_8195A_DAC("dac read reg idx:%x\n",pHalDacInitData->DACIdx);
    //DBG_8195A_DAC("dac read reg offset:%x\n",I2CReg);

    return (u32)HAL_DAC_READ32(pHalDacInitData->DACIdx, I2CReg);
}

