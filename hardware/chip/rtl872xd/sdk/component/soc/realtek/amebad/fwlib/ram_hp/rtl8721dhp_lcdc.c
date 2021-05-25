/**
  ******************************************************************************
  * @file    rtl8721dhp_lcdc.c
  * @author
  * @version V1.0.0
  * @date    2017-08-14
  * @brief   This file contains all the functions prototypes for the LCDC firmware
  *             library, including the following functionalities of LCD controller:

  *           - MCU I/F Initialization
  *           - TE mode configuration in MCU I/F mode
  *           - VSYNC mode configuration in MCU I/F mode 
  *           - trigger DMA mode configuration in MCU I/F mode
  *           - write/read data through MCU I/F in IO mode
  *           - write command through MCU I/F in IO mode
  *           - RGB I/F Initialization
  *           - RGB I/F configuration for DE mode 
  *           - RGB I/F configuration for HV mode
  *           - Synchronization status acquisition in RGB I/F mode
  *           - LED I/F Initialization
  *           - Color channel and color numbers configuration in LED I/F mode
  *           - Timing configuration in LED I/F mode
  *           - OE active width configuration in LED I/F mode
  *           - LCDC DMA configurations
  *           - Interrupts and flags management
  *
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
#include "rtl8721d_lcdc.h"

/*-----------------------------------------------MCU I/F functions----------------------------------------------------*/
/**
  * @brief  Fills each LCDC_MCUInitStruct member with its default value.
  * @param  LCDC_MCUInitStruct: pointer to an LCDC_MCUInitTypeDef structure which will be initialized.
  * @retval   None
  */
void LCDC_MCUStructInit(LCDC_MCUInitTypeDef* LCDC_MCUInitStruct)
{
	/* LCDC_MCUInitStruct members default value */
	LCDC_MCUInitStruct->LCDC_MCUIfMode = LCDC_MCU_IF_16_BIT;
	LCDC_MCUInitStruct->LCDC_MCUMode = LCDC_MCU_IO_MODE; 
	LCDC_MCUInitStruct->LCDC_MCUDMAMode = LCDC_AUTO_DMA_MODE;
	LCDC_MCUInitStruct->LCDC_MCUImgHeight = 320; 
	LCDC_MCUInitStruct->LCDC_MCUImgWidth = 240;
	LCDC_MCUInitStruct->LCDC_MCUSyncMode = LCDC_MCU_SYNC_WITH_INTERNAL_CLK;                               
	LCDC_MCUInitStruct->LCDC_MCUVsyncPulPlrty = LCDC_MCU_VSYNC_PUL_LOW_LEV_ACTIVE;
	LCDC_MCUInitStruct->LCDC_MCUTEPulPlrty = LCDC_MCU_TE_PUL_HIGH_LEV_ACTIVE;
	LCDC_MCUInitStruct->LCDC_MCUTEDelay = 5;
	LCDC_MCUInitStruct->LCDC_MCUDataPulPlrty = LCDC_MCU_DAT_PUL_NORMAL;
	LCDC_MCUInitStruct->LCDC_MCURdPulPlrty = LCDC_MCU_RD_PUL_RISING_EDGE_FETCH;
	LCDC_MCUInitStruct->LCDC_MCUWrPulPlrty = LCDC_MCU_WR_PUL_RISING_EDGE_FETCH;
	LCDC_MCUInitStruct->LCDC_MCURsPulPlrty = LCDC_MCU_RS_PUL_LOW_LEV_CMD_ADDR;
	LCDC_MCUInitStruct->LCDC_MCUCsPulPlrty = LCDC_MCU_CS_PUL_LOW_LEV_ACTIVE;
	LCDC_MCUInitStruct->LCDC_MCUVsyncIdlePrd = 128;						  	
	LCDC_MCUInitStruct->LCDC_MCUVsyncSigWidth = 1;
	LCDC_MCUInitStruct->LCDC_MCURdInacvWidth = 2;					  	
	LCDC_MCUInitStruct->LCDC_MCURdAcvWidth = 2;		  	
	LCDC_MCUInitStruct->LCDC_MCUWrPulWidth = 4;
	LCDC_MCUInitStruct->LCDC_MCUIOTimeOutClkNum = 32;
}

/**
  * @brief    Initializes the LCDC peripheral according to the specified
  *               parameters in the LCDC_MCUInitStruct.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  LCDC_MCUInitStruct: pointer to a LCDC_MCUInitTypeDef structure that contains
  *              the configuration information for the specified LCDC peripheral. 
  * @retval None
  */
