/**
  ******************************************************************************
  * @file    rtl8721d_lcdc.h
  * @author
  * @version V1.0.0
  * @date    2017-08-07
  * @brief   This file contains all the functions prototypes for the LCDC firmware
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

#ifndef _RTL8721D_LCDC_H_
#define _RTL8721D_LCDC_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup LCDC
  * @{
  */


/** @addtogroup LCDC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * LCDC:
  *		- Base Address: LCDC (0x40070000)
  *		- Support RGB565 data format
  *		- SYS_CLK: 100MHz
  *		- Support 8-bit /16-bit MCU i8080 parallel interface
  *				Support resolution of 8-/16-bit mode can be (1024x1024) for still picture display
  *				Support resolution of 8-bit mode can be (645x645) for animate display when refresh rate is 30F/S
  *				Support resolution of 16-bit mode can be (912x912) for animate display when refresh rate is 30F/S
  *		- MCU I/F mode supports:
  *               			IO mode
  *               			Trigger DMA mode
  *               			MCU VSYNC mode
  *               			MCU TE mode
  *		- Support 6-/16-bit RGB parallel Interface
  *				Support resolution of 6-bit mode is less than (527x527) for animate display when refresh rate is 60F/S
  *				Support resolution of 16-bit mode is less than (912x912) for animate display when refresh rate is 60F/S
  *		- RGB I/F mode supports:
  *               			DE mode
  *               			HV mode
  *		-Support LED dot matrix display interface like 04/08/12/75
  *		-LED I/F mode supports:
  *               			single channel and single color mode
  *               			single channel and two colors mode
  *               			single channel and three colors mode
  *               			two channel and single color mode
  *               			two channel and two colors mode
  *               			two channel and three colors mode
  *
  *****************************************************************************************     
  * How to use IO mode in LCDC MCU I/F mode
  *****************************************************************************************
  *      To use the LCDC MCU I/F IO mode, the following steps are mandatory.
  *
  *      1. Enable LCDC peripheral clock.
  *
  *      2. Configure the LCDC pinmux.
  *        
  *      3. Use the following function to set default parameters. Change other parameters if needed, such as 8 bit 
  *          or 16 bit parallel interface, data/write/read/cs/rs pulse active level, write/read pulse width etc.
  *				LCDC_MCUStructInit(LCDC_MCUInitTypeDef* LCDC_MCUInitStruct);
  *          
  *      4. Initialize the hardware use step3 parameters.
  *				LCDC_MCUInit(LCDC_TypeDef* LCDCx, LCDC_MCUInitTypeDef* LCDC_MCUInitStruct);
  *
  *      5. Enable the LCDC using the function LCDC_Cmd().
  *
  *      6. Send command to LCM using the function LCDC_MCUIOWriteCmd() to initialize the LCD module.
  *
  *      7. write/read data to/from LCM using the function LCDC_MCUIOWriteData() and LCDC_MCUIOReadData() 
  *          and send command to LCM to drive LCD displaying.
  *
  *****************************************************************************************     
  * How to use trigger DMA mode in LCDC MCU I/F mode
  *****************************************************************************************
  *      To use the LCDC MCU I/F trigger DMA mode, the following steps are mandatory.
  *
  *      1. Enable LCDC peripheral clock.
  *
  *      2. Configure the LCDC pinmux.
  *        
  *      3. Configure LCDC working in MCU IO mode, send commands and parameters to LCM to initialize LCD.
  *
  *      4. Configure the LCDC_MCUInitStruct parameter corresponding to trigger DMA mode; change other parameters if needed.
  *				LCDC_MCUStructInit(LCDC_MCUInitTypeDef* LCDC_MCUInitStruct);
  *				LCDC_MCUInitStruct->LCDC_MCUMode = LCDC_MCU_DMA_MODE; 
  *				LCDC_MCUInitStruct->LCDC_MCUDMAMode = LCDC_TRIGGER_DMA_MODE;
  *
  *      5. Configure the LCDC_MCUInitStruct parameter corresponding to plane size.
  *
  *      6. Initialize the hardware use step4,5 parameters.
  *				LCDC_MCUInit(LCDC_TypeDef* LCDCx, LCDC_MCUInitTypeDef* LCDC_MCUInitStruct)
  *
  *      7. Configure the LCDC DMA parameters using the function LCDC_DMAModeConfig(), malloc image buffer and arrange
  *          image base address using the function LCDC_DMAImageBaseAddrConfig().
  *
  *      8. Enable LCDC interrupt if needed using the function LCDC_INTConfig().
  *
  *      9. Enable the LCDC using the function LCDC_Cmd().
  *
  *      10. Trigger one frame transfer using the function LCDC_MCUDMATrigger() and update the image buffer to change 
  *            the display effect.
  *
  *****************************************************************************************     
  * How to use VSYNC mode in LCDC MCU I/F mode
  *****************************************************************************************
  *      To use the LCDC MCU I/F VSYNC mode, the following steps are mandatory.
  *
  *      1. Enable LCDC peripheral clock.
  *
  *      2. Configure the LCDC pinmux.
  *        
  *      3. Configure LCDC working in MCU IO mode, send commands and parameters to LCM to let LCD working in VSYNC mode.
  *
  *      4. Configure the LCDC_MCUInitStruct parameter corresponding to VSYNC mode; change other parameters if needed, such as
  *          VSYNC pulse active level, VSYNC pulse width, VSYNC idle period.
  *				LCDC_MCUStructInit(LCDC_MCUInitTypeDef* LCDC_MCUInitStruct);
  *				LCDC_MCUInitStruct->LCDC_MCUMode = LCDC_MCU_DMA_MODE; 
  *				LCDC_MCUInitStruct->LCDC_MCUDMAMode = LCDC_AUTO_DMA_MODE;
  *				LCDC_MCUInitStruct->LCDC_MCUSyncMode = LCDC_MCU_SYNC_WITH_VSYNC;
  *
  *      5. Configure the LCDC_MCUInitStruct parameter corresponding to plane size.
  *
  *      6. Initialize the hardware use step4,5 parameters.
  *				LCDC_MCUInit(LCDC_TypeDef* LCDCx, LCDC_MCUInitTypeDef* LCDC_MCUInitStruct)
  *
  *      7. Configure the LCDC DMA parameters using the function LCDC_DMAModeConfig(), malloc image buffer and arrange
  *          image base address using the function LCDC_DMAImageBaseAddrConfig().
  *
  *      8. Enable the specified LCDC interrupt if needed using the function LCDC_INTConfig().
  *
  *      9. Enable the LCDC using the function LCDC_Cmd().
  *
  *      10. The LCDC can transfer image data to LCM automatically synchronized with the VSYNC signal and you can update the 
  *            image buffer to change the display effect.
  *
  *****************************************************************************************     
  * How to use TE mode in LCDC MCU I/F mode
  *****************************************************************************************
  *      To use the LCDC MCU I/F TE mode, the following steps are mandatory.
  *
  *      1. Enable LCDC peripheral clock.
  *
  *      2. Configure the LCDC pinmux.
  *
  *      3. Configure LCDC working in MCU IO mode, send commands and parameters to LCM to let LCD working in TE mode.
  *
  *      4. Configure the LCDC_MCUInitStruct parameter corresponding to VSYNC mode; change other parameters if needed, such as
  *          VSYNC pulse active level, VSYNC pulse width, VSYNC idle period etc.
  *				LCDC_MCUStructInit(LCDC_MCUInitTypeDef* LCDC_MCUInitStruct);
  *				LCDC_MCUInitStruct->LCDC_MCUMode = LCDC_MCU_DMA_MODE; 
  *				LCDC_MCUInitStruct->LCDC_MCUDMAMode = LCDC_AUTO_DMA_MODE;
  *				LCDC_MCUInitStruct->LCDC_MCUSyncMode = LCDC_MCU_SYNC_WITH_TE;
  *
  *      5. Configure the LCDC_MCUInitStruct parameter corresponding to plane size.
  *
  *      6. Initialize the hardware use step 4,5 parameters.
  *				LCDC_MCUInit(LCDC_TypeDef* LCDCx, LCDC_MCUInitTypeDef* LCDC_MCUInitStruct)
  *
  *      7. Configure the LCDC DMA parameters using the function LCDC_DMAModeConfig(), malloc image buffer and arrange
  *          image base address using the function LCDC_DMAImageBaseAddrConfig().
  *
  *      8. Enable the specified LCDC interrupt if needed using the function LCDC_INTConfig().
  *
  *      9. Enable the LCDC using the function LCDC_Cmd().
  *
  *      10. The LCDC can transfer image data to LCM automatically synchronized with the TE signal from LCM and you can update the 
  *            image buffer to change the display effect.
  *
  *****************************************************************************************     
  * How to use DE mode in LCDC RGB I/F mode
  *****************************************************************************************
  *      To use the LCDC RGB I/F DE mode, the following steps are mandatory.
  *
  *      1. Enable LCDC peripheral clock.
  *
  *      2. Configure the LCDC pinmux.
  *        
  *      3. Configure LCM parameters through SPI or other interfaces if needed.
  *
  *      4. Configure the LCDC_RGBInitStruct parameter corresponding to DE mode; change other parameters if needed, such as
  *          data pulse polarity, ENABLE signal active level, VSYNC pulse active level, HSYNC pulse active level, dot clock active edge,
  *          VFP, VBP, VSW, HBP, HFP, HSW, refresh frequency etc.
  *				LCDC_RGBStructInit(LCDC_RGBInitTypeDef *  LCDC_RGBInitStruct);
  *				LCDC_RGBInitStruct->LCDC_RGBSyncMode = LCDC_RGB_DE_MODE;	
  *
  *      5. Configure the LCDC_RGBInitStruct parameter corresponding to plane size.
  *
  *      6. Initialize the hardware use step 4,5 parameters.
  *				LCDC_RGBInit(LCDC_TypeDef* LCDCx, LCDC_RGBInitTypeDef* LCDC_RGBInitStruct)
  *
  *      7. Configure the LCDC DMA parameters: set burst size using the function LCDC_DMAModeConfig(), 
  *          set DMA FIFO under flow mode and error data using the functions LCDC_DMAUnderFlowModeConfig() and
  *          LCDC_DMAUnderFlowModeConfig(), malloc image buffer and arrange image base address using the function
  *          LCDC_DMAImageBaseAddrConfig().
  *
  *      8. Enable the specified LCDC interrupt if needed using the functions LCDC_INTConfig() and LCDC_LineINTPosConfig().
  *
  *      9. Enable the LCDC using the function LCDC_Cmd().
  *
  *      10. The LCDC can transfer image data to LCM automatically according to the refresh frequency and you can update the 
  *            image buffer to change the display effect.
  *
  *****************************************************************************************     
  * How to use HV mode in LCDC RGB I/F mode
  *****************************************************************************************
  *      To use the LCDC RGB I/F HV mode, the following steps are mandatory.
  *
  *      1. Enable LCDC peripheral clock.
  *
  *      2. Configure the LCDC pinmux.
  *        
  *      3. Configure LCM parameters through SPI or other interfaces if needed.
  *
  *      4. Configure the LCDC_RGBInitStruct parameter corresponding to DE mode; change other parameters if needed, such as
  *          data pulse polarity, VSYNC pulse active level, HSYNC pulse active level, dot clock active edge,
  *          VFP, VBP, VSW, HBP, HFP, HSW, 6 bit parallel I/F or 16 bit parallel I/F mode, refresh frequency.
  *				LCDC_RGBStructInit(LCDC_RGBInitTypeDef *  LCDC_RGBInitStruct);
  *
  *      5. Configure the LCDC_RGBInitStruct parameter corresponding to plane size.
  *
  *      6. Initialize the hardware use step 4,5 parameters.
  *				LCDC_RGBInit(LCDC_TypeDef* LCDCx, LCDC_RGBInitTypeDef* LCDC_RGBInitStruct)
  *
  *      7. Configure the LCDC DMA parameters: set burst size using the function LCDC_DMAModeConfig(), 
  *          set DMA FIFO under flow mode and error data using the functions LCDC_DMAUnderFlowModeConfig() and
  *          LCDC_DMAUnderFlowModeConfig(), malloc image buffer and arrange image base address using the function
  *          LCDC_DMAImageBaseAddrConfig().
  *
  *      8. Enable the specified LCDC interrupt if needed using the functions LCDC_INTConfig() and LCDC_LineINTPosConfig().
  *
  *      9. Enable the LCDC using the function LCDC_Cmd().
  *
  *      10. The LCDC can transfer image data to LCM automatically according to the refresh frequency and you can update the 
  *            image buffer to change the display effect.
  *
  *****************************************************************************************     
  * How to use LCDC LED I/F mode
  *****************************************************************************************
  *      To use the LCDC LED I/F mode, the following steps are mandatory.
  *
  *      1. Enable LCDC peripheral clock.
  *
  *      2. Configure the LCDC pinmux.
  *        
  *      3. Configure the LCDC_LEDInitStruct parameter corresponding to LED I/F mode; change other parameters if needed, such as
  *          color channel, color numbers, timing(latch start time, latch pulse width, OE active width), refresh frequency.
  *				LCDC_LEDStructInit(LCDC_LEDInitTypeDef *  LCDC_LEDInitStruct)
  *
  *      4. Configure the LCDC_LEDInitStruct parameter corresponding to plane size.
  *
  *      5. Initialize the hardware use step 3, 4 parameters.
  *				LCDC_LEDInit(LCDC_TypeDef* LCDCx, LCDC_LEDInitTypeDef* LCDC_LEDInitStruct)
  *
  *      6. Configure the LCDC DMA parameters: set burst size using the function LCDC_DMAModeConfig(), malloc image buffer 
  *          and arrange image base address using the function LCDC_DMAImageBaseAddrConfig().
  *
  *      7. Enable the specified LCDC interrupt if needed using the functions LCDC_INTConfig() and LCDC_LineINTPosConfig().
  *
  *      8. Enable the LCDC using the function LCDC_Cmd().
  *
  *      9. The LCDC can transfer image data to LED array board automatically according to the refresh frequency and you can update the 
  *            image buffer to change the display effect.
  *
  *****************************************************************************************     
  * @endverbatim
  */

