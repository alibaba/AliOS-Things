#ifndef _DRV_UART_H_
#define _DRV_UART_H_

#include "hal_uart.h"

/**
 * Registers an interrupt service routine to be called from the UART interrupt handler.
 *
 * If ISR is NULL, then the UART interrupt is disabled.
 *
 * @param   INT_ENABLE                  Select the type of UART interrupt enable.
 * @param   ISR                         A pointer to an ISR function that is called when a UART interrupt handler occurs.
 *                                      NULL to disable interrupt.
 */
void
drv_uart_register_isr (
    UART_IE_E                       INT_ENABLE,
    UART_ISR                        ISR
    );

/**
 * Initialize the UART hardware.
 */
void
drv_uart_init (
    void
    );

/**
 * Deinitialize the UART hardware.
 */
void
drv_uart_deinit (
    void
    );

/**
 * The UART communications format to the specified settings.
 *
 * @param   BaudRate                    The baud rate of the UART.
 *                                      Contains at least the [1, 921600] range.
 * @param   DataBits                    The number of data bits in each character.
 * @param   StopBits                    The number of stop bits per character.
 * @param   Parity                      That is computed or checked as each character is transmitted or received.
 *
 * @retval   -3                         The baud rate out of range.
 * @retval  -17                         The clock source is no mapping.
 * @retval    0                         The operation completed successfully.
 */
int32_t
drv_uart_set_format (
    int32_t                         BaudRate,
    UART_WORD_LEN_E                 DataBits,
    UART_STOP_BIT_E                 StopBits,
    UART_PARITY_E                   Parity
    );

/**
 * The UART FIFOs to the specified settings.
 *
 * @param   INT_RXFIFO_TRIGGER_LEVEL    In FIFO mode an interrupt will be generated (if enabled) when the number of words
 *                                      in the receiver's FIFO is equal or greater than this trigger level.
 * @param   INT_TXFIFO_THRESHOLD_LOW    In FIFO mode an interrupt will be generated (if enabled) when the number of words
 *                                      in the transmitter's FIFO is equal or less than this threshold.
 *                                      Contains at least the [0, 15] range.
 */
void
drv_uart_set_fifo (
    UART_INT_RXFIFO_TRGLVL_E        INT_RXFIFO_TRIGGER_LEVEL,
    uint8_t                         INT_TXFIFO_THRESHOLD_LOW
    );

/**
 * Write data from buffer to UART transmitter FIFOs.
 *
 * Writes NumberOfBytes data bytes from Buffer to the UART transmitter FIFOs.
 * The number of bytes actually written to the UART transmitter FIFOs is returned.
 *
 * @param   Buffer                      Pointer to the data buffer to be written.
 * @param   NumberOfBytes               Number of bytes to written to the UART transmitter FIFOs.
 * @param   BlockingMode                Set the UART HAL driver to operate in blocking/non-blocking mode.
 *
 * @retval  -9                          Buffer is NULL.
 * @retval  >0                          The number of bytes written to the UART transmitter FIFOs.
 */
int32_t
drv_uart_write_fifo (
    uint8_t const                   *Buffer,
    int32_t                         NumberOfBytes,
    UART_BLOCKING_MODE_E            BlockingMode
    );

/**
 * Reads data from a UART receiver FIFOs into a buffer.
 *
 * Reads NumberOfBytes data bytes from the UART receiver FIFOs.
 * The number of bytes actually read from the UART receiver FIFOs is returned.
 *
 * @param   Buffer                      Pointer to the data buffer to store the data read from the UART receiver FIFOs.
 * @param   NumberOfBytes               Number of bytes to read from the UART receiver FIFOs.
 * @param   BlockingMode                Set the UART HAL driver to operate in blocking/non-blocking mode.
 *
 * @retval  -9                          Buffer is NULL.
 * @retval  >0                          The number of bytes read from the UART receiver FIFOs.
 */
int32_t
drv_uart_read_fifo (
    uint8_t                         *Buffer,
    int32_t                         NumberOfBytes,
    UART_BLOCKING_MODE_E            BlockingMode
    );

/**
 * The UART software reset.
 *
 * Write 1 to trigger a pulse to reset the whole UART.
 */
void
drv_uart_sw_rst (
    void
    );

/**
 * This service reads the UART line status value.
 *
 * @return  Returns the UART line status value.
 */
uint32_t
drv_uart_get_line_status (
    void
    );

/**
 * This service checks the UART receiver is available or not.
 *
 * @retval  1                           The UART receiver is available.
 * @retval  0                           The UART receiver is not available.
 */
uint32_t
drv_uart_is_receiver_available (
    void
    );

/**
 * This service checks the UART transmitter is idle or not.
 *
 * @retval  1                           The UART transmitter is idle.
 * @retval  0                           The UART transmitter is not idle.
 */
uint32_t
drv_uart_is_transmitter_idle (
    void
    );

#endif  // #ifndef _DRV_UART_H_
