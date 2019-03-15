/**
 *****************************************************************************
 * @file     sample_rate_convert.h
 * @author   orson
 * @version  V1.0.0
 * @date     06-May-2014
 * @maintainer: YunWang
 * @brief    declare i2s module driver interface
 *****************************************************************************
 * @attention
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

/**
* @addtogroup 驱动
* @{
* @defgroup SampleRateConvert SampleRateConvert
* @{
*/

#ifndef __SAMPLE_RATE_CONVERT_H__
#define __SAMPLE_RATE_CONVERT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
    
    
/***************************************************************
            以下两个函数用于所有采样率转44.1KHZ使用
***************************************************************/ 
/**
 * @brief  采样率转换模块初始化
 * @param  SrcIndex       提供1个采样率转换实体，编号为0
 * @param  InSampleRate   采样率转换前的采样率，9种采样率
 * @param  OutSampleRate  采样率转换后的采样率，目前只允许44100
 * @param  ChannelNum     声道个数，1 or 2
 * @return TRUE or FALSE
 */   
bool SampleRateConvertInit(uint8_t SrcIndex, uint16_t InSampleRate, uint16_t OutSampleRate, uint8_t ChannelNum);   

/**
 * @brief  采样率转换模块初始化
 * @param  SrcIndex       提供1个采样率转换实体，编号为0
 * @param  InBuf          采样率转换前的PCM数据
 * @param  OutBuf		  采样率转换后的PCM数据
 * @param  SampleCnt      采样率转换前的采样点数
 * @return 转换后的OutBuf缓冲区中有效采样点数
 */
uint16_t SampleRateConvert(uint8_t SrcIndex, int16_t* InBuf, int16_t* OutBuf, uint16_t SampleCnt);


/***************************************************************
            以下函数用于所有采样率升4倍用
***************************************************************/ 
/**
 * @brief  采样率升4倍初始化
 * @param  W_Addr     在升采样率时需要用到的FIR滤波器缓存地址
 * @param  W_Sample   FIR滤波器缓存大小，单位为：Sample
 * @param  Channel    通道号，1 -- 单声道， 2 -- 双声道，目前仅支持单声道
 * @return 无
 */
void SampleRateUpSampleby4Init(int16_t *W_Addr, int32_t W_Sample, int16_t Channel);

/**
 * @brief  采样率升4，目前只支持单声道
 * @param  BufIn    在升采样率时需要用到的FIR滤波器缓存地址
 * @param  BufOut   FIR滤波器缓存大小，单位为：Sample
 * @param  n 		输入的采样点，注意n * 4 + 30不大于W_Sample
 * @return >=0输出的采样点数, -1 输入参数错误
 */
int32_t SampleRateUpSampleby4(int16_t *BufIn, int16_t *BufOut, int32_t n);
    
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif   //

/**
 * @}
 * @}
 */