/*===================================================================================================*/
#define LCDC_SYS_CLK					(100000000)

/*===================================================================================================*/



/* Exported Types --------------------------------------------------------*/

/** @defgroup LCDC_Exported_Types LCDC Exported Types
  * @{
  */

/** 
  * @brief  LCDC MCU I/F Mode Initialization structure definition
  */
typedef struct
{
	u32	LCDC_MCUIfMode; /*!< Specifies the LCDC MCU Interface Mode.
	                             This parameter can be a value of @ref LCDC_MCU_IF_MODE_define. */
	                             
	u32	LCDC_MCUMode;       /*!< Specifies the LCDC MCU Mode.
                                      This parameter can be a value of @ref LCDC_MCU_MODE_define. */

	u32	LCDC_MCUDMAMode;       /*!< Specifies the LCDC MCU DMA Mode.
                                      This parameter can be a value of @ref LCDC_DMA_MODE_define. */

	u32	LCDC_MCUImgHeight;       /*!< Specifies the height of the LCDC PlANE SIZE.
                                      This parameter can be a number between 0x000 and 0xfff. */

	u32	LCDC_MCUImgWidth;       /*!< Specifies the width of the LCDC PlANE SIZE.
                                      This parameter can be a number between 0x000 and 0xfff. */
									  
	u32	LCDC_MCUSyncMode;         /*!< Specifies the LCDC MCU synchronize mode.
                                      This parameter can be a value of @ref LCDC_MCU_SYNC_MODE_define. */
                                      
	u32	LCDC_MCUVsyncPulPlrty;           /*!< Specifies the LCDC MCU vsync pulse polarity.
                                      This parameter can be a value of @ref LCDC_MCU_VSYNC_PUL_POLARITY_define. */

	u32	LCDC_MCUTEPulPlrty;           /*!< Specifies the LCDC MCU TE pulse polarity.
                                      This parameter can be a value of @ref LCDC_MCU_TE_PUL_POLARITY_define.*/

	u32	LCDC_MCUTEDelay;           /*!< Specifies the LCDC MCU TE delay interval, This interval is from
								detected TE signal to starting frame transfer. For more details of this value,
								refer to the TEDELAY field in register LCDC_MCU_CFG in the LCDC Specification.
								This parameter can be a number between 5 and 65539. Unit: WR pulse width. */

	u32	LCDC_MCUDataPulPlrty;           /*!< Specifies the LCDC MCU data pulse polarity.
                                      This parameter can be a value of @ref LCDC_MCU_DATA_PUL_POLARITY_define. */

	u32	LCDC_MCURdPulPlrty;           /*!< Specifies the LCDC MCU read pulse polarity.
                                      This parameter can be a value of @ref LCDC_MCU_READ_PUL_POLARITY_define. */

	u32	LCDC_MCUWrPulPlrty;           /*!< Specifies the LCDC MCU write pulse polarity.
                                      This parameter can be a value of @ref LCDC_MCU_WRITE_PUL_POLARITY_define. */

	u32	LCDC_MCURsPulPlrty;           /*!< Specifies the LCDC MCU RS pulse polarity.
                                      This parameter can be a value of @ref LCDC_MCU_RS_PUL_POLARITY_define. */	

	u32	LCDC_MCUCsPulPlrty;           /*!< Specifies the LCDC MCU CS pulse polarity.
                                      This parameter can be a value of @ref LCDC_MCU_CS_PUL_POLARITY_define. */	

	u32	LCDC_MCUVsyncIdlePrd;           /*!< Specifies the LCDC MCU VSYNC idle polarity. Unit: Write Pulse width. 
                                      This parameter can be a number between 0x0005 and (0x100000-0x5). */
									  	
	u32	LCDC_MCUVsyncSigWidth;           /*!< Specifies the LCDC MCU VSYNC signal width. Unit: Write Pulse width. 
                                      This parameter can be a number between 0x01 and 0x100. */
									  	
	u32	LCDC_MCURdInacvWidth;           /*!< Specifies the LCDC MCU Read Inactive pulse width. Unit: SYS_Clk. 
                                      This parameter can be a number between 0x01 and 0x800. */	
									  	
	u32	LCDC_MCURdAcvWidth;           /*!< Specifies the LCDC MCU Read active pulse width. Unit: SYS_Clk. 
                                      This parameter can be a number between 0x01 and 0x800. */
									  	
	u32	LCDC_MCUWrPulWidth;           /*!< Specifies the LCDC MCU write active pulse width. Unit: SYS_Clk. 
                                      This parameter can be a even number between 0x02 and (0x400). */

	u32	LCDC_MCUIOTimeOutClkNum;           /*!< Specifies the LCDC MCU IO write/read timeout clock number. Unit: write clock. 
                                      This parameter can be a even number between 0x0 and 0xFFFF. */
} LCDC_MCUInitTypeDef;


