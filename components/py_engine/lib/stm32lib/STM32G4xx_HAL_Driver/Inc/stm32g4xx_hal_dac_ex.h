/**
  ******************************************************************************
  * @file    stm32g4xx_hal_dac_ex.h
  * @author  MCD Application Team
  * @brief   Header file of DAC HAL Extended module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32G4xx_HAL_DAC_EX_H
#define STM32G4xx_HAL_DAC_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup STM32G4xx_HAL_Driver
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal_def.h"

#if defined(DAC1) || defined(DAC2) || defined(DAC3) ||defined (DAC4)

/** @addtogroup DACEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  HAL State structures definition
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DACEx_Exported_Constants DACEx Exported Constants
  * @{
  */

/** @defgroup DACEx_lfsrunmask_triangleamplitude DACEx lfsrunmask triangle amplitude
  * @{
  */
#define DAC_LFSRUNMASK_BIT0                0x00000000U                                                         /*!< Unmask DAC channel LFSR bit0 for noise wave generation */
#define DAC_LFSRUNMASK_BITS1_0             (                                                   DAC_CR_MAMP1_0) /*!< Unmask DAC channel LFSR bit[1:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS2_0             (                                  DAC_CR_MAMP1_1                 ) /*!< Unmask DAC channel LFSR bit[2:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS3_0             (                                  DAC_CR_MAMP1_1 | DAC_CR_MAMP1_0) /*!< Unmask DAC channel LFSR bit[3:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS4_0             (                 DAC_CR_MAMP1_2                                  ) /*!< Unmask DAC channel LFSR bit[4:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS5_0             (                 DAC_CR_MAMP1_2                  | DAC_CR_MAMP1_0) /*!< Unmask DAC channel LFSR bit[5:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS6_0             (                 DAC_CR_MAMP1_2 | DAC_CR_MAMP1_1                 ) /*!< Unmask DAC channel LFSR bit[6:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS7_0             (                 DAC_CR_MAMP1_2 | DAC_CR_MAMP1_1 | DAC_CR_MAMP1_0) /*!< Unmask DAC channel LFSR bit[7:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS8_0             (DAC_CR_MAMP1_3                                                   ) /*!< Unmask DAC channel LFSR bit[8:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS9_0             (DAC_CR_MAMP1_3                                   | DAC_CR_MAMP1_0) /*!< Unmask DAC channel LFSR bit[9:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS10_0            (DAC_CR_MAMP1_3                  | DAC_CR_MAMP1_1                 ) /*!< Unmask DAC channel LFSR bit[10:0] for noise wave generation */
#define DAC_LFSRUNMASK_BITS11_0            (DAC_CR_MAMP1_3                  | DAC_CR_MAMP1_1 | DAC_CR_MAMP1_0) /*!< Unmask DAC channel LFSR bit[11:0] for noise wave generation */
#define DAC_TRIANGLEAMPLITUDE_1            0x00000000U                                                         /*!< Select max triangle amplitude of 1 */
#define DAC_TRIANGLEAMPLITUDE_3            (                                                   DAC_CR_MAMP1_0) /*!< Select max triangle amplitude of 3 */
#define DAC_TRIANGLEAMPLITUDE_7            (                                  DAC_CR_MAMP1_1                 ) /*!< Select max triangle amplitude of 7 */
#define DAC_TRIANGLEAMPLITUDE_15           (                                  DAC_CR_MAMP1_1 | DAC_CR_MAMP1_0) /*!< Select max triangle amplitude of 15 */
#define DAC_TRIANGLEAMPLITUDE_31           (                 DAC_CR_MAMP1_2                                  ) /*!< Select max triangle amplitude of 31 */
#define DAC_TRIANGLEAMPLITUDE_63           (                 DAC_CR_MAMP1_2                  | DAC_CR_MAMP1_0) /*!< Select max triangle amplitude of 63 */
#define DAC_TRIANGLEAMPLITUDE_127          (                 DAC_CR_MAMP1_2 | DAC_CR_MAMP1_1                 ) /*!< Select max triangle amplitude of 127 */
#define DAC_TRIANGLEAMPLITUDE_255          (                 DAC_CR_MAMP1_2 | DAC_CR_MAMP1_1 | DAC_CR_MAMP1_0) /*!< Select max triangle amplitude of 255 */
#define DAC_TRIANGLEAMPLITUDE_511          (DAC_CR_MAMP1_3                                                   ) /*!< Select max triangle amplitude of 511 */
#define DAC_TRIANGLEAMPLITUDE_1023         (DAC_CR_MAMP1_3                                   | DAC_CR_MAMP1_0) /*!< Select max triangle amplitude of 1023 */
#define DAC_TRIANGLEAMPLITUDE_2047         (DAC_CR_MAMP1_3                  | DAC_CR_MAMP1_1                 ) /*!< Select max triangle amplitude of 2047 */
#define DAC_TRIANGLEAMPLITUDE_4095         (DAC_CR_MAMP1_3                  | DAC_CR_MAMP1_1 | DAC_CR_MAMP1_0) /*!< Select max triangle amplitude of 4095 */

