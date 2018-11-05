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
 *  @defgroup BCDS_HAL_BSP_SDCARD SDCARD
 *  @brief Generic interface to the Secure Digital SD Card over SPI
 *
 *
 *  @ingroup  BCDS_HAL_BSP_IF
 *  @{
 *
 * A prior call to BSP_Board_Initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the SD device.
 *
 * Once the SD is requested a call to BSP_SDCard_Connect()
 * function is required to map the internal MCU resources to their associated
 * function.
 *
 * The application needs get the handle for the MCU resource driver via
 * BSP_SDCard_GetHandle() and to initialize the MCU resource driver by
 * calling the MCU_<RESOURCE>_Initialize() and providing it with the previously
 * acquired handle and an event callback function.
 *
 * Then a call to BSP_SDCard_Enable() will execute the required sequence
 * to enable the SD peripheral and the MCU resources dedicated for
 * it (SPI) and will associate the interrupt requests to their
 * appropriate service routines.
 *
 * The BSP_SDCard_Disable() will revert back what the
 * BSP_SDCard_Enable() has done and set the SD and the MCU
 * resource into disabled state.
 *
 * The BSP_SDCard_Disconnect() will put the GPIO pins associated to the
 * SD peripheral in analog mode to further reduce the consumption.
 */

#ifndef BCDS_BSP_SD_Card_H
#define BCDS_BSP_SD_Card_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_SD_CARD

/**
 * Type definition for the callback to be triggered upon detection of the SD card
 * @param event reported by the BSP upon SD-card detection event.
 */
typedef void (*BSP_SDCardAutoDetectCallback_T)(uint32_t);


/**
 * @brief Maps the GPIO pins to their desired function.
 * @details This function once called will map the GPIO pins to their desired
 * functions to drive the SD memory peripheral. It is needed to call this
 * function prior to the BSP_SDCard_Enable().
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_SDCard_Connect(void);


/**
 * @brief Enables the control over the peripheral.
 * @details  A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate.This function once called will allow the
 * SD memory peripheral to receive, execute commands and transmit
 * process data.
 *
 * @param autoDetectCallback: In case auto detection is implemented  this
 * function will called upon SD-card detection event. The event could be:
 * 0 if error, 1 if card inserted, 2 if card ejected.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_SDCard_Enable(BSP_SDCardAutoDetectCallback_T autoDetectCallback);

/**
 * @brief Disables the control over the peripheral.
 * @details This function once called will disable control over the
 * SD memory peripheral by executing the disabling procedure as
 * described in the vendor datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_SDCard_Disable(void);

/**
 * @brief Disconnects the SD peripheral.
 * @details This function disconnects the GPIO pins dedicated to the
 * SD memory peripheral and put them into low power consumption state.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_SDCard_Disconnect(void);


/**
 * @brief Returns the handle of the communication interfaces used by the
 * peripheral.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resources I2C, SPI, UART, etc.
 * @return HWHandle_T: Handle for the used MCU resource.
 */
HWHandle_T BSP_SDCard_GetSPIHandle(void);


/**
 * @brief Sets the SD card chip select pin to logic level high.
 * @details If the SD peripheral is enabled (i.e BSP_SDCard_Enable()
 * executed successfully) this function allow to set to high the CS
 * (chip select) pin.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_SDCard_SetCSHigh(void);


/**
 * @brief Sets the SD chip select pin to logic level low.
 * @details If the SD peripheral is enabled (i.e BSP_SDCard_Enable()
 * executed successfully) this function allow to set to low the CS
 * (chip select) pin.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_SDCard_SetCSLow(void);

#endif /* BCDS_FEATURE_BSP_SDCARD */

#endif /* BCDS_BSP_SD_Card_H */

/**  @} */
