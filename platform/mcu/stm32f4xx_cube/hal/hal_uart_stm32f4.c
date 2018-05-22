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

/* Init function for uart1 */
static int32_t uart1_init(uart_dev_t *uart);
static int32_t uart2_init(uart_dev_t *uart);

/* function used to transform hal para to stm32l4 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, uint32_t *data_width_stm32l4);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal, uint32_t *parity_stm32l4);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, uint32_t *stop_bits_stm32l4);
static int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal, uint32_t *flow_control_stm32l4);
static int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4);
static UART_HandleTypeDef * uart_get_handle(uint8_t port);

/* function used to add buffer queue */
static void HAL_UART_IRQHandler_Buf_Queue(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef HAL_UART_Receive_IT_Buf_Queue_1byte(UART_HandleTypeDef *huart, uint8_t *pData, uint32_t timeout);

/* handle for uart */
UART_HandleTypeDef hal_uart_handle[PORT_UART_MAX_NUM];
/* bufferQueue for uart */
kbuf_queue_t g_buf_queue_uart[PORT_UART_MAX_NUM];
char *g_pc_buf_queue_uart[PORT_UART_MAX_NUM] = {0};

void USART1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler_Buf_Queue(&hal_uart_handle[PORT_UART1]);
   krhino_intrpt_exit();
}

void USART2_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler_Buf_Queue(&hal_uart_handle[PORT_UART2]);
   krhino_intrpt_exit();
}

void USART3_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler_Buf_Queue(&hal_uart_handle[PORT_UART3]);
   krhino_intrpt_exit();
}

void UART4_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler_Buf_Queue(&hal_uart_handle[PORT_UART4]);
   krhino_intrpt_exit();
}

void UART5_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler_Buf_Queue(&hal_uart_handle[PORT_UART5]);
   krhino_intrpt_exit();
}

void LPUART1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler_Buf_Queue(&hal_uart_handle[PORT_UART6]);
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
            //pstuarthandle->Init.OneBitSampling         = UART1_ONE_BIT_SAMPLING;
            //pstuarthandle->AdvancedInit.AdvFeatureInit = UART1_ADV_FEATURE_INIT;

            UART1_TX_GPIO_CLK_ENABLE();
            UART1_RX_GPIO_CLK_ENABLE();
            UART1_CLK_ENABLE();

            HAL_NVIC_SetPriority(UART1_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(UART1_IRQn);
        break;
        case PORT_UART2:
            pstuarthandle->Instance = UART2;
            pstuarthandle->Init.OverSampling           = UART2_OVER_SAMPLING;
            //pstuarthandle->Init.OneBitSampling         = UART2_ONE_BIT_SAMPLING;
            //pstuarthandle->AdvancedInit.AdvFeatureInit = UART2_ADV_FEATURE_INIT;

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
            //pstuarthandle->Init.OneBitSampling         = UART3_ONE_BIT_SAMPLING;
            //pstuarthandle->AdvancedInit.AdvFeatureInit = UART3_ADV_FEATURE_INIT;

            UART3_TX_GPIO_CLK_ENABLE();
            UART3_RX_GPIO_CLK_ENABLE();
            UART3_CLK_ENABLE();

            HAL_NVIC_SetPriority(UART3_IRQn, 0, 1);
            HAL_NVIC_EnableIRQ(UART3_IRQn);
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
    UART_HandleTypeDef *handle = NULL;
    int ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    handle = uart_get_handle(uart->port);
    if (handle == NULL) {
        return -1;
    }

    ret = HAL_UART_Transmit(handle, (uint8_t *)data, size, 30000);

    return ret;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    UART_HandleTypeDef *handle = NULL;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    handle = uart_get_handle(uart->port);
    if (handle == NULL) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = HAL_UART_Receive_IT_Buf_Queue_1byte(handle, &pdata[i], timeout);
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

UART_HandleTypeDef * uart_get_handle(uint8_t port)
{
    UART_HandleTypeDef *handle = NULL;
    int32_t ret = 0;

    if (port < PORT_UART_MAX_NUM) {
        handle = &hal_uart_handle[port];
    } else {
        handle = NULL;
    }

    return handle;
}

/**
  * @brief  Receives an amount of data in non blocking mode 
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive_IT_Buf_Queue_1byte(UART_HandleTypeDef *huart, uint8_t *pData, uint32_t timeout)
{
    size_t rev_size = 0;
    int ret = HAL_OK;
    kbuf_queue_t *pBuffer_queue = NULL;

    /* Check that a Rx process is not already ongoing */ 
    if(huart->RxState == HAL_UART_STATE_READY)
    {
        if((pData == NULL )) 
        {
            return HAL_ERROR;
        }
    
        /* Process Locked */
        __HAL_LOCK(huart);
        
        huart->RxXferCount = 1;
        
        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->RxState = HAL_UART_STATE_BUSY_RX;
        
        /* Process Unlocked */
        __HAL_UNLOCK(huart);
            
        /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

        /* Enable the UART Parity Error and Data Register not empty Interrupts */
        SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
        
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

        return ret;
    }
    else
    {
        return HAL_BUSY; 
    }
}

