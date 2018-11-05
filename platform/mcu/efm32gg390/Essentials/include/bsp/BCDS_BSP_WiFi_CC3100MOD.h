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
 *  @defgroup BCDS_HAL_BSP_WIFI_CC3100MOD WIFI_CC3100MOD
 *  @brief Generic interface to the WIFI CC3100MOD BSP API
 *
 *
 *  @ingroup  BCDS_HAL_BSP_IF
 *  @{
 *
 * A prior call to BSP_Board_Initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the WIFI device.
 *
 * Once the WIFI is requested a call to BSP_WIFI_CC3100MOD_Connect()
 * function is required to map the internal MCU resources to their associated
 * function.
 *
 * The application needs get the handle for the MCU resource driver via
 * BSP_WIFI_CC3100MOD_GetHandle() and to initialize the MCU resource driver by
 * calling the MCU_<RESOURCE>_Initialize() and providing it with the previously
 * acquired handle and an event callback function.
 *
 * Then a call to BSP_WIFI_CC3100MOD_Enable() will execute the required sequence
 * to enable the WIFI peripheral and the MCU resources dedicated for
 * it (UART, SPI, I2C, etc.) and will associate the interrupt requests to their
 * appropriate service routines.
 *
 * The BSP_WIFI_CC3100MOD_Disable() will revert back what the
 * BSP_WIFI_CC3100MOD_Enable() has done and set the WIFI and the MCU
 * resource into disabled state.
 *
 * The BSP_WIFI_CC3100MOD_Disconnect() will put the GPIO pins associated to the
 * WIFI peripheral in analog mode to further reduce the consumption.
 */

#ifndef BCDS_BSP_WiFi_CC3100MOD_H
#define BCDS_BSP_WiFi_CC3100MOD_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_WIFI_CC3100MOD

/**
 * Type definition for the wlan interrupt callback
 * The application should then pass the callback at the
 * BSP_WiFi_CC3100MOD_Enable() function.
 *
 * During call the BSP will this function whenever
 * a rising edge occurs in the module interrupt line
 * Refer to the implementation for more details.
 */
typedef void (*BSP_WiFi_CC3100MOD_InterruptCallback_T)(void);

/**
 * @brief Maps the GPIO pins to their desired function.
 * @details This function once called will map the GPIO pins to
 * their desired functions to drive the TI(R) CC3100MOD WIFI
 * peripheral. It is needed to call this function prior to the
 * BSP_WIFI_CC3100MOD_Enable().
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_Connect(void);


/**
 * @brief Enables the control over the peripheral.
 * @details  A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate.This function once called will allow the
 * TI(R) CC3100MOD WIFI peripheral to receive, execute commands and transmit
 * process data.
 * @param callback : function to be called whenever an interrupt occurs on the
 * line
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_Enable(BSP_WiFi_CC3100MOD_InterruptCallback_T callback);

/**
 * @brief Disables the control over the peripheral.
 * @details This function once called will disable control over the TI(R)
 * CC3100MOD WIFI peripheral by executing the disabling procedure as
 * prescribed in the vendor datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_Disable(void);

/**
 * @brief Disconnects the WIFI peripheral.
 * @details This function disconnects the GPIO pins dedicated to the
 * TI(R) CC3100MOD WIFI peripheral and put them into low power consumption
 * status.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_Disconnect(void);


/**
 * @brief Returns the SPI handle of the communication interfaces used by the
 * peripheral.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 * @return HWHandle_T: Handle for the used MCU resource.
 */
HWHandle_T BSP_WiFi_CC3100MOD_GetSPIHandle(void);


/**
 * @brief Returns the UART handle of the communication interfaces used by the
 * peripheral.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 * @return HWHandle_T: Handle for the used MCU resource.
 */
HWHandle_T BSP_WiFi_CC3100MOD_GetUartHandle(void);

/**
 * @brief Wakes the module from hibernate mode.
 * @details If the WIFI peripheral is hibernated this function will execute the
 * wake-up sequence with respect to the timing specified in the component
 * data-sheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_Wakeup(void);


/**
 * @brief Hibernates the WIFI module.
 * @details this function will execute the instruction sequence in order to put
 * the WIFI module into hibernated state.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_Hibernate(void);

/**
 * @brief Resets the WIFI module.
 * @details This function will execute the sequence to reset the CC3100MOD
 * device
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_Reset(void);

/**
 * @brief Sets the CS pin to Low i.e. device selected.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_SetCSLow(void);

/**
 * @brief Sets the CS pin to high i.e. device deselected.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_WiFi_CC3100MOD_SetCSHigh(void);

#endif /* BCDS_FEATURE_BSP_WIFI_CC3100MOD */

#endif /* BCDS_BSP_WiFi_CC3100MOD_H */

/**  @} */
