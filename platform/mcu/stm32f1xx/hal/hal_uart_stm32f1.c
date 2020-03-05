/**
  ******************************************************************************
  * @file    hal_uart_stm32f1.c
  * @author  MCU China FAE team
  * @version 1.0
  * @date    05/01/2019
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  ******************************************************************************
  */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/uart.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"

#include "hal_uart_stm32f1.h"

#ifdef HAL_UART_MODULE_ENABLED
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;

/* function used to transform hal para to stm32 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, uint32_t *data_width_stm32);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal, uint32_t *parity_stm32);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, uint32_t *stop_bits_stm32);
static int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal, uint32_t *flow_control_stm32);
static int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32);
static UART_HandleTypeDef * uart_get_handle(const uint8_t port);

/* function used to add buffer queue */
static PORT_UART_TYPE GetAppPortFromPhyInstanse(const void* uartIns);
static void UartIdleHandler(const USART_TypeDef* ins);
static int32_t uart_receive_start_it(PORT_UART_TYPE uart_port, uint32_t max_buffer_size);
static int32_t uart_receive_start_dma(PORT_UART_TYPE uart_port, uint32_t max_buffer_size);
static int32_t uart_send_it(PORT_UART_TYPE uart_port, const void *data, uint32_t size, uint32_t timeout);
static int32_t uart_send_dma(PORT_UART_TYPE uart_port, const void *data, uint32_t size, uint32_t timeout);
static int32_t uart_receive_it(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout);
static int32_t uart_receive_dma(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout);
static void UART_IT_RxCpltCallback(PORT_UART_TYPE appPort, uint32_t max_buffer_size);
static void UART_DMA_RxCpltCallback(PORT_UART_TYPE appPort, uint32_t max_buffer_size);

typedef struct {
    aos_mutex_t uart_tx_mutex;
    aos_mutex_t uart_rx_mutex;
    aos_sem_t   uart_rx_sem;
    aos_sem_t   uart_tx_sem;
    uint16_t    uart_rx_in;
    uint16_t    uart_rx_out;
    uint8_t     inited;
    UART_HandleTypeDef hal_uart_handle;
    char*       UartRxBuf;
    uint32_t    rxbuf_is_full;
    uint32_t    uart_error_count;
    uint32_t    uart_dma_stop;  //used only for error count now
    uint32_t    previous_dma_leftbyte;
}stm32_uart_t;

static stm32_uart_t stm32_uart[PORT_UART_SIZE];
uint32_t isrflags;

static void UartIRQProcessor(const USART_TypeDef* ins )
{
    const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(ins);
    if( appPort!=PORT_UART_INVALID ) {
        //deal with IDLE interrupt, HAL_UART_IRQHandler doesn't do it , we don't want to change HAL_UART_IRQHandler
        if( __HAL_UART_GET_FLAG(&stm32_uart[appPort].hal_uart_handle, UART_FLAG_IDLE)
            && __HAL_UART_GET_IT_SOURCE(&stm32_uart[appPort].hal_uart_handle, UART_IT_IDLE)
        )
        {
            UartIdleHandler(ins);
        }
        //deal with normal interrupt
        HAL_UART_IRQHandler(&stm32_uart[appPort].hal_uart_handle);
    }
}

void USART_DMA_RX_IRQHandler(const void* uartIns)
{
    const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(uartIns);
    if( appPort!=PORT_UART_INVALID )
    {
        HAL_DMA_IRQHandler(stm32_uart[appPort].hal_uart_handle.hdmarx);
    }
}

