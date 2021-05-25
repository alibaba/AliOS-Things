/**
  ******************************************************************************
  * @file    rtl8721d_sdioh.h
  * @author
  * @version V1.0.0
  * @date    2018-06-29
  * @brief   This file contains all the functions prototypes for the SDIOH firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_SDIO_HOST_H
#define _RTL8721D_SDIO_HOST_H

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup SDIOH 
  * @brief SDIOH driver modules
  * @{
  */

/* Exported Types --------------------------------------------------------*/
/** @defgroup SDIOH_Exported_Types SDIOH Exported Types
  * @{
  */
/**
  * @brief	 SDIOH init structure definition
  */
typedef struct {
	u8 SDIOH_idle_level;	/*!> Indicate the idle pin level mask. When operate in 1-Bit bus width, this value is 0x03.
								When operate in 4-Bit bus width, this value is 0x1F. */
} SDIOH_InitTypeDef;

/**
  * @brief	 SDIOH DMA control structure definition
  */
typedef struct {
    u32 start_addr;		/*!< Specify the DMA start address. Unit: 8 Bytes. */
    u16 blk_cnt;		/*!< Specify the DMA transfer length.  Unit: 512 Bytes). */
    u8 op;				/*!< Specify the data move direction. Should be a value of @ref SDIOH_DMA_Operation. */
    u8 type;			/*!< Specify the transfer type. Shold be a value of @ref SDIOH_DMA_Transfer_Type. */
} SDIOH_DmaCtl;

/**
  * @brief	 SDIOH command parameters structure definition
  */
