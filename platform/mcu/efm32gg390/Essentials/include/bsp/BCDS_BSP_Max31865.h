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

#ifndef BCDS_BSP_MAX31865_H
#define BCDS_BSP_MAX31865_H

#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_MAX31865

/* Put the type and macro definitions here */

/**
 * @brief Maps the GPIO pins to their desired function.
 * @details This function once called will map the GPIO pins to their desired
 * functions to drive the Temperature Sensor peripheral. It is needed to call this
 * function prior to the BSP_Max31865_Enable().
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Max31865_Connect(void);

/**
 * @brief Disconnects the Temperature peripheral.
 * @details This function disconnects the GPIO pins dedicated to the
 * Temperature Sensor peripheral and put them into low power consumption state.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Max31865_Disconnect(void);

/**
 * @brief Enables the control over the peripheral.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Max31865_Enable(void);

/**
 * @brief Disables the control over the peripheral.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Max31865_Disable(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral.
 *
 * @return HWHandle_T
 */
HWHandle_T BSP_Max31865_GetSPIHandle(void);

/**
 * @brief Sets the Max31865 chip select pin to logic level high.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Max31865_SetCSHigh(void);

/**
 * @brief Sets the Max31865 chip select pin to logic level low.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Max31865_SetCSLow(void);

#endif // BCDS_FEATURE_BSP_MAX31865

#endif /* BCDS_BSP_MAX31865_H */
