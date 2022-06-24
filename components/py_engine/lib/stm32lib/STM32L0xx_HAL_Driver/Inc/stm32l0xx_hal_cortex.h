/**
  ******************************************************************************
  * @file    stm32l0xx_hal_cortex.h
  * @author  MCD Application Team
  * @brief   Header file of CORTEX HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright(c) 2016 STMicroelectronics.
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
#ifndef __STM32L0xx_HAL_CORTEX_H
#define __STM32L0xx_HAL_CORTEX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal_def.h"

/** @addtogroup STM32L0xx_HAL_Driver
  * @{
  */

/** @defgroup CORTEX CORTEX
  * @{
  */
/* Exported types ------------------------------------------------------------*/

/** @defgroup CORTEX_Exported_Types CORTEX Exported Types
  * @{
  */

#if (__MPU_PRESENT == 1)
/** @defgroup CORTEX_MPU_Region_Initialization_Structure_definition MPU Region Initialization Structure Definition
  * @{
  */
typedef struct
{
  uint32_t               BaseAddress;           /*!< Specifies the base address of the region to protect.                           */

  uint8_t                Enable;                /*!< Specifies the status of the region.
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Enable                 */
  uint8_t                Number;                /*!< Specifies the number of the region to protect.
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Number                 */

  uint8_t                Size;                  /*!< Specifies the size of the region to protect.
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Size                   */
  uint8_t                SubRegionDisable;      /*!< Specifies the number of the subregion protection to disable.
                                                     This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF    */
  uint8_t                TypeExtField;          /*!< This parameter is NOT used but is kept to keep API unified through all families*/

  uint8_t                AccessPermission;      /*!< Specifies the region access permission type.
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Permission_Attributes  */
  uint8_t                DisableExec;           /*!< Specifies the instruction access status.
                                                     This parameter can be a value of @ref CORTEX_MPU_Instruction_Access            */
  uint8_t                IsShareable;           /*!< Specifies the shareability status of the protected region.
                                                     This parameter can be a value of @ref CORTEX_MPU_Access_Shareable              */
  uint8_t                IsCacheable;           /*!< Specifies the cacheable status of the region protected.
                                                     This parameter can be a value of @ref CORTEX_MPU_Access_Cacheable              */
  uint8_t                IsBufferable;          /*!< Specifies the bufferable status of the protected region.
                                                     This parameter can be a value of @ref CORTEX_MPU_Access_Bufferable             */
}MPU_Region_InitTypeDef;
/**
  * @}
  */
#endif /* __MPU_PRESENT */

/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup CORTEX_Exported_Constants CORTEx Exported Constants
  * @{
  */


#define IS_NVIC_PREEMPTION_PRIORITY(__PRIORITY__)  ((__PRIORITY__) < 0x4U)

#define IS_NVIC_DEVICE_IRQ(IRQ)                ((IRQ) >= 0x0)

/** @defgroup CORTEX_SysTick_clock_source CORTEX SysTick Clock Source
  * @{
  */
#define SYSTICK_CLKSOURCE_HCLK_DIV8    ((uint32_t)0x00000000U)
#define SYSTICK_CLKSOURCE_HCLK         ((uint32_t)0x00000004U)
#define IS_SYSTICK_CLK_SOURCE(__SOURCE__) (((__SOURCE__) == SYSTICK_CLKSOURCE_HCLK) || \
                                       ((__SOURCE__) == SYSTICK_CLKSOURCE_HCLK_DIV8))
/**
  * @}
  */

#if (__MPU_PRESENT == 1)
/** @defgroup CORTEX_MPU_HFNMI_PRIVDEF_Control CORTEX MPU HFNMI and PRIVILEGED Access control
  * @{
  */
