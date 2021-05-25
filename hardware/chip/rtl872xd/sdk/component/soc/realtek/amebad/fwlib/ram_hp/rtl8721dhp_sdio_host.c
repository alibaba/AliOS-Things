#include "ameba_soc.h"

SDIOH_InitTypeDef sdioh_init_para;

/**
  * @brief  Check SDIOH is busy or not.
  * @param  None
  * @retval SDIOH busy status value:
  *           - HAL_BUSY: Busy
  *           - HAL_OK: Not Busy
  */
u32 SDIOH_Busy(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 idle_level = sdioh_init_para.SDIOH_idle_level;

	/* check the SD bus status */
	if((psdioh->SD_BUS_STATUS & idle_level) == idle_level) {
		/* check the CMD & DATA state machine */
		if((psdioh->SD_CMD_STATE & SDIOH_CMD_FSM_IDLE) && (psdioh->SD_DATA_STATE & SDIOH_DATA_FSM_IDLE)) {
			/* check the SD card module state machine */
			if((psdioh->SD_TRANSFER & SDIOH_SD_MODULE_FSM_IDLE)) {
				return HAL_OK;
			} else {
				DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "SD card module state machine isn't in the idle state !\r\n");
				return HAL_BUSY;
			}            
		} else {
			DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "CMD or DATA state machine isn't in the idle state !!\r\n");
			return HAL_BUSY;
		}
	} else {
		DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "CMD or DAT[3:0] pin isn't in the idle state !!\r\n");
		return HAL_BUSY;
	}
}

/**
  * @brief  Check if some error occurs when SDIOH transfer.
  * @param  status: pointer to word which is used to save error status.
  * @retval SDIOH status:
  *           - HAL_ERR_UNKNOWN: Some error occurs when transfer, detailed error information 
  *				can be found in status parameter.
  *           - HAL_OK: No error occurs when transfer.
  */
u32 SDIOH_CheckTxError(u16 *status)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	if(psdioh->SD_TRANSFER & SDIOH_ERR_OCCUR) {
		if(status != NULL)
			*status = psdioh->SD_STATUS1 | (psdioh->SD_STATUS2 << 8);

		return HAL_ERR_UNKNOWN;
	} else
		return HAL_OK;
}

/**
  * @brief  Wait some time for SDIOH tx done.
  * @param  timeout_us: timeout value in microseconds.
  * @retval SDIOH tx status value:
  *           - HAL_TIMEOUT: SDIOH tx timeout.
  *           - HAL_OK: SDIOH tx done within a specified time.
  */
u32 SDIOH_WaitTxDone(u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 tmp;

	do {
		tmp = psdioh->SD_TRANSFER & (SDIOH_TRANSFER_END | SDIOH_SD_MODULE_FSM_IDLE);
		if(tmp == (SDIOH_TRANSFER_END | SDIOH_SD_MODULE_FSM_IDLE))
			return HAL_OK;

		DelayUs(1);
	} while (timeout_us-- != 0);
	
	return HAL_TIMEOUT;
}

/**
  * @brief  Wait some time for SDIOH DMA done.
  * @param  timeout_us: timeout value in microseconds.
  * @retval SDIOH dma status:
  *           - HAL_TIMEOUT: SDIOH DMA timeout.
  *           - HAL_OK: SDIOH DMA done within a specified time.
  */
u32 SDIOH_WaitDMADone(u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	
	do {
		if((psdioh->SD_TRANSFER & SDIOH_TRANSFER_END) && (!(psdioh->DMA_CRL3 & SDIOH_DMA_XFER))) {
			SDIOH_DMAReset();
			return HAL_OK;
		}

		DelayUs(1);
	} while (timeout_us-- != 0);
	
	return HAL_TIMEOUT;
}

/**
  * @brief  Get SDIOH interrupt status.
  * @param  timeout_us: None.
  * @retval SDIOH interrupt status.
  */
u32 SDIOH_GetISR(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	return psdioh->SD_ISR;
}

/**
  * @brief  SDIOH interrupt configure.
  * @param  SDIO_IT: SDIOH interrupt source, which can be one or combination of the following values:
  * 		@arg SDIOH_DMA_CTL_INT_EN
  * 		@arg SDIOH_CARD_ERR_INT_EN
  * 		@arg SDIOH_CARD_END_INT_EN
  * @param newState: can be ENABLE or Disable
  * @retval None.
  */
