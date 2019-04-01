#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#include <stdint.h>
#include <stddef.h>

#define UART_FIFO_LEVEL (16)                ///< The UART has a 16-byte FIFO (First In, First Out).

typedef enum _UART_IE_E {
    UART_DATA_RDY_IE                = 0x0,  ///< Receiver Data Ready or Time out Interrupt Enable.
    UART_RX_LINESTS_IE              = 0x2,  ///< Receiver Line Status Interrupt Enable.
    UART_MDM_STS_IE                 = 0x3,  ///< Modem Status Interrupt Enable.
    UART_TX_THRL_IE                 = 0x5   ///< Tx FIFO Low Threshold Interrupt Enable.
} UART_IE_E;

typedef enum _UART_INT_RXFIFO_TRGLVL_E {
    UART_INT_RXFIFO_TRGLVL_1        = 0x0,  ///< Rx FIFO has  1 charcter trigger interrupt
    UART_INT_RXFIFO_TRGLVL_4        = 0x1,  ///< Rx FIFO has  4 charcter trigger interrupt
    UART_INT_RXFIFO_TRGLVL_8        = 0x2,  ///< Rx FIFO has  8 charcter trigger interrupt
    UART_INT_RXFIFO_TRGLVL_14       = 0x3   ///< Rx FIFO has 14 charcter trigger interrupt
} UART_INT_RXFIFO_TRGLVL_E;

typedef enum _UART_WORD_LEN_E {
    UART_WORD_LEN_5                 = 0x0,  ///< Character's Word Length 5 bits.
    UART_WORD_LEN_6                 = 0x1,  ///< Character's Word Length 6 bits.
    UART_WORD_LEN_7                 = 0x2,  ///< Character's Word Length 7 bits.
    UART_WORD_LEN_8                 = 0x3   ///< Character's Word Length 8 bits.
} UART_WORD_LEN_E;

typedef enum _UART_STOP_BIT_E {
    UART_STOP_BIT_1                 = 0x0,  ///< One stop bit will be transmitted.
    UART_STOP_BIT_2                 = 0x1   ///< Two stop bits (1.5 with 5-bit data) will be transmitted.
} UART_STOP_BIT_E;

typedef enum _UART_PARITY_E {
    UART_PARITY_DISABLE             = 0x0,  ///< No parity bit is transmitted nor expected.
    UART_PARITY_ODD                 = 0x1,  ///< The number of bits including the parity bit must be odd.
    UART_PARITY_EVEN                = 0x3,  ///< The number of bits including the parity bit must be even.
    UART_PARITY_FORCED1             = 0x5,  ///< The parity bit is sent as/checked to be 1.
    UART_PARITY_FORCED0             = 0x7   ///< The parity bit is sent as/checked to be 0.
} UART_PARITY_E;

typedef enum _UART_BLOCKING_MODE_E {
    UART_NON_BLOCKING               = 0x0,  ///< Set the UART HAL driver to operate in non-blocking mode.
    UART_BLOCKING                   = 0x1   ///< Set the UART HAL driver to operate in blocking mode.
} UART_BLOCKING_MODE_E;

typedef union _UART_LSR {
    //
    // Individual bit fields
    //
    struct {
        uint32_t DR                 : 1;    ///< This is the Data Ready bit.
        uint32_t OE                 : 1;    ///< This is the Overrun Error flag.
        uint32_t PE                 : 1;    ///< This is the Parity Error flag.
        uint32_t FE                 : 1;    ///< This is the Framing Error flag.
        uint32_t BI                 : 1;    ///< This is the Break Interrupt indicator.
        uint32_t THRE               : 1;    ///< This is the Transmitter Holding Register Empty flag.
        uint32_t TSRE               : 1;    ///< This is the Transmitter Shift Register Empty flag.
        uint32_t FIFOE              : 1;    ///< This is the receiver FIFO data error bit.
        uint32_t Reserved           : 24;   ///< Reserved.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} UART_LSR;

/**
 * Prototype for the UART interrupt service routine.
 */
typedef
void
(*UART_ISR) (
    void
    );

void
hal_uart_register_isr (
    UART_IE_E                       INT_ENABLE,
    UART_ISR                        ISR
    );

void
hal_uart_init (
    void
    );

void
hal_uart_deinit (
    void
    );

int32_t
hal_uart_set_format (
    int32_t                         BaudRate,
    UART_WORD_LEN_E                 DataBits,
    UART_STOP_BIT_E                 StopBits,
    UART_PARITY_E                   Parity
    );

void
hal_uart_set_fifo (
    UART_INT_RXFIFO_TRGLVL_E        INT_RXFIFO_TRGLVL,
    uint32_t                        INT_TTHR_L,
    uint32_t                        INT_TTHR_H
    );

void
hal_uart_set_loop_back (
    void
    );

uint32_t
hal_uart_direct_read_byte (
    void
    );

void
hal_uart_direct_write_byte (
    uint32_t    data
    );

int32_t
hal_uart_write_fifo (
    uint8_t const                   *Buffer,
    int32_t                         NumberOfBytes,
    UART_BLOCKING_MODE_E            BlockingMode
    );

int32_t
hal_uart_read_fifo (
    uint8_t                         *Buffer,
    int32_t                         NumberOfBytes,
    UART_BLOCKING_MODE_E            BlockingMode
    );

void
hal_uart_sw_rst (
    void
    );

uint32_t
hal_uart_get_line_status (
    void
    );

#endif  // #ifndef _HAL_UART_H_
