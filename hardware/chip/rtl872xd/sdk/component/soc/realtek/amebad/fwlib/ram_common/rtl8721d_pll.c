/**
  ******************************************************************************
  * @file    rtl8721d_pll.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the NCO clock:
  *           - NCO32k
  *           - NCO8M
  *           - EXT32k Disable/Enable 
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

/**
  * @brief  PLL divider set
  * @param  div: Division
  */
void PLL_Div(u32 div)
{
	u32 Tmp;

	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PERI_CLK_CTRL3);
	Tmp &= ~(BIT_MASK_HSYS_I2S_CLKDIV << BIT_SHIFT_HSYS_I2S_CLKDIV);
	Tmp |= ((u32)div) << BIT_SHIFT_HSYS_I2S_CLKDIV;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PERI_CLK_CTRL3, Tmp);
}

/**
  * @brief  I2S or PCM PLL select
  * @param  sel: PLL select
  */
void PLL_Sel(u32 sel)
{
	u32 Tmp;
	
	if ((sel == PLL_PCM) ||(sel == PLL_I2S)){
		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PERI_CLK_CTRL3);
		Tmp &= ~(BIT_MASK_HSYS_AC_SPORT_CKSL << BIT_SHIFT_HSYS_AC_SPORT_CKSL);
		Tmp |= ((u32)sel) << BIT_SHIFT_HSYS_AC_SPORT_CKSL;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PERI_CLK_CTRL3, Tmp);
	}
	else{

	}
}


/**
  * @brief  I2S PLL control register set.
  * @param  new_state: DISABLE/ENABLE
  */
void PLL_I2S_Set(u32 new_state)
{
	u32 Tmp;


	if (ENABLE == new_state){
		//enable 98.304M PLL
		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_FUNC_EN0);
		Tmp |= BIT_SYS_AMACRO_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_FUNC_EN0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0);
		Tmp |= (BIT_LP_PLL_BG_I_EN |BIT_LP_PLL_BG_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0, Tmp);

		DelayUs(2);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0);
		Tmp |= BIT_LP_PLL_MBIAS_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0);
		Tmp |= BIT_PLL_I2S_POWCUT_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0, Tmp);

		DelayUs(400);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_XTAL_CTRL1);
		Tmp &= ~BIT_XTAL_AAC_GM_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_XTAL_CTRL1, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0);
		Tmp |= (BIT_PLL_I2S_DIV_EN | BIT_PLL_I2S_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PWR_ISO_CTRL);
		Tmp &= ~BIT_HSYS_ISO_AUXPLL_I2S;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PWR_ISO_CTRL, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1);
		Tmp |= BIT_PLL_I2S_POW_SDM_FCODE;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1, Tmp);

		while ((HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_TEST) & BIT_PLL_I2S_RDY) != BIT_PLL_I2S_RDY) ;

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0);
		Tmp |= BIT_PLL_I2S_CLK_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0);
		Tmp |= BIT_PLL_I2S_DIV2_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0, Tmp);
	}
	else{
		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0);
		Tmp &= ~(BIT_PLL_I2S_CLK_EN|BIT_PLL_I2S_DIV2_EN|BIT_PLL_I2S_EN|BIT_PLL_I2S_DIV_EN|BIT_PLL_I2S_DIV_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0, Tmp);
	}
}

/**
  * @brief  PCM PLL control register set.
  * @param  new_state: DISABLE/ENABLE
  */
void PLL_PCM_Set(u32 new_state)
{
	u32 Tmp;


	if (ENABLE == new_state){
		//enable 45.1584M PLL
		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_FUNC_EN0);
		Tmp |= BIT_SYS_AMACRO_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_FUNC_EN0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0);
		Tmp |= (BIT_LP_PLL_BG_I_EN |BIT_LP_PLL_BG_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0, Tmp);

		DelayUs(2);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0);
		Tmp |= BIT_LP_PLL_MBIAS_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0);
		Tmp |= BIT_PLL_PCM_POWCUT_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0, Tmp);

		DelayUs(400);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_XTAL_CTRL1);
		Tmp &= ~BIT_XTAL_AAC_GM_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_XTAL_CTRL1, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0);
		Tmp |= (BIT_PLL_PCM_DIV_EN | BIT_PLL_PCM_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PWR_ISO_CTRL);
		Tmp &= ~BIT_HSYS_ISO_AUXPLL_PCM;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PWR_ISO_CTRL, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1);
		Tmp |= BIT_PLL_PCM_POW_SDM_FCODE;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1, Tmp);

		while ((HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_TEST) & BIT_PLL_PCM_RDY) != BIT_PLL_PCM_RDY) ;

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0);
		Tmp |= BIT_PLL_PCM_CLK_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0, Tmp);

		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0);
		Tmp |= BIT_PLL_PCM_DIV2_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0, Tmp);
	}
	else{
		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0);
		Tmp &= ~(BIT_PLL_PCM_CLK_EN|BIT_PLL_PCM_DIV2_EN|BIT_PLL_PCM_EN|BIT_PLL_PCM_DIV_EN|BIT_PLL_PCM_DIV_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0, Tmp);
	}
}