void SDIOH_INTConfig(u8 SDIO_IT, u32 newState)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	if(newState == ENABLE)
		psdioh->SD_ISREN |= SDIO_IT;
	else
		psdioh->SD_ISREN &= ~SDIO_IT;
}

/**
  * @brief  Clear SDIOH pending interrupt status.
  * @param  SDIO_IT: SDIOH interrupt pending bit, which can be one or combination of the following values:
  * 		@arg SDIOH_DMA_TRANSFER_DONE
  * 		@arg SDIOH_CARD_ERROR
  * 		@arg SDIOH_CARD_END
  * @retval None.
  */
void SDIOH_INTClearPendingBit(u8 SDIO_IT)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	psdioh->SD_ISR = SDIO_IT;
}

/**
  * @brief  Check if SDIOH bus switch to a specified state in some time.
  * @param  status: SDIOH bus state, which can be 0 or 1:
  * 		@arg 0: bus switch to low level
  * 		@arg 1: bus switch to high level
  * @param  timeout_us: timeout value in microseconds.
  * @retval None.
  */
u32 SDIOH_CheckBusState(u8 status, u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 idle_level = sdioh_init_para.SDIOH_idle_level;
	u32 tmp;

	do {
		tmp = psdioh->SD_BUS_STATUS & idle_level;
		if(status) {
			if(tmp == idle_level)
				return HAL_OK;
		} else {
			if(tmp == 0)
				return HAL_OK;
		}

		DelayUs(1);
	} while (timeout_us-- != 0);
	
	return HAL_TIMEOUT;
}

/**
  * @brief  Get SDIOH bus width.
  * @param  None.
  * @retval Bus width: 0 or 1:
  *		0: 1-bit bus
  *		1: 4-bit bus
  */
u8 SDIOH_GetBusWidth(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	return psdioh->SD_CONFIG1 & SDIOH_MASK_BUS_WIDTH;
}

/**
  * @brief  Set SDIOH bus width.
  * @param  width: can be SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT.
  * @retval None
  */
void SDIOH_SetBusWidth(u8 width)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 tmp;

	tmp = psdioh->SD_CONFIG1;
	tmp &= ~SDIOH_MASK_BUS_WIDTH;
	tmp |= (width & SDIOH_MASK_BUS_WIDTH);
	
	psdioh->SD_CONFIG1 = tmp;
}

/**
  * @brief  Set SDIOH DMA configuration.
  * @param  dma_ctl: pointer to a SDIOH_DmaCtl structure which keep the dma parameters.
  * @retval None
  */
void SDIOH_DMAConfig(SDIOH_DmaCtl *dma_ctl)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 tmp= 0;

	/* set block length and count */
	if((dma_ctl->type) == SDIOH_DMA_NORMAL) {
		/* 512 bytes (one block size) */
		psdioh->SD_BYTE_CNT_L = 0;
		psdioh->SD_BYTE_CNT_H = 2;
	} else {
		/* 64 bytes( CMD6, R2 response...) */
		psdioh->SD_BYTE_CNT_L = SDIOH_C6R2_BUF_LEN;
		psdioh->SD_BYTE_CNT_H = 0;
	}
	psdioh->SD_BLOCK_CNT_L = (dma_ctl->blk_cnt) & SDIOH_MASK_BLOCL_CNT_L;
	psdioh->SD_BLOCK_CNT_H = ((dma_ctl->blk_cnt) >> 8) & SDIOH_MASK_BLOCL_CNT_H;

	/* set the DMA control register */
	psdioh->DMA_CRL1 = (dma_ctl->start_addr) & SDIOH_MASK_DRAM_SA;	/* DMA start address (unit: 8 Bytes) */
	psdioh->DMA_CRL2 = (dma_ctl->blk_cnt) & SDIOH_MASK_DMA_LEN;		/* DMA transfer length (unit: 512 Bytes) */
	
	if((dma_ctl->type) == SDIOH_DMA_64B)
		tmp = SDIOH_DAT64_SEL;
	else if((dma_ctl->type) == SDIOH_DMA_R2)
		tmp = SDIOH_RSP17_SEL;

	tmp |= (dma_ctl->op << SDIOH_SHIFT_DDR_WR);
	psdioh->DMA_CRL3 = tmp;

	/* clear pending interrupt */
	SDIOH_INTClearPendingBit(SDIOH_DMA_TRANSFER_DONE | SDIOH_CARD_ERROR |SDIOH_CARD_END);

	/* enable DMA transfer */
	tmp = psdioh->DMA_CRL3;
	tmp |= SDIOH_DMA_XFER;
	psdioh->DMA_CRL3 = tmp;
}

