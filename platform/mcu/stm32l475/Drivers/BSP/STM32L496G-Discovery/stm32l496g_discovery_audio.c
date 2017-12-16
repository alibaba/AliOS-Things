/**
  ******************************************************************************
  * @file    stm32l496g_discovery_audio.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the
  *          Audio driver for the STM32L496G-Discovery board.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/*==============================================================================
                                 User NOTES

1. How To use this driver:
--------------------------
   + This driver supports STM32L4xx devices on STM32L496G-Discovery (MB1261) Discovery boards.
        a) to play an audio file through headset. All functions names start by BSP_AUDIO_OUT_xxx.
        b) to record an audio file through digital microphones (MP34DT01TR ST mems)
           or analog microphone (headset microphone). All functions names start by BSP_AUDIO_IN_xxx.

a) PLAY A FILE:
==============
   + Call the function BSP_AUDIO_OUT_Init(
                                    OutputDevice: physical output mode (only OUTPUT_DEVICE_HEADPHONE).
                                    Volume      : Initial volume to be set (0 is min (mute), 100 is max (100%)
                                    AudioFreq   : Audio frequency in Hz (8000, 16000, 22500, 32000...)
                                                  this parameter is relative to the audio file/stream type.
                                   )
      This function configures all the hardware required for the audio application (codec, I2C, SAI,
      GPIOs, DMA and interrupt if needed). This function returns AUDIO_OK if configuration is OK.
      If the returned value is different from AUDIO_OK or the function is stuck then the communication with
      the audio codec has failed.
      - OUTPUT_DEVICE_HEADPHONE: only Headphone output is available on this board.

   + Call the function BSP_AUDIO_OUT_RegisterCallbacks to register user callbacks
     required to manage audio data streaming towards the audio codec (ErrorCallback(),
     HalfTransfer_CallBack() and TransferComplete_CallBack()).

   + Call the function BSP_AUDIO_OUT_Play() to start audio playback (for the first time).
   + Call the function BSP_AUDIO_OUT_Pause() to pause audio playback.
   + Call the function BSP_AUDIO_OUT_Resume() to resume audio playback.
       Note. After calling BSP_AUDIO_OUT_Pause() function for pause, only BSP_AUDIO_OUT_Resume() should be called
          for resume (it is not allowed to call BSP_AUDIO_OUT_Play() in this case).
       Note. This function should be called only when the audio file is played or paused (not stopped).
   + Call the function BSP_AUDIO_OUT_Stop() to stop audio playback.
   + To modify the volume level, the sampling frequency, the device output mode,
      the mute status or the audio configuration or the stop, use the functions: BSP_AUDIO_OUT_SetVolume(),
      AUDIO_OUT_SetFrequency(), BSP_AUDIO_OUT_SetOutputMode(), BSP_AUDIO_OUT_SetMute()and
      BSP_AUDIO_OUT_ChangeAudioConfig().

Driver architecture:
--------------------
   + This driver provides the audio layer high level API: it consists in functions
     exported in the stm32l496g_discovery_audio.h file (e.g. BSP_AUDIO_OUT_Init(),
     BSP_AUDIO_OUT_Play(), ...).
   + This driver also includes the Media Access Layer (MAL): it consists in
     functions allowing to access setup the audio devices. These functions
     are  included as local functions into the stm32l496g_discovery_audio.c file
     (e.g. AUDIO_SAIx_Init()).

Known Limitations:
------------------
   1- Communication with the audio codec (through I2C) may be corrupted if it is interrupted by some
      user interrupt routines (in this case, interrupts could be disabled just before the start of
      communication then re-enabled when it is over). Note that this communication is only done at
      the configuration phase (BSP_AUDIO_OUT_Init() or BSP_AUDIO_OUT_Stop()) and when Volume control modification is
      performed (BSP_AUDIO_OUT_SetVolume() or BSP_AUDIO_OUT_SetMute()or BSP_AUDIO_OUT_SetOutputMode()).
      When the audio data is played, no communication is required with the audio codec.
   2- Parsing of audio file is not implemented (in order to determine audio file properties: Mono/Stereo, Data size,
      File size, Audio Frequency, Audio Data header size ...). The configuration is fixed for the given audio file.
   3- Supports only 16-bits audio data size.

b) RECORD A FILE:
================
   + Call the function BSP_AUDIO_IN_InitEx(
                                    InputDevice: physical input mode (INPUT_DEVICE_DIGITAL_MIC
                                                  INPUT_DEVICE_DIGITAL_MIC1, INPUT_DEVICE_DIGITAL_MIC2
                                                  or INPUT_DEVICE_ANALOG_MIC)
                                    AudioFreq: Audio frequency in Hz (8000, 16000, 22500, 32000 ...)
                                    )
      This function configures all the hardware required for the audio application (DFSDM or SAI,
      GPIOs, DMA and interrupt if needed). This function returns AUDIO_OK if the
      configuration completes successfully.
      - INPUT_DEVICE_DIGITAL_MIC: Record from digital microphones mounted on board.
      - INPUT_DEVICE_DIGITAL_MIC1: Record from digital microphone 1 mounted on board (left microphone).
      - INPUT_DEVICE_DIGITAL_MIC2: Record from digital microphone 2 mounted on board (right microphone).
      - INPUT_DEVICE_ANALOG_MIC: Record from headset microphone.

   + Call the function BSP_AUDIO_IN_RegisterCallbacks to register user callbacks
     used to stream audio data toward the record buffer (ErrorCallback(),
     HalfTransfer_CallBack() and TransferComplete_CallBack()).

   + Call the function BSP_AUDIO_IN_Record(
                            pbuf Main buffer pointer for the recorded data storing
                            size Current size of the recorded buffer
                            )
      to start recording from the microphone.

   + Call the function BSP_AUDIO_IN_STOP() to stop recording.
==============================================================================*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32l496g_discovery_audio.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_AUDIO STM32L496G-DISCOVERY AUDIO
  * @brief This file includes the low layer driver for cs42l51 Audio Codec
  *        available on STM32L496G-Discovery board (MB1261).
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_AUDIO_Private_Types Private Types
  * @{
  */
typedef struct
{
  AUDIO_DrvTypeDef     *AudioDrv;           /* Audio codec driver */
  uint32_t              OutputDevice;       /* Output device */
  uint32_t              Frequency;          /* Playback frequency */
  uint32_t              Volume;             /* Playback volume */
  Audio_CallbackTypeDef CbError;            /* pointer to the callback function invoked when error occurs */
  Audio_CallbackTypeDef CbHalfTransfer;     /* pointer to the callback function invoked when half transfer occurs */
  Audio_CallbackTypeDef CbTransferComplete; /* pointer to the callback function invoked when transfer complete occurs */
} AUDIO_OUT_TypeDef;

typedef struct
{
  AUDIO_DrvTypeDef           *AudioDrv;           /* Audio codec driver */
  DFSDM_Channel_HandleTypeDef hDfsdmLeftChannel;  /* DFSDM channel handle used for left channel */
  DFSDM_Channel_HandleTypeDef hDfsdmRightChannel; /* DFSDM channel handle used for right channel */
  DMA_HandleTypeDef           hDmaDfsdmLeft;      /* DMA handle used for DFSDM regular conversions on left channel */
  DMA_HandleTypeDef           hDmaDfsdmRight;     /* DMA handle used for DFSDM regular conversions on right channel */
  int32_t                    *LeftRecBuff;        /* Buffers for left samples */
  int32_t                    *RightRecBuff;       /* Buffers for right samples */
  uint32_t                    InputDevice;        /* Input device */
  uint32_t                    Frequency;          /* Record Frequency */
  uint32_t                    BitResolution;      /* Record bit resolution */
  uint32_t                    ChannelNbr;         /* Record Channel Number */
  uint16_t                   *pRecBuf;            /* Pointer to record user buffer */
  uint32_t                    RecSize;            /* Size to record in mono, double size to record in stereo */
  Audio_CallbackTypeDef       CbError;            /* pointer to the callback function invoked when a DMA transfer fails */
  Audio_CallbackTypeDef       CbHalfTransfer;     /* pointer to the callback function invoked when half of the DMA transfer is completed */
  Audio_CallbackTypeDef       CbTransferComplete; /* pointer to the callback function invoked when the DMA transfer is completed */
} AUDIO_IN_TypeDef;

/**
  * @}
  */

/* Private defines ------------------------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_AUDIO_Private_Constants Private Constants
  * @{
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_AUDIO_Private_Macros Private Macros
  * @{
  */
/*### PLAY ###*/
/* SCK(kHz) = SAI_CK_x/(SAIClockDivider*2*256) */
#define SAIClockDivider(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 12 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 2 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 6 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 1 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 3 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 0 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 2 : 1  \
 
/*### RECORD ###*/
#define DFSDMOverSampling(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 256 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 256 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 128 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 128 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 64 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 64  \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 32 : 16  \
 
#define DFSDMClockDivider(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 24 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 4 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 24 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 4 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 24 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 4  \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 32 : 32  \
 
