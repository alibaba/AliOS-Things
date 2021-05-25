 /**
  ******************************************************************************
  * @file    rtl8721dlp_qdec.c
  * @author
  * @version V1.0.0
  * @date    2017-10-17
  * @brief   This file contains all the functions prototypes for the QDec firmware
  *             library, including the following functionalities of the Qdecoder
  *           -Initialization
  *           -QDec Control (disable/enable):
  *           -QDec Reset 
  *           -Get status Interface 
  *           -Interrupts and trigger condition management 
  *           -QDec speed meausure
  *           -QDec speed limit Interrupts
  *
  *  @verbatim
  *      
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          1. configure the QDec pinmux.
  *             
  *          2. Init Qdec.
  *
  *          3. Reset sub-function if needed.
  *
  *          4. Set Interrupt trigger condition as needed.
  *
  *          5. enable sub-function as needed.
  *          
  *          6. Polling status or Get stauts according to the interrupt.
  *
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */


#include "ameba_soc.h"

/**
  * @brief  Deinitializes the QDEC peripheral registers .
  * @param  None.
  * @retval None
  */
void QDEC_DeInit(void)
{
   RCC_PeriphClockCmd(APBPeriph_QDEC0, APBPeriph_QDEC0_CLOCK, DISABLE);
}

/**
  * @brief Initializes the QDEC peripheral according to the specified.
  * @param  QDEC_InitStruct: pointer to a QDEC_InitTypeDef structure that
  *   contains the configuration information for the specified QDEC peripheral.
  * @retval None.
  */
void QDEC_Init(QDEC_TypeDef *QDec, QDEC_InitTypeDef *QDEC_InitStruct)
{
	u32 VTmr_rld;
	u8 VTmr_Div = 0;

    /* Check the parameters */
	assert_param(IS_QD_SYS_CLK(QDEC_InitStruct->QD_SysClk));
	assert_param(QDEC_InitStruct->QD_SmpClk <= QDEC_InitStruct->QD_SysClk);
	assert_param(QDEC_InitStruct->QD_SmpClk >= (QDEC_InitStruct->QD_SysClk >> 5));
	assert_param(QDEC_InitStruct->QD_DebounceTmr <= 0x7ff);
	assert_param(IS_QD_RC_COUNT_MOD(QDEC_InitStruct->QD_Rc_Mod));
	assert_param(IS_QD_PC_PHASE_COUTER(QDEC_InitStruct->QD_Cnt_Sc));
	assert_param(QDEC_InitStruct->QD_MPC <= 0xFFFF);
	assert_param(IS_QD_IDX_MODE(QDEC_InitStruct->QD_IdxMod));
	assert_param(IS_QD_PC_RST_MODE(QDEC_InitStruct->QD_PC_Rst_Mod));
	assert_param(IS_QD_PC_RST_PHASE(QDEC_InitStruct->QD_PC_Rst_Phase));
	assert_param(IS_QD_VC_MODE(QDEC_InitStruct->QD_VCMod));

	/*to avoid overflow*/
	/*VTmr_rld =VTmr /(1000/SysClk) = VTmr*SysClk/1000  = Vtmr *(sysclk/1024)*/
	VTmr_rld  = QDEC_InitStruct->QD_VTmr * (QDEC_InitStruct->QD_SysClk/1000);
	assert_param(VTmr_rld <= 0xFFFFFF);
	while((VTmr_rld / (1+VTmr_Div)) > 0xFFFF){
		VTmr_Div++;
	}
	VTmr_rld /= (VTmr_Div+1);
	
	RCC_PeriphClockCmd(APBPeriph_QDEC0, APBPeriph_QDEC0_CLOCK, DISABLE);
	if(QDEC_InitStruct->QD_SysClk == QD_SYS_CLK_2M){
		RCC_PeriphClockSource_QDEC(0, QDEC_CLK_SEL_2M);
	}
	else{
		RCC_PeriphClockSource_QDEC(0, QDEC_CLK_SEL_32K);
	}
	RCC_PeriphClockCmd(APBPeriph_QDEC0, APBPeriph_QDEC0_CLOCK, ENABLE);

	QDec->QDEC_CLK_CFG = (((QDEC_InitStruct->QD_SysClk /QDEC_InitStruct->QD_SmpClk)-1) << 12) |\
							(QDEC_InitStruct->QD_DebounceTmr);
	
	QDec->QDEC_CTRL = QDEC_InitStruct->QD_Rc_Mod |QDEC_InitStruct->QD_Cnt_Sc | \
						(QDEC_InitStruct->QD_DebounceTmr ? QD_DB_EN:0);
	
	QDec->QDEC_MPC = QDEC_InitStruct ->QD_MPC;
	
	QDec->QDEC_ISC = QDEC_InitStruct->QD_IdxMod | QDEC_InitStruct->QD_PC_Rst_Mod | \
					QDEC_InitStruct->QD_PC_Rst_Phase;

	QDec->QDEC_VCTRL = ((VTmr_Div) << 16) | QDEC_InitStruct->QD_VCMod;
	QDec->QDEC_VTRLD = VTmr_rld;

	
}