void LCDC_MCUInit(LCDC_TypeDef* LCDCx, LCDC_MCUInitTypeDef* LCDC_MCUInitStruct)
{
	/*variables for reading register*/
	u32 McuCfgReg;
	u32 McuSyncCfg;
	u32 McuTimingCfg;
	u32 DmaModeCfg;
	u32 PlaneSize;
	u32 LcdcCtrl;

	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_MCU_IF_MODE(LCDC_MCUInitStruct->LCDC_MCUIfMode));
	assert_param(IS_LCDC_MCU_MODE(LCDC_MCUInitStruct->LCDC_MCUMode));
	assert_param(IS_LCDC_DMA_MODE(LCDC_MCUInitStruct->LCDC_MCUDMAMode));
	assert_param(IS_LCDC_MCU_SYNC_MODE(LCDC_MCUInitStruct->LCDC_MCUSyncMode));
	assert_param(IS_LCDC_MCU_VSYNC_PUL_POLARITY(LCDC_MCUInitStruct->LCDC_MCUVsyncPulPlrty));
	assert_param(IS_LCDC_MCU_TE_PUL_POLARITY(LCDC_MCUInitStruct->LCDC_MCUTEPulPlrty));
	assert_param(IS_LCDC_MCU_DAT_PUL_POLARITY(LCDC_MCUInitStruct->LCDC_MCUDataPulPlrty));
	assert_param(IS_LCDC_MCU_RD_PUL_POLARITY(LCDC_MCUInitStruct->LCDC_MCURdPulPlrty));
	assert_param(IS_LCDC_MCU_WR_PUL_POLARITY(LCDC_MCUInitStruct->LCDC_MCUWrPulPlrty));
	assert_param(IS_LCDC_MCU_RS_PUL_POLARITY(LCDC_MCUInitStruct->LCDC_MCURsPulPlrty));
	assert_param(IS_LCDC_MCU_CS_PUL_POLARITY(LCDC_MCUInitStruct->LCDC_MCUCsPulPlrty));

	assert_param((LCDC_MCUInitStruct->LCDC_MCUImgHeight <= 0xfff)&&(LCDC_MCUInitStruct->LCDC_MCUImgWidth <= 0xfff));
	assert_param((LCDC_MCUInitStruct->LCDC_MCUTEDelay>=5) && (LCDC_MCUInitStruct->LCDC_MCUTEDelay<=65539));
	assert_param((LCDC_MCUInitStruct->LCDC_MCUVsyncIdlePrd>=5) && (LCDC_MCUInitStruct->LCDC_MCUVsyncIdlePrd<=0x100000-5));
	assert_param((LCDC_MCUInitStruct->LCDC_MCUVsyncSigWidth>=1) && (LCDC_MCUInitStruct->LCDC_MCUVsyncSigWidth<=0x100));
	assert_param((LCDC_MCUInitStruct->LCDC_MCURdInacvWidth>=1) && (LCDC_MCUInitStruct->LCDC_MCURdInacvWidth<=0x800));
	assert_param((LCDC_MCUInitStruct->LCDC_MCURdAcvWidth>=1) && (LCDC_MCUInitStruct->LCDC_MCURdAcvWidth<=0x800));
	assert_param((LCDC_MCUInitStruct->LCDC_MCUWrPulWidth>=2)&&(LCDC_MCUInitStruct->LCDC_MCUWrPulWidth<=0x400));
	assert_param(LCDC_MCUInitStruct->LCDC_MCUIOTimeOutClkNum<=0xffff);

	/*disable LCDC befrore all operations*/
	/*write 1 to LCDCINSDIS field in register LCDC_CTRL to disable LCDC instantly*/
	LCDCx->LCDC_CTRL |= LCDC_CTRL_LCDCINSDIS;

	/*read registers for configurartion*/
	McuCfgReg = LCDCx->LCDC_MCU_CFG;
	McuSyncCfg = LCDCx->LCDC_MCU_VSYNC_CFG;
	McuTimingCfg = LCDCx->LCDC_MCU_TIMING_CFG;
	DmaModeCfg = LCDCx->LCDC_DMA_MODE_CFG;
	PlaneSize = LCDCx->LCDC_PLANE_SIZE;
	LcdcCtrl = LCDCx->LCDC_CTRL;

	/*IO mode*/
	if(LCDC_MCUInitStruct->LCDC_MCUMode == LCDC_MCU_IO_MODE) {
		
		/*if enable IO mode, the internal clock sychronization must be selected*/
		assert_param(LCDC_MCUInitStruct->LCDC_MCUSyncMode == LCDC_MCU_SYNC_WITH_INTERNAL_CLK);
	} else { /*DMA mode*/
		/*trigger DMA mode*/
		if(LCDC_MCUInitStruct->LCDC_MCUDMAMode == LCDC_TRIGGER_DMA_MODE) {
			
			/*if enable trigger DMA mode, the internal clock sychronization must be selected*/
			assert_param(LCDC_MCUInitStruct->LCDC_MCUSyncMode == LCDC_MCU_SYNC_WITH_INTERNAL_CLK || \
			LCDC_MCUInitStruct->LCDC_MCUSyncMode == LCDC_MCU_SYNC_WITH_VSYNC);
		} else {/*auto DMA mode*/
		
			/*if auto DMA mode is enabled, VSYNC mode or TE mode must be selected*/
			assert_param(LCDC_MCUInitStruct->LCDC_MCUSyncMode != LCDC_MCU_SYNC_WITH_INTERNAL_CLK);
			
			/*Vsync mode*/
			if(LCDC_MCUInitStruct->LCDC_MCUSyncMode == LCDC_MCU_SYNC_WITH_VSYNC) {
				/*if MCU VSYNC mode is used, the VSYNC pulse polarity, VSYNC idle period, VSYNC signal width should be configured*/
				/*VSYNC pulse polarity configuration*/
				McuCfgReg &= (~LCDC_MCU_CFG_MCUSYPL);
				McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCUVsyncPulPlrty;

				/*VSYNC idle period*/
				McuSyncCfg &= (~LCDC_MCU_VSYNC_CFG_MCUVSPD);
				McuSyncCfg |= ((LCDC_MCUInitStruct->LCDC_MCUVsyncIdlePrd-5)<<12);

				/*VSYNC signal width*/
				McuSyncCfg &= (~LCDC_MCU_VSYNC_CFG_MCUVSW);
				McuSyncCfg |= (LCDC_MCUInitStruct->LCDC_MCUVsyncSigWidth-1);
			} else {/*TE mode*/
				/*if MCU TE mode is used, the TE delay interval, TE pulse polarity should be configured*/
				/* TE delay interval configuration*/
				McuCfgReg &= (~LCDC_MCU_CFG_IOMODE_TEDELAY);
				McuCfgReg |= ((LCDC_MCUInitStruct->LCDC_MCUTEDelay-5)<<16);

				/* TE pulse polarityl configuration*/
				McuCfgReg &= (~LCDC_MCU_CFG_TEPL);
				McuCfgReg |= (LCDC_MCUInitStruct->LCDC_MCUTEPulPlrty);
			}
		}
		/*if MCU I/F DMA mode is used, the PLANE size should be configured*/
		PlaneSize &= (~(LCDC_PLANE_SIZE_IMG_HEIGHT | LCDC_PLANE_SIZE_IMG_WIDTH));
		PlaneSize |= LCDC_MCUInitStruct->LCDC_MCUImgWidth;
		PlaneSize |= (LCDC_MCUInitStruct->LCDC_MCUImgHeight<<16);
	}

	/*configure MCU mode*/
	McuCfgReg &= (~LCDC_MCU_CFG_IOMODE_EN);
	McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCUMode;

	/*configure MCU sync mode*/
	McuCfgReg &= (~LCDC_MCU_CFG_MCUSYNCMODE);
	McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCUSyncMode;

	/*configure trigger mode or auto DMA*/
	DmaModeCfg &= (~LCDC_DMA_MODE_CFG_TRIGGER_MODE);
	DmaModeCfg |= LCDC_MCUInitStruct->LCDC_MCUDMAMode;

	/*configure MCU I/F mode(8 bit or 16bit)*/
	LcdcCtrl &= (~LCDC_CTRL_IF_MODE);
	LcdcCtrl |= LCDC_MCUInitStruct->LCDC_MCUIfMode;

	/*configure write, read, RS, CS, data pulse polarity*/
	McuCfgReg &= (~(LCDC_MCU_CFG_CSPL|LCDC_MCU_CFG_RSPL|LCDC_MCU_CFG_WRPL|LCDC_MCU_CFG_RDPL|LCDC_MCU_CFG_DATAPL));
	McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCUCsPulPlrty;
	McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCURsPulPlrty;
	McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCUWrPulPlrty;
	McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCURdPulPlrty;
	McuCfgReg |= LCDC_MCUInitStruct->LCDC_MCUDataPulPlrty;
	
	/*WR/RD pulse width*/
	McuTimingCfg &= (~(LCDC_MCU_TIMING_CFG_WRPULW|LCDC_MCU_TIMING_CFG_RDACTW|LCDC_MCU_TIMING_CFG_RDINACTW));
	LCDC_MCUInitStruct->LCDC_MCUWrPulWidth &= (~0x01);
	McuTimingCfg |= (LCDC_MCUInitStruct->LCDC_MCUWrPulWidth-2);
	McuTimingCfg |= ((LCDC_MCUInitStruct->LCDC_MCURdAcvWidth-1)<<10);
	McuTimingCfg |= ((LCDC_MCUInitStruct->LCDC_MCURdInacvWidth-1)<<21);

	/*configure timeout clock number according to write/read clock*/
	LCDC_MCUInitStruct->LCDC_MCUIOTimeOutClkNum = (LCDC_MCUInitStruct->LCDC_MCUWrPulWidth*16 + LCDC_MCUInitStruct->LCDC_MCURdAcvWidth + \
			LCDC_MCUInitStruct->LCDC_MCURdInacvWidth);

	/*configure IO write/read timeout clock number*/
	LCDCx->LCDC_MCU_IO_TO_CFG &= (~LCDC_MCU_IO_TIMEOUT_CLK_NUM);
	LCDCx->LCDC_MCU_IO_TO_CFG |= LCDC_MCUInitStruct->LCDC_MCUIOTimeOutClkNum;	

	/*write the value configured back to registers*/
	LCDCx->LCDC_MCU_CFG = McuCfgReg;
	LCDCx->LCDC_MCU_VSYNC_CFG = McuSyncCfg;
	LCDCx->LCDC_MCU_TIMING_CFG = McuTimingCfg;
	LCDCx->LCDC_DMA_MODE_CFG = DmaModeCfg;
	LCDCx->LCDC_PLANE_SIZE = PlaneSize;
	LCDCx->LCDC_CTRL = LcdcCtrl;
}

