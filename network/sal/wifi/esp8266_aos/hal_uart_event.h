#include <stdio.h>
#include <stdint.h>

/****** HAL_UART_EVENT *****/
typedef enum {
    eUART_EVENT_SEND_COMPLETE       = 0,  ///< Send completed; however UART may still transmit data
    eUART_EVENT_RECEIVE_COMPLETE    = 1,  ///< Receive completed
    eUART_EVENT_TRANSFER_COMPLETE   = 2,  ///< Transfer completed
    eUART_EVENT_TX_COMPLETE         = 3,  ///< Transmit completed (optional)
    eUART_EVENT_TX_UNDERFLOW        = 4,  ///< Transmit data not available (Synchronous Slave)
    eUART_EVENT_RX_OVERFLOW         = 5,  ///< Receive data overflow
    eUART_EVENT_RX_TIMEOUT          = 6,  ///< Receive character timeout (optional)
    eUART_EVENT_RX_BREAK            = 7,  ///< Break detected on receive
    eUART_EVENT_RX_FRAMING_ERROR    = 8,  ///< Framing error detected on receive
    eUART_EVENT_RX_PARITY_ERROR     = 9,  ///< Parity error detected on receive
    eUART_EVENT_CTS                 = 10, ///< CTS state changed (optional)
    eUART_EVENT_DSR                 = 11, ///< DSR state changed (optional)
    eUART_EVENT_DCD                 = 12, ///< DCD state changed (optional)
    eUART_EVENT_RI                  = 13, ///< RI  state changed (optional)
    eUART_EVENT_RECEIVED            = 14, ///< Data Received, only in usart fifo, call receive()/transfer() get the data
} E_HAL_UART_EVENT;

typedef void (*hal_uart_event_cbfun)(int32_t idx, E_HAL_UART_EVENT event);
