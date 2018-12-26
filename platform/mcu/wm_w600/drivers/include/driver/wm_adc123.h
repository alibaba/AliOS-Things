/**************************************************************************//**
 * @file     wm_adc.h
 * @author
 * @version
 * @date
 * @brief
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/

#ifndef WM_ADC_H
#define WM_ADC_H

#include "wm_type_def.h"

//每次启动dma之后，需要一段稳定时间，所以采集到的数据前面的12个byte不稳定，要舍去
#define ADC_DEST_BUFFER_DMA	(u32)		0x20028000//(u32)0x20037000	//前面的地方高速SPI可能会用	
#define ADC_DEST_BUFFER_SIZE			2000//2000	//以半字为单位


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
#define CONFIG_ADC_VCM(n)				((n&0x3F)<<26)
#define CONFIG_ADC_G_TEMP12(n)			((n&0x03)<<24)


#define ADC_INT_TYPE_ADC				0
#define ADC_INT_TYPE_DMA				1
#define ADC_INT_TYPE_ADC_CMP 			2

#define ADC_REFERENCE_EXTERNAL  		0       //外部参考
#define ADC_REFERENCE_INTERNAL  		1       //内部参考
typedef struct adc_st{
	u8 dmachannel;
	void (*adc_cb)(u16 *buf, u16 len);
	void (*adc_cmp_cb)(u16 *buf, u16 len);
	void (*adc_dma_cb)(u16 *buf,u16 len);
	u16 valuelen;		/*dma 采样数据长度*/
}ST_ADC;


/**
 * @brief          	This function is used to initial adc
 *
 * @param[in]      	ifusedma		whether or not use dma
 * @param[in]      	dmachannel	choose the dma channel  0~7
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_init(u8 ifusedma,u8 dmachannel);


/**
 * @brief          	This function is used to clear adc interrupt status
 *
 * @param[in]      	inttype	the adc interrupt type
 *				
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_clear_irq(int inttype);


/**
 * @brief           	This function is used to register the adc interrupt callback function
 *
 * @param[in]      	inttype           the adc interrupt type
 *				ADC_INT_TYPE_ADC		adc interrupt,user get adc result from the callback function.
 *				ADC_INT_TYPE_DMA		dma interrupt,dma transfer the adc result to the user's buffer.
 * @param[in]      	callback         the adc interrupt callback function
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_irq_register(int inttype, void (*callback)(u16 *buf, u16 len));


/**
 * @brief         	This function is used to read adc convert result
 *
 * @param		None	
 *
 * @retval       	adc result[0~16383]
 *
 * @note           	None
 */
u16 tls_read_adc_result(void);


/**
 * @brief         	This function is used to set adc io config
 *
 * @@param[in]    	Channel          adc channel,from 0 to 8 is single input; 8 to 11 is differential input.
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_io_config(int Channel);


/**
 * @brief         	This function is used to start adc convert with cpu mode
 *
 * @@param[in]    	Channel          adc channel,from 0 to 8 is single input; 8 to 11 is differential input.
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_start_with_cpu(int Channel);


/**
 * @brief         	This function is used to start adc convert with dma mode
 *
 * @param[in]    	Channel           adc channel,from 0 to 8 is single input; 8 to 11 is differential input.
 * @param[in]    	Length             need convert adc data length
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_start_with_dma(int Channel, int Length);


/**
 * @brief         	This function is used to stop adc convert
 *
 * @param[in]    	ifusedma     	whether or not use dma	
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_stop(int ifusedma);


/**
 * @brief         	This function is used to config adc compare mode
 *
 * @param[in]    	cmp_data     	adc compare value	
 * @param[in]    	cmp_pol     	(0)  bigger than cmp_data interrupt  (other) littler than cmp_data interrupt
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_config_cmp_reg(int cmp_data, int cmp_pol);


/**
 * @brief         	This function is used to choose adc reference source
 *
 * @param[in]    	ref    (0) choose the reference internal  	(other) choose the reference inside external
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_reference_sel(int ref);


/**
 * @brief         	This function is used to star adc calibration
 *
 * @param		None	
 *
 * @return       	None
 *
 * @note           	None
 */
void tls_adc_calibration_start(void);


/**
 * @brief         	This function is used to stop adc calibration
 *
 * @param		None	
 *
 * @return       	None
 *
 * @note           	None
 */
void tls_adc_calibration_stop(void);


/**
 * @brief         	This function is used to start test VCC  voltage
 *
 * @param		None	
 *
 * @return       	None
 *
 * @note           	None
 */
void tls_adc_voltage_start(void);



#endif


