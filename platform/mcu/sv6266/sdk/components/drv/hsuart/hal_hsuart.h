#ifndef _HAL_HSUART_H_
#define _HAL_HSUART_H_

#include <stdint.h>
#include <stddef.h>

#define HSUART_FIFO_LEVEL (32)              ///< The HSUART has a 32-byte FIFO (First In, First Out).

typedef enum _HSUART_INTERRUPT_ENABLE_E {
    HSUART_INTERRUPT_ENABRXBUFF     = 0x0,  ///< Receiver Data Ready or Time out Interrupt Enable.
    HSUART_INTERRUPT_ENABTXBUFF     = 0x1,  ///< Transmitter Holding Register Empty Interrupt Enable.
    HSUART_INTERRUPT_ENABLNSTAT     = 0x2,  ///< Receiver Line Status Interrupt Enable.
    HSUART_INTERRUPT_ENABMDSTAT     = 0x3   ///< Modem Status Interrupt Enable.
} HSUART_INTERRUPT_ENABLE_E;

typedef enum _HSUART_INT_RX_FIFO_TRIG_LV_E {
    HSUART_INT_RX_FIFO_TRIG_LV_1    = 0x0,  ///< Rx FIFO has  1 charcter trigger interrupt
    HSUART_INT_RX_FIFO_TRIG_LV_8    = 0x1,  ///< Rx FIFO has  8 charcter trigger interrupt
    HSUART_INT_RX_FIFO_TRIG_LV_16   = 0x2,  ///< Rx FIFO has 16 charcter trigger interrupt
    HSUART_INT_RX_FIFO_TRIG_LV_28   = 0x3   ///< Rx FIFO has 28 charcter trigger interrupt
} HSUART_INT_RX_FIFO_TRIG_LV_E;

typedef enum _HSUART_WLS_E {
    HSUART_WLS_5                    = 0x0,  ///< Character's Word Length 5 bits.
    HSUART_WLS_6                    = 0x1,  ///< Character's Word Length 6 bits.
    HSUART_WLS_7                    = 0x2,  ///< Character's Word Length 7 bits.
    HSUART_WLS_8                    = 0x3   ///< Character's Word Length 8 bits.
} HSUART_WLS_E;

typedef enum _HSUART_STB_E {
    HSUART_STB_1                    = 0x0,  ///< One stop bit will be transmitted.
    HSUART_STB_2                    = 0x1   ///< Two stop bits (1.5 with 5-bit data) will be transmitted.
} HSUART_STB_E;

typedef enum _HSUART_PARITY_E {
    HSUART_PARITY_DISABLE           = 0x0,  ///< No parity bit is transmitted nor expected.
    HSUART_PARITY_ODD               = 0x1,  ///< The number of bits including the parity bit must be odd.
    HSUART_PARITY_EVEN              = 0x3,  ///< The number of bits including the parity bit must be even.
    HSUART_PARITY_FORCED1           = 0x5,  ///< The parity bit is sent as/checked to be 1.
    HSUART_PARITY_FORCED0           = 0x7   ///< The parity bit is sent as/checked to be 0.
} HSUART_PARITY_E;

typedef enum _HSUART_BLOCKING_MODE_E {
    HSUART_NON_BLOCKING             = 0x0,  ///< Set the UART HAL driver to operate in non-blocking mode.
    HSUART_BLOCKING                 = 0x1   ///< Set the UART HAL driver to operate in blocking mode.
} HSUART_BLOCKING_MODE_E;

typedef union _HSUART_LSR {
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
} HSUART_LSR;

typedef union _HSUART_MSR {
    //
    // Individual bit fields
    //
    struct {
        uint32_t DCTS               : 1;    ///< Delta Clear To Send.
        uint32_t Reserved1          : 1;    ///< Reserved.
        uint32_t Reserved2          : 1;    ///< Reserved.
        uint32_t Reserved3          : 1;    ///< Reserved.
        uint32_t CTS                : 1;    ///< Clear To Send.
        uint32_t Reserved5          : 1;    ///< Reserved.
        uint32_t Reserved6          : 1;    ///< Reserved.
        uint32_t Reserved7          : 1;    ///< Reserved.
        uint32_t Reserved8          : 24;   ///< Reserved.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} HSUART_MSR;

typedef void (*HSUART_ISR) (void);

void hal_hsuart_register_isr (HSUART_INTERRUPT_ENABLE_E INT_ENABLE, HSUART_ISR ISR);

void hal_hsuart_init (void);

void hal_hsuart_deinit (void);

int32_t hal_hsuart_set_format (int32_t BaudRate, HSUART_WLS_E DataBits, HSUART_STB_E StopBits, HSUART_PARITY_E Parity);

void hal_hsuart_set_fifo (HSUART_INT_RX_FIFO_TRIG_LV_E INT_RX_TRIG_LV);

void hal_hsuart_enable_dma_ring_buffer (uint8_t *DMA_TX_STR_ADDR, uint8_t *DMA_TX_END_ADDR, uint8_t *DMA_RX_STR_ADDR, uint8_t *DMA_RX_END_ADDR);

void hal_hsuart_disable_dma_ring_buffer (void);

void hal_hsuart_set_hardware_flow_control (uint32_t RTS_AUTO_TH_L, uint32_t RTS_AUTO_TH_H);

void hal_hsuart_set_loop_back (void);

int32_t hal_hsuart_write_fifo (uint8_t const *Buffer, int32_t NumberOfBytes, HSUART_BLOCKING_MODE_E BlockingMode);

int32_t hal_hsuart_read_fifo (uint8_t *Buffer, int32_t NumberOfBytes, HSUART_BLOCKING_MODE_E BlockingMode);

int32_t hal_hsuart_write_dma_ring_buffer (uint8_t Character);

int32_t hal_hsuart_read_dma_ring_buffer (void);

void hal_hsuart_sw_rst (void);

uint32_t hal_hsuart_get_line_status (void);

uint32_t hal_hsuart_get_modem_status (void);

#endif  // #ifndef _HAL_HSUART_H_