#define  MPU_HFNMI_PRIVDEF_NONE      ((uint32_t)0x00000000U)
#define  MPU_HARDFAULT_NMI           ((uint32_t)0x00000002U)
#define  MPU_PRIVILEGED_DEFAULT      ((uint32_t)0x00000004U)
#define  MPU_HFNMI_PRIVDEF           ((uint32_t)0x00000006U)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Enable CORTEX MPU Region Enable
  * @{
  */
#define  MPU_REGION_ENABLE           ((uint8_t)0x01U)
#define  MPU_REGION_DISABLE          ((uint8_t)0x00U)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Instruction_Access CORTEX MPU Instruction Access
  * @{
  */
#define  MPU_INSTRUCTION_ACCESS_ENABLE      ((uint8_t)0x00U)
#define  MPU_INSTRUCTION_ACCESS_DISABLE     ((uint8_t)0x01U)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Access_Shareable CORTEX MPU Instruction Access Shareable
  * @{
  */
#define  MPU_ACCESS_SHAREABLE        ((uint8_t)0x01U)
#define  MPU_ACCESS_NOT_SHAREABLE    ((uint8_t)0x00U)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Access_Cacheable CORTEX MPU Instruction Access Cacheable
  * @{
  */
#define  MPU_ACCESS_CACHEABLE        ((uint8_t)0x01U)
#define  MPU_ACCESS_NOT_CACHEABLE    ((uint8_t)0x00U)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Access_Bufferable CORTEX MPU Instruction Access Bufferable
  * @{
  */
#define  MPU_ACCESS_BUFFERABLE       ((uint8_t)0x01U)
#define  MPU_ACCESS_NOT_BUFFERABLE   ((uint8_t)0x00U)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Size CORTEX MPU Region Size
  * @{
  */
#define   MPU_REGION_SIZE_32B        ((uint8_t)0x04U)
#define   MPU_REGION_SIZE_64B        ((uint8_t)0x05U)
#define   MPU_REGION_SIZE_128B       ((uint8_t)0x06U)
#define   MPU_REGION_SIZE_256B       ((uint8_t)0x07U)
#define   MPU_REGION_SIZE_512B       ((uint8_t)0x08U)
#define   MPU_REGION_SIZE_1KB        ((uint8_t)0x09U)
#define   MPU_REGION_SIZE_2KB        ((uint8_t)0x0AU)
#define   MPU_REGION_SIZE_4KB        ((uint8_t)0x0BU)
#define   MPU_REGION_SIZE_8KB        ((uint8_t)0x0CU)
#define   MPU_REGION_SIZE_16KB       ((uint8_t)0x0DU)
#define   MPU_REGION_SIZE_32KB       ((uint8_t)0x0EU)
#define   MPU_REGION_SIZE_64KB       ((uint8_t)0x0FU)
#define   MPU_REGION_SIZE_128KB      ((uint8_t)0x10U)
#define   MPU_REGION_SIZE_256KB      ((uint8_t)0x11U)
#define   MPU_REGION_SIZE_512KB      ((uint8_t)0x12U)
#define   MPU_REGION_SIZE_1MB        ((uint8_t)0x13U)
#define   MPU_REGION_SIZE_2MB        ((uint8_t)0x14U)
#define   MPU_REGION_SIZE_4MB        ((uint8_t)0x15U)
#define   MPU_REGION_SIZE_8MB        ((uint8_t)0x16U)
#define   MPU_REGION_SIZE_16MB       ((uint8_t)0x17U)
#define   MPU_REGION_SIZE_32MB       ((uint8_t)0x18U)
#define   MPU_REGION_SIZE_64MB       ((uint8_t)0x19U)
#define   MPU_REGION_SIZE_128MB      ((uint8_t)0x1AU)
#define   MPU_REGION_SIZE_256MB      ((uint8_t)0x1BU)
#define   MPU_REGION_SIZE_512MB      ((uint8_t)0x1CU)
#define   MPU_REGION_SIZE_1GB        ((uint8_t)0x1DU)
#define   MPU_REGION_SIZE_2GB        ((uint8_t)0x1EU)
#define   MPU_REGION_SIZE_4GB        ((uint8_t)0x1FU)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Permission_Attributes CORTEX MPU Region Permission Attributes
  * @{
  */
#define  MPU_REGION_NO_ACCESS        ((uint8_t)0x00U)
#define  MPU_REGION_PRIV_RW          ((uint8_t)0x01U)
#define  MPU_REGION_PRIV_RW_URO      ((uint8_t)0x02U)
#define  MPU_REGION_FULL_ACCESS      ((uint8_t)0x03U)
#define  MPU_REGION_PRIV_RO          ((uint8_t)0x05U)
#define  MPU_REGION_PRIV_RO_URO      ((uint8_t)0x06U)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Number CORTEX MPU Region Number
  * @{
  */
#define  MPU_REGION_NUMBER0          ((uint8_t)0x00U)
#define  MPU_REGION_NUMBER1          ((uint8_t)0x01U)
#define  MPU_REGION_NUMBER2          ((uint8_t)0x02U)
#define  MPU_REGION_NUMBER3          ((uint8_t)0x03U)
#define  MPU_REGION_NUMBER4          ((uint8_t)0x04U)
#define  MPU_REGION_NUMBER5          ((uint8_t)0x05U)
#define  MPU_REGION_NUMBER6          ((uint8_t)0x06U)
#define  MPU_REGION_NUMBER7          ((uint8_t)0x07U)
/**
  * @}
  */
#endif /* __MPU_PRESENT */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CORTEX_Exported_Functions CORTEX Exported Functions
  * @{
  */

/** @defgroup CORTEX_Exported_Functions_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions
 * @{
  */
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
void HAL_NVIC_SystemReset(void);
uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb);
/**
  * @}
  */

/** @defgroup CORTEX_Exported_Functions_Group2 Peripheral Control functions
 *  @brief   Cortex control functions
 * @{
 */

uint32_t HAL_NVIC_GetPriority(IRQn_Type IRQn);
uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource);
void HAL_SYSTICK_IRQHandler(void);
void HAL_SYSTICK_Callback(void);
#if (__MPU_PRESENT == 1U)
void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);
#endif /* __MPU_PRESENT */
/**
  * @}
  */

  /**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup CORTEX_Private_Macros CORTEX Private Macros
  * @{
  */

#if (__MPU_PRESENT == 1)
#define IS_MPU_REGION_ENABLE(STATE) (((STATE) == MPU_REGION_ENABLE) || \
                                     ((STATE) == MPU_REGION_DISABLE))

#define IS_MPU_INSTRUCTION_ACCESS(STATE) (((STATE) == MPU_INSTRUCTION_ACCESS_ENABLE) || \
                                          ((STATE) == MPU_INSTRUCTION_ACCESS_DISABLE))

#define IS_MPU_ACCESS_SHAREABLE(STATE)   (((STATE) == MPU_ACCESS_SHAREABLE) || \
                                          ((STATE) == MPU_ACCESS_NOT_SHAREABLE))

#define IS_MPU_ACCESS_CACHEABLE(STATE)   (((STATE) == MPU_ACCESS_CACHEABLE) || \
                                          ((STATE) == MPU_ACCESS_NOT_CACHEABLE))

#define IS_MPU_ACCESS_BUFFERABLE(STATE)   (((STATE) == MPU_ACCESS_BUFFERABLE) || \
                                          ((STATE) == MPU_ACCESS_NOT_BUFFERABLE))

#define IS_MPU_REGION_PERMISSION_ATTRIBUTE(TYPE) (((TYPE) == MPU_REGION_NO_ACCESS)   || \
                                                  ((TYPE) == MPU_REGION_PRIV_RW)     || \
                                                  ((TYPE) == MPU_REGION_PRIV_RW_URO) || \
                                                  ((TYPE) == MPU_REGION_FULL_ACCESS) || \
                                                  ((TYPE) == MPU_REGION_PRIV_RO)     || \
                                                  ((TYPE) == MPU_REGION_PRIV_RO_URO))