/**
  * @}
  */

/** @defgroup DACEx_SawtoothPolarityMode DAC Sawtooth polarity mode
  * @{
  */
#define DAC_SAWTOOTH_POLARITY_DECREMENT        0x00000000U             /*!< Sawtooth wave generation, polarity is decrement */
#define DAC_SAWTOOTH_POLARITY_INCREMENT        (DAC_STR1_STDIR1)       /*!< Sawtooth wave generation, polarity is increment */

/**
  * @}
  */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/** @defgroup DACEx_Private_Macros DACEx Private Macros
  * @{
  */
#if defined(STM32G474xx) || defined(STM32G484xx)
#define IS_DAC_TRIGGER(DACX, TRIGGER) \
  (((TRIGGER) == DAC_TRIGGER_NONE)           || \
   ((TRIGGER) == DAC_TRIGGER_SOFTWARE)       || \
   ((TRIGGER) == DAC_TRIGGER_T7_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T15_TRGO)       || \
   ((TRIGGER) == DAC_TRIGGER_T2_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T4_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_EXT_IT9)        || \
   ((TRIGGER) == DAC_TRIGGER_T6_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T3_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_RST_TRG1) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_RST_TRG2) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_RST_TRG3) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_RST_TRG4) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_RST_TRG5) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_RST_TRG6) || \
   (((DACX) == DAC1) &&                         \
    (((TRIGGER) == DAC_TRIGGER_T8_TRGO)      || \
     ((TRIGGER) == DAC_TRIGGER_HRTIM_TRG01))     \
   )                                         || \
   (((DACX) == DAC2) &&                         \
    (((TRIGGER) == DAC_TRIGGER_T8_TRGO)      || \
     ((TRIGGER) == DAC_TRIGGER_HRTIM_TRG02))     \
   )                                         || \
   (((DACX) == DAC3) &&                         \
    (((TRIGGER) == DAC_TRIGGER_T1_TRGO)      || \
     ((TRIGGER) == DAC_TRIGGER_HRTIM_TRG03))     \
   )                                         || \
   (((DACX) == DAC4) &&                         \
    (((TRIGGER) == DAC_TRIGGER_T8_TRGO)      || \
     ((TRIGGER) == DAC_TRIGGER_HRTIM_TRG01))     \
   )                                            \
  )
#else
#define IS_DAC_TRIGGER(DACX, TRIGGER) \
  (((TRIGGER) == DAC_TRIGGER_NONE)           || \
   ((TRIGGER) == DAC_TRIGGER_SOFTWARE)       || \
   ((TRIGGER) == DAC_TRIGGER_T7_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T15_TRGO)       || \
   ((TRIGGER) == DAC_TRIGGER_T2_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T4_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_EXT_IT9)        || \
   ((TRIGGER) == DAC_TRIGGER_T6_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T3_TRGO)        || \
   (((DACX) == DAC3) ?                          \
    ((TRIGGER) == DAC_TRIGGER_T1_TRGO)          \
    : ((TRIGGER) == DAC_TRIGGER_T8_TRGO)        \
   )                                            \
  )
