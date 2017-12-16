/**
 **************************************************************************************
 * @file    pcm_play.h
 * @brief   PCM Play API
 *
 * @author  Aissen Li
 * @version V1.0.0
 *
 * $Created: 2013-05-09 15:47:04$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __PCM_PLAY_H__
#define __PCM_PLAY_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief Play PCM data initialize
 * @param SampleRate play sampling rate
 * @param Channels   play pcm data channels, the value can only be 1 or 2 means mono or stereo
 * @param DacFlag    use internal or external DAC flag,
 *     @arg  0: means using internal DAC
 *     @arg  1: means using external DAC
 * @return NULL
 *
 * @note if use external DAC, GPIO_A.Pin14 ~ GPIO_A.Pin17 used for SampleRate control
 */
void PcmPlayInit(uint32_t SampleRate, uint32_t Channels, bool DacFlag);

/**
 * @brief Play PCM data, send transfer PCM data to hardware, and blocked util transfer done.
 * @param PcmData   must be 16 bits data, and when Channels is 1(mono), the PcmData must be continus
 *                   when Channels is 2(stereo), the PcmData must be interleaving & lcontinus
 * @param PcmDataLen Pcm data size in byte unit.
 * @return NULL
 */
void PcmPlay(void* PcmData, uint32_t PcmDataLen);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__PCM_PLAY_H__
