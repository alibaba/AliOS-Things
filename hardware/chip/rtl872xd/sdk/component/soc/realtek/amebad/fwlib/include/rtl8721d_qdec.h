/**
  ******************************************************************************
  * @file    rtl8721d_qdec.h
  * @author
  * @version V1.0.0
  * @date    2017-10-16
  * @brief   This file contains all the functions prototypes for the QDecoder.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */


#ifndef _RTL8721D_QDEC_H_
#define _RTL8721D_QDEC_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @addtogroup QDecoder QDecoder
  * @{
  */

/** @addtogroup QDecoder
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * QDEC:
  *		- Base Address: QDEC_DEV
  *		- Sclk: 32K/2Mhz
  *		- HW input : PHA, PHB, IDX 
  *		- Sample Clock:  	Configurable, up to 2Mhz
  *		- Debounce Timer: Configurable
  *		- IRQ: QDECODER_IRQ_LP
  *
  *****************************************************************************************     
  * How to use Normal QDecoder
  *****************************************************************************************
  *      To use the normal QDecoder mode, the following steps are mandatory:
  *
  *          1. configure the QDec pinmux
  *             
  *          2. Init Qdec
  *               QDEC_StructInit(QDEC_InitTypeDef *QDEC_InitStruct)
  *               QDEC_Init(QDEC_TypeDef *QDec, QDEC_InitTypeDef *QDEC_InitStruct)
  *
  *          3. Reset sub-function if need,eg:
  *               QDEC_RstPC(QDEC_TypeDef *QDec)
  *               QDEC_RstRC(QDEC_TypeDef *QDec)
  *               QDEC_RsALL(QDEC_TypeDef *QDec)
  *
  *          4. Set Interrupt trigger condition as needed
  *
  *          5. enable velocity function as needed
  *          
  *          6. Polling status or Get stauts according to the interrupt.
  *
  *****************************************************************************************    
  * How to enable  velocity function 
  *****************************************************************************************    
  *      To enable the velocity function, the following steps are mandatory:
  *       
  *      1. Init VcMod and VTmr
  *          QDEC_SetVcMod(QDEC_TypeDef *QDec, u32 mode)
  *          QDEC_SetVTmr(QDEC_TypeDef *QDec, u32 duration)
  *
  *      2. Reset velocity status if need.
  *          QDEC_VtRst(QDEC_TypeDef *QDec)
  *        
  *      3. Configure interrupt if needed
  *
  *      4. enable velocity function
  *        QDEC_VT_Cmd(QDEC_TypeDef *QDec, u32 NewState)
  *
  *****************************************************************************************     
  * @endverbatim
  */
  
/* Exported types --------------------------------------------------------*/
/** @defgroup QDecoder_Exported_Types QDecoder Exported Types
  * @{
  */

/** 
  * @brief  QDecoder Init structure definition
  */
typedef struct {
	u32 QD_SysClk;				/*!< Specifies QDec  Bus clock .
							This parameter can be a value of @ref QDecoder_Bus_Clock_definitions */
							
	u32 QD_SmpClk;				/*!< Specifies Qdec sample clock. 
							This parameter must be set to a value in the (1/32 ~ 1) * QD_SysClk range. */

	u32 QD_DebounceTmr;		/*!< Specifies glitch width under QD_DebounceTmr * Tsysclk will be filtered.
							This parameter must be set to a value in the 0x1-0x7ff range. */

	u32 QD_Rc_Mod;				/*!< Specifies rotation counter calc mode.
							This parameter can be a value of @ref QDecoder_RC_mode_definition */

	u32 QD_Cnt_Sc;				/*!< Specifies position counter calc mode.
							This parameter can be a value of @ref QDecoder_Cnt_Sc_definition */

	u32 QD_MPC;					/*!< Specifies max position counter value
							This parameter must be set to a value in the 0x0-0xffff range.  */
					
	u32 QD_IdxMod;					/*!< Specifies Qdecoder index signal work mode.
							This parameter can be a value of @ref QDecoder_IDX_Mode_definition */
								
	u32 QD_PC_Rst_Mod;			/*!< Specifies Qdecoder position counter reset mode.
							This parameter can be a value of @ref QDecoder_PC_Reset_Mode_definition*/
								
	u32 QD_PC_Rst_Phase;			/*!< Specifies Qdecoder position counter reset phase.
							This parameter can be a value of @ref QDecoder_PC_Reset_PHASE_definition*/
								
	u32 QD_VTmr;				/*!< Specifies velocity measure timer in ms, .
							This parameter can be a value of in the 0x0-0xffffff*Tsysclk range */
								
	u32 QD_VCMod;				/*!< Specifies velocity counter mode .
							This parameter can be a value of @ref QDecoder_VC_Work_Mode_definitions */
}QDEC_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup QDecoder_Exported_Constants Qdecoder Exported Constants
  * @{
  */

/** @defgroup QDecoder_Peripheral_definitions 
  * @{
  */
#define IS_QDEC_ALL_PERIPH(PERIPH) ((PERIPH) == QDEC_DEV)
/**
  * @}
  */

/** @defgroup QDecoder_Bus_Clock_definitions 
  * @{
  */
#define QD_SYS_CLK_32K			((u32)32768)
#define QD_SYS_CLK_2M			((u32)2000000)
#define IS_QD_SYS_CLK(CLK)		(((CLK) == QD_SYS_CLK_32K) || \
                                   		((CLK) == QD_SYS_CLK_2M))
/**
  * @}
  */

/** @defgroup QDecoder_Clock_selection 
  * @{
  */
#define QDEC_CLK_SEL_32K			((u32)0x00000001)
#define QDEC_CLK_SEL_2M			((u32)0x00000000)
/**
  * @}
  */

/** @defgroup QDecoder_RC_mode_definition 
  * @{
  */
#define QD_RC_COUNT_IDX			((u32)0x00000000 << 28)
#define QD_RC_COUNT_OF				((u32)0x00000001 << 28)
#define IS_QD_RC_COUNT_MOD(MODE)			(((MODE) == QD_RC_COUNT_IDX) || \
                                  			 ((MODE) == QD_RC_COUNT_OF))
