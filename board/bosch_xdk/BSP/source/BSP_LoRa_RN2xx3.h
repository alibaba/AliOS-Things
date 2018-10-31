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
 *  @brief Handling routines provided for LoRa module related interrupts
 *
 */

#ifndef LoRa_RN2xx3_H
#define LoRa_RN2xx3_H

#include "BCDS_BSP_LoRa_RN2xx3.h"

#if BCDS_FEATURE_BSP_LORA_RN2XX3

/**
 * @ingroup BSP_BOARDEXTSETTINGS
 *
 * @defgroup LORA_SETTINGS LORA_RN2XX3 extension board pins
 * @{
 *
 * @brief Settings used if the LORA_RN2XX3 Extension module connected to XDK
 *
 * @warning Please do not change these settings, since these are specific to hardware
 */

#define LORA_STARTUP_DELAY_MS                                                   UINT32_C(100)
#define LORA_WAKEUP_DELAY_MS                                                    UINT32_C(1)
#define LORA_RESET_PROCESSING_DELAY_MS                                          UINT32_C(50)
#define LORA_RESET_PROCESSING_OS_DELAY_MS                                       UINT32_C(150)
#define LORA_POWER_OFF_PROCESSING_DELAY_MS                                      UINT32_C(1000)
#define LORA_POWER_OFF_PROCESSING_OS_DELAY_MS                                   UINT32_C(1100)
#define LORA_RESET_PULLUP_DELAY_MS                                              UINT32_C(50)
#define LORA_TRANSMIT_DISABLE_TIMEOUT_MS                                        UINT32_C(5)
#define LORA_BAUDRATE                                                           UINT32_C(9600)
#define LORA_SERIAL_PORT                                                        UART1
#define LORA_GPIO_TX_PORT                                                       (EXTENSION_PORT_GPIO_PB9)
#define LORA_GPIO_TX_PIN                                                        (EXTENSION_PIN_GPIO_PB9)

#define LORA_BREAK_BAUDRATE                                                     UINT32_C(300)
#define LORA_BREAK_SYNC_CHAR                                                    UINT8_C(0x55)

#define LORA_RTS_PIN                                                            BSP_EXTENSIONPORT_GPIO_PB2
#define LORA_CTS_PIN                                                            BSP_EXTENSIONPORT_GPIO_PF6
#define LORA_RESET_PIN                                                          BSP_EXTENSIONPORT_GPIO_PA0
/**@}*/

#endif /* BCDS_FEATURE_BSP_LoRa_RN2xx3 */

#endif /* LoRa_RN2xx3_H */



