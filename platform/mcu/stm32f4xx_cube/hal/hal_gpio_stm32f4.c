/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stm32f4xx_hal.h"
#ifdef HAL_GPIO_MODULE_ENABLED
#include "hal_gpio_stm32f4.h"

#include "k_api.h"
#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/gpio.h"

#define  GPIO_IRQ_SLOP_MAX  (16)

static gpio_irq_slop_t gGpioSlop[GPIO_IRQ_SLOP_MAX] = {
    {.pin_num = -1},{.pin_num = -1},{.pin_num = -1},{.pin_num = -1},
    {.pin_num = -1},{.pin_num = -1},{.pin_num = -1},{.pin_num = -1},
    {.pin_num = -1},{.pin_num = -1},{.pin_num = -1},{.pin_num = -1},
    {.pin_num = -1},{.pin_num = -1},{.pin_num = -1},{.pin_num = -1},
};

static int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str)
{
    int32_t   ret = 0;
    IRQn_Type pirqn = 0;

    switch (gpio->config) {
    case ANALOG_MODE:
        init_str->Mode      = GPIO_MODE_ANALOG;
        init_str->Pull      = GPIO_NOPULL;
        break;
    case IRQ_MODE:
        init_str->Mode      = GPIO_MODE_INPUT;
        init_str->Pull      = GPIO_NOPULL;
        break;
    case INPUT_PULL_UP:
        init_str->Mode      = GPIO_MODE_INPUT;
        init_str->Pull      = GPIO_PULLUP;
        break;
    case INPUT_PULL_DOWN:
        init_str->Mode      = GPIO_MODE_INPUT;
        init_str->Pull      = GPIO_PULLDOWN;
        break;
    case INPUT_HIGH_IMPEDANCE:
        init_str->Mode      = GPIO_MODE_INPUT;
        init_str->Pull      = GPIO_NOPULL;
        break;
    case OUTPUT_PUSH_PULL:
        init_str->Mode      = GPIO_MODE_OUTPUT_PP;
        init_str->Pull      = GPIO_NOPULL;
        break;
    case OUTPUT_OPEN_DRAIN_NO_PULL:
        init_str->Mode      = GPIO_MODE_OUTPUT_OD;
        init_str->Pull      = GPIO_NOPULL;
        break;
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        init_str->Mode      = GPIO_MODE_OUTPUT_OD;
        init_str->Pull      = GPIO_PULLUP;
        break;
    case OUTPUT_OPEN_DRAIN_AF:
        init_str->Mode      = GPIO_MODE_AF_OD;
        init_str->Pull      = GPIO_NOPULL;
        break;
    case OUTPUT_PUSH_PULL_AF:
        init_str->Mode      = GPIO_MODE_AF_PP;
        init_str->Pull      = GPIO_NOPULL;
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

static int8_t gpio_slop_irq(int32_t irq_num)
{
    int32_t index = -1;

    for(index=0; index < GPIO_IRQ_SLOP_MAX; ++index) {
        if (irq_num != gGpioSlop[index].irq_num)
            continue;

        if (NULL == gGpioSlop[index].handler)
            continue;

        gGpioSlop[index].handler(gGpioSlop[index].priv);
    }

    return 0;
}

static gpio_irq_slop_t * gpio_slop_get(int32_t pin_num)
{
    int32_t index = -1;

    for (index = 0; index < GPIO_IRQ_SLOP_MAX; ++index) {
        if (pin_num == gGpioSlop[index].pin_num) {
            return &gGpioSlop[index];
        }
    }

    return NULL;
}

static int get_mapTable_pos(uint16_t port, uint16_t *pos)
{
    int ret = -1;
    int i   = 0;

    for (i = 0; i < TOTAL_GPIO_NUM; i++) {
        if (port == gpio_mapping_table[i].Port) {
            *pos = i;
            ret = 0;
            break;
        }
     }

    return ret;
}

static int get_irqn_type(IRQn_Type *IRQn, uint16_t pin)
{
    int ret = 0;

    switch (pin)
    {
        case GPIO_PIN_0:
            *IRQn = EXTI0_IRQn;
            break;
        case GPIO_PIN_1:
            *IRQn = EXTI1_IRQn;
            break;
        case GPIO_PIN_2:
            *IRQn = EXTI2_IRQn;
            break;
        case GPIO_PIN_3:
            *IRQn = EXTI3_IRQn;
            break;
        case GPIO_PIN_4:
            *IRQn = EXTI4_IRQn;
            break;
        case GPIO_PIN_5:
        case GPIO_PIN_6:
        case GPIO_PIN_7:
        case GPIO_PIN_8:
        case GPIO_PIN_9:
            *IRQn = EXTI9_5_IRQn;
            break;
        case GPIO_PIN_10:
        case GPIO_PIN_11:
        case GPIO_PIN_12:
        case GPIO_PIN_13:
        case GPIO_PIN_14:
        case GPIO_PIN_15:
            *IRQn = EXTI15_10_IRQn;
            break;
        default:
        ret = -1;
        break;
    }

    return ret;
}



GPIO_TypeDef *hal_gpio_typedef(uint16_t hal_pin)
{
    GPIO_TypeDef *port = NULL;

    if (hal_pin >= HAL_GPIO_CNT) {
        return NULL;
    }

    switch (hal_pin / PINS_IN_GROUP) {
        case GROUP_GPIOA:
            port = GPIOA;
            break;
        case GROUP_GPIOB:
            port = GPIOB;
            break;
        case GROUP_GPIOC:
            port = GPIOC;
            break;
#if defined(GROUP_GPIOD) && defined(GPIOD)
        case GROUP_GPIOD:
            port = GPIOD;
            break;
#endif
#if defined(GROUP_GPIOE) && defined(GPIOE)
        case GROUP_GPIOE:
            port = GPIOE;
            break;
#endif
#if defined(GROUP_GPIOF) && defined(GPIOF)
        case GROUP_GPIOF:
            port = GPIOF;
            break;
#endif
#if defined(GROUP_GPIOG) && defined(GPIOG)
        case GROUP_GPIOG:
            port = GPIOG;
            break;
#endif
#if defined(GROUP_GPIOH) && defined(GPIOH)
        case GROUP_GPIOH:
            port = GPIOH;
            break;
#endif
#if defined(GROUP_GPIOI) && defined(GPIOI)
        case GROUP_GPIOI:
            port = GPIOI;
            break;
#endif
        default:
            port = NULL;
            break;
    }

    return port;
}

uint32_t hal_gpio_pin(uint16_t hal_pin)
{
    uint8_t  pin_t = hal_pin % PINS_IN_GROUP;

    return (uint32_t)(1 << pin_t);
}

void hal_gpio_enable_clk(uint16_t hal_pin)
{
    GPIO_TypeDef *GPIOx;

    GPIOx = hal_gpio_typedef(hal_pin);

    if (GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
#if defined(GROUP_GPIOD) && defined(GPIOD)
    else if (GPIOx == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
#endif
#if defined(GROUP_GPIOE) && defined(GPIOE)
    else if (GPIOx = GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
#endif
#if defined(GROUP_GPIOF) && defined(GPIOF)
    else if (GPIOx == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif
#if defined(GROUP_GPIOG) && defined(GPIOG)
    else if (GPIOx == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif
#if defined(GROUP_GPIOH) && defined(GPIOH)
    else if (GPIOx == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif
#if defined(GROUP_GPIOI) && defined(GPIOI)
    else if (GPIOx == GPIOI)
    {
        __HAL_RCC_GPIOI_CLK_ENABLE();
    }
#endif
}

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    GPIO_TypeDef     *GPIOx = NULL;
    GPIO_InitTypeDef  GPIO_InitStruct;
    int16_t           CurrentPosition = 0;
    int32_t           ret = -1;

    if (NULL == gpio) {
        return -1;
    }

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
    GPIOx = hal_gpio_typedef(gpio->port);
    if (NULL == GPIOx) {
       return -1;
    }

    hal_gpio_enable_clk(gpio->port);

    /* the GPIO map table takes higher priority */
    if (0 == get_mapTable_pos(gpio->port ,&CurrentPosition)) {
        GPIO_InitStruct.Pin = hal_gpio_pin(gpio->port);
        GPIO_InitStruct.Mode = gpio_mapping_table[CurrentPosition].Mode;
        GPIO_InitStruct.Pull = gpio_mapping_table[CurrentPosition].Pull;
        GPIO_InitStruct.Speed = gpio_mapping_table[CurrentPosition].Speed;

        HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

        if ((GPIO_InitStruct.Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_InitStruct.Mode == GPIO_MODE_OUTPUT_OD)) {
            HAL_GPIO_WritePin(GPIOx, GPIO_InitStruct.Pin,gpio_mapping_table[CurrentPosition].defaultLogicalVal);
        }
    } else {
        ret = gpio_para_transform(gpio, &GPIO_InitStruct);
        if (ret) {
            return ret;
        }

        GPIO_InitStruct.Pin = hal_gpio_pin(gpio->port);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

        HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
        /* Set the fault value for the pin to output low */
        HAL_GPIO_WritePin(GPIOx, GPIO_InitStruct.Pin, 0);
    }

    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    GPIO_TypeDef  *GPIOx;
    uint16_t       GPIO_Pin;

    if (NULL == gpio) {
        return -1;
    }

    GPIOx = hal_gpio_typedef(gpio->port);
    if (NULL == GPIOx) {
        return -1;
    }

    GPIO_Pin = hal_gpio_pin(gpio->port);
    HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);

    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    GPIO_TypeDef *GPIOx;
    uint16_t      GPIO_Pin;

    if (NULL == gpio) {
        return -1;
    }

    GPIOx = hal_gpio_typedef(gpio->port);
    if (NULL == GPIOx) {
        return -1;
    }

    GPIO_Pin = hal_gpio_pin(gpio->port);
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PinState_Set);

    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    GPIO_TypeDef *GPIOx;
    uint16_t      GPIO_Pin;

    if (NULL == gpio) {
        return -1;
    }

    GPIOx = hal_gpio_typedef(gpio->port);
    if (NULL == GPIOx) {
        return -1;
    }

    GPIO_Pin = hal_gpio_pin(gpio->port);
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PinState_Reset);

    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    GPIO_TypeDef *GPIOx;
    uint16_t      GPIO_Pin;

    GPIOx = hal_gpio_typedef(gpio->port);
    if (NULL != GPIOx) {
        GPIO_Pin = hal_gpio_pin(gpio->port);
        *value = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
        return 0;
    }

    return -1;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{
    GPIO_TypeDef     *GPIOx = NULL;
    uint16_t          GPIO_Pin;
    GPIO_InitTypeDef  GPIO_InitStruct;
    IRQn_Type         IRQn;
    uint16_t          CurrentPosition = 0;
    gpio_irq_slop_t  *slop = NULL;
    int32_t           ret  = -1;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return ret;
    }

    slop = gpio_slop_get(gpio->port);
    if (NULL != slop) {
        printf("gpio port %d have already have irq set \r\n", gpio->port);
        return ret;
    }

    slop = gpio_slop_get(-1);
    if (NULL == slop) {
        printf("there is no free slop for gpio irq \r\n");
        return ret;
    }

    if (IRQ_MODE != gpio->config) {
        return ret;
    }

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

    if (0 == get_mapTable_pos(gpio->port, &CurrentPosition)) {
        GPIO_InitStruct.Speed = gpio_mapping_table[CurrentPosition].Speed;
    } else {
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    }

    GPIOx = hal_gpio_typedef(gpio->port);
    if (NULL == GPIOx) {
        return ret;
    }

    GPIO_Pin = hal_gpio_pin(gpio->port);

    ret = get_irqn_type(&IRQn, GPIO_Pin);
    if (ret != 0) {
        printf("fail to get pin %d irq type \r\n", GPIO_Pin);
        return -1;
    }

    HAL_NVIC_SetPriority(IRQn, 0, 0); // need discuss the priotity.
    HAL_NVIC_EnableIRQ(IRQn);

    slop->pin_num = gpio->port;
    slop->irq_num = GPIO_Pin;
    slop->handler = handler;
    slop->priv    = arg;


    switch (trigger) {
        case IRQ_TRIGGER_RISING_EDGE  :
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
            break;
        case IRQ_TRIGGER_FALLING_EDGE :
            GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING ;
            break;
        case IRQ_TRIGGER_BOTH_EDGES   :
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
            break;
        default:
            return -1;
    }

    GPIO_InitStruct.Pin  = GPIO_Pin;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    uint16_t         GPIO_Pin;
    uint16_t         CurrentPosition = 0;
    IRQn_Type        IRQn = 0;
    gpio_irq_slop_t *slop = NULL;
    int32_t          ret  = -1;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return ret;
    }

    slop = gpio_slop_get(gpio->port);
    if (NULL == slop) {
        printf("gpio port %d irq doesn't exist\r\n", gpio->port);
        return 0;
    }

    if (IRQ_MODE==gpio->config) {
        GPIO_Pin = hal_gpio_pin(gpio->port);
        ret = get_irqn_type(&IRQn, GPIO_Pin);
        if (ret != 0) {
            return -1;
        }

        HAL_NVIC_DisableIRQ(IRQn);
    }

    slop->pin_num = -1;
    slop->irq_num = -1;
    slop->handler = NULL;
    slop->priv    = NULL;

    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    uint16_t         GPIO_Pin;
    IRQn_Type        IRQn = 0;
    gpio_irq_slop_t *slop = NULL;
    int32_t          ret = -1;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return -1;
    }

    slop = gpio_slop_get(gpio->port);
    if (NULL == slop) {
        printf("gpio port %d irq doesn't exist\r\n", gpio->port);
        return -1;
    }

    if (IRQ_MODE == gpio->config) {
        GPIO_Pin = hal_gpio_pin(gpio->port);
        ret = get_irqn_type(&IRQn, GPIO_Pin);
        if (ret != 0) {
            return -1;
        }

        HAL_NVIC_ClearPendingIRQ(IRQn);

        return 0;
    }

    return -1;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    GPIO_TypeDef *GPIOx = NULL;
    uint16_t      GPIO_Pin;
    int32_t       ret;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return -1;
    }

    GPIOx = hal_gpio_typedef(gpio->port);
    if (NULL == GPIOx) {
        return -1;
    }

    GPIO_Pin = hal_gpio_pin(gpio->port);
    HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

    hal_gpio_disable_irq(gpio);

    return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    gpio_slop_irq(GPIO_Pin);
}

void EXTI0_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

    krhino_intrpt_exit();
}

void EXTI1_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);

    krhino_intrpt_exit();
}

void EXTI2_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);

    krhino_intrpt_exit();
}

void EXTI3_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);

    krhino_intrpt_exit();
}

void EXTI4_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

    krhino_intrpt_exit();
}

void EXTI9_5_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);

    krhino_intrpt_exit();
}

void EXTI15_10_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);

    krhino_intrpt_exit();
}

#endif