/**
  * @brief  Fills each QDEC_InitStruct member with its default value.
  * @param QDEC_InitStruct: pointer to an QDEC_InitTypeDef structure which will be initialized.
  * @retval None.
  */
void QDEC_StructInit(QDEC_InitTypeDef *QDEC_InitStruct)
{
	QDEC_InitStruct->QD_SysClk			= QD_SYS_CLK_32K;
	QDEC_InitStruct->QD_SmpClk			= 10000;
	QDEC_InitStruct->QD_DebounceTmr	= 1;
	QDEC_InitStruct->QD_Rc_Mod				= QD_RC_COUNT_OF;
	QDEC_InitStruct->QD_Cnt_Sc				= QD_PC_1PHSAE_1COUNTER;
	QDEC_InitStruct->QD_MPC				= 0xFFFF;
	QDEC_InitStruct->QD_IdxMod				= QD_IDX_DISABLE;
	
	QDEC_InitStruct->QD_PC_Rst_Mod			= QD_PC_RST_DISABLE;
	QDEC_InitStruct->QD_PC_Rst_Phase		= QD_PC_RST_PHASE_00;
	QDEC_InitStruct->QD_VTmr			= 1000;
	QDEC_InitStruct->QD_VCMod			= QD_VC_MOD_ABS;

}

/**
  * @brief   Enable or disable the QDec.
  * @param  QDec: selected Qdec peripheral.
  * @param  NewState: new state of the QDec.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void QDEC_Cmd(QDEC_TypeDef *QDec, u32 NewState)
{
	u8 trycnt =200;
	u32 Cur_status = 0;

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	
	if (NewState == ENABLE){
		QDec->QDEC_CTRL &= ~QD_MANUAL_INIT;
		QDec->QDEC_CTRL |= QD_AXIS_EN;

		//auto init should check until auto load done
		do{
			Cur_status = QDec->QDEC_PC;
			
			if(Cur_status & QD_AL_STATUS)
				break;

		}while(trycnt--);
	} else if (NewState == DISABLE) {
		QDec->QDEC_CTRL &= ~QD_AXIS_EN;
	}
}



/**
  * @brief  Reset QDec position counter . 
  * @param  QDec: selected Qdec peripheral.
  * @retval None.
  */
void QDEC_RstPC(QDEC_TypeDef *QDec)
{
	u32 reg_ctrl;

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	
	reg_ctrl = QDec->QDEC_CTRL;
	/*disable compare interrupt to avoid error trigger */
	QDec->QDEC_CTRL &= ~(QD_AXIS_EN | QD_PC_CMP_INT_EN);
	
	QDec->QDEC_CTRL |= QD_PC_RST;
	QDec->QDEC_CTRL &= ~QD_PC_RST;

	QDec->QDEC_CTRL = reg_ctrl;
}