/**
  * @brief  Reset SDIOH DMA configuration.
  * @param  None.
  * @retval None
  */
void SDIOH_DMAReset(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	psdioh->SD_BYTE_CNT_L = 0;
	psdioh->SD_BYTE_CNT_H = 0;
	psdioh->SD_BLOCK_CNT_L = 0;
	psdioh->SD_BLOCK_CNT_H = 0;
	psdioh->DMA_CRL1 = 0;
	psdioh->DMA_CRL2 = 0;
	psdioh->DMA_CRL3 = 0;
}

/**
  * @brief  SDIOH send command to card.
  * @param  cmd_attrib: pointer to a SDIOH_CmdTypeDef structure which keeps the command attributes.
  * @param timeout_us: timeout value
  * @retval 0: Send command ok.
  *		  Other values: error occurs
  */
u32 SDIOH_SendCommand(SDIOH_CmdTypeDef *cmd_attrib, u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 cmd = cmd_attrib->idx, cmd_code, tmp;
	u32 val0 = 0, ret;

	ret = SDIOH_Busy();
	if (ret != HAL_OK)
		return ret;

	/* set SD_CONFIGURE2 (0x581) */
	val0 = SDIOH_CRC7_CAL_EN | (cmd_attrib->rsp_type);
	
	if(cmd_attrib->rsp_crc_chk)
		val0 |= SDIOH_CRC7_CHK_EN;
	else
		val0 |= SDIOH_CRC7_CHK_DIS;
	
	if((cmd == SD_CMD_RdSingleBlk) || (cmd == SD_CMD_RdMulBlk) || (cmd == SD_CMD_SendTuningBlk))
		val0 |= SDIOH_CRC16_CHK_EN;
	else
		val0 |= SDIOH_CRC16_CHK_DIS;
	
	if((cmd == SD_CMD_WrBlk) || (cmd == SD_CMD_WrMulBlk))
		val0 |= SDIOH_WAIT_WR_CRCSTA_TO_EN | SDIOH_IGNORE_WR_CRC_ERR_EN;
	else
		val0 |= SDIOH_WAIT_WR_CRCSTA_TO_DIS | SDIOH_IGNORE_WR_CRC_ERR_DIS;
	
	if(cmd == SD_CMD_VolSwitch)
		val0 |= SDIOH_WAIT_BUSY_END_DIS;
	else
		val0 |= SDIOH_WAIT_BUSY_END_EN;

	psdioh->SD_CONFIG2 = val0;

	/* set SD_CONFIGURE3 (0x582) */
	val0 = SDIOH_STOP_STA_WAIT_BUSY_EN | SDIOH_SD30_CLK_STOP_DIS | SDIOH_SD20_CLK_STOP_DIS | \
			SDIOH_SD_CMD_RESP_CHK_DIS | SDIOH_ADDR_MODE_SECTOR;
	
	if((cmd == SD_CMD_RdMulBlk) || (cmd == SD_CMD_WrMulBlk) || (cmd == SD_CMD_StopXsmission))
		val0 |= SDIOH_CMD_STA_WAIT_BUSY_DIS;
	else
		val0 |= SDIOH_CMD_STA_WAIT_BUSY_EN;
	
	if((cmd == SD_CMD_VolSwitch) || (cmd == SD_CMD_StopXsmission))
		val0 |= SDIOH_DATA_PHA_WAIT_BUSY_DIS;
	else
		val0 |= SDIOH_DATA_PHA_WAIT_BUSY_EN;
	
	if((cmd_attrib->rsp_type) == SDIOH_NO_RESP)
		val0 |= SDIOH_CMD_RESP_TO_DIS;
	else
		val0 |= SDIOH_CMD_RESP_TO_EN;

	psdioh->SD_CONFIG3 = val0;

	/* fill the command register */
	psdioh->SD_CMD[0]= HOST_COMMAND | (cmd & 0x3F);
	psdioh->SD_CMD[1] = ((cmd_attrib->arg) >> 24) & 0xFF;
	psdioh->SD_CMD[2] = ((cmd_attrib->arg) >> 16) & 0xFF;
	psdioh->SD_CMD[3] = ((cmd_attrib->arg) >> 8) & 0xFF;
	psdioh->SD_CMD[4] = (cmd_attrib->arg) & 0xFF;
	psdioh->SD_CMD[5] = 0x0;

	/* set the command code */
	switch (cmd) {
		case SD_CMD_SwitchFunc:
		case SD_CMD_SendSts:
			cmd_code = ((cmd_attrib->data_present) == SDIOH_NO_DATA) ? \
					SDIOH_SEND_CMD_GET_RSP : SDIOH_NORMAL_READ;
		break;
		case SD_CMD_SendTuningBlk:
			cmd_code = SDIOH_TUNING;
		break;
		case SD_CMD_RdSingleBlk:
#if defined(SDIO) && (SDIO == EMMC) 
		case EMMC_CMD_SendExtCsd:
#endif 
			cmd_code = SDIOH_AUTO_READ2;
		break;            
		case SD_CMD_RdMulBlk:
			cmd_code = SDIOH_AUTO_READ1;
		break;            
		case SD_CMD_WrBlk:
			cmd_code = SDIOH_AUTO_WRITE2;
		break;
		case SD_CMD_WrMulBlk:
			cmd_code = SDIOH_AUTO_WRITE1;
		break;
		case SD_CMD_SendScr:
			cmd_code = SDIOH_NORMAL_READ;
		break;
		default:
			cmd_code = SDIOH_SEND_CMD_GET_RSP;
	}

	tmp = psdioh->SD_TRANSFER;
	tmp &= ~SDIOH_MASK_COM_CODE;
	tmp |= cmd_code;
	psdioh->SD_TRANSFER = tmp;

	/* start to transfer */
	psdioh->SD_TRANSFER |= SDIOH_START_TRANSFER;

	if(timeout_us != 0)
		ret = SDIOH_WaitTxDone(timeout_us);

	return ret;
}

