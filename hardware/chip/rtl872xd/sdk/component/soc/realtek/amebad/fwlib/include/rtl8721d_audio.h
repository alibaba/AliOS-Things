/**
  ******************************************************************************
  * @file    rtl8721d_audio.h
  * @author
  * @version V1.0.0
  * @date    2017-12-13
  * @brief   This file contains all the functions prototypes for the audio codec firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _RTL8721D_AUDIO_H_
#define _RTL8721D_AUDIO_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup audio
  * @brief audio driver modules
  * @{
  */

/** @addtogroup audio
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * audio sport:
  * 	- Base Address: AUDIO_SPORT_DEV
  * 	- Source clk: 40MHz or 45.1584MHz or 98.304MHz(default)
  * 	- Sample rate: 8/16/32/44.1/48/88.2/96 /11.025/12/22.05 KHz
  * 	- Sample bit: 16 bit, 24 bit, 8bit
  * 	- Channel number: mono or stereo
  * 	- Data format: I2S, Left justified, PCM mode A, PCM mode B, PCM mode A-N, PCM mode B-N
  * 	- Use GDMA to move data
  *
  *****************************************************************************************
  * How to use audio sport
  *****************************************************************************************
  *      To use audio codec sport, the following steps are mandatory:
  *
  *      1. Open audio codec clock and function using
  *			PLLx_Set(0, ENABLE); (x is 0 or 1)
  *			RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, ENABLE);
  *			RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, ENABLE);
  *
  *      2. AUDIO SPORT pin setting:
  *			PAD_CMD(PinName, DISABLE).
  *
  *      3. Fill the AUDIO_SP_InitStruct with the desired parameters.
  *
  *      4. configure AUDIO SPORT with the corresponding configuration.
  *			AUDIO_SP_Init(AUDIO_SP_DEV, &SP_InitStruct)
  *
  *      5. According to audio codec transfer direction, start Tx or Rx or both path
  *		start Tx path:
  *			AUDIO_SP_TdmaCmd(AUDIO_SPORT_DEV, ENABLE);
  *			AUDIO_SP_TxStart(AUDIO_SPORT_DEV, ENABLE);
  *		start Rx path:
  *			AUDIO_SP_RdmaCmd(AUDIO_SPORT_DEV, ENABLE);
  *			AUDIO_SP_RxStart(AUDIO_SPORT_DEV, ENABLE);
  *
  *      6. Use AUDIO_SP_TXGDMA_Init or AUDIO_SP_RXGDMA_Init or both function to activate the GDMA according to transfer direction.
  *
  *      @note    All other functions can be used separately to modify, if needed,
  *			a specific feature of the AUDIO SPORT.
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup AUDIO_Exported_Types AUDIO Exported Types
  * @{
  */

/**
  * @brief	AUDIO SPORT Init structure definition
  */
typedef struct {
	u32 SP_WordLen;			/*!< Specifies the AUDIO SPORT word length
								This parameter can be a value of @ref SP_word_length */

	u32 SP_DataFormat; 		/*!< Specifies the AUDIO SPORT data format
								This parameter can be a value of @ref SP_data_format */

	u32 SP_MonoStereo;			/*!< Specifies the AUDIO SPORT channel number
								This parameter can be a value of @ref SP_channel_number */	

	u32 SP_SelRxCh;				/*!< Specifies the AUDIO SPORT selection of RX channel for ADC path
								This parameter can be a value of @ref SP_SEL_RX_channel */
} SP_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup AUDIO_Exported_Constants AUDIO Exported Constants
  * @{
  */
  
/** @defgroup SP_word_length AUDIO SPORT Word Length
  * @{
  */
#define SP_WL_16							((u32)0x00000000)
#define SP_WL_24							((u32)0x00000002)
#define SP_WL_8							((u32)0x00000003)

#define IS_SP_WORD_LEN(LEN) (((LEN) == SP_WL_16) || \
								((LEN) == SP_WL_24) || \
								((LEN) == SP_WL_8))

/**
  * @}
  */

/** @defgroup SP_data_format AUDIO SPORT Interface Format
  * @{
  */
#define SP_DF_I2S							((u32)0x00000000)
#define SP_DF_LEFT						((u32)0x00000001)
#define SP_DF_PCM_A						((u32)0x00000002)
#define SP_DF_PCM_B						((u32)0x00000003)
#define SP_DF_PCM_AN						((u32)0x00000006)
#define SP_DF_PCM_BN						((u32)0x00000007)