/**
  * @brief  I2S PLL output adjust by ppm.
  * @param  ppm: ~0.75ppm per FOF step
  * @param  action: slower or faster or reset
  *          This parameter can be one of the following values:
  *            @arg PLL_AUTO: reset to auto 98.304M
  *            @arg PLL_FASTER: pll clock faster than 98.304M
  *            @arg PLL_SLOWER: pll clock slower than 98.304M
  */
void PLL_I2S_ClkTune(u32 ppm, u32 action)
{
	u32 Tmp;
	u32 FOF_new;

	assert_param(ppm<=100);

	if (action == PLL_AUTO){
		//switch to auto mode
		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0);
		Tmp |= BIT2;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0, Tmp);
		return;
	}
	
	//switch to manual mode
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0);
	Tmp &= ~BIT2;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL0, Tmp);	
		
	//set div.N
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1);
	Tmp &= ~(BIT_PLL_MASK_I2S_DIVN_SDM<<BIT_PLL_SHIFT_I2S_DIVN_SDM);
	Tmp |= (u32)0x11<<BIT_PLL_SHIFT_I2S_DIVN_SDM;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1, Tmp);		
		
	//adjust fo.N, fo.f
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL3);
	if (action == PLL_FASTER){
		FOF_new = 0x92a+ppm*4/3;
	}
	else if (action == PLL_SLOWER){
		FOF_new = 0x92a-ppm*4/3;
	}
	else{
		FOF_new = 0x92a;
	}
	Tmp &= ~((BIT_PLL_MASK_I2S_SDM_FON<<BIT_PLL_SHIFT_I2S_SDM_FON)|(BIT_PLL_MASK_I2S_SDM_FOF<<BIT_PLL_SHIFT_I2S_SDM_FOF));
	Tmp |= (u32)FOF_new<<BIT_PLL_SHIFT_I2S_SDM_FOF;
	Tmp |= (u32)0x5<<BIT_PLL_SHIFT_I2S_SDM_FON;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL3, Tmp);	

	//reset BIT_PLL_I2S_TRIG_RREQ_EN
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1);
	Tmp |= BIT_PLL_I2S_TRIG_RREQ_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1, Tmp);		

	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1);
	Tmp &= ~BIT_PLL_I2S_TRIG_RREQ_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_I2S_CTRL1, Tmp);	
}


/**
  * @brief  PCM PLL output adjust by ppm.
  * @param  ppm: ~5/6*ppm per FOF step
  * @param  action: slower or faster or reset
  *          This parameter can be one of the following values:
  *            @arg PLL_AUTO: reset to auto 45.1584M
  *            @arg PLL_FASTER: pll clock faster than 45.1584M
  *            @arg PLL_SLOWER: pll clock slower than 45.1584M
  */
void PLL_PCM_ClkTune(u32 ppm, u32 action)
{
	u32 Tmp;
	u32 FOF_new;

	assert_param(ppm<=100);

	if (action == PLL_AUTO){
		//switch to auto mode
		Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0);
		Tmp |= BIT2;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0, Tmp);
		return;
	}
	
	//switch to manual mode
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0);
	Tmp &= ~BIT2;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL0, Tmp);	
		
	//set div.N
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1);
	Tmp &= ~(BIT_PLL_MASK_PCM_DIVN_SDM<<BIT_PLL_SHIFT_PCM_DIVN_SDM);
	Tmp |= (u32)0x10<<BIT_PLL_SHIFT_PCM_DIVN_SDM;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1, Tmp);		
		
	//adjust fo.N, fo.f
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL3);
	if (action == PLL_FASTER){
		FOF_new = 0x1038+ppm*6/5;
	}
	else if (action == PLL_SLOWER){
		FOF_new = 0x1038-ppm*6/5;
	}
	else{
		FOF_new = 0x1038;
	}
	Tmp &= ~((BIT_PLL_MASK_PCM_SDM_FON<<BIT_PLL_SHIFT_PCM_SDM_FON)|(BIT_PLL_MASK_PCM_SDM_FOF<<BIT_PLL_SHIFT_PCM_SDM_FOF));
	Tmp |= (u32)FOF_new<<BIT_PLL_SHIFT_PCM_SDM_FOF;
	Tmp |= (u32)0x0<<BIT_PLL_SHIFT_PCM_SDM_FON;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL3, Tmp);	

	//reset BIT_PLL_PCM_TRIG_RREQ_EN
	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1);
	Tmp |= BIT_PLL_PCM_TRIG_RREQ_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1, Tmp);		

	Tmp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1);
	Tmp &= ~BIT_PLL_PCM_TRIG_RREQ_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_PLL_PCM_CTRL1, Tmp);	
}

/**
  * @brief  PLL control register2 set.
  * @param  BitMask: set bit
  * @param  NewState: DISABLE/ENABLE
  */
void PLL2_Set(u32 BitMask, u32 NewState)
{
	/* To avoid gcc warnings */
	( void ) BitMask;
	( void ) NewState;
#ifdef AMEBAD_TODO
	DBG_8195A("%s Please check this function \n", __func__);
#endif
}

/**
  * @brief  PLL control register3 set.
  * @param  BitMask: set bit
  * @param  NewState: DISABLE/ENABLE
  */
void PLL3_Set(u32 BitMask, u32 NewState)
{
	/* To avoid gcc warnings */
	( void ) BitMask;
	( void ) NewState;

#ifdef AMEBAD_TODO
	DBG_8195A("%s Please check this function \n", __func__);
#endif
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