/**
  * @brief  SDIOH get command response.
  * @param  byte_index: which byte of the response to be read
  * @retval the specified byte value of Response
  */
u8 SDIOH_GetResponse(u8 byte_index)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	return psdioh->SD_CMD[byte_index];
}

/**
  * @brief  Switch speed of SDIOH.
  * @param  clk_div: can be one of the following values:
  *		 @arg SDIOH_CLK_DIV1:  divide by 1
  *		 @arg SDIOH_CLK_DIV2:  divide by 2
  *		 @arg SDIOH_CLK_DIV4:  divide by 4
  *		 @arg SDIOH_CLK_DIV8:  divide by 8
  * @param  mode: mode selection, can be one of the following values:
  *		 @arg SDIOH_SD20_MODE
  * @retval None
  * @note AmebaD only support SD20_MODE
  */
void SDIOH_SwitchSpeed(u8 clk_div, u8 mode)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 value32;
	u8 value8;

	value32 = psdioh->CKGEN_CTL;
	value32 &= ~SDIOH_MASK_CLKDIV;
	value32 |= clk_div;
	psdioh->CKGEN_CTL = value32;

	value8 = psdioh->SD_CONFIG1;
	value8 &= ~SDIOH_MASK_MODE_SEL;
	value8 |= (mode << SDIOH_SHIFT_MODE_SEL);
	psdioh->SD_CONFIG1 = value8;
}

/**
  * @brief  Configure SDIOH to initialization mode or not.
  * @param  NewState: can be ENABLE/DISABLE
  * @param  level: signal level, which can be @arg SDIOH_SIG_VOL_33.
  * @retval  0: SDIOH is configured successfully
  *			others: SDIOH is fail to configure.
  */