#define IS_MPU_REGION_NUMBER(NUMBER)    (((NUMBER) == MPU_REGION_NUMBER0) || \
                                         ((NUMBER) == MPU_REGION_NUMBER1) || \
                                         ((NUMBER) == MPU_REGION_NUMBER2) || \
                                         ((NUMBER) == MPU_REGION_NUMBER3) || \
                                         ((NUMBER) == MPU_REGION_NUMBER4) || \
                                         ((NUMBER) == MPU_REGION_NUMBER5) || \
                                         ((NUMBER) == MPU_REGION_NUMBER6) || \
                                         ((NUMBER) == MPU_REGION_NUMBER7))

#define IS_MPU_REGION_SIZE(SIZE)    (((SIZE) == MPU_REGION_SIZE_256B)  || \
                                     ((SIZE) == MPU_REGION_SIZE_512B)  || \
                                     ((SIZE) == MPU_REGION_SIZE_1KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_2KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_4KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_8KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_16KB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_32KB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_64KB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_128KB) || \
                                     ((SIZE) == MPU_REGION_SIZE_256KB) || \
                                     ((SIZE) == MPU_REGION_SIZE_512KB) || \
                                     ((SIZE) == MPU_REGION_SIZE_1MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_2MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_4MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_8MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_16MB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_32MB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_64MB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_128MB) || \
                                     ((SIZE) == MPU_REGION_SIZE_256MB) || \
                                     ((SIZE) == MPU_REGION_SIZE_512MB) || \
                                     ((SIZE) == MPU_REGION_SIZE_1GB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_2GB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_4GB))

#define IS_MPU_SUB_REGION_DISABLE(SUBREGION)  ((SUBREGION) < (uint16_t)0x00FFU)
#endif /* __MPU_PRESENT */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L0xx_HAL_CORTEX_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

