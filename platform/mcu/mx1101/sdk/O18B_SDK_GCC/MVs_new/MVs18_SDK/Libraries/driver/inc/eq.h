/**
 **************************************************************************************
 * @file    eq.h
 * @brief   EQ part of CODEC
 *
 * @author  Aissen Li
 * @version V6.0.0
 *
 * $Created: 2012-10-22 15:39:09$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __EQ_H__
#define __EQ_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define SW_EQ_PROC_FRAME_SAMPLES    (192)
#define MAX_SW_EQ_FILTERS_COUNT     (10 )
#define MAX_SW_EQ_FILTER_ORDER      (3  )

/**
 * @brief EQ filter type set
 */
typedef enum _EQ_FILTER_TYPE_SET
{
    EQ_FILTER_TYPE_UNKNOW = -1,
    EQ_FILTER_TYPE_PEAKING,
    EQ_FILTER_TYPE_LOW_SHELF,
    EQ_FILTER_TYPE_HIGH_SHELF,
    EQ_FILTER_TYPE_LOW_PASS,
    EQ_FILTER_TYPE_HIGH_PASS,
    EQ_FILTER_TYPE_BAND_PASS
} EQ_FILTER_TYPE_SET;

/**
 * @brief EQ filter parameters struct
 */
typedef struct _EqFilterParams
{
	int16_t    type;          /**< filter type, @see EQ_FILTER_TYPE_SET                               */
	uint16_t   f0;            /**< center frequency (peak) or mid-point frequency (shelf)             */
	int16_t    Q;             /**< quality factor (peak) or slope (shelf), format: Q6.10              */
	uint16_t   sqrtA;         /**< Sqrt root value of Amplitude, Gain = 20*log10(A*A), format: Q6.10  */
} EqFilterParams;

/**
 * @brief EQ filter coefficients saved as: [b0][b1][b2][a1][a2], [a0] equal to 1.0
 */
typedef struct _EqFilterCoefs
{
	int32_t b0: 32;
	int32_t b1: 32;
	int32_t b2: 32;
	int32_t a1: 32;
	int32_t a2: 32;
	int32_t   : 0;
} EqFilterCoefs;

/**
 * @brief  Configure EQ style parameters
 * @param  SamplingRate sample rate8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000 are supported.
 * @param  FilterParams EQ style filters parameters
 * @param  FilterNum    EQ style filters count, the value can be 0~5
 * @param  PreGain      EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void EqStyleConfigure(uint16_t SamplingRate, EqFilterParams* FilterParams, uint32_t FilterNum, uint32_t PreGain);

/**
 * @brief  Set EQ style pre-gain
 * @param  PreGain EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void EqPreGainSet(uint32_t PreGain);

/**
 * @brief  Get EQ style pre-gain
 * @return PreGain EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
uint32_t EqPreGainGet(void);

/**
 * @brief  Enable EQ
 * @param  None
 * @return None
 */
void EqEnable(void);

/**
 * @brief  Disable EQ
 * @param  None
 * @return None
 */
void EqDisable(void);

/**
 * @brief  Check whether eq is enabled
 * @param  NONE
 * @return TRUE:enable, FALSE:disable
 */
bool EqIsUsed(void);

/**
 * @brief  Clear hardware EQ delay buffer
 * @param  None
 * @return None
 */
void EqRamClear(void);

/***********************************************************************************************
 * The Following Code is Software EQ API definition.
 ***********************************************************************************************/

typedef struct _SwEqContext
{
    int16_t FiltersCount;
    int16_t WorkStatus;
    int32_t FiltersHistoryDataX[MAX_SW_EQ_FILTERS_COUNT][2/*#Channel*/][MAX_SW_EQ_FILTER_ORDER - 1];
    int32_t FiltersHistoryDataY[MAX_SW_EQ_FILTERS_COUNT][2/*#Channel*/][MAX_SW_EQ_FILTER_ORDER - 1];

    uint32_t  PreGain;

    EqFilterCoefs FiltersCoefs[MAX_SW_EQ_FILTERS_COUNT];

}SwEqContext;

typedef struct _SwEqWorkBuffer
{
    int32_t PcmBuffer[SW_EQ_PROC_FRAME_SAMPLES];
}SwEqWorkBuffer;

/**
 * @brief  Software EQ initialize
 * @param  SwEQ  Software EQ context pointer
 * @return None
 */
void SwEqInit(SwEqContext* SwEQ);

