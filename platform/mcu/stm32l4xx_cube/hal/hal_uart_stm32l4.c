/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "stm32l4xx_hal.h"
#include "hal_uart_stm32l4.h"

#ifdef HAL_UART_MODULE_ENABLED 

/* Init function for uart1 */
static int32_t uart1_init(uart_dev_t *uart);
static int32_t uart2_init(uart_dev_t *uart);

/* function used to transform hal para to stm32l4 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, uint32_t *data_width_stm32l4);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal, uint32_t *parity_stm32l4);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, uint32_t *stop_bits_stm32l4);
static int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal, uint32_t *flow_control_stm32l4);
static int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4);

/* function used to add buffer queue */
static void UART_RxISR_8BIT_Buf_Queue(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef HAL_UART_Receive_IT_Buf_Queue_1byte(UART_HandleTypeDef *huart, uint8_t *pData, uint32_t timeout);

/* handle for uart */
UART_HandleTypeDef uart1_handle;
UART_HandleTypeDef uart2_handle;

/* bufferQueue for uart */
kbuf_queue_t g_buf_queue_uart1;
char g_buf_uart1[MAX_BUF_UART_BYTES];
kbuf_queue_t g_buf_queue_uart2;
char g_buf_uart2[MAX_BUF_UART_BYTES];

void USART1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&uart1_handle);
   krhino_intrpt_exit();
}

void USART2_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&uart2_handle);
   krhino_intrpt_exit();
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case PORT_UART1:
            uart->priv = &uart1_handle;
            ret = uart1_init(uart);
            break;
        case PORT_UART2:
            uart->priv = &uart2_handle;
            ret = uart2_init(uart);
        break;
        /* if ohter uart exist add init code here */
        default:
        break;
    }

    return ret;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    ret = HAL_UART_Transmit((UART_HandleTypeDef *)uart->priv, (uint8_t *)data, size, 30000);

    return ret;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = HAL_UART_Receive_IT_Buf_Queue_1byte((UART_HandleTypeDef *)uart->priv, &pdata[i], timeout); 
        if (ret == 0) {
            rx_count++;
        } else {
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

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case PORT_UART1:
            ret = HAL_UART_DeInit(&uart1_handle);
            break;
        case PORT_UART2:
            ret = HAL_UART_DeInit(&uart2_handle);
            break;
        /* if other uart exist add Deinit code here */
        default:
        break;
    }

    return ret;
}