/**
  * @brief  Reset QDec rotation counter . 
  * @param  QDec: selected Qdec peripheral.
  * @retval None.
  */
void QDEC_RstRC(QDEC_TypeDef *QDec)
{
	u32 reg_ctrl;

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	
	reg_ctrl = QDec->QDEC_CTRL;

	QDec->QDEC_CTRL &= ~(QD_AXIS_EN | QD_RC_CMP_INT_EN);
	
	QDec->QDEC_CTRL |= QD_RC_RST;
	QDec->QDEC_CTRL &= ~QD_RC_RST;

	QDec->QDEC_CTRL = reg_ctrl;
}

/**
  * @brief  Reset QDec state machine and all other functions . 
  * @param  QDec: selected Qdec peripheral.
  * @retval None.
  */
void QDEC_RstALL(QDEC_TypeDef *QDec)
{
	u32 reg_ctrl;

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	
	reg_ctrl = QDec->QDEC_CTRL;

	QDec->QDEC_CTRL &= ~QD_AXIS_EN;
	
	QDec->QDEC_CTRL |= QD_ALL_RST;
	QDec->QDEC_CTRL &= ~QD_ALL_RST;

	QDec->QDEC_CTRL = reg_ctrl;
}

/**
  * @brief  Configure the number of phase state changed for the position accumulation counter.
  * @param  QDec: selected Qdec peripheral.
  * @param  cnt_sc:This parameter can be one of the following values:
  *            @arg QD_PC_1PHSAE_1COUNTER: position counter change one when Qdecoder change one pahse.
  *            @arg QD_PC_2PHSAE_1COUNTER: position counter change one when Qdecoder change two pahse.
  * @retval None.
  */
void QDEC_SetCntSC(QDEC_TypeDef *QDec, u32 cnt_sc)
{
	u32 reg_ctrl;

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(IS_QD_PC_PHASE_COUTER(cnt_sc));
	
	reg_ctrl = QDec->QDEC_CTRL;
	reg_ctrl &= ~QD_CNT_SC;
	reg_ctrl |= cnt_sc;
	QDec->QDEC_CTRL = reg_ctrl;
}

/**
  * @brief  Set Qdecoder position counter change interrupt trigger valure . 
  * @param  QDec: selected Qdec peripheral.
  * @param  Pchange:This parameter can be one of the following values:
  *            @arg QD_PC_CHG_1COUNTER: position counter change +/-1 will trigger interrupt.
  *            @arg QD_PC_CHG_2COUNTER: position counter change +/-2 will trigger interrupt.
  *            @arg QD_PC_CHG_4COUNTER: position counter change +/-4 will trigger interrupt.
  * @retval None.
  */
void QDEC_SetPChg(QDEC_TypeDef *QDec, u32 Pchange)
{
	u32 reg_ctrl;

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(IS_QD_PC_CHG_COUTER(Pchange));
	
	reg_ctrl = QDec->QDEC_CTRL;
	/*Jerry_dong suggest because hw do not Exaggerate clock domain*/
	QDec->QDEC_CTRL &= ~(QD_AXIS_EN | QD_PC_CHG_INT_EN);
	reg_ctrl &= ~QD_PC_CHG_LEVEL;
	reg_ctrl |= Pchange;
	QDec->QDEC_CTRL = reg_ctrl;

}

/**
  * @brief  Set Qdecoder position counter compare value. 
  * @param  QDec: selected Qdec peripheral.
  * @param  CmpPC: compare position counter value.
  * @retval None.
  */
void QDEC_SetPCC(QDEC_TypeDef *QDec, u32 CmpPC)
{
	u32 reg_mpc;
	
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(CmpPC <= 0xffff);

	reg_mpc = QDec->QDEC_MPC;
	reg_mpc &= ~QD_CMP_PC;
	reg_mpc |= (CmpPC << 16);
	QDec->QDEC_MPC = reg_mpc;
}

/**
  * @brief  Set Qdecoder max position counter. 
  * @param  QDec: selected Qdec peripheral.
  * @param  MaxPC: compare rotation counter value.
  * @retval None.
  */
