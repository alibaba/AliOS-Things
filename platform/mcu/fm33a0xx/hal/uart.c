/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_UART_C
#define HAL_UART_C
#include "stdint.h"
#include "aos/errno.h"
#include "fm33a0xx_include_all.h"
#include "board.h"

// ============== Fm33A0X Uart Define Begin =============== //

typedef struct 
{
    uint8_t * buffer;
    uint16_t  capcity;
    uint16_t  processed;
    uint16_t  point;
    uint16_t  Count;
} SerialDataParams_t;

typedef struct 
{
    SerialDataParams_t recv;
    SerialDataParams_t send;
} SerialServer_t;



SerialServer_t SerialServerParams[6];

static const uint8_t   Uart_Irqn[6] =
{
    UART0_IRQn, UART1_IRQn, UART2_IRQn, UART3_IRQn, UART4_IRQn, UART5_IRQn
};
static const UARTx_Type* uartReg[6]={UART0,UART1,UART2,UART3,UART4,UART5};

static void Serial_InterruptTx (uint8_t port)
{
    SerialDataParams_t * pComParams;
    if (port > UART_5)
    {
        return;
    }
    pComParams = & (SerialServerParams[port].send);

    if (pComParams->Count > 0)
    {
        if (pComParams->processed >= pComParams->capcity)
        {
            pComParams->processed = 0;
        }
        UARTx_TXREG_Write ((UARTx_Type*)uartReg[port], pComParams->buffer[pComParams->processed]);
        pComParams->Count--;
        pComParams->processed++;
    }
    else 
    {
//    pComParams->point = 0;
//    pComParams->processed = 0;
//    UARTx_TXSTA_TXEN_Setable((UARTx_Type*)uartReg[port],DISABLE);
        UART_UARTIE_RxTxIE_SetableEx((UARTx_Type*)uartReg[port],TxInt,DISABLE);
    }
    
}

static void Serial_InterruptRx (uint8_t port)
{
    SerialDataParams_t * pComParams;
    uint16_t  temp_data = 0;

    if (port > UART_5)
    {
        return;
    }

    pComParams = & (SerialServerParams[port].recv);

    if (pComParams->Count < pComParams->capcity)
    {
        temp_data = UARTx_RXREG_Read ((UARTx_Type*)uartReg[port]);
        temp_data &= 0x00ff;

        pComParams->buffer[pComParams->point++] = temp_data;

        if (pComParams->point >= pComParams->capcity)
        {
            pComParams->point = 0;
        }

        pComParams->Count++;
    }
    else
    {
        temp_data = UARTx_RXREG_Read ((UARTx_Type*)uartReg[port]);
        temp_data &= 0x00ff;

        pComParams->buffer[pComParams->point++] = temp_data;

        if (pComParams->point >= pComParams->capcity)
        {
            pComParams->point = 0;
        }

        pComParams->processed++;

        if (pComParams->processed >= pComParams->capcity)
        {
            pComParams->processed = 0;
        }
    }
}