/**
 * @brief  Configure software EQ style parameters
 * @param  SwEQ         software EQ context pointer
 * @param  SamplingRate sample rate, 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000 are supported.
 * @param  FilterParams EQ style filters parameters
 * @param  FilterNum    EQ style filters count, value can be 0~MAX_SW_EQ_FILTERS_COUNT
 * @param  PreGain      EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void SwEqStyleConfigure(SwEqContext* SwEQ, uint16_t SamplingRate, EqFilterParams* FilterParams, uint32_t FilterNum, uint32_t PreGain);

/**
 * @brief  Apply software EQ
 * @param  SwEQ             Software EQ context pointer
 * @param  Buffer           Software EQ work buffer pointer, this buffer can be free after calling function SwEqApply done.
 * @param  PcmDataInOut     Address for both input and output pcm samples
 * @param  PcmSamplesCount  Pcm samples count, value can be 0~SW_EQ_PROC_FRAME_SAMPLES
 * @param  NumChannels      Number of pcm samples channels, value can be 1 or 2
 * @return NONE
 */
void SwEqApply(SwEqContext* SwEQ, SwEqWorkBuffer* Buffer, int16_t* PcmDataInOut, int32_t PcmSamplesCount, int32_t NumChannels);

/**
 * @brief  Set software EQ style pre-gain
 * @param  SwEQ             Software EQ context pointer
 * @param  PreGain software EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void SwEqPreGainSet(SwEqContext* SwEQ, uint32_t PreGain);

/**
 * @brief  Clear software EQ delay buffer
 * @param  SwEQ  Software EQ context pointer
 * @return None
 */
void SwEqRamClear(SwEqContext* SwEQ);

/**
 * @brief  Enable software EQ
 * @param  SwEQ  Software EQ context pointer
 * @return None
 */
void SwEqEnable(SwEqContext* SwEQ);

/**
 * @brief  Disable software EQ
 * @param  SwEQ  Software EQ context pointer
 * @return None
 */
void SwEqDisable(SwEqContext* SwEQ);

/**
 * @brief  Check if software EQ is working
 * @param  SwEQ  Software EQ context pointer
 * @return None
 */
bool SwEqIsEnabled(SwEqContext* SwEQ);

/***********************************************************************************************
 * The Following Code is Mixed (Software+Hardware) EQ API definition.
 ***********************************************************************************************/

typedef SwEqContext     MixedEqContext;
typedef SwEqWorkBuffer  MixedEqWorkBuffer;

/**
 * @brief  Mixed EQ initialize
 * @param  MixedEQ  Mixed EQ context pointer
 * @return None
 */
void MixedEqInit(MixedEqContext* MixedEQ);

/**
 * @brief  Configure mixed EQ style parameters
 * @param  MixedEQ      Mixed EQ context pointer
 * @param  SamplingRate sample rate, 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000 are supported.
 * @param  FilterParams EQ style filters parameters
 * @param  FilterNum    EQ style filters count, value can be 0~MAX_SW_EQ_FILTERS_COUNT
 * @param  PreGain      EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void MixedEqStyleConfigure(MixedEqContext* MixedEQ, uint16_t SamplingRate, EqFilterParams* FilterParams, uint32_t FilterNum, uint32_t PreGain);

/**
 * @brief  Apply mixed EQ
 * @param  MixedEQ          Mixed EQ context pointer
 * @param  Buffer           Software EQ work buffer pointer, this buffer can be free after calling function SwEqApply done.
 * @param  PcmDataInOut     Address for both input and output pcm samples
 * @param  PcmSamplesCount  Pcm samples count, value can be 0~SW_EQ_PROC_FRAME_SAMPLES
 * @param  NumChannels      Number of pcm samples channels, value can be 1 or 2
 * @return NONE
 */
void MixedEqApply(MixedEqContext* MixedEQ, MixedEqWorkBuffer* Buffer, int16_t* PcmDataInOut, int32_t PcmSamplesCount, int32_t NumChannels);

/**
 * @brief  Set mixed EQ style pre-gain
 * @param  MixedEQ          Mixed EQ context pointer
 * @param  PreGain software EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void MixedEqPreGainSet(MixedEqContext* MixedEQ, uint32_t PreGain);

/**
 * @brief  Clear mixed EQ delay buffer
 * @param  MixedEQ  Mixed EQ context pointer
 * @return None
 */
void MixedEqRamClear(MixedEqContext* MixedEQ);

/**
 * @brief  Enable mixed EQ
 * @param  MixedEQ  Mixed EQ context pointer
 * @return None
 */
void MixedEqEnable(MixedEqContext* MixedEQ);

/**
 * @brief  Disable mixed EQ
 * @param  MixedEQ  Mixed EQ context pointer
 * @return None
 */
void MixedEqDisable(MixedEqContext* MixedEQ);

/**
 * @brief  Check if mixed EQ is working
 * @param  MixedEQ  Mixed EQ context pointer
 * @return None
 */
bool MixedEqIsEnabled(MixedEqContext* MixedEQ);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__EQ_H__