void QDEC_SetMPC(QDEC_TypeDef *QDec, u32 MaxPC)
{
	u32 reg_mpc;
	
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(MaxPC <= 0xffff);

	reg_mpc = QDec->QDEC_MPC;
	reg_mpc &= ~QD_MAX_PC;
	reg_mpc |= MaxPC;
	QDec->QDEC_MPC = reg_mpc;
}

/**
  * @brief  Set Qdecoder rotation counter compare value. 
  * @param  QDec: selected Qdec peripheral.
  * @param  CmpRC: compare rotation counter value.
  * @retval None.
  */
void QDEC_SetRCC(QDEC_TypeDef *QDec, u32 CmpRC)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(CmpRC <= 0xfff);

	QDec->QDEC_RC = CmpRC;
}

/**
  * @brief Get Qdecoder rotation counter. 
  * @param  QDec: selected Qdec peripheral.  
  * @retval Qdecoder current rotation counter. 
  */
u32 QDEC_GetRC(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return (((QDec->QDEC_PC) & QD_RC) >> 20);
}

/**
  * @brief Get Qdecoder phase status. 
  * @param  QDec: selected Qdec peripheral.  
  * @retval current phase status of PHA&PHB.
  */
u32 QDEC_GetPhase(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return (((QDec->QDEC_PC) & QD_PHASE_STATE) >> 18);
}

/**
  * @brief Get Qdecoder movement direction. 
  * @param  QDec: selected Qdec peripheral.  
  * @retval movement direction: 0 for decrease, 1 for increase. 
  */
u32 QDEC_GetDir(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return (((QDec->QDEC_PC) & QD_DIR_STATE) >> 16);
}

/**
  * @brief Get Qdecoder position counter. 
  * @param  QDec: selected Qdec peripheral.  
  * @retval Qdecoder current position counter. 
  */
u32 QDEC_GetPC(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return ((QDec->QDEC_PC) & QD_PC);

}

/**
  * @brief   Enables or disables the index pulse detection. 
  * @param  QDec: selected Qdec peripheral.
  * @param  NewState: new state of the Qdecoder index detection. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void QDEC_Idx_Cmd(QDEC_TypeDef *QDec, u32 NewState)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	
	if (NewState == ENABLE)
		QDec->QDEC_ISC |= QD_IDX_EN;
	else if (NewState == DISABLE) 
		QDec->QDEC_ISC &= ~QD_IDX_EN;
}

/**
  * @brief   QDec set position counter reset mode. 
  * @param  QDec: selected Qdec peripheral.
  * @param  mode: reset function work mode,This parameter can be:
  *    QD_PC_RST_DISABLE: disable reset function.
  *    QD_PC_RST_ONCE : only reset on the 1st index pluse .
  *    QD_PC_RST_ALWAYS : always reset on the index pluse.
  * @param  phase: reset phase,This parameter can be:
  *    QD_PC_RST_AUTO_IDX: reset on the idx pluse edge.
  *    QD_PC_RST_PHASE_00: reset on phase 00 during index pluse.
  *    QD_PC_RST_PHASE_01: reset on phase 01 during index pluse.
  *    QD_PC_RST_PHASE_10: reset on phase 10 during index pluse.
  *    QD_PC_RST_PHASE_11: reset on phase 11 during index pluse.
  * @retval None.
  */
void QDEC_SetRstMod(QDEC_TypeDef *QDec, u32 mode, u32 phase)
{
	u32 reg_isc;
	
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(IS_QD_PC_RST_MODE(mode));
	assert_param(IS_QD_PC_RST_PHASE(phase));

	reg_isc = QDec->QDEC_ISC;

	reg_isc &= ~0x0000005B;
	reg_isc |= mode;
	reg_isc |= phase;
	
	QDec->QDEC_ISC = reg_isc;	
}

/**
  * @brief Set velocity Timer duration. 
  * @param  QDec: selected Qdec peripheral.
  * @param  duration:  duration of one velocity measure unit.
  *		after the duration VC will be captured to VCCAP.    
  * @retval None.
  */