/**
 * @brief LCDC RGB I/F Mode Initialization structure definition
 *
 */
typedef struct
{
	u32 LCDC_RGBIfMode;    /*!< Specifies the LCDC RGB Interface Mode.
                                                         This parameter can be  a value of @ref LCDC_RGB_IF_MODE_define */
														 	
	u32	LCDC_RGBImgHeight;       /*!< Specifies the height of the LCDC PlANE SIZE.
                                      This parameter can be a number between 0x000 and 0xfff. */

	u32	LCDC_RGBImgWidth;       /*!< Specifies the width of the LCDC PlANE SIZE.
                                      This parameter can be a number between 0x000 and 0xfff. */

	u32 LCDC_RGBRefreshFreq;    /*!< Specifies the RGB frame refresh frequency.
                                                          This parameter can be a number between 0x000 and 0xfff.*/
														 	
	u32 LCDC_RGBSyncMode;    /*!< Specifies the RGB frame synchronize Mode.
                                                         This parameter can be  a value of @ref LCDC_RGB_SYNC_MODE_define */
														 	
	u32 LCDC_RGBDatPulPlrty;    /*!< Specifies the RGB data pulse polariy.
                                                         This parameter can be  a value of @ref LCDC_RGB_DATA_PUL_POLARITY_define */

	u32 LCDC_RGBEnPulPlrty;    /*!< Specifies the RGB ENABLE pulse polariy.
                                                         This parameter can be  a value of @ref LCDC_RGB_ENABLE_PUL_POLARITY_define */	

	u32 LCDC_RGBHsPulPlrty;    /*!< Specifies the RGB HSYNC pulse polariy.
                                                         This parameter can be  a value of @ref LCDC_RGB_HSYNC_PUL_POLARITY_define */	

	u32 LCDC_RGBVsPulPlrty;    /*!< Specifies the RGB VSYNC pulse polariy.
                                                         This parameter can be  a value of @ref LCDC_RGB_VSYNC_PUL_POLARITY_define */

	u32 LCDC_RGBDclkActvEdge;    /*!< Specifies the RGB DCLK active edge.
                                                         This parameter can be  a value of @ref LCDC_RGB_DCLK_ACTIVE_EDGE_define */

	u32 LCDC_RGBVFP;    		/*!< Specifies the front porch line number.
                                                         This parameter can be a number between 0x1 and 0x10. Unit: line*/
														 	
	u32 LCDC_RGBVBP;    		/*!< Specifies the back porch line number.
                                                         This parameter can be a number between 0x1 and 0x10. Unit: line*/
														 	
	u32 LCDC_RGBVSW;    		/*!< Specifies the vertical signal width.
                                                         This parameter can be a number between 0x1 and 0x10. Unit: line*/	
														 	
	u32 LCDC_RGBHFP;    		/*!< Specifies the Horizontal front porch Unit: DCLK.
                                                         This parameter can be a number between 0x01 and 0x100. Unit: DCLK*/
														 	
	u32 LCDC_RGBHBP;    		/*!< Specifies the Horizontal back porch Unit: DCLK.
                                                         This parameter can be a number between 0x01 and 0x100. Unit: DCLK*/
														 	
	u32 LCDC_RGBHSW;    		/*!< Specifies the Horizontal sync signal width.
                                                         This parameter can be a number between 0x01 and 0x100. Unit: DCLK*/	
} LCDC_RGBInitTypeDef;

/**
 * @brief LCDC LED I/F Mode Initialization structure definition
 *
 */
