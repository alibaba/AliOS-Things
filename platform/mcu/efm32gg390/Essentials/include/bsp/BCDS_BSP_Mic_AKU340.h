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
 *
 *  @file
 *
 *  @defgroup BCDS_HAL_BSP_Mic_AKU340 Mic_AKU340
 *  @brief Hardware abstraction layer for the AKU340 microphone functionalities
 *
 *
 *  @ingroup  BCDS_HAL_BSP_IF
 *  @{
 *
 * Typically the application needs to call BSP_Mic_AKU340_Connect() to establish
 * the link between the ADC and the AKU340 peripheral
 * then it enables it via the BSP_Mic_AKU340_Enable() after that it is possible
 * to sense the signal it using BSP_Mic_AKU340_Sense() which the value of the
 * microphone output voltage.
 *
 * The application can then disable the AKU340 and disconnect it from the MCU
 * via BSP_Mic_AKU340_Disable() and BSP_Mic_AKU340_Disconnect() functions.
 *
 */
#ifndef BCDS_BSP_Mic_AKU340_H
#define BCDS_BSP_Mic_AKU340_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_MIC_AKU340

/**
 * @brief Maps the GPIO pins To their desired function
 * @details The function once called will map the GPIO pins dedicated for the
 * peripheral to  their desired function.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Mic_AKU340_Connect(void);

/**
 * @brief Enables the control over the on board AKU340
 * @details This function once called will enable the AKU340 microphone and
 * start the ADC conversion for its channel.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Mic_AKU340_Enable(void);

/**
 * @deprecated This API is deprecated.
 *
 * @brief Senses the AKU340 Microphone signal
 *
 * @details This function is called to trigger an ADC Conversion at the
 * microphone pin and return the measured value.
 *
 * @return returns INT32_MIN.
 */
BCDS_DEPRECATED(int32_t BSP_Mic_AKU340_Sense(void));

/**
 * @brief Disables the control over the AKU340.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Mic_AKU340_Disable(void);

/**
 * @brief Disconnects the peripheral from the MCU
 * @details This function once called will disconnect the peripheral from the
 * MCU and puts the pins into low power consumption status.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Mic_AKU340_Disconnect(void);

#endif /* BCDS_FEATURE_BSP_MIC_AKU340*/

#endif /* BCDS_BSP_Mic_AKU340_H */

/**  @} */