void QDEC_SetVTmr(QDEC_TypeDef *QDec, u32 duration)
{
	u32 vt_div = 0;
	u32 Temp = 0;
	u32 vt_rld,sysclk;
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL1);
	if (Temp &= BIT_LSYS_QDEC0_CKSL32K) {
		sysclk = QD_SYS_CLK_32K;
	} else {
		sysclk = QD_SYS_CLK_2M;
	}

	/*to avoid overflow*/
	/*VTmr_rld =VTmr /(1000/SysClk) = VTmr*SysClk/1000  = Vtmr *(sysclk/1024)*/
	vt_rld  = duration *(sysclk/1000);
	assert_param(vt_rld <= 0xFFFFFF);
	while((vt_rld / (1+vt_div)) > 0xFFFF){
		vt_div++;
	}
	vt_rld /= (vt_div+1);

	QDec->QDEC_VCTRL &= ~QD_VT_DIV;
	QDec->QDEC_VCTRL |= vt_div << 16;

	QDec->QDEC_VTRLD = vt_rld;
}

/**
  * @brief Set velocity counter mode. 
  * @param  QDec: selected Qdec peripheral.
  * @param  mode:  This parameter can be one of the following values:
  *            @arg QD_VC_MOD_ABS: velocity conter use absolute value of position counter.
  *            @arg QD_VC_MOD_PC: velocity counter use same vaule of position counter.
  * @retval None.
  */
void QDEC_SetVcMod(QDEC_TypeDef *QDec, u32 mode)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(IS_QD_VC_MODE(mode));

	QDec->QDEC_VCTRL &= ~QD_VMUC_MOD;
	QDec->QDEC_VCTRL |= mode;
}

/**
  * @brief   Enables or disables the specified Qdecoder velocity function. 
  * @param  QDec: selected Qdec peripheral.
  * @param  NewState: new state of the Qdecoder velocity function. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void QDEC_VT_Cmd(QDEC_TypeDef *QDec, u32 NewState)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	
	if (NewState == ENABLE)
		QDec->QDEC_VCTRL |= QD_VMUC_EN;
	else if (NewState == DISABLE) 
		QDec->QDEC_VCTRL &= ~QD_VMUC_EN;
}

/**
  * @brief  Reset velocity timer and. 
  * @param  QDec: selected Qdec peripheral.
  * @retval None.
  */
void QDEC_VtRst(QDEC_TypeDef *QDec)
{
	u32 reg_vctrl;
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	reg_vctrl = QDec->QDEC_VCTRL;
	QDec->QDEC_VCTRL &= ~QD_VMUC_EN;
	/*hw clock issue ,may can not sample VMUC Disable*/
	DelayUs(500);

	QDec->QDEC_VCTRL |= QD_VMUC_RST;
	QDec->QDEC_VCTRL &= ~QD_VMUC_RST;

	QDec->QDEC_VCTRL = reg_vctrl;

}

/**
  * @brief  Set Qdec velocity counter up limit value. 
  * @param  QDec: selected Qdec peripheral.
  * @param  limt: the limited value to be setting.
  * @retval None.
  */
void QDEC_SetVcUpLmt(QDEC_TypeDef *QDec, u32 limt)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(limt <= 0xffff);

	QDec->QDEC_VCOMP &= ~QD_VUP_LMT;
	QDec->QDEC_VCOMP |= limt << 16;
}

/**
  * @brief  Set Qdec velocity counter low limit value. 
  * @param  QDec: selected Qdec peripheral.
  * @param  limt: the limited value to be setting.
  * @retval None.
  */
void QDEC_SetVcLowLmt(QDEC_TypeDef *QDec, u32 limt)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param(limt <= 0xffff);

	QDec->QDEC_VCOMP &= ~QD_VLOW_LMT;
	QDec->QDEC_VCOMP |= limt;
}

