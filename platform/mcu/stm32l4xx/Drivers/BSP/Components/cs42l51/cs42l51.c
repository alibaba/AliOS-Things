/**
  ******************************************************************************
  * @file    cs42l51.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-February-2017
  * @brief   This file provides the CS42L51 Audio Codec driver.   
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

/* Includes ------------------------------------------------------------------*/
#include "cs42l51.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @addtogroup CS42L51
  * @brief     This file provides a set of functions needed to drive the 
  *            CS42L51 audio codec.
  * @{
  */

/** @defgroup CS42L51_Exported_Variables
  * @{
  */

/* Audio codec driver structure initialization */  
AUDIO_DrvTypeDef cs42l51_drv = 
{
  cs42l51_Init,
  cs42l51_DeInit,
  cs42l51_ReadID,

  cs42l51_Play,
  cs42l51_Pause,
  cs42l51_Resume,
  cs42l51_Stop,  
  
  cs42l51_SetFrequency,  
  cs42l51_SetVolume,
  cs42l51_SetMute,  
  cs42l51_SetOutputMode,
  cs42l51_Reset,
};
  
/**
  * @}
  */ 

/** @defgroup CS42L51_Private_Types
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup CS42L51_Private_Defines
  * @{
  */
/* Uncomment this line to enable verifying data sent to codec after each write 
   operation (for debug purpose) */
#if !defined (VERIFY_WRITTENDATA)  
#define VERIFY_WRITTENDATA
#endif /* VERIFY_WRITTENDATA */
/**
  * @}
  */ 

/** @defgroup CS42L51_Private_Macros
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup CS42L51_Private_Variables
  * @{
  */

static uint8_t Is_CS42L51_Initialized = 0;
static uint8_t Is_CS42L51_Stop        = 1;

static uint16_t CS42L51_Device = OUTPUT_DEVICE_HEADPHONE;
  
/**
  * @}
  */ 

/** @defgroup CS42L51_Private_Functions
  * @{
  */
static uint8_t CODEC_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
/**
  * @}
  */ 

/** @addtogroup CS42L51_Exported_Functions
  * @{
  */ 

/**
  * @brief Initialize the audio codec and the control interface.
  * @param DeviceAddr: Device address on communication bus.
  * @param Device:     Can be combination values of OUTPUT_DEVICE_HEADPHONE and
  *                       INPUT_DEVICE_MIC1.
  * @param Volume:     Initial output volume level (from 0 (-100dB) to 100 (0dB)).
  * @param AudioFreq:  Initial audio frequency (currently not used).
  * @retval 0 if correct communication, else wrong communication.
  */
uint32_t cs42l51_Init(uint16_t DeviceAddr, uint16_t Device, uint8_t Volume, uint32_t AudioFreq)
{
  uint32_t counter = 0;
  uint8_t  Value;
  
  /* Check if codec is already initialized */
  if(Is_CS42L51_Initialized == 0)
  {
    /* Initialize the Control interface of the Audio Codec */
    AUDIO_IO_Init();

    Is_CS42L51_Initialized = 1;
  }
  else
  {
    /* Set all power down bits to 1 exept PDN to mute ADCs and DACs*/
    counter += CODEC_IO_Write(DeviceAddr, 0x02, 0x7E);
    Value = AUDIO_IO_Read(DeviceAddr, 0x03);
    counter += CODEC_IO_Write(DeviceAddr, 0x03, (Value | 0x0E));
    
    /* Disable zero cross and soft ramp */
    Value = AUDIO_IO_Read(DeviceAddr, 0x09);
    counter += CODEC_IO_Write(DeviceAddr, 0x09, (Value & 0xFC));
    
    /* Power control : Enter standby (PDN = 1) */
    Value = AUDIO_IO_Read(DeviceAddr, 0x02);
    counter += CODEC_IO_Write(DeviceAddr, 0x02, (Value | 0x01));
  }

  /* Mic Power and Speed Control : Auto detect on, Speed mode SSM, tri state off, MCLK divide by 2 off */
  Value = AUDIO_IO_Read(DeviceAddr, 0x03);
  counter += CODEC_IO_Write(DeviceAddr, 0x03, ((Value & 0x0E) | 0xA0));

  /* Interface control : Loopback off, Slave, I2S (SDIN and SOUT), Digital mix off, Mic mix off */
  counter += CODEC_IO_Write(DeviceAddr, 0x04, 0x0C);
  
  /* Mic control : ADC single volume off, ADCB boost off, ADCA boost off, MicBias on AIN3B/MICIN2 pin, MicBias level 0.8xVA, MICB boost 16db, MICA boost 16dB */
  counter += CODEC_IO_Write(DeviceAddr, 0x05, 0x00);

  /* ADC control : ADCB HPF off, ADCB HPF freeze off, ADCA HPF off, ADCA HPF freeze off, Soft ramp B off, Zero cross B off, Soft ramp A off, Zero cross A off */
  counter += CODEC_IO_Write(DeviceAddr, 0x06, 0x00);

  /* ADC Input Select, Invert and Mute : AIN1B to PGAB, AIN3A to PreAmp to PGAA, ADCB invert off, ADCA invert off, ADCB mute on, ADCA mute off */
  counter += CODEC_IO_Write(DeviceAddr, 0x07, 0x32);
  
  /* DAC output control : HP Gain to 1, Single volume control off, PCM invert signals polarity off, DAC channels mute on */
  counter += CODEC_IO_Write(DeviceAddr, 0x08, 0xC3);

  /* DAC control : Signal processing to DAC, Freeze off, De-emphasis off, Analog output auto mute off, DAC soft ramp */
  counter += CODEC_IO_Write(DeviceAddr, 0x09, 0x42);

  /* ALCA and PGAA Control : ALCA soft ramp disable on, ALCA zero cross disable on, PGA A Gain 0dB */  
  counter += CODEC_IO_Write(DeviceAddr, 0x0A, 0xC0);

  /* ALCB and PGAB Control : ALCB soft ramp disable on, ALCB zero cross disable on, PGA B Gain 0dB */  
  counter += CODEC_IO_Write(DeviceAddr, 0x0B, 0xC0);
  
  /* ADCA Attenuator : 0dB */
  counter += CODEC_IO_Write(DeviceAddr, 0x0C, 0x00);

  /* ADCB Attenuator : 0dB */
  counter += CODEC_IO_Write(DeviceAddr, 0x0D, 0x00);

  /* ADCA mixer volume control : ADCA mixer channel mute on, ADCA mixer volume 0dB */
  counter += CODEC_IO_Write(DeviceAddr, 0x0E, 0x80);
  
  /* ADCB mixer volume control : ADCB mixer channel mute on, ADCB mixer volume 0dB */
  counter += CODEC_IO_Write(DeviceAddr, 0x0F, 0x80);
  
  /* PCMA mixer volume control : PCMA mixer channel mute off, PCMA mixer volume 0dB */
  counter += CODEC_IO_Write(DeviceAddr, 0x10, 0x00);

  /* PCMB mixer volume control : PCMB mixer channel mute off, PCMB mixer volume 0dB */
  counter += CODEC_IO_Write(DeviceAddr, 0x11, 0x00);

  /* PCM channel mixer : AOUTA Left, AOUTB Right */
  counter += CODEC_IO_Write(DeviceAddr, 0x18, 0x00);

  if(Device & OUTPUT_DEVICE_HEADPHONE)
  {
    Value = VOLUME_CONVERT(Volume);
    /* AOUTA volume control : AOUTA volume */
    counter += CODEC_IO_Write(DeviceAddr, 0x16, Value);
    /* AOUTB volume control : AOUTB volume */
    counter += CODEC_IO_Write(DeviceAddr, 0x17, Value);
  }
  
  /* Store device */
  CS42L51_Device = Device;
  
  /* Return communication control value */
  return counter;  
}

/**
  * @brief  Deinitialize the audio codec.
  * @param  None
  * @retval  None
  */
void cs42l51_DeInit(void)
{
  /* Deinitialize Audio Codec interface */
  AUDIO_IO_DeInit();
  
  Is_CS42L51_Initialized = 0;
}

/**
  * @brief  Get the CS42L51 ID.
  * @param DeviceAddr: Device address on communication Bus.   
  * @retval The CS42L51 ID 
  */
uint32_t cs42l51_ReadID(uint16_t DeviceAddr)
{
  uint8_t Value;
  
  if(Is_CS42L51_Initialized == 0)
  {
    /* Initialize the Control interface of the Audio Codec */
    AUDIO_IO_Init(); 
    
    Value = AUDIO_IO_Read(DeviceAddr, CS42L51_CHIPID_ADDR);
    Value = (Value & CS42L51_ID_MASK);

    /* Deinitialize Audio Codec interface */
    AUDIO_IO_DeInit();
  }
  else
  {
    Value = AUDIO_IO_Read(DeviceAddr, CS42L51_CHIPID_ADDR);
    Value = (Value & CS42L51_ID_MASK);
  }
  
  return((uint32_t) Value);
}

/**
  * @brief Start the audio Codec play feature.
  * @note For this codec no Play options are required.
  * @param DeviceAddr: Device address on communication Bus.   
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_Play(uint16_t DeviceAddr, uint16_t* pBuffer, uint16_t Size)
{
  uint32_t counter = 0;
  uint8_t  Value;
  
  if(Is_CS42L51_Stop == 1)
  {
    /* Unmute output device */  
    counter += cs42l51_SetMute(DeviceAddr, AUDIO_MUTE_OFF);

    if(CS42L51_Device & OUTPUT_DEVICE_HEADPHONE)
    {
      /* DAC control : Signal processing to DAC, Freeze off, De-emphasis off, Analog output auto mute off, DAC soft ramp */
      counter += CODEC_IO_Write(DeviceAddr, 0x09, 0x42);

      /* Power control 1 : PDN_DACA, PDN_DACB disable. */
      Value = AUDIO_IO_Read(DeviceAddr, 0x02);
      counter += CODEC_IO_Write(DeviceAddr, 0x02, (Value & 0x9F));
    }

    if(CS42L51_Device & INPUT_DEVICE_MIC1)
    {
      /* Power control 1 : PDN_PGAA, PDN_ADCA disable. */
      Value = AUDIO_IO_Read(DeviceAddr, 0x02);
      counter += CODEC_IO_Write(DeviceAddr, 0x02, (Value & 0xF5));
      
      /* Mic Power and Speed Control : PDN_MICA, PDN_MIC_BIAS disable. */
      Value = AUDIO_IO_Read(DeviceAddr, 0x03);
      counter += CODEC_IO_Write(DeviceAddr, 0x03, (Value & 0xF9));
    }
    
    /* Power control : Exit standby (PDN = 0) */
    Value = AUDIO_IO_Read(DeviceAddr, 0x02);
    counter += CODEC_IO_Write(DeviceAddr, 0x02, (Value & 0xFE));

    Is_CS42L51_Stop = 0;
  }
  
  /* Return communication control value */
  return counter;  
}