#define IS_SP_DATA_FMT(FORMAT) (((FORMAT) == SP_DF_I2S) || \
									((FORMAT) == SP_DF_LEFT) || \
									((FORMAT) == SP_DF_PCM_A) || \
									((FORMAT) == SP_DF_PCM_B) || \
									((FORMAT) == SP_DF_PCM_AN) || \
									((FORMAT) == SP_DF_PCM_BN))

/**
  * @}
  */

/** @defgroup SP_channel_number AUDIO SPORT Channel Number
  * @{
  */
#define SP_CH_STEREO						((u32)0x00000000)
#define SP_CH_MONO						((u32)0x00000001)

#define IS_SP_CHN_NUM(NUM) (((NUM) == SP_CH_STEREO) || \
								((NUM) == SP_CH_MONO))

/**
  * @}
  */

/** @defgroup SP_SEL_RX_channel AUDIO SPORT Selection of RX Channel
  * @{
  */
#define SP_RX_CH_LR						((u32)0x00000000)
#define SP_RX_CH_RL						((u32)0x00000001)
#define SP_RX_CH_LL						((u32)0x00000002)
#define SP_RX_CH_RR						((u32)0x00000003)

#define IS_SP_SEL_RX_CH(CH) (((CH) == SP_RX_CH_LR) || \
							((CH) == SP_RX_CH_RL) || \
							((CH) == SP_RX_CH_LL) || \
							((CH) == SP_RX_CH_RR))