u32 SDIOH_InitialModeCmd(u8 NewState, u8 Level)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 ret = HAL_OK;

	ret = SDIOH_Busy();
	if (ret != HAL_OK) {
		return ret;
	}

	if (NewState) {
		psdioh->CKGEN_CTL = SDIOH_SD30_SAMP_CLK_VP1 | SDIOH_SD30_PUSH_CLK_VP0 | \
			SDIOH_CRC_CLK_SSC | SDIOH_CLK_DIV4;

		/* SDCLK (Card Identification mode) : 390.625 KHz */
		psdioh->SD_CONFIG1 = (SDIOH_SD20_MODE << SDIOH_SHIFT_MODE_SEL) | \
			SDIOH_CLK_DIV_BY_128 | SDIOH_INITIAL_MODE | SDIOH_SD30_ASYNC_FIFO_RST_N |\
			(SDIOH_BUS_WIDTH_1BIT << SDIOH_SHIFT_BUS_WIDTH);
	} else {
		/* Switch to DS mode (3.3V) or SDR12 (1.8V). SDCLK (Data Transfer mode) : 25 MHz */
		if(Level == SDIOH_SIG_VOL_18) {
			psdioh->CKGEN_CTL = SDIOH_SD30_SAMP_CLK_VP1 | SDIOH_SD30_PUSH_CLK_VP0 | \
				SDIOH_CRC_CLK_SSC | SDIOH_CLK_DIV4;

			psdioh->SD_CONFIG1 = (SDIOH_SD30_MODE << SDIOH_SHIFT_MODE_SEL) | \
				SDIOH_CLK_DIV_BY_128 | SDIOH_SD30_ASYNC_FIFO_RST_N |\
				(SDIOH_BUS_WIDTH_1BIT << SDIOH_SHIFT_BUS_WIDTH);
		} else {
			psdioh->CKGEN_CTL = SDIOH_SD30_SAMP_CLK_VP1 | SDIOH_SD30_PUSH_CLK_VP0 | \
				SDIOH_CRC_CLK_SSC | (SDIOH_CLK_DIV4);

			psdioh->SD_CONFIG1 = (SDIOH_SD20_MODE << SDIOH_SHIFT_MODE_SEL) | \
				SDIOH_CLK_DIV_BY_128 |(SDIOH_BUS_WIDTH_1BIT << SDIOH_SHIFT_BUS_WIDTH);
		}
	}

	return ret;
}

/**
  * @brief  Initialize SDIOH peripheral.
  * @param  None
  * @retval  0: SDIOH is initialized successfully
  *		   others: SDIOH is fail to initialize
  */
u32 SDIOH_Init (u8 BusBitMode)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 ret = HAL_OK;
	u32 tmp, i = 0;

	/* Set pin idle level */
	if(BusBitMode == SDIOH_BUS_WIDTH_4BIT)
		sdioh_init_para.SDIOH_idle_level = 0x1F;
	else
		sdioh_init_para.SDIOH_idle_level = 0x03;

	/* SDIOH func & clock enable */
	RCC_PeriphClockCmd(APBPeriph_SDIOH, APBPeriph_SDIOH_CLOCK, ENABLE);

	/* Wait for SDIOH clock ready */
	while(1) {
		tmp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HS_SDIO_CTRL);
		if(tmp & BIT_HS_SDIOH_CLK_READY)
			break;

		DelayUs(1);
		if(i++ > 10000) {
			DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "Wait SDIOH clock ready timeout !\r\n");
			return HAL_TIMEOUT;
		}
	}

	/* Enable interrupt */
	psdioh->CARD_INT_EN = SDIOH_SDMMC_INT_EN;

	psdioh->SRAM_CRL = SDIOH_LX_BURST_SIZE_64B | SDIOH_MAP_SEL_DEC;
	psdioh->CARD_SELECT = SDIOH_CARD_SEL_SD_MODULE;
	psdioh->CARD_STOP = SDIOH_TARGET_MODULE_SD;

	/* Initial mode */
	ret = SDIOH_InitialModeCmd(ENABLE, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	psdioh->CARD_CLK_EN_CTL = SDIOH_SD_CARD_MOUDLE_EN;
	
	return ret;
}

/**
  * @brief  De-initialize SDIOH peripheral.
  * @param  None
  * @retval  None
  */
void SDIOH_DeInit (void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	/* disable interrupt & clear all pending interrupts */
	psdioh->CARD_INT_PEND |= SDIOH_SDMMC_INT_PEND;
	psdioh->CARD_INT_EN &= ~ SDIOH_SDMMC_INT_EN;
	SDIOH_INTClearPendingBit(SDIOH_SD_ISR_ALL);
	SDIOH_INTConfig(SDIOH_SD_ISR_ALL, DISABLE);
}

