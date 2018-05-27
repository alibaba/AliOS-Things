/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "hal_uart_stm32l4.h"

#ifdef HAL_UART_MODULE_ENABLED

typedef enum
{
  PORT_UART1 = 1,       /*USART1*/
  PORT_UART2,           /*USART2*/
  PORT_UART3,           /*USART3*/
  PORT_UART4,           /*UART4*/
  PORT_UART5,           /*UART5*/
  PORT_UART6,           /*LPUART1*/
  PORT_UART_MAX_NUM,
} ENUM_UART_PORT_NUM;


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
UART_HandleTypeDef hal_uart_handle[PORT_UART_MAX_NUM];
/* bufferQueue for uart */
kbuf_queue_t g_buf_queue_uart[PORT_UART_MAX_NUM];
char *g_pc_buf_queue_uart[PORT_UART_MAX_NUM] = {0};

void USART1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART1]);
   krhino_intrpt_exit();
}

void USART2_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART2]);
   krhino_intrpt_exit();
}

void USART3_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART3]);
   krhino_intrpt_exit();
}

void UART4_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART4]);
   krhino_intrpt_exit();
}

void UART5_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART5]);
   krhino_intrpt_exit();
}

void LPUART1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART6]);
   krhino_intrpt_exit();
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;
    UART_HandleTypeDef *pstuarthandle = NULL;

    if (uart == NULL) {
        return -1;
    }

    pstuarthandle = &hal_uart_handle[uart->port];
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

    g_pc_buf_queue_uart[uart->port] = aos_malloc(MAX_BUF_UART_BYTES);
    if (NULL == g_pc_buf_queue_uart[uart->port]) {
        printf("fail to malloc memory size %d at %s %d \r\d", MAX_BUF_UART_BYTES, __FILE__, __LINE__);
        return -1;
    }
    memset(g_pc_buf_queue_uart[uart->port], 0, MAX_BUF_UART_BYTES);

    ret = krhino_buf_queue_create(&g_buf_queue_uart[uart->port], "buf_queue_uart",
          g_pc_buf_queue_uart[uart->port], MAX_BUF_UART_BYTES, 1);

    uart->priv = pstuarthandle;

    switch (uart->port) {
        case PORT_UART1:
            pstuarthandle->Instance = UART1;
            pstuarthandle->Init.OverSampling           = UART1_OVER_SAMPLING;
            pstuarthandle->Init.OneBitSampling         = UART1_ONE_BIT_SAMPLING;
            pstuarthandle->AdvancedInit.AdvFeatureInit = UART1_ADV_FEATURE_INIT;

            UART1_TX_GPIO_CLK_ENABLE();
            UART1_RX_GPIO_CLK_ENABLE();
            UART1_CLK_ENABLE();

            HAL_NVIC_SetPriority(UART1_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(UART1_IRQn);
        break;
        case PORT_UART2:
            pstuarthandle->Instance = UART2;
            pstuarthandle->Init.OverSampling           = UART2_OVER_SAMPLING;
            pstuarthandle->Init.OneBitSampling         = UART2_ONE_BIT_SAMPLING;
            pstuarthandle->AdvancedInit.AdvFeatureInit = UART2_ADV_FEATURE_INIT;

            UART2_TX_GPIO_CLK_ENABLE();
            UART2_RX_GPIO_CLK_ENABLE();
            UART2_CLK_ENABLE();

            HAL_NVIC_SetPriority(UART2_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(UART2_IRQn);
        break;
#if defined(UART3)
        case PORT_UART3:
            pstuarthandle->Instance = UART3;
            pstuarthandle->Init.OverSampling           = UART3_OVER_SAMPLING;
            pstuarthandle->Init.OneBitSampling         = UART3_ONE_BIT_SAMPLING;
            pstuarthandle->AdvancedInit.AdvFeatureInit = UART3_ADV_FEATURE_INIT;

            UART3_TX_GPIO_CLK_ENABLE();
            UART3_RX_GPIO_CLK_ENABLE();
            UART3_CLK_ENABLE();

            HAL_NVIC_SetPriority(UART3_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(UART3_IRQn);
        break;
#endif
#if defined(UART4)
        case PORT_UART4:
            pstuarthandle->Instance = UART4;
            pstuarthandle->Init.OverSampling           = UART4_OVER_SAMPLING;
            pstuarthandle->Init.OneBitSampling         = UART4_ONE_BIT_SAMPLING;
            pstuarthandle->AdvancedInit.AdvFeatureInit = UART4_ADV_FEATURE_INIT;

            UART4_TX_GPIO_CLK_ENABLE();
            UART4_RX_GPIO_CLK_ENABLE();
            UART4_CLK_ENABLE();

            HAL_NVIC_SetPriority(UART4_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(UART4_IRQn);
        break;
#endif
#if defined(UART5)
        case PORT_UART5:
            pstuarthandle->Instance = UART5;
            pstuarthandle->Init.OverSampling           = UART5_OVER_SAMPLING;
            pstuarthandle->Init.OneBitSampling         = UART5_ONE_BIT_SAMPLING;
            pstuarthandle->AdvancedInit.AdvFeatureInit = UART5_ADV_FEATURE_INIT;

            UART5_TX_GPIO_CLK_ENABLE();
            UART5_RX_GPIO_CLK_ENABLE();
            UART5_CLK_ENABLE();

            HAL_NVIC_SetPriority(UART5_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(UART5_IRQn);
        break;
#endif
#if defined(UART6)
        case PORT_UART6:
            pstuarthandle->Instance = UART6;
            pstuarthandle->Init.OverSampling           = UART6_OVER_SAMPLING;
            pstuarthandle->Init.OneBitSampling         = UART6_ONE_BIT_SAMPLING;
            pstuarthandle->AdvancedInit.AdvFeatureInit = UART6_ADV_FEATURE_INIT;

            UART6_TX_GPIO_CLK_ENABLE();
            UART6_RX_GPIO_CLK_ENABLE();
            UART6_CLK_ENABLE();

            HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(LPUART1_IRQn);
        break;
#endif
        default :
            printf("uart %d invalid\r\n", uart->port);
        return -1;
    }

    /* init uart */
    HAL_UART_Init(pstuarthandle);

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

    return HAL_UART_DeInit(&hal_uart_handle[uart->port]);;
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
    ret = HAL_OK;
    if (huart->Instance == UART1) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART1];
    }
    else if (huart->Instance == UART2) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART2];
    }
#if defined(UART3)
    else if (huart->Instance == UART3) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART3];
    }
#endif
#if defined(UART4)
    else if (huart->Instance == UART4) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART4];
    }
#endif
#if defined(UART5)
    else if (huart->Instance == UART5) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART5];
    }
#endif
#if defined(UART6)
    else if (huart->Instance == UART6) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART6];
    }
#endif
    else {
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

    ret = HAL_OK;
    if (huart->Instance == UART1) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART1];
    } else if (huart->Instance == UART2) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART2];
    }
#if defined(UART3)
    else if (huart->Instance == UART3) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART3];
    }
#endif
#if defined(UART4)
    else if (huart->Instance == UART4) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART4];
    }
#endif
#if defined(UART5)
    else if (huart->Instance == UART5) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART5];
    }
#endif
#if defined(UART6)
    else if (huart->Instance == UART6) {
        pBuffer_queue = &g_buf_queue_uart[PORT_UART6];
    }
#endif
    else {
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
