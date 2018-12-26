/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
/**
 ******************************************************************************
 * @file    hal_uart_stm32f4.c
 * @author  MCU China FAE team
 * @version 1.0
 * @date    26/12/2018
 * @brief   aos porting layer
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 ******************************************************************************
 */
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "hal_uart_stm32f4.h"
#include <aos/kernel.h>

#ifdef HAL_UART_MODULE_ENABLED

/* Init function for uart1 */
static int32_t uart1_init(uart_dev_t *uart);
static int32_t uart2_init(uart_dev_t *uart);

/* function used to transform hal para to stm32l4 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
                                        uint32_t *data_width_stm32l4);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal,
                                     uint32_t *parity_stm32l4);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal,
                                        uint32_t *stop_bits_stm32l4);
static int32_t
uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal,
                            uint32_t *flow_control_stm32l4);
static int32_t uart_mode_transform(hal_uart_mode_t mode_hal,
                                   uint32_t *mode_stm32l4);
static UART_HandleTypeDef *uart_get_handle(uint8_t port);
static DMA_HandleTypeDef *dma_get_rx_handle(uint8_t port);

/* handle for uart */
UART_HandleTypeDef hal_uart_handle[PORT_UART_MAX_NUM];

typedef struct {
  char buf[MAX_BUF_UART_BYTES]; /*For DMA*/
  int cursor;                   /*For DMA*/
  uint32_t rx_in;               /*For DMA*/
  uint32_t rx_in_plus;          /*For DMA*/
  uint32_t rx_process;          /*For DMA*/
  int32_t dma_start;
  aos_sem_t uart_tx_sem;
  aos_mutex_t uart_tx_mutex;
  aos_mutex_t uart_rx_mutex;
} stm32_uart_t;

stm32_uart_t stm32_uart[PORT_UART_MAX_NUM];

extern DMA_HandleTypeDef hdma_usart3_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef *hal_dma_rx_handle[PORT_UART_MAX_NUM];

void USART3_IRQHandler(void) {
  krhino_intrpt_enter();
  HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART3]);
  krhino_intrpt_exit();
}

void USART6_IRQHandler(void) {
  krhino_intrpt_enter();
  HAL_UART_IRQHandler(&hal_uart_handle[PORT_UART6]);
  krhino_intrpt_exit();
}

