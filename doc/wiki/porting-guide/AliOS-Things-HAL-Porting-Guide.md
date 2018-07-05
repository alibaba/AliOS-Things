EN| [中文](AliOS-Things-HAL-Porting-Guide.zh) 

# Content

  * [1 Porting of HAL](#1硬件抽象层移植)
  * [2 Porting of flash HAL](#2flash抽象层对接)
---

## 1 porting of HAL
Hardware Abstraction Layer (HAL) widely exists in various operating systems, whose main purpose is to shield the difference between different chip platforms, so that applications can be the same in different chips. AliOS Things defines a comprehensive HAL, and you can control the chip once you connect to its corresponding HAL interface, so as to control the hardware peripherals.

### Hardwares defined in AliOS Things HAL are:

| 1    | adc   |
| ---- | ----- |
| 2    | flash |
| 3    | gpio  |
| 4    | i2c   |
| 5    | nand  |
| 6    | nor   |
| 7    | pwm   |
| 8    | rng   |
| 9    | rtc   |
| 10   | sd    |
| 11   | spi   |
| 12   | timer |
| 13   | uart  |
| 14   | wdg   |
| 15   | dac   |

The current definition of HAL has already been compatible with the standards of many chip companies, such as STM32, Beken, NXP. Users can refer to the HAL encapsulation of existing platforms.

The following part will give you the detailed portiong steps in HAL, taking STM32L4 series as an example.

Interface function in HAL is located in /include/hal/soc, and UART's is defined in its corresponding uart.h.

Since the driver function of STM32L4 and the interface defined in HAL is not exactly the same, we need to encapsulate a layer on STM32L4 drive to dock with HAL.

Take uart as an example, in order to dock uart1 and uart2, we need to create two files (hal_uart_stm32l4.c and hal_uart_stm32l4.h), and put the code in encapsulation layer into these two files.

In hal_uart_stm32l4.c, we first need to define the uart handle of the corresponding STM32L4:

```C
/* handle for uart */
UART_HandleTypeDef uart1_handle;
UART_HandleTypeDef uart2_handle;
```
Since the macro definition of component properties in HAL and the driver layer is not the same, [For example, if the configuration data for uart in HAL is 8 bit, we should set the hal_uart_data_width_t as  DATA_WIDTH_8BIT (= 3). However, for STM32L4 initialization, we should set the WordLength of _InitTypeDef as UART_WORDLENGTH_8B (= 0).], some conversion is needed. We have defined the following conversion functions:

```C
/* function used to transform hal para to stm32l4 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, 
               uint32_t *data_width_stm32l4);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal, 
               uint32_t *parity_stm32l4);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, 
               uint32_t *stop_bits_stm32l4);
static int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal, 
               uint32_t *flow_control_stm32l4);
static int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4);

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
        uint32_t *data_width_stm32l4)
{
    uint32_t data_width = 0;
    int32_t	ret = 0;

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
    int32_t	ret = 0;

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
    int32_t	ret = 0;

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
    int32_t	ret = 0;

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
    int32_t	ret = 0;

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
```
Then, implement the functions in HAL one by one.

Initiation operation:
If a serial port is to be set as a standard input/output port, the port in the corresponding uart_dev_t structure must be set to 0, and priv cannot be used at this time. Other serial ports do not have this requirement.
```C
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
        /* if other uart exist add init code here */

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

    ret = uart_dataWidth_transform(uart->config.data_width, 
          &uart1_handle.Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, 
           &uart1_handle.Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, 
           &uart1_handle.Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, 
           &uart1_handle.Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, 
           &uart1_handle.Init.Mode);

    if (ret != 0) {
        return -1;
    }

    uart1_handle.Init.HwFlowCtl              = UART1_HW_FLOW_CTL;
    uart1_handle.Init.OverSampling           = UART1_OVER_SAMPLING;
    uart1_handle.Init.OneBitSampling         = UART1_ONE_BIT_SAMPLING;
    uart1_handle.AdvancedInit.AdvFeatureInit = UART1_ADV_FEATURE_INIT;

    /* init uart */
    uart1_MspInit();
    ret = HAL_UART_Init(&uart1_handle);
		
    return ret;
}

void uart1_MspInit(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable GPIO clock */
    UART1_TX_GPIO_CLK_ENABLE();
    UART1_RX_GPIO_CLK_ENABLE();

    /* Enable USART clock */
    UART1_CLK_ENABLE();
 
    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin       = UART1_TX_PIN;
    gpio_init_structure.Mode      = UART1_TX_MODE;
    gpio_init_structure.Speed     = UART1_TX_SPEED;
    gpio_init_structure.Pull      = UART1_TX_PULL;
    gpio_init_structure.Alternate = UART1_TX_ALTERNATE;
    HAL_GPIO_Init(UART1_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin       = UART1_RX_PIN;
    gpio_init_structure.Mode      = UART1_RX_MODE;
    gpio_init_structure.Alternate = UART1_RX_ALTERNATE;
    HAL_GPIO_Init(UART1_RX_GPIO_PORT, &gpio_init_structure);

    HAL_NVIC_SetPriority(UART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(UART1_IRQn);
}

int32_t uart2_init(uart_dev_t *uart)
{
    int32_t ret = 0;

    uart2_handle.Instance                    = UART2;
    uart2_handle.Init.BaudRate               = uart->config.baud_rate;

    ret = uart_dataWidth_transform(uart->config.data_width, 
          &uart2_handle.Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, 
           &uart2_handle.Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, 
           &uart2_handle.Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, 
           &uart2_handle.Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, 
           &uart2_handle.Init.Mode);

    if (ret != 0) {
        return -1;
    }

    uart2_handle.Init.HwFlowCtl              = UART2_HW_FLOW_CTL;
    uart2_handle.Init.OverSampling           = UART2_OVER_SAMPLING;
    uart2_handle.Init.OneBitSampling         = UART2_ONE_BIT_SAMPLING;
    uart2_handle.AdvancedInit.AdvFeatureInit = UART2_ADV_FEATURE_INIT;

    /* init uart */
    uart2_MspInit();
    ret = HAL_UART_Init(&uart2_handle);
		
    return ret;
}

void uart2_MspInit(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable GPIO clock */
    UART2_TX_GPIO_CLK_ENABLE();
    UART2_RX_GPIO_CLK_ENABLE();

    /* Enable USART clock */
    UART2_CLK_ENABLE();
 
    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin       = UART2_TX_PIN;
    gpio_init_structure.Mode      = UART2_TX_MODE;
    gpio_init_structure.Speed     = UART2_TX_SPEED;
    gpio_init_structure.Pull      = UART2_TX_PULL;
    gpio_init_structure.Alternate = UART2_TX_ALTERNATE;
    HAL_GPIO_Init(UART2_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin       = UART2_RX_PIN;
    gpio_init_structure.Mode      = UART2_RX_MODE;
    gpio_init_structure.Alternate = UART2_RX_ALTERNATE;
    HAL_GPIO_Init(UART2_RX_GPIO_PORT, &gpio_init_structure);

    HAL_NVIC_SetPriority(UART2_IRQn, 0,1);
    HAL_NVIC_EnableIRQ(UART2_IRQn);
}
```
Send data:
```C
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = -1;

    if((uart != NULL) && (data != NULL)) {
        ret = HAL_UART_Transmit_IT((UART_HandleTypeDef *)uart->priv,
              (uint8_t *)data, size);
    }

    return ret;
}
```

Receive data:
```C
int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = -1;

    if((uart != NULL) && (data != NULL)) {
        ret = HAL_UART_Receive_IT((UART_HandleTypeDef *)uart->priv,
              (uint8_t *)data, size);
    }

    return ret;
}
```
Finalize the project:
```C
int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case PORT_UART1:
      	    ret = uart1_DeInit();
            break;
        case PORT_UART2:
      	    ret = uart2_DeInit();
            break;
        /* if other uart exist add Deinit code here */

        default:
            break;
    }

    return ret;
}

int32_t uart1_DeInit(void)
{
    int32_t ret = -1;

    /* uart deinitialization */
    ret = HAL_UART_DeInit(&uart1_handle);
    uart1_DeMspInit();

    return ret;
}

void uart1_DeMspInit(void)
{
    /* Disable USART clock */
    UART1_CLK_DISABLE();

    /* USART TX/RX pins deinitializations */
    UART1_TX_GPIO_CLK_DISABLE();
    UART1_RX_GPIO_CLK_DISABLE();
}

int32_t uart2_DeInit(void)
{
    int32_t ret = -1;

    /* uart deinitialization */
    ret = HAL_UART_DeInit(&uart2_handle);
    uart2_DeMspInit();

    return ret;
}

void uart2_DeMspInit(void)
{
    /* Disable USART clock */
    UART2_CLK_DISABLE();

    /* USART TX/RX pins deinitializations */
    UART2_TX_GPIO_CLK_DISABLE();
    UART2_RX_GPIO_CLK_DISABLE();
}
```
The corresponding hal_uart_stm32l4.h is :
```C
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_UART_STM32L4_H
#define __HAL_UART_STM32L4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"  
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"

#define PORT_UART1 1
#define PORT_UART2 2
#define PORT_UART3 3
#define PORT_UART4 4

/***************************** uart1 configure *******************************/
#define UART1                          USART1

#define UART1_HW_FLOW_CTL        UART_HWCONTROL_NONE		 
#define UART1_OVER_SAMPLING      UART_OVERSAMPLING_16		 
#define UART1_ONE_BIT_SAMPLING   UART_ONE_BIT_SAMPLE_DISABLE		 
#define UART1_ADV_FEATURE_INIT   UART_ADVFEATURE_NO_INIT		 

#define UART1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define UART1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()
#define UART1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define UART1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()
#define UART1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define UART1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()

#define UART1_TX_PIN                   GPIO_PIN_6
#define UART1_TX_MODE                  GPIO_MODE_AF_PP
#define UART1_TX_SPEED                 GPIO_SPEED_FREQ_VERY_HIGH
#define UART1_TX_PULL                  GPIO_PULLUP
#define UART1_TX_ALTERNATE             GPIO_AF7_USART1
#define UART1_TX_GPIO_PORT             GPIOB

#define UART1_RX_PIN                   GPIO_PIN_7
#define UART1_RX_MODE                  GPIO_MODE_AF_PP
#define UART1_RX_ALTERNATE             GPIO_AF7_USART1
#define UART1_RX_GPIO_PORT             GPIOB

#define UART1_IRQn                     USART1_IRQn


/***************************** uart1 configure *******************************/
#define UART2                          USART2

#define UART2_HW_FLOW_CTL        UART_HWCONTROL_NONE		 
#define UART2_OVER_SAMPLING      UART_OVERSAMPLING_16		 
#define UART2_ONE_BIT_SAMPLING   UART_ONE_BIT_SAMPLE_DISABLE		 
#define UART2_ADV_FEATURE_INIT   UART_ADVFEATURE_NO_INIT		 

#define UART2_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()
#define UART2_CLK_DISABLE()            __HAL_RCC_USART2_CLK_DISABLE()
#define UART2_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART2_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define UART2_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART2_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()

#define UART2_TX_PIN                   GPIO_PIN_2
#define UART2_TX_MODE                  GPIO_MODE_AF_PP
#define UART2_TX_SPEED                 GPIO_SPEED_FREQ_HIGH
#define UART2_TX_PULL                  GPIO_NOPULL
#define UART2_TX_ALTERNATE             GPIO_AF7_USART2
#define UART2_TX_GPIO_PORT             GPIOA

#define UART2_RX_PIN                   GPIO_PIN_3
#define UART2_RX_MODE                  GPIO_MODE_AF_PP
#define UART2_RX_ALTERNATE             GPIO_AF7_USART2
#define UART2_RX_GPIO_PORT             GPIOA

#define UART2_IRQn                     USART2_IRQn

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */

```
When the mentioned code is completed, the porting of uart in HAL is completed, and you can operate on underlying hardwares through functions in HAL. Porting in other devices is similar to this.

When the system is initialized, you can call functions to send and receive data by defining corresponding handles and initialize them.
```C
uart_dev_t uart_dev_com1;
static void uart_init(void)
{
    uart_dev_com1.port = PORT_UART1;
    uart_dev_com1.config.baud_rate = 115200;
    uart_dev_com1.config.data_width = DATA_WIDTH_8BIT;
    uart_dev_com1.config.parity = NO_PARITY;
    uart_dev_com1.config.stop_bits = STOP_BITS_1;
    uart_dev_com1.config.flow_control = FLOW_CONTROL_DISABLED;	
    uart_dev_com1.config.mode = MODE_TX_RX;

    hal_uart_init(&uart_dev_com1);
}

```
More porting examples in HAL can refer to platform/mcu/stm32l4xx/src/STM32L496G-Discovery/hal.


## 2 porting in flash abstraction layer
### 2.1 porting in flash abstraction layer
Code example of porting in flash abstraction layer : [参考实现](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/stm32l4xx/hal/flash_port.c)。  
It mainly concerns the following function modification:
```C
  hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)

  int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)

  int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                          const void *in_buf, uint32_t in_buf_len)

  int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                                const void *in_buf, uint32_t in_buf_len)

  int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf,
                         uint32_t in_buf_len)

  int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set,
                                  uint32_t size)  

  int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
```

### 2.2 porting of KV 

- developers need to implement related interfaces in flash HAL;

- developers need to declare the dependency of components in Makefile : $(NAME) _COMPONENTS = modules.fs.kv;

- developer needs to define the flash partition number used by the KV through CONFIG_AOS_KV_PTN 

- if the minimum erasure unit of the flash used by developers is greater than 4096 bytes, the size of the logical block in KV should be adjusted (the default is 4096 bytes).

- Developers need to define the size of the flash partition specified through CONFIG_AOS_KV_BUFFER_SIZE, which should more than 2 KV's logical block size, and the default value is 8192 bytes.

- KV length limit:

  - the maximum length should be less than 255 bytes;
  - the maximum length can be set in definition of ITEM_MAX_VAL_LEN, with a default value of 512 bytes.

  ​