void USART_DMA_TX_IRQHandler(const void* uartIns)
{
    const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(uartIns);
    if( appPort!=PORT_UART_INVALID )
    {
        HAL_DMA_IRQHandler(stm32_uart[appPort].hal_uart_handle.hdmatx);
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
    UartIRQProcessor(USART1);
    krhino_intrpt_exit();
}

void USART2_IRQHandler(void)
{
    krhino_intrpt_enter();
    UartIRQProcessor(USART2);
    krhino_intrpt_exit();
}

void USART3_IRQHandler(void)
{
    krhino_intrpt_enter();
    UartIRQProcessor(USART3);
    krhino_intrpt_exit();
}

#ifdef UART4
void UART4_IRQHandler(void)
{
    krhino_intrpt_enter();
    UartIRQProcessor(UART4);
    krhino_intrpt_exit();
}
#endif

#ifdef UART5
void UART5_IRQHandler(void)
{
    krhino_intrpt_enter();
    UartIRQProcessor(UART5);
    krhino_intrpt_exit();
}
#endif

#ifdef USART6
void USART6_IRQHandler(void)
{
    krhino_intrpt_enter();
    UartIRQProcessor(USART6);
    krhino_intrpt_exit();
}
#endif

#ifdef UART7
void UART7_IRQHandler(void)
{
    krhino_intrpt_enter();
    UartIRQProcessor(UART7);
    krhino_intrpt_exit();
}
#endif


#if (HAL_VERSION >= 30100)
static int uart_gpio_init(UART_MAPPING* uartIns)
{
    GPIO_InitTypeDef        GPIO_InitStruct = {0};
    gpio_uart_pin_config_t *uart_pin_conf;
    GPIO_TypeDef           *GPIOx;
    uint8_t                 pin_count;
    uint8_t                 i;
    uint8_t                 pin;

    if (uartIns->uartPhyP == USART2) {
        /* Peripheral clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();
    }

    if (uartIns->uartPhyP == USART1) {
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }

    uart_pin_conf = uartIns->pin_conf;
    if (uart_pin_conf == NULL) {
        return -1;
    }
    pin_count = uartIns->pin_cnt;

    for (i = 0; i < pin_count; i++) {
        memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

        pin = uart_pin_conf[i].pin;

        GPIOx = hal_gpio_typedef(pin);
        if (NULL == GPIOx) {
           return -1;
        }

        hal_gpio_enable_clk(pin);

        if ((uartIns->uartPhyP == USART1) && (uart_pin_conf[i].pin_name == UART_RX)) {
            GPIO_InitStruct.Pin = hal_gpio_pin(pin);
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        } else {
            GPIO_InitStruct.Pin = hal_gpio_pin(pin);
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        }
        HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    }

    return 0;
}

static int uart_gpio_uninit(UART_MAPPING* uartIns)
{
    gpio_uart_pin_config_t *uart_pin_conf;
    GPIO_TypeDef           *GPIOx;
    uint8_t                 pin_count;
    uint8_t                 i;
    uint8_t                 pin;

    if (uartIns == NULL) {
        return -1;
    }

    if (uartIns->uartPhyP == USART2) {
        __HAL_RCC_USART2_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }

    if (uartIns->uartPhyP == USART1) {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }

    uart_pin_conf = uartIns->pin_conf;
    if (uart_pin_conf == NULL) {
        return -1;
    }
    pin_count = uartIns->pin_cnt;

    for (i = 0; i < pin_count; i++) {
        pin = uart_pin_conf[i].pin;

        GPIOx = hal_gpio_typedef(pin);
        if (NULL == GPIOx) {
           return -1;
        }

        HAL_GPIO_DeInit(GPIOx, pin);
    }

    return 0;
}

static int uart_dma_init(UART_MAPPING* uartIns)
{
    UART_HandleTypeDef *huart;

    if (uartIns == NULL) {
        return -1;
    }

    huart = &stm32_uart[uartIns->uartFuncP].hal_uart_handle;

    if (uartIns->uartPhyP == USART2) {
        /* USART2 DMA Init */
        /* USART2_TX Init */
        hdma_usart2_tx.Instance = DMA1_Channel7;
        hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart2_tx.Init.Mode = DMA_NORMAL;
        hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
        {
            Error_Handler();
            return -1;
        }

        __HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);

        /* USART2_RX Init */
        hdma_usart2_rx.Instance = DMA1_Channel6;
        hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart2_rx.Init.Mode = DMA_NORMAL;
        hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
        {
            Error_Handler();
            return -1;
        }

        __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }

    return 0;
}