#define DFSDMFilterOrder(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? DFSDM_FILTER_SINC4_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? DFSDM_FILTER_SINC3_ORDER  \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? DFSDM_FILTER_SINC4_ORDER : DFSDM_FILTER_SINC5_ORDER  \
 
#define DFSDMRightBitShift(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 8 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 8 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 6 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 6 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 8 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 2 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 4 : 4  \
 
/* Saturate the record PCM sample */
#define SaturaLH(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_AUDIO_Private_Variables Private Variables
  * @{
  */
/* Audio output context information */
static AUDIO_OUT_TypeDef hAudioOut = {0};

/* Audio input context information */
static AUDIO_IN_TypeDef hAudioIn = {0};

/* SAI DMA handle */
static DMA_HandleTypeDef hDmaSaiTx;
static DMA_HandleTypeDef hDmaSaiRx;

static uint32_t DmaLeftRecHalfBuffCplt;
static uint32_t DmaLeftRecBuffCplt;
static uint32_t DmaRightRecHalfBuffCplt;
static uint32_t DmaRightRecBuffCplt;


/**
  * @}
  */

/* Exported variables ---------------------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_AUDIO_Exported_Variables Exported Variables
  * @{
  */
/* SAIx handle */
SAI_HandleTypeDef               BSP_AUDIO_hSai_Tx;
SAI_HandleTypeDef               BSP_AUDIO_hSai_Rx;

/* DFSDM filter handle */
DFSDM_Filter_HandleTypeDef      BSP_AUDIO_hDfsdmLeftFilter;
DFSDM_Filter_HandleTypeDef      BSP_AUDIO_hDfsdmRightFilter;
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_AUDIO_Private_Functions Private Functions
  * @{
  */
static uint8_t AUDIO_SAIx_Init(uint32_t AudioFreq);
static uint8_t AUDIO_SAIx_DeInit(void);
static uint8_t AUDIO_DFSDMx_Init(uint32_t AudioFreq);
static uint8_t AUDIO_DFSDMx_DeInit(void);
static uint8_t AUDIO_SAIPLLConfig(uint32_t AudioFreq);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup STM32L496G_DISCOVERY_AUDIO_Exported_Functions
  * @{
  */

/**
  * @brief  Configures the audio codec related peripherals.
  * @param  OutputDevice: OUTPUT_DEVICE_HEADPHONE.
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @retval BSP AUDIO status
  * @note   The SAI PLL input clock must be configure in the user application.
  *         The SAI PLL configuration done within this function assumes that
  *         the SAI PLL input clock runs at 8 MHz.
  */
uint8_t BSP_AUDIO_OUT_Init(uint16_t OutputDevice,
                           uint8_t  Volume,
                           uint32_t AudioFreq)
{
  /* Initialize the audio output context */
  hAudioOut.AudioDrv           = &cs42l51_drv;
  hAudioOut.OutputDevice       = OutputDevice;
  hAudioOut.Frequency          = AudioFreq;
  hAudioOut.Volume             = Volume;
  hAudioOut.CbError            = (Audio_CallbackTypeDef)NULL;
  hAudioOut.CbHalfTransfer     = (Audio_CallbackTypeDef)NULL;
  hAudioOut.CbTransferComplete = (Audio_CallbackTypeDef)NULL;

  /* Check if input device is currently used */
  if (hAudioIn.InputDevice != 0)
  {
    /* If input device is currently used, SAI PLL is already initialized */
    /* Check that AudioFreq for record and playback is the same */
    if (hAudioIn.Frequency != hAudioOut.Frequency)
    {
      return AUDIO_ERROR;
    }
  }
  else
  {
    /* Configure the SAI PLL according to the requested audio frequency */
    if (AUDIO_SAIPLLConfig(AudioFreq) != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }

  /* If input device is analogic mic, SAI is already initialized */
  if (hAudioIn.InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    /* SAI data transfer preparation: prepare the Media to be used for the audio
    transfer from memory to SAI peripheral. */
    if (AUDIO_SAIx_Init(AudioFreq) != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }

  /* Initialize the audio codec internal registers */
  if (hAudioOut.AudioDrv->Init(AUDIO_I2C_ADDRESS,
                               (hAudioOut.OutputDevice | hAudioIn.InputDevice),
                               Volume,
                               AudioFreq) != 0)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  De-Initializes audio codec related peripherals
  * @retval BSP AUDIO status

  */
uint8_t BSP_AUDIO_OUT_DeInit(void)
{
  if (hAudioIn.InputDevice == INPUT_DEVICE_ANALOG_MIC)
  {
    /* Reset playback path on audio codec */
    if (hAudioIn.AudioDrv->Init(AUDIO_I2C_ADDRESS,
                                hAudioIn.InputDevice,
                                (uint8_t) hAudioOut.Volume,
                                hAudioIn.Frequency) != 0)
    {
      return AUDIO_ERROR;
    }
  }
  else
  {
    /* De-initializes SAI interface */
    if (AUDIO_SAIx_DeInit() != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }

    /* DeInit audio codec */
    hAudioOut.AudioDrv->DeInit();
  }

  /* Disable SAI PLL if no more device is used */
  if (hAudioIn.InputDevice == 0)
  {
    if (AUDIO_SAIx_PLL_DISABLE() != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }

  /* Reset the audio output context */
  memset(&hAudioOut, 0, sizeof(hAudioOut));

  return AUDIO_OK;
}

/**
  * @brief  Starts playing audio stream from a data buffer for a determined size.
  * @param  pData: pointer on PCM samples buffer
  * @param  Size: Number of audio data HALF WORD.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_Play(uint16_t *pData, uint32_t Size)
{
  /* Initiate a DMA transfer of PCM samples towards the serial audio interface */
  if (HAL_SAI_Transmit_DMA(&BSP_AUDIO_hSai_Tx, (uint8_t *)pData, DMA_MAX(Size)) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  /* Call the audio Codec Play function */
  if (hAudioOut.AudioDrv->Play(AUDIO_I2C_ADDRESS, pData, Size) != 0)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  Sends n-Bytes on the SAI interface.
  * @param  pData: pointer on PCM samples buffer
  * @param  Size: number of data to be written
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size)
{
  /* Initiate a DMA transfer of PCM samples towards the serial audio interface */
  if (HAL_SAI_Transmit_DMA(&BSP_AUDIO_hSai_Tx, (uint8_t *)pData, Size) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  This function Pauses the audio file stream. In case
  *         of using DMA, the DMA Pause feature is used.
  * @note When calling BSP_AUDIO_OUT_Pause() function for pause, only
  *       BSP_AUDIO_OUT_Resume() function should be called for resume
  *       (use of BSP_AUDIO_OUT_Play() function for resume could lead
  *       to unexpected behavior).
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_Pause(void)
{
  /* Call the Audio Codec Pause function */
  if (hAudioOut.AudioDrv->Pause(AUDIO_I2C_ADDRESS) != 0)
  {
    return AUDIO_ERROR;
  }

  /* Pause DMA transfer of PCM samples towards the serial audio interface */
  if (HAL_SAI_DMAPause(&BSP_AUDIO_hSai_Tx) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  This function  Resumes the audio file stream.
  * @note When calling BSP_AUDIO_OUT_Pause() function for pause, only
  *       BSP_AUDIO_OUT_Resume() function should be called for resume
  *       (use of BSP_AUDIO_OUT_Play() function for resume could lead to
  *       unexpected behavior).
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_Resume(void)
{
  /* Call the Audio Codec Resume function */
  if (hAudioOut.AudioDrv->Resume(AUDIO_I2C_ADDRESS) != 0)
  {
    return AUDIO_ERROR;
  }

  /* Resume DMA transfer of PCM samples towards the serial audio interface */
  if (HAL_SAI_DMAResume(&BSP_AUDIO_hSai_Tx) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  Stops audio playing and Power down the Audio Codec.
  * @param  Option: could be one of the following parameters
  *           - CODEC_PDWN_SW: for software power off (by writing registers).
  *                            Then no need to reconfigure the Codec after power on.
  *           - CODEC_PDWN_HW: completely shut down the codec (physically).
  *                            Then need to reconfigure the Codec after power on.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_Stop(uint32_t Option)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Option);

  /* Call Audio Codec Stop function */
  if (hAudioOut.AudioDrv->Stop(AUDIO_I2C_ADDRESS, Option) != 0)
  {
    return AUDIO_ERROR;
  }

  /* Wait at least 100ms */
  HAL_Delay(100);

  /* Stop DMA transfer of PCM samples towards the serial audio interface */
  if (HAL_SAI_DMAStop(&BSP_AUDIO_hSai_Tx) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  Controls the current audio volume level.
  * @param  Volume: Volume level to be set in percentage from 0% to 100% (0 for
  *         Mute and 100 for Max volume level).
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume)
{
  /* Call the codec volume control function with converted volume value */
  if (hAudioOut.AudioDrv->SetVolume(AUDIO_I2C_ADDRESS, Volume) != 0)
  {
    return AUDIO_ERROR;
  }

  hAudioOut.Volume = Volume;

  return AUDIO_OK;
}

/**
  * @brief  Enables or disables the MUTE mode by software
  * @param  Cmd: Could be AUDIO_MUTE_ON to mute sound or AUDIO_MUTE_OFF to
  *         unmute the codec and restore previous volume level.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd)
{
  /* Call the Codec Mute function */
  if (hAudioOut.AudioDrv->SetMute(AUDIO_I2C_ADDRESS, Cmd) != 0)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  Switch dynamically (while audio file is being played) the output
  *          target (speaker or headphone).
  * @param  Output: The audio output target: OUTPUT_DEVICE_SPEAKER,
  *         OUTPUT_DEVICE_HEADPHONE or OUTPUT_DEVICE_BOTH
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_SetOutputMode(uint8_t Output)
{
  /* Call the Codec output device function */
  if (hAudioOut.AudioDrv->SetOutputMode(AUDIO_I2C_ADDRESS, Output) != 0)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  Updates the audio frequency.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   The SAI PLL input clock must be configure in the user application.
  *         The SAI PLL configuration done within this function assumes that
  *         the SAI PLL input clock runs at 8 MHz.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_OUT_SetFrequency(uint32_t AudioFreq)
{
  uint8_t TxData[2] = {0x00, 0x00};

  /* Configure the SAI PLL according to the requested audio frequency */
  if (AUDIO_SAIPLLConfig(AudioFreq) != AUDIO_OK)
  {
    return AUDIO_ERROR;
  }

  /* Disable SAI peripheral to allow access to SAI internal registers */
  __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Tx);
  __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Rx);

  /* Update the SAI audio frequency configuration */
  BSP_AUDIO_hSai_Tx.Init.Mckdiv = SAIClockDivider(AudioFreq);
  HAL_SAI_Init(&BSP_AUDIO_hSai_Tx);
  BSP_AUDIO_hSai_Rx.Init.Mckdiv = SAIClockDivider(AudioFreq);
  HAL_SAI_Init(&BSP_AUDIO_hSai_Rx);

  /* Enable SAI peripheral to generate MCLK */
  __HAL_SAI_ENABLE(&BSP_AUDIO_hSai_Tx);
  /* Transmit one byte to start FS generation */
  if (HAL_SAI_Transmit(&BSP_AUDIO_hSai_Tx, TxData, 2, 1000) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  hAudioOut.Frequency = AudioFreq;

  return AUDIO_OK;
}

/**
  * @brief  Changes the Audio Out Configuration.
  * @param  AudioOutOption: specifies the audio out new configuration
  *         This parameter can be any value of @ref BSP_Audio_Out_Option
  * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
  *         audio out configuration.
  * @retval None
  */
void BSP_AUDIO_OUT_ChangeAudioConfig(uint32_t AudioOutOption)
{
  uint8_t TxData[2] = {0x00, 0x00};

  /********** Playback Buffer circular/normal mode **********/
  if (AudioOutOption & BSP_AUDIO_OUT_CIRCULARMODE)
  {
    /* Deinitialize the Stream to update DMA mode */
    HAL_DMA_DeInit(BSP_AUDIO_hSai_Tx.hdmatx);

    /* Update the SAI audio Transfer DMA mode */
    BSP_AUDIO_hSai_Tx.hdmatx->Init.Mode = DMA_CIRCULAR;

    /* Configure the DMA Stream with new Transfer DMA mode */
    HAL_DMA_Init(BSP_AUDIO_hSai_Tx.hdmatx);
  }
  else /* BSP_AUDIO_OUT_NORMALMODE */
  {
    /* Deinitialize the Stream to update DMA mode */
    HAL_DMA_DeInit(BSP_AUDIO_hSai_Tx.hdmatx);

    /* Update the SAI audio Transfer DMA mode */
    BSP_AUDIO_hSai_Tx.hdmatx->Init.Mode = DMA_NORMAL;

    /* Configure the DMA Stream with new Transfer DMA mode */
    HAL_DMA_Init(BSP_AUDIO_hSai_Tx.hdmatx);
  }

  /********** Playback Buffer stereo/mono mode **********/
  if (AudioOutOption & BSP_AUDIO_OUT_STEREOMODE)
  {
    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Tx);

    /* Update the SAI audio frame slot configuration */
    BSP_AUDIO_hSai_Tx.Init.MonoStereoMode = SAI_STEREOMODE;
    HAL_SAI_Init(&BSP_AUDIO_hSai_Tx);

    /* Enable SAI peripheral to generate MCLK */
    __HAL_SAI_ENABLE(&BSP_AUDIO_hSai_Tx);
    /* Transmit one byte to start FS generation */
    HAL_SAI_Transmit(&BSP_AUDIO_hSai_Tx, TxData, 2, 1000);
  }
  else /* BSP_AUDIO_OUT_MONOMODE */
  {
    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Tx);

    /* Update the SAI audio frame slot configuration */
    BSP_AUDIO_hSai_Tx.Init.MonoStereoMode = SAI_MONOMODE;
    HAL_SAI_Init(&BSP_AUDIO_hSai_Tx);

    /* Enable SAI peripheral to generate MCLK */
    __HAL_SAI_ENABLE(&BSP_AUDIO_hSai_Tx);
    /* Transmit one byte to start FS generation */
    HAL_SAI_Transmit(&BSP_AUDIO_hSai_Tx, TxData, 2, 1000);
  }
}

/**
  * @brief  register user callback functions
  * @param  ErrorCallback: pointer to the error callback function
  * @param  HalfTransferCallback: pointer to the half transfer callback function
  * @param  TransferCompleteCallback: pointer to the transfer complete callback function
  * @retval None
  */
void BSP_AUDIO_OUT_RegisterCallbacks(Audio_CallbackTypeDef ErrorCallback,
                                     Audio_CallbackTypeDef HalfTransferCallback,
                                     Audio_CallbackTypeDef TransferCompleteCallback)
{
  hAudioOut.CbError            = ErrorCallback;
  hAudioOut.CbHalfTransfer     = HalfTransferCallback;
  hAudioOut.CbTransferComplete = TransferCompleteCallback;
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  hsai: SAI handle
  * @retval None
  */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Invoke the registered 'TransferComplete' function (if any) */
  if (hAudioOut.CbTransferComplete != (Audio_CallbackTypeDef)NULL)
  {
    hAudioOut.CbTransferComplete();
  }
}

/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  hsai: SAI handle
  * @retval None
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Invoke the registered 'HalfTransfer' callback function (if any) */
  if (hAudioOut.CbHalfTransfer != (Audio_CallbackTypeDef)NULL)
  {
    hAudioOut.CbHalfTransfer();
  }
}

/**
  * @brief  SAI error callbacks.
  * @param  hsai: SAI handle
  * @retval None
  */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  /* Invoke the registered 'ErrorCallback' callback function (if any) */
  if (hAudioOut.CbError != (Audio_CallbackTypeDef)NULL)
  {
    hAudioOut.CbError();
  }
  /* Invoke the registered 'ErrorCallback' callback function (if any) */
  if (hAudioIn.CbError != (Audio_CallbackTypeDef)NULL)
  {
    hAudioIn.CbError();
  }
}

/**
  * @}
  */

/** @addtogroup STM32L496G_EVAL_AUDIO_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes micropone related peripherals.
  * @note   This function assumes that the SAI input clock (through PLL_M)
  *         is already configured and ready to be used.
  * @param  AudioFreq: Audio frequency to be configured for the SAI peripheral.
  * @param  BitRes: Audio frequency to be configured for the SAI peripheral.
  * @param  ChnlNbr: Audio frequency to be configured for the SAI peripheral.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_IN_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
  return BSP_AUDIO_IN_InitEx(INPUT_DEVICE_DIGITAL_MIC, AudioFreq, BitRes, ChnlNbr);
}

/**
  * @brief  Initialize wave recording.
  * @param  InputDevice: INPUT_DEVICE_DIGITAL_MIC, INPUT_DEVICE_DIGITAL_MIC1,
  *                      INPUT_DEVICE_DIGITAL_MIC2 or INPUT_DEVICE_ANALOG_MIC.
  * @param  AudioFreq: Audio frequency to be configured.
  * @param  BitRes: Audio bit resolution to be configured..
  * @param  ChnlNbr: Number of channel to be configured.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_InitEx(uint16_t InputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
  /* Update the audio input context */
  hAudioIn.AudioDrv           = &cs42l51_drv;
  hAudioIn.InputDevice        = InputDevice;
  hAudioIn.Frequency          = AudioFreq;
  hAudioIn.BitResolution      = BitRes;
  hAudioIn.ChannelNbr         = ChnlNbr;
  hAudioIn.CbError            = (Audio_CallbackTypeDef)NULL;
  hAudioIn.CbHalfTransfer     = (Audio_CallbackTypeDef)NULL;
  hAudioIn.CbTransferComplete = (Audio_CallbackTypeDef)NULL;

  /* Check channel number according device : only record mono with analog mic and stereo with digital mic are allowed */
  if (((InputDevice == INPUT_DEVICE_DIGITAL_MIC) && (ChnlNbr == 1)) ||
      ((InputDevice == INPUT_DEVICE_DIGITAL_MIC1) && (ChnlNbr == 2)) ||
      ((InputDevice == INPUT_DEVICE_DIGITAL_MIC2) && (ChnlNbr == 2)) ||
      ((InputDevice == INPUT_DEVICE_ANALOG_MIC) && (ChnlNbr == 2)))
  {
    return AUDIO_ERROR;
  }

  /* Check if output device is currently used */
  if (hAudioOut.OutputDevice != 0)
  {
    /* If output device is currently used, SAI PLL is already initialized */
    /* Check that AudioFreq for record and playback is the same */
    if (hAudioIn.Frequency != hAudioOut.Frequency)
    {
      return AUDIO_ERROR;
    }
  }
  else
  {
    /* Configure the SAI PLL according to the requested audio frequency */
    if (AUDIO_SAIPLLConfig(AudioFreq) != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }

  if (InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    /* Initializes the Digital Filter for Sigma-Delta Modulators interface */
    if (AUDIO_DFSDMx_Init(AudioFreq) != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }
  else
  {
    /* INPUT_DEVICE_ANALOG_MIC */
    /* If output device is currently used, SAI is already initialized */
    if (hAudioOut.OutputDevice == 0)
    {
      /* SAI data transfer preparation: prepare the Media to be used for the audio
      transfer from SAI peripheral to memory. */
      if (AUDIO_SAIx_Init(AudioFreq) != AUDIO_OK)
      {
        return AUDIO_ERROR;
      }
    }

    /* Initialize the audio codec internal registers */
    if (hAudioIn.AudioDrv->Init(AUDIO_I2C_ADDRESS,
                                (hAudioOut.OutputDevice | hAudioIn.InputDevice),
                                hAudioOut.Volume,
                                AudioFreq) != 0)
    {
      return AUDIO_ERROR;
    }
  }

  /* Initialise transfer control flag */
  DmaLeftRecHalfBuffCplt  = 0;
  DmaLeftRecBuffCplt      = 0;
  DmaRightRecHalfBuffCplt = 0;
  DmaRightRecBuffCplt     = 0;

  return AUDIO_OK;
}

/**
  * @brief  De-Initializes microphone related peripherals.
  * @retval BSP AUDIO status

  */
uint8_t BSP_AUDIO_IN_DeInit(void)
{
  if (hAudioIn.InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    /* De-initializes the Digital Filter for Sigma-Delta Modulators interface */
    if (AUDIO_DFSDMx_DeInit() != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }
  else
  {
    /* INPUT_DEVICE_ANALOG_MIC */
    /* Check if output device is currently used */
    if (hAudioOut.OutputDevice != 0)
    {
      /* Reset record path on audio codec */
      if (hAudioOut.AudioDrv->Init(AUDIO_I2C_ADDRESS,
                                   hAudioOut.OutputDevice,
                                   (uint8_t) hAudioOut.Volume,
                                   hAudioOut.Frequency) != 0)
      {
        return AUDIO_ERROR;
      }
    }
    else
    {
      /* De-initializes SAI interface */
      if (AUDIO_SAIx_DeInit() != AUDIO_OK)
      {
        return AUDIO_ERROR;
      }

      /* DeInit audio codec */
      hAudioIn.AudioDrv->DeInit();
    }
  }

  /* Disable SAI PLL if no more device is used */
  if (hAudioOut.OutputDevice == 0)
  {
    if (AUDIO_SAIx_PLL_DISABLE() != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }

  /* Reset the audio input context */
  memset(&hAudioIn, 0, sizeof(hAudioIn));

  return AUDIO_OK;
}

/**
  * @brief  Starts audio recording.
  * @param  pbuf: Main buffer pointer for the recorded data storing
  * @param  size: Current size of the recorded buffer
  * @note   The Right channel is start at first with synchro on start of Left channel
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_IN_Record(uint16_t *pbuf, uint32_t size)
{
  hAudioIn.pRecBuf = pbuf;
  hAudioIn.RecSize = size;


  if (hAudioIn.InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
    {
      /* Allocate hAudioIn.LeftRecBuff buffer */
#if defined(BSP_AUDIO_USE_RTOS)
      hAudioIn.LeftRecBuff  = (int32_t *)k_malloc((size / hAudioIn.ChannelNbr) * sizeof(int32_t));
#else
      hAudioIn.LeftRecBuff  = (int32_t *)malloc((size / hAudioIn.ChannelNbr) * sizeof(int32_t));
#endif
      if (hAudioIn.LeftRecBuff == NULL)
      {
        return AUDIO_ERROR;
      }
    }
    
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
    {
      /* Allocate hAudioIn.RightRecBuff buffer */
#if defined(BSP_AUDIO_USE_RTOS)
      hAudioIn.RightRecBuff  = (int32_t *)k_malloc((size / hAudioIn.ChannelNbr) * sizeof(int32_t));
#else
      hAudioIn.RightRecBuff  = (int32_t *)malloc((size / hAudioIn.ChannelNbr) * sizeof(int32_t));
#endif
      if (hAudioIn.RightRecBuff == NULL)
      {
        return AUDIO_ERROR;
      }
    }
    
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
    {
      /* Call the Media layer start function for right channel */
      if (HAL_DFSDM_FilterRegularStart_DMA(&BSP_AUDIO_hDfsdmRightFilter,
                                           (int32_t *)hAudioIn.RightRecBuff,
                                           (hAudioIn.RecSize / hAudioIn.ChannelNbr)) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
    
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
    {
      /* Call the Media layer start function for left channel */
      if (HAL_DFSDM_FilterRegularStart_DMA(&BSP_AUDIO_hDfsdmLeftFilter,
                                           (int32_t *)hAudioIn.LeftRecBuff,
                                           (hAudioIn.RecSize / hAudioIn.ChannelNbr)) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
  }
  else
  {
    /* INPUT_DEVICE_ANALOG_MIC */
    /* Call the audio Codec Play function */
    if (hAudioIn.AudioDrv->Play(AUDIO_I2C_ADDRESS, pbuf, size) != 0)
    {
      return AUDIO_ERROR;
    }

    /* Start the process receive DMA */
    if (HAL_OK != HAL_SAI_Receive_DMA(&BSP_AUDIO_hSai_Rx, (uint8_t *)pbuf, size))
    {
      return AUDIO_ERROR;
    }
  }

  return AUDIO_OK;
}

/**
  * @brief  Updates the audio frequency.
  * @param  AudioFreq: Audio frequency used to record the audio stream.
  * @note   This API should be called after the BSP_AUDIO_IN_Init() to adjust the
  *         audio frequency.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_IN_SetFrequency(uint32_t AudioFreq)
{
  uint8_t TxData[2] = {0x00, 0x00};

  /* Configure the SAI PLL according to the requested audio frequency */
  if (AUDIO_SAIPLLConfig(AudioFreq) != AUDIO_OK)
  {
    return AUDIO_ERROR;
  }

  if (hAudioIn.InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    /* De-initializes the Digital Filter for Sigma-Delta Modulators interface */
    if (AUDIO_DFSDMx_DeInit() != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }

    /* Initializes the Digital Filter for Sigma-Delta Modulators interface */
    if (AUDIO_DFSDMx_Init(AudioFreq) != AUDIO_OK)
    {
      return AUDIO_ERROR;
    }
  }
  else
  {
    /* INPUT_DEVICE_ANALOG_MIC */
    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Tx);
    __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Rx);

    /* Update the SAI audio frequency configuration */
    BSP_AUDIO_hSai_Tx.Init.Mckdiv = SAIClockDivider(AudioFreq);
    HAL_SAI_Init(&BSP_AUDIO_hSai_Tx);
    BSP_AUDIO_hSai_Rx.Init.Mckdiv = SAIClockDivider(AudioFreq);
    HAL_SAI_Init(&BSP_AUDIO_hSai_Rx);

    /* Enable SAI peripheral to generate MCLK */
    __HAL_SAI_ENABLE(&BSP_AUDIO_hSai_Tx);
    /* Transmit one byte to start FS generation */
    if (HAL_SAI_Transmit(&BSP_AUDIO_hSai_Tx, TxData, 2, 1000) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  hAudioIn.Frequency = AudioFreq;

  return AUDIO_OK;
}

/**
  * @brief  Regular conversion complete callback.
  * @note   In interrupt mode, user has to read conversion value in this function
            using HAL_DFSDM_FilterGetRegularValue.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  uint32_t index;
  uint32_t recbufsize = (hAudioIn.RecSize / hAudioIn.ChannelNbr);

  if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC)
  {
    for (index = (recbufsize / 2); index < recbufsize; index++)
    {
      hAudioIn.pRecBuf[2 * index]       = (uint16_t)(SaturaLH((hAudioIn.LeftRecBuff[index] >> 8), -32768, 32767));
      hAudioIn.pRecBuf[(2 * index) + 1] = (uint16_t)(SaturaLH((hAudioIn.RightRecBuff[index] >> 8), -32768, 32767));
    }
  }
  else if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC1)
  {
    for (index = (recbufsize / 2); index < recbufsize; index++)
    {
      hAudioIn.pRecBuf[index] = (uint16_t)(SaturaLH((hAudioIn.LeftRecBuff[index] >> 8), -32768, 32767));
    }
  }
  else if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)
  {
    for (index = (recbufsize / 2); index < recbufsize; index++)
    {
      hAudioIn.pRecBuf[index] = (uint16_t)(SaturaLH((hAudioIn.RightRecBuff[index] >> 8), -32768, 32767));
    }
  }

  /* Invoke the registered 'TransferCompete' callback function (if any) */
  if (hAudioIn.CbTransferComplete != (Audio_CallbackTypeDef)NULL)
  {
    if (hdfsdm_filter == &BSP_AUDIO_hDfsdmLeftFilter)
    {
      if (DmaLeftRecBuffCplt)
      {
        BSP_ErrorHandler();
      }

      DmaLeftRecBuffCplt = 1;
    }
    else
    {
      if (DmaRightRecBuffCplt)
      {
        BSP_ErrorHandler();
      }

      DmaRightRecBuffCplt = 1;
    }

    if (((DmaLeftRecBuffCplt != 0) && (DmaRightRecBuffCplt != 0) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC)) ||
        ((DmaLeftRecBuffCplt != 0) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC1)) ||
        ((DmaRightRecBuffCplt != 0) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)))
    {
      hAudioIn.CbTransferComplete();
      DmaLeftRecBuffCplt = 0;
      DmaRightRecBuffCplt = 0;
    }
  }
}

/**
  * @brief  Half regular conversion complete callback.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  uint32_t index;
  uint32_t recbufsize = (hAudioIn.RecSize / hAudioIn.ChannelNbr);

  if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC)
  {
    for (index = 0; index < (recbufsize / 2); index++)
    {
      hAudioIn.pRecBuf[2 * index]       = (uint16_t)(SaturaLH((hAudioIn.LeftRecBuff[index] >> 8), -32768, 32767));
      hAudioIn.pRecBuf[(2 * index) + 1] = (uint16_t)(SaturaLH((hAudioIn.RightRecBuff[index] >> 8), -32768, 32767));
    }
  }
  else if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC1)
  {
    for (index = 0; index < (recbufsize / 2); index++)
    {
      hAudioIn.pRecBuf[index] = (uint16_t)(SaturaLH((hAudioIn.LeftRecBuff[index] >> 8), -32768, 32767));
    }
  }
  else if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)
  {
    for (index = 0; index < (recbufsize / 2); index++)
    {
      hAudioIn.pRecBuf[index] = (uint16_t)(SaturaLH((hAudioIn.RightRecBuff[index] >> 8), -32768, 32767));
    }
  }
  
  /* Invoke the registered 'HalfTransfer' callback function (if any) */
  if (hAudioIn.CbHalfTransfer != (Audio_CallbackTypeDef)NULL)
  {
    if (hdfsdm_filter == &BSP_AUDIO_hDfsdmLeftFilter)
    {
      if (DmaLeftRecHalfBuffCplt)
      {
        BSP_ErrorHandler();
      }

      DmaLeftRecHalfBuffCplt = 1;
    }
    else
    {
      if (DmaRightRecHalfBuffCplt)
      {
        BSP_ErrorHandler();
      }

      DmaRightRecHalfBuffCplt = 1;
    }

    if (((DmaLeftRecHalfBuffCplt != 0) && (DmaRightRecHalfBuffCplt != 0) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC)) ||
        ((DmaLeftRecHalfBuffCplt != 0) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC1)) ||
        ((DmaRightRecHalfBuffCplt != 0) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)))
    {
      hAudioIn.CbHalfTransfer();
      DmaLeftRecHalfBuffCplt = 0;
      DmaRightRecHalfBuffCplt = 0;
    }
  }
}

/**
  * @brief  Error callback.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterErrorCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  /* Invoke the registered 'ErrorCallback' callback function (if any) */
  if (hAudioIn.CbError != (Audio_CallbackTypeDef)NULL)
  {
    hAudioIn.CbError();
  }
}

/**
  * @brief  SAI Rx Transfer completed callbacks.
  * @param  hsai: SAI handle
  * @retval None
  */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Invoke the registered 'TransferComplete' function (if any) */
  if (hAudioIn.CbTransferComplete != (Audio_CallbackTypeDef)NULL)
  {
    hAudioIn.CbTransferComplete();
  }
}

/**
  * @brief  SAI Rx Half Transfer completed callbacks.
  * @param  hsai: SAI handle
  * @retval None
  */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Invoke the registered 'HalfTransfer' callback function (if any) */
  if (hAudioIn.CbHalfTransfer != (Audio_CallbackTypeDef)NULL)
  {
    hAudioIn.CbHalfTransfer();
  }
}

