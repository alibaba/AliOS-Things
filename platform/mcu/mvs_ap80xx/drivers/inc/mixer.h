/******************************************************************************
 * @file    mixer.h
 * @author  Orson
 * @version V1.0.0
 * @date    29-April-2014
 * @brief   audio mixer
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

/**
* @addtogroup 驱动
* @{
* @defgroup Mixer Mixer
* @{
*/

#ifndef __MIXER_H__
#define __MIXER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define MIXER_FORMAT_MONO			1	/**单声道音源，每个采样点2个字节。单声道输入，双声道输出*/
#define MIXER_FORMAT_STERO			2	/**双声道音源，每个采样点4个字节。双声道输入，双声道输出*/
#define MIXER_FORMAT_MONO_1	        3	/**单声道音源，每个采样点2个字节。单声道输入，单声道输出，从DAC左声道输出*/
#define MIXER_FORMAT_MONO_2     	4	/**单声道音源，每个采样点2个字节。单声道输入，单声道输出，从DAC右声道输出*/
#define MIXER_FORMAT_STERO_1	    5	/**双声道音源，每个采样点4个字节。双声道输入，单声道输出，抽取输入源的左声道，从DAC左声道输出*/
#define MIXER_FORMAT_STERO_2	    6	/**双声道音源，每个采样点4个字节。双声道输入，单声道输出，抽取输入源的右声道，从DAC右声道输出*/
#define MIXER_FORMAT_STERO_3	    7	/**双声道音源，每个采样点4个字节。双声道输入，单声道输出，输入源的左右声道相加除以2，从DAC左声道输出*/
#define MIXER_FORMAT_STERO_4	    8	/**双声道音源，每个采样点4个字节。双声道输入，单声道输出，输入源的左右声道相加除以2，从DAC右声道输出*/


/**
 * @brief  Mixer模块初始化
 * @param  PcmFifoAddr PcmFifo起始地址
 * @param  PcmFifoSize PcmFifo长度
 * @return 无
 * @note PcmFifo必须在PMEM中
 */
void MixerInit(void* PcmFifoAddr, uint16_t PcmFifoSize);

/**
 * @brief  设置Mixer模块内部是否做采样率转换，只对输入通道0有效
 * @param  SrcEnable 是否转采样使能型号
 * @arg        SrcEnable = FALSE：不对输入的PCM数据做采样率转换
 * @arg        SrcEnable = TRUE： Mixer模块内部将输入的PCM数据采样率统一转换为44.1KHZ
 * @return 无
 */
void MixerSrcEnable(bool SrcEnable);

/**
 * @brief  Mixer模块使能某个输入通道
 * @param  SourceID mixer某个输入通道
 * @return 无
 */
void MixerEnable(uint8_t SourceID);

/**
 * @brief  Mixer模块禁止某个输入通道
 * @param  SourceID mixer某个输入通道
 * @return 无
 */
void MixerDisable(uint8_t SourceID);

/**
 * @brief  Mixer模块动态改变某个通道的音量
 * @param  SourceID mixer某个输入通道（0--3）
 * @param  LeftVolume 0--8191, 4095: 0dB, 8191: +6dB
 * @param  RightVolume 0--8191, 4095: 0dB, 8191: +6dB
 * @return 无
 */
void MixerConfigVolume(uint8_t SourceID, uint16_t LeftVolume, uint16_t RightVolume);

/**
 * @brief  Mixer模块MUTE某个输入通道
 * @param  SourceID mixer某个输入通道
 * @return 无
 */
void MixerMute(uint8_t SourceID);

/**
 * @brief  Mixer模块UNMUTE某个输入通道
 * @param  SourceID mixer某个输入通道
 * @return 无
 */
void MixerUnmute(uint8_t SourceID);

/**
 * @brief  Mixer模块动态改变某个通道的采样率与格式
 * @param  SourceID  mixer某个输入通道(0--3)
 * @param  SampleRate 采样率，输入源0支持9种采样率，其他输入源只支持44.1KHZ采样率
 * @param  PcmFormat  1--单声道，2--双声道
 * @return 无
 */
void MixerConfigFormat(uint8_t SourceID, uint16_t SampleRate, uint8_t PcmFormat);

/**
 * @brief  Mixer模块检查某通道是否可以设置新的数据
 * @param  SourceID mixer某个输入通道
 * @return 可以设置新数据返回TRUE，否则返回FALSE
 */
bool MixerIsDone(uint8_t SourceID);

/**
 * @brief  Mixer模块设置新的数据
 * @param  SourceID mixer某个输入通道（0--3）
 * @param  PcmBuf  PCM缓冲区首地址
 * @param  SampleCnt  采样点数
 * @return 无
 */
void MixerSetData(uint8_t SourceID, void* PcmBuf, uint16_t SampleCnt);

/**
 * @brief  Mixer模块获取某输入源在Mixer内剩余的采样点数，折算为在该输入源采样率下的点数
 * @param  SourceID mixer某个输入通道
 * @return 改输入源在mixer内的剩余采样点数
 */
uint16_t MixerGetRemainSamples(uint8_t SourceID);

/**
 * @brief  Mixer模块设置fadein/fadeout速度
 * @param  SourceID mixer某个输入通道
 * @param  FadeinTime  0--2000，音量从0渐变到4095的时间，单位: ms
 * @param  FadeoutTime 0--2000，音量从4095渐变到0的时间，单位: ms
 * @return 无
 */
void MixerSetFadeSpeed(uint8_t SourceID, uint16_t FadeinTime, uint16_t FadeoutTime);

/**
 * @brief  Mixer模块音量控制使能/禁能
 * @param  en TRUE/FALSE
 * @arg 	TRUE - MixerConfigVolume可以控制音量
 * @arg 	FALSE - MixerConfigVolume不能控制音量，Mixer对音频数据不做任何增益/衰减
 * @return 无
 */
void MixerVolEn(bool en);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