/**
  * @brief    Trigger the hardware to transfer one frame from DMA buffer based 
  *               on sync signal.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @retval None
  */
void LCDC_MCUDMATrigger(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*write 1 to TRIGER_ONETIME field in register LCDC_DMA_MODE_CFG*/
	LCDCx->LCDC_DMA_MODE_CFG |= LCDC_DMA_MODE_CFG_TRIGGER_ONE_TIME;
}

/**
  * @brief     Get the MCU I/F IO mode run status.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @retval   status value:
  *              - 1: IO mode run
  *              - 0: DMA mode run
  */
u32 LCDC_MCUGetRunStatus(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*aquire the IO mode run status*/
	if(LCDCx->LCDC_MCU_CFG & LCDC_MCU_CFG_IOMODE_RUN) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief     write data to MCU I/F bus.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  Data: the data to transmit.
  * @retval   None
  */
void LCDC_MCUIOWriteData(LCDC_TypeDef* LCDCx, u16 Data)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*write data to tx fifo*/
	LCDCx->LCDC_MCU_IO_DATA= ((u32)Data&LCDC_MCU_IO_WR_DAT_FG); 
}

/**
  * @brief     read data from MCU I/F bus.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @retval   the read value
  */
u16 LCDC_MCUIOReadData(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*read data from bus*/
	return (u16)LCDCx->LCDC_MCU_IO_DATA; 
}

/**
  * @brief     write command to MCU I/F bus.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  Cmd: the command to transmit.
  * @retval   None
  */
void LCDC_MCUIOWriteCmd(LCDC_TypeDef* LCDCx, u16 Cmd)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*write command to tx fifo*/
	LCDCx->LCDC_MCU_IO_DATA = ((u32)Cmd|LCDC_MCU_IO_WR_CMD_FG);
}

/*--------------------------------------------RGB I/F functions-----------------------------------------------*/
/**
  * @brief  Fills each LCDC_RGBInitStruct member with its default value.
  * @param  LCDC_RGBInitStruct: pointer to an LCDC_RGBInitTypeDef structure which will be initialized.
  * @retval   None
  */
void LCDC_RGBStructInit(LCDC_RGBInitTypeDef *  LCDC_RGBInitStruct)
{
	/* LCDC_MCUInitStruct members default value */
	LCDC_RGBInitStruct->LCDC_RGBIfMode = LCDC_RGB_IF_16_BIT;						 	
	LCDC_RGBInitStruct->LCDC_RGBImgHeight = 272;
	LCDC_RGBInitStruct->LCDC_RGBImgWidth = 480;
	LCDC_RGBInitStruct->LCDC_RGBRefreshFreq = 60;													 	
	LCDC_RGBInitStruct->LCDC_RGBSyncMode = LCDC_RGB_HV_MODE;													 	
	LCDC_RGBInitStruct->LCDC_RGBDatPulPlrty = LCDC_RGB_DAT_PUL_NORMAL;
	LCDC_RGBInitStruct->LCDC_RGBEnPulPlrty = LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE;	
	LCDC_RGBInitStruct->LCDC_RGBHsPulPlrty = LCDC_RGB_HS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStruct->LCDC_RGBVsPulPlrty = LCDC_RGB_VS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStruct->LCDC_RGBDclkActvEdge = LCDC_RGB_DCLK_RISING_EDGE_FETCH;
	LCDC_RGBInitStruct->LCDC_RGBVFP = 8;													 	
	LCDC_RGBInitStruct->LCDC_RGBVBP = 8;													 	
	LCDC_RGBInitStruct->LCDC_RGBVSW = 1;														 	
	LCDC_RGBInitStruct->LCDC_RGBHFP= 5;													 	
	LCDC_RGBInitStruct->LCDC_RGBHBP = 40;													 	
	LCDC_RGBInitStruct->LCDC_RGBHSW = 1;
}

/**
  * @brief    Initializes the LCDC peripheral according to the specified
  *               parameters in the LCDC_RGBInitStruct.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  LCDC_RGBInitStruct: pointer to a LCDC_RGBInitTypeDef structure that contains
  *              the configuration information for the specified LCDC peripheral. 
  * @retval None
  */