/**
  * @brief  Stops audio recording.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_IN_Stop(void)
{
  if (hAudioIn.InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
    {
      /* Call the Media layer stop function for right channel */
      if (HAL_DFSDM_FilterRegularStop_DMA(&BSP_AUDIO_hDfsdmRightFilter) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
    {
      /* Call the Media layer stop function for left channel */
      if (HAL_DFSDM_FilterRegularStop_DMA(&BSP_AUDIO_hDfsdmLeftFilter) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
    
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
    {
      /* Free hAudioIn.LeftRecBuff buffer */
#if defined(BSP_AUDIO_USE_RTOS)
      k_free((void *)hAudioIn.LeftRecBuff);
#else
      free((void *)hAudioIn.LeftRecBuff);
#endif
    }
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
    {
      /* Free hAudioIn.RightRecBuff buffer */
#if defined(BSP_AUDIO_USE_RTOS)
      k_free((void *)hAudioIn.RightRecBuff);
#else
      free((void *)hAudioIn.RightRecBuff);
#endif
    }
  }
  else
  {
    /* INPUT_DEVICE_ANALOG_MIC */
    /* Call Audio Codec Stop function */
    if (hAudioIn.AudioDrv->Stop(AUDIO_I2C_ADDRESS, CODEC_PDWN_HW) != 0)
    {
      return AUDIO_ERROR;
    }

    /* Wait at least 100ms */
    HAL_Delay(100);

    /* Stop DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_DMAStop(&BSP_AUDIO_hSai_Rx) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  return AUDIO_OK;
}

/**
  * @brief  Pauses the audio file stream.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_IN_Pause(void)
{
  if (hAudioIn.InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    /* Call the Media layer stop function */
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
    {
      if (HAL_DFSDM_FilterRegularStop_DMA(&BSP_AUDIO_hDfsdmRightFilter) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
    
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
    {
      if (HAL_DFSDM_FilterRegularStop_DMA(&BSP_AUDIO_hDfsdmLeftFilter) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
  }
  else
  {
    /* INPUT_DEVICE_ANALOG_MIC */
    /* Pause DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_DMAPause(&BSP_AUDIO_hSai_Rx) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  return AUDIO_OK;
}

/**
  * @brief  Resumes the audio file stream.
  * @retval BSP AUDIO status
  */
uint8_t BSP_AUDIO_IN_Resume(void)
{
  if (hAudioIn.InputDevice != INPUT_DEVICE_ANALOG_MIC)
  {
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
    {
      /* Call the Media layer start function for right channel */
      if (HAL_DFSDM_FilterRegularStart_DMA(&BSP_AUDIO_hDfsdmRightFilter,
                                           (int32_t *)hAudioIn.RightRecBuff,
                                           (hAudioIn.RecSize / hAudioIn.ChannelNbr)) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
    
    if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
    {
      /* Call the Media layer start function for left channel */
      if (HAL_DFSDM_FilterRegularStart_DMA(&BSP_AUDIO_hDfsdmLeftFilter,
                                           (int32_t *)hAudioIn.LeftRecBuff,
                                           (hAudioIn.RecSize / hAudioIn.ChannelNbr)) != HAL_OK)
      {
        return AUDIO_ERROR;
      }
    }
  }
  else
  {
    /* INPUT_DEVICE_ANALOG_MIC */
    /* Resume DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_DMAResume(&BSP_AUDIO_hSai_Rx) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  return AUDIO_OK;
}

/**
  * @brief  register user callback functions
  * @param  ErrorCallback: pointer to the error callback function
  * @param  HalfTransferCallback: pointer to the half transfer callback function
  * @param  TransferCompleteCallback: pointer to the transfer complete callback function
  * @retval None
  */
void BSP_AUDIO_IN_RegisterCallbacks(Audio_CallbackTypeDef ErrorCallback,
                                    Audio_CallbackTypeDef HalfTransferCallback,
                                    Audio_CallbackTypeDef TransferCompleteCallback)
{
  hAudioIn.CbError            = ErrorCallback;
  hAudioIn.CbHalfTransfer     = HalfTransferCallback;
  hAudioIn.CbTransferComplete = TransferCompleteCallback;
}
/**
  * @}
  */

/* private functions --------------------------------------------------------*/
/** @addtogroup STM32L496G_DISCOVERY_AUDIO_Private_Functions
  * @{
  */
/**
  * @brief  Initializes the Audio Codec audio interface (SAI).
  * @param  AudioFreq: Audio frequency to be configured for the SAI peripheral.
  * @retval BSP AUDIO status
  */
static uint8_t AUDIO_SAIx_Init(uint32_t AudioFreq)
{
  uint8_t TxData[2] = {0x00, 0x00};

  /* Initialize the BSP_AUDIO_hSai_Xx instances parameter */
  BSP_AUDIO_hSai_Tx.Instance = SAI1_Block_A;
  BSP_AUDIO_hSai_Rx.Instance = SAI1_Block_B;

  /* Disable SAI peripheral to allow access to SAI internal registers */
  __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Tx);
  __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Rx);

  /*******************************/
  /* SAI block used for playback */
  /*******************************/
  /* Configure SAI_Block_x used for transmit
  LSBFirst: Disabled
  DataSize: 16 */
  BSP_AUDIO_hSai_Tx.Init.AudioMode      = SAI_MODEMASTER_TX;
  BSP_AUDIO_hSai_Tx.Init.Synchro        = SAI_ASYNCHRONOUS;
  BSP_AUDIO_hSai_Tx.Init.SynchroExt     = SAI_SYNCEXT_DISABLE;
  BSP_AUDIO_hSai_Tx.Init.OutputDrive    = SAI_OUTPUTDRIVE_ENABLE;
  BSP_AUDIO_hSai_Tx.Init.NoDivider      = SAI_MASTERDIVIDER_ENABLE;
  BSP_AUDIO_hSai_Tx.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_1QF;
  BSP_AUDIO_hSai_Tx.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_MCKDIV;
  BSP_AUDIO_hSai_Tx.Init.Mckdiv         = SAIClockDivider(AudioFreq);
  BSP_AUDIO_hSai_Tx.Init.MonoStereoMode = SAI_STEREOMODE;
  BSP_AUDIO_hSai_Tx.Init.CompandingMode = SAI_NOCOMPANDING;
  BSP_AUDIO_hSai_Tx.Init.TriState       = SAI_OUTPUT_NOTRELEASED;
  BSP_AUDIO_hSai_Tx.Init.Protocol       = SAI_FREE_PROTOCOL;
  BSP_AUDIO_hSai_Tx.Init.DataSize       = SAI_DATASIZE_16;
  BSP_AUDIO_hSai_Tx.Init.FirstBit       = SAI_FIRSTBIT_MSB;
  BSP_AUDIO_hSai_Tx.Init.ClockStrobing  = SAI_CLOCKSTROBING_FALLINGEDGE;

  /* Configure SAI_Block_x Frame
  Frame Length: 32
  Frame active Length: 16
  FS Definition: Start frame + Channel Side identification
  FS Polarity: FS active Low
  FS Offset: FS asserted one bit before the first bit of slot 0 */
  BSP_AUDIO_hSai_Tx.FrameInit.FrameLength = 32;
  BSP_AUDIO_hSai_Tx.FrameInit.ActiveFrameLength = 16;
  BSP_AUDIO_hSai_Tx.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  BSP_AUDIO_hSai_Tx.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  BSP_AUDIO_hSai_Tx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

  /* Configure SAI Block_x Slot
  Slot First Bit Offset: 0
  Slot Size  : 16
  Slot Number: 2
  Slot Active: Slots 0 and 1 actives */
  BSP_AUDIO_hSai_Tx.SlotInit.FirstBitOffset = 0;
  BSP_AUDIO_hSai_Tx.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  BSP_AUDIO_hSai_Tx.SlotInit.SlotNumber = 2;
  BSP_AUDIO_hSai_Tx.SlotInit.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1;

  /*****************************/
  /* SAI block used for record */
  /*****************************/
  /* Configure SAI_Block_x used for receive
  LSBFirst: Disabled
  DataSize: 16 */
  BSP_AUDIO_hSai_Rx.Init.AudioMode      = SAI_MODESLAVE_RX;
  BSP_AUDIO_hSai_Rx.Init.Synchro        = SAI_SYNCHRONOUS;
  BSP_AUDIO_hSai_Rx.Init.SynchroExt     = SAI_SYNCEXT_DISABLE;
  BSP_AUDIO_hSai_Rx.Init.OutputDrive    = SAI_OUTPUTDRIVE_ENABLE;
  BSP_AUDIO_hSai_Rx.Init.NoDivider      = SAI_MASTERDIVIDER_ENABLE;
  BSP_AUDIO_hSai_Rx.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_1QF;
  BSP_AUDIO_hSai_Rx.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_MCKDIV;
  BSP_AUDIO_hSai_Rx.Init.Mckdiv         = SAIClockDivider(AudioFreq);
  BSP_AUDIO_hSai_Rx.Init.MonoStereoMode = SAI_MONOMODE;
  BSP_AUDIO_hSai_Rx.Init.CompandingMode = SAI_NOCOMPANDING;
  BSP_AUDIO_hSai_Rx.Init.TriState       = SAI_OUTPUT_NOTRELEASED;
  BSP_AUDIO_hSai_Rx.Init.Protocol       = SAI_FREE_PROTOCOL;
  BSP_AUDIO_hSai_Rx.Init.DataSize       = SAI_DATASIZE_16;
  BSP_AUDIO_hSai_Rx.Init.FirstBit       = SAI_FIRSTBIT_MSB;
  BSP_AUDIO_hSai_Rx.Init.ClockStrobing  = SAI_CLOCKSTROBING_FALLINGEDGE;

  /* Configure SAI_Block_x Frame
  Frame Length: 32
  Frame active Length: 16
  FS Definition: Start frame + Channel Side identification
  FS Polarity: FS active Low
  FS Offset: FS asserted one bit before the first bit of slot 0 */
  BSP_AUDIO_hSai_Rx.FrameInit.FrameLength = 32;
  BSP_AUDIO_hSai_Rx.FrameInit.ActiveFrameLength = 16;
  BSP_AUDIO_hSai_Rx.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  BSP_AUDIO_hSai_Rx.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  BSP_AUDIO_hSai_Rx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

  /* Configure SAI Block_x Slot
  Slot First Bit Offset: 0
  Slot Size  : 16
  Slot Number: 2
  Slot Active: Slots 0 and 1 actives */
  BSP_AUDIO_hSai_Rx.SlotInit.FirstBitOffset = 0;
  BSP_AUDIO_hSai_Rx.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  BSP_AUDIO_hSai_Rx.SlotInit.SlotNumber = 2;
  BSP_AUDIO_hSai_Rx.SlotInit.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1;

  /*********************************/
  /* Initializes the SAI peripheral*/
  /*********************************/
  if (HAL_SAI_Init(&BSP_AUDIO_hSai_Tx) != HAL_OK)
  {
    return AUDIO_ERROR;
  }
  if (HAL_SAI_Init(&BSP_AUDIO_hSai_Rx) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  /******************************************/
  /* Enable SAI peripheral to generate MCLK */
  /******************************************/
  __HAL_SAI_ENABLE(&BSP_AUDIO_hSai_Tx);
  /* Transmit one byte to start FS generation */
  if (HAL_SAI_Transmit(&BSP_AUDIO_hSai_Tx, TxData, 2, 1000) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  De-initializes the Audio Codec audio interface (SAI).
  * @retval BSP AUDIO status
  */
static uint8_t AUDIO_SAIx_DeInit(void)
{
  /* Disable the SAI audio block */
  __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Tx);
  __HAL_SAI_DISABLE(&BSP_AUDIO_hSai_Rx);

  /* De-initializes the SAI peripheral */
  if (HAL_SAI_DeInit(&BSP_AUDIO_hSai_Tx) != HAL_OK)
  {
    return AUDIO_ERROR;
  }
  if (HAL_SAI_DeInit(&BSP_AUDIO_hSai_Rx) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @brief  SAI MSP Init
  * @param  hsai : pointer to a SAI_HandleTypeDef structure
  * @retval None
  */
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable SAI clock */
  __HAL_RCC_SAI1_CLK_ENABLE();

  if (hsai->Instance == SAI1_Block_A)
  {
    /* SAI pins configuration: FS, SCK, MCLK and SD pins */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    GPIO_InitStruct.Pin       = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* SAI1_MCLK_A */
    GPIO_InitStruct.Pin       = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* SAI1_FS_A */
    GPIO_InitStruct.Pin       = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); /* SAI1_SCK_A */
    GPIO_InitStruct.Pin       = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* SAI1_SD_A */

    /* Configure the hDmaSaiTx handle parameters */
    __HAL_RCC_DMA2_CLK_ENABLE();
    hDmaSaiTx.Init.Request             = DMA_REQUEST_1;
    hDmaSaiTx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hDmaSaiTx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hDmaSaiTx.Init.MemInc              = DMA_MINC_ENABLE;
    hDmaSaiTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hDmaSaiTx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    hDmaSaiTx.Init.Mode                = DMA_CIRCULAR;
    hDmaSaiTx.Init.Priority            = DMA_PRIORITY_HIGH;
    hDmaSaiTx.Instance                 = DMA2_Channel1;
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmatx, hDmaSaiTx);
    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hDmaSaiTx);
    /* Configure the DMA Stream */
    HAL_DMA_Init(&hDmaSaiTx);
    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
  }
  else /* SAI1_BlockB */
  {
    /* SAI pins configuration: SD pin */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    GPIO_InitStruct.Pin       = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* SAI1_SD_B */

    /* Configure the hDmaSaiRx handle parameters */
    __HAL_RCC_DMA2_CLK_ENABLE();
    hDmaSaiRx.Init.Request             = DMA_REQUEST_1;
    hDmaSaiRx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hDmaSaiRx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hDmaSaiRx.Init.MemInc              = DMA_MINC_ENABLE;
    hDmaSaiRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hDmaSaiRx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    hDmaSaiRx.Init.Mode                = DMA_CIRCULAR;
    hDmaSaiRx.Init.Priority            = DMA_PRIORITY_HIGH;
    hDmaSaiRx.Instance                 = DMA2_Channel2;
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmarx, hDmaSaiRx);
    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hDmaSaiRx);
    /* Configure the DMA Stream */
    HAL_DMA_Init(&hDmaSaiRx);
    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
  }
}

/**
  * @brief  SAI MSP De-init
  * @param  hsai : pointer to a SAI_HandleTypeDef structure
  * @retval None
  */
void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    /* Disable SAI DMA Channel IRQ  */
    HAL_NVIC_DisableIRQ(DMA2_Channel1_IRQn);

    /* Reset the DMA Stream configuration*/
    HAL_DMA_DeInit(&hDmaSaiTx);

    /* De-initialize FS, SCK, MCK and SD pins*/
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);  /* SAI1_MCLK_A */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4);  /* SAI1_FS_A */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10); /* SAI1_SCK_A */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_6);  /* SAI1_SD_A */

    /* Don't disable SAI clock used for other SAI block */
    /*__HAL_RCC_SAI1_CLK_DISABLE(); */
  }
  else /* SAI1_BlockB */
  {
    /* Disable SAI DMA Channel IRQ  */
    HAL_NVIC_DisableIRQ(DMA2_Channel2_IRQn);

    /* Reset the DMA Stream configuration*/
    HAL_DMA_DeInit(&hDmaSaiRx);

    /* De-initialize SD pin */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_3);  /* SAI1_SD_B */

    /* Disable SAI clock */
    __HAL_RCC_SAI1_CLK_DISABLE();
  }
}

