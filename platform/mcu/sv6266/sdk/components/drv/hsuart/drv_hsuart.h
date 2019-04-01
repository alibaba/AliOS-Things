#ifndef _DRV_HSUART_H_
#define _DRV_HSUART_H_

#include "hal_hsuart.h"

typedef enum _HSUART_IE_E {
    HSUART_RX_DATA_READY_IE         = 0x0,  ///< Receiver Data Ready or Time out Interrupt Enable.
    HSUART_THR_EMPTY_IE             = 0x1,  ///< Transmitter Holding Register Empty Interrupt Enable.
    HSUART_RX_LINE_STATUS_IE        = 0x2,  ///< Receiver Line Status Interrupt Enable.
    HSUART_MODEM_STATUS_IE          = 0x3   ///< Modem Status Interrupt Enable.
} HSUART_IE_E;

/**
 * Registers an interrupt service routine to be called from the High Speed UART interrupt handler.
 *
 * If ISR is NULL, then the High Speed UART interrupt is disabled.
 *
 * @param   INT_ENABLE                  Select the type of High Speed UART interrupt enable.
 * @param   ISR                         A pointer to an ISR function that is called when a High Speed UART interrupt handler occurs.
 *                                      NULL to disable interrupt.
 */
void drv_hsuart_register_isr (HSUART_IE_E INT_ENABLE, HSUART_ISR ISR);

/**
 * Initialize the High Speed UART hardware.
 */
void drv_hsuart_init (void);

/**
 * Deinitialize the High Speed UART hardware.
 */
void drv_hsuart_deinit (void);

/**
 * The High Speed UART communications format to the specified settings.
 *
 * @param   BaudRate                    The baud rate of the High Speed UART.
 *                                      Contains at least the [1, 5000000] range.
 * @param   DataBits                    The number of data bits in each character.
 * @param   StopBits                    The number of stop bits per character.
 * @param   Parity                      That is computed or checked as each character is transmitted or received.
 *
 * @retval   -3                         The baud rate out of range.
 * @retval  -17                         The clock source is no mapping.
 * @retval    0                         The operation completed successfully.
 */
int32_t drv_hsuart_set_format (int32_t BaudRate, HSUART_WLS_E DataBits, HSUART_STB_E StopBits, HSUART_PARITY_E Parity);

/**
 * The High Speed UART FIFOs to the specified settings.
 *
 * @param   INT_RX_FIFO_TRIGGER_LEVEL   In FIFO mode an interrupt will be generated (if enabled) when the number of words
 *                                      in the receiver's FIFO is equal or greater than this trigger level.
 */
void drv_hsuart_set_fifo (HSUART_INT_RX_FIFO_TRIG_LV_E INT_RX_FIFO_TRIGGER_LEVEL);

/**
 * The High Speed UART auto flow control to the specified settings.
 *
 * @param   AutoRtsThresholdLow         Sets the auto RTS active when the number of words in the receiver's FIFO is equal or less than this threshold.
 *                                      Contains at least the [0, 31] range.
 * @param   AutoRtsThresholdHigh        Sets the auto RTS inactive when the number of words in the receiver's FIFO is equal or greater than this threshold.
 *                                      Contains at least the [0, 31] range.
 */
void drv_hsuart_set_hardware_flow_control (uint8_t AutoRtsThresholdLow, uint8_t AutoRtsThresholdHigh);

/**
 * Write data from buffer to High Speed UART transmitter FIFOs.
 *
 * Writes NumberOfBytes data bytes from Buffer to the High Speed UART transmitter FIFOs.
 * The number of bytes actually written to the High Speed UART transmitter FIFOs is returned.
 *
 * @param   Buffer                      Pointer to the data buffer to be written.
 * @param   NumberOfBytes               Number of bytes to written to the High Speed UART transmitter FIFOs.
 * @param   BlockingMode                Set the High Speed UART HAL driver to operate in blocking/non-blocking mode.
 *
 * @retval  -9                          Buffer is NULL.
 * @retval  >0                          The number of bytes written to the High Speed UART transmitter FIFOs.
 */
int32_t drv_hsuart_write_fifo (uint8_t const *Buffer, int32_t NumberOfBytes, HSUART_BLOCKING_MODE_E BlockingMode);

/**
 * Reads data from a High Speed UART receiver FIFOs into a buffer.
 *
 * Reads NumberOfBytes data bytes from the High Speed UART receiver FIFOs.
 * The number of bytes actually read from the High Speed UART receiver FIFOs is returned.
 *
 * @param   Buffer                      Pointer to the data buffer to store the data read from the High Speed UART receiver FIFOs.
 * @param   NumberOfBytes               Number of bytes to read from the High Speed UART receiver FIFOs.
 * @param   BlockingMode                Set the High Speed UART HAL driver to operate in blocking/non-blocking mode.
 *
 * @retval  -9                          Buffer is NULL.
 * @retval  >0                          The number of bytes read from the High Speed UART receiver FIFOs.
 */
int32_t drv_hsuart_read_fifo (uint8_t *Buffer, int32_t NumberOfBytes, HSUART_BLOCKING_MODE_E BlockingMode);

/**
 * The High Speed UART software reset.
 *
 * Write 1 to trigger a pulse to reset the whole High Speed UART.
 */
void drv_hsuart_sw_rst (void);

/**
 * This service reads the High Speed UART line status value.
 *
 * @return  Returns the High Speed UART line status value.
 */
uint32_t drv_hsuart_get_line_status (void);

/**
 * This service checks the High Speed UART receiver is available or not.
 *
 * @retval  1                           The High Speed UART receiver is available.
 * @retval  0                           The High Speed UART receiver is not available.
 */
uint32_t drv_hsuart_is_receiver_available (void);

/**
 * This service checks the High Speed UART transmitter is idle or not.
 *
 * @retval  1                           The High Speed UART transmitter is idle.
 * @retval  0                           The High Speed UART transmitter is not idle.
 */
uint32_t drv_hsuart_is_transmitter_idle (void);

/**
 * This service reads the High Speed UART modem status value.
 *
 * @return  Returns the High Speed UART modem status value.
 */
uint32_t drv_hsuart_get_modem_status (void);

#endif  // #ifndef _DRV_HSUART_H_