/**
  * @brief Pause playing on the audio codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_Pause(uint16_t DeviceAddr)
{  
  uint32_t counter = 0;
 
  /* Pause the audio file playing */
  /* Mute the output first */
  counter += cs42l51_SetMute(DeviceAddr, AUDIO_MUTE_ON);
   
  return counter;
}

/**
  * @brief Resume playing on the audio codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_Resume(uint16_t DeviceAddr)
{
  uint32_t counter = 0;

  /* Unmute the output  */
  counter += cs42l51_SetMute(DeviceAddr, AUDIO_MUTE_OFF);
  
  return counter;
}

/**
  * @brief Stop audio Codec playing. It powers down the codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @param CodecPdwnMode: selects the  power down mode (currently not used).
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_Stop(uint16_t DeviceAddr, uint32_t CodecPdwnMode)
{
  uint32_t counter = 0;
  uint8_t  Value;
  
  /* Set all power down bits to 1 exept PDN to mute ADCs and DACs*/
  counter += CODEC_IO_Write(DeviceAddr, 0x02, 0x7E);
  Value = AUDIO_IO_Read(DeviceAddr, 0x03);
  counter += CODEC_IO_Write(DeviceAddr, 0x03, (Value | 0x0E));
  
  /* Disable zero cross and soft ramp */
  Value = AUDIO_IO_Read(DeviceAddr, 0x09);
  counter += CODEC_IO_Write(DeviceAddr, 0x09, (Value & 0xFC));

  /* Power control : Enter standby (PDN = 1) */
  Value = AUDIO_IO_Read(DeviceAddr, 0x02);
  counter += CODEC_IO_Write(DeviceAddr, 0x02, (Value | 0x01));

  Is_CS42L51_Stop = 1;
  return counter;
}

