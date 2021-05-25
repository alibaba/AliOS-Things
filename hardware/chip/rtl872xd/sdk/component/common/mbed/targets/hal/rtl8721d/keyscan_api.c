/** mbed Microcontroller Library
  ******************************************************************************
  * @file    keyscan_api.c
  * @author 
  * @version V1.0.0
  * @date    2019-04-19
  * @brief   This file provides mbed API for KeyScan.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "objects.h"
#include "PinNames.h"
#include "keyscan_api.h"

#include "pinmap.h"

KeyScan_InitTypeDef KeyScanInitData;

u32 keyrow[8] = {_PA_12, _PA_13, _PA_14, _PA_15, _PA_16, _PA_18, _PA_17, _PA_21};
u32 keycolumn[8] = {_PA_26, _PA_25, _PA_19, _PA_17, _PA_18, _PA_16, _PA_15, _PA_20};

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param  col: selected column num depending on KeyColumn(eg: col 0 & col 2, col is 5) .
  * @param  row: selected column num depending on KeyRow .
  * @return void
  */
void keyscan_array_pinmux(u32 col, u32 row)
{
	u32 temp;

	for(temp = 0; temp < 8; temp++) {
		if(col & (1 << temp)) {
			PAD_PullCtrl(keycolumn[temp], GPIO_PuPd_NOPULL);
			Pinmux_Config(keycolumn[temp], PINMUX_FUNCTION_KEYSCAN_COL);
			DBG_8195A("column:%d\n", keycolumn[temp]);
		}
	}

	for(temp = 0; temp < 8; temp++) {
		if(row & (1 << temp)) {
			PAD_PullCtrl(keyrow[temp], GPIO_PuPd_UP);
			Pinmux_Config(keyrow[temp], PINMUX_FUNCTION_KEYSCAN_ROW);
			DBG_8195A("row:%d\n", keyrow[temp]);
		}
	}
}

/**
  * @brief  Get data number of keyscan FIFO.
  * @param  obj: keyscan object defined in application software.
  * @retval data number of keyscan FIFO
  */
 u8 keyscan_getdatanum(keyscan_t *obj)
{
	u8 num;
	(void) obj;

	num = KeyScan_GetDataNum(KEYSCAN_DEV);
	return num;
}

/**
  * @brief  Read data from keyscan FIFO.
  * @param  obj: keyscan object defined in application software.
  * @param  pBuf: buffer to save data read from KeyScan FIFO.
  * @param  num: number of data to be read.
  * @retval None
  */
void keyscan_read(keyscan_t *obj, u32* pbuf, u8 num )
{
	(void) obj;
	KeyScan_Read(KEYSCAN_DEV, pbuf, num);
}

/**
  * @brief  Initializes the KeyScan device.
  * @param  obj: keyscan object defined in application software.
  * @retval None
  */
void keyscan_init(keyscan_t *obj)
{

	/* keyscan not used */
	if ((obj->col == 0) || (obj->row == 0)) {
		return;
	}

	/*init keyscan array*/
	keyscan_array_pinmux(obj->col, obj->row);
	
	/*default value*/
	KeyScan_StructInit(&KeyScanInitData);

	KeyScanInitData.KS_ColSel = obj->col;
	KeyScanInitData.KS_RowSel = obj->row;
	KeyScanInitData.KS_ClkDiv = obj->clk;
	KeyScanInitData.KS_WorkMode = obj->workmode<<3;
	KeyScanInitData.KS_LimitLevel = obj->keylimit;
	KeyScanInitData.KS_OverCtrl = obj->overctrl<<1;

	KeyScan_Init(KEYSCAN_DEV, &KeyScanInitData);
}

