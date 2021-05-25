/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

static FlashInfo_TypeDef *current_IC;

/* Flag to check configuration register or not. Necessary for wide-range VCC MXIC flash */
static u8 check_config_reg = 0;

extern FlashInfo_TypeDef Flash_AVL[];
extern u16 Flash_ReadMode;
extern u16 Flash_Speed;


IMAGE2_RAM_TEXT_SECTION
BOOL _flash_calibration_highspeed(u8 SpicBitMode, u8 div)
{
	u32 window_size = 0;
	u32 phase_shift_idx = 0;
	u32 line_delay = 0;
	u32 line_delay_temp = 0;
	u32 window_temp = 0;
	u32 window_size_temp = 0;
	//u32 window_start_temp = 0;
	//u32 window_end_temp = 0;

	/* init last point before calibration */
	FLASH_CalibrationInit(30);

	/* 4.808 ms take when enter this function */
	/* 511us will take for flash full calibration */
	for (line_delay_temp = 1; line_delay_temp <= 5; line_delay_temp++) {
		window_temp = FLASH_CalibrationNew(&flash_init_para, SpicBitMode, div, 2, line_delay_temp, 1);

		window_size_temp = (window_temp & 0xFF);
		//window_start_temp = ((window_temp >> 16) & 0xFF);
		//window_end_temp = ((window_temp >> 24) & 0xFF);

		if (flash_init_para.phase_shift_idx != 0) {
			flash_init_para.phase_shift_idx |= BIT(0); /* odd is better */
		}
		
		//DBG_8195A("calibration_result:[%d:%d:%d][%x:%x] \n", line_delay_temp, window_size_temp, flash_init_para.phase_shift_idx,
		//	window_start_temp, window_end_temp);
		//BKUP_Write(BKUP_REG3, ((window_start_temp << 24) | (line_delay_temp << 16) | (window_size_temp << 8) | flash_init_para.phase_shift_idx));
		
		if (window_size_temp > window_size) {
			window_size = window_size_temp;
			line_delay = line_delay_temp;
			phase_shift_idx = flash_init_para.phase_shift_idx;
		}

		flash_init_para.phase_shift_idx = 0;
	}
	if (window_size > 0) {
		DBG_8195A("calibration_ok:[%d:%d:%d] \n", line_delay, window_size, phase_shift_idx);

		flash_init_para.phase_shift_idx = phase_shift_idx;
		flash_init_para.FLASH_rd_sample_phase_cal = line_delay;
		flash_init_para.FLASH_rd_sample_phase = flash_init_para.FLASH_rd_sample_phase_cal;

		return _TRUE;
	} else {
		flash_init_para.phase_shift_idx = 0;
		flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
	}

	return _FALSE;
}

void flash_calibration_backup(u8 flash_speed, u8 read_mode)
{
	RRAM_TypeDef* RRAM = ((RRAM_TypeDef *) RRAM_BASE);

	DBG_8195A("RRAM: %x %dB \n", RRAM_BASE, sizeof(RRAM_TypeDef));

	RRAM->FLASH_ClockDiv = flash_speed;
	RRAM->FLASH_ReadMode = read_mode;
	RRAM->FLASH_phase_shift_idx = flash_init_para.phase_shift_idx;
	RRAM->FLASH_rd_sample_phase_cal = flash_init_para.FLASH_rd_sample_phase;
	RRAM->FLASH_cur_bitmode = flash_init_para.FLASH_cur_bitmode;

	RRAM->FLASH_cur_cmd = flash_init_para.FLASH_cur_cmd;
	RRAM->FALSH_quad_valid_cmd = flash_init_para.FALSH_quad_valid_cmd;
	RRAM->FALSH_dual_valid_cmd = flash_init_para.FALSH_dual_valid_cmd;
	RRAM->FLASH_pseudo_prm_en = flash_init_para.FLASH_pseudo_prm_en;
	RRAM->FLASH_addr_phase_len = flash_init_para.FLASH_addr_phase_len;
	RRAM->FLASH_cmd_wr_status2 = flash_init_para.FLASH_cmd_wr_status2;
	RRAM->FLASH_QuadEn_bit = flash_init_para.FLASH_QuadEn_bit;
	
	RRAM->FLASH_rd_dummy_cyle0 = flash_init_para.FLASH_rd_dummy_cyle[0];
	RRAM->FLASH_rd_dummy_cyle1 = flash_init_para.FLASH_rd_dummy_cyle[1];
	RRAM->FLASH_rd_dummy_cyle2 = flash_init_para.FLASH_rd_dummy_cyle[2];
}

