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
#ifndef _ADC_MCU_ADC_H_
#define _ADC_MCU_ADC_H_

#include "BCDS_HAL.h"
#include "BCDS_Basics.h"
#include "em_adc.h"

/* Include all headers which are needed by this file. */
/**
 * @brief ADC_T is a generic hardware handle,which is void pointer guaranteed to store pointer to integer
 */
typedef HWHandle_T ADC_T;
/**
 * @brief Enumerator to represent the Input channel, This is applicable for single trigger mode only
 */
enum Adc_Channel_E
{
    ADC_ENABLE_CH4,
    ADC_ENABLE_CH5,
    ADC_ENABLE_CH6,
    ADC_ENABLE_CH7,
    ADC_ENABLE_TEMP,
    ADC_ENABLE_VDDDIV3,
    ADC_ENABLE_VDD,
    ADC_ENABLE_VSS,
    ADC_ENABLE_VREFDIV2,
    ADC_ENABLE_CH_MAX
};
typedef enum Adc_Channel_E Adc_Channel_T;

/**
 * @brief Enumerator to represent the reference voltage
 */
enum Adc_Reference_E
{
    ADC_REF_1V25 = _ADC_SINGLECTRL_REF_1V25,/**< Internal 1.25V reference. */
    ADC_REF_2V5 = _ADC_SINGLECTRL_REF_2V5,/** Internal 2.5V reference. */
    ADC_REF_VDD = _ADC_SINGLECTRL_REF_VDD,/** Buffered VDD. */
    ADC_REF_5VDIFF = _ADC_SINGLECTRL_REF_5VDIFF,/** Internal differential 5V reference. */
    ADC_REF_ExtSingle = _ADC_SINGLECTRL_REF_EXTSINGLE, /** Single ended ext. ref. from pin 6. */
    ADC_REF_ExtDiff = _ADC_SINGLECTRL_REF_2XEXTDIFF, /** Differential ext. ref. from pin 6 and 7. */
    ADC_REF_2xVDD = _ADC_SINGLECTRL_REF_2XVDD, /** Unbuffered 2xVDD. */
};
typedef enum Adc_Reference_E Adc_Reference_T;

/**
 * @brief Enumerator to represent the resolution
 */
enum Adc_Resolution_E
{
    ADC_RESOLUTION_12BIT = _ADC_SINGLECTRL_RES_12BIT, /**< 12 bit sampling. */
    ADC_RESOLUTION_8BIT = _ADC_SINGLECTRL_RES_8BIT, /**< 8 bit sampling. */
    ADC_RESOLUTION_6BIT = _ADC_SINGLECTRL_RES_6BIT, /**< 6 bit sampling. */
    ADC_RESOLUTION_OVS = _ADC_SINGLECTRL_RES_OVS, /**< Oversampling. */
};
typedef enum Adc_Resolution_E Adc_Resolution_T;

/**
 * @brief Enumerator to represent the Acquisition time
 */
enum Adc_AcqTime_E
{
    ADC_ACQ_TIME_1 = _ADC_SINGLECTRL_AT_1CYCLE, /**< 1 clock cycle. */
    ADC_ACQ_TIME_2 = _ADC_SINGLECTRL_AT_2CYCLES, /**< 2 clock cycle. */
    ADC_ACQ_TIME_4 = _ADC_SINGLECTRL_AT_4CYCLES, /**< 4 clock cycle. */
    ADC_ACQ_TIME_8 = _ADC_SINGLECTRL_AT_8CYCLES, /**< 8 clock cycle. */
    ADC_ACQ_TIME_16 = _ADC_SINGLECTRL_AT_16CYCLES, /**< 16 clock cycle. */
    ADC_ACQ_TIME_32 = _ADC_SINGLECTRL_AT_32CYCLES, /**< 32 clock cycle. */
    ADC_ACQ_TIME_64 = _ADC_SINGLECTRL_AT_64CYCLES, /**< 64 clock cycle. */
    ADC_ACQ_TIME_128 = _ADC_SINGLECTRL_AT_128CYCLES, /**< 128 clock cycle. */
    ADC_ACQ_TIME_256 = _ADC_SINGLECTRL_AT_256CYCLES, /**< 256 Clock cycle. */

};
typedef enum Adc_AcqTime_E Adc_AcqTime_T;
/**
 * @brief Constant values to represent the Input channel, This is applicable for scan trigger mode only
 * @note  For multiple channel selection User needs to bitwise OR the respective Scan channels mentioned below for
 *        Proper operation
 */