/**
  * @brief Set higher or lower the codec volume level.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param Volume: output volume level (from 0 (-100dB) to 100 (0dB)).
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_SetVolume(uint16_t DeviceAddr, uint8_t Volume)
{
  uint32_t counter = 0;
  uint8_t  convertedvol = VOLUME_CONVERT(Volume);

  /* AOUTA volume control : AOUTA volume */
  counter += CODEC_IO_Write(DeviceAddr, 0x16, convertedvol);
  /* AOUTB volume control : AOUTB volume */
  counter += CODEC_IO_Write(DeviceAddr, 0x17, convertedvol);

  return counter;
}

/**
  * @brief Set new frequency.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param AudioFreq: Audio frequency used to play the audio stream.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_SetFrequency(uint16_t DeviceAddr, uint32_t AudioFreq)
{
  return 0;
}

/**
  * @brief Enable or disable the mute feature on the audio codec.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
  *             mute mode.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_SetMute(uint16_t DeviceAddr, uint32_t Cmd)
{
  uint32_t counter = 0;
  uint8_t  Value;
  
  /* Read DAC output control register */
  Value = AUDIO_IO_Read(DeviceAddr, 0x08);

  /* Set the Mute mode */
  if(Cmd == AUDIO_MUTE_ON)
  {
    /* Mute DAC channels */
    counter += CODEC_IO_Write(DeviceAddr, 0x08, (Value | 0x03));
  }
  else /* AUDIO_MUTE_OFF Disable the Mute */
  {
    /* Unmute DAC channels */
    counter += CODEC_IO_Write(DeviceAddr, 0x08, (Value & 0xFC));
  }
  return counter;
}