void LCDC_RGBInit(LCDC_TypeDef* LCDCx, LCDC_RGBInitTypeDef* LCDC_RGBInitStruct)
{

	/*variables for reading register*/
	u32 RgbCfg;
	u32 RgbVsyncCfg;
	u32 RgbHsyncCfg;
	u32 DmaModeCfg;
	u32 PlaneSize;
	u32 LcdcCtrl;

	/*variables for calculating DCLK divider*/
	u32 LcdcClkDiv;
	u32 HDotNum;
	u32 VLineNum;
	u32 TempFreq;
	
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_RGB_IF_MODE(LCDC_RGBInitStruct->LCDC_RGBIfMode));
	assert_param(IS_LCDC_RGB_SYNC_MODE(LCDC_RGBInitStruct->LCDC_RGBSyncMode));
	assert_param(IS_LCDC_RGB_DAT_PUL_POLARITY(LCDC_RGBInitStruct->LCDC_RGBDatPulPlrty));
	assert_param(IS_LCDC_RGB_EN_PUL_POLARITY(LCDC_RGBInitStruct->LCDC_RGBEnPulPlrty));
	assert_param(IS_LCDC_RGB_HS_PUL_POLARITY(LCDC_RGBInitStruct->LCDC_RGBHsPulPlrty));
	assert_param(IS_LCDC_RGB_VS_PUL_POLARITY(LCDC_RGBInitStruct->LCDC_RGBVsPulPlrty));
	assert_param(IS_LCDC_RGB_DCLK_EDGE(LCDC_RGBInitStruct->LCDC_RGBDclkActvEdge));

	assert_param((LCDC_RGBInitStruct->LCDC_RGBImgHeight <= 0xfff)&&(LCDC_RGBInitStruct->LCDC_RGBImgWidth<=0xfff));
	assert_param((LCDC_RGBInitStruct->LCDC_RGBVBP>=1)&&(LCDC_RGBInitStruct->LCDC_RGBVBP<=0x10));
	assert_param((LCDC_RGBInitStruct->LCDC_RGBVFP>=1)&&(LCDC_RGBInitStruct->LCDC_RGBVFP<=0x10));
	assert_param((LCDC_RGBInitStruct->LCDC_RGBVSW>=1)&&(LCDC_RGBInitStruct->LCDC_RGBVSW<=0x10));
	assert_param((LCDC_RGBInitStruct->LCDC_RGBHFP>=1)&&(LCDC_RGBInitStruct->LCDC_RGBHFP<=0x100));
	assert_param((LCDC_RGBInitStruct->LCDC_RGBHBP>=1)&&(LCDC_RGBInitStruct->LCDC_RGBHBP<=0x100));
	assert_param((LCDC_RGBInitStruct->LCDC_RGBHSW>=1)&&(LCDC_RGBInitStruct->LCDC_RGBHSW<=0x100));

	/*disable LCDC befrore all operations*/
	/*write 1 to LCDCINSDIS field in register LCDC_CTRL to disable LCDC instantly*/
	LCDCx->LCDC_CTRL |= LCDC_CTRL_LCDCINSDIS;
	
	/*read registers for configurartion*/
	RgbCfg = LCDCx->LCDC_RGB_CFG;
	RgbVsyncCfg = LCDCx->LCDC_RGB_VSYNC_CFG;
	RgbHsyncCfg = LCDCx->LCDC_RGB_HSYNC_CFG;
	DmaModeCfg = LCDCx->LCDC_DMA_MODE_CFG;
	PlaneSize = LCDCx->LCDC_PLANE_SIZE;
	LcdcCtrl = LCDCx->LCDC_CTRL;

	/*if the RGB DE mode is used, configure the DE pulse polarity*/
	if(LCDC_RGBInitStruct->LCDC_RGBSyncMode == LCDC_RGB_DE_MODE) {
		RgbCfg &= (~LCDC_RGB_CFG_ENPL);
		RgbCfg |= LCDC_RGBInitStruct->LCDC_RGBEnPulPlrty;
	}

	/*configure RGB sync mode*/
	RgbCfg &= (~LCDC_RGB_CFG_RGB_SYNC_MODE);
	RgbCfg |= LCDC_RGBInitStruct->LCDC_RGBSyncMode;	

	/*configure plane size*/
	PlaneSize &= (~(LCDC_PLANE_SIZE_IMG_HEIGHT | LCDC_PLANE_SIZE_IMG_WIDTH));
	PlaneSize |= LCDC_RGBInitStruct->LCDC_RGBImgWidth;
	PlaneSize |= (LCDC_RGBInitStruct->LCDC_RGBImgHeight<<16);	

	/*configure RGB I/F mode*/
	LcdcCtrl &= (~LCDC_CTRL_IF_MODE);
	LcdcCtrl |= LCDC_RGBInitStruct->LCDC_RGBIfMode;	

	/*configure RGB data, VSYNC, HSYNC pulse polarity and DCLK active edge*/
	RgbCfg &= (~(LCDC_RGB_CFG_DATPL|LCDC_RGB_CFG_HSPL|LCDC_RGB_CFG_VSPL|LCDC_RGB_CFG_DCLKPL));
	RgbCfg |= LCDC_RGBInitStruct->LCDC_RGBDatPulPlrty;
	RgbCfg |= LCDC_RGBInitStruct->LCDC_RGBHsPulPlrty;
	RgbCfg |= LCDC_RGBInitStruct->LCDC_RGBVsPulPlrty;;
	RgbCfg |= LCDC_RGBInitStruct->LCDC_RGBDclkActvEdge;

	/*configure vertical synchronization parameters: VFP, VBP, VSW*/
	RgbVsyncCfg &= (~(LCDC_RGB_VSYNC_CFG_VSW|LCDC_RGB_VSYNC_CFG_VBP|LCDC_RGB_VSYNC_CFG_VFP));
	RgbVsyncCfg |= (LCDC_RGBInitStruct->LCDC_RGBVSW-1);
	RgbVsyncCfg |= ((LCDC_RGBInitStruct->LCDC_RGBVBP-1)<<8);
	RgbVsyncCfg |= ((LCDC_RGBInitStruct->LCDC_RGBVFP-1)<<16);

	/*configure horizontal synchronization parameters: HFP, HBP, HSW*/
	RgbHsyncCfg &= (~(LCDC_RGB_HSYNC_CFG_HSW|LCDC_RGB_HSYNC_CFG_HBP|LCDC_RGB_HSYNC_CFG_HFP));
	RgbHsyncCfg |= (LCDC_RGBInitStruct->LCDC_RGBHSW-1);
	RgbHsyncCfg |= ((LCDC_RGBInitStruct->LCDC_RGBHBP-1)<<8);
	RgbHsyncCfg |= ((LCDC_RGBInitStruct->LCDC_RGBHFP-1)<<16);

	/*configure auto DMA mode; if LCDC RGB I/F mode is used, the auto DMA mode must be enabled*/
	DmaModeCfg &= (~LCDC_DMA_MODE_CFG_TRIGGER_MODE);
	DmaModeCfg |= LCDC_AUTO_DMA_MODE;

	/*calculate the DCLK clock divider according to the plane size, refreh frequency and LCDC_SYS_CLK*/
	HDotNum = LCDC_RGBInitStruct->LCDC_RGBImgWidth + LCDC_RGBInitStruct->LCDC_RGBHSW + LCDC_RGBInitStruct->LCDC_RGBHBP + \
				LCDC_RGBInitStruct->LCDC_RGBHFP;
	VLineNum = LCDC_RGBInitStruct->LCDC_RGBImgHeight + LCDC_RGBInitStruct->LCDC_RGBVSW + LCDC_RGBInitStruct->LCDC_RGBVBP + \
				LCDC_RGBInitStruct->LCDC_RGBVFP;

	/*if RGB 6bit mode is used, triple the DCLK number in each line*/
	if(LCDC_RGBInitStruct->LCDC_RGBIfMode == LCDC_RGB_IF_6_BIT) {
		HDotNum = 3*HDotNum;
	}

	/*calculate the actual divider*/
	TempFreq = LCDC_RGBInitStruct->LCDC_RGBRefreshFreq*HDotNum*VLineNum;
	if((LCDC_SYS_CLK % TempFreq) >= (TempFreq + 1) / 2){
		LcdcClkDiv = LCDC_SYS_CLK / TempFreq + 1;
	} else {
		LcdcClkDiv = LCDC_SYS_CLK / TempFreq;
	}

	/*Because DCLK=SYS_CLK/(CLKDIV+2), the actual divider must be not less than 2 and not more than 65536*/
	assert_param((LcdcClkDiv>=2)&&((LcdcClkDiv<=65536)));

	/*ensure CLKDIV is a even value*/
	if(LcdcClkDiv%2 != 0){
		LcdcClkDiv = LcdcClkDiv - 1;
	}

	/*fill the CLKDIV field in the register LCDC_CLK_DIV*/
	LCDCx->LCDC_CLK_DIV &= (~LCDC_CLK_DIV_DCLKDIV);
	LCDCx->LCDC_CLK_DIV |= (LcdcClkDiv-2);

	/*write the value configured back to registers*/
	LCDCx->LCDC_RGB_CFG = RgbCfg;
	LCDCx->LCDC_RGB_VSYNC_CFG = RgbVsyncCfg;
	LCDCx->LCDC_RGB_HSYNC_CFG = RgbHsyncCfg;
	LCDCx->LCDC_DMA_MODE_CFG = DmaModeCfg;
	LCDCx->LCDC_PLANE_SIZE = PlaneSize;
	LCDCx->LCDC_CTRL = LcdcCtrl;
}

