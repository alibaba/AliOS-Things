/**
  ******************************************************************************
  * @file    rtl8721dlp_sgpio.c
  * @author
  * @version V1.0.0
  * @date    2017-10-16
  * @brief   This file provides firmware rom patch functions to manage clk 
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"
#include "rtl8721d_sgpio.h"
/**
  * @brief  Fills each SGPIO_RXInitStruct member with its default value.
  * @param  SGPIO_RXInitStruct: pointer to an SGPIO_RXInitTypeDef structure which will be initialized.
  * @retval None
  */
void SGPIO_RX_StructInit(SGPIO_RXInitTypeDef* SGPIO_RXInitStruct)
{
	/* Load HAL initial data structure default value */
	SGPIO_RXInitStruct->RxDataSrc_Sel = RX_DATA_SRC_EXTERNAL_INPUT;
	SGPIO_RXInitStruct->RxTimerEdge_Sel = RX_TIMER_RISING_EDGE;
	SGPIO_RXInitStruct->RxPRVal = (2-1);
	SGPIO_RXInitStruct->RxPRTC = 0x0;
	SGPIO_RXInitStruct->RxTC = 0x0;
	SGPIO_RXInitStruct->RxData = 0x0;
	SGPIO_RXInitStruct->RxData_DP = 0x0;
	SGPIO_RXInitStruct->RxData_Dir = RX_DATA_DIR_MSB;
	SGPIO_RXInitStruct->RxPosTC = (8-1);
}

/**
  * @brief  Initializes the SGPIOx peripheral according to the specified 
  *			parameters in the SGPIO_RXInitTypeDef.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_RXInitStruct: pointer to a SGPIO_RXInitTypeDef structure that contains
  * 		the configuration information for the specified SGPIO peripheral.
  * @retval None
  */
void SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef* SGPIO_RXInitStruct)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(IS_RX_DATA_SRC(SGPIO_RXInitStruct->RxDataSrc_Sel));
	assert_param(IS_RX_TIMER_TRIGGER_MODE(SGPIO_RXInitStruct->RxTimerEdge_Sel));
	assert_param(SGPIO_RXInitStruct->RxPRVal <= 0xffff);
	assert_param(SGPIO_RXInitStruct->RxPRTC <= 0xffff);
	assert_param(SGPIO_RXInitStruct->RxTC <= 0xffff);
	assert_param(SGPIO_RXInitStruct->RxPosTC <= 0x1f);
	assert_param(IS_RX_DATA_DIR(SGPIO_RXInitStruct->RxData_Dir));

	/* Mask all interrupt */
	SGPIOx->SGPIO_IMR |= BIT_SGPIO_INTR_MASK;

	/* Disable the SGPIO RX Timer and RX Prescale Timer*/
	SGPIOx->SGPIO_RXTMR_CTRL &= (~BIT_SGPIO_RXTMR_CEN);
	
	/*Avoid clock domain crossing issue*/
	while(SGPIOx->SGPIO_RXTMR_CTRL & BIT_SGPIO_RXTMR_CEN) {
	}
		
	/* Set RX Timer Trigger Mode */
	TempVal = SGPIOx->SGPIO_RXTMR_CTRL;
	TempVal &= (~BIT_SGPIO_RXTMREDGE_SEL);
	TempVal |= SGPIO_RXInitStruct->RxTimerEdge_Sel;
	SGPIOx->SGPIO_RXTMR_CTRL = TempVal;

	SGPIOx->SGPIO_RXPRVAL = SGPIO_RXInitStruct->RxPRVal;
	SGPIOx->SGPIO_RXPRTC = SGPIO_RXInitStruct->RxPRTC;
	SGPIOx->SGPIO_RXTC = SGPIO_RXInitStruct->RxPRTC;

	/* Set RX Data Source */
	TempVal = SGPIOx->SGPIO_DATA_CTRL;
	TempVal &= (~BIT_SGPIO_RXDATASRC_SEL);
	TempVal |= SGPIO_RXInitStruct->RxDataSrc_Sel;
	SGPIOx->SGPIO_DATA_CTRL = TempVal;

	/* Set RX Data Direction */
	TempVal = SGPIOx->SGPIO_DATA_CTRL;
	TempVal &= (~BIT_SGPIO_RXDATA_DIR);
	TempVal |= SGPIO_RXInitStruct->RxData_Dir;
	SGPIOx->SGPIO_DATA_CTRL = TempVal;

	SGPIOx->SGPIO_RXPOSR = SGPIO_RXInitStruct->RxPosTC;

	/* Unmask all interrupt */
	SGPIOx->SGPIO_IMR &= (~BIT_SGPIO_INTR_MASK);

}

/**
  * @brief  Set the Multiple Timer control when RXMR0 matches RXTC.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_RXMR0Val: specifies the value of SGPIO RX Match 0 Register.
  * @param  SGPIO_RXMR0MULCtl: specifies the control of MULTC when RXMR0 matches RXTC.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_RX_MR0MULRST_EN: Reset MULTC .
  *            @arg BIT_SGPIO_RX_MR0MULSTOP_EN: MULTC will stop and MULTMR_CEN will be cleared.
  * @retval None
  */
void SGPIO_RXMR0MULConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR0Val, u32 SGPIO_RXMR0MULCtl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_RXMR0Val <= 0xffff);

	SGPIOx->SGPIO_RXMR0 = SGPIO_RXMR0Val;

	TempVal = SGPIOx->SGPIO_RXMC_CTRL;
	TempVal &= (~BIT_SGPIO_RX_MR0MUL_CTRL);
	TempVal |= SGPIO_RXMR0MULCtl;
	SGPIOx->SGPIO_RXMC_CTRL = TempVal;
}

/**
  * @brief  Get RX Data from rx data duplicate register.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @retval   RX Data in duplicate register
  */
