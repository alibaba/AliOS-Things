/******************************************************************************
 * @file    vcom.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   manages virtual com port
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "hw.h"
#include "uart-board.h"
#include "gpio-board.h"
#include <stdarg.h>
#include "low_power.h"
#include "dbg_port.h"

/* Force include of hal uart in order to inherite HAL_UART_StateTypeDef definition */
#include "stm32l0xx_hal_dma.h"
#include "stm32l0xx_hal_uart.h"

#include <k_api.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* based on UART_HandleTypeDef */
static struct
{
    char buffTx[256]; /**< buffer to transmit */
    char buffRx[256]; /**< Circular buffer of received chars */
    int rx_idx_free; /**< 1st free index in BuffRx */
    int rx_idx_toread; /**< next char to read in buffRx, when not rx_idx_free */
    HW_LockTypeDef Lock; /**< Locking object */

    __IO HAL_UART_StateTypeDef gState; /**< UART state information related to global Handle management
     and also related to Tx operations. */
    __IO HAL_UART_StateTypeDef RxState; /**< UART state information related to Rx operations. */
} uart_context;


/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Transmit uart_context.buffTx from start to len - 1
 * @param  1st index to transmit
 * @param  Last index not to transmit
 * @return Last index not transmitted
 */
static int buffer_transmit( int start, int len );

/**
 * @brief  Takes one character that has been received and save it in uart_context.buffRx
 * @param  received character
 */
static void receive( char rx );

/* Functions Definition ------------------------------------------------------*/

static int buffer_transmit( int start, int len )
{
    int i;
    for ( i = start; i < len; i++ )
    {
        LL_USART_ClearFlag_TC( USART2 );
        LL_USART_TransmitData8( USART2, uart_context.buffTx[i] );
        wwdg_feed_check_timer();
        while ( LL_USART_IsActiveFlag_TC( USART2 ) != SET )
        {
            ;
        }
    }
    LL_USART_ClearFlag_TC( USART2 );
    return len;
}

void vcom_DeInit( void )
{
    LL_USART_DeInit( USART2 );

    /*##-1- Reset peripherals ##################################################*/
    LL_APB1_GRP1_ForceReset( LL_APB1_GRP1_PERIPH_USART2 );
    LL_APB1_GRP1_ReleaseReset( LL_APB1_GRP1_PERIPH_USART2 );

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HW_GPIO_DeInit( GPIOA, GPIO_PIN_2 );
    /* Configure UART Rx as alternate function  */
    HW_GPIO_DeInit( GPIOA, GPIO_PIN_3 );

    /*##-3- Disable the NVIC for UART ##########################################*/
    NVIC_DisableIRQ( USART2_IRQn );
}

__weak void vcom_parameter_set( LL_USART_InitTypeDef *usart_para )
{
    return;
}

void vcom_Init( void )
{
    LL_USART_InitTypeDef USART_InitStruct;
    LL_USART_InitTypeDef Usart_Para = {
        .BaudRate = 115200,
        .DataWidth = LL_USART_DATAWIDTH_8B,
        .StopBits = LL_USART_STOPBITS_1,
        .Parity = LL_USART_PARITY_NONE
    };
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    vcom_DeInit( );
    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_USART2 );

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
     PA9   ------> USART2_TX
     PA10  ------> USART2_RX
     */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    vcom_parameter_set( &Usart_Para );
    USART_InitStruct.BaudRate = Usart_Para.BaudRate;
    USART_InitStruct.DataWidth = Usart_Para.DataWidth;
    USART_InitStruct.StopBits = Usart_Para.StopBits;
    USART_InitStruct.Parity = Usart_Para.Parity;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    LL_USART_Init( USART2, &USART_InitStruct );

    LL_USART_DisableOverrunDetect( USART2 );

    LL_USART_ConfigAsyncMode( USART2 );

    LL_USART_Enable( USART2 );

    NVIC_SetPriority( USART2_IRQn, 0 );
    NVIC_EnableIRQ( USART2_IRQn );

    /* enable RXNE */
    LL_USART_EnableIT_RXNE( USART2 );

    uart_context.gState = HAL_UART_STATE_READY;
    uart_context.RxState = HAL_UART_STATE_READY;
    LowPower_Disable( e_LOW_POWER_UART );
}

#if (RHINO_CONFIG_PANIC > 0)
//use DBG_Send in exception print
void print_str() __attribute__ ((alias ("vcom_Send")));
#endif