static int uart_dma_uninit(UART_MAPPING* uartIns)
{
    UART_HandleTypeDef *huart;

    if (uartIns == NULL) {
        return -1;
    }

    huart = &stm32_uart[uartIns->uartFuncP].hal_uart_handle;

    if (uartIns->uartPhyP == USART2) {
        HAL_DMA_DeInit(huart->hdmatx);
        HAL_DMA_DeInit(huart->hdmarx);
    }
    return 0;
}
#endif

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;
    UART_HandleTypeDef *pstuarthandle = NULL;
    UART_MAPPING* uartIns = NULL;

    if (uart == NULL) {
        return -1;
    }

    //no found this port in function-physical uartIns, no need initialization
    uartIns = GetUARTMapping(uart->port);
    if( NULL== uartIns ) {
        return -1;
    }

#if (HAL_VERSION >= 30100)
    if (uart_gpio_init(uartIns) != 0) {
        return -1;
    }

    if (uart_dma_init(uartIns) != 0) {
        return -1;
    }
#endif

    memset(&stm32_uart[uart->port],0,sizeof(stm32_uart_t));

    pstuarthandle = &stm32_uart[uart->port].hal_uart_handle;
    pstuarthandle->Init.BaudRate = uart->config.baud_rate;
    ret = uart_dataWidth_transform(uart->config.data_width, &pstuarthandle->Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, &pstuarthandle->Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &pstuarthandle->Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, &pstuarthandle->Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, &pstuarthandle->Init.Mode);
    if (ret) {
        printf("invalid uart data \r\n");
        memset(pstuarthandle, 0, sizeof(*pstuarthandle));
        return -1;
    }

    if(NULL == stm32_uart[uart->port].UartRxBuf) {
        stm32_uart[uart->port].UartRxBuf = aos_malloc(uartIns->attr.max_buf_bytes);
    }

    if (NULL == stm32_uart[uart->port].UartRxBuf) {
        printf("Fail to malloc memory size %d at %s %d \r\d", uartIns->attr.max_buf_bytes, __FILE__, __LINE__);
        return -1;
    }
    memset(stm32_uart[uart->port].UartRxBuf, 0, uartIns->attr.max_buf_bytes);

    //uart->priv = pstuarthandle; //priv must not be used in uart driver

    pstuarthandle->Instance = (USART_TypeDef*)uartIns->uartPhyP;
    pstuarthandle->Init.OverSampling = uartIns->attr.overSampling;

    /* init uart */
    ret = HAL_UART_Init(pstuarthandle);
    if (ret != HAL_OK) {
        printf("uart %d init fail \r\n", uart->port);
        aos_free(stm32_uart[uart->port].UartRxBuf);
        stm32_uart[uart->port].UartRxBuf = NULL;
        return ret;
    }

    aos_mutex_new(&stm32_uart[uart->port].uart_tx_mutex);
    aos_mutex_new(&stm32_uart[uart->port].uart_rx_mutex);
    aos_sem_new(&stm32_uart[uart->port].uart_rx_sem, 0);
    aos_sem_new(&stm32_uart[uart->port].uart_tx_sem, 0);

    /* if UART Rx DMA Handle is NULL, then start data receive in interrupt mode
     * otherwise in DMA mode
     */
    if(pstuarthandle->hdmarx == NULL) {
        ret = uart_receive_start_it(uart->port,uartIns->attr.max_buf_bytes);
    }
    else {
        //init uart dma receive
        ret = uart_receive_start_dma(uart->port,uartIns->attr.max_buf_bytes);
    }

    if (ret) {
        aos_free(stm32_uart[uart->port].UartRxBuf);
        stm32_uart[uart->port].UartRxBuf = NULL;
        aos_mutex_free(&stm32_uart[uart->port].uart_tx_mutex);
        aos_mutex_free(&stm32_uart[uart->port].uart_rx_mutex);
        aos_sem_free(&stm32_uart[uart->port].uart_rx_sem);
        aos_sem_free(&stm32_uart[uart->port].uart_tx_sem);
    }
    return ret;
}

