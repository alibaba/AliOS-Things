/**
 *****************************************************************************
 * @file     audio_adc.h
 * @author   Wang Yun
 * @version  V2.0.0
 * @date     06-June-2014
 * @brief    audio_adc module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#ifndef __AUDIO_ADC_H__
#define __AUDIO_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

    
#define ADC_TO_PMEM                             (0)
#define ADC_TO_I2S                              (1)

#define ALC_HOLD_PGA_GAIN                       (0)
#define ALC_MUTE_OUTPUT                         (1)
#define ALC_SOFT_ON_MUTE                        (2)
#define ALC_RESERV                              (3)

#define ALC_NOISE_DB                            0x01//(0x15)


#define ALC_NOISE_EN                            (1)
#define ALC_NOISE_DIS                           (0)


#define ALC_MAX_GAIN                            (2)
#define ALC_MIN_GAIN                            (7)

/**
 * ADC input channle
 */
typedef enum _ADC_CHANNEL
{
    ADC_CH_NONE	  = 0x00,
    ADC_CH_LINE_L = 0x01,
    ADC_CH_FM1_L  = 0x02,
    ADC_CH_FM2_L  = 0x04,
    ADC_CH_MIC_L  = 0x08,

    ADC_CH_LINE_R = 0x10,
    ADC_CH_FM1_R  = 0x20,
    ADC_CH_FM2_R  = 0x40,
    ADC_CH_MIC_R  = 0x80,

    ADC_CH_MDAC_LOOPBACK_L = 0x100,
    ADC_CH_MDAC_LOOPBACK_R = 0x200,
    ADC_CH_MIX_LOOPBACK_L  = 0x400,
    ADC_CH_MIX_LOOPBACK_R  = 0x800,

    ADC_CH_MASK   = 0xFFF
} ADC_CHANNEL;

/**
 * ALC mode sel
 */
typedef enum _ALC_CHANNEL
{
    ALC_DISABLE	        = 0x00,       /**<disable alc */
    ALC_RIGHT_ONLY      = 0x01,       /**<only right channel enable alc */
    ALC_LEFT_ONLY       = 0x02,       /**<only left channel enable alc */
    ALC_STEREO_OPEN     = 0x03        /**<left and right channel alc all enable*/
} ALC_CHANNEL;


/*****************************************************************************
*                   ADC SECTION
******************************************************************************/
/**
 * @brief  Audio ADC交换左右声道的采样值
 * @param  IsSwitch 1-switch, 0-as normal
 * @return NONE
 */
void AdcChannelSwap(uint8_t IsSwap);

/**
 * @brief  Audio ADC时钟源选择
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @return NONE
 */
bool AdcConfig(uint8_t ClkMode);

/**
 * @brief  在PMEM中开辟存储Audio ADC采样数据的空间，并打开写入到PMEM的通道
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @param  PmemAddr assign start addr of adc write pmem, base addr offset relative
 * @param  PmemDepth assign depth of pmem
 * @return NONE
 */
bool AdcToPmem(uint8_t ClkMode, uint16_t PmemAddr, uint16_t PmemDepth);

/**
 * @brief  配置Audio ADC采样值到I2s out并且同时写入PMEM中，Audio ADC的采样率由I2S的LRCLK决定
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @return NONE
 */
void AdcToI2s(uint8_t ClkMode);

/**
 * @brief  close audio adc module
 * @param  NONE
 * @return NONE
 */
void AdcClose(void);

/**
 * @brief  set digital volume of Audio ADC
 * @param  LVol	-> [0x001:-72dB, 0xFFF:0dB]
 * @param  RVol	-> [0x001:-72dB, 0xFFF:0dB]
 * @return NONE
 * @note   Digital adc mute When value equals 0x0 
 */
void AdcVolumeSet(uint16_t Left, uint16_t Right);

/**
 * @brief  adc write pmem enable
 * @param  NONE
 * @return NONE
 */
void AdcPmemWriteEn(void);

/**
 * @brief  adc write pmem disable
 * @param  NONE
 * @return NONE
 */
void AdcPmemWriteDis(void);

/**
 * @brief  微调Audio ADC采样率，正值代表加快，负值代表减慢，0代表不进行采样率微调
 * @param  AdjEn TRUE: enable, FALSE: disable dac samplerate adjustment function 
 * @param  Ppm range:[-524287,524287] >0: clock speed up val*0.2ppm  <0:clock slow down val*0.2ppm  
 * @return None
 */
void AdcSampleRateAdjust(bool AdjEn, int32_t Ppm);

/**
 * @brief  config alc relative register and enable alc function,more detail please see reg_spec.xls
 * @param  DecayTime [0x0: 24 ms, 0x0A: 24.576 sec, time double per step, support 384ms(speech) 24.58s(music)]
 * @param  AttackTime [0x0: 6 ms, 0x0A: 6.144 sec, time double per step, support 24ms(speech) 384ms(music) ]
 * @return None
 */
void AdcAlcTimeConfig(uint8_t DecayTime, uint8_t HoldTime, uint8_t AttackTime);

/**
 * @brief  config alc relative register and enable alc function,more detail please see reg_spec.xls
 * @param  TargetLevel [0x0: -31.5 dBFS, 0x0F: -9dBFS, -1.5dB step, support 0x0D(-12dB)]
 * @return None
 */