typedef struct
{
	u32 LCDC_LEDIfMode;    	/*!< Specifies the LCDC LED Interface Mode.
                                                         This parameter can be  a value of @ref LCDC_LED_IF_MODE_define */

	u32	LCDC_LEDImgHeight;        /*!< Specifies the height of the LED PlANE SIZE.
                                      This parameter can be a number between 0x000 and 0xfff. */

	u32	LCDC_LEDImgWidth;       /*!< Specifies the width of the LED PlANE SIZE.
                                      This parameter can be a number between 0x000 and 0xfff. */

	u32 LCDC_LEDRefreshFreq;    /*!< Specifies the LED frame refresh frequency.
                                                          This parameter can be a number between 0x000 and 0xfff.*/

	u32 LCDC_LEDColorChnl;    /*!< Specifies the LED I/F color channel.
                                                         This parameter can be  a value of @ref LCDC_LED_COLOR_CHANNEL_define */
														 	
	u32 LCDC_LEDColorNum;    /*!< Specifies the LED I/F color number.
                                                         This parameter can be  a value of @ref LCDC_LED_COLOR_NUMBER_define */

	u32 LCDC_LEDClkPlrty;    	/*!< Specifies the LED I/F DCLK polarity.
                                                         This parameter can be  a value of @ref LCDC_LED_DCLK_POLARITY_define */
														 	
	u32 LCDC_LEDOePlrty;    	/*!< Specifies the LED I/F OE pulse polarity.
                                                         This parameter can be  a value of @ref LCDC_LED_OE_POLARITY_define */	

	u32 LCDC_LEDLatPlrty;    	/*!< Specifies the LED I/F Latch polarity.
                                                         This parameter can be  a value of @ref LCDC_LED_LATCH_POLARITY_define */	

	u32 LCDC_LEDDatPlrty;    	/*!< Specifies the LED I/F Data polarity.
                                                         This parameter can be  a value of @ref LCDC_LED_Data_POLARITY_define */	

	u32 LCDC_LEDOEActw;    	/*!< Specifies the LED I/F OE Active Width Time.
                                                         This parameter can be a number between 0x001 and 0x1000. Unit: DCLK */
														 	
	u32 LCDC_LEDLatchWidth;    /*!< Specifies the LED I/F LAT Width Time.
                                                         This parameter can be a number between 0x01 and 0x100. Unit: DCLK */

	u32 LCDC_LEDFrmIdlePd;    /*!< Specifies the LED I/F Frame dile period.
                                                         This parameter can be a number between 0x001 and 0x1000. Unit: DCLK */

	u32 LCDC_LEDLineIdlePd;    /*!< Specifies the LED I/F Line idle period.
                                                         This parameter can be a number between 0x01 and 0x100. Unit: DCLK */												 
}LCDC_LEDInitTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup LCDC_Exported_Constants LCDC Exported Constants
  * @{
  */

/** @defgroup LCDC_MCU_IF_MODE_define
  * @{
  */
#define LCDC_MCU_IF_8_BIT		((u32)0x00000000)
#define LCDC_MCU_IF_16_BIT		((u32)0x00010000)
#define IS_LCDC_MCU_IF_MODE(MODE) (((MODE) == LCDC_MCU_IF_8_BIT) || \
									((MODE) == LCDC_MCU_IF_16_BIT))

/**
  * @}
  */

/** @defgroup LCDC_MCU_MODE_define
  * @{
  */
#define LCDC_MCU_DMA_MODE		((u32)0x00000000)
#define LCDC_MCU_IO_MODE		((u32)0x00000800)
#define IS_LCDC_MCU_MODE(MODE) (((MODE) == LCDC_MCU_IO_MODE) || \
									((MODE) == LCDC_MCU_DMA_MODE))

/**
  * @}
  */

/** @defgroup LCDC_DMA_MODE_define
  * @{
  */
#define LCDC_AUTO_DMA_MODE		((u32)0x00000000)
#define LCDC_TRIGGER_DMA_MODE		((u32)0x00000001)
#define IS_LCDC_DMA_MODE(MODE) (((MODE) == LCDC_AUTO_DMA_MODE) || \
									((MODE) == LCDC_TRIGGER_DMA_MODE))

/**
  * @}
  */

/** @defgroup LCDC_MCU_SYNC_MODE_define
  * @{
  */
#define LCDC_MCU_SYNC_WITH_INTERNAL_CLK		((u32)0x00000000)
#define LCDC_MCU_SYNC_WITH_VSYNC				((u32)0x00000200)
#define LCDC_MCU_SYNC_WITH_TE					((u32)0x00000400)
#define IS_LCDC_MCU_SYNC_MODE(MODE) (((MODE) == LCDC_MCU_SYNC_WITH_INTERNAL_CLK) || \
									((MODE) == LCDC_MCU_SYNC_WITH_VSYNC) || \
									((MODE) == LCDC_MCU_SYNC_WITH_TE))

/**
  * @}
  */

/** @defgroup LCDC_MCU_VSYNC_PUL_POLARITY_define
  * @{
  */
#define LCDC_MCU_VSYNC_PUL_LOW_LEV_ACTIVE		((u32)0x00000000)
#define LCDC_MCU_VSYNC_PUL_HIGH_LEV_ACTIVE		((u32)0x00000040)
#define IS_LCDC_MCU_VSYNC_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_MCU_VSYNC_PUL_LOW_LEV_ACTIVE) || \
									((POLARITY) == LCDC_MCU_VSYNC_PUL_HIGH_LEV_ACTIVE))

/**
  * @}
  */

/** @defgroup LCDC_MCU_TE_PUL_POLARITY_define
  * @{
  */
#define LCDC_MCU_TE_PUL_LOW_LEV_ACTIVE		((u32)0x00000000)
#define LCDC_MCU_TE_PUL_HIGH_LEV_ACTIVE		((u32)0x00000020)
#define IS_LCDC_MCU_TE_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_MCU_TE_PUL_LOW_LEV_ACTIVE) || \
									((POLARITY) == LCDC_MCU_TE_PUL_HIGH_LEV_ACTIVE))

/**
  * @}
  */

/** @defgroup LCDC_MCU_DATA_PUL_POLARITY_define
  * @{
  */
#define LCDC_MCU_DAT_PUL_NORMAL		((u32)0x00000000)
#define LCDC_MCU_DAT_PUL_INVERT		((u32)0x00000010)
#define IS_LCDC_MCU_DAT_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_MCU_DAT_PUL_NORMAL) || \
									((POLARITY) == LCDC_MCU_DAT_PUL_INVERT))

/**
  * @}
  */

/** @defgroup LCDC_MCU_READ_PUL_POLARITY_define
  * @{
  */
#define LCDC_MCU_RD_PUL_RISING_EDGE_FETCH		((u32)0x00000000)
#define LCDC_MCU_RD_PUL_FALLING_EDGE_FETCH	((u32)0x00000008)
#define IS_LCDC_MCU_RD_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_MCU_RD_PUL_RISING_EDGE_FETCH) || \
									((POLARITY) == LCDC_MCU_RD_PUL_FALLING_EDGE_FETCH))

/**
  * @}
  */

/** @defgroup LCDC_MCU_WRITE_PUL_POLARITY_define
  * @{
  */
#define LCDC_MCU_WR_PUL_RISING_EDGE_FETCH	((u32)0x00000000)
#define LCDC_MCU_WR_PUL_FALLING_EDGE_FETCH	((u32)0x00000004)
#define IS_LCDC_MCU_WR_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_MCU_WR_PUL_RISING_EDGE_FETCH) || \
									((POLARITY) == LCDC_MCU_WR_PUL_FALLING_EDGE_FETCH))

/**
  * @}
  */

/** @defgroup LCDC_MCU_RS_PUL_POLARITY_define
  * @{
  */
#define LCDC_MCU_RS_PUL_LOW_LEV_CMD_ADDR		((u32)0x00000000)
#define LCDC_MCU_RS_PUL_HIGH_LEV_CMD_ADDR		((u32)0x00000002)
#define IS_LCDC_MCU_RS_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_MCU_RS_PUL_LOW_LEV_CMD_ADDR) || \
									((POLARITY) == LCDC_MCU_RS_PUL_HIGH_LEV_CMD_ADDR))

/**
  * @}
  */

/** @defgroup LCDC_MCU_CS_PUL_POLARITY_define
  * @{
  */
#define LCDC_MCU_CS_PUL_LOW_LEV_ACTIVE		((u32)0x00000000)
#define LCDC_MCU_CS_PUL_HIGH_LEV_ACTIVE		((u32)0x00000001)
#define IS_LCDC_MCU_CS_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_MCU_CS_PUL_LOW_LEV_ACTIVE) || \
									((POLARITY) == LCDC_MCU_CS_PUL_HIGH_LEV_ACTIVE))

/**
  * @}
  */


/** @defgroup LCDC_RGB_IF_MODE_define
  * @{
  */
