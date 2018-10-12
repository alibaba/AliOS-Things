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
#include "vcom.h"
#include "hw_gpio.h"
#include <stdarg.h>
#include "low_power.h"

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

static char dbgTxBuff[256];

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

static int dbg_buffer_transmit( int start, int len )
{
    int i;
    for ( i = start; i < len; i++ )
    {
        LL_USART_ClearFlag_TC( USART4 );
        LL_USART_TransmitData8( USART4, dbgTxBuff[i] );

        while ( LL_USART_IsActiveFlag_TC( USART4 ) != SET )
        {
            ;
        }
    }
    LL_USART_ClearFlag_TC( USART4 );
    return len;
}

void DBG_Uart_Deinit( void )
{
    LL_USART_DeInit( USART4 );

    /*##-1- Reset peripherals ##################################################*/
    LL_APB1_GRP1_ForceReset( LL_APB1_GRP1_PERIPH_USART4 );
    LL_APB1_GRP1_ReleaseReset( LL_APB1_GRP1_PERIPH_USART4 );

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HW_GPIO_DeInit( GPIOA, GPIO_PIN_0 );
    /* Configure UART Rx as alternate function  */
    HW_GPIO_DeInit( GPIOA, GPIO_PIN_1 );

    /*##-3- Disable the NVIC for UART ##########################################*/
    NVIC_DisableIRQ( USART4_5_IRQn );
}

void DBG_Uart_Init( void )
{
    LL_USART_InitTypeDef USART_InitStruct;

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    DBG_Uart_Deinit( );
    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_USART4 );

    /**USART4 GPIO Configuration
     PA0   ------> USART4_TX
     PA1   ------> USART4_RX
     */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
    LL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
    LL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
#ifdef CONFIG_LINKWAN_AT
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
#else
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;
#endif
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    LL_USART_Init( USART4, &USART_InitStruct );

    LL_USART_DisableOverrunDetect( USART4 );

    LL_USART_ConfigAsyncMode( USART4 );

    LL_USART_Enable( USART4 );
    NVIC_SetPriority(USART4_5_IRQn, 0);
    NVIC_EnableIRQ(USART4_5_IRQn);
    LL_USART_EnableIT_RXNE(USART4);
}

#if (RHINO_CONFIG_PANIC > 0)
//use DBG_Send in exception print
void print_str() __attribute__ ((alias ("DBG_Send")));
#endif

void DBG_Send( const char *format, ... )
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
            len = current_len + vsnprintf( dbgTxBuff + current_len,
                                           sizeof(dbgTxBuff) - current_len,
                                           format, args );
        }
        RHINO_CPU_INTRPT_ENABLE();
        va_end( args );
        return;
    }
    else
    {
        len = vsnprintf( dbgTxBuff, sizeof(dbgTxBuff), format, args );
    }

    current_len = len;
    RHINO_CPU_INTRPT_ENABLE();

    start = 0;

    do
    {
        stop = dbg_buffer_transmit( start, current_len );

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
    } while ( current_len != stop );

    va_end( args );
}

void vcom_Init( void )
{
    LL_LPUART_InitTypeDef LPUART_InitStruct;

    /*## Configure the UART peripheral ######################################*/
    /* Put the UART peripheral in the Asynchronous mode (UART Mode) */
    /* UART1 configured as follow:
     - Word Length = 8 Bits
     - Stop Bit = One Stop bit
     - Parity = ODD parity
     - BaudRate = 921600 baud
     - Hardware flow control disabled (RTS and CTS signals) */

    /*
     * Clock initialization:
     * - LSE source selection
     * - LPUART clock enable
     * - GPIO clocks are enabled in vcom_IoInit()
     */
    LL_RCC_SetLPUARTClockSource( LL_RCC_LPUART1_CLKSOURCE_HSI );
    UARTX_CLK_ENABLE();
    vcom_IoInit( );

    LPUART_InitStruct.BaudRate = 9600;
    LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
    LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
    LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
    LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
    LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;

    if ( LL_LPUART_Init( UARTX, &LPUART_InitStruct ) != SUCCESS )
    {
        Error_Handler( );
    }

    /* Configuring the LPUART specific LP feature - the wakeup from STOP */
    LL_LPUART_EnableInStopMode( UARTX );

    /* WakeUp from stop mode on start bit detection*/
    LL_LPUART_SetWKUPType( UARTX, LL_LPUART_WAKEUP_ON_STARTBIT );
    //LL_LPUART_SetWKUPType(UARTX, LL_LPUART_WAKEUP_ON_RXNE);
    LL_LPUART_EnableIT_WKUP( UARTX );

    LL_LPUART_Enable( UARTX );
    while ( LL_LPUART_IsActiveFlag_TEACK( UARTX ) == RESET )
    {
        ;
    }
    while ( LL_LPUART_IsActiveFlag_REACK( UARTX ) == RESET )
    {
        ;
    }

    uart_context.gState = HAL_UART_STATE_READY;
    uart_context.RxState = HAL_UART_STATE_READY;
}