u32 SGPIO_GetRXData(SGPIO_TypeDef *SGPIOx)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	return SGPIOx->SGPIO_RXDATA_DP;
}

/**
  * @brief  Set the reload value of RXPOSTC and control of RXTC.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_PosRstVal: specifies the reload value of RXPOSTC.
  * @param  SGPIO_RXPOSCtl: specifies the control of RXTC when RXPOSTC reaches 0.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_RX_POSRST_EN: Reset RXTC .
  *            @arg BIT_SGPIO_RX_POSSTOP_EN: RXTC will stop and MULTMR_CEN will be cleared.
  * @retval None
  */
void SGPIO_RXPosConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_PosRstVal, u32 SGPIO_RXPOSCtl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_PosRstVal <= 0x1f);

	TempVal = SGPIOx->SGPIO_RXMC_CTRL;
	TempVal &= (~BIT_SGPIO_RX_POS_CTRL);
	TempVal |= SGPIO_RXPOSCtl;
	SGPIOx->SGPIO_RXMC_CTRL = TempVal;

	TempVal = SGPIOx->SGPIO_RXPOSR;
	TempVal &= (~BIT_SGPIO_RXPOSRST);
	TempVal |= (SGPIO_PosRstVal << 8);
	SGPIOx->SGPIO_RXPOSR = TempVal;
}

/**
  * @brief  Set the SGPIO RXMR0.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_RXMR0Val: specifies the value of SGPIO RX Match 0 Register.
  * @param  SGPIO_RXMR0Ctl: specifies the control of RXTC when RXMR0 matches RXTC.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_RX_MR0_IE: Generate an interrupt .
  *            @arg BIT_SGPIO_RX_MR0RST_EN: Reset RXTC .
  *            @arg BIT_SGPIO_RX_MR0STOP_EN: RXTC will stop and RXTMR_CEN will be cleared.
  *            @arg BIT_SGPIO_RX_MR0SCLK_EN: Generate a shift clock for RX Data Register.
  * @retval None
  */
void SGPIO_RXMR0Config(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR0Val, u32 SGPIO_RXMR0Ctl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_RXMR0Val <= 0xffff);

	SGPIOx->SGPIO_RXMR0 = SGPIO_RXMR0Val;

	TempVal = SGPIOx->SGPIO_RXMC_CTRL;
	TempVal &= (~BIT_SGPIO_RX_MR0_CTRL);
	TempVal |= SGPIO_RXMR0Ctl;
	SGPIOx->SGPIO_RXMC_CTRL = TempVal;
}

/**
  * @brief  Set the SGPIO RXMR1.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_RXMR1Val: specifies the value of SGPIO RX Match 1 Register.
  * @param  SGPIO_RXMR1Ctl: specifies the control of RXTC when RXMR1 matches RXTC.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_RX_MR1_IE: Generate an interrupt .
  *            @arg BIT_SGPIO_RX_MR1RST_EN: Reset RXTC .
  *            @arg BIT_SGPIO_RX_MR1STOP_EN: RXTC will stop and RXTMR_CEN will be cleared.
  *            @arg BIT_SGPIO_RX_MR1SCLK_EN: Generate a shift clock for RX Data Register.
  * @retval None
  */
void SGPIO_RXMR1Config(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR1Val, u32 SGPIO_RXMR1Ctl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_RXMR1Val <= 0xffff);

	SGPIOx->SGPIO_RXMR1 = SGPIO_RXMR1Val;

	TempVal = SGPIOx->SGPIO_RXMC_CTRL;
	TempVal &= (~BIT_SGPIO_RX_MR1_CTRL);
	TempVal |= SGPIO_RXMR1Ctl;
	SGPIOx->SGPIO_RXMC_CTRL = TempVal;
}

/**
  * @brief  Set the SGPIO RXMR2.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_RXMR2Val: specifies the value of SGPIO RX Match 2 Register.
  * @param  SGPIO_RXMR2Ctl: specifies the control of RXTC when RXMR0 matches RXTC.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_RX_MR2_IE: Generate an interrupt .
  *            @arg BIT_SGPIO_RX_MR2RST_EN: Reset RXTC .
  *            @arg BIT_SGPIO_RX_MR2STOP_EN: RXTC will stop and RXTMR_CEN will be cleared.
  *            @arg BIT_SGPIO_RX_MR2SCLK_EN: Generate a shift clock for RX Data Register.
  * @retval None
  */
void SGPIO_RXMR2Config(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR2Val, u32 SGPIO_RXMR2Ctl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_RXMR2Val <= 0xffff);

	SGPIOx->SGPIO_RXMR2 = SGPIO_RXMR2Val;

	TempVal = SGPIOx->SGPIO_RXMC_CTRL;
	TempVal &= (~BIT_SGPIO_RX_MR2_CTRL);
	TempVal |= SGPIO_RXMR2Ctl;
	SGPIOx->SGPIO_RXMC_CTRL = TempVal;
}

/**
  * @brief  Set the SGPIO RX Match Register Output control.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_RXMR0TXCtl: specifies the output control when RXMR0 matches RXTC.
  *          This parameter can be a value of @ref RXMR0_Output_Control_Options
  * @param  SGPIO_RXMR1TXCtl: specifies the output control when RXMR1 matches RXTC.
  *          This parameter can be a value of @ref RXMR1_Output_Control_Options
  * @param  SGPIO_RXMR2TXCtl: specifies the output control when RXMR2 matches RXTC. 
  *          This parameter can be a value of @ref RXMR2_Output_Control_Options
  * @retval None
  */