enum Adc_ScanChannelMask_E
{
    ADC_ENABLE_CH4_SCAN = ADC_SCANCTRL_INPUTMASK_CH4,
    ADC_ENABLE_CH5_SCAN = ADC_SCANCTRL_INPUTMASK_CH5,
    ADC_ENABLE_CH6_SCAN = ADC_SCANCTRL_INPUTMASK_CH6,
};

/**
 * @brief Structure to represent the events that can be received from the ADC in the callback function.
 */
struct Mcu_Adc_Event_S
{
    uint32_t ScanComplete :1;
    uint32_t SingleComplete :1;
    uint32_t ErrorInConversion :1;
    uint32_t ScanReadDataOverflow :1;
    uint32_t SingleReadDataOverflow :1;
};
typedef struct Mcu_Adc_Event_S Mcu_Adc_Event_T;

/**
 * @brief union to combine event bit field structure with a uint32_t value
 */
union Mcu_Adc_Event_U
{
    struct Mcu_Adc_Event_S Bitfield;
    uint32_t RegisterValue;
};

/**
 * @brief struct to configure ADC in single mode
 */
struct Mcu_Adc_ConfigSingle_S
{
    Adc_Channel_T Channel;
    uint16_t* BufferPtr;
    Adc_Reference_T Reference;
    Adc_Resolution_T Resolution;
    Adc_AcqTime_T AcqTime;
};
typedef struct Mcu_Adc_ConfigSingle_S Mcu_Adc_ConfigSingle_T, *Mcu_Adc_ConfigSinglePtr_T;

/**
 * @brief struct to configure ADC in scan mode
 */
struct Mcu_Adc_ConfigScan_S
{
    uint32_t ChannelScanMask;
    uint16_t *BufferPtr;
    uint32_t SamplingRateInHz;
    uint16_t NoOfSamples;
    Adc_Reference_T Reference;
    Adc_Resolution_T Resolution;
    Adc_AcqTime_T AcqTime;
};
typedef struct Mcu_Adc_ConfigScan_S Mcu_Adc_ConfigScan_T, *Mcu_Adc_ConfigScanPtr_T;

/**
 * @brief Application callback template
 * @param[in] adc       MCU handle for ADC
 * @param[in] event     Represents the various application events
 * @param[in] bufferPtr Represents the pointer value of the result buffer for processing
 */
typedef void (*Mcu_Adc_Callback_T)(ADC_T adc, Mcu_Adc_Event_T event, uint16_t *bufferPtr);

/* Put the type and macro definitions here */

/* Put the function declarations here */

/**
 * @brief   Initialize the ADC Interface.
 *
 * @param [in]  adc : ADC handle.
 *
 * @param [in] callback : Function pointer to a callback function
 *                        which handles the ADC events.
 *
 * @retval  RETCODE_OK upon successful execution, error otherwise
 */
Retcode_T Mcu_Adc_Initialize(ADC_T adc, Mcu_Adc_Callback_T callback);

/**
 * @brief   De-initialize and powers down the ADC interface.
 *
 * @param [in]  adc : ADC handle.
 *
 * @retval RETCODE_OK upon successful execution, error otherwise
 */
Retcode_T Mcu_Adc_DeInitialize(ADC_T adc);

/**
 * @brief   Start Single Mode on ADC Interface.
 *
 * @param [in]  adc : ADC handle.
 *
 * @param [in] configStart : Structure which holds the configurations related to single mode.
 *
 * @retval  RETCODE_OK upon successful execution, error otherwise
 */
Retcode_T Mcu_Adc_StartSingle(ADC_T adc, Mcu_Adc_ConfigSinglePtr_T configStart);

/**
 * @brief   Start Scan Mode on ADC Interface & Start the PRS Timer as producer.
 *
 * @param [in]  adc : ADC handle.
 *
 * @param [in] configScan : Structure which holds the configurations related to scan mode.
 *
 * @Note The sampling rate must be limited to  minimum and maximum range mentioned by macro ADC_MIN_SAMPLING_FREQUENCY and ADC_MAX_SAMPLING_FREQUENCY respectively
 *
 * @retval  RETCODE_OK upon successful execution, error otherwise
 */
Retcode_T Mcu_Adc_StartScan(ADC_T adc, Mcu_Adc_ConfigScanPtr_T configScan);

/**
 * @brief   Stop the Scan mode on ADC Interface and stops the PRS timer used for sampling adc channels.
 *
 * @param [in]  adc : ADC handle.
 *
 * @retval RETCODE_OK upon successful execution, error otherwise
 */
Retcode_T Mcu_Adc_StopScan(ADC_T adc);

#endif /* _ADC_MCU_ADC_H_ */