#define LCDC_RGB_IF_6_BIT		((u32)0x00080000)
#define LCDC_RGB_IF_16_BIT		((u32)0x00090000)
#define IS_LCDC_RGB_IF_MODE(MODE) (((MODE) == LCDC_RGB_IF_6_BIT) || \
									((MODE) == LCDC_RGB_IF_16_BIT))

/**
  * @}
  */

/** @defgroup LCDC_RGB_SYNC_MODE_define
  * @{
  */
#define LCDC_RGB_DE_MODE		((u32)0x00000000)
#define LCDC_RGB_HV_MODE		((u32)0x02000000)
#define IS_LCDC_RGB_SYNC_MODE(MODE) (((MODE) == LCDC_RGB_DE_MODE) || \
									((MODE) == LCDC_RGB_HV_MODE))

/**
  * @}
  */

/** @defgroup LCDC_RGB_DATA_PUL_POLARITY_define
  * @{
  */
#define LCDC_RGB_DAT_PUL_NORMAL		((u32)0x00000000)
#define LCDC_RGB_DAT_PUL_INVERT		((u32)0x00100000)
#define IS_LCDC_RGB_DAT_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_RGB_DAT_PUL_NORMAL) || \
									((POLARITY) == LCDC_RGB_DAT_PUL_INVERT))

/**
  * @}
  */

/** @defgroup LCDC_RGB_ENABLE_PUL_POLARITY_define
  * @{
  */
#define LCDC_RGB_EN_PUL_LOW_LEV_ACTIVE	((u32)0x00000000)
#define LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE	((u32)0x00080000)
#define IS_LCDC_RGB_EN_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_RGB_EN_PUL_LOW_LEV_ACTIVE) || \
									((POLARITY) == LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE))

/**
  * @}
  */

/** @defgroup LCDC_RGB_HSYNC_PUL_POLARITY_define
  * @{
  */
#define LCDC_RGB_HS_PUL_LOW_LEV_SYNC	((u32)0x00000000)
#define LCDC_RGB_HS_PUL_HIGH_LEV_SYNC	((u32)0x00040000)
#define IS_LCDC_RGB_HS_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_RGB_HS_PUL_LOW_LEV_SYNC) || \
									((POLARITY) == LCDC_RGB_HS_PUL_HIGH_LEV_SYNC))

/**
  * @}
  */

/** @defgroup LCDC_RGB_VSYNC_PUL_POLARITY_define
  * @{
  */
#define LCDC_RGB_VS_PUL_LOW_LEV_SYNC	((u32)0x00000000)
#define LCDC_RGB_VS_PUL_HIGH_LEV_SYNC	((u32)0x00020000)
#define IS_LCDC_RGB_VS_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_RGB_VS_PUL_LOW_LEV_SYNC) || \
									((POLARITY) == LCDC_RGB_VS_PUL_HIGH_LEV_SYNC))

/**
  * @}
  */

/** @defgroup LCDC_RGB_DCLK_ACTIVE_EDGE_define
  * @{
  */
#define LCDC_RGB_DCLK_RISING_EDGE_FETCH	((u32)0x00000000)
#define LCDC_RGB_DCLK_FALLING_EDGE_FETCH	((u32)0x00010000)
#define IS_LCDC_RGB_DCLK_EDGE(EDGE) (((EDGE) == LCDC_RGB_DCLK_RISING_EDGE_FETCH) || \
									((EDGE) == LCDC_RGB_DCLK_FALLING_EDGE_FETCH))

/**
  * @}
  */

/** @defgroup LCDC_LED_IF_MODE_define
  * @{
  */
#define LCDC_LED_IF		((u32)0x000f0000)
#define IS_LCDC_LED_IF_MODE(MODE) (((MODE) == LCDC_LED_IF))

/**
  * @}
  */

/** @defgroup LCDC_LED_COLOR_CHANNEL_define
  * @{
  */
#define LCDC_LED_ONE_CHANNEL	((u32)0x00000000)
#define LCDC_LED_TWO_CHANNEL	((u32)0x00000004)
#define IS_LCDC_LED_CHANNEL(CHANNEL) (((CHANNEL) == LCDC_LED_ONE_CHANNEL) || \
									((CHANNEL) == LCDC_LED_TWO_CHANNEL))

/**
  * @}
  */

/** @defgroup LCDC_LED_COLOR_NUMBER_define
  * @{
  */
#define LCDC_LED_ONE_COLOR			((u32)0x00000000)
#define LCDC_LED_TWO_COLOR			((u32)0x00000001)
#define LCDC_LED_THREE_COLOR		((u32)0x00000002)
#define IS_LCDC_LED_COLOR_NUM(NUM) (((NUM) == LCDC_LED_ONE_COLOR) || \
									((NUM) == LCDC_LED_TWO_COLOR) || \
									((NUM) == LCDC_LED_THREE_COLOR))
/**
  * @}
  */


/** @defgroup LCDC_LED_DCLK_POLARITY_define
  * @{
  */
#define LCDC_LED_DCLK_RISING_EDGE_FETCH	((u32)0x00000000)
#define LCDC_LED_DCLK_FALLING_EDGE_FETCH	((u32)0x00000010)
#define IS_LCDC_LED_DCLK_EDGE(EDGE) (((EDGE) == LCDC_LED_DCLK_RISING_EDGE_FETCH) || \
									((EDGE) == LCDC_LED_DCLK_FALLING_EDGE_FETCH))

/**
  * @}
  */

/** @defgroup LCDC_LED_LATCH_POLARITY_define
  * @{
  */
#define LCDC_LED_LACTH_RISING_EDGE_FETCH	((u32)0x00000000)
#define LCDC_LED_LATCH_FALLING_EDGE_FETCH	((u32)0x00000020)
#define IS_LCDC_LED_LATCH_EDGE(EDGE) (((EDGE) == LCDC_LED_LACTH_RISING_EDGE_FETCH) || \
									((EDGE) == LCDC_LED_LATCH_FALLING_EDGE_FETCH))

/**
  * @}
  */

/** @defgroup LCDC_LED_OE_POLARITY_define
  * @{
  */
#define LCDC_LED_OE_LOW_LEV_ACTIVE		((u32)0x00000000)
#define LCDC_LED_OE_HIGH_LEV_ACTIVE	((u32)0x00000040)
#define IS_LCDC_LED_OE_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_LED_OE_LOW_LEV_ACTIVE) || \
									((POLARITY) == LCDC_LED_OE_HIGH_LEV_ACTIVE))

/**
  * @}
  */

/** @defgroup LCDC_LED_DATA_POLARITY_define
  * @{
  */
#define LCDC_LED_DAT_PUL_NORMAL		((u32)0x00000000)
#define LCDC_LED_DAT_PUL_INVERT			((u32)0x00000080)
#define IS_LCDC_LED_DAT_PUL_POLARITY(POLARITY) (((POLARITY) == LCDC_LED_DAT_PUL_NORMAL) || \
									((POLARITY) == LCDC_LED_DAT_PUL_INVERT))

/**
  * @}
  */

/** @defgroup LCDC_DMA_UNDERFLOW_MODE_define
  * @{
  */
#define LCDC_DMAUNFW_OUTPUT_LASTDATA		((u32)0x00000000)
#define LCDC_DMAUNFW_OUTPUT_ERRORDATA	((u32)0x02000000)
#define IS_LCDC_DMA_DMA_UNDERFLOW_MODE(MODE) (((MODE) == LCDC_DMAUNFW_OUTPUT_LASTDATA) || \
									((MODE) == LCDC_DMAUNFW_OUTPUT_ERRORDATA))

/**
  * @}
  */

/** @defgroup LCDC_INTERRUPTS_define
  * @{
  */