int32_t hal_uart_init(uart_dev_t *uart) {
  int32_t ret = -1;
  UART_HandleTypeDef *pstuarthandle = NULL;

  if (uart == NULL) {
    return -1;
  }

  pstuarthandle = &hal_uart_handle[uart->port];
  pstuarthandle->Init.BaudRate = uart->config.baud_rate;
  ret = uart_dataWidth_transform(uart->config.data_width,
                                 &pstuarthandle->Init.WordLength);
  ret |=
      uart_parity_transform(uart->config.parity, &pstuarthandle->Init.Parity);
  ret |= uart_stop_bits_transform(uart->config.stop_bits,
                                  &pstuarthandle->Init.StopBits);
  ret |= uart_flow_control_transform(uart->config.flow_control,
                                     &pstuarthandle->Init.HwFlowCtl);
  ret |= uart_mode_transform(uart->config.mode, &pstuarthandle->Init.Mode);
  if (ret) {
    printf("invalid uart data \r\n");
    memset(pstuarthandle, 0, sizeof(*pstuarthandle));
  }

  uart->priv = pstuarthandle;

  switch (uart->port) {
  case PORT_UART3:
    pstuarthandle->Instance = UART3;
    pstuarthandle->Init.OverSampling = UART3_OVER_SAMPLING;

    UART3_TX_GPIO_CLK_ENABLE();
    UART3_RX_GPIO_CLK_ENABLE();
    UART3_CLK_ENABLE();

    HAL_NVIC_SetPriority(UART3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(UART3_IRQn);
    hal_dma_rx_handle[PORT_UART3] = &hdma_usart3_rx;
    break;
  case PORT_UART6:
    pstuarthandle->Instance = UART6;
    pstuarthandle->Init.OverSampling = UART6_OVER_SAMPLING;

    UART6_TX_GPIO_CLK_ENABLE();
    UART6_RX_GPIO_CLK_ENABLE();
    UART6_CLK_ENABLE();

    HAL_NVIC_SetPriority(UART6_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(UART6_IRQn);
    hal_dma_rx_handle[PORT_UART6] = &hdma_usart6_rx;
    break;
  default:
    printf("uart %d invalid\r\n", uart->port);
    return -1;
  }

  /* init uart */
  HAL_UART_Init(pstuarthandle);

  stm32_uart[uart->port].cursor = 0;
  stm32_uart[uart->port].rx_in = 0;
  stm32_uart[uart->port].rx_in_plus = 0;
  stm32_uart[uart->port].rx_process = 0;
  ret = aos_sem_new(&stm32_uart[uart->port].uart_tx_sem, 0);
  if (ret != 0) {
    printf("Failed to allocate sem\n"); /*Caution: Here printf may not outpu*/
  }

  ret = aos_mutex_new(&stm32_uart[uart->port].uart_tx_mutex);
  if (ret != 0) {
    printf("Failed to allocate mutex\n"); /*Caution: Here printf may not outpu*/
  }
  ret = aos_mutex_new(&stm32_uart[uart->port].uart_rx_mutex);
  if (ret != 0) {
    printf("Failed to allocate mutex\n"); /*Caution: Here printf may not outpu*/
  }

  HAL_UART_Receive_DMA(pstuarthandle, stm32_uart[uart->port].buf,
                       MAX_BUF_UART_BYTES);
  stm32_uart[uart->port].dma_start = 1;

  return ret;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size,
                      uint32_t timeout) {
  UART_HandleTypeDef *handle = NULL;
  int ret = -1;

  if ((uart == NULL) || (data == NULL)) {
    return -1;
  }

  handle = uart_get_handle(uart->port);
  if (handle == NULL) {
    return -1;
  }

  aos_mutex_lock(&stm32_uart[uart->port].uart_tx_mutex, AOS_WAIT_FOREVER);
  ret = HAL_UART_Transmit_DMA(handle, (uint8_t *)data, size);
  if (ret == HAL_OK) {
    ret = aos_sem_wait(&stm32_uart[uart->port].uart_tx_sem, AOS_WAIT_FOREVER);
  }
  aos_mutex_free(&stm32_uart[uart->port].uart_tx_mutex);

  return ret;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout) {
  uint8_t *pdata = (uint8_t *)data;

  int i = 0;
  int32_t rx_count = 0;
  int32_t ret = -1;

  DMA_HandleTypeDef *handle = NULL;
  uint32_t delay = 0;
  uint32_t wrapped = 0;
  uint32_t cursor = 0;

  if ((uart == NULL) || (data == NULL)) {
    return -1;
  }

  handle = dma_get_rx_handle(uart->port);
  if (handle == NULL) {
    return -1;
  }

  cursor = stm32_uart[uart->port].cursor;

  /*Timeout processing*/
  do {
    uint32_t empty = __HAL_DMA_GET_COUNTER(handle);

    rx_count = (MAX_BUF_UART_BYTES - empty - cursor);
    if (rx_count < 0) {
      wrapped = 1;
      rx_count += MAX_BUF_UART_BYTES;
    } else {
      wrapped = 0;
    }

    if (expect_size <= rx_count) {
      rx_count = expect_size;
      /*recacluate status of wrapping*/
      if (cursor + rx_count <= MAX_BUF_UART_BYTES) {
        wrapped = 0;
      }
      break;
    } else {
      i = i + 10;
      aos_msleep(10);
    }
  } while (i < timeout);

  /*If the buffer is not wrapped, copy it directly*/
  if (wrapped == 0) {
    memcpy((char *)data, stm32_uart[uart->port].buf + cursor, rx_count);
    stm32_uart[uart->port].cursor += rx_count;
  } else {
    /*If the buffer is wrapped, we need to copy it piece by piece*/

    int len1 = MAX_BUF_UART_BYTES - cursor; /*Length before wrapping back*/
    int len2 = rx_count - len1;             /*length after wrapping back*/
    memcpy((char *)data, stm32_uart[uart->port].buf + cursor, len1);
    memcpy((char *)data + len1, stm32_uart[uart->port].buf, len2);

    stm32_uart[uart->port].cursor = len2;
  }
  stm32_uart[uart->port].rx_in_plus =
      MAX_BUF_UART_BYTES - __HAL_DMA_GET_COUNTER(handle);

  if (recv_size != NULL) {
    *recv_size = rx_count;
  }

  /*Used for potential diagnosing*/
  stm32_uart[uart->port].rx_process += rx_count;

  if (rx_count != 0) {
    ret = 0;
  } else {
    ret = -1;
  }

  return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart) {
  int32_t ret = -1;

  if (uart == NULL) {
    return -1;
  }

  return HAL_UART_DeInit(&hal_uart_handle[uart->port]);
  ;
}

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
                                 uint32_t *data_width_stm32l4) {
  uint32_t data_width = 0;
  int32_t ret = 0;

  if (data_width_hal == DATA_WIDTH_8BIT) {
    data_width = UART_WORDLENGTH_8B;
  } else if (data_width_hal == DATA_WIDTH_9BIT) {
    data_width = UART_WORDLENGTH_9B;
  } else {
    ret = -1;
  }

  if (ret == 0) {
    *data_width_stm32l4 = data_width;
  }

  return ret;
}

int32_t uart_parity_transform(hal_uart_parity_t parity_hal,
                              uint32_t *parity_stm32l4) {
  uint32_t parity = 0;
  int32_t ret = 0;

  if (parity_hal == NO_PARITY) {
    parity = UART_PARITY_NONE;
  } else if (parity_hal == ODD_PARITY) {
    parity = UART_PARITY_EVEN;
  } else if (parity_hal == EVEN_PARITY) {
    parity = UART_PARITY_ODD;
  } else {
    ret = -1;
  }

  if (ret == 0) {
    *parity_stm32l4 = parity;
  }

  return ret;
}

int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal,
                                 uint32_t *stop_bits_stm32l4) {
  uint32_t stop_bits = 0;
  int32_t ret = 0;

  if (stop_bits_hal == STOP_BITS_1) {
    stop_bits = UART_STOPBITS_1;
  } else if (stop_bits_hal == STOP_BITS_2) {
    stop_bits = UART_STOPBITS_2;
  } else {
    ret = -1;
  }

  if (ret == 0) {
    *stop_bits_stm32l4 = stop_bits;
  }

  return ret;
}

