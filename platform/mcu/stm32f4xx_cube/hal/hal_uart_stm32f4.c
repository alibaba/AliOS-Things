/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "hal_uart_stm32f4.h"

#ifdef HAL_UART_MODULE_ENABLED


/* function used to transform hal para to stm32l4 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, uint32_t *data_width_stm32l4);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal, uint32_t *parity_stm32l4);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, uint32_t *stop_bits_stm32l4);
static int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal, uint32_t *flow_control_stm32l4);
static int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4);
static UART_HandleTypeDef * uart_get_handle(const uint8_t port);

/* function used to add buffer queue */
static PORT_UART_TYPE GetAppPortFromPhyInstanse(const void* uartIns);


typedef struct {
  aos_mutex_t uart_tx_mutex;
  aos_mutex_t uart_rx_mutex;
  aos_sem_t uart_rx_sem;
  uint16_t  uart_rx_in;
  uint16_t  uart_rx_out;
  uint8_t   inited;
  UART_HandleTypeDef hal_uart_handle;
  char*      UartRxBuf;
}stm32_uart_t;

static stm32_uart_t stm32_uart[PORT_UART_SIZE];

static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
  uint16_t* tmp;    
  
  /* Check that a Rx process is ongoing */
  if(huart->RxState == HAL_UART_STATE_BUSY_RX) 
  {
    uint8_t data = 0xFF;
    if(huart->Init.WordLength == UART_WORDLENGTH_8B)
    {
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        data = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);      
      }
      else
      {
        data = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);       
      }
        const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(huart->Instance);
    	if( appPort!=PORT_UART_INVALID )
    	{
            if(stm32_uart[appPort].inited!=0)
            {
                stm32_uart[appPort].UartRxBuf[stm32_uart[appPort].uart_rx_in++] = data;
                if(stm32_uart[appPort].uart_rx_in==MAX_BUF_UART_BYTES)
                {
                    stm32_uart[appPort].uart_rx_in=0;
                }
                aos_sem_signal(&stm32_uart[appPort].uart_rx_sem);            
            }

    	}   
    }
 

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


