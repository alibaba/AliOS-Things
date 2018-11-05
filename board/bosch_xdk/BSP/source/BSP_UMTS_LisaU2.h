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
 *  @brief Handling routines provided for Bluetooth module related interrupts
 *
 */

#ifndef UMTS_LisaU2_H
#define UMTS_LisaU2_H

#include "BCDS_BSP_UMTS_LisaU2.h"

#if BCDS_FEATURE_BSP_UMTS_LISAU2

#define UMTS_RESET_PROCESSING_DELAY_MS                                          UINT32_C(50)/**< Macro to represent the required delay during reset */
#define UMTS_RESET_PROCESSING_OS_DELAY_MS                                       UINT32_C(150)/**< Macro to represent the required delay during reset */
#define UMTS_POWER_OFF_PROCESSING_DELAY_MS                                      UINT32_C(1000)/**< Macro to represent the required delay  during Power ON*/
#define UMTS_POWER_OFF_PROCESSING_OS_DELAY_MS                                   UINT32_C(1100)/**< Macro to represent the required delay  during Power OFF*/
#define UMTS_POWER_ON_PIN                                                       BSP_EXTENSIONPORT_GPIO_PA1/**< Macro to represent the Power pin*/
#define UMTS_RESET_PIN                                                          BSP_EXTENSIONPORT_GPIO_PE2/**< Macro to represent the Reset pin*/
#define UMTS_UART_BAUDRATE                                                      115200 /**< Macro to represent UART baudrate */
#define UMTS_RTS_PIN                                                            BSP_EXTENSIONPORT_GPIO_PB2/**< Macro to represent the RTS pin*/
#define UMTS_CTS_PIN                                                            BSP_EXTENSIONPORT_GPIO_PF6/**< Macro to represent the CTS pin*/

#endif /* BCDS_FEATURE_BSP_UMTS_LISAU2 */

#endif /* UMTS_LisaU2_H */


