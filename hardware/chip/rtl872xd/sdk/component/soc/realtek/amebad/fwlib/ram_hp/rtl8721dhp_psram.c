/**
  ******************************************************************************
  * @file    rtl8721dhp_psram.c
  * @author
  * @version V1.0.0
  * @date    2017-09-17
  * @brief   This file contains all the functions prototypes for the psram firmware
  *             library, including the following functionalities of the PSRAM CTRL:
  *           -Initialization
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

#define PSRAM_INIT_LATENCY_MIN		3
#define PSRAM_INIT_LATENCY_CLK		3
#define PSRAM_CALIB_N_MAX			32
static u8 winbond_init_latency_spec[4] = {
	PSRAM_INIT_LATENCY_3CLK,
	PSRAM_INIT_LATENCY_4CLK,
	PSRAM_INIT_LATENCY_5CLK,
	PSRAM_INIT_LATENCY_6CLK,
}; 

static u32 PSRAM_CALIB_PATTERN[6] = {
	0x11223344,
	0xA55AA55A,
	0x5AA55AA5,
	0x44332211,
	0x96696996,
	0x69969669,
};
/**
  * @brief  Fills each PSRAM_InitStruct member with its default value.
  * @param  PSRAM_InitStruct: pointer to an PSRAM_InitTypeDef structure which will be initialized.
  * @retval None
  */
void PSRAM_CTRL_StructInit(PCTL_InitTypeDef *PCTL_InitStruct)
{
	PCTL_InitStruct->PCTL_dfi_cs_wr_dly = 1;
	PCTL_InitStruct->PCTL_dfi_cs_rd_dly = 1;
	PCTL_InitStruct->PCTL_tphy_wrdata = (2 + PSRAM_INIT_LATENCY_CLK);
	PCTL_InitStruct->PCTL_fix_tphy_lat = PSRAM_UNSET_TPHY_LATENCY;
	PCTL_InitStruct->PCTL_tphy_rddata = (3 + PSRAM_INIT_LATENCY_CLK);
	PCTL_InitStruct->PCTL_dfi_path_dly = PSRAM_INIT_LATENCY_CLK;

	PCTL_InitStruct->PCTL_tcph_ps = 15000;
	PCTL_InitStruct->PCTL_tpu_ps = 150000000;
	PCTL_InitStruct->PCTL_tcem_ps = 2000000;
	PCTL_InitStruct->PCTL_clk_ps = 20000;

	PCTL_InitStruct->PCTL_wl = PSRAM_INIT_LATENCY_CLK;
	PCTL_InitStruct->PCTL_rl = PSRAM_INIT_LATENCY_CLK;

	PCTL_InitStruct->PCTL_mr0_burst_len = PSRAM_BURST_LENGTH_128B;
	PCTL_InitStruct->PCTL_mr0_burst_type = PSRAM_BURST_LEGACY_TYPE;
	PCTL_InitStruct->PCTL_mr0_lat_mode = PSRAM_VARIABLE_LATENCY;
	PCTL_InitStruct->PCTL_mr0_init_lat = winbond_init_latency_spec[PSRAM_INIT_LATENCY_CLK - PSRAM_INIT_LATENCY_MIN];
	PCTL_InitStruct->PCTL_mr0_drv_strength = PSRAM_DRV_STRENGTH_50OHMS;
	PCTL_InitStruct->PCTL_mr0_dpd_en = PSRAM_NORMAL_MODE;
	PCTL_InitStruct->PCTL_mr1_pasr = PSRAM_PASR_FULL_ARRAY;
	PCTL_InitStruct->PCTL_mr1_half_slp = PSRAM_HALF_SLP_DIS;
	PCTL_InitStruct->PCTL_mr1_refresh_rate = PSRAM_REFRESH_RATE_FAST;
}

/**
  * @brief Initializes the PSRAM controller according to the specified
  *   parameters in the PSRAM_InitStruct
  * @param  PSRAM_InitStruct: pointer to a PSRAM_InitTypeDef structure that
  *   contains the configuration information for the specified PSRAM controller
  * @retval None
  */