#define LCDC_IT_FRM_START		((u32)0x00000020)
#define LCDC_IT_IO_TIMEOUT		((u32)0x00000010)
#define LCDC_IT_LINE				((u32)0x00000008)
#define LCDC_IT_FRDN				((u32)0x00000004)
#define LCDC_IT_DMAUNDFW		((u32)0x00000001)
#define IS_LCDC_CONFIG_IT(IT) (((IT) == LCDC_IT_DMAUNDFW) || \
								((IT) == LCDC_IT_FRDN) || \
									((IT) == LCDC_IT_LINE)|| \
									((IT) == LCDC_IT_IO_TIMEOUT)|| \
									((IT) == LCDC_IT_FRM_START))

#define IS_LCDC_CLEAR_IT(IT) ((((IT) & (u32)0xFFFFFFC2) == 0x00) && ((IT) != 0x00))

/**
  * @}
  */

/** @defgroup LCDC_type_define
  * @{
  */

#define IS_LCDC_ALL_PERIPH(PERIPH)   (PERIPH == LCDC)

/**
  * @}
  */


/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/** @defgroup LCDC_Exported_Functions LCDC Exported Functions
  * @{
  */
/** @defgroup LCDC_MCU_Interface_functions 
  * @{
  */

_LONG_CALL_ void LCDC_MCUStructInit(LCDC_MCUInitTypeDef* LCDC_MCUInitStruct);
_LONG_CALL_ void LCDC_MCUInit(LCDC_TypeDef* LCDCx, LCDC_MCUInitTypeDef* LCDC_MCUInitStruct);
_LONG_CALL_ void LCDC_MCUDMATrigger(LCDC_TypeDef* LCDCx);
_LONG_CALL_ u32 LCDC_MCUGetRunStatus(LCDC_TypeDef* LCDCx);
_LONG_CALL_ void LCDC_MCUIOWriteData(LCDC_TypeDef* LCDCx, u16 Data);
_LONG_CALL_ u16 LCDC_MCUIOReadData(LCDC_TypeDef* LCDCx);
_LONG_CALL_ void LCDC_MCUIOWriteCmd(LCDC_TypeDef* LCDCx, u16 Cmd);
/**
  * @}
  */ 

/** @defgroup LCDC_RGB_Interface_functions 
  * @{
  */
_LONG_CALL_ void LCDC_RGBStructInit(LCDC_RGBInitTypeDef *  LCDC_RGBInitStruct);
_LONG_CALL_ void LCDC_RGBInit(LCDC_TypeDef* LCDCx, LCDC_RGBInitTypeDef* LCDC_RGBInitStruct);
_LONG_CALL_ void LCDC_RGBGetSyncStatus(LCDC_TypeDef* LCDCx, u32 * pHSStatus, u32 * pVSStatus);
/**
  * @}
  */ 

/** @defgroup LCDC_LED_Interface_functions 
  * @{
  */
_LONG_CALL_ void LCDC_LEDStructInit(LCDC_LEDInitTypeDef *  LCDC_LEDInitStruct);
_LONG_CALL_ void LCDC_LEDInit(LCDC_TypeDef* LCDCx, LCDC_LEDInitTypeDef* LCDC_LEDInitStruct);
_LONG_CALL_ void LCDC_LEDSetOEWidth(LCDC_TypeDef* LCDCx, u32 OEActvWidth);

/**
  * @}
  */

/** @defgroup LCDC_DMA_Configure_functions 
  * @{
  */
_LONG_CALL_ void LCDC_DMAModeConfig(LCDC_TypeDef* LCDCx, u32 BurstSize);
_LONG_CALL_ void LCDC_DMAUnderFlowConfig(LCDC_TypeDef* LCDCx, u32 DmaUnFlwMode, u32 ErrorData);
_LONG_CALL_ void LCDC_DMAImageBaseAddrConfig(LCDC_TypeDef* LCDCx, u32 ImgBaseAddr);

/**
  * @}
  */ 

/** @defgroup LCDC_Interrupt_functions 
  * @{
  */
_LONG_CALL_ void LCDC_INTConfig(LCDC_TypeDef* LCDCx, u32 LCDC_IT, u32 NewState);
_LONG_CALL_ void LCDC_LineINTPosConfig(LCDC_TypeDef* LCDCx, u32 LineNum);
_LONG_CALL_ u32 LCDC_GetINTStatus(LCDC_TypeDef* LCDCx);
_LONG_CALL_ u32 LCDC_GetRawINTStatus(LCDC_TypeDef* LCDCx);
_LONG_CALL_ void LCDC_ClearAllINT(LCDC_TypeDef* LCDCx);
_LONG_CALL_ void LCDC_ClearINT(LCDC_TypeDef* LCDCx, u32 LCDC_IT);
_LONG_CALL_ void LCDC_GetCurPosStatus(LCDC_TypeDef* LCDCx, u32* pCurPosX, u32* pCurPosY);
_LONG_CALL_ void LCDC_GetDmaUnINTCnt(LCDC_TypeDef* LCDCx, u32* DmaUnIntCnt);

/**
  * @}
  */

/** @defgroup LCDC_global_functions 
  * @{
  */
_LONG_CALL_ void LCDC_Cmd(LCDC_TypeDef* LCDCx, u32 NewState);
_LONG_CALL_ void LCDC_InsDisable(LCDC_TypeDef* LCDCx);
_LONG_CALL_ void LCDC_DeInit(LCDC_TypeDef* LCDCx);
/**
  * @}
  */


/**
  * @}
  */