/**
  * @}
  */

/** @addtogroup STM32L496G_DISCOVERY_AUDIO_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Digital Filter for Sigma-Delta Modulators interface (DFSDM).
  * @param  AudioFreq: Audio frequency to be used to set correctly the DFSDM peripheral.
  * @retval BSP AUDIO status
  */
static uint8_t AUDIO_DFSDMx_Init(uint32_t AudioFreq)
{
  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
  {
    /*####CHANNEL 3####*/
    hAudioIn.hDfsdmLeftChannel.Init.OutputClock.Activation   = ENABLE;
    hAudioIn.hDfsdmLeftChannel.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
    /* Set the DFSDM clock OUT audio frequency configuration */
    hAudioIn.hDfsdmLeftChannel.Init.OutputClock.Divider      = DFSDMClockDivider(AudioFreq);
    hAudioIn.hDfsdmLeftChannel.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
    hAudioIn.hDfsdmLeftChannel.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
    hAudioIn.hDfsdmLeftChannel.Init.Input.Pins               = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
    /* Request to sample stable data for LEFT micro on Rising edge */
    hAudioIn.hDfsdmLeftChannel.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_RISING;
    hAudioIn.hDfsdmLeftChannel.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    hAudioIn.hDfsdmLeftChannel.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
    hAudioIn.hDfsdmLeftChannel.Init.Awd.Oversampling         = 10;
    hAudioIn.hDfsdmLeftChannel.Init.Offset                   = 0;
    hAudioIn.hDfsdmLeftChannel.Init.RightBitShift            = DFSDMRightBitShift(AudioFreq);
    hAudioIn.hDfsdmLeftChannel.Instance                      = DFSDM1_Channel3;

    /* Init the DFSDM Channel */
    if (HAL_DFSDM_ChannelInit(&hAudioIn.hDfsdmLeftChannel) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
  {
    /*####CHANNEL 2####*/
    hAudioIn.hDfsdmRightChannel.Init.OutputClock.Activation   = ENABLE;
    hAudioIn.hDfsdmRightChannel.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
    /* Set the DFSDM clock OUT audio frequency configuration */
    hAudioIn.hDfsdmRightChannel.Init.OutputClock.Divider      = DFSDMClockDivider(AudioFreq);
    hAudioIn.hDfsdmRightChannel.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
    hAudioIn.hDfsdmRightChannel.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
    hAudioIn.hDfsdmRightChannel.Init.Input.Pins               = DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS;
    /* Request to sample stable data for LEFT micro on Rising edge */
    hAudioIn.hDfsdmRightChannel.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_FALLING;
    hAudioIn.hDfsdmRightChannel.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    hAudioIn.hDfsdmRightChannel.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
    hAudioIn.hDfsdmRightChannel.Init.Awd.Oversampling         = 10;
    hAudioIn.hDfsdmRightChannel.Init.Offset                   = 0;
    hAudioIn.hDfsdmRightChannel.Init.RightBitShift            = DFSDMRightBitShift(AudioFreq);
    hAudioIn.hDfsdmRightChannel.Instance                      = DFSDM1_Channel2;
    
    /* Init the DFSDM Channel */
    if (HAL_DFSDM_ChannelInit(&hAudioIn.hDfsdmRightChannel) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
  {
    /*####FILTER 0####*/
    BSP_AUDIO_hDfsdmLeftFilter.Init.RegularParam.Trigger         = DFSDM_FILTER_SW_TRIGGER;
    BSP_AUDIO_hDfsdmLeftFilter.Init.RegularParam.FastMode        = ENABLE;
    BSP_AUDIO_hDfsdmLeftFilter.Init.RegularParam.DmaMode         = ENABLE;
    BSP_AUDIO_hDfsdmLeftFilter.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
    BSP_AUDIO_hDfsdmLeftFilter.Init.InjectedParam.ScanMode       = DISABLE;
    BSP_AUDIO_hDfsdmLeftFilter.Init.InjectedParam.DmaMode        = DISABLE;
    BSP_AUDIO_hDfsdmLeftFilter.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
    BSP_AUDIO_hDfsdmLeftFilter.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
    BSP_AUDIO_hDfsdmLeftFilter.Init.FilterParam.SincOrder        = DFSDMFilterOrder(AudioFreq);
    /* Set the DFSDM Filters Oversampling to have correct sample rate */
    BSP_AUDIO_hDfsdmLeftFilter.Init.FilterParam.Oversampling     = DFSDMOverSampling(AudioFreq);
    BSP_AUDIO_hDfsdmLeftFilter.Init.FilterParam.IntOversampling  = 1;
    BSP_AUDIO_hDfsdmLeftFilter.Instance                          = DFSDM1_Filter0;
    
    /* Init the DFSDM Filter */
    if (HAL_DFSDM_FilterInit(&BSP_AUDIO_hDfsdmLeftFilter) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
    
    /* Configure regular channel */
    if (HAL_DFSDM_FilterConfigRegChannel(&BSP_AUDIO_hDfsdmLeftFilter,
                                         DFSDM_CHANNEL_3,
                                         DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
  {
    /*####FILTER 1####*/
    if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)
    {
      BSP_AUDIO_hDfsdmRightFilter.Init.RegularParam.Trigger       = DFSDM_FILTER_SW_TRIGGER;
    }
    else
    {
      BSP_AUDIO_hDfsdmRightFilter.Init.RegularParam.Trigger       = DFSDM_FILTER_SYNC_TRIGGER;
    }
    BSP_AUDIO_hDfsdmRightFilter.Init.RegularParam.FastMode        = ENABLE;
    BSP_AUDIO_hDfsdmRightFilter.Init.RegularParam.DmaMode         = ENABLE;
    BSP_AUDIO_hDfsdmRightFilter.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
    BSP_AUDIO_hDfsdmRightFilter.Init.InjectedParam.ScanMode       = DISABLE;
    BSP_AUDIO_hDfsdmRightFilter.Init.InjectedParam.DmaMode        = DISABLE;
    BSP_AUDIO_hDfsdmRightFilter.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
    BSP_AUDIO_hDfsdmRightFilter.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
    BSP_AUDIO_hDfsdmRightFilter.Init.FilterParam.SincOrder        = DFSDMFilterOrder(AudioFreq);
    /* Set the DFSDM Filters Oversampling to have correct sample rate */
    BSP_AUDIO_hDfsdmRightFilter.Init.FilterParam.Oversampling     = DFSDMOverSampling(AudioFreq);
    BSP_AUDIO_hDfsdmRightFilter.Init.FilterParam.IntOversampling  = 1;
    BSP_AUDIO_hDfsdmRightFilter.Instance                          = DFSDM1_Filter1;
    
    /* Init the DFSDM Filter */
    if (HAL_DFSDM_FilterInit(&BSP_AUDIO_hDfsdmRightFilter) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
    
    /* Configure regular channel */
    if (HAL_DFSDM_FilterConfigRegChannel(&BSP_AUDIO_hDfsdmRightFilter,
                                         DFSDM_CHANNEL_2,
                                         DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  return AUDIO_OK;
}

/**
  * @brief  De-initializes the Digital Filter for Sigma-Delta Modulators interface (DFSDM).
  * @retval BSP AUDIO status
  */
static uint8_t AUDIO_DFSDMx_DeInit(void)
{
  /* De-initializes the DFSDM filters to allow access to DFSDM internal registers */
  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
  {
    if (HAL_DFSDM_FilterDeInit(&BSP_AUDIO_hDfsdmRightFilter) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }
  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
  {
    if (HAL_DFSDM_FilterDeInit(&BSP_AUDIO_hDfsdmLeftFilter) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  /* De-initializes the DFSDM channels to allow access to DFSDM internal registers */
  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC2) == INPUT_DEVICE_DIGITAL_MIC2)
  {
    if (HAL_DFSDM_ChannelDeInit(&hAudioIn.hDfsdmRightChannel) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }
  if ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) == INPUT_DEVICE_DIGITAL_MIC1)
  {
    if (HAL_DFSDM_ChannelDeInit(&hAudioIn.hDfsdmLeftChannel) != HAL_OK)
    {
      return AUDIO_ERROR;
    }
  }

  /* DFSDM reset */
  __HAL_RCC_DFSDM1_FORCE_RESET();
  __HAL_RCC_DFSDM1_RELEASE_RESET();

  return AUDIO_OK;
}

/**
  * @brief  Initializes the DFSDM channel MSP.
  * @param  hdfsdm_channel : DFSDM channel handle.
  * @retval None
  */
void HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel)
{
  if (((hdfsdm_channel->Instance == DFSDM1_Channel3) && ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) != 0)) || \
      ((hdfsdm_channel->Instance == DFSDM1_Channel2) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)))
  {
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable DFSDM clock */
    __HAL_RCC_DFSDM1_CLK_ENABLE();

    /* DFSDM pins configuration: DFSDM1_CKOUT, DFSDM1_DATIN3 pins */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    GPIO_InitStruct.Pin       = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); /* DFSDM1_CKOUT */
    GPIO_InitStruct.Pin       = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); /* DFSDM1_DATIN3 */

    /* Enable MIC_VDD (PH1) */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pin       = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);
  }
}

/**
  * @brief  De-initializes the DFSDM channel MSP.
  * @param  hdfsdm_channel : DFSDM channel handle.
  * @retval None
  */
void HAL_DFSDM_ChannelMspDeInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel)
{
  if (((hdfsdm_channel->Instance == DFSDM1_Channel3) && ((hAudioIn.InputDevice & INPUT_DEVICE_DIGITAL_MIC1) != 0)) || \
      ((hdfsdm_channel->Instance == DFSDM1_Channel2) && (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)))
  {
    /* Disable MIC_VDD (PH1) */
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_1);

    /* De-initialize DFSDM1_CKOUT, DFSDM1_DATIN3 pins */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);  /* DFSDM1_CKOUT */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_7);  /* DFSDM1_DATIN3 */

    /* Disable DFSDM1 */
    __HAL_RCC_DFSDM1_CLK_DISABLE();
  }
}