void PSRAM_CTRL_Init(PCTL_InitTypeDef *PCTL_InitStruct)
{
	PCTL_TypeDef *psram_ctrl = PSRAM_DEV;
	u32 temp;

	/* enter sleep mode before power on to avoid current crush */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1);
	temp |= BIT_LDO_PSRAM_SLEEP_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1, temp);
	
	/*Open PSRAM 1.8V power */
	/* 0111: 1.800V  */
	/* 1000: 1.815V  */
	/* 1001: 1.830V  */
	/* 1010: 1.846V  */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1);
	temp &= ~(BIT_MASK_LDO_PSRAM_ADJ << BIT_SHIFT_LDO_PSRAM_ADJ);
	temp |= (0x07 << BIT_SHIFT_LDO_PSRAM_ADJ); /* 1.800V */
	temp |= BIT_LDO_PSRAM_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1, temp);	

	RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);

	/* 300us for a stable voltage */
	/* exit sleep mode before access psram for a stable current */
	DelayUs(300);
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1);
	temp &= ~BIT_LDO_PSRAM_SLEEP_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1, temp);

	/* Check the parameters */
	assert_param(IS_PSRAM_ALL_PERIPH(psram_ctrl));

	/*Disable memory access*/
	psram_ctrl->CSR |= BIT_PSRAM_MEM_IDLE;
	/*Wait memory state idle*/
	while(!(psram_ctrl->CSR & BIT_PSRAM_MEM_IDLE));

	temp = psram_ctrl->IOCR0;
	temp &=(~(BIT_PSRAM_DFI_CS_WR_DLY_MASK | BIT_PSRAM_DFI_CS_RD_DLY_MASK |\
			BIT_PSRAM_TPHY_WRDATA_MASK | BIT_PSRAM_TPHY_RDDATA_EN_MASK |\
			BIT_PSRAM_FIX_TPHY_LAT_MASK | BIT_PSRAM_DFI_PATH_DLY_MASK));
	temp |= ((PCTL_InitStruct->PCTL_dfi_cs_wr_dly << BIT_PSRAM_DFI_CS_WR_DLY_SHIFT) |\
			(PCTL_InitStruct->PCTL_dfi_cs_rd_dly << BIT_PSRAM_DFI_CS_RD_DLY_SHIFT) |\
			(PCTL_InitStruct->PCTL_tphy_wrdata << BIT_PSRAM_TPHY_WRDATA_SHIFT) |\
			(PCTL_InitStruct->PCTL_fix_tphy_lat << BIT_PSRAM_FIX_TPHY_LAT_SHIFT) |\
			(PCTL_InitStruct->PCTL_tphy_rddata << BIT_PSRAM_TPHY_RDDATA_EN_SHIFT) |\
			(PCTL_InitStruct->PCTL_dfi_path_dly << BIT_PSRAM_DFI_PATH_DLY_SHIFT));
	psram_ctrl->IOCR0 = temp;
	psram_ctrl->DRR = ((((PCTL_InitStruct->PCTL_tpu_ps/(PCTL_InitStruct->PCTL_clk_ps*1000) + 1)) << BIT_PSRAM_PU_TIME_SHIFT) |\
				((PCTL_InitStruct->PCTL_tcem_ps/PCTL_InitStruct->PCTL_clk_ps) << BIT_PSRAM_CEM_TIME_SHIFT) |\
				(((PCTL_InitStruct->PCTL_tcph_ps/PCTL_InitStruct->PCTL_clk_ps) + 1) << BIT_PSRAM_CPH_TIME_SHIFT));

	psram_ctrl->MR_INFO = ((PCTL_InitStruct->PCTL_rl << BIT_PSRAM_RL_LATENCY_SHIFT) |\
					(PCTL_InitStruct->PCTL_wl << BIT_PSRAM_WL_LATENCY_SHIFT));

	temp = psram_ctrl->MR0;
	temp &= (BIT_PSRAM_MR0_BURST_RSVD_MASK << BIT_PSRAM_MR0_BURST_RSVD_SHIFT);
	temp |= ((PCTL_InitStruct->PCTL_mr0_burst_len << BIT_PSRAM_MR0_BURST_LENGTH_SHIFT) |\
				(PCTL_InitStruct->PCTL_mr0_burst_type << BIT_PSRAM_MR0_BURST_TYPE_SHIFT) |\
				(PCTL_InitStruct->PCTL_mr0_lat_mode << BIT_PSRAM_MR0_BURST_LAT_MODE_SHIFT) |\
				(PCTL_InitStruct->PCTL_mr0_init_lat << BIT_PSRAM_MR0_BURST_INIT_LAT_SHIFT) |\
				(PCTL_InitStruct->PCTL_mr0_drv_strength << BIT_PSRAM_MR0_BURST_DRV_STRENGTH_SHIFT) |\
				(PCTL_InitStruct->PCTL_mr0_dpd_en << BIT_PSRAM_MR0_BURST_DPD_MODE_SHIFT));
	psram_ctrl->MR0 = temp;

	psram_ctrl->MR1 = ((PCTL_InitStruct->PCTL_mr1_pasr << BIT_PSRAM_MR1_PASR_SHIFT) |\
				(PCTL_InitStruct->PCTL_mr1_half_slp << BIT_PSRAM_MR1_HALF_SLP_MODE_SHIFT) |\
				(PCTL_InitStruct->PCTL_mr1_refresh_rate << BIT_PSRAM_MR1_REFRESH_RATE_SHIFT) );

	/*Enable PSRAM init function*/
	psram_ctrl->CCR = BIT_PSRAM_INIT;

	/*Wait PSRAM init function down*/
	while(!(psram_ctrl->CCR & BIT_PSRAM_INIT));

	/*Enable memory access*/
	psram_ctrl->CSR &= (~BIT_PSRAM_MEM_IDLE);
	/*Wait memory access allowed*/
	while(psram_ctrl->CSR & BIT_PSRAM_MEM_IDLE);
}