/**
  * @}
  */

/** @defgroup QDecoder_Cnt_Sc_definition 
  * @{
  */
#define QD_PC_1PHSAE_1COUNTER			((u32)0x00000000 << 13)
#define QD_PC_2PHSAE_1COUNTER			((u32)0x00000001 << 13)
#define IS_QD_PC_PHASE_COUTER(CNT)			(((CNT) == QD_PC_2PHSAE_1COUNTER) || \
                                  			 ((CNT) == QD_PC_1PHSAE_1COUNTER))
/**
  * @}
  */

/** @defgroup QDecoder_PC_Chang_Level_definition 
  * @{
  */
#define QD_PC_CHG_1COUNTER			((u32)0x00000000 << 3)
#define QD_PC_CHG_2COUNTER			((u32)0x00000001 << 3)
#define QD_PC_CHG_4COUNTER			((u32)0x00000002 << 3)
#define IS_QD_PC_CHG_COUTER(CNT)			(((CNT) == QD_PC_CHG_1COUNTER) || \
                                  			 ((CNT) == QD_PC_CHG_2COUNTER) ||((CNT) == QD_PC_CHG_4COUNTER))
/**
  * @}
  */

  /** @defgroup QDecoder_IDX_Mode_definition 
  * @{
  */
#define QD_IDX_DISABLE			((u32)0x00000000 << 31)
#define QD_IDX_NORMAL			(((u32)0x00000001 << 31) | ((u32)0x00000000 << 5))
#define QD_IDX_INVERSE			(((u32)0x00000001 << 31) | ((u32)0x00000001 << 5))

#define IS_QD_IDX_MODE(MODE)			(((MODE) == QD_IDX_DISABLE) || \
							((MODE) == QD_IDX_NORMAL) || ((MODE) == QD_IDX_INVERSE))
/**
  * @}
  */
  
  /** @defgroup QDecoder_PC_Reset_PHASE_definition 
  * @{
  */
