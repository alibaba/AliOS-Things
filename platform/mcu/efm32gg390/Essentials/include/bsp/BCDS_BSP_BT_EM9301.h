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
 *  @ingroup  BCDS_HAL_BSP_IF
 *
 *  @defgroup BCDS_HAL_BSP_BT_EM9301 Bluetooth_EM9301
 *
 *
 *  @{
 *  @brief Generic interface to the Bluetooth EM9301 BSP API
 *
 * @details A prior call to BSP_Board_initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the Bluetooth device.
 *
 * Once the Bluetooth is requested a call to BSP_BT_EM9301_Connect() function
 * is then required to map the internal MCU resources to their associated
 * function.
 *
 * The application needs get the handle for the MCU resource driver via
 * BSP_BT_EM9301_GetHandle() and to initialize the MCU resource driver by
 * calling the MCU_<RESOURCE>_Initialize() and providing it with the previously
 * acquired handle and an event callback function.
 *
 * Then a call to BSP_BT_EM9301_Enable() will execute the required sequence to
 * enable the Bluetooth peripheral and the MCU resources dedicated for it (UART,
 * SPI, I2C, etc.) and will associate the interrupt requests to their
 * appropriate service routines.
 *
 * The BSP_BT_EM9301_Disable() will revert back what the BSP_BT_EM9301_Enable()
 * has done and set the Bluetooth and the MCU resource into disabled state.
 *
 * The BSP_BT_EM9301_Disconnect() will put the GPIO pins associated to the
 * Bluetooth peripheral in analog mode to further reduce the consumption.
 *
 * In the datasheet of the device there was necessity to complement those
 * functions with other minimum service function.
 *
 * The BSP_BT_EM9301_Wakeup() wakes up the peripheral from sleep mode.
 *
 * The BSP_BT_EM9301_Reset() attempts to reset the peripheral if the commands fail
 * to execute.
 *
 *  @file
 */

#ifndef BCDS_BSP_BT_EM9301_H
#define BCDS_BSP_BT_EM9301_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_BT_EM9301

/**
 * @brief Maps the GPIO pins to their desired function.
 *
 * @details This function once called will map the GPIO pins to their desired
 * operating mode used for driving the EM Microelectronic(R) EM9301 Bluetooth
 * peripheral.
 * It is necessary to call this function before calling BSP_BT_EM9301_Enable().
 * @param control parameter of the connect function.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_BT_EM9301_Connect(void);

/**
 * @brief Enables the control over the peripheral
 *
 * @details A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate. This function once called will allow the
 * EM Microelectronic(R) EM9301 Bluetooth peripheral to receive, execute
 * commands and transmit process data. It will execute the sequence to startup
 * the peripheral as described in its datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_BT_EM9301_Enable(void);

/**
 * @brief Disables the control over the Bluetooth peripheral.
 * @details This function once called will disable control over the
 * EM Microelectronic(R) EM9301 Bluetooth peripheral as described in the
 * datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_BT_EM9301_Disable(void);

/**
 * @brief Disconnects the Bluetooth peripheral.
 *
 * @details This function disconnects the GPIOs dedicatded for the
 * EM Microelectronic(R) EM9301 Bluetooth peripheral. Effectively, it puts those
 * pins into low power consuption status.
 *
 */
Retcode_T BSP_BT_EM9301_Disconnect(void);

/**
 * @brief Resets the peripheral.
 * @details This function once called will reset the EM Microelectronic(R)
 * EM9301 Bluetooth peripheral as described in the vendors datasheet
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_BT_EM9301_Reset(void);

/**
 * @brief Drives the WU (Wake-Up) pin to high level
 * @details This function once called will drive to high the WU (Wake-Up) pin of
 * the EM Microelectronic(R) EM9301 Bluetooth peripheral.
 * It results in the Bluetooth peripheral to transit from sleep mode to standby
 * mode as described in the vendors datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_BT_EM9301_SetWUHigh(void);

/**
 * @brief Drives the WU (Wake-Up) pin to low level
 * @details This function once called will drive to low the WU (Wake-Up) pin of the
 * EM Microelectronic(R) EM9301 Bluetooth peripheral.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_BT_EM9301_SetWULow(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resources SPI, UART, etc.
 *
 * @retval HWHandle_T: Handle for the used MCU resource.
 */
HWHandle_T BSP_BT_EM9301_GetUARTHandle(void);

#endif /* BCDS_FEATURE_BSP_BT_EM9301 */

#endif /* BCDS_BSP_BT_EM9301_H */

/**  @} */