/**
  * @brief Generate PSRAM command address value for dpin mode
  * @param  PSRAM_CA: pointer to the CA array to be generated
  * @param  StartAddr: Specify the target address 
  * @param  BurstType: Indicate the burst transmission mode
  *			@arg PSRAM_WRAPPED_TYPE: indicate wrapper burst.
  *			@arg PSRAM_LINEAR_TYPE: indicate linear burst.
  * @param  AddSpace: Indicate access the memory or register space
  *			@arg PSRAM_MEM_SPACE: indicate memory space.
  *			@arg PSRAM_REG_SPACE: indicate register space.
  * @param  RW: Indicate the transaction mode
  *			@arg PSRAM_WRITE_TRANSACTION: indicate a write transaction.
  *			@arg PSRAM_READ_TRANSACTION: indicate a read transaction.
  * @retval None
  */
void PSRAM_CTRL_CA_Gen(u8* PSRAM_CA, u32 StartAddr, u8 BurstType, u8 AddSpace, u8 RW)
{
	assert_param(IS_PSRAM_BURST_TYPE(BurstType));
	assert_param(IS_PSRAM_ADDR_SPACE(AddSpace));
	assert_param(IS_PSRAM_WR_TRANSACTION(RW));
	
	PSRAM_CA[0] = StartAddr & 0x7;
	PSRAM_CA[1] = 0;
	PSRAM_CA[2] = (StartAddr >> 3) & 0xf;
	PSRAM_CA[3] = (StartAddr >> 11) & 0xf;
	PSRAM_CA[4] = (StartAddr >> 19) & 0xf;
	PSRAM_CA[5] = ((StartAddr >> 27) & 0xf) | (BurstType << 5) | (AddSpace << 6) | (RW << 7);
}

/**
  * @brief Access PSRAM memory by dpin mode
  * @param  PSRAM_CA: pointer to the command address array
  * @param  PSRAM_data: pointer to the data read from or write to
  * @param  PSRAM_byteen: Indicate the byte mask when write
  * @param  RW: Indicate the transaction mode
  *			@arg PSRAM_WRITE_TRANSACTION: indicate a write transaction.
  *			@arg PSRAM_READ_TRANSACTION: indicate a read transaction.
  * @retval None
  * @note cache should be disable in Dpin mode
  */