static int16_t Serial_SvSend (uint8_t port, void * vBuf, uint16_t uSize)
{
    SerialDataParams_t * pComParams;
    uint16_t  uCount = 0;
    uint16_t  uLenTemp = 0;
    uint8_t *pBuf = (uint8_t*)vBuf;
    pComParams = & (SerialServerParams[port].send);

    if (pComParams->point >= pComParams->processed)
    {
        uLenTemp  = (pComParams->capcity) - (pComParams->point);
        uLenTemp  = (uLenTemp > uSize ? uSize: uLenTemp); //查找最小值
        uLenTemp  = (uLenTemp >
             (pComParams->capcity - pComParams->Count) ? (pComParams->capcity - pComParams->Count): uLenTemp);
        memcpy ((pComParams->buffer + pComParams->point), (pBuf + uCount), 
            uLenTemp);
        pComParams->point += uLenTemp;

        if (pComParams->point >= pComParams->capcity)
        {
            pComParams->point = 0;
        }

        pComParams->Count += uLenTemp;
        uCount    += uLenTemp;
        uSize     -= uLenTemp;
    }

    if (uSize)
    {
        uLenTemp  = (pComParams->processed) - (pComParams->point);
        uLenTemp  = (uLenTemp > uSize ? uSize: uLenTemp); //查找最小值
        uLenTemp  = (uLenTemp >
             (pComParams->capcity - pComParams->Count) ? (pComParams->capcity - pComParams->Count): uLenTemp);
        pBuf      += uCount;
        memcpy ((pComParams->buffer + pComParams->point), (pBuf), uLenTemp);

        pComParams->point += uLenTemp;

        if (pComParams->point >= pComParams->capcity)
        {
            pComParams->point = 0;
        }

        pComParams->Count += uLenTemp;
        uCount    += uLenTemp;
        uSize     -= uLenTemp;
    }
    if(UART_UARTIE_RxTxIE_GetableEx((UARTx_Type*)uartReg[port],TxInt)==DISABLE)
    {
        UARTx_TXSTA_TXEN_Setable((UARTx_Type*)uartReg[port],ENABLE);
        UART_UARTIE_RxTxIE_SetableEx((UARTx_Type*)uartReg[port],TxInt,ENABLE);
    }

    return uCount;
}

static int16_t Serial_SvRecv (uint8_t port, uint8_t * pBuf, uint16_t uSize)
{
    SerialDataParams_t * pComParams;
    uint16_t  uCount = 0;
    uint16_t  uLenTemp = 0;
    
    pComParams = & (SerialServerParams[port].recv);

    if (pComParams->point <= pComParams->processed)
    {
        uLenTemp  = (pComParams->capcity) - (pComParams->processed);
        uLenTemp  = (uLenTemp > uSize ? uSize: uLenTemp);
        uLenTemp  = (uLenTemp > pComParams->Count ? pComParams->Count: uLenTemp);

        memcpy (pBuf, (pComParams->buffer + pComParams->processed), uLenTemp);

        pComParams->processed += uLenTemp;

        if (pComParams->processed >= pComParams->capcity)
        {
            pComParams->processed = 0;
        }

        pComParams->Count -= uLenTemp;
        uCount    += uLenTemp;
        uSize     -= uLenTemp;

    }

    if ((uSize) && (pComParams->point > pComParams->processed))
    {
        uLenTemp  = (pComParams->point) - (pComParams->processed);
        uLenTemp  = (uLenTemp > uSize ? uSize: uLenTemp);
        uLenTemp  = (uLenTemp > pComParams->Count ? pComParams->Count: uLenTemp);

        pBuf      += uCount;

        memcpy (pBuf, (pComParams->buffer + pComParams->processed), uLenTemp);

        pComParams->processed += uLenTemp;

        if (pComParams->processed >= pComParams->capcity)
        {
            pComParams->processed = 0;
        }

        pComParams->Count -= uLenTemp;
        uCount    += uLenTemp;
        uSize     -= uLenTemp;
    }
    
    return uCount;
}

static uint32_t SerialGetSendRemain(uint8_t port)
{
    SerialDataParams_t * pComParams;
 
    pComParams = & (SerialServerParams[port].send);
    return pComParams->Count;
}
static uint32_t SerialGetRecvRemain(uint8_t port)
{
    SerialDataParams_t * pComParams;
    
    pComParams = & (SerialServerParams[port].recv);
    return pComParams->Count;
}

static void SerialBuffInit(uint8_t port ,uint16_t RxSize,uint16_t TxSize)
{
    if (port > UART_5)
    {
        return;
    }
    SerialServerParams[port].recv.buffer = (uint8_t*)aos_malloc(RxSize);
    SerialServerParams[port].recv.capcity = RxSize;
    SerialServerParams[port].recv.processed = 0;
    SerialServerParams[port].recv.point = 0;
    SerialServerParams[port].recv.Count = 0;
    SerialServerParams[port].send.buffer = (uint8_t*)aos_malloc(TxSize);
    SerialServerParams[port].send.capcity = TxSize;
    SerialServerParams[port].send.processed = 0;
    SerialServerParams[port].send.point = 0;
    SerialServerParams[port].send.Count = 0;
}
static void SerialBuffFinalize(uint8_t port )
{
    if (port > UART_5)
    {
        return;
    }
    aos_free(SerialServerParams[port].recv.buffer);
    aos_free(SerialServerParams[port].send.buffer);
    memset(&SerialServerParams[port],0,sizeof(SerialServer_t));
}