/**
  * @brief    Get the RGB synchronization status.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  pHSStatus: horizontal synchronization status pointer.
  *		    HSStatus can be one of the following values:
  *     	    @arg LCDC_RGB_SYNC_STATUS_HSYNC:  HSYNC
  *                @arg LCDC_RGB_SYNC_STATUS_HFP    :  HFP
  *     	    @arg LCDC_RGB_SYNC_STATUS_HACTIVE:  ACTIVE
  *                @arg LCDC_RGB_SYNC_STATUS_HBP    :  HBP
  * @param  pVSStatus: vertical synchronization status pointer. 
  *		    VSStatus can be one of the following values:
  *     	    @arg LCDC_RGB_SYNC_STATUS_VSYNC:  VSYNC
  *                @arg LCDC_RGB_SYNC_STATUS_VFP    :  VFP
  *     	    @arg LCDC_RGB_SYNC_STATUS_VACTIVE:  ACTIVE
  *                @arg LCDC_RGB_SYNC_STATUS_VBP    :  VBP
  * @retval None
  */
void LCDC_RGBGetSyncStatus(LCDC_TypeDef* LCDCx, u32 * pHSStatus, u32 * pVSStatus)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*Get horizontal synchronization status*/
	*pHSStatus = (LCDCx->LCDC_RGB_SYNC_STATUS & LCDC_RGB_SYNC_STATUS_HSSTATUS);

	/*Get vertical synchronization status*/
	*pVSStatus = (LCDCx->LCDC_RGB_SYNC_STATUS & LCDC_RGB_SYNC_STATUS_VSSTATUS);
}

/*----------------------------------------------LED I/F functions---------------------------------------------*/
/**
  * @brief  Fills each LCDC_LEDInitStruct member with its default value.
  * @param  LCDC_LEDInitStruct: pointer to an LCDC_LEDInitTypeDef structure which will be initialized.
  * @retval   None
  */
void LCDC_LEDStructInit(LCDC_LEDInitTypeDef *  LCDC_LEDInitStruct)
{
	/* LCDC_MCUInitStruct members default value */
	LCDC_LEDInitStruct->LCDC_LEDIfMode = LCDC_LED_IF;
	LCDC_LEDInitStruct->LCDC_LEDImgHeight = 16;
	LCDC_LEDInitStruct->LCDC_LEDImgWidth = 64; 
	LCDC_LEDInitStruct->LCDC_LEDRefreshFreq = 100;
	LCDC_LEDInitStruct->LCDC_LEDColorChnl = LCDC_LED_ONE_CHANNEL;								 	
	LCDC_LEDInitStruct->LCDC_LEDColorNum = LCDC_LED_ONE_COLOR;
	LCDC_LEDInitStruct->LCDC_LEDOEActw = 7;											 	
	LCDC_LEDInitStruct->LCDC_LEDLatchWidth = 2;
	LCDC_LEDInitStruct->LCDC_LEDFrmIdlePd = 2;
	LCDC_LEDInitStruct->LCDC_LEDClkPlrty = LCDC_LED_DCLK_RISING_EDGE_FETCH;
	LCDC_LEDInitStruct->LCDC_LEDLatPlrty = LCDC_LED_LACTH_RISING_EDGE_FETCH;
	LCDC_LEDInitStruct->LCDC_LEDOePlrty = LCDC_LED_OE_LOW_LEV_ACTIVE;
	LCDC_LEDInitStruct->LCDC_LEDDatPlrty = LCDC_LED_DAT_PUL_NORMAL;
	LCDC_LEDInitStruct->LCDC_LEDLineIdlePd = 1;
}