void PSRAM_CTRL_DPin_Mem(u8* PSRAM_CA, u32* PSRAM_data, u32 PSRAM_byteen, u8 RW)
{
	PCTL_TypeDef *psram_ctrl = PSRAM_DEV;
	assert_param(IS_PSRAM_WR_TRANSACTION(RW));
	/*Disable memory access*/
	psram_ctrl->CSR |= BIT_PSRAM_MEM_IDLE;
	/*Wait memory state idle*/
	while(!(psram_ctrl->CSR & BIT_PSRAM_MEM_IDLE));

	psram_ctrl->CMD_DPIN_NDGE = (PSRAM_CA[0] | (PSRAM_CA[2] << 8) | (PSRAM_CA[4] << 16));
	psram_ctrl->CMD_DPIN = (PSRAM_CA[1] | (PSRAM_CA[3] << 8) | (PSRAM_CA[5] << 16));

	if(PSRAM_WRITE_TRANSACTION == RW) {
		/*Set dpin write data mode*/
		psram_ctrl->CSR &= (~BIT_PSRAM_DPIN_MODE);
		psram_ctrl->CSR |= PSRAM_DPIN_WRITE_MODE;
		
		/*set dpin write data mask*/
		psram_ctrl->DPDRI = 8;
		psram_ctrl->DPDR = PSRAM_byteen;
		/*set dpin write data*/
		psram_ctrl->DPDRI = 4;
		psram_ctrl->DPDR = *PSRAM_data;
	} else {
		/*Set dpin read data mode*/
		psram_ctrl->CSR &= (~BIT_PSRAM_DPIN_MODE);
		psram_ctrl->CSR |= PSRAM_DPIN_READ_MODE;
	}

	/*start dpin function*/
	psram_ctrl->CCR = BIT_PSRAM_DPIN;
	/*Wait dpin function down*/
	while(!(psram_ctrl->CCR & BIT_PSRAM_DPIN));

	if(PSRAM_READ_TRANSACTION == RW) {
		/*get dpin read data*/
		psram_ctrl->DPDRI = 0;
		*PSRAM_data = psram_ctrl->DPDR;
	}

	/*Enable memory access*/
	psram_ctrl->CSR &= (~BIT_PSRAM_MEM_IDLE);
	/*Wait memory access*/
	while(psram_ctrl->CSR & BIT_PSRAM_MEM_IDLE);
}

/**
  * @brief Access PSRAM register by dpin mode
  * @param  PSRAM_CA: pointer to the command address array
  * @param  PSRAM_data: pointer to the data read from or write to
  * @param  RW: Indicate the transaction mode
  *			@arg PSRAM_WRITE_TRANSACTION: indicate a write transaction.
  *			@arg PSRAM_READ_TRANSACTION: indicate a read transaction.
  * @retval None
  * @note cache should be disable in Dpin mode
  * @note Dpin register write is in little-endian
  */