/**
  * @brief  Initializes the DFSDM filter MSP.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  if (hdfsdm_filter->Instance == DFSDM1_Filter0)
  {
    /* Enable the DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure the hAudioIn.hDmaDfsdmLeft handle parameters */
    hAudioIn.hDmaDfsdmLeft.Init.Request             = DMA_REQUEST_0;
    hAudioIn.hDmaDfsdmLeft.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hAudioIn.hDmaDfsdmLeft.Init.PeriphInc           = DMA_PINC_DISABLE;
    hAudioIn.hDmaDfsdmLeft.Init.MemInc              = DMA_MINC_ENABLE;
    hAudioIn.hDmaDfsdmLeft.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hAudioIn.hDmaDfsdmLeft.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hAudioIn.hDmaDfsdmLeft.Init.Mode                = DMA_CIRCULAR;
    hAudioIn.hDmaDfsdmLeft.Init.Priority            = DMA_PRIORITY_HIGH;
    hAudioIn.hDmaDfsdmLeft.Instance                 = DMA1_Channel4;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hdfsdm_filter, hdmaReg, hAudioIn.hDmaDfsdmLeft);

    /* Reset DMA handle state */
    __HAL_DMA_RESET_HANDLE_STATE(&hAudioIn.hDmaDfsdmLeft);

    /* Configure the DMA Channel */
    HAL_DMA_Init(&hAudioIn.hDmaDfsdmLeft);

    /* DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  }
  else /* DFSDM1_Filter1 */
  {
    if (hAudioIn.InputDevice == INPUT_DEVICE_DIGITAL_MIC2)
    {
      /* Enable the DMA clock needed if only MIC2 is used */
      __HAL_RCC_DMA1_CLK_ENABLE();
    }

    /* Configure the hAudioIn.hDmaDfsdmRight handle parameters */
    hAudioIn.hDmaDfsdmRight.Init.Request             = DMA_REQUEST_0;
    hAudioIn.hDmaDfsdmRight.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hAudioIn.hDmaDfsdmRight.Init.PeriphInc           = DMA_PINC_DISABLE;
    hAudioIn.hDmaDfsdmRight.Init.MemInc              = DMA_MINC_ENABLE;
    hAudioIn.hDmaDfsdmRight.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hAudioIn.hDmaDfsdmRight.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hAudioIn.hDmaDfsdmRight.Init.Mode                = DMA_CIRCULAR;
    hAudioIn.hDmaDfsdmRight.Init.Priority            = DMA_PRIORITY_HIGH;
    hAudioIn.hDmaDfsdmRight.Instance                 = DMA1_Channel5;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hdfsdm_filter, hdmaReg, hAudioIn.hDmaDfsdmRight);

    /* Reset DMA handle state */
    __HAL_DMA_RESET_HANDLE_STATE(&hAudioIn.hDmaDfsdmRight);

    /* Configure the DMA Channel */
    HAL_DMA_Init(&hAudioIn.hDmaDfsdmRight);

    /* DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  }
}

/**
 * @brief  De-initializes the DFSDM filter MSP.
 * @param  hdfsdm_filter : DFSDM filter handle.
 * @retval None
 */