/**
  * @brief    Initializes the LCDC peripheral according to the specified
  *               parameters in the LCDC_LEDInitStruct.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  LCDC_LEDInitStruct: pointer to a LCDC_LEDInitTypeDef structure that contains
  *              the configuration information for the specified LCDC peripheral. 
  * @retval None
  */
void LCDC_LEDInit(LCDC_TypeDef* LCDCx, LCDC_LEDInitTypeDef* LCDC_LEDInitStruct)
{

	/*variables for reading register*/
	u32 LedCfg;
	u32 LedTiming;
	u32 DmaModeCfg;
	u32 PlaneSize;
	u32 LcdcCtrl;
	u32 LedIdle;

	/*variables for calculating DCLK divider*/
	u32 LcdcClkDiv;
	u32 HDotNum;
	u32 VLineNum;
	u32 TempFreq;
	
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_LED_IF_MODE(LCDC_LEDInitStruct->LCDC_LEDIfMode));
	assert_param(IS_LCDC_LED_CHANNEL(LCDC_LEDInitStruct->LCDC_LEDColorChnl));
	assert_param(IS_LCDC_LED_COLOR_NUM(LCDC_LEDInitStruct->LCDC_LEDColorNum));
	assert_param(IS_LCDC_LED_DCLK_EDGE(LCDC_LEDInitStruct->LCDC_LEDClkPlrty));
	assert_param(IS_LCDC_LED_LATCH_EDGE(LCDC_LEDInitStruct->LCDC_LEDLatPlrty));
	assert_param(IS_LCDC_LED_OE_PUL_POLARITY(LCDC_LEDInitStruct->LCDC_LEDOePlrty));
	assert_param(IS_LCDC_LED_DAT_PUL_POLARITY(LCDC_LEDInitStruct->LCDC_LEDDatPlrty));

	assert_param((LCDC_LEDInitStruct->LCDC_LEDImgHeight <= 0xfff)&&(LCDC_LEDInitStruct->LCDC_LEDImgWidth <= 0xfff));
	assert_param((LCDC_LEDInitStruct->LCDC_LEDOEActw >= 1) && (LCDC_LEDInitStruct->LCDC_LEDOEActw <= 0x10000));
	assert_param((LCDC_LEDInitStruct->LCDC_LEDFrmIdlePd >= 1) && (LCDC_LEDInitStruct->LCDC_LEDFrmIdlePd <= 0x1000));
	assert_param((LCDC_LEDInitStruct->LCDC_LEDLatchWidth >= 1) && (LCDC_LEDInitStruct->LCDC_LEDLatchWidth <= 0x100));
	assert_param((LCDC_LEDInitStruct->LCDC_LEDLineIdlePd >= 1) && (LCDC_LEDInitStruct->LCDC_LEDLineIdlePd <= 0x100));

	/*disable LCDC befrore all operations*/
	/*write 1 to LCDCINSDIS field in register LCDC_CTRL to disable LCDC instantly*/
	LCDCx->LCDC_CTRL |= LCDC_CTRL_LCDCINSDIS;

	/*read register for configuration*/
	LedCfg = LCDCx->LCDC_LED_CFG;
	LedTiming = LCDCx->LCDC_LED_TIMING;
	DmaModeCfg = LCDCx->LCDC_DMA_MODE_CFG;
	PlaneSize = LCDCx->LCDC_PLANE_SIZE;
	LcdcCtrl = LCDCx->LCDC_CTRL;
	LedIdle = LCDCx->LCDC_LED_IDLE;

	/*configure LED I/F mode*/
	LcdcCtrl &= (~LCDC_CTRL_IF_MODE);
	LcdcCtrl |= LCDC_LEDInitStruct->LCDC_LEDIfMode;

	/*configure LED plane size*/
	PlaneSize &= (~(LCDC_PLANE_SIZE_IMG_HEIGHT | LCDC_PLANE_SIZE_IMG_WIDTH));
	PlaneSize |= LCDC_LEDInitStruct->LCDC_LEDImgWidth;
	PlaneSize |= (LCDC_LEDInitStruct->LCDC_LEDImgHeight<<16);	
	
	/*configure color channel and color number*/
	LedCfg &= (~(LCDC_LED_CFG_COLORNUM | LCDC_LED_CFG_COLORCHAN));
	LedCfg |= LCDC_LEDInitStruct->LCDC_LEDColorNum;	
	LedCfg |= LCDC_LEDInitStruct->LCDC_LEDColorChnl;

	/*configure LED I/F bus polarity*/
	LedCfg &= (~(LCDC_LED_CFG_CLKPL|LCDC_LED_CFG_LATPL|LCDC_LED_CFG_OEPL|LCDC_LED_CFG_DATPL));
	LedCfg |= LCDC_LEDInitStruct->LCDC_LEDClkPlrty;
	LedCfg |= LCDC_LEDInitStruct->LCDC_LEDLatPlrty;	
	LedCfg |= LCDC_LEDInitStruct->LCDC_LEDOePlrty;
	LedCfg |= LCDC_LEDInitStruct->LCDC_LEDDatPlrty;

	/*configure LED I/F timing*/
	LedTiming &= (~(LCDC_LED_TIMING_LATW | LCDC_LED_TIMING_OEACTW));
	LedTiming |= LCDC_LEDInitStruct->LCDC_LEDLatchWidth-1;
	LedTiming |= ((LCDC_LEDInitStruct->LCDC_LEDOEActw-1)<<8);

	/*configure LED I/F line and frame idle timing*/
	LedIdle &= (~(LCDC_LED_TIMING_LINEIDLE | LCDC_LED_TIMING_FRMIDLEPD));
	LedIdle |= LCDC_LEDInitStruct->LCDC_LEDLineIdlePd-1;
	LedIdle |= ((LCDC_LEDInitStruct->LCDC_LEDFrmIdlePd-1)<<20);

	/*configure auto DMA mode, if LCDC LED I/F mode is used, the auto DMA mode must be enabled*/
	DmaModeCfg &= (~LCDC_DMA_MODE_CFG_TRIGGER_MODE);
	DmaModeCfg |= LCDC_AUTO_DMA_MODE;

	HDotNum = LCDC_LEDInitStruct->LCDC_LEDImgWidth + LCDC_LEDInitStruct->LCDC_LEDLineIdlePd;
	VLineNum = LCDC_LEDInitStruct->LCDC_LEDImgHeight;
	if(LCDC_LEDInitStruct->LCDC_LEDColorChnl == LCDC_LED_TWO_CHANNEL) {
		VLineNum = VLineNum/2;
	}
	TempFreq = (HDotNum*VLineNum+LCDC_LEDInitStruct->LCDC_LEDFrmIdlePd)*LCDC_LEDInitStruct->LCDC_LEDRefreshFreq;

	/*calculate the actual divider*/
	if((LCDC_SYS_CLK % TempFreq) >= (TempFreq + 1) / 2){
		LcdcClkDiv = LCDC_SYS_CLK / TempFreq + 1;
	} else {
		LcdcClkDiv = LCDC_SYS_CLK / TempFreq;
	}

	/*Because DCLK=SYS_CLK/(CLKDIV+2), the actual divider must be not less than 2*/
	assert_param((LcdcClkDiv>=2)&&(LcdcClkDiv<=65536));	

	/*ensure CLKDIV is a even value*/
	if(LcdcClkDiv%2 != 0){
		LcdcClkDiv = LcdcClkDiv - 1;
	}

	/*fill the CLKDIV field in the register LCDC_CLK_DIV*/
	LCDCx->LCDC_CLK_DIV &= (~LCDC_CLK_DIV_DCLKDIV);
	LCDCx->LCDC_CLK_DIV |= (LcdcClkDiv-2);

	/*write the value configured back to registers*/
	LCDCx->LCDC_LED_CFG = LedCfg;
	LCDCx->LCDC_LED_TIMING = LedTiming;
	LCDCx->LCDC_DMA_MODE_CFG = DmaModeCfg;
	LCDCx->LCDC_PLANE_SIZE = PlaneSize;
	LCDCx->LCDC_CTRL = LcdcCtrl;
	LCDCx->LCDC_LED_IDLE = LedIdle;
}