void PSRAM_CTRL_DPin_Reg(u8* PSRAM_CA, u32* PSRAM_data, u8 RW)
{
	PCTL_TypeDef *psram_ctrl = PSRAM_DEV;
	u32 temp;
	u32 tphy_wrdata_backup;
	
	assert_param(IS_PSRAM_WR_TRANSACTION(RW));
	/*Disable memory access*/
	psram_ctrl->CSR |= BIT_PSRAM_MEM_IDLE;
	/*Wait memory state idle*/
	while(!(psram_ctrl->CSR & BIT_PSRAM_MEM_IDLE));

	if(PSRAM_WRITE_TRANSACTION == RW) {

		/*set reg write latency*/
		temp = psram_ctrl->IOCR0;
		tphy_wrdata_backup = (temp & BIT_PSRAM_TPHY_WRDATA_MASK) >> BIT_PSRAM_TPHY_WRDATA_SHIFT;
		temp &= (~BIT_PSRAM_TPHY_WRDATA_MASK);
		temp |= (((tphy_wrdata_backup - 2) << BIT_PSRAM_TPHY_WRDATA_SHIFT) | BIT_PSRAM_FIX_TPHY_LAT_MASK);
		psram_ctrl->IOCR0 = temp;
		//DBG_8195A("PSRAM->IOCR0 = %x\n",PSRAM->IOCR0);

		/*set dpin write data*/
		psram_ctrl->DPDRI = 4;
		psram_ctrl->DPDR = *PSRAM_data;

		/*set dpin write data*/
		psram_ctrl->DPDRI = 8;
		psram_ctrl->DPDR = 0xF;

		/*Set dpin write data mode*/
		psram_ctrl->CSR &= (~BIT_PSRAM_DPIN_MODE);
		psram_ctrl->CSR |= PSRAM_DPIN_WRITE_MODE;
	} else {
		/*Set dpin read data mode*/
		psram_ctrl->CSR &= (~BIT_PSRAM_DPIN_MODE);
		psram_ctrl->CSR |= PSRAM_DPIN_READ_MODE;
	}

	psram_ctrl->CMD_DPIN_NDGE = (PSRAM_CA[0] | (PSRAM_CA[2] << 8) | (PSRAM_CA[4] << 16));
	psram_ctrl->CMD_DPIN = (PSRAM_CA[1] | (PSRAM_CA[3] << 8) | (PSRAM_CA[5] << 16));

	/*start dpin function*/
	psram_ctrl->CCR = BIT_PSRAM_DPIN;
	/*Wait dpin function down*/
	while(!(psram_ctrl->CCR & BIT_PSRAM_DPIN));

	if(PSRAM_WRITE_TRANSACTION == RW) {
		/*backup latency*/
		temp = psram_ctrl->IOCR0;
		temp &= (~(BIT_PSRAM_TPHY_WRDATA_MASK | BIT_PSRAM_FIX_TPHY_LAT_MASK));
		temp |= (tphy_wrdata_backup << BIT_PSRAM_TPHY_WRDATA_SHIFT);
		psram_ctrl->IOCR0 = temp;
	} else {
		/*get dpin read data*/
		psram_ctrl->DPDRI = 0;
		*PSRAM_data = psram_ctrl->DPDR;
	}

	/*Enable memory access*/
	psram_ctrl->CSR &= (~BIT_PSRAM_MEM_IDLE);
	/*Wait memory access*/
	while(psram_ctrl->CSR & BIT_PSRAM_MEM_IDLE);
}

/**
  * @brief Get PSRAM phy register value 
  * @param  offset: phy register address
  * @retval The phy register value
  */
u32 PSRAM_PHY_REG_Read(u8 offset)
{
	PCTL_TypeDef *psram_ctrl = PSRAM_DEV;
	
	/*set phy reg offset*/
	psram_ctrl->USER0_INDEX = offset;
	/*get phy reg value*/
	return psram_ctrl->USER0_DATA;
}

/**
  * @brief Set PSRAM phy register value
  * @param  offset: phy register address
  * @param  reg_val: the value to be set
  * @retval None
  */
void PSRAM_PHY_REG_Write(u8 offset, u32 reg_val)
{
	PCTL_TypeDef *psram_ctrl = PSRAM_DEV;
	
	/*set phy reg offset*/
	psram_ctrl->USER0_INDEX = offset;
	/*set phy reg value*/
	psram_ctrl->USER0_DATA = reg_val;
}

/**
  * @brief PSRAM FW calibration function
  * @retval None
  * @note cache will be disable during calibration
  */
