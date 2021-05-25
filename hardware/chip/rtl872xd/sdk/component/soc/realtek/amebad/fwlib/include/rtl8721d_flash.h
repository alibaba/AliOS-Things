/**
  ******************************************************************************
  * @file    rtl8721d_flash.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the Flash firmware
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

#ifndef _RTL8710B_SPI_FLASH_H
#define _RTL8710B_SPI_FLASH_H

#include "rtl8721d_flashclk.h"

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup FLASH 
  * @brief SPI Flash driver modules
  * @{
  */

/** @addtogroup FLASH
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * SPI_Flash_Controller is used to communicate with SPI_Flash.
  * Support auto mode and user mode to access Flash.
  * Support multi-channel(1/2/4)data bit to transmit/receive.
  * Programmable feature:
  *		SPI channel number - Control channel bits of serial transfer.
  *		SPI Clock rate - Control the bit rate of serial transfer.
  *		Flash command registers - Flexible to set the different command codes for 
  *			different flash vendors in automatic mode.
  *		Dummy cycles- Allow users to add dummy cycles in receiving data path for 
  *			timing tuning or extra pipelining registers.
  *		Flash address size- Define the size of Flash to enable the slave select output 
  *			in automatic mode.
  *
  *****************************************************************************************
  * How to use SPI_Flash_Controller to Read/Program/Erase flash
  *****************************************************************************************
  * To use the SPI_Flash_Controller to Read/Program/Erase flash, you can follow the steps
  * below:
  *       
  *   1. Enable SPIC clock by using RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK_XTAL, ENABLE) function.
  *
  *   2. Call Pinmux_SpicCtrl(PinLocation, ON) to configure SPIC pinmux.
  *
  *   3. Fill the variable flash_init_para of type FLASH_InitTypeDef with default parameters 
  *      using one of the following functions according to flash vendor:
  *               flash vendor            Struct Init Function
  *               Winbond                 FLASH_StructInit(&flash_init_para)         
  *               Gigadevice              FLASH_StructInit_GD(&flash_init_para)
  *               Micron                  FLASH_StructInit_Micron(&flash_init_para)
  *               MXIC                    FLASH_StructInit_MXIC(&flash_init_para)
  *
  *         Note: (1)We support 4 flash chip vendors above in SDK.  
  *               (2)If the flash chip type is Gigadevic and flash size is more than 2MB, 
  *               you must set the FLASH_cmd_wr_status2 parameter in flash_init_para 
  *               according to spec, because the Write Status Register1 command is
  *               different from write Status Register2 command.
  *               (3)If it is Micron flash chip, it is mandatory to set dummy cycles to 
  *               Nonvolatile Configuration Register of flash. The number of dummy cycles is
  *               determined by Clock Frequency and Bit Mode according to spec.
  *               (4)If the flash chip type is Micron N25q00aa, you must set FLASH_cmd_chip_e
  *               parameter according to spec because it is different from default setting.
  *
  *   4. Initialize SPIC to designated BitMode using FLASH_Init().
  * 
  *   5. Enable Quad I/O by setting QuadEnable bit in FLASH status register.
  *   
  *   6. Switch SPIC to higher clock rate using RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK_PLL, ENABLE) function.
  *
  *   7. Calibrate by calling FLASH_CalibrationNew() function.
  * 
  *   8. Then you can Read/Program/Erase flash by calling corresponding functions.
  *      Remember to add flash_write_lock() function before those operations and 
  *      flash_write_unlock() after them to protect them when FLASH XIP. 
  *
  *       Note: (1)If flash code has some updates when XIP, you need to flush cache by calling
  *             Cache_Flush() function.
  *             (2)When XIP, the flash initialization and calibration have already finished,
  *             you can Read/Program/Erase flash directly without excuting 1~6 steps described
  *             above.
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup FLASH_Exported_Types FLASH Exported Types
  * @{
  */

/** 
  * @brief  FLASH Init structure definition  
  */