IMAGE2_RAM_TEXT_SECTION
u32 flash_calibration_highspeed(u8 div)
{
	u32 Ret = _SUCCESS;
	u8 spic_mode = flash_init_para.FLASH_cur_bitmode;


	flash_init_para.debug = 0;
	
	/* SPIC clock switch to PLL */
	FLASH_ClockDiv(div);
	
	if (_flash_calibration_highspeed(spic_mode, div) == _TRUE) {
		/* we should open calibration new first, and then set phase index */
		FLASH_CalibrationNewCmd(ENABLE);
		FLASH_CalibrationPhaseIdx(flash_init_para.phase_shift_idx);

		/* this code is rom code, so it is safe */
		FLASH_Init(spic_mode);

		DBG_8195A("FLASH CALIB[NEW OK]\n");
	} else {
		/* calibration fail, revert SPIC clock to XTAL */
		RCC_PeriphClockSource_SPIC(BIT_SHIFT_FLASH_CLK_XTAL);

		DBG_8195A("FLASH CALIB[NEW FAIL]\n");		
		
		Ret = _FAIL;
	}


	return Ret;
}

static u8 flash_get_option(u32 sys_data, BOOL is_speed)
{
	u16 tmp = 0x8000;
	u8 cnt = 0;
	
	while(tmp){
		if((sys_data & tmp) != 0){
			break;
		}else{
			tmp = tmp >> 1;
			cnt++;
		}
	}

	if(is_speed) {
		if(cnt == 0)
			cnt = 1;
		else if(cnt == 1)
			cnt = 0;
		else if(cnt > 4)
			cnt = 4;
	}
	
	return cnt;
}

static FlashInfo_TypeDef *flash_get_chip_info(u32 flash_id)
{
	u32 i = 0;
	u32 temp;

	while(Flash_AVL[i].flash_class != FlashClassNone) {
		temp = flash_id & Flash_AVL[i].id_mask;
		if(Flash_AVL[i].flash_id == temp)
			return &Flash_AVL[i];
		
		i++;
	}

	return NULL;
}

static void flash_get_vendor(void)
{
	RRAM_TypeDef* RRAM = ((RRAM_TypeDef *) RRAM_BASE);
	u8 flash_ID[4];

	/* Read flash ID */
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, flash_ID);

	DBG_8195A("Flash ID:%x, %x, %x\n", flash_ID[0], flash_ID[1], flash_ID[2]);

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) |(flash_ID[1] << 8) |flash_ID[0]);
	if(current_IC == NULL) {
		DBG_8195A("This flash type is not supported!\n");
		assert_param(0);
	}
	RRAM->FLASH_class = current_IC->flash_class;
	RRAM->FLASH_ID2 = flash_ID[2];

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
		case FlashClass1: 
			FLASH_StructInit(&flash_init_para);
			RRAM->FLASH_StructInit = (u32)FLASH_StructInit;
		break;
		case FlashClass2:
			FLASH_StructInit_GD(&flash_init_para);
			if(flash_ID[2] > 0x15) /* GD capacity more than 2MB */
				flash_init_para.FLASH_cmd_wr_status2 = 0x31;
			RRAM->FLASH_StructInit = (u32)FLASH_StructInit_GD;
		break;
		case FlashClass3:
			FLASH_StructInit_MXIC(&flash_init_para);
			RRAM->FLASH_StructInit = (u32)FLASH_StructInit_MXIC;
		break;
		case FlashClass4:	/* EON without QE bit */
			FLASH_StructInit_MXIC(&flash_init_para);
			flash_init_para.FLASH_QuadEn_bit = 0; 
			RRAM->FLASH_StructInit = (u32)FLASH_StructInit_MXIC;
		break;
		case FlashClass5:
			FLASH_StructInit_Micron(&flash_init_para);
			RRAM->FLASH_StructInit = (u32)FLASH_StructInit_Micron;
		break;
		case FlashClass6:	/* MXIC wide-range VCC chip */
			FLASH_StructInit_MXIC(&flash_init_para);
			check_config_reg = 1;
			RRAM->FLASH_StructInit = (u32)FLASH_StructInit_MXIC;
		break;
		case FlashClassUser:
			assert_param(current_IC->FlashInitHandler != NULL);
			current_IC->FlashInitHandler();
			RRAM->FLASH_StructInit = (u32)NULL;
		break;
		default:
		break;
	}

	if(HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_SPIC_ADDR_4BYTE_EN)
		flash_init_para.FLASH_addr_phase_len = ADDR_4_BYTE;

}