static int32_t uart_receive_start_it(PORT_UART_TYPE uart_port, uint32_t max_buffer_size)
{
    UART_HandleTypeDef *pstuarthandle = NULL;

    pstuarthandle = &stm32_uart[uart_port].hal_uart_handle;
    //HAL_UART_RxCpltCallback is called per 1 byte, to update uart_rx_in
    if(HAL_UART_Receive_IT(pstuarthandle,(uint8_t*)&stm32_uart[uart_port].UartRxBuf[0],1)!= HAL_OK)
    {
        return -1;
    }
    stm32_uart[uart_port].inited = 1;

    return 0;
}

static int32_t uart_receive_start_dma(PORT_UART_TYPE uart_port, uint32_t max_buffer_size)
{
    UART_HandleTypeDef *pstuarthandle = NULL;
    uint32_t temp_reg;

    pstuarthandle = &stm32_uart[uart_port].hal_uart_handle;

    //enable IDLE interrupt
    __HAL_UART_ENABLE_IT(pstuarthandle, UART_IT_IDLE);
    //clear the IDLE interrupt flag first
    __HAL_UART_CLEAR_IDLEFLAG(pstuarthandle);

    if(HAL_UART_Receive_DMA(pstuarthandle,(uint8_t*)&stm32_uart[uart_port].UartRxBuf[0], max_buffer_size/2) != HAL_OK)
    {
        return -1;
    }

    stm32_uart[uart_port].inited = 1;
    stm32_uart[uart_port].previous_dma_leftbyte = max_buffer_size/2;

    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = -1;
    UART_HandleTypeDef *handle = NULL;
    UART_MAPPING* uartIns = NULL;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    uartIns = GetUARTMapping(uart->port);
    if( NULL== uartIns ) {
        return -1;
    }

    handle = uart_get_handle(uart->port);
    if (handle == NULL) {
        return -1;
    }
    if(stm32_uart[uart->port].inited!=1)
    {
        return -1;
    }

    /* if  UART Tx DMA Handle is NULL, then start data send in interrupt mode
     * otherwise in DMA mode
     */
    if(handle->hdmatx ==NULL) {
        ret = uart_send_it(uart->port,data, size, timeout);
    }
    else {
        ret = uart_send_dma(uart->port,data, size, timeout);
    }

    return ret;
}

static int32_t uart_send_it(PORT_UART_TYPE uart_port, const void *data, uint32_t size, uint32_t timeout)
{
    HAL_StatusTypeDef sendRlt = HAL_BUSY;

    aos_mutex_lock(&stm32_uart[uart_port].uart_tx_mutex, AOS_WAIT_FOREVER);
    sendRlt = HAL_UART_Transmit_IT(&(stm32_uart[uart_port].hal_uart_handle), (uint8_t *)data, size);
    aos_sem_wait(&stm32_uart[uart_port].uart_tx_sem, timeout);
    aos_mutex_unlock(&stm32_uart[uart_port].uart_tx_mutex);

    return (sendRlt==HAL_OK)?0:-1;
}

static int32_t uart_send_dma(PORT_UART_TYPE uart_port, const void *data, uint32_t size, uint32_t timeout)
{
    HAL_StatusTypeDef sendRlt = HAL_BUSY;

    aos_mutex_lock(&stm32_uart[uart_port].uart_tx_mutex, AOS_WAIT_FOREVER);

    sendRlt = HAL_UART_Transmit_DMA(&(stm32_uart[uart_port].hal_uart_handle), (uint8_t *)data, size);
    //If failed to start Uart DMA transmit
    if (sendRlt == HAL_OK)
    {
        /*wait for the end of transfer*/
        aos_sem_wait(&stm32_uart[uart_port].uart_tx_sem, timeout);
    }
    aos_mutex_unlock(&stm32_uart[uart_port].uart_tx_mutex);
    return (sendRlt==HAL_OK)?0:-1;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    int32_t ret = -1;
    UART_MAPPING* uartIns = NULL;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    uartIns = GetUARTMapping(uart->port);
    if(NULL== uartIns) {
        return -1;
    }

    if (aos_mutex_lock(&stm32_uart[uart->port].uart_rx_mutex, timeout)) {
        printf("uart port % recv fail to get mutex \r\n", uart->port);
        return -1;
    }

    if(stm32_uart[uart->port].hal_uart_handle.hdmarx == NULL) {
        ret = uart_receive_it(uart, data, expect_size, recv_size, timeout);
    }
    else {
        ret = uart_receive_dma(uart, data, expect_size, recv_size, timeout);
    }

    aos_mutex_unlock(&stm32_uart[uart->port].uart_rx_mutex);
    return ret;
}

