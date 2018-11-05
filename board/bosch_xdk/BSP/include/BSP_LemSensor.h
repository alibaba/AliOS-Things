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

#ifndef BSP_LEMSENSOR_H
#define BSP_LEMSENSOR_H

#include "BCDS_HAL.h"
#include "BSP_ExtensionPort.h"
#if BCDS_FEATURE_BSP_LEM_SENSOR

#define LEM_SENSOR_CONNECT_STATUS_PIN                 BSP_EXTENSIONPORT_GPIO_PE2

/**
 * @brief Connects the peripheral (LEM Sensor).
 * @details This function once called will map the GPIO pins to their desired
 * functions to drive the LEM Sensor peripheral. It is needed to call this
 * function prior to the BSP_LemSensor_Enable().
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LemSensor_Connect(void);

/**
 * @brief Disconnects the peripheral (LEM Sensor).
 * @details This function disconnects the GPIO pins dedicated to the
 * LEM Sensor peripheral and put them into disabled state.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LemSensor_Disconnect(void);

/**
 * @brief Enables the control over the peripheral(LEM Sensor).
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LemSensor_Enable(void);

/**
 * @brief Disables the control over the peripheral(LEM Sensor).
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LemSensor_Disable(void);

/**
 * @brief Get the Status of the peripheral(LEM Sensor).
 *
 * @param[in/out] isLemConnected: To get the LEM Sensor connected status, if the sensor connected it's true otherwise false.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LemSensor_GetStatus(uint8_t *isLemConnected);

#endif //BCDS_FEATURE_BSP_LEM_SENSOR
#endif //BSP_LEMSENSOR_H