/**
  * @}
  */
  
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup AUDIO_Exported_Functions AUDIO Exported Functions
  * @{
  */

/** @defgroup AUDIO_SPORT_functions 
  * @{
  */
_LONG_CALL_ void AUDIO_SP_StructInit(SP_InitTypeDef* SP_InitStruct);
_LONG_CALL_ void AUDIO_SP_Init(AUDIO_SPORT_TypeDef* SPORTx, SP_InitTypeDef* SP_InitStruct);
_LONG_CALL_ void AUDIO_SP_TxStart(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState);
_LONG_CALL_ void AUDIO_SP_RxStart(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState);
_LONG_CALL_ void AUDIO_SP_TdmaCmd(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState);
_LONG_CALL_ void AUDIO_SP_RdmaCmd(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState);
_LONG_CALL_ void AUDIO_SP_SetWordLen(AUDIO_SPORT_TypeDef* SPORTx, u32 SP_WordLen);
_LONG_CALL_ u32 AUDIO_SP_GetWordLen(AUDIO_SPORT_TypeDef* SPORTx);
_LONG_CALL_ void AUDIO_SP_SetMonoStereo(AUDIO_SPORT_TypeDef* SPORTx, u32 SP_MonoStereo);
_LONG_CALL_ BOOL AUDIO_SP_TXGDMA_Init(u32 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTxData, u32 Length);
_LONG_CALL_ BOOL AUDIO_SP_RXGDMA_Init(u32 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxData, u32 Length);
/**
  * @}
  */
/**
  * @}
  */
  
/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup AUDIO_Register_Definitions AUDIO Register Definitions
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup AUDIO_SI_Register_Definitions AUDIO SI Register Definitions
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup SI_CTRLR
 * @{
 *****************************************************************************/
#define BIT_CTRLR_SI_WR_START				((u32)0x00000001 << 0)
#define BIT_CTRLR_SI_RD_START				((u32)0x00000001 << 4)
#define BIT_CTRLR_SI_DISABLE					((u32)0x00000001 << 7)
#define BIT_CTRLR_SI_ADDR					((u32)0x000000FF << 8)
#define BIT_CTRLR_SI_DATA					((u32)0x0000FFFF << 16)
/** @} */

/**************************************************************************//**
 * @defgroup SI_CLK_EN
 * @{
 *****************************************************************************/
#define BIT_SI_CLK_EN							((u32)0x00000001 << 0)
/** @} */
/** @} */

/**************************************************************************//**
 * @defgroup AUDIO_SPORT_Register_Definitions AUDIO SPORT Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup SP_CTRLR0
 * @{
 *****************************************************************************/
#define SP_CTRLR0_RST						((u32)0x00000001 << 0)	/* Bit[0], reset SPORT1 module*/
#define SP_CTRLR0_TX_INV_I2S_SCLK			((u32)0x00000001 << 1)	/* Bit[1], invert sclk to TX path (DAC path)*/
#define SP_CTRLR0_RX_INV_I2S_SCLK			((u32)0x00000001 << 2)	/* Bit[2], invert sclk to RX path (ADC path)*/
#define SP_CTRLR0_SLAVE_CLK_SEL				((u32)0x00000001 << 3)	/* Bit[3], To be an I2S or PCM slave (CLK path)*/
#define SP_CTRLR0_SLAVE_DATA_SEL			((u32)0x00000001 << 4)	/* Bit[4], To be an I2S or PCM slave (data path)*/
#define SP_CTRLR0_WCLK_INV					((u32)0x00000001 << 5)	/* Bit[5], invert I2S/PCM word clock*/
#define SP_CTRLR0_LOOPBACK					((u32)0x00000001 << 6)	/* Bit[6], self loopback mode*/
#define SP_CTRLR0_DSP_CTL_MODE				((u32)0x00000001 << 7)	/* Bit[7], 1: DSP and SPORT1 handshaking is enabled; 0: GDMA and SPORT1 handshaking is enabled*/
#define SP_CTRLR0_DATA_FORMAT_SEL			((u32)0x00000003 << 8)	/* Bit[9:8], data format*/
#define SP_CTRLR0_EN_PCM_N_MODE_SEL		((u32)0x00000001 << 10)/* Bit[10], pcm n mode*/
#define SP_CTRLR0_EN_I2S_MONO				((u32)0x00000001 << 11)/* Bit[11], 1: mono; 0: stereo*/
#define SP_CTRLR0_DATA_LEN_SEL				((u32)0x00000003 << 12)/* Bit[13:12], data len*/
#define SP_CTRLR0_INV_I2S_SCLK				((u32)0x00000001 << 14)/* Bit[14], invert I2S/PCM bit clock*/
#define SP_CTRLR0_I2S_SELF_LPBK_EN			((u32)0x00000001 << 15)/* Bit[15], internal loopback mode*/
#define SP_CTRLR0_TX_DISABLE					((u32)0x00000001 << 16)/* Bit[16], disable or enable SPORT TX*/
#define SP_CTRLR0_START_TX					((u32)0x00000001 << 17)/* Bit[17], TX start*/
#define SP_CTRLR0_ADC_COMP					((u32)0x00000003 << 18)/* Bit[19:18], ADC compress*/
#define SP_CTRLR0_SEl_I2S_TX_CH				((u32)0x00000003 << 20)/* Bit[21:20], I2S TX channel select @ DAC path*/
#define SP_CTRLR0_TX_LSB_FIRST				((u32)0x00000001 << 22)/* Bit[22], TX MSB or LSB first select*/
#define SP_CTRLR0_RX_LSB_FIRST				((u32)0x00000001 << 23)/* Bit[23], RX MSB or LSB first select*/
#define SP_CTRLR0_RX_DISABLE					((u32)0x00000001 << 24)/* Bit[24], disable or enable SPORT RX*/
#define SP_CTRLR0_START_RX					((u32)0x00000001 << 25)/* Bit[25], RX start*/
#define SP_CTRLR0_DAC_COMP					((u32)0x00000003 << 26)/* Bit[27:26], DAC compress*/
#define SP_CTRLR0_SEL_I2S_RX_CH				((u32)0x00000003 << 28)/* Bit[29:28], I2S RX channel select @ ADC path*/
#define SP_CTRLR0_MCLK_SEL					((u32)0x00000001 << 30)/* Bit[30], MCLK output select*/
#define SP_CTRLR0_LONG_FRAME_SYNC			((u32)0x00000001 << 31)/* Bit[31], short frame sync or long frame sync select*/
/** @} */

/**************************************************************************//**
 * @defgroup SP_CTRLR1
 * @{
 *****************************************************************************/
#define SP_CTRLR1_FRAME_SYNC_OFFSET		((u32)0x000000FF)		/* Bit[7:0], To control the length of "long_frame_sync" signal when it is ON.*/ 
#define SP_CTRLR1_DEBUG_BUS_SEL				((u32)0x00000007 << 8)	/* Bit[10:8], debug_bus*/ 
#define SP_CTRLR1_CLEAR_TX_ERR_CNT			((u32)0x00000001 << 12)/* Bit[12], clear TX error counter*/ 	
#define SP_CTRLR1_CLEAR_RX_ERR_CNT			((u32)0x00000001 << 13)/* Bit[13], clear RX error counter*/ 
#define SP_CTRLR1_MODE_40MHZ				((u32)0x00000001 << 16)/* Bit[16], clock source is 40MHz or not*/ 
#define SP_CTRLR1_MODE_128FS				((u32)0x00000001 << 17)/* Bit[17], clock source is 128*fs or not*/ 
#define SP_CTRLR1_TDMA_REQ					((u32)0x00000001 << 18)/* Bit[18], Tx DAM request*/
#define SP_CTRLR1_RDMA_REQ					((u32)0x00000001 << 19)/* Bit[19], Rx DAM request*/
#define SP_CTRLR1_TX_SRC_BYTE_SWAP			((u32)0x00000001 << 20)/* Bit[20], swap H/L bytes read from the source memory*/ 
#define SP_CTRLR1_TX_SRC_LR_SWAP			((u32)0x00000001 << 21)/* Bit[21], swap L/R audio samples read from the source memory*/ 
#define SP_CTRLR1_RX_SNK_BYTE_SWAP			((u32)0x00000001 << 22)/* Bit[22], swap H/L bytes written to the sink memory*/ 
#define SP_CTRLR1_RX_SNK_LR_SWAP			((u32)0x00000001 << 23)/* Bit[23], swap L/R audio samples written to the sink memory*/ 
#define SP_CTRLR1_INT_ENABLE					((u32)0x000000FF << 24)/* Bit[31:24], for the interrupt of "sp_ready_to_tx"/"sp_ready_to_rx" */ 
/** @} */

/**************************************************************************//**
 * @defgroup SP_DSP_INT_CR
 * @{
 *****************************************************************************/
#define SP_TX_DSP_CLEAR_INT					((u32)0x00000001<<0)	/* Bit[0], clear TX interrupt (DSP mode) */ 
#define SP_RX_DSP_CLEAR_INT					((u32)0x00000001<<1)	/* Bit[1], clear RX interrupt (DSP mode)*/ 
#define SP_TX_FIFO_DEPTH_HALF_SEL			((u32)0x00000001<<4)	/* Bit[4], 1'b1 TX FIFO depth will reduce to half. It can reduce the data path latency*/ 
#define SP_RX_FIFO_DEPTH_HALF_SEL			((u32)0x00000001<<5)	/* Bit[5], 1'b1 RX FIFO depth will reduce to half. It can reduce the data path latency*/
#define SP_TX_DMA_SINGLE_NO_REQ				((u32)0x00000001<<18)	/* Bit[18], 1'b1 TX dma single no request*/
#define SP_RX_DMA_SINGLE_NO_REQ				((u32)0x00000001<<19)	/* Bit[19], 1'b1 RX dma single no request*/
/** @} */

/**************************************************************************//**
 * @defgroup SP_FIFO_SR
 * @{
 *****************************************************************************/
#define SP_TX0_WCNT_BUS						((u32)0x000000FF<<0)	/* Bit[7:0],TX0 FIFO write counter status (SPK path)*/
#define SP_TX1_WCNT_BUS						((u32)0x000000FF<<8)	/* Bit[15:8],TX1 FIFO write counter status (SPK path)*/
#define SP_RX0_RCNT_BUS						((u32)0x000000FF<<16)	/* Bit[23:16],RX0 FIFO read counter status (MIC path)*/
#define SP_RX1_RCNT_BUS						((u32)0x000000FF<<24)	/* Bit[31:24],RX1 FIFO read counter status (MIC path)*/
/** @} */

/**************************************************************************//**
 * @defgroup SP_ERROR_CNT_SR
 * @{
 *****************************************************************************/
#define SP_TX_ERR_CNT						((u32)0x000000FF<<0)	/* Bit[7:0],TX error counter (SPK path)*/
#define SP_RX_ERR_CNT						((u32)0x000000FF<<8)	/* Bit[15:8],RX error counter (MIC path)*/
/** @} */

/**************************************************************************//**
 * @defgroup SP_CLK_DIV
 * @{
 *****************************************************************************/
#define SP_CLK_MI							((u32)0x0000FFFF<<0)	/* Bit[15:0],BCLK clock divider */
#define SP_CLK_NI								((u32)0x00007FFF<<16)	/* Bit[30:16],BCLK clock divider */
#define SP_CLK_MI_NI_UPDATE					((u32)0x00000001<<31)	/* Bit[31],update "mi" and "ni" to get the new clock rate */
/** @} */
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */
/* Other Definitions --------------------------------------------------------*/
typedef struct
{
	AUDIO_SPORT_TypeDef* SPORTx;
	u32 Tx_HandshakeInterface;
	u32 Rx_HandshakeInterface;
} AUDIO_DevTable;

extern const AUDIO_DevTable AUDIO_DEV_TABLE[1];
#define AUDIO_BLOCK_SIZE 2048
#endif /* _RTL8721D_I2S_H_ */

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