void HAL_UART_IRQHandler_User(UART_HandleTypeDef *huart)
{
   uint32_t isrflags   = READ_REG(huart->Instance->SR);
   uint32_t cr1its     = READ_REG(huart->Instance->CR1);
   uint32_t cr3its     = READ_REG(huart->Instance->CR3);
   uint32_t errorflags = 0x00U;
   uint32_t dmarequest = 0x00U;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if(errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(huart);
      return;
    }
  }  
    /* If some errors occur */
  if((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {
    /* UART parity error interrupt occurred ----------------------------------*/
    if(((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }
    
    /* UART noise error interrupt occurred -----------------------------------*/
    if(((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }
    
    /* UART frame error interrupt occurred -----------------------------------*/
    if(((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }
    
    /* UART Over-Run interrupt occurred --------------------------------------*/
    if(((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    { 
      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }
    return;
  } /* End if some error occurs */

}

    
//USART3->Logical Port
static void SaveUartBuf( const USART_TypeDef* ins )
{
   const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(ins);
   if( appPort!=PORT_UART_INVALID )
   {
     HAL_UART_IRQHandler_User(&stm32_uart[appPort].hal_uart_handle);
   }
}

//Get UART Instanse & attribute from Logical Port
static UART_MAPPING* GetUARTMapping(const PORT_UART_TYPE port)
{
    int8_t i = 0;
    UART_MAPPING* rc = NULL;
    for(i=0; i<PORT_UART_SIZE; i++)
    {
        if(UART_MAPPING_TABLE[i].uartFuncP == port)
        {
            rc = &UART_MAPPING_TABLE[i];
            break;
        }
    }
    return rc;
}

static PORT_UART_TYPE GetAppPortFromPhyInstanse(const void* uartIns)
{
    PORT_UART_TYPE rc = PORT_UART_INVALID;
    int8_t i = 0;
    for(i; i<PORT_UART_SIZE; i++)
    {
        if( (USART_TypeDef*)UART_MAPPING_TABLE[i].uartPhyP == (USART_TypeDef*)uartIns)
        {
            rc = UART_MAPPING_TABLE[i].uartFuncP;
            break;
        }
    }
    return rc;
}

void USART1_IRQHandler(void)
{
   krhino_intrpt_enter();
   SaveUartBuf(USART1);
   
   krhino_intrpt_exit();
}

void USART2_IRQHandler(void)
{
   krhino_intrpt_enter();
   SaveUartBuf(USART2);   
   krhino_intrpt_exit();
}

void USART3_IRQHandler(void)
{
   krhino_intrpt_enter();   
   SaveUartBuf(USART3); 
   krhino_intrpt_exit();
}

void UART4_IRQHandler(void)
{
   krhino_intrpt_enter();
   SaveUartBuf(UART4); 
   krhino_intrpt_exit();
}

void UART5_IRQHandler(void)
{
   krhino_intrpt_enter();
   SaveUartBuf(UART5);
   krhino_intrpt_exit();
}

void USART6_IRQHandler(void)
{
   krhino_intrpt_enter();
   SaveUartBuf(USART6);   
   krhino_intrpt_exit();
}

void UART7_IRQHandler(void)
{
   krhino_intrpt_enter();
   SaveUartBuf(UART7);   
   krhino_intrpt_exit();
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;
    UART_HandleTypeDef *pstuarthandle = NULL;

    if (uart == NULL) {
        return -1;
    }
    UART_MAPPING* uartIns = GetUARTMapping(uart->port);
    if( NULL== uartIns ){ //no found this port in function-physical uartIns, no need initialization
        return -1;
    }
    memset(&stm32_uart[uart->port],0,sizeof(stm32_uart_t));

    pstuarthandle = &stm32_uart[uart->port].hal_uart_handle;
    pstuarthandle->Init.BaudRate               = uart->config.baud_rate;
    ret = uart_dataWidth_transform(uart->config.data_width, &pstuarthandle->Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, &pstuarthandle->Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &pstuarthandle->Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, &pstuarthandle->Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, &pstuarthandle->Init.Mode);
    if (ret) {
        printf("invalid uart data \r\n");
        memset(pstuarthandle, 0, sizeof(*pstuarthandle));
    }    

    if(NULL == stm32_uart[uart->port].UartRxBuf){
        stm32_uart[uart->port].UartRxBuf = aos_malloc(MAX_BUF_UART_BYTES);
    }
    
    if (NULL == stm32_uart[uart->port].UartRxBuf) {
        printf("Fail to malloc memory size %d at %s %d \r\d", MAX_BUF_UART_BYTES, __FILE__, __LINE__);
        return -1;
    }
    memset(stm32_uart[uart->port].UartRxBuf, 0, MAX_BUF_UART_BYTES);

    uart->priv = pstuarthandle;

    pstuarthandle->Instance = (USART_TypeDef*)uartIns->uartPhyP;
    pstuarthandle->Init.OverSampling = uartIns->attr.overSampling;
    
    HAL_NVIC_SetPriority(uartIns->attr.irqNumber, uartIns->attr.pPri, uartIns->attr.sPri);
    HAL_NVIC_EnableIRQ(uartIns->attr.irqNumber);

    /* init uart */
    HAL_UART_Init(pstuarthandle);    

    aos_mutex_new(&stm32_uart[uart->port].uart_tx_mutex);
    aos_mutex_new(&stm32_uart[uart->port].uart_rx_mutex);
    aos_sem_new(&stm32_uart[uart->port].uart_rx_sem, 0);
    if(HAL_UART_Receive_IT(pstuarthandle,&stm32_uart[uart->port].UartRxBuf,MAX_BUF_UART_BYTES)!= HAL_OK)
    {
       Error_Handler(); 
    }
    stm32_uart[uart->port].inited = 1;
    return ret;
}


int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    UART_HandleTypeDef *handle = NULL;
    int ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }    

    handle = uart_get_handle(uart->port);
    if (handle == NULL) {
        return -1;
    }
    if(stm32_uart[uart->port].inited!=1)
    {
        int a = stm32_uart[uart->port].inited;
        return -1;
    }

    uint8_t retry = 0;
    HAL_StatusTypeDef sendRlt = HAL_BUSY;
    aos_mutex_lock(&stm32_uart[uart->port].uart_tx_mutex, AOS_WAIT_FOREVER);
    do
    {
        sendRlt = HAL_UART_Transmit(handle, (uint8_t *)data, size, 30000);
        if(HAL_BUSY != sendRlt)
        {
            break;
        }
        aos_msleep(10);
    }while(retry++<3);
    aos_mutex_unlock(&stm32_uart[uart->port].uart_tx_mutex);

    return (sendRlt==HAL_OK)?0:-1;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;  
    uint32_t rx_count = 0;
    int32_t ret = -1;
    
    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    aos_mutex_lock(&stm32_uart[uart->port].uart_rx_mutex, AOS_WAIT_FOREVER); 

    while ( rx_count < expect_size )
    {
        while(stm32_uart[uart->port].uart_rx_out != stm32_uart[uart->port].uart_rx_in)
        {
            pdata[rx_count++] = stm32_uart[uart->port].UartRxBuf[stm32_uart[uart->port].uart_rx_out++];
            if(stm32_uart[uart->port].uart_rx_out == MAX_BUF_UART_BYTES)
            {
                stm32_uart[uart->port].uart_rx_out = 0;
            }
        
            if(rx_count==expect_size)
            {
                break;
            }
        }
        if(rx_count==expect_size)
        {
            break;
        }

        if(RHINO_SUCCESS==aos_sem_wait(&stm32_uart[uart->port].uart_rx_sem, timeout))
        {
            while(stm32_uart[uart->port].uart_rx_out != stm32_uart[uart->port].uart_rx_in)
            {
                pdata[rx_count++] = stm32_uart[uart->port].UartRxBuf[stm32_uart[uart->port].uart_rx_out++];
                if(stm32_uart[uart->port].uart_rx_out == MAX_BUF_UART_BYTES)
                {
                    stm32_uart[uart->port].uart_rx_out = 0;
                }

                if(rx_count==expect_size)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }

    if (recv_size != NULL)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    aos_mutex_unlock(&stm32_uart[uart->port].uart_rx_mutex);

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) {
        return -1;
    }

    ret = HAL_UART_DeInit(&stm32_uart[uart->port].hal_uart_handle);
    if(NULL != stm32_uart[uart->port].UartRxBuf){
        free(stm32_uart[uart->port].UartRxBuf);
        stm32_uart[uart->port].UartRxBuf = NULL;
    }

    if (aos_mutex_is_valid(&stm32_uart[uart->port].uart_tx_mutex)) {
        aos_mutex_free(&stm32_uart[uart->port].uart_tx_mutex);
    }

    if (aos_sem_is_valid(&stm32_uart[uart->port].uart_rx_sem)) {
        aos_sem_free(&stm32_uart[uart->port].uart_rx_sem);
    }

    if (aos_mutex_is_valid(&stm32_uart[uart->port].uart_rx_mutex)) {
        aos_mutex_free(&stm32_uart[uart->port].uart_rx_mutex);
    }


    return ret;
}

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
        uint32_t *data_width_stm32l4)
{
    uint32_t data_width = 0;
    int32_t ret = 0;

    if(data_width_hal == DATA_WIDTH_8BIT)
    {
        data_width = UART_WORDLENGTH_8B;
    }
    else if(data_width_hal == DATA_WIDTH_9BIT)
    {
        data_width = UART_WORDLENGTH_9B;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *data_width_stm32l4 = data_width;
    }

    return ret;
}

int32_t uart_parity_transform(hal_uart_parity_t parity_hal,
        uint32_t *parity_stm32l4)
{
    uint32_t parity = 0;
    int32_t ret = 0;

    if(parity_hal == NO_PARITY)
    {
        parity = UART_PARITY_NONE;
    }
    else if(parity_hal == ODD_PARITY)
    {
        parity = UART_PARITY_EVEN;
    }
    else if(parity_hal == EVEN_PARITY)
    {
        parity = UART_PARITY_ODD;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *parity_stm32l4 = parity;
    }

    return ret;
}

int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal,
        uint32_t *stop_bits_stm32l4)
{
    uint32_t stop_bits = 0;
    int32_t ret = 0;

    if(stop_bits_hal == STOP_BITS_1)
    {
        stop_bits = UART_STOPBITS_1;
    }
    else if(stop_bits_hal == STOP_BITS_2)
    {
        stop_bits = UART_STOPBITS_2;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *stop_bits_stm32l4 = stop_bits;
    }

    return ret;
}

int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal,
        uint32_t *flow_control_stm32l4)
{
    uint32_t flow_control = 0;
    int32_t ret = 0;

    if(flow_control_hal == FLOW_CONTROL_DISABLED)
    {
        flow_control = UART_HWCONTROL_NONE;
    }
    else if(flow_control_hal == FLOW_CONTROL_CTS)
    {
        flow_control = UART_HWCONTROL_CTS;
    }
    else if(flow_control_hal == FLOW_CONTROL_RTS)
    {
        flow_control = UART_HWCONTROL_RTS;
    }
    else if(flow_control_hal == FLOW_CONTROL_RTS)
    {
        flow_control = UART_HWCONTROL_RTS_CTS;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *flow_control_stm32l4 = flow_control;
    }

    return ret;
}

int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4)
{
    uint32_t mode = 0;
    int32_t ret = 0;

    if(mode_hal == MODE_TX)
    {
        mode = UART_MODE_TX;
    }
    else if(mode_hal == MODE_RX)
    {
        mode = UART_MODE_RX;
    }
    else if(mode_hal == MODE_TX_RX)
    {
        mode = UART_MODE_TX_RX;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *mode_stm32l4 = mode;
    }

    return ret;
}

/**
  * @brief  Get UART Handler
  * @param  port Logical UART Port, e.g. STD UART is always 0
  * @retval UART Handler
  */
UART_HandleTypeDef * uart_get_handle(const uint8_t port)
{
    UART_HandleTypeDef *handle = NULL;
    int32_t ret = 0;

    if (port < PORT_UART_SIZE) {
        handle = &stm32_uart[port].hal_uart_handle;
    } else {
        handle = NULL;
    }

    return handle;
}

#endif
