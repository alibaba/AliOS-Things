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
 *  @defgroup BCDS_HAL_BSP_LORA_RN2xx3 LoRa RN2483/RN2903
 *  @brief Generic interface to the LPWAN (LoRa) RN2483/RN2903 BSP API
 *
 *
 *  @ingroup  BCDS_HAL_BSP_IF
 *  @{
 *
 * A prior call to BSP_Board_Initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the LPWAN (LoRa) device.
 *
 * Once the LPWAN (LoRa) is requested a call to BSP_LoRa_RN2xx3_Connect()
 * function is required to map the internal MCU resources to their associated
 * function.
 *
 * The application needs get the handle for the MCU resource driver via
 * BSP_LoRa_RN2xx3_GetUartHandle() and to initialize the MCU resource driver by
 * calling the MCU_<RESOURCE>_Initialize() and providing it with the previously
 * acquired handle and an event callback function.
 *
 * Then a call to BSP_LoRa_RN2xx3_Enable() will execute the required sequence
 * to enable the LoRa module and the MCU resources dedicated for
 * it (UART, SPI, I2C, etc.) and will associate the interrupt requests to their
 * appropriate service routines.
 *
 * The BSP_LoRa_RN2xx3_Disable() will revert back what the
 * BSP_LoRa_RN2xx3_Enable() has done and set the LPWAN (LoRa) and the MCU
 * resource into disabled state.
 *
 * The BSP_LoRa_RN2xx3_Disconnect() will put the GPIO pins associated to the
 * LoRa module in analog mode to further reduce the consumption.
 */

#ifndef BCDS_BSP_LoRa_RN2xx3_H
#define BCDS_BSP_LoRa_RN2xx3_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_LORA_RN2XX3

/**
 * enumeration representing return codes from the LoRa module BSP interface
 */
enum BSP_LoRa_RN2xx3_Retcode_E
{
    RETCODE_BSP_LORA_SERIAL_NOT_INITIALIZED = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_BSP_LORA_DISABLED,
};

/**
 * @brief Maps the GPIO pins to their desired function.
 * @details This function once called will map the GPIO pins to
 * their desired functions to drive the Microchip(R) RN2483/RN2903 LPWAN (LoRa)
 * peripheral. It is needed to call this function prior to the
 * BSP_LoRa_RN2xx3_Enable().
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LoRa_RN2xx3_Connect(void);


/**
 * @brief Enables the control over the peripheral.
 * @details  A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate.This function once called will allow the
 * Microchip(R) RN2483/RN2903 LPWAN (LoRa) peripheral to receive, execute commands and transmit
 * process data.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LoRa_RN2xx3_Enable(void);

/**
 * @brief Disables the control over the peripheral.
 * @details This function once called will disable control over the Microchip(R)
 * RN2483/RN2903 LPWAN (LoRa) peripheral by executing the disabling procedure as
 * prescribed in the vendor datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LoRa_RN2xx3_Disable(void);

/**
 * @brief Disconnects the LPWAN (LoRa) peripheral.
 * @details This function disconnects the GPIO pins dedicatded to the
 * Microchip(R) RN2483/RN2903 LPWAN (LoRa) peripheral and put them into low power consumption
 * status.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LoRa_RN2xx3_Disconnect(void);

/**
 * @brief Returns the UART handle of the communication interface used by the
 * peripheral.
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 * @return HWHandle_T: Handle for the used MCU resource.
 */
HWHandle_T BSP_LoRa_RN2xx3_GetUartHandle(void);

/**
 * @brief Sets the reset pin of the module to the desired state Low or High
 *  @param 0 for Low
 *         1 for High
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LoRa_RN2xx3_SetResetPin(uint32_t state);

/**
 * @brief Changes the baudrate of the communication interface
 * @param baudrate desired baudrate
 *          -if 0 will keep the current baudrate and send a break command with
 * synchronization frame could be used to wakeup the module from sleep according
 * to datasheet.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LoRa_RN2xx3_ChangeBaudrate(uint32_t baudrate);

/**
 * @brief Sends wake-up signal to the LoRa module
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LoRa_RN2xx3_WakeUp(void);

#endif /* BCDS_FEATURE_BSP_LORA_RN2XX3 */

#endif /* BCDS_BSP_LoRa_RN2xx3_H */

/**  @} */
