/**
 * \file
 * \brief ATMEGA's ATCA Hardware abstraction layer for SWI interface over AT90USB1287 UART drivers.
 *
 * Prerequisite: add UART Polled support to application in Atmel Studio
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef SWI_UART_AT90USB1287_ASF_H
#define SWI_UART_AT90USB1287_ASF_H

#include <asf.h>
#include "cryptoauthlib.h"
#include "serial.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */


/** \brief
    - this HAL implementation assumes you've included the ASF UART libraries in your project, otherwise,
    the HAL layer will not compile because the ASF UART drivers are a dependency *
 */

#define MAX_SWI_BUSES    1  // AT90USB1287 has only 1 port

#define RECEIVE_MODE     0  // UART Receive mode, RX enabled
#define TRANSMIT_MODE    1  // UART Transmit mode, RX disabled
#define RX_DELAY        10  // Delay before responses come
#define TX_DELAY        90  // Delay before new flag is sent

/** \brief this is the hal_data for ATCA HAL for SWI UART
 */
typedef struct atcaSWImaster
{
    // struct usart_module for Atmel SWI interface
    usart_if usart_instance;
    int      ref_ct;
    // for conveniences during interface release phase
    int bus_index;
} ATCASWIMaster_t;


ATCA_STATUS swi_uart_init(ATCASWIMaster_t *instance);
ATCA_STATUS swi_uart_deinit(ATCASWIMaster_t *instance);
void swi_uart_setbaud(ATCASWIMaster_t *instance, uint32_t baudrate);
void swi_uart_mode(ATCASWIMaster_t *instance, uint8_t mode);
void swi_uart_discover_buses(int swi_uart_buses[], int max_buses);

ATCA_STATUS swi_uart_send_byte(ATCASWIMaster_t *instance, uint8_t data);
ATCA_STATUS swi_uart_receive_byte(ATCASWIMaster_t *instance, uint8_t *data);

/** @} */

#endif // SWI_UART_AT90USB1287_ASF_H
