/**
 **************************************************************************************
 * @file    pcm_transfer.h
 *
 * @author  Aissen Li
 * @version V2.0.1
 * @date    2012/05/09
 *
 * @brief   PCM Transfer Module Functions API
 **************************************************************************************
 *
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __PCM_TRANSFER_H__
#define __PCM_TRANSFER_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "audio_utility.h"

typedef enum _PCM_TX_ERROR_CODE
{
	PCM_TX_INVALID_PCM_ADDR = -255,
	PCM_TX_OK = 0
} PCM_TX_ERROR_CODE;

/**
 * @brief  Initialize PCM transfer module with default PCM FIFO start address and size.
 *          if the default PCM FIFO start address and size were not suitable, please call
 *          PcmFifoInit(...) or PcmFifoInitialize(...) function after PcmTxInitWithDefaultPcmFifoSize(...)
 *          to re-initialize PCM FIFO start address and size
 * @param  Mode          PCM data layout, @see PCM_DATA_MODE
 * @return initialize error code
 *     @arg PCM_TX_OK                 initialize success without no error
 *     @arg @see PCM_FIFO_ERROR_CODE  pcm fifo initialize error code
 */
int32_t PcmTxInitWithDefaultPcmFifoSize(PCM_DATA_MODE Mode);

/**
 * @brief  Transfer PCM Data, send a transfer signal to hardware, but not blocked to transfer done
 * @param  Ch0Addr address of left  channel PCM data
 * @param  Ch1Addr address of right channel PCM data
 * @param  PcmSamples samples for each channel
 * @return transfer error code
 *     @arg PCM_TX_OK                 transfer success without no error
 *     @arg PCM_TX_INVALID_PCM_ADDR   invalid transfer PCM data address
 *
 * @note   If transfer for all mono pcm data (such as DATA_MODE_MONO_XXX), and some stereo pcm data stored with
 *         interleave mode (such as DATA_MODE_STEREO_LR, DATA_MODE_STEREO_RL)£¬ parameter Ch1Addr will be
 *         ignored.
 */
int32_t PcmTxTransferData(void* Ch0Addr, void* Ch1Addr, uint32_t PcmSamples);

/**
 * @brief  Transfer PCM data for all mono pcm data (such as DATA_MODE_MONO_XXX), and some stereo pcm data stored with
 *         interleave mode (such as DATA_MODE_STEREO_LR, DATA_MODE_STEREO_RL)
 * @param  PcmAddr address of left channel PCM data
 * @param  PcmSamples samples for each channel
 * @return transfer error code
 *     @arg PCM_TX_OK                 transfer success without no error
 *     @arg PCM_TX_INVALID_PCM_ADDR   invalid transfer PCM data address
 */
#define PcmTxTransferData1(PcmAddr, PcmSamples)            PcmTxTransferData(PcmAddr, PcmAddr, PcmSamples)

/**
 * @brief  Transfer PCM data for stereo pcm data stored with split mode (such as DATA_MODE_STEREO_HIGH, DATA_MODE_STEREO_LOW, DATA_MODE_STEREO_S)
 * @param  Ch0Addr address of left  channel PCM data
 * @param  Ch1Addr address of right channel PCM data
 * @param  PcmSamples samples for each channel
 * @return transfer error code
 *     @arg PCM_TX_OK                 transfer success without no error
 *     @arg PCM_TX_INVALID_PCM_ADDR   invalid transfer PCM data address
 */
#define PcmTxTransferData2(Ch0Addr, Ch1Addr, PcmSamples)   PcmTxTransferData(Ch0Addr, Ch1Addr, PcmSamples)

/**
 * @brief  Check whether PCM data transfer done
 * @param  NONE
 * @return PCM data transfer done flag, 0: not done, 1: done
 */
bool PcmTxIsTransferDone(void);

/**
 * @brief  Clear PCM transfer done signal
 * @param  NONE
 * @return NONE
 */
void PcmTxDoneSignalClear(void);

/**
 * @brief  Set PCM data mode
 * @param  Mode  PCM data layout, @see PCM_DATA_MODE
 * @return NONE
 */
void PcmTxSetPcmDataMode(PCM_DATA_MODE Mode);

/**
 * @brief  Get PCM data mode
 * @param  NONE
 * @return PCM data layout, @see PCM_DATA_MODE
 */
PCM_DATA_MODE PcmTxGetPcmDataMode(void);

/*********************************************************************************
 * FIXME: The follow code may not be used in furture SDK, remove them in that case
 *********************************************************************************/

/**
 * @brief  Set PCM Tx gain
 * @param  gain format is Q16.16, default value 0x10000 means 0dB
 * @return NONE
 */
void PcmTxSetGain(uint32_t Gain);

/**
 * @brief  Get PCM Tx gain
 * @param  NONE
 * @return Gain data, format is Q16.16, default value 0x10000 means 0dB
 */
uint32_t PcmTxGetGain(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__PCM_TRANSFER_H__
