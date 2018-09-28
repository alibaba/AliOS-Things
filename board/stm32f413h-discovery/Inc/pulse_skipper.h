/**
  ******************************************************************************
  * @file    DFSDM/DFSDM_PulseSkipper/Inc/pulse_skipper.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for pulse skipper.
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
#include "stm32f4xx_hal.h"
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint32_t  DFSDM1PulseSkipperCh;    /*!< Channels selection to generate pulse skipper of DFSDM1.
                                         This parameter can be a value of @ref DFSDM1_PULSESKIPPER_CH */
  uint32_t  DFSDM2PulseSkipperCh;    /*!< Channels selection to generate pulse skipper of DFSDM2.
                                         This parameter can be a value of @ref DFSDM2_PULSESKIPPER_CH */
}PulseSkipper_InitTypeDef;
/* Exported constants --------------------------------------------------------*/
/** 
  * @brief  Skipper Delay parameters structure definition  
  */  

/**
  * @brief DFSDM1_PULSESKIPPER_CH
  */
#define DFSDM1_PULSE_SKIPPER_CH02         0x00000001U
#define DFSDM1_PULSE_SKIPPER_CH13         0x00000002U

/**
  * @brief DFSDM2_PULSESKIPPER_CH
  */
#define DFSDM2_PULSE_SKIPPER_CH04         0x00000010U
#define DFSDM2_PULSE_SKIPPER_CH15         0x00000020U
#define DFSDM2_PULSE_SKIPPER_CH26         0x00000040U
#define DFSDM2_PULSE_SKIPPER_CH37         0x00000080U


#define DFSDM1_TIM4_OC1                   TIM_CHANNEL_1
#define DFSDM1_TIM4_OC2                   TIM_CHANNEL_2

#define DFSDM2_TIM3_OC1                   TIM_CHANNEL_1
#define DFSDM2_TIM3_OC2                   TIM_CHANNEL_2
#define DFSDM2_TIM3_OC3                   TIM_CHANNEL_3
#define DFSDM2_TIM3_OC4                   TIM_CHANNEL_4

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void             Pulse_Skipper_Init(void);
void             Pulse_Skipper_Bitstream_Start(void);
void             Pulse_Skipper_Bitstream_Stop(void);
void             Pulse_Skipper_DeInit(void);
void             Pulse_Skipper_Generate_Pulse(PulseSkipper_InitTypeDef* skipperstruct);

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
