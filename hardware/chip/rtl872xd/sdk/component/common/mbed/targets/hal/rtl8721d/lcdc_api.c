   /** mbed Microcontroller Library
  ******************************************************************************
  * @file    lcdc_api.c
  * @author 
  * @version V1.0.0
  * @date    2019-04-19
  * @brief   This file provides mbed API for LCDC.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "objects.h"
#include "lcdc_api.h"
#include "pinmap.h"
#include "PinNames.h"


static PinName lcdc_mcu_pin[21] = {PB_28, PB_23, PB_22, PB_21, PB_20, PA_19, PA_20, 
								PA_23, PA_24, PA_31, PB_0, PA_30, PA_28, PA_26,
								PA_25, PB_8, PB_9, PB_10, PB_11, PB_18, PB_19};

static PinName lcdc_rgb_pin[20] = {PB_28, PB_23, PB_22, PB_20, PA_19, PA_20, PA_23,
								PA_24, PA_31, PB_0, PA_30, PA_28, PA_26, PA_25,
								PB_8, PB_9, PB_10, PB_11, PB_18, PB_19};

static PinName lcdc_led_pin[9] = {PB_28, PB_23, PB_22, PA_19, PA_20, PA_23, PA_24, 
								PA_31, PB_0};

static PinName lcdc_led_linesel_pin[10] = {PA_30, PA_28, PA_26, PA_25, PB_8, PB_9, PB_10, 
								PB_11, PB_18, PB_19};



static void lcdc_pin_init(PinName* lcdc_pin, u32 length)
{
	for(u32 i = 0; i < length; i++){
		Pinmux_Config(lcdc_pin[i], PINMUX_FUNCTION_LCD);
	}
}

static int _log2(int n){
	int a = 0;
	while(n%2 == 0){
		n = n/2;
		a++;
	}
	return a;
}

/**
  * @brief    Initializes the LCDC peripheral.
  * @param  obj: lcdc object define in application software.
  * @retval None
  */	
void lcdc_init(lcdc_t * obj)
{
	if(obj->lcdc_type == 0){
		LCDC_MCUInit(LCDC, &obj->lcdc_if.LCDC_MCUInitStruct);
		
		if(obj->lcdc_if.LCDC_MCUInitStruct.LCDC_MCUIfMode == LCDC_MCU_IF_16_BIT){
			lcdc_pin_init(lcdc_mcu_pin, 21);
		}else{
			lcdc_pin_init(lcdc_mcu_pin, 13);
		}
		
	}else if(obj->lcdc_type == 1){
		LCDC_RGBInit(LCDC, &obj->lcdc_if.LCDC_RGBInitStruct);
		
		if(obj->lcdc_if.LCDC_RGBInitStruct.LCDC_RGBIfMode == LCDC_RGB_IF_16_BIT){
			lcdc_pin_init(lcdc_rgb_pin, 20);
		}else{
			lcdc_pin_init(lcdc_rgb_pin, 10);
		}
		
	}else if(obj->lcdc_type == 2){
		LCDC_LEDInit(LCDC, &obj->lcdc_if.LCDC_LEDInitStruct);
		
		if(obj->lcdc_if.LCDC_LEDInitStruct.LCDC_LEDColorChnl == LCDC_LED_ONE_CHANNEL){
			int line_sel;
			line_sel = _log2(obj->lcdc_if.LCDC_LEDInitStruct.LCDC_LEDImgHeight);

			lcdc_pin_init(lcdc_led_linesel_pin, line_sel);
			
			switch(obj->lcdc_if.LCDC_LEDInitStruct.LCDC_LEDColorNum ){
				case LCDC_LED_ONE_COLOR:
					lcdc_pin_init(lcdc_led_pin, 4);
					break;
				case LCDC_LED_TWO_COLOR:
					lcdc_pin_init(lcdc_led_pin, 5);
					break;
				case LCDC_LED_THREE_COLOR:
					lcdc_pin_init(lcdc_led_pin, 6);
					break;
			}
		}else{
			int line_sel;
			line_sel = _log2(obj->lcdc_if.LCDC_LEDInitStruct.LCDC_LEDImgHeight/2);

			lcdc_pin_init(lcdc_led_linesel_pin, line_sel);
			
			switch(obj->lcdc_if.LCDC_LEDInitStruct.LCDC_LEDColorNum ){
				case LCDC_LED_ONE_COLOR:
					lcdc_pin_init(lcdc_led_pin, 5);
					break;
				case LCDC_LED_TWO_COLOR:
					lcdc_pin_init(lcdc_led_pin, 7);
					break;
				case LCDC_LED_THREE_COLOR:
					lcdc_pin_init(lcdc_led_pin, 9);
					break;
			}
		}
	}else{
		DBG_8195A("LCDC IF ERROR: unknown lcdc interface type!!\n");
	}
}

/**
  * @brief    Trigger the hardware to transfer one frame from DMA buffer based 
  *               on sync signal.
  * @param  obj: lcdc object define in application software.
  * @retval None
  */
void lcdc_mcu_dma_trigger(lcdc_t * obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
		
	LCDC_MCUDMATrigger(LCDC);
}

/**
  * @brief     write data to MCU I/F bus.
  * @param  obj: lcdc object define in application software.
  * @param  Data: the data to transmit.
  * @retval   None
  */