static void UART_IRQHandler (uint8_t port,UARTx_Type *uartx)
{
    if (UART_UARTIF_RxTxIF_ChkEx(uartx,TxInt))
    {
        Serial_InterruptTx(port);
    }
    if (UART_UARTIF_RxTxIF_ChkEx(uartx,RxInt))
    {
        Serial_InterruptRx(port);
    }

}

void UART0_IRQHandler (void)
{
    UART_IRQHandler (UART_0,UART0);
}

void UART1_IRQHandler (void)
{
    UART_IRQHandler (UART_1,UART1);
}

void UART2_IRQHandler (void)
{
    UART_IRQHandler (UART_2,UART2);
}

void UART3_IRQHandler (void)
{
    UART_IRQHandler (UART_3,UART3);
}

void UART4_IRQHandler (void)
{
    UART_IRQHandler (UART_4,UART4);
}

void UART5_IRQHandler (void)
{
    UART_IRQHandler (UART_5,UART5);
}




// ============== Fm33A0X Uart Define End =============== //

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */

int32_t hal_uart_init(uart_dev_t *uart)
{
    UART_InitTypeDef UART_para;
    RCC_ClocksType RCC_Clocks;  
     UARTx_Type *UARTx = NULL;
    
    if (uart->port > UART_5)
    {
        return EIO;
    }
    
    if ((uart->config.data_width > DATA_WIDTH_9BIT ||
             uart->config.data_width <= DATA_WIDTH_6BIT) ||
            (uart->config.parity > EVEN_PARITY) ||
            (uart->config.stop_bits > STOP_BITS_2) ||
            (uart->config.flow_control > FLOW_CONTROL_CTS_RTS) ||
            (uart->config.mode > MODE_TX_RX))
    {
        return EIO;
    }
    
    RCC_PERCLK_SetableEx(UARTCOMCLK, ENABLE); //UART0~5 all clk enable
 
    switch (uart->port)
    {
        case UART_0:
            RCC_PERCLK_SetableEx(UART0CLK, ENABLE);   //UARTx clk enable
                        //  UART0 IO config
                        AltFunIO(UART0RX_Port, UART0RX_Pin, 0);     //UART0 RX
                        AltFunIO(UART0TX_Port, UART0TX_Pin, 0);     //UART0 TX
                
                        /*NVIC interrupt config*/
                        NVIC_DisableIRQ(UART0_IRQn);
                        //NVIC_SetPriority(UART0_IRQn,2);//interrupt priority config
                        //NVIC_EnableIRQ(UART0_IRQn);   
            UARTx = UART0;
            
            break;
        case UART_1:
            RCC_PERCLK_SetableEx(UART1CLK, ENABLE);   //UARTx clk enable  
                        //  UART1 IO config
                        AltFunIO(UART1RX_Port, UART1RX_Pin, 0);     //UART1 RX
                        AltFunIO(UART1TX_Port, UART1TX_Pin, 0);     //UART1 TX
                        
                        /*NVIC interrupt config*/
                        NVIC_DisableIRQ(UART1_IRQn);
                        //NVIC_SetPriority(UART1_IRQn,2);//interrupt priority config
                        //NVIC_EnableIRQ(UART1_IRQn);   
            UARTx = UART1;
            break;
        case UART_2:
            RCC_PERCLK_SetableEx(UART2CLK, ENABLE);   //UARTx clk enable
                        //  UART2 IO config
                        AltFunIO(UART2RX_Port, UART2RX_Pin, 0);     //UART2 RX
                        AltFunIO(UART2TX_Port, UART2TX_Pin, 0);     //UART2 TX
                        
                        /*NVIC interrupt config*/
                        NVIC_DisableIRQ(UART2_IRQn);
                        //NVIC_SetPriority(UART2_IRQn,2);//interrupt priority config
                        //NVIC_EnableIRQ(UART2_IRQn);   
            UARTx = UART2;
            break;
        case UART_3:
            RCC_PERCLK_SetableEx(UART3CLK, ENABLE);   //UARTx clk enable
                        //  UART3 IO config
                        AltFunIO(UART3RX_Port, UART3RX_Pin, 0);     //UART3 RX
                        AltFunIO(UART3TX_Port, UART3TX_Pin, 0);     //UART3 TX
                        
                        /*NVIC interrupt config*/
                        NVIC_DisableIRQ(UART3_IRQn);
                        //NVIC_SetPriority(UART3_IRQn,2);//interrupt priority config
                        //NVIC_EnableIRQ(UART3_IRQn);   
            UARTx = UART3;
            break;
        case UART_4:
            RCC_PERCLK_SetableEx(UART4CLK, ENABLE);   //UARTx clk enable  
                        //  UART4 IO config
                        AltFunIO(UART4RX_Port, UART4RX_Pin, 0);     //UART4 RX
                        AltFunIO(UART4TX_Port, UART4TX_Pin, 0);     //UART4 TX
                        
                        /*NVIC interrupt config*/
                        NVIC_DisableIRQ(UART4_IRQn);
                        //NVIC_SetPriority(UART4_IRQn,2);//
                        //NVIC_EnableIRQ(UART4_IRQn);   
            UARTx = UART4;
            break;
        case UART_5:
            RCC_PERCLK_SetableEx(UART5CLK, ENABLE);   //UARTx clk enable  
                        //  UART5 IO config
                        AltFunIO(UART5RX_Port, UART5RX_Pin, 0);     //UART5 RX
                        AltFunIO(UART5TX_Port, UART5TX_Pin, 0);     //UART5 TX
                        
                        /*NVIC interrupt config*/
                        NVIC_DisableIRQ(UART5_IRQn);
                        //NVIC_SetPriority(UART5_IRQn,2);//interrupt priority config
                        //NVIC_EnableIRQ(UART5_IRQn);   
            UARTx = UART5;
            break;
        default:
            break;
    }
    SerialBuffInit(uart->port,64,64);
    RCC_GetClocksFreq(&RCC_Clocks);//get APBClock
    memset(&UART_para,0,sizeof(UART_para));
    if (MODE_TX == uart->config.mode)
    {
        //UART_para.TXEN = ENABLE;
    }
    else if (MODE_RX == uart->config.mode)
    {
        UART_para.RXEN = ENABLE;
    }
    else
    {
        //UART_para.TXEN = ENABLE;
        UART_para.RXEN = ENABLE;
    }
    UART_para.RXIE = ENABLE;
    //UART_para.TXIE = ENABLE;
    UART_para.SPBRG = UART_BaudREGCalc(uart->config.baud_rate,RCC_Clocks.APBCLK_Frequency);
    UART_para.PDSEL = (uart->config.data_width - DATA_WIDTH_7BIT);
        
        if(Eight8Bit == uart->config.data_width-DATA_WIDTH_7BIT)
        {
                if(EVEN_PARITY == uart->config.parity)
                {
                        UART_para.PDSEL = UARTx_RXSTA_PDSEL_8BIT_EVEN;//8bit偶校验
                }
                else if(ODD_PARITY == uart->config.parity)
                {
                        UART_para.PDSEL = UARTx_RXSTA_PDSEL_8BIT_ODD;//8bit奇校验
                }
                else
                {
                        UART_para.PDSEL = UARTx_RXSTA_PDSEL_8BIT_NONE;//8bit数据无校验
                }
        }
        else if(Nine9Bit == uart->config.data_width-DATA_WIDTH_7BIT)
        {
                UART_para.PDSEL = UARTx_RXSTA_PDSEL_9BIT_NONE;//9bit数据仅支持无校验模式
        }
        else
        {
                UART_para.PDSEL = UARTx_RXSTA_PDSEL_8BIT_NONE;//7bit配置会直接覆盖其他配置
                UART_para.RTX7EN = ENABLE;//收发7bit数据使能(覆盖PDSEL)
        }
        
        if(STOP_BITS_2 == uart->config.stop_bits)
        {
                UART_para.STOPSEL = UARTx_TXSTA_STOPSEL_2STOPBIT;
        }
        else
        {
                UART_para.STOPSEL = UARTx_TXSTA_STOPSEL_1STOPBIT;
        }
        
        
    UART_Init(UARTx, &UART_para); //uart init
    
    NVIC_SetPriority ((IRQn_Type)Uart_Irqn[uart->port], 1);
    NVIC_EnableIRQ ((IRQn_Type)Uart_Irqn[uart->port]);
    return 0;
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart  the UART interface
 * @param[in]  data  pointer to the start of data
 * @param[in]  size  number of bytes to transmit
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t send_size = 0;
    uint32_t lastMs;
    uint8_t *buf = (uint8_t *)data;
    if(uart->port>UART_5)
    {
        return EIO;
    }
    lastMs = krhino_sys_time_get();
    lastMs -= 1000/aos_get_hz();
    while (send_size < size)
    {
        IWDT_Clr();             //feed dog
        krhino_intrpt_enter();
        send_size+=Serial_SvSend(uart->port,buf,size);
        krhino_intrpt_exit();
        if(krhino_sys_time_get()-lastMs>timeout)
        {
            return EIO;
        }
        if(SerialGetSendRemain(uart->port)==0)
        {
            if(send_size>=size)
            {
                return 0;
            }
        }
    }
    
    return 0;
}


