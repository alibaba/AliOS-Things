/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_SERIAL_H
#define YUNOS_BSP_SERIAL_H

#include <stdint.h>

/**
 * Init the uart module
 * @param[out]  count       the number of serial
 * @return      0, success; -1, fail
 */
int  yunos_bsp_uart_init(uint8_t * count);

/**
 * Configure the UART baud, bits, parity, fifos, etc. This \
   method is called the first time that the serial port is opened.
 * @param[in]   id          the id of serial
 * @param[in]   cb          the callback function
 * @param[in]   arg         the parameters of callback function
 * @return      0, success; -1, fail
 */
int  yunos_bsp_uart_setup(uint8_t id, int (*cb)(void *), void *arg);

/**
 * Set the serial line format and speed.
 * @param[in]   parity      0=none, 1=odd, 2=even
 * @param[in]   bits        Number of bits (7 or 8)
 * @param[in]   stopbits2   True: Configure with 2 stop bits instead of 1
 * @param[in]   baud        Configured baud
 * @return      void
 */
int yunos_bsp_uart_set_format(uint8_t id, uint32_t baud, uint8_t bits, uint8_t parity, bool stopbits2);

/**
 * Get the serial line format and speed.
 * @param[in]   parity      0=none, 1=odd, 2=even
 * @param[in]   bits        Number of bits (7 or 8)
 * @param[in]   stopbits2   True: Configure with 2 stop bits instead of 1
 * @param[in]   baud        Configured baud
 * @return      0 on success,-1 on failed
 */
int yunos_bsp_uart_get_format(uint8_t id, uint32_t * baud, uint8_t * bits, uint8_t * parity, bool * stopbits2);

/**
 * Disable the UART.  This method is called when the serial port is closed
 * @param[in]   id          the id of serial
 * @return      void
 */
void yunos_bsp_uart_shutdown(uint8_t id);

/**
 * Get a character from uart
 * @param[in]   id          the id of serial
 * @return      the character to get
 * @note        Called (usually) from the interrupt level to receive one
 *              character from the UART.  Error bits associated with the
 *              receipt are provided in the return 'status'.
 */
int yunos_bsp_uart_getc(uint8_t id);

/**
 * Call to enable or disable RX
 * @param[in]   id          the id of serial
 * @param[in]   enable      the mode of enable
 * @return      void
 */
void yunos_bsp_enable_rxint(uint8_t id, uint8_t enable);

/**
 * Return true if the receive register is not empty
 * @param[in]   id          the id of serial
 * @return      0, success; error code, fail
 */
uint8_t yunos_bsp_uart_rxavailable(uint8_t id);

/**
 * This method will send one byte on the UART
 * @param[in]   id          the id of serial
 * @param[in]   ch          the character to send
 * @return      void
 */
void yunos_bsp_uart_putc(uint8_t id, int ch);

/**
 * Call to enable or disable TX
 * @param[in]   id          the id of serial
 * @param[in]   enable      the mode to set
 * @return      void
 */
void yunos_bsp_enable_txint(uint8_t id, uint8_t enable);

/**
 * Return true if the transmit data register is empty
 * @param[in]   id          the id of serial
 * @return      0, success; error code, fail
 */
uint8_t yunos_bsp_uart_txready(uint8_t id);

/**
 * Set serial one wire mode or not.
 * @param[in]   id          the id of serial
 * @param[in]   is_onewire  the mode to set
 * @return      0, success;error code, fail
 */
int yunos_bsp_uart_set_onewire(uint8_t id, uint8_t is_onewire);

/**
 * Enable serial the direction pin .
 * @param[in]   pin_num     the number of pin
 * @param[in]   status      the mode to set, weather to enable
 * @return      0, success; error code, fail
 */
int yunos_bsp_uart_halfdup_enable(uint8_t pin_num, bool status);

/**
 * Choose the serial read and write to enable
 * @param[in]   id          the id of serial
 * @param[in]   pin_num     the number of pin
 * @param[in]   dir         the direction to be set
 * @return      void
 */
void yunos_bsp_uart_chsdir(uint8_t id, uint8_t pin_num, bool dir);

#endif /* YUNOS_BSP_SERIAL_H */