#endif

#if defined(STM32G474xx) || defined(STM32G484xx)
#define IS_DAC_TRIGGER2(DACX, TRIGGER) \
  (((TRIGGER) == DAC_TRIGGER_NONE)            || \
   ((TRIGGER) == DAC_TRIGGER_SOFTWARE)        || \
   ((TRIGGER) == DAC_TRIGGER_T7_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_T15_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T2_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_T4_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_EXT_IT10)        || \
   ((TRIGGER) == DAC_TRIGGER_T6_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_T3_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_STEP_TRG1) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_STEP_TRG2) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_STEP_TRG3) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_STEP_TRG4) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_STEP_TRG5) || \
   ((TRIGGER) == DAC_TRIGGER_HRTIM_STEP_TRG6) || \
   (((DACX) == DAC1) &&                          \
    ((TRIGGER) == DAC_TRIGGER_T8_TRGO)           \
   ) ||                                          \
   (((DACX) == DAC2) &&                          \
    ((TRIGGER) == DAC_TRIGGER_T8_TRGO)           \
   ) ||                                          \
   (((DACX) == DAC3) &&                          \
    ((TRIGGER) == DAC_TRIGGER_T1_TRGO)           \
   ) ||                                          \
   (((DACX) == DAC4) &&                          \
    ((TRIGGER) == DAC_TRIGGER_T8_TRGO)           \
   )                                             \
  )
#else
#define IS_DAC_TRIGGER2(DACX, TRIGGER) \
  (((TRIGGER) == DAC_TRIGGER_NONE)            || \
   ((TRIGGER) == DAC_TRIGGER_SOFTWARE)        || \
   ((TRIGGER) == DAC_TRIGGER_T7_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_T15_TRGO)        || \
   ((TRIGGER) == DAC_TRIGGER_T2_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_T4_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_EXT_IT10)        || \
   ((TRIGGER) == DAC_TRIGGER_T6_TRGO)         || \
   ((TRIGGER) == DAC_TRIGGER_T3_TRGO)         || \
   (((DACX) == DAC3) ?                           \
    ((TRIGGER) == DAC_TRIGGER_T1_TRGO)           \
    :((TRIGGER) == DAC_TRIGGER_T8_TRGO)          \
   )                                             \
  )
#endif
#define  IS_DAC_HIGH_FREQUENCY_MODE(MODE) (((MODE) == DAC_HIGH_FREQUENCY_INTERFACE_MODE_DISABLE)         || \
                                           ((MODE) == DAC_HIGH_FREQUENCY_INTERFACE_MODE_ABOVE_80MHZ)     || \
                                           ((MODE) == DAC_HIGH_FREQUENCY_INTERFACE_MODE_ABOVE_160MHZ)    || \
                                           ((MODE) == DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC))

#define IS_DAC_SAMPLETIME(TIME) ((TIME) <= 0x000003FFU)

#define IS_DAC_HOLDTIME(TIME)   ((TIME) <= 0x000003FFU)

#define IS_DAC_SAMPLEANDHOLD(MODE) (((MODE) == DAC_SAMPLEANDHOLD_DISABLE) || \
                                    ((MODE) == DAC_SAMPLEANDHOLD_ENABLE))

#define IS_DAC_TRIMMINGVALUE(TRIMMINGVALUE) ((TRIMMINGVALUE) <= 0x1FU)

#define IS_DAC_NEWTRIMMINGVALUE(TRIMMINGVALUE) ((TRIMMINGVALUE) <= 0x1FU)

#define IS_DAC_CHIP_CONNECTION(CONNECT) (((CONNECT) == DAC_CHIPCONNECT_EXTERNAL) || \
                                         ((CONNECT) == DAC_CHIPCONNECT_INTERNAL) || \
                                         ((CONNECT) == DAC_CHIPCONNECT_BOTH))