void  lcdc_mcu_write_data(lcdc_t * obj, u16 data)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_MCUIOWriteData(LCDC, data);
}

/**
  * @brief     read data from MCU I/F bus.
  * @param  obj: lcdc object define in application software.
  * @retval   the read value
  */
u16 lcdc_mcu_read_data(lcdc_t * obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	return LCDC_MCUIOReadData(LCDC);
}

/**
  * @brief     write command to MCU I/F bus.
  * @param  obj: lcdc object define in application software.
  * @param  Cmd: the command to transmit.
  * @retval   None
  */
void lcdc_mcu_write_cmd(lcdc_t * obj, u16 cmd)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_MCUIOWriteCmd(LCDC, cmd);
}

/**
  * @brief    Configure LCDC DMA burst size .
  * @param  obj: lcdc object define in application software.
  * @param  BurstSize: DMA burst size; Unit 64 Bytes.
  * @note     If BurstSize=1, the actual burstsize = 1x64 Bytes; if the BurstSize=2, 
  *                the actual burstsize = 2x64 = 128 Bytes.... The parameter "BurstSize" is not more
  *                than 8.
  * @retval   None
  */
void lcdc_dma_burstsize_config(lcdc_t * obj, u32 BurstSize)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_DMAModeConfig(LCDC, BurstSize);
}

/**
  * @brief    Configure LCDC DMA under flow mode and under flow error data .
  * @param  obj: lcdc object define in application software.
  * @param  DmaUnFlwMode: DMA under flow mode, this parameter
  *                can be one of the following values:
  *     	    @arg LCDC_DMAUNFW_OUTPUT_LASTDATA:  output last data
  *                @arg LCDC_DMAUNFW_OUTPUT_ERRORDATA:  output error data
  * @param  ErrorData: the output data when  DMA FIFO underflow occurred. When under flow mode is configured as
  *                LCDC_DMAUNFW_OUTPUT_ERRORDATA, this parameter is needed, and otherwise it can be ignored.
  * @retval   None
  */
void lcdc_dma_underflow_config(lcdc_t * obj, u32 DmaUnFlwMode, u32 ErrorData)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_DMAUnderFlowConfig(LCDC, DmaUnFlwMode, ErrorData);
}

/**
  * @brief    Configure image base address.
  * @param  obj: lcdc object define in application software.
  * @param  ImgBaseAddr: the image base address.
  * @retval   None
  */
void lcdc_dma_addr_config(lcdc_t * obj, u32 ImgBaseAddr)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_DMAImageBaseAddrConfig(LCDC, ImgBaseAddr);
}

/**
  * @brief    Set lcdc interrupt handler.
  * @param  obj: lcdc object define in application software.
  * @param  func: the interrupt handler function.
  * @retval   None
  */
void lcdc_set_irq_handler(lcdc_t * obj, void * func)
{
	InterruptRegister((IRQ_FUN)func, LCDC_IRQ, (u32)obj, 5);
	InterruptEn(LCDC_IRQ, 5);
}

/**
  * @brief  Enables or disables the specified LCDC interrupts.
  * @param  obj: lcdc object define in application software.
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
void lcdc_set_irq(lcdc_t * obj, u32 LCDC_IT,  u32 NewState)
{
	/* To avoid gcc warnings */
	( void ) obj;

	LCDC_INTConfig(LCDC, LCDC_IT, NewState);
}

/**
  * @brief  Clears the LCDC's interrupt pending bits.
  * @param  obj: lcdc object define in application software.
  * @param  LCDC_IT: specifies the interrupt to be cleared.
  *   This parameter can be any combination of the following values:
  *     @arg LCDC_IT_LINE:line interrupt
  *     @arg LCDC_IT_FRDN: refresh frame done interrupt
  *     @arg LCDC_IT_DMAUNDFW: DMA FIFO under flow interrupt
  *     @arg LCDC_IT_IO_TIMEOUT: IO write/read timeout interrupt
  *     @arg LCDC_IT_FRM_START: Frame Start interrupt
  * @retval None
  */
void lcdc_clear_irq(lcdc_t * obj, u32 LCDC_IT)
{
	/* To avoid gcc warnings */
	( void ) obj;

	LCDC_ClearINT(LCDC, LCDC_IT);
}

/**
  * @brief  Get lcdc interrupt status.
  * @param  obj: lcdc object define in application software.
  * @retval interrupt status
  */
u32  lcdc_get_irq_status(lcdc_t * obj)
{
	/* To avoid gcc warnings */
	( void ) obj;

	return LCDC_GetINTStatus(LCDC);
}

/**
  * @brief  Enables the LCDC.
  * @param  obj: lcdc object define in application software.
  * @retval None
  */
void lcdc_enable(lcdc_t * obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_Cmd(LCDC, ENABLE);
}

/**
  * @brief  Disables the LCDC.
  * @param  obj: lcdc object define in application software.
  * @retval None
  */
void lcdc_disable(lcdc_t * obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_Cmd(LCDC, DISABLE);
}


/**
  * @brief  Deinitializes the LCDC.
  * @param  obj: lcdc object define in application software.
  * @note   Disable LCDC instantly, clear and disable all interrupts.
  * @retval None
  */
void lcdc_deinit(lcdc_t * obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	LCDC_DeInit(LCDC);
}
