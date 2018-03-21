/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "platform_autoconf.h" 
#include "diag.h"
#include "rtl8195a_adc.h"
#include "hal_adc.h"

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
HalADCInit8195a(
    IN  VOID    *Data
)
{
    PHAL_ADC_INIT_DAT   pHalAdcInitData     =   (PHAL_ADC_INIT_DAT)Data;
    u32 AdcTempDat;
    u8  AdcTempIdx  =   pHalAdcInitData->ADCIdx;

    /* Enable ADC power cut */
/*
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_POWER);
    AdcTempDat  |=  BIT_ADC_PWR_AUTO;
    HAL_ADC_WRITE32(REG_ADC_POWER, AdcTempDat);
*/
    
    /* ADC Control register set-up*/
    AdcTempDat  = 0;
    AdcTempDat  |= (BIT_CTRL_ADC_COMP_ONLY(pHalAdcInitData->ADCCompOnly)    |
                    BIT_CTRL_ADC_ONESHOT(pHalAdcInitData->ADCOneShotEn)     |
                    BIT_CTRL_ADC_OVERWRITE(pHalAdcInitData->ADCOverWREn)    |
                    BIT_CTRL_ADC_ENDIAN(pHalAdcInitData->ADCEndian)         |
                    BIT_CTRL_ADC_BURST_SIZE(pHalAdcInitData->ADCBurstSz)    |
                    BIT_CTRL_ADC_THRESHOLD(pHalAdcInitData->ADCOneShotTD)   |
                    BIT_CTRL_ADC_DBG_SEL(pHalAdcInitData->ADCDbgSel));
    HAL_ADC_WRITE32(REG_ADC_CONTROL,AdcTempDat);

    DBG_8195A_ADC_LVL(HAL_ADC_LVL,"REG_ADC_CONTROL:%x\n", HAL_ADC_READ32(REG_ADC_CONTROL));
    
    /* ADC compare value and compare method setting*/    
    switch (AdcTempIdx) {
        case ADC0_SEL:
            AdcTempDat  =   HAL_ADC_READ32(REG_ADC_COMP_VALUE_L);
            AdcTempDat  &=  ~(BIT_ADC_COMP_TH_0(0xFFFF));
            AdcTempDat  |=  BIT_CTRL_ADC_COMP_TH_0(pHalAdcInitData->ADCCompTD);
            HAL_ADC_WRITE32(REG_ADC_COMP_VALUE_L, AdcTempDat);
            break;
            
        case ADC1_SEL:
            AdcTempDat  =   HAL_ADC_READ32(REG_ADC_COMP_VALUE_L);
            AdcTempDat  &=  ~(BIT_ADC_COMP_TH_1(0xFFFF));
            AdcTempDat  |=  BIT_CTRL_ADC_COMP_TH_1(pHalAdcInitData->ADCCompTD);
            HAL_ADC_WRITE32(REG_ADC_COMP_VALUE_L, AdcTempDat);
            break;
            
        case ADC2_SEL:
            AdcTempDat  =   HAL_ADC_READ32(REG_ADC_COMP_VALUE_H);
            AdcTempDat  &=  ~(BIT_ADC_COMP_TH_2(0xFFFF));
            AdcTempDat  |=  BIT_CTRL_ADC_COMP_TH_2(pHalAdcInitData->ADCCompTD);
            HAL_ADC_WRITE32(REG_ADC_COMP_VALUE_H, AdcTempDat);
            break;
            
        case ADC3_SEL:
            AdcTempDat  =   HAL_ADC_READ32(REG_ADC_COMP_VALUE_H);
            AdcTempDat  &=  ~(BIT_ADC_COMP_TH_3(0xFFFF));
            AdcTempDat  |=  BIT_CTRL_ADC_COMP_TH_3(pHalAdcInitData->ADCCompTD);
            HAL_ADC_WRITE32(REG_ADC_COMP_VALUE_H, AdcTempDat);
            break;
        default:
            return _EXIT_FAILURE;
    }

    /* ADC compare mode setting */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_COMP_SET);
    AdcTempDat  &=  (~(0x01 << pHalAdcInitData->ADCIdx));
    AdcTempDat  |=  (BIT_CTRL_ADC_COMP_0_EN(pHalAdcInitData->ADCCompCtrl) << 
                                                    pHalAdcInitData->ADCIdx);
    HAL_ADC_WRITE32(REG_ADC_COMP_SET, AdcTempDat);

    /* ADC audio mode set-up */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
    AdcTempDat  &=  ~(BIT_ADC_AUDIO_EN);
    AdcTempDat  |=  BIT_CTRL_ADC_AUDIO_EN(pHalAdcInitData->ADCAudioEn);
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);

    /* ADC enable manually setting */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
    AdcTempDat  &=  ~(BIT_ADC_EN_MANUAL);
    AdcTempDat  |=  BIT_CTRL_ADC_EN_MANUAL(pHalAdcInitData->ADCEnManul);
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);


    /* ADC analog parameter 0 */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
    DBG_ADC_INFO("AD0:%x\n", AdcTempDat);
    //AdcTempDat  |= (BIT0);
    if (pHalAdcInitData->ADCInInput == 1){
        AdcTempDat  &= (~BIT14);
    }
    else {
        AdcTempDat  |= (BIT14);
    }
    AdcTempDat  &= (~(BIT3|BIT2));
        
    /* Adjust VCM for C-Cut*/