static int32_t uart_receive_it(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    UART_MAPPING* uartIns = GetUARTMapping(uart->port);

    while (rx_count < expect_size)
    {
        while(stm32_uart[uart->port].uart_rx_out != stm32_uart[uart->port].uart_rx_in)
        {
            pdata[rx_count++] = stm32_uart[uart->port].UartRxBuf[stm32_uart[uart->port].uart_rx_out++];
            if(stm32_uart[uart->port].uart_rx_out == uartIns->attr.max_buf_bytes)
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
                if(stm32_uart[uart->port].uart_rx_out == uartIns->attr.max_buf_bytes)
                {
                    stm32_uart[uart->port].uart_rx_out = 0;
                }

                if(rx_count == expect_size)
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

    return ret;
}

static int32_t uart_receive_dma(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    uint32_t cr1its = 0;
    uint32_t dmaisr = 0;

    UART_MAPPING* uartIns = GetUARTMapping(uart->port);

    while (rx_count < expect_size)
    {
        while((stm32_uart[uart->port].uart_rx_out != stm32_uart[uart->port].uart_rx_in) || stm32_uart[uart->port].rxbuf_is_full)
        {
            stm32_uart[uart->port].rxbuf_is_full = 0;
            pdata[rx_count++] = stm32_uart[uart->port].UartRxBuf[stm32_uart[uart->port].uart_rx_out++];

            if(stm32_uart[uart->port].uart_rx_out == uartIns->attr.max_buf_bytes)
            {
                stm32_uart[uart->port].uart_rx_out = 0;
            }

            if(rx_count == expect_size)
            {
                break;
            }
        }

        if(rx_count == expect_size)
        {
            break;
        }

        if(RHINO_SUCCESS == aos_sem_wait(&stm32_uart[uart->port].uart_rx_sem, timeout))
        {
            while(stm32_uart[uart->port].uart_rx_out != stm32_uart[uart->port].uart_rx_in)
            {
                pdata[rx_count++] = stm32_uart[uart->port].UartRxBuf[stm32_uart[uart->port].uart_rx_out++];
                if(stm32_uart[uart->port].uart_rx_out == uartIns->attr.max_buf_bytes)
                {
                    stm32_uart[uart->port].uart_rx_out = 0;
                }

                if(rx_count == expect_size)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }

        /* check DMA status
         * make sure DMA is active when buffer has space to receive new data
         * if(stm32_uart[uart->port].uart_dma_stop == 1)
         * EN bit in CR register will be cleared by hardware when a DMA end of transfer
         */
        cr1its = READ_REG(stm32_uart[uart->port].hal_uart_handle.hdmarx->Instance->CCR);
        if(cr1its & DMA_CCR_EN == RESET)
        {
            if((stm32_uart[uart->port].uart_rx_in == 0 && stm32_uart[uart->port].uart_rx_out>=(uartIns->attr.max_buf_bytes)/2)
                ||((stm32_uart[uart->port].uart_rx_in == (uartIns->attr.max_buf_bytes)/2 )&& (stm32_uart[uart->port].uart_rx_out <=(uartIns->attr.max_buf_bytes)/2))
            ||((stm32_uart[uart->port].uart_rx_in == stm32_uart[uart->port].uart_rx_out)&&!stm32_uart[uart->port].rxbuf_is_full))
            {
                if(HAL_UART_Receive_DMA(&(stm32_uart[uart->port].hal_uart_handle),(uint8_t*)&stm32_uart[uart->port].UartRxBuf[stm32_uart[uart->port].uart_rx_in],(uartIns->attr.max_buf_bytes)/2)!= HAL_OK)
                {
                    Error_Handler();
                }
                stm32_uart[uart->port].uart_dma_stop = 0;
                stm32_uart[uart->port].previous_dma_leftbyte = (uartIns->attr.max_buf_bytes)/2;
            }
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
        cr1its = READ_REG(stm32_uart[uart->port].hal_uart_handle.hdmarx->Instance->CCR);
        printf("uart_rx_in %d uart_rx_out %d rxbuf_is_full %d uart_dma_stop %d \r\n",
            stm32_uart[uart->port].uart_rx_in, stm32_uart[uart->port].uart_rx_out,
            stm32_uart[uart->port].rxbuf_is_full, stm32_uart[uart->port].uart_dma_stop);
        dmaisr = READ_REG(stm32_uart[uart->port].hal_uart_handle.hdmarx->DmaBaseAddress->ISR);
        printf("cr1its 0x%x dma1 isr 0x%x \r\n", cr1its, dmaisr);
        ret = -1;
    }

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = -1;
    UART_MAPPING* uartIns = NULL;

    if (uart == NULL) {
        return -1;
    }

    uartIns = GetUARTMapping(uart->port);
    if( NULL== uartIns ){
        return -1;
    }

#if (HAL_VERSION >= 30100)
    if (uart_gpio_uninit(uartIns) != 0) {
        return -1;
    }

    if (uart_dma_uninit(uartIns) != 0) {
        return -1;
    }
#endif

    if (stm32_uart[uart->port].inited == 0) {
        return -1;
    }

    ret = HAL_UART_DeInit(&stm32_uart[uart->port].hal_uart_handle);
    if(NULL != stm32_uart[uart->port].UartRxBuf){
        free(stm32_uart[uart->port].UartRxBuf);
        stm32_uart[uart->port].UartRxBuf = NULL;
    }

    if (aos_sem_is_valid(&stm32_uart[uart->port].uart_tx_sem)) {
        aos_sem_free(&stm32_uart[uart->port].uart_tx_sem);
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

    stm32_uart[uart->port].inited = 0;
    return ret;
}

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
        uint32_t *data_width_stm32)
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
        *data_width_stm32 = data_width;
    }

    return ret;
}

int32_t uart_parity_transform(hal_uart_parity_t parity_hal,
        uint32_t *parity_stm32)
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
        *parity_stm32 = parity;
    }

    return ret;
}