void AdcAlcTargetLevelConfig(uint8_t TargetLevel);

/**
 * @brief  config alc relative register and enable alc function,more detail please see reg_spec.xls
 * @param  AlcSel 0: no alc 1: right only  2: left only  3: stereo
 * @param  TargetLevel [0x0: -31.5 dBFS, 0x0F: -9dBFS, -1.5dB step, support 0x0D(-12dB)]
 * @param  DecayTime [0x0: 24 ms, 0x0A: 24.576 sec, time double per step, support 384ms(speech) 24.58s(music)]
 * @param  AttackTime [0x0: 6 ms, 0x0A: 6.144 sec, time double per step, support 24ms(speech) 384ms(music)]
 * @return None
 */
void AdcAlcConfig(uint8_t AlcSel, uint8_t TargetLevel, uint8_t DecayTime, uint8_t AttackTime);

/**
 * @brief  noise gate threshold set
 * @param  NoiseGateType
 *         @arg 0: hold PGA gain constant (default)
 *         @arg 1: mute output
 *         @arg 2: soft_mute/un_mute output
 * @param  NoiseGateThreshold [0: -76.5 dBFS (default), 0x1F: -30 dBFS]
 * @param  NoiseEnable 0: noise disable (default) 1: noise enable
 * @return None
 */
void AdcAlcNoiseGateSet(uint8_t NoiseGateType, uint8_t NoiseGateThreshold, uint8_t NoiseEnable);

/**
 * @brief  config alc max gain and min gain
 * @param  MaxGain  [0x07:+30dB, 0x0:-12dB]
 * @param  MinGain  [0x07:-30dB, 0x0:-72dB]
 * @return None
 */
void AdcAlcGainSet(uint8_t MaxGain, uint8_t MinGain);

/**
 * @brief  enable or disable ALC
 * @param  AlcSel 0: no alc 1: right only  2: left only  3: stereo
 * @return None
 */
void AdcAlcSel(ALC_CHANNEL AlcSel);

/**
 * @brief  the remaining length of data in memory, unit:samples
 * @param  None
 * @return the remaining length of data
 */
uint32_t AdcPmemPcmRemainLenGet(void);

/**
 * @brief  read ADC data from PMEM
 * @param  Buffer  read data to this buffer, and all pcm data fill in WAV format
 * @param  NumSamplesRequired read data in samples
 * @param  PcmDataMode 0x00: DstBuffer[]= 0 0 ... 0(half)0 0 ... 0(half)
 *                           fill zero into buf
 *                     0x01: DstBuffer[]= L L ... L 
 *                           only read left ch pcm data into buf
 *                     0x02: DstBuffer[]= R R ... R
 *                           only read right ch pcm data into buf
 *                     0x03: DstBuffer[]= L R L R ... L R
 *                           read both ch pcm data into buf
 *                     0x04: DstBuffer[]= L L ... L(half)R R ... R(half)
 *                           read Left ch pcm data into top half buf, read Left ch pcm data into bottom half buf
 * @return The actual read data length
 */
uint32_t AdcPcmDataRead(uint8_t* Buffer, uint32_t NumSamplesRequired, uint32_t PcmDataMode);



/**
 * @brief  initialize analog section of audio ADC module
 * @param  NONE
 * @return NONE
 */
void CodecAdcAnaInit(void);

/**
 * @brief  deinitialize analog section of audio ADC module
 * @param  NONE
 * @return NONE
 */
void CodecAdcAnaDeInit(void);

/**
 * @brief  sel adc channel input
 * @param  AdcChannelSel LINE IN,FM or MIC
 * @return NONE
 */
bool CodecAdcChannelSel(uint16_t AdcChannelSel);

/**
 * @brief  Adc part MIC channel gain set
 * @param  VolLeft	-> [0x00:42.0dB, 0x3F:-18.0dB]
 * @param  VolLeft	-> [0x00:42.0dB, 0x3F:-18.0dB]
 * @return NONE
 */
void CodecAdcMicGainConfig(uint8_t VolLeft, uint8_t VolRight);

/**
 * @brief  Adc part LINE IN channel gain set
 * @param  VolLeft	-> [0x00: 5.34dB, 0x3F:-61.44dB]
 * @param  VolLeft	-> [0x00: 5.34dB, 0x3F:-61.44dB]
 * @return NONE
 */
void CodecAdcLineInGainConfig(uint8_t VolLeft, uint8_t VolRight);

/**
 * @brief  Adc part FM channel gain set
 * @param  VolLeft	-> [0x00: 21.4dB, 0x3F:-53.3dB]
 * @param  VolLeft	-> [0x00: 21.4dB, 0x3F:-53.3dB]
 * @return NONE
 */
void CodecAdcFmGainConfig(uint8_t VolLeft, uint8_t VolRight);


//config analog reg for sigma detla adc line in path
void AsdmAnaLinePathConfig(void);

extern void PcmDataExtract(uint16_t* DstPcmData0, uint16_t* DstPcmData1, uint16_t* SrcPcmData, uint32_t NumSamplesRequired, uint32_t PcmDataMode);

/**
 * @brief  adc write pmem point get
 * @param  NONE
 * @return NONE
 */
uint16_t AdcMixCurrWritePointGet(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif   //__S_ADC_H__