void SGPIO_RXMRxTXConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR0TXCtl, u32 SGPIO_RXMR1TXCtl, u32 SGPIO_RXMR2TXCtl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(IS_RXMR0_OUTCTRL_OPTION(SGPIO_RXMR0TXCtl));
	assert_param(IS_RXMR1_OUTCTRL_OPTION(SGPIO_RXMR1TXCtl));
	assert_param(IS_RXMR2_OUTCTRL_OPTION(SGPIO_RXMR2TXCtl));

	TempVal = SGPIOx->SGPIO_MULEMR;
	TempVal &= (~BIT_SGPIO_RXEMC_CTRL);
	TempVal |= (SGPIO_RXMR0TXCtl | SGPIO_RXMR1TXCtl | SGPIO_RXMR2TXCtl);
	SGPIOx->SGPIO_MULEMR = TempVal;
}


/**
  * @brief  Reset the SGPIO RX Timer and RX Prescale Timer.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @retval None
  */
void SGPIO_RXTmr_Reset(SGPIO_TypeDef *SGPIOx)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	/* Reset the SGPIO RX Timer and RX Prescale Timer*/
	SGPIOx->SGPIO_RXTMR_CTRL |= BIT_SGPIO_RXTMR_CRST;
}

/**
  * @brief  Enables or disables the SGPIO RX Timer and RX Prescale Timer.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  NewState: new state of the RX Timer and RX Prescale Timer. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_RXTmr_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	if (NewState != DISABLE) {
		/* Enable the SGPIO RX Timer and RX Prescale Timer*/
		SGPIOx->SGPIO_RXTMR_CTRL |= BIT_SGPIO_RXTMR_CEN;

		/*Avoid clock domain crossing issue*/
		while(!(SGPIOx->SGPIO_RXTMR_CTRL & BIT_SGPIO_RXTMR_CEN)) {
		}
	} else {
		/* Disable the SGPIO RX Timer and RX Prescale Timer*/
		SGPIOx->SGPIO_RXTMR_CTRL &= (~BIT_SGPIO_RXTMR_CEN);

		/*Avoid clock domain crossing issue*/
		while(SGPIOx->SGPIO_RXTMR_CTRL & BIT_SGPIO_RXTMR_CEN) {
		}
	}
}

/**
  * @brief  Set RX Pattern Match Mode.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  RXDATADP: the specific value of RXDATA match pattern should be set.
  * @param  RXDATAMask: mask for pattern match function. 
  * @param  NewState: new state of the patten match function. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_RXPatternMatchConfig(SGPIO_TypeDef *SGPIOx, u32 RXDATADP, u32 RXDATAMask, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	if (NewState != DISABLE) {
		SGPIOx->SGPIO_RXDATAMASK = RXDATAMask & BIT_SGPIO_RXDATAMASK;

		SGPIOx->SGPIO_RXDATA_DP = RXDATADP;
		/* Enable the patten match function*/
		SGPIOx->SGPIO_DATA_CTRL |= BIT_SGPIO_PATTERN_MATCH_MODE;
	} else {
		/* Disable the patten match function*/
		SGPIOx->SGPIO_DATA_CTRL &= (~BIT_SGPIO_PATTERN_MATCH_MODE);
	}
}

/**
  * @brief  Fills each SGPIO_MULInitStruct member with its default value.
  * @param  SGPIO_MULInitStruct: pointer to an SGPIO_MULInitTypeDef structure which will be initialized.
  * @retval None
  */
void SGPIO_MUL_StructInit(SGPIO_MULInitTypeDef* SGPIO_MULInitStruct)
{
	/* Load HAL initial data structure default value */
	SGPIO_MULInitStruct->BiOut = MUL_DISABLE_BIOUT;
	SGPIO_MULInitStruct->MulMode_Sel = MUL_TIMER_MODE;
	SGPIO_MULInitStruct->MulPRVal = (2-1);
	SGPIO_MULInitStruct->MulPRTC = 0x0;
	SGPIO_MULInitStruct->MulTC = 0x0;
	SGPIO_MULInitStruct->MulData_Dir = MUL_DATA_DIR_LSB;
	SGPIO_MULInitStruct->MulPosTC = (8-1);
	SGPIO_MULInitStruct->MulPosRST = (8-1);
}

/**
  * @brief  Initializes the SGPIOx peripheral according to the specified 
  *			parameters in the SGPIO_MULInitTypeDef.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULInitStruct: pointer to a SGPIO_MULInitTypeDef structure that contains
  * 		the configuration information for the specified SGPIO peripheral.
  * @retval None
  */
void SGPIO_MUL_Init(SGPIO_TypeDef *SGPIOx, SGPIO_MULInitTypeDef* SGPIO_MULInitStruct)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(IS_MUL_BIOUT_MODE(SGPIO_MULInitStruct->BiOut));
	assert_param(IS_MUL_MODE(SGPIO_MULInitStruct->MulMode_Sel));
	assert_param(SGPIO_MULInitStruct->MulPRVal <= 0xffff);
	assert_param(SGPIO_MULInitStruct->MulPRTC <= 0xffff);
	assert_param(SGPIO_MULInitStruct->MulTC <= 0xffff);
	assert_param(SGPIO_MULInitStruct->MulPosTC <= 0x1f);
	assert_param(SGPIO_MULInitStruct->MulPosRST <= 0x1f);	
	assert_param(IS_MUL_DATA_DIR(SGPIO_MULInitStruct->MulData_Dir));

	/* Mask all interrupt */
	SGPIOx->SGPIO_IMR |= BIT_SGPIO_INTR_MASK;

	/* Disable the SGPIO Multiple Timer and Multiple Prescale Timer*/
	SGPIOx->SGPIO_MULTMR_CTRL &= (~BIT_SGPIO_MULTMR_CEN);

	/*Avoid clock domain crossing issue*/
	while(SGPIOx->SGPIO_MULTMR_CTRL & BIT_SGPIO_MULTMR_CEN) {
	}
	
	/* Set Multiple Timer TX pin */
	TempVal = SGPIOx->SGPIO_MULEMR;
	TempVal &= (~BIT_SGPIO_BIOUT);
	TempVal |= SGPIO_MULInitStruct->BiOut;
	SGPIOx->SGPIO_MULEMR = TempVal;
	
	/* Set Multiple Timer Work Mode */
	TempVal = SGPIOx->SGPIO_MULTMR_CTRL;
	TempVal &= (~BIT_SGPIO_MULMODE_SEL);
	TempVal |= SGPIO_MULInitStruct->MulMode_Sel;
	SGPIOx->SGPIO_MULTMR_CTRL = TempVal;

	SGPIOx->SGPIO_MULPRVAL = SGPIO_MULInitStruct->MulPRVal;
	SGPIOx->SGPIO_MULPRTC = SGPIO_MULInitStruct->MulPRTC;
	SGPIOx->SGPIO_MULTC = SGPIO_MULInitStruct->MulPRTC;

	/* Set MUL Data Direction */
	TempVal = SGPIOx->SGPIO_DATA_CTRL;
	TempVal &= (~BIT_SGPIO_TXDATA_DIR);
	TempVal |= SGPIO_MULInitStruct->MulData_Dir;
	SGPIOx->SGPIO_DATA_CTRL = TempVal;

	SGPIOx->SGPIO_MULPOSR = (SGPIO_MULInitStruct->MulPosRST << 8) |SGPIO_MULInitStruct->MulPosTC;

	/* Unmask all interrupt */
	SGPIOx->SGPIO_IMR &= (~BIT_SGPIO_INTR_MASK);
}


