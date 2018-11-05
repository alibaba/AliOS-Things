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
 *  @defgroup BCDS_HAL_BSP_IRSensor_Node IRSensor_Node
 *  @brief Generic interface to the  IR sensor node BSP API
 *
 *
 *  @ingroup  BCDS_HAL_BSP_IF
 *  @{
 *
 * @note A sensor node is defined as a group of peripheral sensors sharing the
 * same MCU resource such an I2C bus.

 * A prior call to BSP_Board_Initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the sensor node.
 *
 * Once the sensor node is requested a call to BSP_SensorNode_Connect() function
 * is required to map the internal MCU resources to their associated function.

 * The application needs get the handle for the MCU resource driver via
 * BSP_SensorNode_GetHandle() and to initialize the MCU resource driver by
 * calling the MCU_<RESOURCE>_Initialize() and providing it with the previously
 * acquired handle and an event callback function.

 * Then a call to BSP_SensorNode_Enable() will execute the required sequence
 * to enable the specified sensor peripheral and the MCU resources dedicated for
 * it (UART, SPI, I2C, etc.) and will associate the interrupt requests to their
 * appropriate service routines.
 *
 * @note It is necessary for the application developer to to check if  the
 * specified sensor has dedicated interrupt lines in this case the application
 * developer must provide a callback to process those interrupts.

 * The BSP_SensorNode_Disable() will revert back what the
 * BSP_SensorNode_Enable() has done and set the sensor and the MCU resource
 * into disabled state.

 * The BSP_SensorNode_Disconnect() will put the GPIO pins associated to the
 * sensors peripheral in analog mode to further reduce the consumption.
 */

#ifndef BCDS_BSP_IRSensorNode_H
#define BCDS_BSP_IRSensorNode_H

#include "BCDS_HAL.h"
/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_IRSENSOR_NODE

/**
 * type definition for the sensor callback
 * This callback is created by the application whenever the sensor has
 * dedicated interrupt lines. The application should then
 * pass the callback at the BSP_SensorNode_Enable() function.
 *
 * the first parameter is the interrupt identifier and the second parameter is
 * the edge resulted in the interrupt (falling edge or rising)
 * refer to the implementation for more details.
 */
typedef void (*BSP_IRSensor_InterruptCallback_T)(uint32_t, uint32_t);

/**
 * @brief Maps the GPIO pins to their desired function.
 * @details This function once called will map the GPIO pins related to the
 * sensor node to their desired functions.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_IRSensorNode_Connect(void);


/**
 * @brief Enables the control over the peripheral.
 * @details  A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate.This function once called will allow the
 * required sensor peripheral to receive, execute commands and transmit
 * process data.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_IRSensorNode_Enable(void);

/**
 * @brief Disables the control over the peripheral.
 * @details This function once called will disable control over the requested
 * sensor peripheral by executing the disabling procedure as prescribed in the
 * vendor datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_IRSensorNode_Disable(void);

/**
 * @brief Disconnects the sensor node.
 * @details This function disconnects the GPIO pins dedicated to the sensor
 * node and put them into low power consumption status.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_IRSensorNode_Disconnect(void);


/**
 * @brief Returns the handle of the communication interface used by the
 * peripheral in case I2C is used for the sensor node.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resources I2C, SPI, UART, etc.
 * @return HWHandle_T: Handle for the used MCU resource.
 */
HWHandle_T BSP_IRSensorNode_GetI2CHandle(void);

#endif /* BCDS_FEATURE_BSP_IRSENSOR_NODE */

#endif /* BCDS_BSP_IRSensorNode_H */

/**  @} */
