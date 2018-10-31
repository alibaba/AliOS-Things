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
 * @brief  This module contains the common software API's for ADC peripheral interface.
 *
 * @details  The main purpose of this module is to have the hardware related code for ADC peripheral, such that design principle, separation of concern is achieved.
 *           The low level (HW) related code is implemented in this module
 */
#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

/* Include all headers which are needed by this file. */

#include "BCDS_HAL.h"

#if BCDS_FEATURE_ADC

/* Put the type and macro definitions here */

/* Put the function declarations here */

/**
 * @brief Returns the handle for the ADC resource of MCU
 *
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application can call this function in order to get the handle needed for the MCU ADC resource used by the ADC driver.
 * This should be called after calling BSP_Adc_Connect()
 *
 * @return HWHandle_T: Handle for the used MCU resource, in this case it is ADC interface
 */
HWHandle_T BSP_Adc_GetHandle(void);

/**
 * @brief  Call this API to prepare the ADC for its proper operation
 *
 * @details  This prepares the necessary mappings and data handling required for ADC driver.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 *
 * @note This is the first call that needs to be called from the application code to use ADC functionality
 */
Retcode_T BSP_Adc_Connect(void);

/**
 * @brief Enables the control over the ADC peripheral interface
 *
 * @details A prior call to Mcu_Adc_Initialize() is necessary for this
 * function in order to operate. This function once called will prepare the
 * ADC peripheral to a functional state such that user can start using the ADC driver API
 *
 * Prerequisite: Before calling BSP_Adc_Enable() API user needs to call the following API's
 *                BSP_Adc_Connect(), Mcu_Adc_Initialize() for proper operation.
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_NULL_POINTER if either there is a pointer corruption or the relevant ADC driver callback are not mapped
 */
Retcode_T BSP_Adc_Enable(void);

/**
 * @brief Disables the control over the ADC peripheral interface
 *
 * @details This function once called will disable control over the ADC peripheral to a Non-functional state.
 *           After this call the ADC driver API calls will not be successful.
 *
 * Prerequisite: User must have called BSP_Adc_Connect() before calling this API
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_NULL_POINTER if either there is a pointer corruption
 */
Retcode_T BSP_Adc_Disable(void);

/**
 * @brief  Call this API to properly tear down the ADC operation
 *
 * @details  This API Make adc instance to NULL so that API calls from ADI Driver must not get success, once after called this function.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Adc_Disconnect(void);

#endif // BCDS_FEATURE_ADC

#endif /* _BSP_ADC_H_ */