int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal,
                                    uint32_t *flow_control_stm32l4) {
  uint32_t flow_control = 0;
  int32_t ret = 0;

  if (flow_control_hal == FLOW_CONTROL_DISABLED) {
    flow_control = UART_HWCONTROL_NONE;
  } else if (flow_control_hal == FLOW_CONTROL_CTS) {
    flow_control = UART_HWCONTROL_CTS;
  } else if (flow_control_hal == FLOW_CONTROL_RTS) {
    flow_control = UART_HWCONTROL_RTS;
  } else if (flow_control_hal == FLOW_CONTROL_RTS) {
    flow_control = UART_HWCONTROL_RTS_CTS;
  } else {
    ret = -1;
  }

  if (ret == 0) {
    *flow_control_stm32l4 = flow_control;
  }

  return ret;
}

int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4) {
  uint32_t mode = 0;
  int32_t ret = 0;

  if (mode_hal == MODE_TX) {
    mode = UART_MODE_TX;
  } else if (mode_hal == MODE_RX) {
    mode = UART_MODE_RX;
  } else if (mode_hal == MODE_TX_RX) {
    mode = UART_MODE_TX_RX;
  } else {
    ret = -1;
  }

  if (ret == 0) {
    *mode_stm32l4 = mode;
  }

  return ret;
}

UART_HandleTypeDef *uart_get_handle(uint8_t port) {
  UART_HandleTypeDef *handle = NULL;
  int32_t ret = 0;

  if (port < PORT_UART_MAX_NUM) {
    handle = &hal_uart_handle[port];
  } else {
    handle = NULL;
  }

  return handle;
}

static DMA_HandleTypeDef *dma_get_rx_handle(uint8_t port) {
  DMA_HandleTypeDef *handle = NULL;
  int32_t ret = 0;

  if (port < PORT_UART_MAX_NUM) {
    handle = hal_dma_rx_handle[port];
  } else {
    handle = NULL;
  }

  return handle;
}

/**
 * @brief  DMA conversion error callback
 * @note   This function is executed when the transfer error interrupt
 *         is generated during DMA transfer
 * @retval None
 */
static void USART3_TransferError(DMA_HandleTypeDef *DmaHandle) {
  aos_sem_signal(&stm32_uart[PORT_UART3].uart_tx_sem);
}

/**
 * @brief  DMA UART communication abort callback, when initiated by HAL services
 * on Error (To be called at end of DMA Abort procedure following error
 * occurrence).
 * @param  hdma DMA handle.
 * @retval None
 */
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma) {
  UART_HandleTypeDef *huart =
      (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  huart->RxXferCount = 0U;
  huart->TxXferCount = 0U;

  HAL_UART_ErrorCallback(huart);
}

/**
 * @brief This function handles DMA1 stream1 global interrupt.
 */
void DMA1_Stream1_IRQHandler(void) {
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}
/**
 * @brief This function handles DMA1 stream3 global interrupt.
 */
void DMA1_Stream3_IRQHandler(void) {
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA2_Stream1_IRQHandler(void) {
  /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */

  /* USER CODE END DMA2_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart6_rx);
  /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

  /* USER CODE END DMA2_Stream1_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream6 global interrupt.
 */
void DMA2_Stream6_IRQHandler(void) {
  /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

  /* USER CODE END DMA2_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart6_tx);
  /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

  /* USER CODE END DMA2_Stream6_IRQn 1 */
}

/**
 * @brief Tx Transfer completed callback.
 * @param huart: UART handle.
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  for (int i = 0; i < PORT_UART_MAX_NUM; i++) {
    if (&hal_uart_handle[i] == huart) {
      aos_sem_signal(&stm32_uart[i].uart_tx_sem);
      break;
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  for (int i = 0; i < PORT_UART_MAX_NUM; i++) {
    if (&hal_uart_handle[i] == huart) {
      HAL_UART_Receive_DMA(huart, stm32_uart[i].buf, MAX_BUF_UART_BYTES);
      stm32_uart[i].rx_in += MAX_BUF_UART_BYTES;
      break;
    }
  }
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {}

#endif