/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    uint32_t recv_size = 0;
    uint32_t lastMs;
    uint8_t *buf = (uint8_t *)data;
    
    //param check
    if ((NULL == uart) ||
        (NULL == data))
    {
        return EIO;
    }
    if(uart->port>UART_5)
    {
        return EIO;
    }
        
    lastMs = krhino_sys_time_get();
    lastMs -= 1000/aos_get_hz();
    while (recv_size < expect_size)
    {
        
        IWDT_Clr();             //feed dog
        krhino_intrpt_enter();
        recv_size+= Serial_SvRecv(uart->port,buf,expect_size-recv_size);
        krhino_intrpt_exit();
        if(krhino_sys_time_get()-lastMs>timeout)
        {
            return EIO;
        }
    }
    
    return 0;
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                                            uint32_t *recv_size, uint32_t timeout)
{
    uint32_t recv_index = 0;
    uint32_t lastMs;
    uint8_t *buf = (uint8_t *)data;
    
    //param check
    if ((NULL == uart) ||
        (NULL == data))
    {
        return EIO;
    }
 if(uart->port>UART_5)
    {
        return EIO;
    }
    lastMs = krhino_sys_time_get();
    lastMs -= 1000/aos_get_hz();
    while (recv_index < expect_size)
    {
 
        IWDT_Clr();             //feed dog
        krhino_intrpt_enter();
        recv_index += Serial_SvRecv(uart->port,buf,expect_size-recv_index);
        krhino_intrpt_exit();
        if(krhino_sys_time_get()-lastMs>timeout)
        {
            if(recv_size!=NULL){
                *recv_size = recv_index;
            }
            return EIO;
        }
        
    }
    if(recv_size!=NULL){
        *recv_size = recv_index;
    }
    return 0;
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
 
    if (NULL == uart)
    {
        return EIO;
    }
    if(uart->port>UART_5)
    {
        return EIO;
    }
 
    UARTx_Deinit((UARTx_Type*)uartReg[uart->port]);
    SerialBuffFinalize(uart->port);
    return 0;
}

#endif /* HAL_UART_H */