#ifdef CONFIG_CHIP_C_CUT
    AdcTempDat  |= (BIT22);
#endif
    
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
    DBG_ADC_INFO("AD0:%x\n", AdcTempDat);

    /* ADC analog parameter 1 */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
    AdcTempDat  &= (~BIT1);
    AdcTempDat  |= (BIT2|BIT0);
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
    DBG_ADC_INFO("AD1:%x\n", AdcTempDat);
        
    /* ADC analog parameter 2 */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD2);
    DBG_ADC_INFO("AD2:%x\n", AdcTempDat);
    AdcTempDat  = 0x67884400;
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD2, AdcTempDat);
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD2);
    DBG_ADC_INFO("AD2:%x\n", AdcTempDat);

    /* ADC analog parameter 3 */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD3);
    DBG_ADC_INFO("AD3:%x\n", AdcTempDat);
    AdcTempDat  =  0x77780039;
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD3, AdcTempDat);
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD3);
    DBG_ADC_INFO("AD3:%x\n", AdcTempDat);

    /* ADC analog parameter 4 */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD4);
    DBG_ADC_INFO("AD4:%x\n", AdcTempDat);
    AdcTempDat  =  0x0004d501;
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD4, AdcTempDat);
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD4);
    DBG_ADC_INFO("AD4:%x\n", AdcTempDat);

    /* ADC analog parameter 5 */
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD5);
    DBG_ADC_INFO("AD5:%x\n", AdcTempDat);
    AdcTempDat  =  0x1E010800;
    HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD5, AdcTempDat);
    AdcTempDat  =   HAL_ADC_READ32(REG_ADC_ANAPAR_AD5);
    DBG_ADC_INFO("AD5:%x\n", AdcTempDat);

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
HalADCDeInit8195a(
    IN  VOID    *Data
)
{
    u32 AdcTempDat;
    Data = Data;
    AdcTempDat      =   HAL_ADC_READ32(REG_ADC_POWER);
    AdcTempDat      &=  ~(BIT_ADC_PWR_AUTO);
    HAL_ADC_WRITE32(REG_ADC_POWER, AdcTempDat);

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
HalADCEnableRtl8195a(
    IN  VOID    *Data
){
    //PHAL_ADC_INIT_DAT   pHalAdcInitData     =   (PHAL_ADC_INIT_DAT)Data;
    u32 AdcTempDat;
	DBG_ADC_INFO("HalADCEnableRtl8195a\n");
    Data = Data;
    AdcTempDat      =   HAL_ADC_READ32(REG_ADC_POWER);
	
    AdcTempDat      &=  (~BIT_ADC_PWR_AUTO);
    AdcTempDat      |= 0x02;
    HAL_ADC_WRITE32(REG_ADC_POWER, AdcTempDat);
	AdcTempDat      |= 0x04;
    HAL_ADC_WRITE32(REG_ADC_POWER, AdcTempDat);
	AdcTempDat      &= (~0x08);
    HAL_ADC_WRITE32(REG_ADC_POWER, AdcTempDat);
	
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_POWER);
	DBG_ADC_INFO("HalADCEnableRtl8195a, power reg:%x\n",AdcTempDat);
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
HalADCIntrCtrl8195a(
    IN  VOID    *Data
){
    PHAL_ADC_INIT_DAT   pHalAdcInitData     =   (PHAL_ADC_INIT_DAT)Data;
    
    HAL_ADC_WRITE32(REG_ADC_INTR_EN, pHalAdcInitData->ADCIntrMSK);

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
u32
HalADCReceiveRtl8195a(
    IN  VOID    *Data
){
    u32 AdcTempDat;
    Data = Data;
    AdcTempDat = HAL_ADC_READ32(REG_ADC_FIFO_READ);
    
    return (AdcTempDat);
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		HalI2CReadRegRtl8195a
//
// Description:
//		Directly read a I2C register according to the register offset.
//
// Arguments:
//		[in] VOID    *Data -
//			The I2C parameter data struct.
//		[in] I2CReg -
//			The I2C register offset.
//
// Return:
//		The register content in u32 format.
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
u32
HalADCReadRegRtl8195a(
        IN  VOID    *Data,
        IN  u8      I2CReg
){
    u32 AdcTempDat;
    Data = Data;
    AdcTempDat  =   HAL_ADC_READ32(I2CReg);
    return (AdcTempDat);
}