#define IS_DAC_TRIMMING(TRIMMING) (((TRIMMING) == DAC_TRIMMING_FACTORY) || \
                                   ((TRIMMING) == DAC_TRIMMING_USER))

#define IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(VALUE) (((VALUE) == DAC_LFSRUNMASK_BIT0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS1_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS2_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS3_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS4_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS5_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS6_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS7_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS8_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS9_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS10_0) || \
                                                      ((VALUE) == DAC_LFSRUNMASK_BITS11_0) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_1) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_3) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_7) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_15) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_31) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_63) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_127) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_255) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_511) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_1023) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_2047) || \
                                                      ((VALUE) == DAC_TRIANGLEAMPLITUDE_4095))

#define IS_DAC_SAWTOOTH_POLARITY(POLARITY) (((POLARITY) == DAC_SAWTOOTH_POLARITY_DECREMENT) || \
                                            ((POLARITY) == DAC_SAWTOOTH_POLARITY_INCREMENT))

#define IS_DAC_RESET_DATA(DATA) ((DATA) <= 0x00000FFFUL)
#define IS_DAC_STEP_DATA(DATA)  ((DATA) <= 0x0000FFFFUL)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/* Extended features functions ***********************************************/

/** @addtogroup DACEx_Exported_Functions
  * @{
  */

/** @addtogroup DACEx_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *****************************************************/

HAL_StatusTypeDef HAL_DACEx_TriangleWaveGenerate(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Amplitude);
HAL_StatusTypeDef HAL_DACEx_NoiseWaveGenerate(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Amplitude);
HAL_StatusTypeDef HAL_DACEx_SawtoothWaveGenerate(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Polarity,
                                                 uint32_t ResetData, uint32_t StepData);
HAL_StatusTypeDef HAL_DACEx_SawtoothWaveDataReset(DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DACEx_SawtoothWaveDataStep(DAC_HandleTypeDef *hdac, uint32_t Channel);

HAL_StatusTypeDef HAL_DACEx_DualStart(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DACEx_DualStop(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DACEx_DualStart_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t *pData, uint32_t Length,
                                          uint32_t Alignment);
HAL_StatusTypeDef HAL_DACEx_DualStop_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DACEx_DualSetValue(DAC_HandleTypeDef *hdac, uint32_t Alignment, uint32_t Data1, uint32_t Data2);
uint32_t HAL_DACEx_DualGetValue(DAC_HandleTypeDef *hdac);

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_ErrorCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_DMAUnderrunCallbackCh2(DAC_HandleTypeDef *hdac);


/**
  * @}
  */

/** @addtogroup DACEx_Exported_Functions_Group3
  * @{
  */
/* Peripheral Control functions ***********************************************/

HAL_StatusTypeDef HAL_DACEx_SelfCalibrate(DAC_HandleTypeDef *hdac, DAC_ChannelConfTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_DACEx_SetUserTrimming(DAC_HandleTypeDef *hdac, DAC_ChannelConfTypeDef *sConfig, uint32_t Channel,
                                            uint32_t NewTrimmingValue);
uint32_t HAL_DACEx_GetTrimOffset(DAC_HandleTypeDef *hdac, uint32_t Channel);

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup DACEx_Private_Functions
  * @{
  */

/* DAC_DMAConvCpltCh2 / DAC_DMAErrorCh2 / DAC_DMAHalfConvCpltCh2 */
/* are called by HAL_DAC_Start_DMA */
void DAC_DMAConvCpltCh2(DMA_HandleTypeDef *hdma);
void DAC_DMAErrorCh2(DMA_HandleTypeDef *hdma);
void DAC_DMAHalfConvCpltCh2(DMA_HandleTypeDef *hdma);

/**
  * @}
  */

/**
  * @}
  */

#endif /* DAC1 || DAC2 || DAC3 || DAC4 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*STM32G4xx_HAL_DAC_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