typedef struct {
	u32 FLASH_Id;				/*!< Specifies the flash vendor ID.
								This parameter can be a value of @ref FLASH_VENDOR_ID_definitions */
	u8 FLASH_cur_bitmode;		/*!< Specifies the current bitmode of SPIC.
								This parameter can be a value of @ref FLASH_BIT_Mode_definitions */
	u8 FLASH_baud_rate;			/*!< Specifies the spi_sclk divider value. The frequency of spi_sclk is derived from:
								Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */
	u8 FLASH_baud_boot;			/*!< Specifies the spi_sclk divider value for rom boot. The frequency of spi_sclk is derived from:
								Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */
	u32 FLASH_cur_cmd; 			/*!< Specifies the current read cmd which is used to read data from flash
								in current bitmode. */
						
	/* status bits define */
	u32 FLASH_QuadEn_bit;		/*!< Specifies the QE bit in status register which is used to enable Quad I/O mode . */
	u32 FLASH_Busy_bit;			/*!< Specifies the WIP(Write in Progress) bit in status register which indicates whether 
								the device is busy in program/erase/write status register progress. */
	u32 FLASH_WLE_bit; 			/*!< Specifies the WEL(Write Enable Latch) bit in status register which indicates whether
								the device will accepts program/erase/write status register instructions*/
	u32 FLASH_Status2_exist;		/*!< Specifies whether this flash chip has Status Register2 or not.
								This parameter can be 0/1. 0 means it doesn't have Status Register2, 1 means 
								it has Status Register2.*/
	
	/* calibration data */
	u8 FLASH_rd_sample_phase_cal;	/*!< Specifies the read sample phase obtained from calibration. this is cal sample phase get from high speed cal */
	u8 FLASH_rd_sample_phase;	/*!< Specifies the read sample phase obtained from calibration. this is current sample phase */
	u8 FLASH_rd_dummy_cyle[3];	/*!< Specifies the read dummy cycle of different bitmode according to 
								flash datasheet*/

	/* valid R/W command set */
	u32 FLASH_rd_dual_o; 			/*!< Specifies dual data read cmd */
	u32 FLASH_rd_dual_io; 			/*!< Specifies dual data/addr read cmd */
	u32 FLASH_rd_quad_o; 		/*!< Specifies quad data read cmd */
	u32 FLASH_rd_quad_io; 		/*!< Specifies quad data/addr read cmd */   
	u32 FLASH_wr_dual_i; 			/*!< Specifies dual data write cmd */
	u32 FLASH_wr_dual_ii;			/*!< Specifies dual data/addr write cmd */
	u32 FLASH_wr_quad_i; 			/*!< Specifies quad data write cmd */
	u32 FLASH_wr_quad_ii;			/*!< Specifies quad data/addr write cmd */
	u32 FALSH_dual_valid_cmd;		/*!< Specifies valid cmd of dual bitmode to program/read flash in auto mode */
	u32 FALSH_quad_valid_cmd;	/*!< Specifies valid cmd of quad bitmode to program/read flash in auto mode */

	/* other command set */
	u8 FLASH_cmd_wr_en;			/*!< Specifies the Write Enable(WREN) instruction which is for setting WEL bit*/
	u8 FLASH_cmd_rd_id;			/*!< Specifies the Read ID instruction which is for getting the identity of the flash divice.*/
	u8 FLASH_cmd_rd_status;		/*!< Specifies the Read Status Register instruction which is for getting the status of flash */
	u8 FLASH_cmd_rd_status2;		/*!< Specifies the Read Status Register2 instruction which is for getting the status2 of flash */
	u8 FLASH_cmd_wr_status;		/*!< Specifies the Write Status Register instruction which is for setting the status register of flash */
	u8 FLASH_cmd_wr_status2;		/*!< Specifies the Write Status Register2 instruction which is for setting the status register2 of flash. 
								 In some flash chips, status2 write cmd != status1 write cmd, 
								 like: GD25Q32C, GD25Q64C,GD25Q128C etc.*/
	u8 FLASH_cmd_chip_e;			/*!< Specifies the Erase Chip instruction which is for erasing the whole chip*/
	u8 FLASH_cmd_block_e;		/*!< Specifies the Erase Block instruction which is for erasing 64kB*/
	u8 FLASH_cmd_sector_e;		/*!< Specifies the Erase Sector instruction which is for erasing 4kB*/
	u8 FLASH_cmd_pwdn_release;	/*!< Specifies the Release from Deep Power Down instruction which is for exiting power down mode.*/
	u8 FLASH_cmd_pwdn;			/*!< Specifies the Deep Power Down instruction which is for entering power down mode.*/

	/* debug log */
	u8 debug;					/*!< Specifies whether or not to print debug log.*/

	/* new calibration */
	u8 phase_shift_idx;			/*!< Specifies the phase shift idx in new calibration.*/

	u8 FLASH_addr_phase_len;	/*!< Specifies the number of bytes in address phase (between command phase and write/read phase).
								This parameter can be 0/1/2/3. 0 means 4-byte address mode in SPI Flash.*/
	u8 FLASH_pseudo_prm_en;		/*!< Specifies whether SPIC enables SPIC performance read mode or not.*/
	u8 FLASH_pinmux;			/*!< Specifies which pinmux is used. PINMUX_S0 or PINMUX_S1*/			

	u32 FLASH_rd_fast_single;	/*!< Specifies fast read cmd in auto mode.*/
} FLASH_InitTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
  * @{
  */ 

/** @defgroup FLASH_CLK_Div_definitions 
  * @{
  */ 
#define FLASH_CLK_DIV2P0		0
#define FLASH_CLK_DIV2P5		1
#define FLASH_CLK_DIV3P0		2
#define FLASH_CLK_DIV3P5		3
#define FLASH_CLK_DIV4P0		4
/**
  * @}
  */

/** @defgroup FLASH_BIT_Mode_definitions 
  * @{
  */
#define SpicOneBitMode			0
#define SpicDualBitMode			1
#define SpicQuadBitMode		2
/**
  * @}
  */

/** @defgroup FLASH_ERASE_Type_definitions 
  * @{
  */
#define EraseChip				0
#define EraseBlock				1
#define EraseSector			2
/**
  * @}
  */

/** @defgroup FLASH_WAIT_Type_definitions 
  * @{
  */
#define WAIT_SPIC_BUSY		0
#define WAIT_FLASH_BUSY		1
#define WAIT_WRITE_DONE		2
#define WAIT_WRITE_EN		3
/**
  * @}
  */

/** @defgroup SPIC_ADDR_PHASE_LEN_definitions 
  * @{
  */
#define ADDR_3_BYTE				0x3
#define ADDR_4_BYTE				0x0
#define ADDR_3_BYTE_USER_PRM		0x0
#define ADDR_4_BYTE_USER_PRM		0x4
/**
  * @}
  */

/** @defgroup WINBOND_W25Q16DV_Spec
  * @{
  */
#define FLASH_CMD_WREN			0x06            //write enable
#define FLASH_CMD_WRDI			0x04            //write disable
#define FLASH_CMD_WRSR			0x01            //write status register
#define FLASH_CMD_RDID			0x9F            //read idenfication
#define FLASH_CMD_RDSR			0x05            //read status register
#define FLASH_CMD_RDSR2			0x35            //read status register-2
#define FLASH_CMD_READ			0x03            //read data
#define FLASH_CMD_FREAD			0x0B            //fast read data
#define FLASH_CMD_RDSFDP		0x5A            //Read SFDP
#define FLASH_CMD_RES			0xAB            //Read Electronic ID
#define FLASH_CMD_REMS			0x90            //Read Electronic Manufacturer & Device ID
#define FLASH_CMD_DREAD			0x3B            //Double Output Mode command
#define FLASH_CMD_SE				0x20            //Sector Erase
#define FLASH_CMD_BE				0xD8            //0x52 //64K Block Erase
#define FLASH_CMD_CE				0x60            //Chip Erase(or 0xC7)
#define FLASH_CMD_PP				0x02            //Page Program
#define FLASH_CMD_DP				0xB9            //Deep Power Down
#define FLASH_CMD_RDP			0xAB            //Release from Deep Power-Down
#define FLASH_CMD_2READ			0xBB            // 2 x I/O read  command
#define FLASH_CMD_4READ			0xEB            // 4 x I/O read  command
#define FLASH_CMD_QREAD			0x6B            // 1I / 4O read command
#define FLASH_CMD_4PP			0x32            //quad page program //this is diff with MXIC
#define FLASH_CMD_FF				0xFF            //Release Read Enhanced
#define FLASH_CMD_REMS2			0x92            // read ID for 2x I/O mode //this is diff with MXIC
#define FLASH_CMD_REMS4			0x94            // read ID for 4x I/O mode //this is diff with MXIC
#define FLASH_CMD_RDSCUR		0x48            // read security register //this is diff with MXIC
#define FLASH_CMD_WRSCUR		0x42            // write security register //this is diff with MXIC

#define FLASH_DM_CYCLE_2O		0x08
#define FLASH_DM_CYCLE_2IO		0x04
#define FLASH_DM_CYCLE_4O		0x08
#define FLASH_DM_CYCLE_4IO		0x06

#define FLASH_STATUS_BUSY		((u32)0x00000001)
#define FLASH_STATUS_WLE		((u32)0x00000002)
/**
  * @}
  */
  
/** @defgroup WINBOND_W25Q256FV_Spec
  * @{
  */

#define FLASH_CMD_ENT_ADDR4B 	0xB7
#define FLASH_CMD_EXT_ADDR4B	0xE9

/**
  * @}
  */

/** @defgroup FLASH_VENDOR_ID_definitions
  * @{
  */
#define FLASH_ID_OTHERS			0
#define FLASH_ID_MXIC			1
#define FLASH_ID_WINBOND		2
#define FLASH_ID_MICRON			3
#define FLASH_ID_EON				4
#define FLASH_ID_GD				5
#define FLASH_ID_BOHONG			6

/**
  * @}
  */

/** @defgroup FLASH_MANUFACTURER_ID_definitions
  * @{
  */
#define MANUFACTURER_ID_MXIC			0xC2
#define MANUFACTURER_ID_WINBOND		0xEF
#define MANUFACTURER_ID_MICRON			0x20
#define MANUFACTURER_ID_BOHONG			0x68
#define MANUFACTURER_ID_GD				0xC8
#define MANUFACTURER_ID_EON				0x1C
#define MANUFACTURER_ID_FM				0xA1

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup FLASH_Exported_Functions FLASH Exported Functions
  * @{
  */
_LONG_CALL_ void FLASH_Erase(u32 EraseType, u32 Address);
_LONG_CALL_ void FLASH_SetStatus(u8 Cmd, u32 Len, u8* Status);
_LONG_CALL_ void FLASH_SetStatusBits(u32 SetBits, u32 NewState);
_LONG_CALL_ void FLASH_WaitBusy(u32 WaitType);
_LONG_CALL_ void FLASH_WriteEn(void);
_LONG_CALL_ void FLASH_TxCmd(u8 cmd, u8 DataPhaseLen, u8* pData);
_LONG_CALL_ void FLASH_RxCmd(u8 cmd, u32 read_len, u8* read_data);
_LONG_CALL_ void FLASH_StructInit(FLASH_InitTypeDef* FLASH_InitStruct);
_LONG_CALL_ void FLASH_StructInit_Micron(FLASH_InitTypeDef* FLASH_InitStruct);
_LONG_CALL_ void FLASH_StructInit_MXIC(FLASH_InitTypeDef* FLASH_InitStruct);
_LONG_CALL_ void FLASH_StructInit_GD(FLASH_InitTypeDef* FLASH_InitStruct);
_LONG_CALL_ u8 FLASH_Init(u8 SpicBitMode);
_LONG_CALL_ void FLASH_SetSpiMode(FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode);
_LONG_CALL_ void FLASH_DeepPowerDown(u32 NewState);
_LONG_CALL_ void FLASH_TxData256B(u32 StartAddr, u32 DataPhaseLen, u8* pData);
_LONG_CALL_ void FLASH_TxData12B(u32 StartAddr, u8 DataPhaseLen, u8* pData);
_LONG_CALL_ void FLASH_RxData(u8 cmd, u32 StartAddr, u32 read_len, u8* read_data);
_LONG_CALL_ u32 FLASH_Calibration(FLASH_InitTypeDef* FLASH_InitStruct, u8 SpicBitMode, u8 LineDelay);

_LONG_CALL_ u32 FLASH_ClockDiv(u8 Div);
_LONG_CALL_ u32 FLASH_CalibrationNew(FLASH_InitTypeDef* FLASH_InitStruct, u8 SpicBitMode, u8 Div, u8 CalStep, u8 LineDelay, u8 StartIdx);
_LONG_CALL_ u32 FLASH_CalibrationNewCmd(u32 NewStatus);
_LONG_CALL_ u32 FLASH_CalibrationPhaseIdx(u8 phase_idx);
_LONG_CALL_ u32 FLASH_CalibrationPhase(u8 phase_int, u8 phase_sel);
_LONG_CALL_ u32 FLASH_Calibration500MPSCmd(u32 NewStatus);
_LONG_CALL_ u32 FLASH_CalibrationInit(u8 CalibrationEnd);
_LONG_CALL_ void FLASH_ClockSwitch(u32 Source, u32 Protection);
_LONG_CALL_ int  FLASH_WriteStream(u32 address, u32 len, u8 * data);

/**
  * @}
  */

/** @defgroup FLASH_Exported_XIP_Functions FLASH Exported XIP Functions
  * @note these functions will lock cpu when exec to forbit XIP, and flush cache after exec
  * @{
  */
void FLASH_Write_Lock(void);
void FLASH_Write_Unlock(void);
void FLASH_RxCmdXIP(u8 cmd, u32 read_len, u8* read_data);
void FLASH_SetStatusXIP(u8 Cmd, u32 Len, u8* Status);
void FLASH_SetStatusBitsXIP(u32 SetBits, u32 NewState);
void FLASH_TxData12BXIP(u32 StartAddr, u8 DataPhaseLen, u8* pData);
void FLASH_TxData256BXIP(u32 StartAddr, u32 DataPhaseLen, u8* pData);
void FLASH_EraseXIP(u32 EraseType, u32 Address);
void FLASH_EreaseDwordsXIP(u32 address, u32 dword_num);
void FLASH_Write_IPC_Int(VOID *Data, u32 IrqStatus, u32 ChanNum);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup FLASH_Register_Definitions FLASH Register Definitions
 * @{
 *****************************************************************************/
   
/**************************************************************************//**
 * @defgroup SPIC_CTRLR0
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_CTRLR0 register  *******************/
#define BIT_CMD_CH(x)				(((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)			(((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)			(((x) & 0x00000003) << 16)
#define BIT_TMOD(x)				(((x) & 0x00000003) << 8)
#define BIT_SCPOL					(0x00000001 << 7)
#define BIT_SCPH					(0x00000001 << 6)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_CTRLR1
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_CTRLR1 register  *******************/
#define BIT_NDF(x)					((x) & 0xffff)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_SSIENR
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_SSIENR register  *******************/
#define BIT_ATCK_CMD				(0x00000001 << 1)
#define BIT_SPIC_EN				(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_SER
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_SER register  *******************/
#define BIT_SER					(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_BAUDR
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_BAUDR register  *******************/
#define BIT_SCKDV(x)				((x) & 0x0fff)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_SR
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_SR register  *******************/
#define BIT_TXE					(0x00000001 << 5)
#define BIT_RFF					(0x00000001 << 4)
#define BIT_RFNE					(0x00000001 << 3)
#define BIT_TFE					(0x00000001 << 2)
#define BIT_TFNF					(0x00000001 << 1)
#define BIT_BUSY					(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_IMR
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_IMR register  *******************/
#define BIT_TXSIM					(0x00000001 << 9)
#define BIT_ACEIM                             	(0x00000001 << 8)
#define BIT_BYEIM                             	(0x00000001 << 7)
#define BIT_WBEIM                             	(0x00000001 << 6)
#define BIT_FSEIM                             	(0x00000001 << 5)
#define BIT_RXFIM                             	(0x00000001 << 4)
#define BIT_RXOIM                             	(0x00000001 << 3)
#define BIT_RXUIM                             	(0x00000001 << 2)
#define BIT_TXOIM                             	(0x00000001 << 1)
#define BIT_TXEIM                             	(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_ISR
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_ISR register  *******************/
#define BIT_TXSIS                             	(0x00000001 << 9)
#define BIT_ACEIS                             	(0x00000001 << 8)
#define BIT_BYEIS                             	(0x00000001 << 7)
#define BIT_WBEIS                             	(0x00000001 << 6)
#define BIT_FSEIS                             	(0x00000001 << 5)
#define BIT_RXFIS                             	(0x00000001 << 4)
#define BIT_RXOIS                             	(0x00000001 << 3)
#define BIT_RXUIS                             	(0x00000001 << 2)
#define BIT_TXOIS                             	(0x00000001 << 1)
#define BIT_TXEIS                             	(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_RISR
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_RISR register  *******************/
#define BIT_ACEIR                             	(0x00000001 << 8)
#define BIT_BYEIR                             	(0x00000001 << 7)
#define BIT_WBEIR                             	(0x00000001 << 6)
#define BIT_FSEIR                             	(0x00000001 << 5)
#define BIT_RXFIR                             	(0x00000001 << 4)
#define BIT_RXOIR                             	(0x00000001 << 3)
#define BIT_RXUIR                             	(0x00000001 << 2)
#define BIT_TXOIR                             	(0x00000001 << 1)
#define BIT_TXEIR                             	(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_CTRLR2
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_CTRLR2 register  *******************/
#define BIT_FIFO_ENTRY(x)			(((x) & 0x0000000f) << 4)
#define BIT_WR_SEQ				(0x00000001 << 3)
#define BIT_WPN_DNUM			(0x00000001 << 2) /* Indicate the WPn input pin of SPI Flash is connected to, 0(default): WP=spi_sout[2], 1:WP=spi_sout[3]. */
#define BIT_WPN_SET				(0x00000001 << 1) /* To implement write protect function. spi_wen_out and the bit of spi_sout which connects to WPN would be initial to 0. */
#define BIT_SO_DUM				(0x00000001) /* SO input pin of SPI Flash, 0: SO connects to spi_sout[0]. 1(default): SO connects to spi_sout[1].*/
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_ADDR_LENGTH
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_ADDR_LENGTH register  *******************/
#define BIT_ADDR_PHASE_LENGTH(x)	((x) & 0x00000007)
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_AUTO_LENGTH
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_AUTO_LENGTH register  *******************/
#define BIT_CS_H_WR_DUM_LEN(x)	(((x) & 0x0000000f) << 28)
#define BIT_CS_H_RD_DUM_LEN(x)	(((x) & 0x00000003) << 26)
#define BIT_AUTO_DUM_LEN(x)		(((x) & 0x000000ff) << 18)
#define BIT_AUTO_ADDR_LENGTH(x)	(((x) & 0x00000003) << 16)
#define BIT_RD_DUMMY_LENGTH(x)	(((x) & 0x00000fff))
/** @} */

/**************************************************************************//**
 * @defgroup SPIC_VALID_CMD
 * @{
 *****************************************************************************/
/********************  Bits definition for SPIC_VALID_CMD register  *******************/
#define BIT_CTRLR0_CH			(0x00000001 << 12)
#define BIT_PRM_EN				(0x00000001 << 11)
#define BIT_WR_BLOCKING			(0x00000001 << 9)
#define BIT_WR_QUAD_II			(0x00000001 << 8)
#define BIT_WR_QUAD_I			(0x00000001 << 7)
#define BIT_WR_DUAL_II			(0x00000001 << 6)
#define BIT_WR_DUAL_I			(0x00000001 << 5)
#define BIT_RD_QUAD_IO			(0x00000001 << 4)
#define BIT_RD_QUAD_O			(0x00000001 << 3)
#define BIT_RD_DUAL_IO			(0x00000001 << 2)
#define BIT_RD_DUAL_I			(0x00000001 << 1)
#define BIT_FRD_SINGEL			(0x00000001)
#define SPIC_VALID_CMD_MASK		(0x7fff)

#define DUAL_PRM_CYCLE_NUM		4
#define QUAD_PRM_CYCLE_NUM		2
/** @} */

/**
  * @}
  */

/**
  * @}
  */
/**
  * @}
  */

/* Other definations --------------------------------------------------------*/
extern FLASH_InitTypeDef flash_init_para;
extern u32 SPIC_CALIB_PATTERN[2];

typedef void (*FLASH_STRUCT_INIT_FUNC)(FLASH_InitTypeDef* FLASH_InitStruct);

#define SPIC_LOWSPEED_SAMPLE_PHASE	1
#endif  //_RTL8710B_SPI_FLASH_H
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