/**
  * @brief  Set the SGPIO MULMR0.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULMR0Val0: specifies the value of MULMR0 when TXDATA[0] is 0.
  * @param  SGPIO_MULMR0Val1: specifies the value of MULMR0 when TXDATA[0] is 1.
  * @param  SGPIO_MULMR0Ctl: specifies the control of MULTC when MULMR0 matches MULTC.
  *          This parameter can be one or combinations of the following values:
  *            @arg NULL: No control.
  *            @arg BIT_SGPIO_MUL_MR0_IE: Generate an interrupt.
  *            @arg BIT_SGPIO_MUL_MR0RST_EN: Reset MULTC.
  *            @arg BIT_SGPIO_MUL_MR0STOP_EN: MULTC will stop and MULTMR_CEN will be cleared.
  *            @arg BIT_SGPIO_MUL_MR0SCLK_EN: Generate a shift clock for Multiple Data Register.
  * @retval None
  */
void SGPIO_MULMR0MulConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0Val0, u32 SGPIO_MULMR0Val1, u32 SGPIO_MULMR0Ctl)
{
	u32 TempVal;
	
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_MULMR0Val0 <= 0xffff);
	assert_param(SGPIO_MULMR0Val1 <= 0xffff);

	TempVal = SGPIOx->SGPIO_MULMR01GP0;
	TempVal &= (~BIT_SGPIO_MULMR0_GP0);
	TempVal |= SGPIO_MULMR0Val0;
	SGPIOx->SGPIO_MULMR01GP0 = TempVal;

	TempVal = SGPIOx->SGPIO_MULMR01GP1;
	TempVal &= (~BIT_SGPIO_MULMR0_GP1);
	TempVal |= SGPIO_MULMR0Val1;
	SGPIOx->SGPIO_MULMR01GP1 = TempVal;

	TempVal = SGPIOx->SGPIO_MULMC_CTRL;
	TempVal &= (~BIT_SGPIO_MUL_MR0_CTRL);
	TempVal |= SGPIO_MULMR0Ctl;
	SGPIOx->SGPIO_MULMC_CTRL = TempVal;
}

/**
  * @brief  Set the SGPIO MULMR0 and control of RXTC.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULMR0Val0: specifies the value of MULMR0 when TXDATA[0] is 0.
  * @param  SGPIO_MULMR0Val1: specifies the value of MULMR0 when TXDATA[0] is 1.
  * @param  SGPIO_MULMR0RXCtl: specifies the control of RXTC when MULMR0 matches MULTC.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_MUL_MR0RXRST_EN: Reset RXTC .
  *            @arg BIT_SGPIO_MUL_MR0RXSTOP_EN: RXTC will stop and RXTMR_CEN will be cleared.
  * @retval None
  */
void SGPIO_MULMR0RXConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0Val0, u32 SGPIO_MULMR0Val1, u32 SGPIO_MULMR0RXCtl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_MULMR0Val0 <= 0xffff);
	assert_param(SGPIO_MULMR0Val1 <= 0xffff);

	TempVal = SGPIOx->SGPIO_MULMR01GP0;
	TempVal &= (~BIT_SGPIO_MULMR0_GP0);
	TempVal |= SGPIO_MULMR0Val0;
	SGPIOx->SGPIO_MULMR01GP0 = TempVal;

	TempVal = SGPIOx->SGPIO_MULMR01GP1;
	TempVal &= (~BIT_SGPIO_MULMR0_GP1);
	TempVal |= SGPIO_MULMR0Val1;
	SGPIOx->SGPIO_MULMR01GP1 = TempVal;

	TempVal = SGPIOx->SGPIO_MULMC_CTRL;
	TempVal &= (~BIT_SGPIO_MUL_MR0RX_CTRL);
	TempVal |= SGPIO_MULMR0RXCtl;
	SGPIOx->SGPIO_MULMC_CTRL = TempVal;
}

/**
  * @brief  Set the SGPIO MULMRx register for group 0.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULMR0GP0: specifies the value of MULMR0 when TXDATA[0] is 0.
  * @param  SGPIO_MULMR1GP0: specifies the value of MULMR1 when TXDATA[0] is 0.
  * @param  SGPIO_MULMR2GP0: specifies the value of MULMR2 when TXDATA[0] is 0.
  * @param  SGPIO_MULMR3GP0: specifies the value of MULMR3 when TXDATA[0] is 0.
  * @retval None
  */