/**
  * @brief  Receives an amount of data in non blocking mode 
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Receive_IT_Buf_Queue(UART_HandleTypeDef *huart)
{
  uint16_t tmp;
  uint8_t data; 
  kbuf_queue_t *pBuffer_queue = NULL;
  int ret = 0;

  /* Check that a Rx process is ongoing */
  if(huart->RxState == HAL_UART_STATE_BUSY_RX) 
  {

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
        if(huart->Init.WordLength == UART_WORDLENGTH_9B)
        {
          if(huart->Init.Parity == UART_PARITY_NONE)
          {
            tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
            krhino_buf_queue_send(pBuffer_queue, &tmp, 2);
          }
          else
          {
            tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
            data = (uint8_t)tmp;
            krhino_buf_queue_send(pBuffer_queue, &data, 1);
          }
        }
        else
        {
          if(huart->Init.Parity == UART_PARITY_NONE)
          {
            data = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
          }
          else
          {
            data = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
          }
          krhino_buf_queue_send(pBuffer_queue, &data, 1);
        }
    }

    if(--huart->RxXferCount == 0U)
    {
      /* Disable the UART Parity Error Interrupt and RXNE interrupt*/
      CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;
     
      HAL_UART_RxCpltCallback(huart);

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/**
  * @brief  Sends an amount of data in non blocking mode.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
  uint16_t* tmp;
  
  /* Check that a Tx process is ongoing */
  if(huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    if(huart->Init.WordLength == UART_WORDLENGTH_9B)
    {
      tmp = (uint16_t*) huart->pTxBuffPtr;
      huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        huart->pTxBuffPtr += 2U;
      }
      else
      {
        huart->pTxBuffPtr += 1U;
      }
    } 
    else
    {
      huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

    if(--huart->TxXferCount == 0U)
    {
      /* Disable the UART Transmit Complete Interrupt */
      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE);

      /* Enable the UART Transmit Complete Interrupt */    
      SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Wraps up transmission in non blocking mode.
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
  /* Disable the UART Transmit Complete Interrupt */    
  CLEAR_BIT(huart->Instance->CR1, USART_CR1_TCIE);
  
  /* Tx process is ended, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;
    
  HAL_UART_TxCpltCallback(huart);
  
  return HAL_OK;
}

/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart UART handle.
  * @retval None
  */
static void UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
}

/**
  * @brief  DMA UART communication abort callback, when initiated by HAL services on Error
  *         (To be called at end of DMA Abort procedure following error occurrence).
  * @param  hdma DMA handle.
  * @retval None
  */
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  huart->RxXferCount = 0U;
  huart->TxXferCount = 0U;

  HAL_UART_ErrorCallback(huart);
}


/**
  * @brief  This function handles UART interrupt request.
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_IRQHandler_Buf_Queue(UART_HandleTypeDef *huart)
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
      UART_Receive_IT_Buf_Queue(huart);
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

    /* Call UART Error Call back function if need be --------------------------*/    
    if(huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver -----------------------------------------------*/
      if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        UART_Receive_IT_Buf_Queue(huart);
      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
      if(((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) || dmarequest)
      {
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
        UART_EndRxTransfer(huart);
        
        /* Disable the UART DMA Rx request if enabled */
        if(HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
        {
          CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
          
          /* Abort the UART DMA Rx channel */
          if(huart->hdmarx != NULL)
          {
            /* Set the UART DMA Abort callback : 
               will lead to call HAL_UART_ErrorCallback() at end of DMA abort procedure */
            huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;
            if(HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
            {
              /* Call Directly XferAbortCallback function in case of error */
              huart->hdmarx->XferAbortCallback(huart->hdmarx);
            }
          }
          else
          {
            /* Call user error callback */
            HAL_UART_ErrorCallback(huart);
          }
        }
        else
        {
          /* Call user error callback */
          HAL_UART_ErrorCallback(huart);
        }
      }
      else
      {
        /* Non Blocking error : transfer could go on. 
           Error is notified to user through user error callback */
        HAL_UART_ErrorCallback(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;
  } /* End if some error occurs */

  /* UART in mode Transmitter ------------------------------------------------*/
  if(((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    UART_Transmit_IT(huart);
    return;
  }
  
  /* UART in mode Transmitter end --------------------------------------------*/
  if(((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    UART_EndTransmit_IT(huart);
    return;
  }
}
#endif