int32_t uart1_init(uart_dev_t *uart)
{
    int32_t ret = 0;

    uart1_handle.Instance                    = UART1;
    uart1_handle.Init.BaudRate               = uart->config.baud_rate;

    ret = uart_dataWidth_transform(uart->config.data_width, &uart1_handle.Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, &uart1_handle.Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &uart1_handle.Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, &uart1_handle.Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, &uart1_handle.Init.Mode);

    if (ret != 0) {
        return -1;
    }

    uart1_handle.Init.OverSampling           = UART1_OVER_SAMPLING;
    uart1_handle.Init.OneBitSampling         = UART1_ONE_BIT_SAMPLING;
    uart1_handle.AdvancedInit.AdvFeatureInit = UART1_ADV_FEATURE_INIT;

    /* Enable GPIO clock */
    UART1_TX_GPIO_CLK_ENABLE();
    UART1_RX_GPIO_CLK_ENABLE();

    /* Enable USART clock */
    UART1_CLK_ENABLE();

    /* init uart */
    HAL_UART_Init(&uart1_handle);
    
    HAL_NVIC_SetPriority(UART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(UART1_IRQn);

    ret = krhino_buf_queue_create(&g_buf_queue_uart1, "buf_queue_uart",
          g_buf_uart1, MAX_BUF_UART_BYTES, 1);

    return ret;
}

int32_t uart2_init(uart_dev_t *uart)
{
    int32_t ret = 0;

    uart2_handle.Instance                    = UART2;
    uart2_handle.Init.BaudRate               = uart->config.baud_rate;

    ret = uart_dataWidth_transform(uart->config.data_width, &uart2_handle.Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, &uart2_handle.Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &uart2_handle.Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, &uart2_handle.Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, &uart2_handle.Init.Mode);

    if (ret != 0) {
        return -1;
    }

    uart2_handle.Init.OverSampling           = UART2_OVER_SAMPLING;
    uart2_handle.Init.OneBitSampling         = UART2_ONE_BIT_SAMPLING;
    uart2_handle.AdvancedInit.AdvFeatureInit = UART2_ADV_FEATURE_INIT;

    /* Enable GPIO clock */
    UART2_TX_GPIO_CLK_ENABLE();
    UART2_RX_GPIO_CLK_ENABLE();

    /* Enable USART clock */
    UART2_CLK_ENABLE();
    /* init uart */
    HAL_UART_Init(&uart2_handle);

    HAL_NVIC_SetPriority(UART2_IRQn, 0,1);
    HAL_NVIC_EnableIRQ(UART2_IRQn);

    ret = krhino_buf_queue_create(&g_buf_queue_uart2, "buf_queue_uart",
          g_buf_uart2, MAX_BUF_UART_BYTES, 1);

    return ret;
}

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
        uint32_t *data_width_stm32l4)
{
    uint32_t data_width = 0;
    int32_t ret = 0;

    if(data_width_hal == DATA_WIDTH_7BIT)
    {
        data_width = UART_WORDLENGTH_7B;
    }
    else if(data_width_hal == DATA_WIDTH_8BIT)
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
  * @brief Receive an amount of data in interrupt mode with buffer queue.
  * @param huart UART handle.
  * @param pData Pointer to data buffer.
  * @param Size  Amount of data to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive_IT_Buf_Queue_1byte(UART_HandleTypeDef *huart, uint8_t *pData, uint32_t timeout)
{
  size_t rev_size = 0;
  int ret = 0;
    kbuf_queue_t *pBuffer_queue = NULL;

  /* Check that a Rx process is not already ongoing */
  if(huart->RxState == HAL_UART_STATE_READY)
  {
    if(pData == NULL)
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->pRxBuffPtr  = pData;
    huart->RxXferSize  = 1;
    huart->RxXferCount = 0xFFFF;
    huart->RxISR       = NULL;

    /* Computation of UART mask to apply to RDR register */
    UART_MASK_COMPUTATION(huart);

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

#if defined(USART_CR1_FIFOEN)
    /* Configure Rx interrupt processing*/
    if ((huart->FifoMode == UART_FIFOMODE_ENABLE) && (Size >= huart->NbRxDataToProcess))
    {
      /* Set the Rx ISR function pointer according to the data word length */
      if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
      {
        huart->RxISR = UART_RxISR_16BIT_FIFOEN;
      }
      else
      {
        huart->RxISR = UART_RxISR_8BIT_FIFOEN;
      }

      /* Process Unlocked */
      __HAL_UNLOCK(huart);
  
      /* Enable the UART Parity Error interrupt and RX FIFO Threshold interrupt */
      SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
      SET_BIT(huart->Instance->CR3, USART_CR3_RXFTIE);
    }
    else
#endif
    {
      /* Set the Rx ISR function pointer according to the data word length */
      if ((huart->Init.WordLength != UART_WORDLENGTH_9B) || (huart->Init.Parity != UART_PARITY_NONE))
      {
        huart->RxISR = UART_RxISR_8BIT_Buf_Queue;
      }

       /* Process Unlocked */
      __HAL_UNLOCK(huart);

     /* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
#if defined(USART_CR1_FIFOEN)
      SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
#else
      SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
#endif
    }
  }

    if (huart->Instance == UART1) 
    {
        pBuffer_queue = &g_buf_queue_uart1;
        ret = HAL_OK;
    }
    else if (huart->Instance == UART2)
    {
        pBuffer_queue = &g_buf_queue_uart2;
        ret = HAL_OK;
    }
    else
    {
      ret = HAL_ERROR;
    }

  if (ret == HAL_OK)
  {
    if(timeout != HAL_MAX_DELAY)
    {
      ret = krhino_buf_queue_recv(pBuffer_queue, timeout, pData, &rev_size);
    }
    else
    {
      ret = krhino_buf_queue_recv(pBuffer_queue, RHINO_WAIT_FOREVER, pData, &rev_size);
    }

    if((ret == 0) && (rev_size == 1))
    {
      ret = HAL_OK;
    }
    else
    {
      ret = HAL_BUSY;
    }
  }

  return (HAL_StatusTypeDef)ret;
}

/**
  * @brief RX interrrupt handler for 7 or 8 bits data word length with buffer queue .
  * @param huart UART handle.
  * @retval None
  */
static void UART_RxISR_8BIT_Buf_Queue(UART_HandleTypeDef *huart)
{
  uint16_t uhMask = huart->Mask;
  uint16_t  uhdata;
  uint8_t data;
  kbuf_queue_t *pBuffer_queue = NULL;
  int32_t ret = -1;

  /* Check that a Rx process is ongoing */
  if(huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
    data = (uint8_t)(uhdata & (uint8_t)uhMask);

    if (huart->Instance == UART1) 
    {
        pBuffer_queue = &g_buf_queue_uart1;
        ret = HAL_OK;
    }
    else if (huart->Instance == UART2)
    {
        pBuffer_queue = &g_buf_queue_uart2;
        ret = HAL_OK;
    }
    else
    {
      ret = HAL_ERROR;
    }

    if (ret == HAL_OK)
    {
        krhino_buf_queue_send(pBuffer_queue, &data, 1);
    }

    if(--huart->RxXferCount == 0)
    {
      /* Disable the UART Parity Error Interrupt and RXNE interrupt*/
#if defined(USART_CR1_FIFOEN)
      CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
#else
      CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
#endif

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

      /* Clear RxISR function pointer */
      huart->RxISR = NULL;

      HAL_UART_RxCpltCallback(huart);
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}
#endif