typedef struct{
    u32 arg;			/*!< Specify the argument to be transfered with command. */
    u8 idx;				/*!< Specify the command to be transfered. */
    u8 rsp_type;		/*!< Specify the response type. Should be a value of @ref SDIOH_Card_Response_Classfication. */
    u8 rsp_crc_chk;		/*!< Specify CRC7 check enable or not. Should be ENABLE or DISABLE. */
    u8 data_present;	/*!< Specify which thers is data need to read after get response from card. Should be a value of 
    						@ref SDIOH_Data_Present */
} SDIOH_CmdTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup SDIOH_Exported_Constants SDIOH Exported Constants
  * @{
  */

/** @defgroup SDIOH_Working_Mode
  * @{
  */
#define SDIOH_NORMAL_WRITE				0
#define SDIOH_AUTO_WRITE3				1
#define SDIOH_AUTO_WRITE4				2
#define SDIOH_AUTO_READ3				5
#define SDIOH_AUTO_READ4				6
#define SDIOH_SEND_CMD_GET_RSP			8
#define SDIOH_AUTO_WRITE1				9
#define SDIOH_AUTO_WRITE2				10
#define SDIOH_NORMAL_READ				12
#define SDIOH_AUTO_READ1				13
#define SDIOH_AUTO_READ2				14
#define SDIOH_TUNING					15
/**
  * @}
  */

/** @defgroup SDIOH_Card_Response_Type
  * @{
  */
#define SDIOH_NO_RESP					0
#define SDIOH_RESP_R1					1
#define SDIOH_RESP_R2					2
#define SDIOH_RESP_R3					3
#define SDIOH_RESP_R6					4
#define SDIOH_RESP_R7					5
/**
  * @}
  */

/** @defgroup SDIOH_Card_Response_Classification
  * @{
  */
#define SDIOH_NO_RESP					0
#define SDIOH_RSP_6B					1
#define SDIOH_RSP_17B					2
/**
  * @}
  */

/** @defgroup SDIOH_Data_Present
  * @{
  */
#define SDIOH_NO_DATA					0
#define SDIOH_DATA_EXIST				1
/**
  * @}
  */

/** @defgroup SDIOH_DMA_Transfer_Type
  * @{
  */
#define SDIOH_DMA_NORMAL				0
#define SDIOH_DMA_64B					1
#define SDIOH_DMA_R2					2
/**
  * @}
  */

/** @defgroup SDIOH_Mode_Definition
  * @{
  */
#define SDIOH_SD20_MODE					0
#define SDIOH_DDR_MODE					1
#define SDIOH_SD30_MODE					2
/**
  * @}
  */

/** @defgroup SDIOH_Bus_Width
  * @{
  */
#define SDIOH_BUS_WIDTH_1BIT			0
#define SDIOH_BUS_WIDTH_4BIT			1
/**
  * @}
  */

/** @defgroup SDIOH_DMA_Operation
  * @{
  */
#define SDIOH_DMA_WRITE					0
#define SDIOH_DMA_READ					1
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDIOH_Exported_Functions SDIOH Exported Functions
  * @{
  */

/** @defgroup SDIO_HOST_Functions SDIO Host Functions
  * @{
  */
_LONG_CALL_ u32 SDIOH_Busy(void);
_LONG_CALL_ u32 SDIOH_CheckTxError(u16 *status);
_LONG_CALL_ u32 SDIOH_WaitTxDone(u32 timeout_us);
_LONG_CALL_ u32 SDIOH_WaitDMADone(u32 timeout_us);
_LONG_CALL_ u32 SDIOH_GetISR(void);
_LONG_CALL_ void SDIOH_INTConfig(u8 SDIO_IT, u32 newState);
_LONG_CALL_ void SDIOH_INTClearPendingBit(u8 SDIO_IT);
_LONG_CALL_ u32 SDIOH_CheckBusState(u8 status, u32 timeout_us);
_LONG_CALL_ u8 SDIOH_GetBusWidth(void);
_LONG_CALL_ void SDIOH_SetBusWidth(u8 width);
_LONG_CALL_ void SDIOH_DMAConfig(SDIOH_DmaCtl *dma_ctl);
_LONG_CALL_ void SDIOH_DMAReset(void);
_LONG_CALL_ u32 SDIOH_SendCommand(SDIOH_CmdTypeDef *cmd_attrib, u32 timeout_us);
_LONG_CALL_ u8 SDIOH_GetResponse(u8 byte_index);
_LONG_CALL_ void SDIOH_SwitchSpeed(u8 clk_div, u8 mode);
_LONG_CALL_ u32 SDIOH_InitialModeCmd(u8 NewState, u8 Level);
_LONG_CALL_ u32 SDIOH_Init (u8 BusBitMode);
_LONG_CALL_ void SDIOH_DeInit (void);

/**
* @}
*/

/**
* @}
*/

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup SDIOH_Register_Definitions SDIOH Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup SDIOH_SRAM_CRL
 * @{
 *****************************************************************************/
#define SDIOH_SHIFT_LX_BURST_SIZE									6
#define SDIOH_LX_BURST_SIZE_64B										(0 << SDIOH_SHIFT_LX_BURST_SIZE)
#define SDIOH_SHIFT_MAP_SEL											5
#define SDIOH_MAP_SEL_DEC											(1 << SDIOH_SHIFT_MAP_SEL)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_DMA_CRL1
 * @{
 *****************************************************************************/
#define SDIOH_MASK_DRAM_SA											0x0FFFFFFF
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_DMA_CRL2
 * @{
 *****************************************************************************/
#define SDIOH_MASK_DMA_LEN											0x0000FFFF
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_DMA_CRL3
 * @{
 *****************************************************************************/
#define SDIOH_DAT64_SEL												BIT(5)
#define SDIOH_RSP17_SEL												BIT(4)
#define SDIOH_SHIFT_DDR_WR											1
#define SDIOH_DMA_XFER												BIT(0)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_ISR
 * @{
 *****************************************************************************/
#define SDIOH_DMA_TRANSFER_DONE										BIT(4)
#define SDIOH_CARD_ERROR											BIT(2)
#define SDIOH_CARD_END												BIT(1)
#define SDIOH_SD_ISR_ALL											(0x16)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_ISREN
 * @{
 *****************************************************************************/
#define SDIOH_DMA_CTL_INT_EN										BIT(4)
#define SDIOH_CARD_ERR_INT_EN										BIT(2)
#define SDIOH_CARD_END_INT_EN										BIT(1)
#define SDIOH_WRITE_DATA											BIT(0)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_CKGEN_CTL
 * @{
 *****************************************************************************/
#define SDIOH_SHIFT_SD30_SAMP_CLK_SRC								12
#define SDIOH_SHIFT_SD30_PUSH_CLK_SRC								8
#define SDIOH_SHIFT_CRC_CLK_SRC										4
#define SDIOH_SD30_SAMP_CLK_VP1										(2 << SDIOH_SHIFT_SD30_SAMP_CLK_SRC)
#define SDIOH_SD30_PUSH_CLK_VP0										(1 << SDIOH_SHIFT_SD30_PUSH_CLK_SRC)
#define SDIOH_CRC_CLK_SSC											(0 << SDIOH_SHIFT_CRC_CLK_SRC)
#define SDIOH_MASK_CLKDIV											(0x7)
#define SDIOH_CLK_DIV1												0
#define SDIOH_CLK_DIV2												1
#define SDIOH_CLK_DIV4												2
#define SDIOH_CLK_DIV8												3

/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_CARD_STOP
 * @{
 *****************************************************************************/
#define SDIOH_TARGET_MODULE_SD										BIT(2)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_CARD_SELECT
 * @{
 *****************************************************************************/
#define SDIOH_CARD_SEL_SD_MODULE									0x2
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_CARD_EXIST
 * @{
 *****************************************************************************/
#define SDIOH_SD_WP													BIT(5)
#define SDIOH_SD_EXIST												BIT(2)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_CARD_INT_EN
 * @{
 *****************************************************************************/
#define SDIOH_SDMMC_INT_EN											BIT(2)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_CARD_INT_PEND
 * @{
 *****************************************************************************/
#define SDIOH_SDMMC_INT_PEND										BIT(2)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_CARD_CLK_EN_CTL
 * @{
 *****************************************************************************/
#define SDIOH_SD_CARD_MOUDLE_EN										BIT(2)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_CONFIG1
 * @{
 *****************************************************************************/
#define SDIOH_INITIAL_MODE											BIT(7)
#define SDIOH_CLK_DIV_BY_128										0
#define SDIOH_CLK_DIV_BY_256										BIT(6)
#define SDIOH_SD30_ASYNC_FIFO_RST_N									BIT(4)
#define SDIOH_SHIFT_MODE_SEL										2
#define SDIOH_MASK_MODE_SEL											0xc
#define SDIOH_SHIFT_BUS_WIDTH										0
#define SDIOH_MASK_BUS_WIDTH										0x3
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_CONFIG2
 * @{
 *****************************************************************************/
#define SDIOH_CRC7_CAL_EN											0
#define SDIOH_CRC7_CAL_DIS											BIT(7)
#define SDIOH_CRC16_CHK_EN											0
#define SDIOH_CRC16_CHK_DIS											BIT(6)
#define SDIOH_WAIT_WR_CRCSTA_TO_EN									0
#define SDIOH_WAIT_WR_CRCSTA_TO_DIS									BIT(5)
#define SDIOH_IGNORE_WR_CRC_ERR_EN									0
#define SDIOH_IGNORE_WR_CRC_ERR_DIS									BIT(4)
#define SDIOH_WAIT_BUSY_END_DIS										0
#define SDIOH_WAIT_BUSY_END_EN										BIT(3)
#define SDIOH_CRC7_CHK_EN											0
#define SDIOH_CRC7_CHK_DIS											BIT(2)

/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_CONFIG3
 * @{
 *****************************************************************************/
#define SDIOH_STOP_STA_WAIT_BUSY_EN									0
#define SDIOH_STOP_STA_WAIT_BUSY_DIS								BIT(7)
#define SDIOH_CMD_STA_WAIT_BUSY_EN									0
#define SDIOH_CMD_STA_WAIT_BUSY_DIS									BIT(6)
#define SDIOH_DATA_PHA_WAIT_BUSY_EN									BIT(5)
#define SDIOH_DATA_PHA_WAIT_BUSY_DIS								0
#define SDIOH_SD30_CLK_STOP_EN										BIT(4)
#define SDIOH_SD30_CLK_STOP_DIS										0
#define SDIOH_SD20_CLK_STOP_EN										BIT(3)
#define SDIOH_SD20_CLK_STOP_DIS										0
#define SDIOH_SD_CMD_RESP_CHK_EN									BIT(2)
#define SDIOH_SD_CMD_RESP_CHK_DIS									0
#define SDIOH_ADDR_MODE_SECTOR										0
#define SDIOH_ADDR_MODE_BYTE										BIT(1)
#define SDIOH_CMD_RESP_TO_EN										BIT(0)
#define SDIOH_CMD_RESP_TO_DIS										0
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_STATUS1_2
 * @{
 *****************************************************************************/
#define SDIOH_SD_TUNNING_PAT_COMP_ERR								BIT(0)
#define SDIOH_GET_WRCRC_STA_TO_ERR									BIT(1)
#define SDIOH_MASK_WR_CRC_STA										0x1C
#define SDIOH_WR_CRC_ERR											BIT(5)
#define SDIOH_CRC16_ERR												BIT(6)
#define SDIOH_CRC7_ERR												BIT(7)
#define SDIOH_SD_CMD_RSP_TO_ERR										BIT(8)
#define SDIOH_SD_CMD_RSP_INVALID									BIT(9)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_BLOCK_CNT_L
 * @{
 *****************************************************************************/
#define SDIOH_MASK_BLOCL_CNT_L										0xFF
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_BLOCK_CNT_H
 * @{
 *****************************************************************************/
#define SDIOH_MASK_BLOCL_CNT_H										0x7F
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_TRANSFER
 * @{
 *****************************************************************************/
#define SDIOH_START_TRANSFER										BIT(7)
#define SDIOH_TRANSFER_END											BIT(6)
#define SDIOH_SD_MODULE_FSM_IDLE									BIT(5)
#define SDIOH_ERR_OCCUR												BIT(4)
#define SDIOH_MASK_COM_CODE											0xF
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_CMD_STATE
 * @{
 *****************************************************************************/
#define SDIOH_CMD_FSM_IDLE											BIT(7)
/** @} */

/**************************************************************************//**
 * @defgroup SDIOH_SD_DATA_STATE
 * @{
 *****************************************************************************/
#define SDIOH_DATA_FSM_IDLE											BIT(7)
/** @} */
/** @} */

/**
* @}
*/

/**
* @}
*/


#undef SDIOH_SUPPORT_SD30

#define SDIOH_CMD_CPLT_TIMEOUT		  	5000		/* Max. timeout value when checking the flag of command complete, unit: us */
#define SDIOH_XFER_CPLT_TIMEOUT			1000000	/* Max. timeout value when checking the flag of transfer complete, unit: us */

#define SDIOH_READ_TIMEOUT				100000
#define SDIOH_WRITE_TIMEOUT				250000
#define SDIOH_ERASE_TIMEOUT				2000000//250000

#define HOST_COMMAND					BIT(6)	/* Transmission bit of register "SD_CMD0", indicating the direction of transmission (host = 1)*/
#define SDIOH_CMD_IDX_MASK				0x3F		/* Command index mask of register "SD_CMD0" */
#define SDIOH_CMD8_VHS					0x1		/* Value of "VHS" field in CMD8, 2.7-3.6V */
#define SDIOH_CMD8_CHK_PATN				0xAA		/* Value of "Check pattern" field in CMD8 */
#define SDIOH_OCR_VDD_WIN				0xFF8000	/* Value of "OCR" field in ACMD41, OCR bit[23:0] */

#define SDIOH_C6R2_BUF_LEN				64		/* Buffer for CMD6, R2, etc.*/
#define SDIOH_CSD_LEN					16

/* SDIOH_Card_Response_Byte_Index */
#define SDIO_RESP0						0
#define SDIO_RESP1						1
#define SDIO_RESP2						2
#define SDIO_RESP3						3
#define SDIO_RESP4						4
#define SDIO_RESP5						5

/* SDIOH_Signal_Level */
#define SDIOH_SIG_VOL_33				0
#define SDIOH_SIG_VOL_18	 			1



#endif
