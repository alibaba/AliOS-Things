/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 * @brief Add a brief description here.
 *
 * @details Put here the documentation of this header file. Explain the interface exposed
 * by this header, e.g. what is the purpose of use, how to use it, etc.
 */
#ifndef MCU_ADC_HANDLE_H_
#define MCU_ADC_HANDLE_H_

/* Include all headers which are needed by this file. */
#include "Mcu_Adc.h"
#include "em_adc.h"

/* Put the type and macro definitions here */
typedef void (*ADC_IRQ_Callback_T)(ADC_T Adc);
typedef void (*ADC_DMA_Callback_T)(ADC_T Adc);

struct Mcu_Adc_Driver_S
{
    Mcu_Adc_Callback_T AppCallback;  /**< Callback function given in Mcu_Adc_Initialize to notify application */
    uint32_t NumOfSamples;
    uint32_t NumOfScanChannels;
    void *   PointerValue;                   /**< Hook anything to it ,Reserved For future use */
    uint16_t *SingleBuffer;
    uint16_t *ScanBuffer1;
    uint16_t *ScanBuffer2;
    bool     Initialized;              /**< driver initialization state */
};
typedef struct Mcu_Adc_Driver_S Mcu_Adc_Driver_T;

/**
* @brief   Structure used as ADC handle.
* @detail  A pointer to this structure is wrapped in ADC_T for interface functions.
*
* @note This handle is a forward declared structure in BSP and
*/
struct Mcu_Adc_Handle_S
{
    ADC_TypeDef* Instance;                       /**< HW instance for the Adc set by the BSP */
    enum BCDS_HAL_TransferMode_E TransferMode;   /**< Set by BSP to tell MCU whether to use interrupt-mode or DMA */
    ADC_IRQ_Callback_T IRQCallback;              /**< Function invoked by BSP in case IRQ and interrupt mode */
    ADC_DMA_Callback_T DmaSingleCallback;        /**< Function invoked by BSP in case IRQ and Single Conversion Complete DMA mode */
    ADC_DMA_Callback_T DmaScanCallback;          /**< Function invoked by BSP in case  IRQ and Scan Conversion Complete DMA mode  */
    void * Link1RegPtr;                                /**< general purpose link register 1 (used for e.g. DMA Single Conversion Complete handle) */
    void * Link2RegPtr;                                /**< general purpose link register 2 (used for e.g. DMA Scan Conversion Complete handle) */
    Mcu_Adc_Driver_T   _DriverCtx;               /**< context pointer to driver context, private member */
};


/* Put the function declarations here */

#endif /* MCU_ADC_HANDLE_H_ */

