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

#ifndef __MIXER_H__
#define __MIXER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define MIXER_FORMAT_MONO			1	//单声道音源，每个采样点2个字节
#define MIXER_FORMAT_STERO			2	//双声道音源，每个采样点4个字节


//Mixer模块初始化
//PcmFifoAddr: 必须在PMEM中
void MixerInit(void* PcmFifoAddr, uint16_t PcmFifoSize);
	
//设置Mixer模块内部是否做采样率转换，只对输入通道0有效
//SrcEnable = FALSE：不对输入的PCM数据做采样率转换
//SrcEnable = TRUE： Mixer模块内部将输入的PCM数据采样率统一转换为44.1KHZ
void MixerSrcEnable(bool SrcEnable);
	
//使能某个输入通道
void MixerEnable(uint8_t SourceID);

//禁止某个输入通道
void MixerDisable(uint8_t SourceID);

//动态改变某个通道的音量
//SourceID:    0--3
//LeftVolume:  0--8191, 4095: 0dB, 8191: +6dB
//RightVolume: 0--8191, 4095: 0dB, 8191: +6dB
void MixerConfigVolume(uint8_t SourceID, uint16_t LeftVolume, uint16_t RightVolume);

//MUTE某个输入通道
void MixerMute(uint8_t SourceID);

//UNMUTE某个输入通道
void MixerUnmute(uint8_t SourceID);

//动态改变某个通道的采样率与格式
//SourceID:    0--3
//SampleRate:  采样率，输入源0支持9种采样率，其他输入源只支持44.1KHZ采样率
//PcmFormat:   1--单声道，2--双声道
void MixerConfigFormat(uint8_t SourceID, uint16_t SampleRate, uint8_t PcmFormat);

//检查某通道是否可以设置新的数据
bool MixerIsDone(uint8_t SourceID);

//SourceID:    0--3
//PcmBuf:      PCM缓冲区首地址
//SampleCnt:   采样点数
void MixerSetData(uint8_t SourceID, void* PcmBuf, uint16_t SampleCnt);

//获取某输入源在Mixer内剩余的采样点数，折算为在该输入源采样率下的点数
//SourceID:    0--3
uint16_t MixerGetRemainSamples(uint8_t SourceID);

//设置fadein/fadeout速度
//SourceID:    0--3
//FadeinTime:  0--2000，音量从0渐变到4095的时间，单位: ms
//FadeoutTime: 0--2000，音量从4095渐变到0的时间，单位: ms
void MixerSetFadeSpeed(uint8_t SourceID, uint16_t FadeinTime, uint16_t FadeoutTime);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