#define QD_PC_RST_AUTO_IDX			((u32)0x00000001 << 6)
#define QD_PC_RST_PHASE_00			((u32)0x00000000 )
#define QD_PC_RST_PHASE_01			((u32)0x00000001)
#define QD_PC_RST_PHASE_10			((u32)0x00000002)
#define QD_PC_RST_PHASE_11			((u32)0x00000003)
#define IS_QD_PC_RST_PHASE(PHASE)			(((PHASE) == QD_PC_RST_AUTO_IDX) || \
							((PHASE) <= 0x00000003))
/**
  * @}
  */

  /** @defgroup QDecoder_PC_Reset_Mode_definition 
  * @{
  */
#define QD_PC_RST_DISABLE			((u32)0x00000000 << 3)
#define QD_PC_RST_ONCE				((u32)0x00000001 << 3)
#define QD_PC_RST_ALWAYS			((u32)0x00000002 << 3)

#define IS_QD_PC_RST_MODE(MODE)			(((MODE) == QD_PC_RST_DISABLE) || \
							((MODE) == QD_PC_RST_ONCE) || ((MODE) == QD_PC_RST_ALWAYS))
/**
  * @}
  */
  
/** @defgroup QDecoder_VC_Work_Mode_definitions 
  * @{
  */
#define QD_VC_MOD_ABS			((u32)0x00000000 << 9)
#define QD_VC_MOD_PC			((u32)0x00000001 << 9)
#define IS_QD_VC_MODE(MODE)			(((MODE) == QD_VC_MOD_ABS) || \
                                   ((MODE) == QD_VC_MOD_PC))