int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal,
        uint32_t *stop_bits_stm32)
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
        *stop_bits_stm32 = stop_bits;
    }

    return ret;
}

int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal,
        uint32_t *flow_control_stm32)
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
        *flow_control_stm32 = flow_control;
    }

    return ret;
}

int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32)
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
        *mode_stm32 = mode;
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
    if (port < PORT_UART_SIZE)
    {
        handle = &stm32_uart[port].hal_uart_handle;
    }
    else
    {
        handle = NULL;
    }
    return handle;
}

/**
  * @brief  Tx Transfer completed callback
  * @param  huart: UART handle.
  * @note   This example shows a simple way to report end of DMA Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(huart->Instance);
    aos_sem_signal(&stm32_uart[appPort].uart_tx_sem);
}

/**
  * @brief  Rx Transfer completed callbacks. It will be called from UART_DMAReceiveCplt
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(huart->Instance);
    UART_MAPPING* uartIns = GetUARTMapping(appPort);

    //check if it is called from UART_DMAReceiveCplt
    if(stm32_uart[appPort].hal_uart_handle.hdmarx != NULL)
    {
        UART_DMA_RxCpltCallback(appPort, uartIns->attr.max_buf_bytes);
    }
    else
    {
        UART_IT_RxCpltCallback(appPort, uartIns->attr.max_buf_bytes);
    }
    aos_sem_signal(&stm32_uart[appPort].uart_rx_sem);
}

static void UART_IT_RxCpltCallback(PORT_UART_TYPE appPort, uint32_t max_buffer_size)
{
    //TODO:
    //should take care to avoid UART_RX_IN get accross UART_RX_OUT, which will loss unread data
    //if receive buffer is too small, or application read data out of receive buffer at very low frequence
    //this situation may happen
    if(++stm32_uart[appPort].uart_rx_in >= max_buffer_size)
    {
        stm32_uart[appPort].uart_rx_in = 0;
    }
    HAL_UART_Receive_IT(&stm32_uart[appPort].hal_uart_handle, (uint8_t*)&stm32_uart[appPort].UartRxBuf[stm32_uart[appPort].uart_rx_in], 1);
}

static void UART_DMA_RxCpltCallback(PORT_UART_TYPE appPort, uint32_t max_buffer_size)
{
    UART_HandleTypeDef huart = stm32_uart[appPort].hal_uart_handle;
    //switch DMA Destination to another buffer region
    if(huart.hdmarx->Instance->CMAR == (uint32_t)&stm32_uart[appPort].UartRxBuf[0])
    {
        if(stm32_uart[appPort].uart_rx_out < max_buffer_size/2)
        {
            HAL_UART_Receive_DMA(&huart,(uint8_t*)&stm32_uart[appPort].UartRxBuf[max_buffer_size/2], max_buffer_size/2);
            stm32_uart[appPort].uart_dma_stop = 0;
            stm32_uart[appPort].previous_dma_leftbyte = max_buffer_size/2;
        } else {
            stm32_uart[appPort].uart_dma_stop = 1;
        }

        stm32_uart[appPort].uart_rx_in = (uint16_t)max_buffer_size/2;
        if(stm32_uart[appPort].uart_rx_in == stm32_uart[appPort].uart_rx_out)
            stm32_uart[appPort].rxbuf_is_full = 1;
    } else {
        if(stm32_uart[appPort].uart_rx_out >= max_buffer_size/2)
        {
            HAL_UART_Receive_DMA(&huart,(uint8_t*)&stm32_uart[appPort].UartRxBuf[0],max_buffer_size/2);
            stm32_uart[appPort].uart_dma_stop = 0;
            stm32_uart[appPort].uart_rx_in = 0;
            stm32_uart[appPort].previous_dma_leftbyte = max_buffer_size/2;
        } else {
            stm32_uart[appPort].uart_dma_stop = 1;
            stm32_uart[appPort].uart_rx_in = 0;//in case uart_rx_in = uart_rx_out, should take in account of "rxbuf_is_full" to identify if buffer is full
            if(stm32_uart[appPort].uart_rx_in == stm32_uart[appPort].uart_rx_out)
            {
			   stm32_uart[appPort].rxbuf_is_full = 1;
            }
        }
    }
}

/**
  * @brief  Rx IDLE callbacks.
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_IdleCallback(UART_HandleTypeDef *huart)
{
    uint32_t left_byte;
    const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(huart->Instance);
    UART_MAPPING* uartIns = GetUARTMapping(appPort);

    if(stm32_uart[appPort].uart_dma_stop)
    {
        if(stm32_uart[appPort].uart_error_count < 0xffffffff)
        {
            stm32_uart[appPort].uart_error_count++;
        }
    }
    else
    {
        left_byte = __HAL_DMA_GET_COUNTER(huart->hdmarx);
        /* if left_byte=0, means DMA transfer complete interrupt maybe has happened
         * uart_rx_in will be update in DMA TC interrupt
         * don't do it repeatedly
         */
        if(left_byte < stm32_uart[appPort].previous_dma_leftbyte && left_byte != 0)
        {
            stm32_uart[appPort].uart_rx_in += stm32_uart[appPort].previous_dma_leftbyte - left_byte;
            if(stm32_uart[appPort].uart_rx_in == uartIns->attr.max_buf_bytes)
            {
                stm32_uart[appPort].uart_rx_in = 0;
            }

            stm32_uart[appPort].previous_dma_leftbyte = left_byte;

            if(stm32_uart[appPort].uart_rx_in == stm32_uart[appPort].uart_rx_out)
            {
                stm32_uart[appPort].rxbuf_is_full = 1;
            }
        }
    }
    aos_sem_signal(&stm32_uart[appPort].uart_rx_sem);
}

static void UartIdleHandler(const USART_TypeDef* ins)
{
    UART_HandleTypeDef* huart_handle;
    const PORT_UART_TYPE appPort = GetAppPortFromPhyInstanse(ins);
    huart_handle = &(stm32_uart[appPort].hal_uart_handle);

    //clear IDLE idle interrupt flag
    __HAL_UART_CLEAR_IDLEFLAG(huart_handle);
    HAL_UART_IdleCallback(huart_handle);
}
#endif