void HAL_DFSDM_FilterMspDeInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  if (hdfsdm_filter->Instance == DFSDM1_Filter0)
  {
    /* Disable DMA  Channel IRQ */
    HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);

    /* De-initialize the DMA Channel */
    HAL_DMA_DeInit(&hAudioIn.hDmaDfsdmLeft);
  }
  else /* DFSDM1_Filter1 */
  {
    /* Disable DMA  Channel IRQ */
    HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);

    /* De-initialize the DMA Channel */
    HAL_DMA_DeInit(&hAudioIn.hDmaDfsdmRight);
  }
}

/**
  * @brief  Configures the SAI PLL clock according to the required audio frequency.
  * @param  Frequency: Audio frequency.
  * @retval BSP AUDIO status
  * @note   The SAI PLL input clock must be configured in the user application.
  *         The SAI PLL configuration done within this function assumes that
  *         the SAI PLL input clock runs at 8 MHz.
  */
static uint8_t AUDIO_SAIPLLConfig(uint32_t Frequency)
{
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

  /* Retrieve actual RCC configuration */
  HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

  if ((Frequency == AUDIO_FREQUENCY_11K)
      || (Frequency == AUDIO_FREQUENCY_22K)
      || (Frequency == AUDIO_FREQUENCY_44K))
  {
    /* Configure PLLSAI prescalers */
    /* SAI clock config
    PLLSAI2_VCO= 8 Mhz * PLLSAI1N = 8 * 24 = VCO_192M
    SAI_CK_x = PLLSAI2_VCO/PLLSAI1P = 192/17 = 11.294 Mhz */
    RCC_ExCLKInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_SAI1;
    RCC_ExCLKInitStruct.PLLSAI2.PLLSAI2N        = 24;
    RCC_ExCLKInitStruct.PLLSAI2.PLLSAI2P        = 17;
    RCC_ExCLKInitStruct.PLLSAI2.PLLSAI2ClockOut = RCC_PLLSAI2_SAI2CLK;
    RCC_ExCLKInitStruct.Sai1ClockSelection      = RCC_SAI1CLKSOURCE_PLLSAI2;
  }
  else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K, AUDIO_FREQUENCY_96K */
  {
    /* SAI clock config
    PLLSAI2_VCO= 8 Mhz * PLLSAI1N = 8 * 43 = VCO_344M
    SAI_CK_x = PLLSAI1_VCO/PLLSAI2P = 344/7 = 49.142 Mhz */
    RCC_ExCLKInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_SAI1;
    RCC_ExCLKInitStruct.PLLSAI2.PLLSAI2N        = 43;
    RCC_ExCLKInitStruct.PLLSAI2.PLLSAI2P        = 7;
    RCC_ExCLKInitStruct.PLLSAI2.PLLSAI2ClockOut = RCC_PLLSAI2_SAI2CLK;
    RCC_ExCLKInitStruct.Sai1ClockSelection      = RCC_SAI1CLKSOURCE_PLLSAI2;
  }

  if (HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct) != HAL_OK)
  {
    return AUDIO_ERROR;
  }

  return AUDIO_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
