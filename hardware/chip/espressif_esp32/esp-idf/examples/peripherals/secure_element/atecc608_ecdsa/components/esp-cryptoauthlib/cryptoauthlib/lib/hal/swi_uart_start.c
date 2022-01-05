/**
 * \file
 *
 * \brief
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

#include <stdlib.h>
#include <stdio.h>
#include <peripheral_clk_config.h>
#include "swi_uart_start.h"
#include "basic/atca_helpers.h"

#define USART_BAUD_RATE(baud, sercom_freq)      (65536 - ((65536 * 16.0F * baud) / sercom_freq))

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/** \brief
   - this HAL implementation assumes you've included the START SERCOM UART libraries in your project, otherwise,
   the HAL layer will not compile because the START UART drivers are a dependency *
 */

/** \brief HAL implementation of SWI UART init.
 * \param[in] instance  instance
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS swi_uart_init(ATCASWIMaster_t *instance)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    instance->USART_SWI = USART_1;
    instance->sercom_core_freq = SWI_USART_SERCOM_CORE_FREQ;

    status |= usart_sync_set_mode(&(instance->USART_SWI), USART_MODE_ASYNCHRONOUS);
    // Set data size to 7
    status |= usart_sync_set_character_size(&(instance->USART_SWI), USART_CHARACTER_SIZE_7BITS);
    // Set parity to no parity
    status |= usart_sync_set_parity(&(instance->USART_SWI), USART_PARITY_NONE);
    // Set data byte to one stopbit
    status |= usart_sync_set_stopbits(&(instance->USART_SWI), USART_STOP_BITS_ONE);
    // Set baudrate to 230400
    status |= usart_sync_set_baud_rate(&(instance->USART_SWI), USART_BAUD_RATE(230400, instance->sercom_core_freq));
    // Enable SWI UART
    status |= usart_sync_enable(&(instance->USART_SWI));
    return status;
}

/** \brief HAL implementation of SWI UART deinit.
 * \param[in] instance  instance
 * \return ATCA_SUCCESS
 */
ATCA_STATUS swi_uart_deinit(ATCASWIMaster_t *instance)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    usart_sync_disable(&(instance->USART_SWI));

    return status;
}

/** \brief HAL implementation of SWI UART change baudrate.
 * \param[in] instance  instance
 * \param[in] baudrate (typically 230400 or 115200)
 */
void swi_uart_setbaud(ATCASWIMaster_t *instance, uint32_t baudrate)
{
    // Set baudrate for UART module
    usart_sync_set_baud_rate(&(instance->USART_SWI), USART_BAUD_RATE(baudrate, instance->sercom_core_freq));
}


/** \brief HAL implementation of SWI UART change mode.
 * \param[in] instance  instance
 * \param[in] mode (TRANSMIT_MODE or RECEIVE_MODE)
 */
void swi_uart_mode(ATCASWIMaster_t *instance, uint8_t mode)
{

    usart_sync_disable(&(instance->USART_SWI));

    if (mode == TRANSMIT_MODE)
    {
        // Set baudrate to 230400
        usart_sync_set_baud_rate(&(instance->USART_SWI), USART_BAUD_RATE(230400, instance->sercom_core_freq));
        // Disable Receiver
        hri_sercomusart_clear_CTRLB_RXEN_bit(instance->USART_SWI.device.hw);
        hri_sercomusart_set_CTRLB_TXEN_bit(instance->USART_SWI.device.hw);
    }
    else if (mode == RECEIVE_MODE)
    {
        // Set baudrate to 160000
        usart_sync_set_baud_rate(&(instance->USART_SWI), USART_BAUD_RATE(170000, instance->sercom_core_freq));
        // Enable Receiver
        hri_sercomusart_clear_CTRLB_TXEN_bit(instance->USART_SWI.device.hw);
        hri_sercomusart_set_CTRLB_RXEN_bit(instance->USART_SWI.device.hw);
    }
    usart_sync_enable(&(instance->USART_SWI));
    instance->bus_index &= 0x07;
}

/** \brief discover UART buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] swi_uart_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 */
void swi_uart_discover_buses(int swi_uart_buses[], int max_buses)
{
    /* if every SERCOM was a likely candidate bus, then would need to initialize the entire array to all SERCOM n numbers.
     * As an optimization and making discovery safer, make assumptions about bus-num / SERCOM map based on SAMD21 Xplained Pro board
     * If you were using a raw SAMD21 on your own board, you would supply your own bus numbers based on your particular hardware configuration.
     */
    swi_uart_buses[0] = 4;   // default samd21 for xplained dev board
}

/** \brief HAL implementation of SWI UART send byte over ASF.  This function send one byte over UART
 * \param[in] instance  instance
 * \param[in] data      number of byte to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS swi_uart_send_byte(ATCASWIMaster_t *instance, uint8_t data)
{
    uint8_t retries = 2;
    int32_t byte_sent = 0;

#ifdef DEBUG_PIN
    gpio_toggle_pin_level(PA20);
#endif
    // Send one byte over UART module
    while ((retries > 0) && (byte_sent < 1))
    {
        byte_sent = io_write(&(instance->USART_SWI.io), &data, 1);
        retries--;
    }
#ifdef DEBUG_PIN
    gpio_toggle_pin_level(PA20);
#endif
    if (byte_sent <= 0)
    {
        return ATCA_TIMEOUT;
    }
    else
    {
        while (!_usart_sync_is_transmit_done(&instance->USART_SWI.device))
        {
            ;
        }
        return ATCA_SUCCESS;
    }
}

/** \brief HAL implementation of SWI UART receive bytes over ASF.  This function receive one byte over UART
 * \param[in]    instance instance
 * \param[out] data     pointer to space to receive the data
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS swi_uart_receive_byte(ATCASWIMaster_t *instance, uint8_t *data)
{
    int32_t byte_sent = 0;
    uint8_t retries = 3;

#ifdef DEBUG_PIN
    gpio_toggle_pin_level(PA21);
#endif
    // Receive one byte over UART module
    while ((retries-- > 0) && (byte_sent < 1))
    {
        delay_us(30);
        if (usart_sync_is_rx_not_empty(&instance->USART_SWI))
        {
            byte_sent = io_read(&instance->USART_SWI.io, data, 1);
        }
    }
#ifdef DEBUG_PIN
    gpio_toggle_pin_level(PA21);
#endif
    if (byte_sent <= 0)
    {
        return ATCA_TIMEOUT;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}

/** @} */
