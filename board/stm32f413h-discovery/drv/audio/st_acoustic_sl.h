/**
  ******************************************************************************
  * @file    DFSDM/DFSDM_PulseSkipper/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUDIO_DEMO_H
#define __AUDIO_DEMO_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>
#include "../stm32f413h_discovery.h"
#include <pulse_skipper.h>
#include "wm8994/wm8994.h"


#define DFSDM_DATIN0_CHANNEL                            DFSDM_CHANNEL_0
#define DFSDM_DATIN1_CHANNEL                            DFSDM_CHANNEL_1
#define DFSDM_DATIN2_CHANNEL                            DFSDM_CHANNEL_2
#define DFSDM_DATIN3_CHANNEL                            DFSDM_CHANNEL_3
#define DFSDM_DATIN4_CHANNEL                            DFSDM_CHANNEL_4
#define DFSDM_DATIN5_CHANNEL                            DFSDM_CHANNEL_5
#define DFSDM_DATIN6_CHANNEL                            DFSDM_CHANNEL_6
#define DFSDM_DATIN7_CHANNEL                            DFSDM_CHANNEL_7

#define DFSDM1_DATIN0_INSTANCE                          DFSDM1_Channel0
#define DFSDM1_DATIN1_INSTANCE                          DFSDM1_Channel1
#define DFSDM1_DATIN2_INSTANCE                          DFSDM1_Channel2
#define DFSDM1_DATIN3_INSTANCE                          DFSDM1_Channel3
#define DFSDM1_FILTER0                                  DFSDM1_Filter0
#define DFSDM1_FILTER1                                  DFSDM1_Filter1

#define DFSDM2_DATIN0_INSTANCE                          DFSDM2_Channel0
#define DFSDM2_DATIN1_INSTANCE                          DFSDM2_Channel1
#define DFSDM2_DATIN2_INSTANCE                          DFSDM2_Channel2
#define DFSDM2_DATIN3_INSTANCE                          DFSDM2_Channel3
#define DFSDM2_DATIN4_INSTANCE                          DFSDM2_Channel4
#define DFSDM2_DATIN5_INSTANCE                          DFSDM2_Channel5
#define DFSDM2_DATIN6_INSTANCE                          DFSDM2_Channel6
#define DFSDM2_DATIN7_INSTANCE                          DFSDM2_Channel7
#define DFSDM2_FILTER0                                  DFSDM2_Filter0
#define DFSDM2_FILTER1                                  DFSDM2_Filter1
#define DFSDM2_FILTER2                                  DFSDM2_Filter2
#define DFSDM2_FILTER3                                  DFSDM2_Filter3

#define DFSDM1_CKOUT_PIN                                GPIO_PIN_8
#define DFSDM1_CKOUT_PORT                               GPIOA
#define DFSDM1_CKOUT_ALTERNATE                          GPIO_AF6_DFSDM1
#define __DFSDM1_CKOUT_ENABLE()                         __HAL_RCC_GPIOA_CLK_ENABLE() 
#define __DFSDM1_CKOUT_DISABLE()                        __HAL_RCC_GPIOA_CLK_DISABLE()

#define DFSDM2_CKOUT_PIN                                GPIO_PIN_2
#define DFSDM2_CKOUT_PORT                               GPIOD
#define DFSDM2_CKOUT_ALTERNATE                          GPIO_AF3_DFSDM2
#define __DFSDM2_CKOUT_ENABLE()                         __HAL_RCC_GPIOD_CLK_ENABLE() 
#define __DFSDM2_CKOUT_DISABLE()                        __HAL_RCC_GPIOD_CLK_DISABLE()

#define DFSDM1_DATIN1_PIN                               GPIO_PIN_6
#define DFSDM1_DATIN1_PORT                              GPIOD
#define DFSDM1_DATIN1_ALTERNATE                         GPIO_AF6_DFSDM1
#define __DFSDM1_DATIN1_ENABLE()                        __HAL_RCC_GPIOD_CLK_ENABLE()
#define __DFSDM1_DATIN1_DISABLE()                       __HAL_RCC_GPIOD_CLK_DISABLE()

#define DFSDM2_DATIN1_PIN                               GPIO_PIN_7
#define DFSDM2_DATIN1_PORT                              GPIOA
#define DFSDM2_DATIN1_ALTERNATE                         GPIO_AF7_DFSDM2
#define __DFSDM2_DATIN1_ENABLE()                        __HAL_RCC_GPIOA_CLK_ENABLE()
#define __DFSDM2_DATIN1_DISABLE()                       __HAL_RCC_GPIOA_CLK_DISABLE()

#define DFSDM2_DATIN7_PIN                               GPIO_PIN_7
#define DFSDM2_DATIN7_PORT                              GPIOB
#define DFSDM2_DATIN7_ALTERNATE                         GPIO_AF6_DFSDM2
#define __DFSDM2_DATIN7_ENABLE()                        __HAL_RCC_GPIOB_CLK_ENABLE()
#define __DFSDM2_DATIN7_DISABLE()                       __HAL_RCC_GPIOB_CLK_DISABLE()

#define __HAL_RCC_DFSDMx_CLK_ENABLE()                   do {__HAL_RCC_DFSDM1_CLK_ENABLE(); \
                                                            __HAL_RCC_DFSDM2_CLK_ENABLE();    \
                                                        }while(0)
#define __HAL_RCC_DFSDMxAUDIO_CONFIG()                  do {__HAL_RCC_DFSDM1AUDIO_CONFIG(RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB1); \
                                                            __HAL_RCC_DFSDM2AUDIO_CONFIG(RCC_DFSDM2AUDIOCLKSOURCE_I2SAPB1);    \
                                                        }while(0)                                                  


#define DFSDM_OVER_SAMPLING(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 256 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 256 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 128 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 128 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 64 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 64  \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 32 : 25  \

#define DFSDM_CLOCK_DIVIDER(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 24 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 48 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 24 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 48 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 24 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 48  \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 32 : 72  \
        
#define DFSDM_FILTER_ORDER(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? DFSDM_FILTER_SINC4_ORDER \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? DFSDM_FILTER_SINC4_ORDER  \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? DFSDM_FILTER_SINC4_ORDER : DFSDM_FILTER_SINC4_ORDER  \

#define DFSDM_MIC_BIT_SHIFT(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 5 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 4 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 2 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 2 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 5 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 6  \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 2 : 0  \


#define SaturaLH(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

typedef struct {
	int32_t Z; 
	int32_t oldOut; 
	int32_t oldIn; 
}HP_FilterState_TypeDef;

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

