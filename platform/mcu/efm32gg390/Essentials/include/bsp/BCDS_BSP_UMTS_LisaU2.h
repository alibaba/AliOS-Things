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
 *  @defgroup BCDS_HAL_BSP_UMTS_LISAU2 UMTS_LISAU2
 *  @brief Generic interface to the UMTS/HSPA peripheral UBLOX LISAU2
 *
 *
 *  @ingroup  BCDS_HAL_BSP_IF
 *  @{
 *
 * A prior call to BSP_Board_Initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the UMTS/HSPA device.
 * Once the UMTS is requested a call to BSP_UMTS_LisaU2_Connect() function is
 * then required to map the internal MCU resources to their associated function.
 *
 * The application needs get the handle for the MCU resource driver via
 * BSP_UMTS_LisaU2_GetHandle() and to initialize the MCU resource driver by
 * calling the MCU_<RESOURCE>_Initialize() and providing it with the previously
 * acquired handle and an event callback function pointer.
 *
 * Then a call to BSP_UMTS_LisaU2_Enable() will execute the required sequence to
 * enable the UMTS/HSPA peripheral and the MCU resources dedicated for IT (UART, SPI,
 * I2C, etc.) and will associate the interrupt requests to their appropriate
 * service routines.
 *
 * The BSP_UMTS_LisaU2_Disable() will revert back what the
 * BSP_UMTS_LisaU2_Enable() has done and set the UMTS and the MCU resource into
 * disabled state.
 *
 * The BSP_UMTS_LisaU2_Disconnect() will put the GPIO pins associated to the
 * UMTS/HSPA peripheral in analog mode to further reduce the consumption.
 *
 * In the datasheet of the device there was necessity to complement those
 * functions with other minimum service functions:
 *
 * The BSP_UMTS_LisaU2_Wakeup() wakes up the peripheral from Sleep mode.
 *
 * The BSP_UMTS_LisaU2_Sleep() puts the peripheral into sleep mode.
 *
 * The BSP_UMTS_LisaU2_Restart() Restarts the peripheral.
 *
 * The BSP_UMTS_LisaU2_Reset() attempts to reset the peripheral if the commands
 * fail to execute.
 *
 */

#ifndef BCDS_BSP_UMTS_LisaU2_H
#define BCDS_BSP_UMTS_LisaU2_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_UMTS_LISAU2

/**
 * @brief Maps the GPIO pins to their desired function.
 * @details This function once called will map the GPIO pins to
 * their desired functions to drive the UBLOX(R) LisaU2 UMTS/HSPA peripheral. It is
 * needed to call this function prior to the BSP_UMTS_LisaU2_Enable().
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 *
 */
 Retcode_T BSP_UMTS_LisaU2_Connect(void);

/**
 * @brief Enables the control over the peripheral
 * @details  A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate.This function once called will allow the
 * UBLOX(R) LisaU2 UMTS/HSPA peripheral to receive, execute commands and transmit
 * process data.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_UMTS_LisaU2_Enable(void);

/**
 * @brief Disables the control over the peripheral.
 * @details This function once called will disable control over the  UBLOX(R)
 * LisaU2 UMTS/HSPA peripheral by executing the Disabling Procedure as prescribed in the
 * vendors datasheets.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_UMTS_LisaU2_Disable(void);

/**
 * @brief Disconnects the UMTS/HSPA peripheral.
 * @details This function disconnects all the GPIOs dedicated for the
 * UBLOX(R) LisaU2 UMTS/HSPA peripheral and put them into low power consumption
 * status.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_UMTS_LisaU2_Disconnect(void);

/**
 * @brief Resets the peripheral.
 * @details This function once called will reset the  UBLOX(R) LisaU2 UMTS
 * peripheral by pulling up the reset pin to low level.

 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_UMTS_LisaU2_Reset(void);

/**
 * @brief Reads the state of the input CTS pin
 * @details The module hardware flow control output (CTS line) is set to the ON
 * state (low level) at UART initialization. If the hardware flow control is
 * enabled, as it is by default, the CTS line indicates when the UART interface
 * is enabled (data can be sent and received). The module drives the CTS line to
 * the ON state or to the OFF state when it is either able or not able to accept
 * data from the DTE over the UART
 *
 * @return Error (if module is not enabled) or Logic level of the input pin
 * (high or low)
 */
uint32_t BSP_UMTS_LisaU2_GetCTS(void);

/**
 * @brief sets the RTS pin to Low
 * @details
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_UMTS_LisaU2_SetRTSLow(void);

/**
 * @brief sets the RTS pin to Low
 * @details
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_UMTS_LisaU2_SetRTSHigh(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resources SPI, UART, etc.
 *
 * @return HWHandle_T: Handle for the used MCU resource or NULL in case not
 * initialized or not implemented.
 */
HWHandle_T BSP_UMTS_LisaU2_GetUARTHandle(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resources SPI, UART, etc.
 *
 * @return HWHandle_T: Handle for the used MCU resource or NULL in case not
 * initialized or not implemented.
 */
HWHandle_T BSP_UMTS_LisaU2_GetSPIHandle(void);

#endif /* BCDS_FEATURE_BSP_UMTS_LISAU2 */

#endif /* BCDS_BSP_UMTS_LisaU2_H */

/**  @} */