/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup LCDC_Register_Definitions LCDC Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup LCDC_CTRL
 * @{
 *****************************************************************************/
#define LCDC_CTRL_LCDCEN				((u32)0x00000001)			/*BIT[0], Enable LCDC*/
#define LCDC_CTRL_LCDCDIS				((u32)0x00000001<<1)		/*BIT[1], Disable LCDC*/
#define LCDC_CTRL_LCDCINSDIS			((u32)0x00000001<<2)		/*BIT[2], Disable LCDC instantly*/
#define LCDC_CTRL_IF_MODE				((u32)0x000F0000)			/*LCDC_CTRL[19:16], bit field for LCDC Interface Mode*/
#define LCDC_CTRL_8_BIT_MCU				((u32)0x00000000)			/*LCDC_CTRL[19:16]=0000B, 8 bit MCU I/F*/
#define LCDC_CTRL_16_BIT_MCU			((u32)0x00010000)			/*LCDC_CTRL[19:16]=0001B, 16 bit MCU I/F*/
#define LCDC_CTRL_6_BIT_RGB				((u32)0x00080000)			/*LCDC_CTRL[19:16]=1000B, 6 bit RGB I/F*/
#define LCDC_CTRL_16_BIT_RGB			((u32)0x00090000)			 /*LCDC_CTRL[19:16]=1001B, 16 bit RGB I/F*/
#define LCDC_CTRL_LED_IF					((u32)0x000F0000)			/*LCDC_CTRL[19:16]=1111B, LED I/F*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_PLANE_SIZE
 * @{
 *****************************************************************************/
#define LCDC_PLANE_SIZE_IMG_HEIGHT		((u32)0x0FFF0000)			/*Bit[27:16], bits for image height setting*/
#define LCDC_PLANE_SIZE_IMG_WIDTH		((u32)0x00000FFF)			/*Bit[11:0],   bits for image width setting*/
/** @} */

/**************************************************************************//**
 * @defgroup LCDC_UNDFLW_CFG
 * @{
 *****************************************************************************/
#define LCDC_UNDFLW_CFG_UNDMODE		((u32)0x00000001<<25)	      	/*Bit[25], output mode when DMA FIFO under flow happens*/
#define LCDC_UNDFLW_CFG_ERROUTDATA	((u32)0x0000FFFF)			/*Bit[15:0], bits for DMA under flow error data field*/
/** @} */

/**************************************************************************//**
 * @defgroup LCDC_DMA_MODE_CFG
 * @{
 *****************************************************************************/								
#define LCDC_DMA_MODE_CFG_TRIGGER_MODE		((u32)0x00000001) 		/*Bit[0], trigger mode or auto mode selection*/
#define LCDC_DMA_MODE_CFG_TRIGGER_ONE_TIME	((u32)0x00000001<<1) 	/*Bit[1], trigger DMA to transfer one frame*/
#define LCDC_DMA_MODE_CFG_RDOTST				((u32)0x00000007<<2) 	/*Bit[4:2], bits for DMA burst size configuration*/
#define LCDC_DMA_MODE_CFG_DMAINTV				((u32)0xFFFFFF00) 		/*Bit[31:8], The interval cycle count between two DMA requests. 
																	    Unit: bus clock cycle.( for debug)*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_CLK_DIV
 * @{
 *****************************************************************************/
#define LCDC_CLK_DIV_DCLKDIV			((u32)0x0000FFFF)       			/*Bit[15:0], bits for DCLK clock divider*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_IRQ_EN
 * @{
 *****************************************************************************/
#define LCDC_IRQ_EN_DMAUNINTEN			((u32)0x00000001)	     		/*Bit[0], DMA FIFO underflow interrupt enable*/
#define LCDC_IRQ_EN_LCDFRDINTEN		((u32)0x00000001<<2)	     	/*Bit[2], LCD/LED refresh done interrupt enable*/
#define LCDC_IRQ_EN_LCDLINEINTEN		((u32)0x00000001<<3)	     	/*Bit[3], LCD/LED line interrupt enable*/
#define LCDC_IRQ_EN_IOTIMEOUTEN		((u32)0x00000001<<4)	     	/*Bit[4], IO write/read timeout interrupt enable*/
#define LCDC_IRQ_EN_FRMSTARTEN			((u32)0x00000001<<5)	     	/*Bit[5], Frame start interrupt enable*/
/** @} */

/**************************************************************************//**
 * @defgroup LCDC_IRQ_STATUS
 * @{
 *****************************************************************************/
#define LCDC_IRQ_STATUS_DMAUNINTS			((u32)0x00000001)			/*Bit[0], DMA FIFO underflow interrupt status,Write 1 clear*/
#define LCDC_IRQ_STATUS_LCDFRDINTS			((u32)0x00000001<<2)		/*Bit[2], LCD refresh frame done interrupt status,Write 1 clear*/
#define LCDC_IRQ_STATUS_LINEINTS			((u32)0x00000001<<3)		/*Bit[3], Line Interrupt status,Write 1 clear*/
#define LCDC_IRQ_STATUS_IOTIMEOUTINTS		((u32)0x00000001<<4)		/*Bit[4], IO write/read timeout Interrupt status,Write 1 clear*/
#define LCDC_IRQ_STATUS_FRMSTARTINTS		((u32)0x00000001<<5)		/*Bit[5], Frame start Interrupt status,Write 1 clear*/
/** @} */

/**************************************************************************//**
 * @defgroup LCDC_IRQ_RAW
 * @{
 *****************************************************************************/
#define LCDC_IRQ_RAW_DMAUNINTRS		((u32)0x00000001)			/*Bit[0], DMA FIFO underflow interrupt raw status*/
#define LCDC_IRQ_RAW_LCDFRDINTS		((u32)0x00000001<<2)		/*Bit[2], LCD refresh frame done interrupt raw status*/
#define LCDC_IRQ_RAW_LINEINTS			((u32)0x00000001<<3)		/*Bit[3], Line Interrupt raw status*/
#define LCDC_IRQ_RAW_IOTIMEOUTINTS		((u32)0x00000001<<4)		/*Bit[4], IO write/read timeout Interrupt raw status*/
#define LCDC_IRQ_RAW_FRMSTARTINTS		((u32)0x00000001<<5)		/*Bit[5], frame start Interrupt raw status*/
/** @} */

/**************************************************************************//**
 * @defgroup LCDC_LINE_INT_POS
 * @{
 *****************************************************************************/
#define LCDC_LINE_INTPOS				((u32)0x00000FFF)			/*Bit[11:0], Bits for Line Interrupt Position*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_CUR_POS_STATUS
 * @{
 *****************************************************************************/
#define LCDC_CUR_POS_STS_X				((u32)0x00000FFF)			/*BIT[11:0], current X position*/
#define LCDC_CUR_POS_STS_Y				((u32)0x00000FFF<<16)		/*BIT[27:16], current Y position*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_STATUS
 * @{
 *****************************************************************************/
#define LCDC_STATUS_DMAUNINTCNT		((u32)0x0000FFFF)			/*Bit[15:0], DMA FIFO underflow interrupt count*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_RGB_CFG
 * @{
 *****************************************************************************/
#define LCDC_RGB_CFG_DCLKPL			((u32)0x00000001<<16)     		/*Bit[16], The polarity of the DCLK active edge*/
#define LCDC_RGB_CFG_VSPL				((u32)0x00000001<<17)     		/*Bit[17], The VSYNC pulse polarity*/
#define LCDC_RGB_CFG_HSPL				((u32)0x00000001<<18)     		/*Bit[18], The HSYNC pulse polarity*/
#define LCDC_RGB_CFG_ENPL				((u32)0x00000001<<19)     		/*Bit[19], The ENABLE pulse polarity*/
#define LCDC_RGB_CFG_DATPL				((u32)0x00000001<<20)     		/*Bit[20], The Data pulse polarity*/
#define LCDC_RGB_CFG_RGBIFUPDATE		((u32)0x00000001<<24)     		/*Bit[24], Force HW updates RGB I/F parameters 
																	    after current LCD refresh frame done. CPU writes 1 
																	    to force HW updating parameters. After updating, 
																	    HW cleared this bit.(ex. DMA Address update should 
																	    take care of this bit)*/
#define LCDC_RGB_CFG_RGB_SYNC_MODE	((u32)0x000000003<<25)     		/*Bit[26:25], bit field for RGB sync mode*/
#define LCDC_RGB_CFG_RGB_DE_MODE		((u32)0x000000000)     			/*Bit[26:25] = 00B, DE mode,frame synchronized with ENABLE signal*/
#define LCDC_RGB_CFG_RGB_HV_MODE		((u32)0x000000001<<25)     		/*Bit[26:25] = 01B, HV mode,frame synchronized with sync signal*/


/** @} */

/**************************************************************************//**
 * @defgroup LCDC_RGB_VSYNC_CFG
 * @{
 *****************************************************************************/
#define LCDC_RGB_VSYNC_CFG_VSW		((u32)0x0000000F)				/*Bit[3:0], Vertical sync signal width*/
#define LCDC_RGB_VSYNC_CFG_VBP			((u32)0x0000000F<<8)			/*Bit[11:8], Back porch line number*/
#define LCDC_RGB_VSYNC_CFG_VFP			((u32)0x0000000F<<16)			/*Bit[19:16], Front porch line number*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_RGB_HSYNC_CFG
 * @{
 *****************************************************************************/
#define LCDC_RGB_HSYNC_CFG_HSW		((u32)0x000000FF)     	   		/*Bit[7:0], Horizontal sync signal width*/
#define LCDC_RGB_HSYNC_CFG_HBP		((u32)0x000000FF<<8)			/*Bit[15:8], Horizontal back porch*/
#define LCDC_RGB_HSYNC_CFG_HFP			((u32)0x000000FF<<16)			/*Bit[23:16], Horizontal front porch*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_RGB_SYNC_STATUS
 * @{
 *****************************************************************************/
#define LCDC_RGB_SYNC_STATUS_VSSTATUS				((u32)0x00000003)     	   /*Bit[1:0], bit field for VSYNC Status*/
#define LCDC_RGB_SYNC_STATUS_VSYNC				((u32)0x00000000)     	   /*Bit[1:0]=00B HSYNC*/
#define LCDC_RGB_SYNC_STATUS_VFP					((u32)0x00000001)     	   /*Bit[1:0]=01B, HFP*/
#define LCDC_RGB_SYNC_STATUS_VACTIVE				((u32)0x00000002)     	   /*Bit[1:0]=10B, ACTIVE*/
#define LCDC_RGB_SYNC_STATUS_VBP					((u32)0x00000003)     	   /*Bit[1:0]=11B, HBP*/

#define LCDC_RGB_SYNC_STATUS_HSSTATUS				((u32)0x00000003<<2)		/*Bit[3:2], bit field for HSYNC Status*/
#define LCDC_RGB_SYNC_STATUS_HSYNC				((u32)0x00000000<<2)	 	/*Bit[3:2]=00B HSYNC*/
#define LCDC_RGB_SYNC_STATUS_HFP					((u32)0x00000001<<2)		/*Bit[3:2]=01B, HFP*/
#define LCDC_RGB_SYNC_STATUS_HACTIVE				((u32)0x00000002<<2)		/*Bit[3:2]=10B, ACTIVE*/
#define LCDC_RGB_SYNC_STATUS_HBP					((u32)0x00000003<<2)		/*Bit[3:2]=11B, HBP*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_MCU_CFG
 * @{
 *****************************************************************************/
#define LCDC_MCU_CFG_CSPL							((u32)0x00000001)     	   	/*Bit[0], CS pulse polarity*/
#define LCDC_MCU_CFG_RSPL							((u32)0x00000001<<1)    		/*Bit[1], RS pulse polarity*/
#define LCDC_MCU_CFG_WRPL							((u32)0x00000001<<2)    		/*Bit[2], WR pulse polarity*/
#define LCDC_MCU_CFG_RDPL							((u32)0x00000001<<3)    		/*Bit[3], RD pulse polarity*/
#define LCDC_MCU_CFG_DATAPL						((u32)0x00000001<<4)    		/*Bit[4], DATA pulse polarity*/
#define LCDC_MCU_CFG_TEPL							((u32)0x00000001<<5)    		/*Bit[5], TE pulse polarity*/
#define LCDC_MCU_CFG_MCUSYPL						((u32)0x00000001<<6)    		/*Bit[6], MCU VSYNC pulse polarity*/
#define LCDC_MCU_CFG_MCUIFUPDATE					((u32)0x00000001<<8)    		/*Bit[8], Force HW updates MCU I/F Timing 
																			    shadow register at specific timing*/
#define LCDC_MCU_CFG_MCUSYNCMODE					((u32)0x00000003<<9)    		/*Bit[10:9], bit field for MCU SYNC mode setting*/
#define LCDC_MCU_CFG_SYNC_INTERNAL_CLK			((u32)0x00000000<<9)    		/*Bit[10:9]=00B, synchronized with the internal clock*/
#define LCDC_MCU_CFG_SYNC_VSYNC_INPUT				((u32)0x00000001<<9)    		/*Bit[10:9]=01B, synchronized with the VSYNC INPUT*/
#define LCDC_MCU_CFG_SYNC_TE						((u32)0x00000002<<9)    		/*Bit[10:9]=10B, synchronized with the TE*/

#define LCDC_MCU_CFG_IOMODE_EN					((u32)0x00000001<<11)    	/*Bit[11], MCU I/F mode enable*/
#define LCDC_MCU_CFG_IOMODE_RUN					((u32)0x00000001<<12)    	/*Bit[12], MCU I/F mode run*/
#define LCDC_MCU_CFG_IOMODE_TEDELAY				((u32)0x0000FFFF<<16)    	/*Bit[31:16], TE delay interval -5*/


/** @} */

/**************************************************************************//**
 * @defgroup LCDC_MCU_VSYNC_CFG
 * @{
 *****************************************************************************/
#define LCDC_MCU_VSYNC_CFG_MCUVSW				((u32)0x000000FF)     	   	/*Bit[7:0], VSYNC signal widdth - 1*/
#define LCDC_MCU_VSYNC_CFG_MCUVSPD				((u32)0x0000FFFF<<12)  	   	/*Bit[31:12], VSYNC idle period*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_MCU_TIMING_CFG
 * @{
 *****************************************************************************/
#define LCDC_MCU_TIMING_CFG_WRPULW				((u32)0x000003FF)     	   	/*Bit[9:0], write pulse width */
#define LCDC_MCU_TIMING_CFG_RDACTW				((u32)0x000007FF<<10)     	/*Bit[20:10], RD active pulse width -1*/
#define LCDC_MCU_TIMING_CFG_RDINACTW				((u32)0x000007FF<<21)     	/*Bit[31:21], RD inactive pulse width -1*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_MCU_IO_DATA
 * @{
 *****************************************************************************/
#define LCDC_MCU_IO_WR_CMD_FG						((u32)0x80000000)     		/*Bit[31], bit field for writing command control*/
#define LCDC_MCU_IO_WR_DAT_FG						((u32)0x7FFFFFFF)     		/*For writing data mask*/
#define LCDC_MCU_IO_RW_DATA						((u32)0x0000FFFF)     	   	/*Bit[15:0], bit field for write/read data*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_MCU_IO_TO_CFG
 * @{
 *****************************************************************************/
#define LCDC_MCU_IO_TIMEOUT_CLK_NUM				((u32)0x0000FFFF)     		/*Bit[15:0], bit field for write/read timeout clock number, unit: write clock*/

/** @} */


/**************************************************************************//**
 * @defgroup LCDC_LED_CFG
 * @{
 *****************************************************************************/
#define LCDC_LED_CFG_COLORNUM						((u32)0x00000003)     	   	/*Bit[1:0], bit field for Color number in frame buffer*/
#define LCDC_LED_CFG_ONECOLOR						((u32)0x00000000)     	   	/*Bit[1:0]=00B, one color*/
#define LCDC_LED_CFG_TWOCOLOR						((u32)0x00000001)     	   	/*Bit[1:0]=01B, two color*/
#define LCDC_LED_CFG_THREECOLOR					((u32)0x00000002)     	   	/*Bit[1:0]=10B, three color*/

#define LCDC_LED_CFG_COLORCHAN						((u32)0x00000001<<2)     	/*Bit[2], color channel*/

#define LCDC_LED_CFG_LEDIFUPDATE					((u32)0x00000001<<3)     	/*Bit[3], Force HW updates LED I/F parameters after current LED refresh frame done.*/

#define LCDC_LED_CFG_CLKPL							((u32)0x00000001<<4)     	/*Bit[4], The polarity of the DCLK active edge*/
#define LCDC_LED_CFG_LATPL							((u32)0x00000001<<5)     	/*Bit[5], The polarity of the LATCH active edge.*/
#define LCDC_LED_CFG_OEPL							((u32)0x00000001<<6)     	/*Bit[6], The OE pulse polarity.*/
#define LCDC_LED_CFG_DATPL							((u32)0x00000001<<7)     	/*Bit[7], The Data pulse polarity..*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_LED_TIMING
 * @{
 *****************************************************************************/
#define LCDC_LED_TIMING_LATW						((u32)0x000000FF)     	   	/*Bit[7:0], bit field for LAT Width Time - 1. Unit: dotclock*/
#define LCDC_LED_TIMING_OEACTW						((u32)0x0000FFFF<<8)     	/*Bit[23:8], bit field for OE Active Width Time - 1. Unit: dotclock*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_LED_IDLE
 * @{
 *****************************************************************************/
#define LCDC_LED_TIMING_LINEIDLE					((u32)0x000000FF)     	   	/*Bit[7:0], bit field for line idle Width Time - 1. Unit: dotclock*/
#define LCDC_LED_TIMING_FRMIDLEPD					((u32)0x00000FFF<<20)     	/*Bit[31:20], bit field for frame idle period interval - 1. Unit: dotclock*/

/** @} */

/**************************************************************************//**
 * @defgroup LCDC_IMG_BASE_ADDR
 * @{
 *****************************************************************************/
#define LCDC_IMG_BASE_ADDR_MASK					((u32)0xFFFFFFFF)     	   	/*Bit[31:0], Image DMA source address.*/

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
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