void SGPIO_MULMRxGP0ValConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0GP0, u32 SGPIO_MULMR1GP0, u32 SGPIO_MULMR2GP0, u32 SGPIO_MULMR3GP0)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_MULMR0GP0 <= 0xffff);
	assert_param(SGPIO_MULMR1GP0 <= 0xffff);
	assert_param(SGPIO_MULMR2GP0 <= 0xffff);
	assert_param(SGPIO_MULMR3GP0 <= 0xffff);

	SGPIOx->SGPIO_MULMR01GP0 = (SGPIO_MULMR1GP0 << 16) | SGPIO_MULMR0GP0;
	SGPIOx->SGPIO_MULMR23GP0 = (SGPIO_MULMR3GP0 << 16) | SGPIO_MULMR2GP0;
}

/**
  * @brief  Set the SGPIO MULMRx register for group 1.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULMR0GP1: specifies the value of MULMR0 when TXDATA[0] is 1.
  * @param  SGPIO_MULMR1GP1: specifies the value of MULMR1 when TXDATA[0] is 1.
  * @param  SGPIO_MULMR2GP1: specifies the value of MULMR2 when TXDATA[0] is 1.
  * @param  SGPIO_MULMR3GP1: specifies the value of MULMR3 when TXDATA[0] is 1.
  * @retval None
  */
void SGPIO_MULMRxGP1ValConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0GP1, u32 SGPIO_MULMR1GP1, u32 SGPIO_MULMR2GP1, u32 SGPIO_MULMR3GP1)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_MULMR0GP1 <= 0xffff);
	assert_param(SGPIO_MULMR1GP1 <= 0xffff);
	assert_param(SGPIO_MULMR2GP1 <= 0xffff);
	assert_param(SGPIO_MULMR3GP1 <= 0xffff);

	SGPIOx->SGPIO_MULMR01GP1 = (SGPIO_MULMR1GP1 << 16) | SGPIO_MULMR0GP1;
	SGPIOx->SGPIO_MULMR23GP1 = (SGPIO_MULMR3GP1 << 16) | SGPIO_MULMR2GP1;
}

/**
  * @brief  Set the SGPIO MULMRx control.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULMR0TXCtl: specifies the control of MULTC when MULMR0 matches MULTC.
  *          This parameter can be one or combinations of the following values:
  *            @arg NULL: No control.
  *            @arg BIT_SGPIO_MUL_MR0_IE: Generate an interrupt.
  *            @arg BIT_SGPIO_MUL_MR0RST_EN: Reset MULTC.
  *            @arg BIT_SGPIO_MUL_MR0STOP_EN: MULTC will stop and MULTMR_CEN will be cleared.
  *            @arg BIT_SGPIO_MUL_MR0SCLK_EN: Generate a shift clock for Multiple Data Register.
  * @param  SGPIO_MULMR1TXCtl: specifies the output control when MULMR1 matches MULTC.
  *          This parameter can be a value of @ref MULMR1_Output_Control_Option
  * @param  SGPIO_MULMR2TXCtl: specifies the output control when MULMR2 matches MULTC.
  *          This parameter can be a value of @ref MULMR2_Output_Control_Option
  * @param  SGPIO_MULMR3TXCtl: specifies the output control when MULMR3 matches MULTC.
  *          This parameter can be a value of @ref MULMR3_Output_Control_Option
  * @retval None
  */
void SGPIO_MULMRxTXCtlConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0TXCtl, u32 SGPIO_MULMR1TXCtl, u32 SGPIO_MULMR2TXCtl, u32 SGPIO_MULMR3TXCtl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(IS_MULMR1_OUTCTRL_OPTION(SGPIO_MULMR1TXCtl));
	assert_param(IS_MULMR2_OUTCTRL_OPTION(SGPIO_MULMR2TXCtl));
	assert_param(IS_MULMR3_OUTCTRL_OPTION(SGPIO_MULMR3TXCtl));

	/*set MULMR0 control*/
	TempVal = SGPIOx->SGPIO_MULMC_CTRL;
	TempVal &= (~BIT_SGPIO_MUL_MR0_CTRL);
	TempVal |= SGPIO_MULMR0TXCtl;
	SGPIOx->SGPIO_MULMC_CTRL = TempVal;

	/*set MULMRx output control*/
	TempVal = SGPIOx->SGPIO_MULEMR;
	TempVal &= (~(BIT_SGPIO_MULEMC1 | BIT_SGPIO_MULEMC2 | BIT_SGPIO_MULEMC3));
	TempVal |= (SGPIO_MULMR1TXCtl | SGPIO_MULMR2TXCtl | SGPIO_MULMR3TXCtl);
	SGPIOx->SGPIO_MULEMR = TempVal;
}

/**
  * @brief  Set TX Data to multiple data register.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MulDataVal: specifies the value of SGPIO_MULDATA
  * @param  SGPIO_MulDataDPVal: specifies the reload value of SGPIO_MULDATA
  * @retval None
  */
void SGPIO_SetTXData(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MulDataVal, u32 SGPIO_MulDataDPVal)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_MulDataVal <= 0xffffffff);
	assert_param(SGPIO_MulDataDPVal <= 0xffffffff);

	SGPIOx->SGPIO_MULDATA = SGPIO_MulDataVal;
	SGPIOx->SGPIO_MULDATA_DP = SGPIO_MulDataDPVal;
}

/**
  * @brief  Config output level.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_OutputCtl: specifies the value of SGPIO_OUTPUT
  *          This parameter can be one of the following values:
  *            @arg BIT_SGPIO_OUTPUT_LOW: output low.
  *            @arg BIT_SGPIO_OUTPUT_HIGH: output high.
  * @retval None
  */
