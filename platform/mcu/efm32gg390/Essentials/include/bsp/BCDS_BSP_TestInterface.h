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
 *  @{
 *
 * A prior call to BSP_Board_Initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the test interface.
 * Once the Test is requested a call to BSP_TestInterface_Connect() function is
 * required to map the internal MCU resources to their associated function.
 *
 * The application needs get the handle for the MCU resource driver via
 * BSP_TestInterface_GetHandle() and to initialize the MCU resource driver by
 * calling the MCU_<RESOURCE>_Initialize() and providing it with the previously
 * acquired handle and an event callback function.
 *
 * Then a call to BSP_TestInterface_Enable() will execute the required sequence
 * to enable the test interface and will associate the interrupt requests to their
 * appropriate service routines.
 *
 * The BSP_TestInterface_Disable() will revert back what the
 * BSP_TestInterface_Enable() has done and set the Test and the MCU resource into
 * disabled state.
 *
 * The BSP_TestInterface_Disconnect() will put the GPIO pins associated to the
 * Test peripheral in analog mode to further reduce the consumption.
 */

#ifndef BCDS_BSP_TestInterface_H
#define BCDS_BSP_TestInterface_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_TEST_INTERFACE

/**
 * @brief Maps the GPIO pins to their desired function.
 * @details This function once called will map the GPIO pins to
 * their desired functions to drive the  Test peripheral. It is
 * needed to call this function prior to the BSP_TestInterface_Enable().
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_TestInterface_Connect(void);


/**
 * @brief Enables the control over the peripheral.
 * @details  A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate.This function once called will allow the
 *  Test peripheral to receive, execute commands and transmit
 * process data.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_TestInterface_Enable(void);

/**
 * @brief Disables the control over the peripheral.
 * @details This function once called will disable control over the test
 * interface.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_TestInterface_Disable(void);

/**
 * @brief Disconnects the Test peripheral.
 * @details This function disconnects all GPIO pins dedicated to the
 *  test interface and put them into low power consumption status.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_TestInterface_Disconnect(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral in case LEUART is used.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 *
 * @return HWHandle_T: Handle for the used MCU resource.
 */

HWHandle_T BSP_TestInterface_GetLEUARTHandle(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral in case UART is used.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 *
 * @return HWHandle_T: Handle for the used MCU resource.
 */

HWHandle_T BSP_TestInterface_GetUARTHandle(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral in case USB is used.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 *
 * @return HWHandle_T: Handle for the used MCU resource.
 */

HWHandle_T BSP_TestInterface_GetUSBHandle(void);

/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral in case SPI is used.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 *
 * @return HWHandle_T: Handle for the used MCU resource.
 */
HWHandle_T BSP_TestInterface_GetSPIHandle(void);

#endif /* BCDS_FEATURE_BSP_TEST_INTERFACE */

#endif /* BCDS_BSP_TestInterface_H */

/**  @} */