BOOL PSRAM_calibration(VOID)
{
	u32 caltempN = 0;
	u32 tempPHYPara = (PSRAM_PHY_REG_Read(REG_PSRAM_CAL_PARA) & (~BIT_PSRAM_CFG_CAL_N_MASK));
	u32 tempPHYC = PSRAM_PHY_REG_Read(REG_PSRAM_CAL_CTRL);
	u32 *tempdatawr = PSRAM_CALIB_PATTERN;
	u32 tempdatard[6] = {0,0,0,0,0,0};
	int window_start = -1;
	int window_size = 0;
	int window_end = -1;
	int windowt_start = -1;
	int windowt_size = 0;
	int windowt_end = -1;

	/*Disable HW calibration*/
	PSRAM_PHY_REG_Write(REG_PSRAM_CAL_CTRL, (tempPHYC & (~BIT_PSRAM_CFG_CAL_EN)));

	for(caltempN = 0; caltempN < PSRAM_CALIB_N_MAX; caltempN++){

		PSRAM_PHY_REG_Write(REG_PSRAM_CAL_PARA,(tempPHYPara | caltempN));

		HAL_WRITE32(PSRAM_BASE, 0x0, tempdatawr[0]);
		HAL_WRITE32(PSRAM_BASE, 0x50000, tempdatawr[1]);
		HAL_WRITE32(PSRAM_BASE, 0x100000, tempdatawr[2]);
		HAL_WRITE32(PSRAM_BASE, 0x150000, tempdatawr[3]);
		HAL_WRITE32(PSRAM_BASE, 0x200000, tempdatawr[4]);
		HAL_WRITE32(PSRAM_BASE, 0x250000, tempdatawr[5]);

		DCache_CleanInvalidate(PSRAM_BASE, 0x400000);

		tempdatard[0] = HAL_READ32(PSRAM_BASE, 0x0);
		tempdatard[1] = HAL_READ32(PSRAM_BASE, 0x50000);
		tempdatard[2] = HAL_READ32(PSRAM_BASE, 0x100000);
		tempdatard[3] = HAL_READ32(PSRAM_BASE, 0x150000);
		tempdatard[4] = HAL_READ32(PSRAM_BASE, 0x200000);
		tempdatard[5] = HAL_READ32(PSRAM_BASE, 0x250000);

		if(_memcmp(tempdatard, PSRAM_CALIB_PATTERN, 24) == 0) {
			//DBG_8195A("ok %x %x %x %x %x %x\n",tempdatard[0],tempdatard[1],tempdatard[2],tempdatard[3],tempdatard[4],tempdatard[5]);
			if(windowt_start < 0) {
				windowt_start = caltempN;
			}
			windowt_end = windowt_start + windowt_size;
			windowt_size ++;

			if(caltempN == (PSRAM_CALIB_N_MAX - 1)) {
				if(windowt_size > window_size) {
					window_start = windowt_start;
					window_end = windowt_end;
					window_size = windowt_size;
				}
			}
		} else {
			//DBG_8195A("fail %x %x %x %x %x %x\n",tempdatard[0],tempdatard[1],tempdatard[2],tempdatard[3],tempdatard[4],tempdatard[5]);
			if(windowt_start >= 0) {
				if(windowt_size > window_size) {
					window_start = windowt_start;
					window_end = windowt_end;
					window_size = windowt_size;
				}
					windowt_start = -1;
					windowt_size = 0;
					windowt_end = -1;
			}
		}
	}

	//DBG_8195A("CalNmin = %x CalNmax = %x WindowSize = %x \n", window_start, window_end, window_size);

	if(window_size < 9) {
		DBG_8195A("PSRAM Calibration fail\n");
		return _FALSE;
	}

	tempPHYPara &= (~0xfffff);
	tempPHYPara |= ((((window_end - window_start)/2 - 2) << BIT_PSRAM_CFG_CAL_JMAX_SHIFT) |\
		(((window_end - window_start)/2 - 2) << BIT_PSRAM_CFG_CAL_J_SHIFT) |\
		((window_end + window_start)/2) << BIT_PSRAM_CFG_CAL_N_SHIFT);
	
	/*set N/J initial value HW calibration*/
	PSRAM_PHY_REG_Write(REG_PSRAM_CAL_PARA, tempPHYPara);

	/*start HW calibration*/
	PSRAM_PHY_REG_Write(REG_PSRAM_CAL_CTRL, (tempPHYC | BIT_PSRAM_CFG_CAL_EN));

	return _TRUE;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/