void SGPIO_OutputConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_OutputCtl)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	SGPIOx->SGPIO_OUTPUT = SGPIO_OutputCtl;
}

/** 
  * @brief  Reset the SGPIO Multiple Timer and Multiple Prescale Timer.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @retval None
  */
void SGPIO_MULTmr_Reset(SGPIO_TypeDef *SGPIOx)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	/* Reset the SGPIO Multiple Timer and Multiple Prescale Timer*/
	SGPIOx->SGPIO_MULTMR_CTRL |= BIT_SGPIO_MULTMR_CRST;
}

/**
  * @brief  Enables or disables the SGPIO Multiple Timer and Multiple Prescale Timer.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  NewState: new state of the Multiple Timer and Multiple Prescale Timer. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_MULTmr_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	if (NewState != DISABLE) {
		/* Enable the SGPIO Multiple Timer and Multiple Prescale Timer*/
		SGPIOx->SGPIO_MULTMR_CTRL |= BIT_SGPIO_MULTMR_CEN;

		/*Avoid clock domain crossing issue*/
		while(!(SGPIOx->SGPIO_MULTMR_CTRL & BIT_SGPIO_MULTMR_CEN)) {
		}
	} else {
		/* Disable the SGPIO Multiple Timer and Multiple Prescale Timer*/
		SGPIOx->SGPIO_MULTMR_CTRL &= (~BIT_SGPIO_MULTMR_CEN);

		/*Avoid clock domain crossing issue*/
		while(SGPIOx->SGPIO_MULTMR_CTRL & BIT_SGPIO_MULTMR_CEN) {
		}
	}
}

/**
  * @brief  Set the SGPIO MULMR0.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULMCNTVal: specifies the value of MUL_MCNT.
  * @param  SGPIO_MULMR0Ctl: specifies the control of MULTC when MUL_MCNT reaches zero.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_MUL_MCNT_IE: Generate an interrupt .
  *            @arg BIT_SGPIO_MUL_MCNTRST_EN: Reset MULTC .
  *            @arg BIT_SGPIO_MUL_MCNTSTOP_EN: MULTC will stop and MULTMR_CEN will be cleared.
  * @retval None
  */
void SGPIO_MULMCNTConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMCNTVal, u32 SGPIO_MULMCNTCtl)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(SGPIO_MULMCNTVal <= 0xff);

	SGPIOx->SGPIO_MUL_MCNT = SGPIO_MULMCNTVal;

	TempVal = SGPIOx->SGPIO_MULMC_CTRL;
	TempVal &= (~BIT_SGPIO_MUL_MCNT_CTRL);
	TempVal |= SGPIO_MULMCNTCtl;
	SGPIOx->SGPIO_MULMC_CTRL = TempVal;
}

/**
  * @brief  Enables or disables the SGPIO MCNT function.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  NewState: new state of the MCNT function. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_MULMCNT_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	if (NewState != DISABLE) {
		/* Enable the SGPIO MCNT function*/
		SGPIOx->SGPIO_MULMC_CTRL |= BIT_SGPIO_MUL_MR0MCNT_EN;
	} else {
		/* Disable the SGPIO MCNT function*/
		SGPIOx->SGPIO_MULMC_CTRL &= (~BIT_SGPIO_MUL_MR0MCNT_EN);
	}
}

/**
  * @brief  Fills each SGPIO_CAPInitStruct member with its default value.
  * @param  SGPIO_CAPInitStruct: pointer to an SGPIO_CAPInitTypeDef structure which will be initialized.
  * @retval None
  */
void SGPIO_CAP_StructInit(SGPIO_CAPInitTypeDef* SGPIO_CAPInitStruct)
{
	/* Load HAL initial data structure default value */
	SGPIO_CAPInitStruct->CapSrc_Sel = CAPSRC_RX_INPUT;
	SGPIO_CAPInitStruct->CapEdge_Sel = CAP_RX_FALLING_EDGE;
	SGPIO_CAPInitStruct->Cap_ValidEn = CAP_RX_VALID_ENABLE;
	SGPIO_CAPInitStruct->Cap_RxTCStop_Ctrl = CAP_RX_STOP_ENABLE;
	SGPIO_CAPInitStruct->Cap_RxTCRst_Ctrl = CAP_RX_RESET_ENABLE;
	SGPIO_CAPInitStruct->CapI_En = CAP_INTR_ENABLE;
}

/**
  * @brief  Initializes the SGPIOx peripheral according to the specified 
  *			parameters in the SGPIO_CAPInitTypeDef.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_CAPInitStruct: pointer to a SGPIO_CAPInitTypeDef structure that contains
  * 		the configuration information for the specified SGPIO peripheral.
  * @retval None
  */
void SGPIO_CAP_Init(SGPIO_TypeDef *SGPIOx, SGPIO_CAPInitTypeDef* SGPIO_CAPInitStruct)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(IS_CAPSRC_SEL_OPTION(SGPIO_CAPInitStruct->CapSrc_Sel));
	assert_param(IS_CAPEDGE_SEL_OPTION(SGPIO_CAPInitStruct->CapEdge_Sel));
	assert_param(IS_CAP_RX_VALID_OPTION(SGPIO_CAPInitStruct->Cap_ValidEn));
	assert_param(IS_CAP_RX_STOP_OPTION(SGPIO_CAPInitStruct->Cap_RxTCStop_Ctrl));
	assert_param(IS_CAP_RX_RESET_OPTION(SGPIO_CAPInitStruct->Cap_RxTCRst_Ctrl));
	assert_param(IS_CAP_INTR_OPTION(SGPIO_CAPInitStruct->CapI_En));

	TempVal = SGPIOx->SGPIO_CAP_CTRL;
	TempVal &= (~(BIT_SGPIO_CAPVALID_EN | BIT_SGPIO_CAP_RXTCSTOP_EN | BIT_SGPIO_CAP_RXTCRST_EN |\
		BIT_SGPIO_CAP_RXEDGE_SEL | BIT_SGPIO_CAPSRC_SEL | BIT_SGPIO_CAP_IE));

	if (CAPSRC_RX_INPUT == SGPIO_CAPInitStruct->CapSrc_Sel) {
		TempVal |= (SGPIO_CAPInitStruct->CapSrc_Sel | SGPIO_CAPInitStruct->CapEdge_Sel);
	} else {
		TempVal |= SGPIO_CAPInitStruct->CapSrc_Sel;
		SGPIOx->SGPIO_MULMC_CTRL |= BIT_SGPIO_MUL_MR0CAPSRC_EN;
	}

	TempVal |= (SGPIO_CAPInitStruct->Cap_ValidEn | SGPIO_CAPInitStruct->Cap_RxTCStop_Ctrl |\
		SGPIO_CAPInitStruct->Cap_RxTCRst_Ctrl | SGPIO_CAPInitStruct->CapI_En);
	
	SGPIOx->SGPIO_CAP_CTRL = TempVal;
}