/**
  * @}
  */
  
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup QDecoder_Exported_Functions QDecoder Exported Functions
  * @{
  */

/** @defgroup QDecoder_Exported_Normal_Functions QDec Exported Normal Functions
  * @{
  */
void QDEC_DeInit(void);
void QDEC_Init(QDEC_TypeDef *QDec, QDEC_InitTypeDef *QDEC_InitStruct);
void QDEC_StructInit(QDEC_InitTypeDef *QDEC_InitStruct);
void QDEC_Cmd(QDEC_TypeDef *QDec, u32 NewState);
void QDEC_RstPC(QDEC_TypeDef *QDec);
void QDEC_RstRC(QDEC_TypeDef *QDec);
void QDEC_RstALL(QDEC_TypeDef *QDec);
void QDEC_SetCntSC(QDEC_TypeDef *QDec, u32 cnt_sc);
void QDEC_SetPChg(QDEC_TypeDef *QDec, u32 Pchange);
void QDEC_SetPCC(QDEC_TypeDef *QDec, u32 CmpPC);
void QDEC_SetMPC(QDEC_TypeDef *QDec, u32 MaxPC);
void QDEC_SetRCC(QDEC_TypeDef *QDec, u32 CmpRC);
u32 QDEC_GetRC(QDEC_TypeDef *QDec);
u32 QDEC_GetPhase(QDEC_TypeDef *QDec);
u32 QDEC_GetDir(QDEC_TypeDef *QDec);
u32 QDEC_GetPC(QDEC_TypeDef *QDec);
void QDEC_Idx_Cmd(QDEC_TypeDef *QDec, u32 NewState);
void QDEC_SetRstMod(QDEC_TypeDef *QDec, u32 mode, u32 phase);
void QDEC_SetVTmr(QDEC_TypeDef *QDec, u32 duration);
void QDEC_SetVcMod(QDEC_TypeDef *QDec, u32 mode);
void QDEC_VT_Cmd(QDEC_TypeDef *QDec, u32 NewState);
void QDEC_VtRst(QDEC_TypeDef *QDec);
void QDEC_SetVcUpLmt(QDEC_TypeDef *QDec, u32 limt);
void QDEC_SetVcLowLmt(QDEC_TypeDef *QDec, u32 limt);
u32 QDEC_GetVT(QDEC_TypeDef *QDec);
u32 QDEC_GetVC(QDEC_TypeDef *QDec);
u32 QDEC_GetVCCAP(QDEC_TypeDef *QDec);
u32 QDEC_GetPCCAP(QDEC_TypeDef *QDec);
void QDEC_INTConfig(QDEC_TypeDef *QDec, u32 INT, u32 newState);
void QDEC_INTMask(QDEC_TypeDef *QDec, u32 mask, u32 newState);
void QDEC_ClearINT(QDEC_TypeDef *QDec, u32 INT);
u32 QDEC_GetRawINT(QDEC_TypeDef *QDec);
u32 QDEC_GetIMR(QDEC_TypeDef *QDec);
/**
  * @}
  */



/**
  * @}
  */
  

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup QDecoder_Register_Definitions QDecoder Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup QDEC_CLK_CFG
 * @{
 *****************************************************************************/
#define QD_SMP_DIV						((u32)0x0001F000)		/*Bit[16:12], Divider for input signal sampling clock*/
#define QD_DBN_TM						((u32)0x000007FF)		/*Bit[10:0], Divider for De-bounce timer clock*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_CTRL
 * @{
 *****************************************************************************/
#define QD_AXIS_EN					((u32)0x00000001 << 31)	/*Bit[31], Qdecoder enable control */
#define QD_PC_RST					((u32)0x00000001 << 30)	/*Bit[30], Qdecoder position counter reset control*/
#define QD_RC_RST					((u32)0x00000001 << 29)	/*Bit[29], Qdecoder rotation counter reset control*/
#define QD_RC_MOD					((u32)0x00000001 << 28)	/*Bit[38], Qdecoder rotation mode*/
#define QD_ALL_RST					((u32)0x00000001 << 27)	/*Bit[37], Qdecoder ALL reset control*/

#define QD_RC_CMP_INT_EN			((u32)0x00000001 << 24)	/*Bit[24], Qdecoder rotation counter compare INT enable*/
#define QD_PC_ERR_INT_EN			((u32)0x00000001 << 23)	/*Bit[23], Qdecoder position counter error INT enable*/
#define QD_IDX_PULSE_INT_EN		((u32)0x00000001 << 22)	/*Bit[22], Qdecoder index pulse INT enable*/
#define QD_RC_UF_INT_EN			((u32)0x00000001 << 21)	/*Bit[21], Qdecoder rotation counter underflow INT enable*/
#define QD_RC_OF_INT_EN			((u32)0x00000001 << 20)	/*Bit[20], Qdecoder rotation counter overflow INT enable*/
#define QD_PC_CMP_INT_EN			((u32)0x00000001 << 19)	/*Bit[19], Qdecoder position counter compare INT enable*/
#define QD_DR_CH_INT_EN			((u32)0x00000001 << 18)	/*Bit[18], Qdecoder direction changed INT enable*/
#define QD_PC_CHG_INT_EN			((u32)0x00000001 << 17)	/*Bit[17], Qdecoder position counter changged INT enable*/
#define QD_PC_OF_INT_EN			((u32)0x00000001 << 16)	/*Bit[16], Qdecoder position counter overflow INT enable*/
#define QD_PC_UF_INT_EN			((u32)0x00000001 << 15)	/*Bit[15], Qdecoder position counter underflow INT enable*/
#define QD_PHASE_ILL_INT_EN		((u32)0x00000001 << 14)	/*Bit[14], Qdecoder phase changed illegal INT enable*/
#define QD_COUNTER_INT				((u32)0x01FFC000)			/*Qdecoder counter related INT*/

#define QD_CNT_SC					((u32)0x00000001 << 13)	/*Bit[13], Qdecoder number of phase state changed for the position accumulation counter*/
#define QD_DB_EN					((u32)0x00000001 << 12)	/*Bit[12], Qdecoder de-bouncing enable*/
#define QD_PC_CHG_LEVEL			((u32)0x00000003 << 3)	/*Bit[4:3], Qdecoder position changed interrupt trigger level*/
#define QD_MANUAL_INIT				((u32)0x00000001 << 2)	/*Bit[2], Qdecoder manually initial disable*/
#define QD_INIT_PHASE				((u32)0x00000003)			/*Bit[1:0], Qdecoder init pahse*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_MPC
 * @{
 *****************************************************************************/
#define QD_CMP_PC					((u32)0x0000FFFF << 16)	/*Bit[31:16], Qdecoder position compare register*/
#define QD_MAX_PC					((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder Max position counter register*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_RC
 * @{
 *****************************************************************************/
#define QD_CMP_RC					((u32)0x00000FFF)			/*Bit[11:0], Qdecoder rotation compare register*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_PC
 * @{
 *****************************************************************************/
#define QD_RC						((u32)0x00000FFF << 20)	/*Bit[31:20], Qdecoder current rotation counter*/
#define QD_PHASE_STATE				((u32)0x00000003 << 18)	/*Bit[19:18], Qdecoder current phase state*/
#define QD_AL_STATUS				((u32)0x00000001 << 17)	/*Bit[17], Qdecoder auto load status*/
#define QD_DIR_STATE				((u32)0x00000001 <<16)	/*Bit[16], Qdecoder move direction*/
#define QD_PC						((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder current position counter*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_ISC
 * @{
 *****************************************************************************/
#define QD_IDX_EN					((u32)0x00000001 << 31)	/*Bit[31], Qdecoder index enable*/
#define QD_AUTO_IDX_EN				((u32)0x00000001 << 6)	/*Bit[6], Qdecoder Auto-index enable*/
#define BIT_QD_IDX_INVERSE				((u32)0x00000001 << 5)	/*Bit[5], Qdecoder index signal input inverse */
#define QD_POS_RST					((u32)0x00000003 << 3)	/*Bit[4:3], Qdecoder position reset control*/
#define QD_POS_RST_PHASE			((u32)0x00000003)			/*Bit[1:0], Qdecoder reset phase setting*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_VCTRL
 * @{
 *****************************************************************************/
#define QD_VT_DIV					((u32)0x000000FF << 16)	/*Bit[23:16], Qdecoder Velocity clock divider*/
#define QD_VMUC_MOD				((u32)0x00000001 << 9)	/*Bit[9], Qdecoder Velocity counter increase mode*/
#define QD_VUPLMT_INT_EN			((u32)0x00000001 << 7)	/*Bit[7], Qdecoder Velocity up limit interrupt enable */
#define QD_VLOWLMT_INT_EN			((u32)0x00000003 << 6)	/*Bit[6], Qdecoder Velocity low limit interrupt enable*/
#define QD_VCCAP_INT_EN			((u32)0x00000001 << 4)	/*Bit[4], Qdecoder Velocity counter capture interrupt enable */
#define QD_VELOCITY_INT				((u32)0x000000D0)
#define QD_VMUC_RST				((u32)0x00000001 << 2)	/*Bit[2], Qdecoder Velocity reset*/
#define QD_VMUC_EN					((u32)0x00000001)			/*Bit[0], Qdecoder Velocity enable*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_VC
 * @{
 *****************************************************************************/
#define QD_VC							((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder velocity accumulation counter*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_VCCAP
 * @{
 *****************************************************************************/
#define QD_VCCAP						((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder velocity accumulation counter capture register*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_PCCAP
 * @{
 *****************************************************************************/
#define QD_PCCAP						((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder position counter capture register*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_VTRLD
 * @{
 *****************************************************************************/
#define QD_VTRLD						((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder velocity timer reload register*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_VT
 * @{
 *****************************************************************************/
#define QD_VT							((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder velocity timer register*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_VCOMP
 * @{
 *****************************************************************************/
#define QD_VUP_LMT						((u32)0x0000FFFF << 16)	/*Bit[31:16], Qdecoder velocity counter up limit register*/
#define QD_VLOW_LMT					((u32)0x0000FFFF)			/*Bit[15:0], Qdecoder velocity counter low limit register*/
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_IMR
 * @{
 *****************************************************************************/
#define QD_RC_COMP_INT_MASK			((u32)0x00000001 << 15)	/*Bit[15], Qdecoder rotation counter compare INT mask*/
#define QD_VUPLMT_INT_MASK			((u32)0x00000001 << 13)	/*Bit[13], Qdecoder Velocity up limit interrupt mask */
#define QD_VLOWLMT_INT_MASK			((u32)0x00000001 << 12)	/*Bit[12], Qdecoder Velocity low limit interrupt mask*/
#define QD_VCCAP_INT_MASK				((u32)0x00000001 << 10)	/*Bit[10], Qdecoder Velocity counter capture interrupt mask*/
#define QD_PC_ERR_INT_MASK			((u32)0x00000001 << 9)	/*Bit[9], Qdecoder position counter error INT mask*/
#define QD_IDX_PULSE_INT_MASK			((u32)0x00000001 << 8)	/*Bit[8], Qdecoder index pulse INT mask*/
#define QD_RC_UF_INT_MASK				((u32)0x00000001 << 7)	/*Bit[7], Qdecoder rotation counter underflow INT mask*/
#define QD_RC_OF_INT_MASK				((u32)0x00000001 << 6)	/*Bit[6], Qdecoder rotation counter overflow INT mask*/
#define QD_PC_CMP_INT_MASK			((u32)0x00000001 << 5)	/*Bit[5], Qdecoder position counter compare INT mask*/
#define QD_DR_CH_INT_MASK				((u32)0x00000001 << 4)	/*Bit[4], Qdecoder direction changed INT mask*/
#define QD_PHASE_ILL_INT_MASK			((u32)0x00000001 << 3)	/*Bit[3], Qdecoder phase changed illegal INT mask*/
#define QD_PC_UF_INT_MASK				((u32)0x00000001 << 2)	/*Bit[2], Qdecoder position counter underflow INT mask*/
#define QD_PC_OF_INT_MASK				((u32)0x00000001 << 1)	/*Bit[1], Qdecoder position counter overflow INT mask*/
#define QD_PC_CHG_INT_MASK			((u32)0x00000001 << 0)	/*Bit[0], Qdecoder position counter changed INT mask*/
#define QD_ALL_INT_MASK				((u32)0x0000B7FF)
/** @} */

/**************************************************************************//**
 * @defgroup QDEC_ISR
 * @{
 *****************************************************************************/
#define QD_RC_COMP_INT_STATUS			((u32)0x00000001 << 15)	/*Bit[15], Qdecoder rotation counter compare INT status*/
#define QD_VUPLMT_INT_STATUS			((u32)0x00000001 << 13)	/*Bit[13], Qdecoder Velocity up limit interrupt status */
#define QD_VLOWLMT_INT_STATUS		((u32)0x00000001 << 12)	/*Bit[12], Qdecoder Velocity low limit interrupt status*/
#define QD_VCCAP_INT_STATUS			((u32)0x00000001 << 10)	/*Bit[10], Qdecoder Velocity counter capture interrupt status*/
#define QD_PC_ERR_INT_STATUS			((u32)0x00000001 << 9)	/*Bit[9], Qdecoder position counter error INT status*/
#define QD_IDX_PULSE_INT_STATUS		((u32)0x00000001 << 8)	/*Bit[8], Qdecoder index pulse INT status*/
#define QD_RC_UF_INT_STATUS			((u32)0x00000001 << 7)	/*Bit[7], Qdecoder rotation counter underflow INT status*/
#define QD_RC_OF_INT_STATUS			((u32)0x00000001 << 6)	/*Bit[6], Qdecoder rotation counter overflow INT status*/
#define QD_PC_CMP_INT_STATUS			((u32)0x00000001 << 5)	/*Bit[5], Qdecoder position counter compare INT status*/
#define QD_DR_CH_INT_STATUS			((u32)0x00000001 << 4)	/*Bit[4], Qdecoder direction changed INT status*/
#define QD_PHASE_ILL_INT_STATUS		((u32)0x00000001 << 3)	/*Bit[3], Qdecoder phase changed illegal INT status*/
#define QD_PC_UF_INT_STATUS			((u32)0x00000001 << 2)	/*Bit[2], Qdecoder position counter underflow INT status*/
#define QD_PC_OF_INT_STATUS			((u32)0x00000001 << 1)	/*Bit[1], Qdecoder position counter overflow INT status*/
#define QD_PC_CHG_INT_STATUS			((u32)0x00000001 << 0)	/*Bit[0], Qdecoder position counter changed INT status*/
#define QD_ALL_INT_STATUS				((u32)0x0000B7FF)
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/


#endif

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/

