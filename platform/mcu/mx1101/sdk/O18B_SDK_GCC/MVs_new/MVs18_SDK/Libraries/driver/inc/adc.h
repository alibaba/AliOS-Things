/**
*************************************************************************************
* @file	sradc.h
* @author	Yancy
* @version	v1.0.0
* @date    2013/05/29
* @brief	Sar ADC driver header file
* Copyright (C) Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
*************************************************************************************
*/
/**************************************************************************************
 *@maintainer Fenston
 *@date 2014/05/08
 **************************************************************************************
 **/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * ADC 通道选择
 * Note: 不要更改以下宏定义!
 */
#define     ADC_CHANNEL_LDOIN          0	 /**< ADC channel: LDOIN*/
#define     ADC_CHANNEL_B5             2	 /**< ADC channel: GPIO B5*/
#define     ADC_CHANNEL_B6             3	 /**< ADC channel: GPIO B6 */
#define     ADC_CHANNEL_B7             4	 /**< ADC channel: GPIO B7 */
#define     ADC_CHANNEL_B22            5	 /**< ADC channel: GPIO B22*/
#define     ADC_CHANNEL_B23            6	 /**< ADC channel: GPIO B23*/
#define     ADC_CHANNEL_B24            7	 /**< ADC channel: GPIO B24*/
#define     ADC_CHANNEL_B25            8	 /**< ADC channel: GPIO B25*/
#define     ADC_CHANNEL_C7             9	 /**< ADC channel: GPIO C7*/
#define     ADC_CHANNEL_C8            10	 /**< ADC channel: GPIO C8 */
#define     ADC_CHANNEL_C9            11	 /**< ADC channel: GPIO C9*/

/**
  * @brief  配置gpio为sar adc通道
  * @param  AdcChannel: 需要配置的adc channel
  *     @arg ADC_CHANNEL_B5
  *     @arg ADC_CHANNEL_B6
  *     @arg ADC_CHANNEL_B7
  *     @arg ADC_CHANNEL_B22
  *     @arg ADC_CHANNEL_B23
  *     @arg ADC_CHANNEL_B24
  *     @arg ADC_CHANNEL_B25
  *     @arg ADC_CHANNEL_C7
  *     @arg ADC_CHANNEL_C8
  *     @arg ADC_CHANNEL_C9
  * @return void
  */
void SarAdcGpioSel(uint8_t AdcChannel);

/**
  * @brief  读取ADC通道的采样值，参考电压为LDO3V3
  * @param  Channel: ADC channel ID
  *     @arg ADC_CHANNEL_LDOIN
  *     @arg ADC_CHANNEL_B5
  *     @arg ADC_CHANNEL_B6
  *     @arg ADC_CHANNEL_B7
  *     @arg ADC_CHANNEL_B22
  *     @arg ADC_CHANNEL_B23
  *     @arg ADC_CHANNEL_B24
  *     @arg ADC_CHANNEL_B25
  *     @arg ADC_CHANNEL_C7
  *     @arg ADC_CHANNEL_C8
  *     @arg ADC_CHANNEL_C9
  * @return 获取的采样值
  */
uint16_t SarAdcChannelGetValue(uint8_t Channel);


/**
  * @brief  5V稳定电源供电系统使用SarAdc对LDO3V3再次Trim。
  * @param  void
  * @return void
  */
void SarAdcTrimLdo3V3(void);


/**
  * @brief  调用SarAdcGetLdoinVoltage前的初始化，只需调用一次。
  * @param  void
  * @return void
  */
void SarAdcLdoinVolInit(void);

/**
  * @brief  读取LDOIN的电压值
  * @param  NONE
  * @return 以毫伏为单位的电压值
  * @note   在调用SarAdcGetLdoinVoltage函数之前，需调用SarAdcLdoinVolInit进行初始化
  */
uint16_t SarAdcGetLdoinVoltage(void);

/**
  * @brief  读取GPIO的电压值
  * @param  Channel: ADC channel ID
  * @return 以毫伏为单位的电压值
  */
uint16_t SarAdcGetGpioVoltage(uint8_t Channel);

/**
  * @brief  读取锂电池剩余电量
  * @param  None
  * @return 剩余电量的百分比，返回值为0~100
  */
uint8_t SarAdcGetRemainBatteryCapacity(void);


/**
  * @brief  PowerDown ADC for low power
  * @param  void
  * @return void
  * @note   在进入休眠时通过powerdown ADC降低功耗
  */
void SarAdcPowerDown(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