/**
  * @brief  Get SGPIO RX timer counter capture value.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @retval RX timer counter capture Value
  */
u32 SGPIO_GetCapVal(SGPIO_TypeDef *SGPIOx)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	
	return SGPIOx->SGPIO_CAPR;
}

/**
  * @brief  set SGPIO Capture compare function.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  CapComp_Val: compare this value with CAPR. 
  * @param  CapComp_Mode: RXDATA receives value set when CAPR is bigger than CapComp_Val. 
  *   This parameter can be a value of @ref Cap_CmpVal_Sel_definitions.
  * @retval None
  */
void SGPIO_Cap_CompConfig(SGPIO_TypeDef *SGPIOx, u32 CapComp_Val, u32 CapComp_Mode)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	assert_param(CapComp_Val <= 0xffff);
	assert_param(IS_CAP_CMPVAL_SEL_OPTION(CapComp_Mode));

	TempVal = SGPIOx->SGPIO_CAP_CTRL;
	TempVal &= (~(BIT_SGPIO_CAP_CMPVAL | BIT_SGPIO_CAP_CMPVAL_SEL));
	TempVal |= ((CapComp_Val << 16) | CapComp_Mode);
	SGPIOx->SGPIO_CAP_CTRL = TempVal;
}

/**
  * @brief  Enables or disables the SGPIO Capture Function.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  NewState: new state of the Capture function. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_Cap_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	if (NewState != DISABLE) {
		/* Enable the CAPR load with RXTC when there is an event*/
		SGPIOx->SGPIO_CAP_CTRL |= BIT_SGPIO_CAP_EN;
	} else {
		/* Disable the CAPR load with RXTC when there is an event*/
		SGPIOx->SGPIO_CAP_CTRL &= (~BIT_SGPIO_CAP_EN);
	}
}

/**
  * @brief  Set the SGPIO Interrupt Enable.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_IT: specifies the SGPIO interrupt sources to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_PATTERN_MATCH_IE: Matching pattern Interrupt Enable.
  *            @arg BIT_SGPIO_MULLOAD_IE: Loading multiple data Interrupt Enable.
  *            @arg BIT_SGPIO_RXLOAD_IE: Loading RX data Interrupt Enable.
  *            @arg BIT_SGPIO_MULSFT_IE: Multiple shift clock Interrupt Enable.
  *            @arg BIT_SGPIO_RXSFT_IE: RX shift clock Interrupt Enable.
  * @param  NewState: specifies the state of the interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_INTConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_IT, u32 NewState)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	TempVal = SGPIOx->SGPIO_DATA_CTRL;

	if (NewState == ENABLE) {
		TempVal |= SGPIO_IT;
	} else {
		TempVal &= (~SGPIO_IT);
	}

	SGPIOx->SGPIO_DATA_CTRL = TempVal;
}

/**
  * @brief  Set the SGPIO Interrupt Mask.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_IT: specifies the SGPIO Interrupt sources to be masked or not.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_MULMCNT_IM: MUL_MCNT counts to zero Interrupt Mask.
  *            @arg BIT_SGPIO_PATTERN_MATCH_IM: Matching pattern Interrupt Mask.
  *            @arg BIT_SGPIO_MULLOAD_IM: Loading multiple data Interrupt Mask.
  *            @arg BIT_SGPIO_RXLOAD_IM: Loading RX data Interrupt Mask.
  *            @arg BIT_SGPIO_MULSFT_IM: Multiple shift clock Interrupt Mask.
  *            @arg BIT_SGPIO_RXSFT_IM: RX shift clock Interrupt Mask.
  *            @arg BIT_SGPIO_CAPI_IM: Capture load event Interrupt Mask.
  *            @arg BIT_SGPIO_MUL_MR0I_IM: Multiple MR0 match Interrupt Mask.
  *            @arg BIT_SGPIO_RX_MR2I_IM: RX MR2 match Interrupt Mask.
  *            @arg BIT_SGPIO_RX_MR1I_IM: RX MR1 match Interrupt Mask.
  *            @arg BIT_SGPIO_RX_MR0I_IM: RX MR0 match Interrupt Mask.
  * @param  NewState: specifies the mask state of the interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_INTMask(SGPIO_TypeDef *SGPIOx, u32 SGPIO_IT, u32 NewState)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	TempVal = SGPIOx->SGPIO_IMR;

	if (NewState == ENABLE) {
		TempVal |= SGPIO_IT;
	} else {
		TempVal &= (~SGPIO_IT);
	}

	SGPIOx->SGPIO_IMR = TempVal;
}

/**
  * @brief  Get SGPIO Raw Interrupt Status.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @retval SGPIO raw interrupt status
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_MULMCNT_IS: MUL_MCNT counts to zero Interrupt Raw Status.
  *            @arg BIT_SGPIO_PATTERN_MATCH_IS: Matching pattern Interrupt Raw Status.
  *            @arg BIT_SGPIO_MULLOAD_IS: Loading multiple data Interrupt Raw Status.
  *            @arg BIT_SGPIO_RXLOAD_IS: Loading RX data Interrupt Raw Status.
  *            @arg BIT_SGPIO_MULSFT_IS: Multiple shift clock Interrupt Raw Status.
  *            @arg BIT_SGPIO_RXSFT_IS: RX shift clock Interrupt Raw Status.
  *            @arg BIT_SGPIO_CAPI_IS: Capture load event Interrupt Raw Status.
  *            @arg BIT_SGPIO_MUL_MR0I_IS: Multiple MR0 match Interrupt Raw Status.
  *            @arg BIT_SGPIO_RX_MR2I_IS: RX MR2 match Interrupt Raw Status.
  *            @arg BIT_SGPIO_RX_MR1I_IS: RX MR1 match Interrupt Raw Status.
  *            @arg BIT_SGPIO_RX_MR0I_IS: RX MR0 match Interrupt Raw Status.
  */