/**
  * @brief  Enables or disables the specified KeyScan interrupts mask.
  * @param  obj: keyscan object defined in application software.
  * @param  keyscan_IT: specifies the KeyScan interrupt sources to be enabled or masked.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_KS_SCAN_EVENT_INT_MSK: Mask Scan event interrupt status
  *            @arg BIT_KS_FIFO_LIMIT_INT_MSK: Mask FIFO limit interrupt status
  *            @arg BIT_KS_FIFO_OVERFLOW_INT_MSK: Mask FIFO overflow interrupt
  *            @arg BIT_KS_FIFO_FULL_INT_MSK: Mask FIFO full interrupt
  *            @arg BIT_KS_SCAN_FINISH_INT_MSK: Mask scan finish interrupt
  *            @arg BIT_KS_FIFO_NOTEMPTY_INT_MSK: Mask FIFO nonempty interrupt
  *            @arg BIT_KS_ALL_RELEASE_INT_MSK: Mask All Release interrupt
  * @param  newstate: new state of the specified KeyScan interrupts mask.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void keyscan_set_irq(keyscan_t * obj, u32 keyscan_IT, u32 newstate)
{
	( void ) obj;

	KeyScan_INTConfig(KEYSCAN_DEV, keyscan_IT, newstate);
}

/**
  * @brief  Clear the specified KeyScan interrupt pending bit.
  * @param  obj: keyscan object defined in application software.
  * @param  keyscan_IT: specifies the KeyScan interrupt to be cleared.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_KS_FIFO_LIMIT_INT_STATUS: FIFO limit interrupt status
  *            @arg BIT_KS_FIFO_OVERFLOW_INT_STATUS: FIFO overflow interrupt status
  *            @arg BIT_KS_SCAN_FINISH_INT_STATUS: Scan finish interrupt status
  *            @arg BIT_KS_ALL_RELEASE_INT_STATUS: All Release interrupt status
  * @note   BIT_KS_SCAN_EVENT_INT_STATUS is automatically cleared by hardware when the data is read. 
  *         BIT_KS_FIFO_FULL_INT_STATUS is automatically cleared by hardware when the buffer level 
  *         goes below the BIT_KS_FIFO_THREHOLD_LEVEL threshold. 
  *         BIT_KS_FIFO_NOTEMPTY_INT_STATUS is automatically cleared by hardware when the FIFO is empty.
  * @retval None
  */
void keyscan_clear_irq(keyscan_t * obj, u32 keyscan_IT)
{
	( void ) obj;

	KeyScan_ClearINT(KEYSCAN_DEV, keyscan_IT);
}

/**
  * @brief  Get KeyScan interrupt status.
  * @param  obj: keyscan object defined in application software.
  * @retval interrupt status
  */
u32 keyscan_get_irq_status(keyscan_t * obj)
{
	( void ) obj;
	
	u32 irq_status = KeyScan_GetINT(KEYSCAN_DEV);
	return irq_status;
}

/**
  * @brief  Set keyscan interrupt handler.
  * @param  obj: keyscan object define in application software.
  * @param  func: the interrupt handler function.
  * @retval None
  */
void keyscan_set_irq_handler(keyscan_t * obj, void * func)
{
	( void ) obj;
	InterruptRegister((IRQ_FUN)func, KEYSCAN_IRQ, (u32)obj, 5);
	InterruptEn(KEYSCAN_IRQ, 5);
}

/**
  * @brief  Enable the specified KeyScan peripheral.
  * @param  obj: keyscan object defined in application software.
  * @retval None
  */
void keyscan_enable(keyscan_t *obj)
{
	(void) obj;

	KeyScan_Cmd(KEYSCAN_DEV, ENABLE);
}

/**
  * @brief  disable the specified KeyScan peripheral.
  * @param  obj: keyscan object defined in application software.
  * @retval None
  */
void keyscan_disable(keyscan_t *obj)
{
	(void) obj;

	KeyScan_Cmd(KEYSCAN_DEV, DISABLE);
}

/**
  * @brief  Clears the FIFO data.
  * @param  obj: keyscan object defined in application software.
  * @retval None
  */
void keyscan_clearfifodata(keyscan_t *obj)
{
	(void) obj;
	KeyScan_ClearFIFOData(KEYSCAN_DEV);
}

/** 
  * @}
  */

/** 
  * @}
  */

/** 
  * @}
  */
  
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