static void flash_set_status_register(void)
{
	u8 StatusLen = 1;
	u32 data = 0;
	u32 status;
	u32 mask = current_IC->sta_mask;

	if (flash_init_para.FLASH_QuadEn_bit !=0)
		data |= flash_init_para.FLASH_QuadEn_bit;

	/* read status1 register */
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status, 1, (u8*)&status);
	
	/* check if status2 exist */
	if (flash_init_para.FLASH_Status2_exist) {
		StatusLen = 2;
		FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status2, 1, ((u8*)&status) + 1);	
		
	} else if (check_config_reg) {	/* for MXIC wide-range flash, 1 status register + 2 config register */
		/* Read configuration register */
		FLASH_RxCmd(0x15, 2, ((u8*)&status) + 1);
		StatusLen = 3;
		
		/* L/H Switch */
		data |= (BIT(9) << 8);
	}

	status &= mask;	
	if(_memcmp((void*)&status, (void*)&data, StatusLen)) {
		if(!flash_init_para.FLASH_cmd_wr_status2) {
			FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, StatusLen, (u8*)&data);
		} else {
			FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, 1, (u8*)&data);
			FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status2, 1, ((u8*)&data) + 1);
		}
	}
}

IMAGE2_RAM_TEXT_SECTION
u32 flash_rx_mode_switch(u8 read_mode)
{
	u32 Ret = _SUCCESS;
	u8 tmp_dc = 0, status = 0, spic_mode = 0, i;
	u32 pdata[2];

	for(i = read_mode; i < 5; i++) {
		if(i == ReadQuadIOMode){
			flash_init_para.FLASH_pseudo_prm_en = 1;
			flash_init_para.FALSH_quad_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_QUAD_IO | BIT_PRM_EN);

			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[2];
			flash_init_para.FLASH_rd_dummy_cyle[2] -= QUAD_PRM_CYCLE_NUM;

			spic_mode = SpicQuadBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_4READ;
			
		}else if(i == ReadQuadOMode){
			flash_init_para.FLASH_pseudo_prm_en = 0;
			flash_init_para.FALSH_quad_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_QUAD_O);

			flash_init_para.FLASH_rd_dummy_cyle[2] = FLASH_DM_CYCLE_4O;
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[2];

			spic_mode = SpicQuadBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_QREAD;
			
		}else if(i == ReadDualIOMode){
			flash_init_para.FLASH_pseudo_prm_en = 1;
			flash_init_para.FALSH_dual_valid_cmd = (BIT_WR_BLOCKING |BIT_FRD_SINGEL  | BIT_CTRLR0_CH | BIT_PRM_EN);

			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[1];
			flash_init_para.FLASH_rd_dummy_cyle[1] -= DUAL_PRM_CYCLE_NUM;

			spic_mode = SpicDualBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_2READ;
			
		}else if(i == ReadDualOMode){
			flash_init_para.FLASH_pseudo_prm_en = 0;
			flash_init_para.FALSH_dual_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_DUAL_I);

			flash_init_para.FLASH_rd_dummy_cyle[1] = FLASH_DM_CYCLE_2O;
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[1];

			spic_mode = SpicDualBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_DREAD;
		}else{
			flash_init_para.FLASH_pseudo_prm_en = 0;

			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[0];
			spic_mode = SpicOneBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_READ;
		}

		if(flash_init_para.FLASH_Id == FLASH_ID_MICRON){
			FLASH_RxCmd(0x85, 1, &status);

			status = (status & 0x0f) | (tmp_dc << 4);
			FLASH_SetStatus(0x81, 1, &status);
		}

		flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
		FLASH_Init(spic_mode);

		DCache_Invalidate(SPI_FLASH_BASE, 8);
		pdata[0] = HAL_READ32(SPI_FLASH_BASE, 0x00);
		pdata[1] = HAL_READ32(SPI_FLASH_BASE, 0x04);

		if(_memcmp(pdata, SPIC_CALIB_PATTERN, 8) == 0) {
			DBG_8195A("read_mode:%d\n", i);
			break;
		} else {
			if(flash_init_para.debug)
				DBG_8195A("read mode %d fail\n", i);
		}
	}

	if(i == 5) {
		DBG_8195A("Flash read mode switch FAIL!\n");
		Ret = _FAIL;
	}

	return Ret;
}

void flash_operation_config(void)
{
	u8 read_mode;
	u8 flash_speed;

	if (SOCPS_DsleepWakeStatusGet() == TRUE) {
		return;
	}

	read_mode = flash_get_option(Flash_ReadMode, _FALSE);
	flash_speed = flash_get_option(Flash_Speed, _TRUE);
	//DBG_8195A("flash_speed: %d\n", flash_speed);
	__asm volatile( "cpsid i" );
	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	flash_get_vendor();

	/* Set flash status register: set QE, clear protection bits */
	if (SOCPS_DsleepWakeStatusGet() == FALSE) {
		flash_set_status_register();
	}

	/* Set flash I/O mode and high-speed calibration */
	flash_rx_mode_switch(read_mode);
	flash_calibration_highspeed(flash_speed);
	flash_calibration_backup(flash_speed, read_mode);
	__asm volatile( "cpsie i" );
}