u32 SGPIO_GetRawINT(SGPIO_TypeDef *SGPIOx)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));
	
	return SGPIOx->SGPIO_ISR;
}

/**
  * @brief  clear SGPIO Interrupt Status.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_IT: SGPIO interrupt status to be cleared
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_SGPIO_MULMCNT_IS: MUL_MCNT counts to zero Interrupt Raw Status.
  *            @arg BIT_SGPIO_PATTERN_MATCH_IS: Matching pattern Interrupt Raw Status.
  *            @arg BIT_SGPIO_MULLOAD_IS: Loading multiple data Interrupt Raw Status.
  *            @arg BIT_SGPIO_RXLOAD_IS: Loading RX data Interrupt Raw Status.
  *            @arg BIT_SGPIO_MULSFT_IS: Multiple shift clock Interrupt Raw Status.
  *            @arg BIT_SGPIO_RXSFT_IS: RX shift clock Interrupt Raw Status.
  *            @arg BIT_SGPIO_CAPI_IS: Capture load event Interrupt Raw Status.
  *            @arg BIT_SGPIO_MUL_MR0I_IS: Multiple MR0 match Interrupt Raw Status.
  *            @arg BIT_SGPIO_RX_MR2I_IS: RX MR2 match Interrupt Raw Status.
  *            @arg BIT_SGPIO_RX_MR1I_IS: RX MR1 match Interrupt Raw Status.
  *            @arg BIT_SGPIO_RX_MR0I_IS: RX MR0 match Interrupt Raw Status.
  */
void SGPIO_ClearRawINT(SGPIO_TypeDef *SGPIOx, u32 SGPIO_IT)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	SGPIOx->SGPIO_ISR = SGPIO_IT;
}
/**
  * @brief  Enables or disables the SGPIO multiple match FIFO mode.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  NewState: new state of the multiple match FIFO mode. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_MULFIFO_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	if (NewState != DISABLE) {
		/* Enable the SGPIO multiple match FIFO mode*/
		SGPIOx->SGPIO_MULMC_CTRL |= BIT_SGPIO_MUL_FMODE_EN;
	} else {
		/* Disable the SGPIO multiple match FIFO mode*/
		SGPIOx->SGPIO_MULMC_CTRL &= (~BIT_SGPIO_MUL_FMODE_EN);
	}
}

/**
  * @brief  Set SGPIO multiple match FIFO data.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  SGPIO_MULFIFOVal: new data to be pushed into multiple match FIFO. 
  * @retval None
  */
void SGPIO_MULFIFO_Set(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULFIFOVal)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	SGPIOx->SGPIO_MULFDATA = SGPIO_MULFIFOVal;
}

/**
  * @brief  Enables or disables the SGPIO multiple match DMA mode.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  NewState: new state of the multiple match FIFO mode. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SGPIO_MULDMA_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_SGPIO_ALL_PERIPH(SGPIOx));

	if (NewState != DISABLE) {
		/* Enable the SGPIO multiple match DMA mode*/
		SGPIOx->SGPIO_MULMC_CTRL |= BIT_SGPIO_MUL_DMA_EN;

		/* Trigger the SGPIO multiple match FIFO load with data for the first time*/
		SGPIOx->SGPIO_MULMC_CTRL |= BIT_SGPIO_MUL_DMA_START;
		while((1 == SGPIOx->SGPIO_MULMC_CTRL) & BIT_SGPIO_MUL_DMA_START);
	} else {
		/* Disable the SGPIO multiple match DMA mode*/
		SGPIOx->SGPIO_MULMC_CTRL &= (~BIT_SGPIO_MUL_DMA_EN);
	}
}

/**
  * @brief    Init and Enable SGPIO TX GDMA.
  * @param  SGPIOx: selected SGPIO peripheral.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxBuf: Tx Buffer.
  * @param  TxCount: Tx Count.
  * @retval   TRUE/FLASE
  */
BOOL SGPIO_MULGDMA_Init(
	SGPIO_TypeDef *SGPIOx,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u32 *pTxBuf,
	int TxCount
	)
{
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 12);
	if (GdmaChnl == 0xFF) {
		/*  No Available DMA channel */
		return _FALSE;
	}	

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->MuliBlockCunt     = 0;
	GDMA_InitStruct->MaxMuliBlock      = 1;

	GDMA_InitStruct->GDMA_DIR      = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA_HANDSHAKE_INTERFACE_SGPIO_TX;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&(SGPIOx->SGPIO_MULFDATA);
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum       = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (TransferType|ErrType);

	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_BlockSize = 4 * TxCount;

	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);

	GDMA_InitStruct->GDMA_SrcAddr = (u32)(pTxBuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return _TRUE;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