void vcom_DeInit( void )
{
    LL_LPUART_DeInit( UARTX );

    /*##-1- Reset peripherals ##################################################*/
    UARTX_FORCE_RESET();
    UARTX_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HW_GPIO_DeInit( UARTX_TX_GPIO_PORT, UARTX_TX_PIN );
    /* Configure UART Rx as alternate function  */
    HW_GPIO_DeInit( UARTX_RX_GPIO_PORT, UARTX_RX_PIN );

    /*##-3- Disable the NVIC for UART ##########################################*/
    NVIC_DisableIRQ( UARTX_IRQn );
}

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
    LL_LPUART_EnableIT_PE( UARTX );

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    LL_LPUART_EnableIT_ERROR( UARTX );

    /* Process Unlocked */
    HW_UNLOCK( &uart_context );
}

void vcom_IoInit( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = UARTX_TX_AF;

    HW_GPIO_Init( UARTX_TX_GPIO_PORT, UARTX_TX_PIN, &GPIO_InitStruct );

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Alternate = UARTX_RX_AF;

    HW_GPIO_Init( UARTX_RX_GPIO_PORT, UARTX_RX_PIN, &GPIO_InitStruct );

    /*##-3- Configure the NVIC for UART ########################################*/
    /* NVIC for UART */
    NVIC_SetPriority( UARTX_IRQn, 0 );
    NVIC_EnableIRQ( UARTX_IRQn );

    /* enable RXNE */
    LL_LPUART_EnableIT_RXNE( UARTX );
}

void vcom_IoDeInit( void )
{
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HW_GPIO_Init( UARTX_TX_GPIO_PORT, UARTX_TX_PIN, &GPIO_InitStructure );

    HW_GPIO_Init( UARTX_RX_GPIO_PORT, UARTX_RX_PIN, &GPIO_InitStructure );
}

FlagStatus IsNewCharReceived( void )
{
    FlagStatus status;
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    status = ((uart_context.rx_idx_toread == uart_context.rx_idx_free) ? RESET : SET);

    RHINO_CPU_INTRPT_ENABLE();
    return status;
}

uint8_t GetNewChar( void )
{
    uint8_t NewChar;
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    NewChar = uart_context.buffRx[uart_context.rx_idx_toread];
    uart_context.rx_idx_toread = (uart_context.rx_idx_toread + 1) % sizeof(uart_context.buffRx);

    RHINO_CPU_INTRPT_ENABLE();
    return NewChar;
}

void vcom_IRQHandler( void )
{
    int rx_ready = 0;
    char rx;

    /* UART Wake Up interrupt occured ------------------------------------------*/
    if ( LL_LPUART_IsActiveFlag_WKUP( UARTX ) && (LL_LPUART_IsEnabledIT_WKUP( UARTX ) != RESET) )
    {
        LL_LPUART_ClearFlag_WKUP( UARTX );

#if defined(EML3047_LORAWAN)
        /* forbid stop mode */
        LowPower_Disable( e_LOW_POWER_UART );
#endif

        /* Enable the UART Data Register not empty Interrupt */
        LL_LPUART_EnableIT_RXNE( UARTX );
    }

    if ( LL_LPUART_IsActiveFlag_RXNE( UARTX ) && (LL_LPUART_IsEnabledIT_RXNE( UARTX ) != RESET) )
    {
        /* no need to clear the RXNE flag because it is auto cleared by reading the data*/
        rx = LL_LPUART_ReceiveData8( UARTX );
        rx_ready = 1;
        //PRINTF("%02X\n", rx);

#if defined(EML3047_LORAWAN)
        /* allow stop mode*/
        LowPower_Enable( e_LOW_POWER_UART );
#endif
    }

    if ( LL_LPUART_IsActiveFlag_PE( UARTX ) || LL_LPUART_IsActiveFlag_FE( UARTX ) || LL_LPUART_IsActiveFlag_ORE( UARTX )
         || LL_LPUART_IsActiveFlag_NE( UARTX ) )
    {
        PRINTF("Error when receiving\n")
        ;
        /* clear error IT */
        LL_LPUART_ClearFlag_PE( UARTX );
        LL_LPUART_ClearFlag_FE( UARTX );
        LL_LPUART_ClearFlag_ORE( UARTX );
        LL_LPUART_ClearFlag_NE( UARTX );

        rx = 0x01;
        rx_ready = 1;
    }

    if ( rx_ready == 1 )
    {
        receive( rx );
    }
}

/* Private functions Definition ------------------------------------------------------*/

static int buffer_transmit( int start, int len )
{
    int i;
    for ( i = start; i < len; i++ )
    {
        LL_LPUART_ClearFlag_TC( UARTX );
        LL_LPUART_TransmitData8( UARTX, uart_context.buffTx[i] );

        while ( LL_LPUART_IsActiveFlag_TC( UARTX ) != SET )
        {
            ;
        }
    }
    LL_LPUART_ClearFlag_TC( UARTX );
    return len;
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
        PRINTF("uart_context.buffRx buffer overflow %d\r\n")
        ;
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