/**
  * @brief  Get Qdec velocity Timer value. 
  * @param  QDec: selected Qdec peripheral.
  * @retval velocity timerr value.
  */
u32 QDEC_GetVT(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return QDec->QDEC_VT;
}

/**
  * @brief  Get Qdec velocity counter value. 
  * @param  QDec: selected Qdec peripheral.
  * @retval velocity counter value.
  */
u32 QDEC_GetVC(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return QDec->QDEC_VC;
}

/**
  * @brief  Get Qdec velocity counter capture value. 
  * @param  QDec: selected Qdec peripheral.
  * @retval velocity counter capture value.
  */
u32 QDEC_GetVCCAP(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return QDec->QDEC_VCCAP;
}

/**
  * @brief  Get Qdec position counter capture value. 
  * @param  QDec: selected Qdec peripheral.
  * @retval positon counter capture value.
  */
u32 QDEC_GetPCCAP(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return QDec->QDEC_PCCAP;
}


/**
  * @brief  Enables or disables the specified QDec interrupts.
  * @param  QDec: selected Qdec peripheral.
  * @param  INT: specifies the QDec interrupts sources to be enabled or disabled.
  *   This parameter can be the following values:
  *     @arg QD_RC_CMP_INT_EN: Qdecoder rotation counter compare INT enable.
  *     @arg QD_VUPLMT_INT_EN: Qdecoder Velocity up limit interrupt enable.
  *     @arg QD_VLOWLMT_INT_EN: Qdecoder Velocity low limit interrupt enable.
  *     @arg QD_VCCAP_INT_EN: Qdecoder Velocity counter capture interrupt enable.
  *     @arg QD_PC_ERR_INT_EN: Qdecoder position counter error INT enable.
  *     @arg QD_IDX_PULSE_INT_EN: Qdecoder index pulse INT enable.
  *     @arg QD_RC_UF_INT_EN: Qdecoder rotation counter underflow INT enable.
  *     @arg QD_RC_OF_INT_EN: Qdecoder rotation counter overflow INT enable.
  *     @arg QD_PC_CMP_INT_EN: Qdecoder position counter compare INT enable.
  *     @arg QD_DR_CH_INT_EN: Qdecoder direction changed INT enable.
  *     @arg QD_PHASE_ILL_INT_EN: Qdecoder phase changed illegal INT enable.
  *     @arg QD_PC_UF_INT_EN: Qdecoder position counter underflow INT enable.
  *     @arg QD_PC_OF_INT_EN: Qdecoder position counter overflow INT enable.
  *     @arg QD_PC_CHG_INT_EN: Qdecoder position counter changed INT enable.
  * @param  NewState: new state of the specified QDecinterrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void QDEC_INTConfig(QDEC_TypeDef *QDec, u32 INT, u32 newState)
{
	u32 velocity_int,counter_int;
	
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	counter_int = INT & QD_COUNTER_INT;
	velocity_int = INT & QD_VELOCITY_INT;
	
	if (newState == ENABLE) {
		QDec->QDEC_CTRL |= counter_int;
		QDec->QDEC_VCTRL |= velocity_int;
	}else if (newState == DISABLE) {
		QDec->QDEC_CTRL &= ~counter_int;
		QDec->QDEC_VCTRL &= ~velocity_int;
	}
}

/**
  * @brief  Mask or unmask the specified QDec interrupts.
  * @param  QDec: selected Qdec peripheral.
  * @param  mask: specifies the Qdec interrupts sources to be mask or unmask.
  *   This parameter can be one or combinations of the following values:
  *     @arg QD_RC_COMP_INT_MASK: Qdecoder rotation counter compare INT mask.
  *     @arg QD_VUPLMT_INT_MASK: Qdecoder Velocity up limit interrupt mask.
  *     @arg QD_VLOWLMT_INT_MASK: Qdecoder Velocity low limit interrupt mask.
  *     @arg QD_VCCAP_INT_MASK: Qdecoder Velocity counter capture interrupt mask.
  *     @arg QD_PC_ERR_INT_MASK: Qdecoder position counter error INT mask.
  *     @arg QD_IDX_PULSE_INT_MASK: Qdecoder index pulse INT mask.
  *     @arg QD_RC_UF_INT_MASK: Qdecoder rotation counter underflow INT mask.
  *     @arg QD_RC_OF_INT_MASK: Qdecoder rotation counter overflow INT mask.
  *     @arg QD_PC_CMP_INT_MASK: Qdecoder position counter compare INT mask.
  *     @arg QD_DR_CH_INT_MASK: Qdecoder direction changed INT mask.
  *     @arg QD_PHASE_ILL_INT_MASK: Qdecoder phase changed illegal INT mask.
  *     @arg QD_PC_UF_INT_MASK: Qdecoder position counter underflow INT mask.
  *     @arg QD_PC_OF_INT_MASK: Qdecoder position counter overflow INT mask.
  *     @arg QD_PC_CHG_INT_MASK: Qdecoder position counter changed INT mask.
  * @param  NewState: new state of the specified QDec interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void QDEC_INTMask(QDEC_TypeDef *QDec, u32 mask, u32 newState)
{

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param((mask & (~QD_ALL_INT_MASK)) == 0);

	if (newState == ENABLE)
		QDec->QDEC_IMR |= mask;
	else if (newState == DISABLE) 
		QDec->QDEC_IMR &= ~mask;
}

/**
  * @brief  Clear the specified QDec interrupts.
  * @param  QDec: selected Qdec peripheral.
  * @param  INT: specifies the Qdec interrupts sources to be clear.
  *   This parameter can be one or combinations of the following values:
  *     @arg QD_RC_COMP_INT_STATUS: Qdecoder rotation counter compare INT status.
  *     @arg QD_VUPLMT_INT_STATUS: Qdecoder Velocity up limit interrupt status.
  *     @arg QD_VLOWLMT_INT_STATUS: Qdecoder Velocity low limit interrupt status.
  *     @arg QD_VCCAP_INT_STATUS: Qdecoder Velocity counter capture interrupt status.
  *     @arg QD_PC_ERR_INT_STATUS: Qdecoder position counter error INT status.
  *     @arg QD_IDX_PULSE_INT_STATUS: Qdecoder index pulse INT status.
  *     @arg QD_RC_UF_INT_STATUS: Qdecoder rotation counter underflow INT status.
  *     @arg QD_RC_OF_INT_STATUS: Qdecoder rotation counter overflow INT status.
  *     @arg QD_PC_CMP_INT_STATUS: Qdecoder position counter compare INT status.
  *     @arg QD_DR_CH_INT_STATUS: Qdecoder direction changed INT status.
  *     @arg QD_PHASE_ILL_INT_STATUS: Qdecoder phase changed illegal INT status.
  *     @arg QD_PC_UF_INT_STATUS: Qdecoder position counter underflow INT status.
  *     @arg QD_PC_OF_INT_STATUS: Qdecoder position counter overflow INT status.
  *     @arg QD_PC_CHG_INT_STATUS: Qdecoder position counter changed INT status.
  * @retval None
  */
void QDEC_ClearINT(QDEC_TypeDef *QDec, u32 INT)
{

	assert_param(IS_QDEC_ALL_PERIPH(QDec));
	assert_param((INT & (~QD_ALL_INT_STATUS)) == 0);

	QDec->QDEC_ISR |= INT;
}

/**
  * @brief Get the specified QDec interrupt status.
  * @param  QDec: selected Qdec peripheral.
  * @retval The new state of IR_INT (SET or RESET).
  */
u32 QDEC_GetRawINT(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return QDec->QDEC_ISR;
}

/**
  * @brief Get the specified QDec interrupt mask status.
  * @param  QDec: selected Qdec peripheral.
  * @retval The mask state of IR_INT (SET or RESET).
  */
u32 QDEC_GetIMR(QDEC_TypeDef *QDec)
{
	assert_param(IS_QDEC_ALL_PERIPH(QDec));

	return QDec->QDEC_IMR;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/
