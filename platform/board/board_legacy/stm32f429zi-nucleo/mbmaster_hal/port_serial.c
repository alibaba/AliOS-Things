/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>
#if (RHINO_CONFIG_MODBUS > 0)
#include <mbmaster_api.h>
#include "aos/hal/uart.h"
#include "stm32f4xx_hal.h"

static uart_dev_t  uart_mb;
UART_HandleTypeDef *huart;

void uart_receive_isr(UART_HandleTypeDef *);
void uart_send_isr(UART_HandleTypeDef *);

char rev_buff[10];

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
#if 0
    huart->pRxBuffPtr = rev_buff;
    huart->RxXferSize = 1;
    huart->RxXferCount = 1;

    huart = uart_mb.priv;
#endif
    return true;
}

void mb_serial_enable( bool enable_rx, bool enable_tx )
{
    mb_log( MB_LOG_DEBUG, "serial config:RX=0x%x,TX=0x%x\n", enable_rx, enable_tx);
#if 1
    if (!enable_rx && enable_tx) {
			#if 0
        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
        CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
        huart->RxState = HAL_UART_STATE_READY;
        //huart->RxISR = NULL;
    
        if(huart->gState == HAL_UART_STATE_READY) {
            huart->ErrorCode = HAL_UART_ERROR_NONE;
            huart->gState = HAL_UART_STATE_BUSY_TX;
            //mb_frame_byte_send_func();
            //huart->TxISR = uart_send_isr;
            SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
        }
				#else
					//mb_frame_byte_send_func();
				#endif
    } else if (enable_rx && ! enable_tx){
				#if 0
        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
        huart->gState = HAL_UART_STATE_READY;
        // huart->RxISR = NULL;
        huart->RxXferCount = 1;
        huart->pRxBuffPtr = rev_buff;

        if(huart->RxState == HAL_UART_STATE_READY) {
            huart->ErrorCode = HAL_UART_ERROR_NONE;
            huart->RxState = HAL_UART_STATE_BUSY_RX;
            //mb_frame_byte_received_func();

            // __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);      /* recover from run over */
            // __HAL_UART_CLEAR_IT(huart, USART_ICR_ORECF);

            /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
            SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

            //huart->RxISR = uart_receive_isr;
            SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
             
        }
				#else
					//mb_frame_byte_received_func();
				#endif
    } else if (!enable_rx && ! enable_tx) {
			  #if 0
        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
        huart->gState = HAL_UART_STATE_READY;
        // huart->RxISR = NULL;
        
        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
        CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
        huart->RxState = HAL_UART_STATE_READY;
        //huart->RxISR = NULL;
			  #endif
    }
#endif
}

void mb_serial_close( void )
{
    
}
#if 0
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    mb_frame_byte_send_func();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    mb_frame_byte_received_func();
}
#endif
bool mb_serial_send_byte( int8_t* data, uint8_t size)
{
#if 0
    if( HAL_UART_Transmit(huart, &data, 1, 30000) != HAL_OK) 
    {
        printf("########## mb data send failed ##########\n");
        return false;
    }
    SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
#else
    hal_uart_send(&uart_mb, data, size, 30000);
#endif
    return true;
}

bool mb_serial_rev_byte( int8_t *data )
{
    int recvsize = 0;
    int ret = 0;
#if 0
    *data = rev_buff[0];
    huart->pRxBuffPtr = rev_buff;
    huart->RxXferCount = 1;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
#else 
    ret = hal_uart_recv_II(&uart_mb, data, 1, &recvsize, AOS_WAIT_FOREVER);
    if (ret) {
        printf("%s fail to recv data \r\n", __func__);
    }
#endif
    return true;
}

#if 0
void UART7_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(huart);   
   krhino_intrpt_exit();
}
#endif
// void uart_receive_isr(UART_HandleTypeDef *uart_handler)
// {
//     if(huart->ErrorCode != HAL_UART_ERROR_NONE) {
//         mb_log(MB_LOG_ERROR, "uart_receive_isr errorcode=0x%x\n", huart->ErrorCode);
//     }

//     mb_frame_byte_received_func();
// }

// void uart_send_isr(UART_HandleTypeDef *uart_handler)
// {
//     mb_frame_byte_send_func();
// }



#endif
