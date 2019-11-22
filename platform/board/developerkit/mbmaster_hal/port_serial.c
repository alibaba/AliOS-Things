/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>
#if (RHINO_CONFIG_MODBUS > 0)
#include <mbmaster_api.h>
#include "aos/hal/uart.h"
#include "stm32l4xx_hal.h"

static uart_dev_t  uart_mb;
UART_HandleTypeDef *huart;

void uart_receive_isr(UART_HandleTypeDef *);
void uart_send_isr(UART_HandleTypeDef *);

bool mb_serial_init( uint8_t port, uint32_t baud_rate, uint8_t data_width, mb_parity_t parity )
{
    switch(data_width) {
        case 7:
            uart_mb.config.data_width = DATA_WIDTH_7BIT;
            break;
        case 8:
            uart_mb.config.data_width = DATA_WIDTH_8BIT;
            break;
        case 9:
            uart_mb.config.data_width = DATA_WIDTH_9BIT;
            break;
    }

    switch(parity) {
        case MB_PAR_NONE:
            uart_mb.config.parity = NO_PARITY;
            break;
        case MB_PAR_ODD:
            uart_mb.config.parity = ODD_PARITY;
            break;
        case MB_PAR_EVEN:
            uart_mb.config.parity = EVEN_PARITY;
            break;
    }

    uart_mb.port                = port;
    uart_mb.config.baud_rate    = baud_rate;
    uart_mb.config.mode         = MODE_TX_RX;
    uart_mb.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_mb.config.stop_bits    = STOP_BITS_1;

    hal_uart_init(&uart_mb);

    huart = uart_mb.priv;

    return true;
}

void mb_serial_enable( bool enable_rx, bool enable_tx )
{
    mb_log( MB_LOG_DEBUG, "serial config:RX=0x%x,TX=0x%x\n", enable_rx, enable_tx);

    if (!enable_rx && enable_tx) {
        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
        CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
        huart->RxState = HAL_UART_STATE_READY;
        huart->RxISR = NULL;
    
        if(huart->gState == HAL_UART_STATE_READY) {
            huart->ErrorCode = HAL_UART_ERROR_NONE;
            huart->gState = HAL_UART_STATE_BUSY_TX;
            huart->TxISR = uart_send_isr;
            SET_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
        }
    } else if (enable_rx && ! enable_tx){

        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
        huart->gState = HAL_UART_STATE_READY;
         huart->RxISR = NULL;

        if(huart->RxState == HAL_UART_STATE_READY) {
            huart->ErrorCode = HAL_UART_ERROR_NONE;
            huart->RxState = HAL_UART_STATE_BUSY_RX;

            __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);      /* recover from run over */
            __HAL_UART_CLEAR_IT(huart, USART_ICR_ORECF);

            /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
           SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

            huart->RxISR = uart_receive_isr;
            SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
             
        }

    } else if (!enable_rx && ! enable_tx) {
        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
        huart->gState = HAL_UART_STATE_READY;
         huart->RxISR = NULL;

        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
        CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
        huart->RxState = HAL_UART_STATE_READY;
        huart->RxISR = NULL;
    }
}

void mb_serial_close( void )
{
    
}

bool mb_serial_send_byte( int8_t data )
{
    huart->Instance->TDR = data;
    return true;
}

bool mb_serial_rev_byte( int8_t *data )
{
    uint16_t uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
    *data = (uint8_t)(uhdata);

    return true;
}

void uart_receive_isr(UART_HandleTypeDef *uart_handler)
{
    if(huart->ErrorCode != HAL_UART_ERROR_NONE) {
        mb_log(MB_LOG_ERROR, "uart_receive_isr errorcode=0x%x\n", huart->ErrorCode);
    }

    mb_frame_byte_received_func();
}

void uart_send_isr(UART_HandleTypeDef *uart_handler)
{
    mb_frame_byte_send_func();
}

#endif