/**
  * @brief     Confugure OE active width.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  OEActvWidth: the OE signal active width; Unit: DCLK.
  * @retval   None
  */
void LCDC_LEDSetOEWidth(LCDC_TypeDef* LCDCx, u32 OEActvWidth)
{
	u32 LedTiming;
	
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param((OEActvWidth >= 1) && (OEActvWidth <= 0x1000));

	LedTiming = LCDCx->LCDC_LED_TIMING;

	/*fill the OEACTW field in register LCDC_LED_TIMING*/
	LedTiming &= ~LCDC_LED_TIMING_OEACTW;
	LedTiming |= ((OEActvWidth-1)<<8);
	LCDCx->LCDC_LED_TIMING = LedTiming;
	
	/*set the bit LEDIFUPDATE in register LCDC_LED_CFG*/
	LCDCx->LCDC_LED_CFG |= LCDC_LED_CFG_LEDIFUPDATE;
}

/*-----------------------------------------------DMA functions------------------------------------------------*/
  /**
  * @brief    Configure LCDC DMA burst size .
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  BurstSize: DMA burst size; Unit 64 Bytes.
  * @note     If BurstSize=1, the actual burstsize = 1x64 Bytes; if the BurstSize=2, 
  *                the actual burstsize = 2x64 = 128 Bytes.... The parameter "BurstSize" is not more
  *                than 8.
  * @retval   None
  */
void LCDC_DMAModeConfig(LCDC_TypeDef* LCDCx, u32 BurstSize)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param((BurstSize>=1) && (BurstSize<=8));

	/*fill the RDOTST field in register LCDC_DMA_MODE_CFG*/
	LCDCx->LCDC_DMA_MODE_CFG &= ~LCDC_DMA_MODE_CFG_RDOTST;
	LCDCx->LCDC_DMA_MODE_CFG |= (BurstSize-1)<<2;	
}

  /**
  * @brief    Configure LCDC DMA under flow mode and under flow error data .
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  DmaUnFlwMode: DMA under flow mode, this parameter
  *                can be one of the following values:
  *     	    @arg LCDC_DMAUNFW_OUTPUT_LASTDATA:  output last data
  *                @arg LCDC_DMAUNFW_OUTPUT_ERRORDATA:  output error data
  * @param  ErrorData: the output data when  DMA FIFO underflow occurred. When under flow mode is configured as
  *                LCDC_DMAUNFW_OUTPUT_ERRORDATA, this parameter is needed, and otherwise it can be ignored.
  * @retval   None
  */
void LCDC_DMAUnderFlowConfig(LCDC_TypeDef* LCDCx, u32 DmaUnFlwMode, u32 ErrorData)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_DMA_DMA_UNDERFLOW_MODE(DmaUnFlwMode));
	assert_param(ErrorData <= 0xFFFF);

	/*fill the DMAUNMODE field in register LCDC_UNDFLW_CFG*/
	LCDCx->LCDC_UNDFLW_CFG &= ~LCDC_UNDFLW_CFG_UNDMODE;
	LCDCx->LCDC_UNDFLW_CFG |= DmaUnFlwMode;

	if(DmaUnFlwMode==LCDC_DMAUNFW_OUTPUT_ERRORDATA) {
		/*fill the ERROUTDATA field in register LCDC_UNDFLW_CFG*/
		LCDCx->LCDC_UNDFLW_CFG &= ~LCDC_UNDFLW_CFG_ERROUTDATA;
		LCDCx->LCDC_UNDFLW_CFG |= ErrorData;
	}
}

  /**
  * @brief    Configure image base address.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  ImgBaseAddr: the image base address.
  * @retval   None
  */
void LCDC_DMAImageBaseAddrConfig(LCDC_TypeDef* LCDCx, u32 ImgBaseAddr)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*fill the register LCDC_IMG_BASE_ADDR*/
	LCDCx->LCDC_IMG_BASE_ADDR = ImgBaseAddr;
}
 
/*---------------------------------------Interrupt functions--------------------------------------------*/