void vcom_Send( const char *format, ... )
{
    va_list args;
    static __IO uint16_t len = 0;
    uint16_t current_len;
    int start;
    int stop;
    CPSR_ALLOC();

    va_start( args, format );

    RHINO_CPU_INTRPT_DISABLE();
    if ( len != 0 )
    {
        if ( len != sizeof(uart_context.buffTx) )
        {
            current_len = len; /* use current_len instead of volatile len in below computation */
            len = current_len + vsnprintf( uart_context.buffTx + current_len,
                                           sizeof(uart_context.buffTx) - current_len,
                                           format, args );
        }
        RHINO_CPU_INTRPT_ENABLE();
        va_end( args );
        return;
    }
    else
    {
        len = vsnprintf( uart_context.buffTx, sizeof(uart_context.buffTx), format, args );
    }

    current_len = len;
    RHINO_CPU_INTRPT_ENABLE();

    start = 0;

    do
    {
        stop = buffer_transmit( start, current_len );

        {
            RHINO_CPU_INTRPT_DISABLE();
            if ( len == stop )
            {
                len = 0;
                RHINO_CPU_INTRPT_ENABLE();
            }
            else
            {
                start = stop;
                current_len = len;
                RHINO_CPU_INTRPT_ENABLE();
            }
        }
        wwdg_feed_check_timer();
    } while ( current_len != stop );

    va_end( args );
}

void vcom_ReceiveInit( void )
{
    if ( uart_context.RxState != HAL_UART_STATE_READY )
    {
        return;
    }

    /* Process Locked */
    HW_LOCK( &uart_context );

    uart_context.RxState = HAL_UART_STATE_BUSY_RX;

    /* Enable the UART Parity Error Interrupt */
    LL_USART_EnableIT_PE( USART2 );

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    LL_USART_EnableIT_ERROR( USART2 );

    /* Process Unlocked */
    HW_UNLOCK( &uart_context );
}



FlagStatus IsNewCharReceived( void )
{
    FlagStatus status;

    BACKUP_PRIMASK();
    DISABLE_IRQ();

    status = ((uart_context.rx_idx_toread == uart_context.rx_idx_free) ? RESET : SET);

    RESTORE_PRIMASK();
    return status;
}

uint8_t GetNewChar( void )
{
    uint8_t NewChar;

    BACKUP_PRIMASK();
    DISABLE_IRQ();

    NewChar = uart_context.buffRx[uart_context.rx_idx_toread];
    uart_context.rx_idx_toread = (uart_context.rx_idx_toread + 1) % sizeof(uart_context.buffRx);

    RESTORE_PRIMASK();
    return NewChar;
}

/* add by liwp */
uint16_t vcom_Receive( uint8_t *pucBuf )
{
    uint16_t length = 0;

    BACKUP_PRIMASK();
    DISABLE_IRQ();

    while( uart_context.rx_idx_toread != uart_context.rx_idx_free )
    {
        *pucBuf = uart_context.buffRx[uart_context.rx_idx_toread];
        uart_context.rx_idx_toread = (uart_context.rx_idx_toread + 1) % sizeof(uart_context.buffRx);
        pucBuf++;
        length++;
    }

    RESTORE_PRIMASK();
    return length;
}
/* add end */

void vcom_IRQHandler( void )
{
    int rx_ready = 0;
    char rx;

    if ( LL_USART_IsActiveFlag_RXNE( USART2 ) && (LL_USART_IsEnabledIT_RXNE( USART2 ) != RESET) )
    {
        /* no need to clear the RXNE flag because it is auto cleared by reading the data*/
        rx = LL_USART_ReceiveData8( USART2 );
        rx_ready = 1;
        //PRINTF("%02X\n", rx);

        /* allow stop mode*/
        //LowPower_Enable( e_LOW_POWER_UART );
    }

    if ( LL_USART_IsActiveFlag_PE( USART2 ) || LL_USART_IsActiveFlag_FE( USART2 ) || LL_USART_IsActiveFlag_ORE( USART2 )
         || LL_USART_IsActiveFlag_NE( USART2 ) )
    {
        PRINTF("Error when receiving\n")
        ;
        /* clear error IT */
        LL_USART_ClearFlag_PE( USART2 );
        LL_USART_ClearFlag_FE( USART2 );
        LL_USART_ClearFlag_ORE( USART2 );
        LL_USART_ClearFlag_NE( USART2 );

        rx = 0x01;
        rx_ready = 1;
    }

    if ( rx_ready == 1 )
    {
        receive( rx );
    }
}


static void receive( char rx )
{
    int next_free;

    /** no need to clear the RXNE flag because it is auto cleared by reading the data*/
    uart_context.buffRx[uart_context.rx_idx_free] = rx;
    next_free = (uart_context.rx_idx_free + 1) % sizeof(uart_context.buffRx);
    if ( next_free != uart_context.rx_idx_toread )
    {
        /* this is ok to read as there is no buffer overflow in input */
        uart_context.rx_idx_free = next_free;
    }
    else
    {
        /* force the end of a command in case of overflow so that we can process it */
        uart_context.buffRx[uart_context.rx_idx_free] = '\r';
        PRINTF("uart_context.buffRx buffer overflow\r\n")
        ;
    }
}

void _printf_float()
{
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
