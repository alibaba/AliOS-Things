/***************************************************************************** 
* 
* File Name : wm_adc.h 
* 
* Description: adc Driver Module 
* 
* Copyright (c) 2014 Winner Microelectronics Co., Ltd. 
* All rights reserved. 
* 
* Author : dave
* 
* Date : 2014-8-15
*****************************************************************************/ 

#ifndef WM_ADC_H
#define WM_ADC_H

#include "wm_type_def.h"

//每次启动dma之后，需要一段稳定时间，所以采集到的数据前面的12个byte不稳定，要舍去
#define ADC_DEST_BUFFER_DMA     		(u32)0x20028000//(u32)0x20037000	//前面的地方高速SPI可能会用	
#define ADC_DEST_BUFFER_SIZE			65532//2000			//以半字为单位	
#define SAMPLE_NUM_PER_CHANNEL  		20//2000



#define CONFIG_ADC_CAL_OFFSET_TEMP12   	(1<<23)
#define CONFIG_ADC_G_CTRL12   			(1<<22)
#define CONFIG_ADC_CMP_POL				(1<<21) 
#define CONFIG_ADC_REF_SEL     			(1<<20)
#define CONFIG_ADC_BUF_BYPASS			(1<<19)
#define CONFIG_ADC_DMA_MASK 			(0xFF<<11)
#define CONFIG_ADC_EN_CAL				(1<<10)
#define CONFIG_ADC_INT_CMP				(1<<9)
#define CONFIG_ADC_INT					(1<<8)
#define CONFIG_ADC_CMP_INT_MASK     	(1<<7)
#define CONFIG_ADC_INT_MASK     		(1<<6)
#define CONFIG_ADC_TEMP_ON				(1<<5)
#define CONFIG_ADC_START  				(1<<4)
#define CONFIG_ADC_CHL_MASK 			(0xF)

#define CONFIG_ADC_CHL_OFFSET			(0x0E)
#define CONFIG_ADC_CHL_VOLT				(0x0D)
#define CONFIG_ADC_CHL_TEMP				(0x0C)




#define CONFIG_ADC_INPUT_CMP_VAL(n)		((n&0x3FFF)<<14)
#define CONFIG_ADC_VCM(n)				(((u32)n&0x3F)<<26)
#define CONFIG_ADC_G_TEMP12(n)			((n&0x03)<<24)



#define ADC_INT_TYPE_ADC				0
#define ADC_INT_TYPE_DMA				1
#define ADC_INT_TYPE_ADC_COMP 			2

#define ADC_REFERENCE_EXTERNAL  		0       //外部参考
#define ADC_REFERENCE_INTERNAL  		1       //内部参考

typedef struct adc_st{
	u8 dmachannel;
	void (*adc_cb)(u16 *buf, u16 len);
	void (*adc_bigger_cb)(u16 *buf, u16 len);
	void (*adc_dma_cb)(u16 *buf,u16 len);
	u16 valuelen;		/*dma 采样数据长度*/
	u16 offset;
}ST_ADC;

/**********************************************************************************************************
* Description: 	This function is used to init adc.
*
* Arguments  : 	ifusedma		if use dma
*				dmachannel	dma channel
*
* Returns    : 
* 		
* Notes:			If the requested dma channel is already used by other task, system will auto use other dma channel.
**********************************************************************************************************/
void tls_adc_init(u8 ifusedma,u8 dmachannel);


/**********************************************************************************************************
* Description: 	This function is used to register interrupt callback function.
*
* Arguments  : 	inttype			interrupt type
*				ADC_INT_TYPE_ADC		adc interrupt,user get adc result from the callback function.
*				ADC_INT_TYPE_DMA		dma interrupt,dma transfer the adc result to the user's buffer.
*				callback			interrupt callback function.
* Returns    : 
* 		
**********************************************************************************************************/
void tls_adc_irq_register(int inttype, void (*callback)(u16 *buf, u16 len));

/**********************************************************************************************************
* Description: 	This function is used to clear the interrupt source.
*
* Arguments  : 	inttype			interrupt type
*
* Returns    : 
* 		
**********************************************************************************************************/
void tls_adc_clear_irq(int inttype);

/**********************************************************************************************************
* Description: 	This function is used to start adc,use dma for transfer data.
*
* Arguments  : 	channel		adc channel,from 0 to 3 is single input;4 and 5 is differential input.
*				accuracy		Sampling precision
*							ADC_SAMPLE_ACCURACY_7Bits
*							ADC_SAMPLE_ACCURACY_9Bits
*							ADC_SAMPLE_ACCURACY_10Bits
*							ADC_SAMPLE_ACCURACY_12Bits
*				length		byte data length,is an integer multiple of half word,need <= 0x500
* Returns    : 
* 		
**********************************************************************************************************/
void tls_adc_start_with_dma(int Channel, int Length);

/**********************************************************************************************************
* Description: 	This function is used to start adc.
*
* Arguments  : 	channel		adc channel,from 0 to 3 is single input;4 and 5 is differential input.
*				accuracy		Sampling precision
*							ADC_SAMPLE_ACCURACY_7Bits
*							ADC_SAMPLE_ACCURACY_9Bits
*							ADC_SAMPLE_ACCURACY_10Bits
*							ADC_SAMPLE_ACCURACY_12Bits
* Returns    : 
**********************************************************************************************************/
void tls_adc_start_with_cpu(int Channel);
void tls_adc_enable_calibration_buffer_offset(void);
void tls_adc_voltage_start_with_cpu(void);
void tls_adc_temp_offset_with_cpu(u8 calTemp12);
void tls_adc_voltage_start_with_dma(int Length);




/**********************************************************************************************************
* Description: 	This function is used to read adc result.
*
* Arguments  : 	
* Returns    : 
**********************************************************************************************************/
u16 tls_read_adc_result(void);

/**********************************************************************************************************
* Description: 	This function is used to stop the adc.
*
* Arguments  : 	ifusedma		if use dma
* Returns    : 
**********************************************************************************************************/
void tls_adc_stop(int ifusedma);

/**********************************************************************************************************
* Description: 	This function is used to config adc bigger register.
*
* Arguments  : 	comp_data		compare data
* Returns    : 
**********************************************************************************************************/
void tls_adc_config_cmp_reg(int cmp_data, int cmp_pol);

/**********************************************************************************************************
* Description: 	This function is used to set adc reference source.
*
* Arguments  : 	ref     ADC_REFERENCE_EXTERNAL
                                          ADC_REFERENCE_INTERNAL 
* Returns    : 
**********************************************************************************************************/
void tls_adc_reference_sel(int ref);
void tls_adc_set_clk(int div);
void signedToUnsignedData(u16 *adcValue, u16 *offset);
void tls_adc_buffer_bypass_set(u8 isset);
void tls_adc_cmp_start(int Channel, int cmp_data, int cmp_pol);


#endif