/**
  * @brief  Enables or disables the specified LCDC interrupts.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  LCDC_IT: specifies the LCDC interrupts sources to be enabled or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg LCDC_IT_DMAUNDFW: DMA FIFO underflow interrupt
  *     @arg LCDC_IT_FRDN: LCD refresh done interrupt
  *     @arg LCDC_IT_LINE: line interrupt
  *     @arg LCDC_IT_IO_TIMEOUT: IO write/read timeout interrupt
  *     @arg LCDC_IT_FRM_START: Frame Start interrupt
  * @param  NewState: new state of the specified LCDC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LCDC_INTConfig(LCDC_TypeDef* LCDCx, u32 LCDC_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	if (NewState != DISABLE) {
		/* Enable the selected LCDC interrupts */
		LCDCx->LCDC_IRQ_EN |= LCDC_IT;
	} else {
		/* Disable the selected LCDC interrupts */
		LCDCx->LCDC_IRQ_EN &= (~LCDC_IT);
	}
}

/**
  * @brief   Configure line interrupt position.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  LineNum: the line number. 
  * @retval None
  */
void LCDC_LineINTPosConfig(LCDC_TypeDef* LCDCx, u32 LineNum)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(LineNum<=0xFFF);

	/*fill the LINE_INT_POS field in register LCDC_LINE_INT_POS*/
	LCDCx->LCDC_LINE_INT_POS &= (~LCDC_LINE_INTPOS);
	LCDCx->LCDC_LINE_INT_POS |= LineNum;
}

/**
  * @brief  Get lcdc interrupt status.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @retval interrupt status
  */
u32 LCDC_GetINTStatus(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));	

	return LCDCx->LCDC_IRQ_STATUS;
}

/**
  * @brief  Get LCDC Raw Interrupt Status.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @retval raw interrupt status
  */
u32 LCDC_GetRawINTStatus(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));	

	return LCDCx->LCDC_IRQ_RAW;
}

/**
  * @brief  Clears all of the LCDC interrupt pending bit.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @retval None
  */
void LCDC_ClearAllINT(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));	

	/*write 1 to clear all interrupts*/
	LCDCx->LCDC_IRQ_STATUS = 0xFFFFFFFF;
}

/**
  * @brief  Clears the LCDC's interrupt pending bits.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  LCDC_IT: specifies the interrupt to be cleared.
  *   This parameter can be any combination of the following values:
  *     @arg LCDC_IT_LINE:line interrupt
  *     @arg LCDC_IT_FRDN: refresh frame done interrupt
  *     @arg LCDC_IT_DMAUNDFW: DMA FIFO under flow interrupt
  *     @arg LCDC_IT_IO_TIMEOUT: IO write/read timeout interrupt
  *     @arg LCDC_IT_FRM_START: Frame Start interrupt
  * @retval None
  */
void LCDC_ClearINT(LCDC_TypeDef* LCDCx, u32 LCDC_IT)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));
	assert_param(IS_LCDC_CLEAR_IT(LCDC_IT));

	/*clear the specified interrupt*/
	LCDCx->LCDC_IRQ_STATUS = LCDC_IT;
}

/**
  * @brief  Get the current position.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  pCurPosX: the current X position pointer.
  * @param  pCurPosY: the current Y position pointer.
  * @retval None
  */
void LCDC_GetCurPosStatus(LCDC_TypeDef* LCDCx, u32* pCurPosX, u32* pCurPosY)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*Get the X position*/
	*pCurPosX = (LCDCx->LCDC_CUR_POS_STATUS & LCDC_CUR_POS_STS_X);

	/*Get the Y position*/
	*pCurPosY = ((LCDCx->LCDC_CUR_POS_STATUS & LCDC_CUR_POS_STS_Y)>>16);
}

/**
  * @brief  Get the DMA FIFO under flow interrupt count.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  DmaUnIntCnt: the DMA under flow interrupt count pointer.
  * @retval None
  */
void LCDC_GetDmaUnINTCnt(LCDC_TypeDef* LCDCx, u32* DmaUnIntCnt)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*get the DMA under flow interrupt count*/
	*DmaUnIntCnt = (LCDCx->LCDC_STATUS & LCDC_STATUS_DMAUNINTCNT);
}

/*------------------------------------------------Global  APIs------------------------------------------------*/
/**
  * @brief  Enables or disables the LCDC.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @param  NewState: new state of the LCDC. 
  *   This parameter can be: ENABLE or DISABLE.
  * @note   when NewState is DISABLE, during the period of valid line(VTIMING =valid data), 
  *              the disable action will be performed after the last valid line had transferred. During 
  *              the other periods, performed the disable action instantly. If disable the LCDC instantly,
  *              use the API LCDC_InsDisable().
  * @retval None
  */
void LCDC_Cmd(LCDC_TypeDef* LCDCx, u32 NewState)
{
	u32 TempCtrl;

	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	TempCtrl = LCDCx->LCDC_CTRL;

	if (NewState != DISABLE) {
		/* clear DISABLE bits, or it leads to enable LCDC unsuccessfully*/
		TempCtrl &= ~(LCDC_CTRL_LCDCDIS|LCDC_CTRL_LCDCINSDIS);

		/*set ENBALE bit*/
		TempCtrl |= LCDC_CTRL_LCDCEN;
	} else {

		/*clear instant disable bit*/
		TempCtrl &= ~LCDC_CTRL_LCDCINSDIS;
	
		/*set DISABLE bit*/
		TempCtrl |= LCDC_CTRL_LCDCDIS;
	}

	LCDCx->LCDC_CTRL = TempCtrl;
}

/**
  * @brief  Disable the LCDC instantly.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @note   Disable LCDC instantly. HW will reset LCDC internal states and disable LCDC 
  *              then clear both this bit and LCDCEN.
  * @retval None
  */
void LCDC_InsDisable(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*write 1 to LCDCINSDIS field in register LCDC_CTRL to disable LCDC instantly*/
	LCDCx->LCDC_CTRL |= LCDC_CTRL_LCDCINSDIS;
}

/**
  * @brief  Deinitializes the LCDC.
  * @param  LCDCx: where LCDCx can be LCDC.
  * @note   Disable LCDC instantly, clear and disable all interrupts.
  * @retval None
  */
void LCDC_DeInit(LCDC_TypeDef* LCDCx)
{
	/*check the parameters*/
	assert_param(IS_LCDC_ALL_PERIPH(LCDCx));

	/*disable LCDC instantly*/
	LCDCx->LCDC_CTRL |= LCDC_CTRL_LCDCINSDIS;
	
	/*clear all interrupts*/
	LCDCx->LCDC_IRQ_STATUS = 0xFFFFFFFF;

	/*disable all interrupts*/
	LCDCx->LCDC_IRQ_EN = 0;
}