/**
  * @brief Switch dynamically (while audio file is played) the output target 
  *         (speaker, headphone, etc).
  * @note This function is currently not used (only headphone output device).
  * @param DeviceAddr: Device address on communication Bus.
  * @param Output: specifies the audio output device target. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_SetOutputMode(uint16_t DeviceAddr, uint8_t Output)
{
  return 0;
}

/**
  * @brief Reset CS42L51 registers.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t cs42l51_Reset(uint16_t DeviceAddr)
{
  if(Is_CS42L51_Initialized == 1)
  {
    /* Deinitialize Audio Codec interface */
    AUDIO_IO_DeInit();

    /* Initialize the Control interface of the Audio Codec */
    AUDIO_IO_Init();
  }
  return 0;
}

/**
  * @}
  */

/** @addtogroup CS42L51_Private_Functions
  * @{
  */ 

/**
  * @brief  Write and optionally read back a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address 
  * @param  Value: Data to be written
  * @retval None
  */
static uint8_t CODEC_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  uint32_t result = 0;
  
  AUDIO_IO_Write(Addr, Reg, Value);
  
#ifdef VERIFY_WRITTENDATA
  /* Verify that the data has been correctly written */  
  result = (AUDIO_IO_Read(Addr, Reg) == Value)? 0:1;
#endif /* VERIFY_WRITTENDATA */
  
  return result;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
