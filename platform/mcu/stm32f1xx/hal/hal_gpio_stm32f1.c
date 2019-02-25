/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "hal_gpio_stm32f1.h"
#include "aos/hal/gpio.h"
#ifdef HAL_GPIO_MODULE_ENABLED

#define  GPIO_IRQ_SLOP_MAX  (16)

typedef  struct gpio_irq_slop {
    int32_t pin_num;
    int32_t irq_num;
    gpio_irq_handler_t handler;
    void * priv;
} gpio_irq_slop_t;

static gpio_irq_slop_t gGpioSlop[GPIO_IRQ_SLOP_MAX]= {
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
};
static int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str);
static int32_t get_gpio_group(gpio_dev_t *gpio, GPIO_TypeDef **GPIOx);
static uint32_t get_gpio_pin(uint8_t pin);
int32_t gpio_has_priv(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str);


static  gpio_irq_slop_t * gpio_slop_get(int32_t pin_num)
{

    int32_t index = -1;
    int8_t found = 0;
    for(index=0; index<GPIO_IRQ_SLOP_MAX; ++index) {
        if(pin_num == gGpioSlop[index].pin_num){
            return &gGpioSlop[index];
        }
    }
    return(NULL);
}


static  int8_t  gpio_slop_irq(int32_t irq_num)
{

    int32_t index = -1;
    int8_t found = 0;
    for(index=0; index<GPIO_IRQ_SLOP_MAX; ++index) {
        if(irq_num != gGpioSlop[index].irq_num)continue;
        if(NULL == gGpioSlop[index].handler)continue;
        gGpioSlop[index].handler(gGpioSlop[index].priv);
    }
    return(0);
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

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    int32_t ret = -1;
    GPIO_TypeDef *GPIOx = NULL;
    GPIO_InitTypeDef  GPIO_InitStruct;

    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret){
        return ret;
    }

    if (GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
#if defined(GPIOD)
    else if (GPIOx == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
#endif
#if defined(GPIOE)
    else if (GPIOx == GPIOE) {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
#endif
#if defined(GPIOF)
    else if (GPIOx == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif
#if defined(GPIOG)
    else if (GPIOx == GPIOG) {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif
#if defined(GPIOH)
    else if (GPIOx == GPIOH) {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif
    else {
        return -1;
    }

    ret = gpio_para_transform(gpio, &GPIO_InitStruct);
    if (ret) {
        return ret;
    }

    GPIO_InitStruct.Pin = get_gpio_pin(gpio->port);
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    ret |= gpio_has_priv(gpio, &GPIO_InitStruct);

    return ret;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    uint16_t pin = 0;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
        HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_SET);
    };

    return ret;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    uint16_t pin = 0;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
        HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_RESET);
    };

    return ret;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    uint16_t pin = 0;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
        HAL_GPIO_TogglePin(GPIOx, pin);
    };

    return ret;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    uint16_t pin = 0;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = 0;

    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
        *value = HAL_GPIO_ReadPin(GPIOx, pin);
    };

    return ret;
}

int32_t gpio_get_int_num(IRQn_Type *pirqn, uint8_t port)
{
    uint32_t gpio_pin = 0;

    if (pirqn == NULL)
        return -1;

    gpio_pin = get_gpio_pin(port);
    switch (gpio_pin) {
    case GPIO_PIN_0:
        *pirqn = EXTI0_IRQn;
        break;
    case GPIO_PIN_1:
        *pirqn = EXTI1_IRQn;
        break;
    case GPIO_PIN_2:
        *pirqn = EXTI2_IRQn;
        break;
    case GPIO_PIN_3:
        *pirqn = EXTI3_IRQn;
        break;
    case GPIO_PIN_4:
        *pirqn = EXTI4_IRQn;
        break;
    case GPIO_PIN_5:
    case GPIO_PIN_6:
    case GPIO_PIN_7:
    case GPIO_PIN_8:
    case GPIO_PIN_9:
        *pirqn = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
    case GPIO_PIN_13:
    case GPIO_PIN_14:
    case GPIO_PIN_15:
        *pirqn = EXTI15_10_IRQn;
        break;
    default:
        return -1;
    }

    return 0;
}

int32_t gpio_has_priv(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str)
{
    int32_t ret = 0;

    if (gpio->priv == NULL) {
        return 0;
    }
    switch (gpio->config) {
    case IRQ_MODE:
        switch (*(gpio_irq_trigger_t *)gpio->priv) {
        case IRQ_TRIGGER_RISING_EDGE:
            init_str->Mode      = GPIO_MODE_IT_RISING;
            init_str->Pull      = GPIO_NOPULL;
            break;
        case IRQ_TRIGGER_FALLING_EDGE:
            init_str->Mode      = GPIO_MODE_IT_FALLING;
            init_str->Pull      = GPIO_NOPULL;
            break;
        case IRQ_TRIGGER_BOTH_EDGES:
            init_str->Mode      = GPIO_MODE_IT_RISING_FALLING;
            init_str->Pull      = GPIO_NOPULL;
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case OUTPUT_PUSH_PULL:
    case OUTPUT_OPEN_DRAIN_NO_PULL:
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        switch (*(uint8_t *)gpio->priv) {
        case 1:
            hal_gpio_output_high(gpio);
            break;
        case 0:
            hal_gpio_output_low(gpio);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        break;
    }

    return ret;
}

int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str)
{
    int32_t ret = 0;
    IRQn_Type pirqn = 0;

    switch (gpio->config) {
    case ANALOG_MODE:
        init_str->Mode      = GPIO_MODE_ANALOG;
        break;
    case IRQ_MODE:
        ret = gpio_get_int_num(&pirqn, gpio->port);
        if (ret == 0) {
            HAL_NVIC_SetPriority(pirqn, 0, 0);
            HAL_NVIC_EnableIRQ(pirqn);
        }
        ret |= gpio_has_priv(gpio, init_str);
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
        init_str->Pull      = GPIO_PULLUP;
        init_str->Speed     = GPIO_SPEED_FREQ_HIGH;
        break;
    case OUTPUT_OPEN_DRAIN_NO_PULL:
        init_str->Mode      = GPIO_MODE_OUTPUT_OD;
        init_str->Pull      = GPIO_NOPULL;
        init_str->Speed     = GPIO_SPEED_FREQ_LOW;
        break;
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        init_str->Mode      = GPIO_MODE_OUTPUT_OD;
        init_str->Pull      = GPIO_PULLUP;
        init_str->Speed     = GPIO_SPEED_FREQ_LOW;
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

int32_t get_gpio_group(gpio_dev_t *gpio, GPIO_TypeDef **GPIOx)
{
    uint16_t group = 0;
    int32_t ret = 0;

    if (gpio == NULL) {
        return -1;
    }

    group = gpio->port / PINS_IN_GROUP;

    switch (group) {
    case GROUP_GPIOA:
        *GPIOx = GPIOA;
        break;
    case GROUP_GPIOB:
        *GPIOx = GPIOB;
        break;
    case GROUP_GPIOC:
        *GPIOx = GPIOC;
        break;
#if defined(GROUP_GPIOD) && defined(GPIOD)
    case GROUP_GPIOD:
        *GPIOx = GPIOD;
        break;
#endif
#if defined(GROUP_GPIOE) && defined(GPIOE)
    case GROUP_GPIOE:
        *GPIOx = GPIOE;
        break;
#endif
#if defined(GROUP_GPIOF) && defined(GPIOF)
    case GROUP_GPIOF:
        *GPIOx = GPIOF;
        break;
#endif
#if defined(GROUP_GPIOG) && defined(GPIOG)
    case GROUP_GPIOG:
        *GPIOx = GPIOG;
        break;
#endif
#if defined(GROUP_GPIOH) && defined(GPIOH)
    case GROUP_GPIOH:
        *GPIOx = GPIOH;
        break;
#endif
    default:
        ret = -1;
        break;
    }

    return ret;
}

uint32_t get_gpio_pin(uint8_t pin)
{
    uint32_t result = 1;
    uint8_t pin_t = pin % PINS_IN_GROUP;

    return (uint32_t)1<<pin_t;
}

static int32_t gpio_irq_mode_init(gpio_dev_t *gpio,gpio_irq_trigger_t trigger)
{

    int32_t ret = -1;
    gpio_irq_trigger_t * new_irq = calloc(1,sizeof(*new_irq));
    if(NULL == new_irq) {
        return (-1);
    }
    *new_irq = trigger;
    gpio->priv = new_irq;
    ret = hal_gpio_init(gpio);
    if(NULL != new_irq) {
        free(new_irq);
        new_irq = NULL;
    }
    return ret;
}



int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{

    int32_t ret = -1;

    if (IRQ_MODE != gpio->config) {
	return (-1);
    }

    gpio_irq_slop_t * slop = gpio_slop_get(-1);
    if(NULL == slop) {
        return(-1);
    }
    ret = gpio_irq_mode_init(gpio,trigger);
    if(0 != ret) {
        return (-1);
    }
    slop->pin_num = gpio->port;
    slop->irq_num = get_gpio_pin(gpio->port);
    slop->handler = handler;
    slop->priv = arg;
    return (0);
}



int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{

    int32_t ret = -1;

    if (IRQ_MODE != gpio->config) {
	return (-1);
    }

   IRQn_Type pirqn = 0;
    gpio_irq_slop_t * slop = gpio_slop_get(gpio->port);
    if(NULL == slop) {
        return(-1);
    }
    ret = gpio_get_int_num(&pirqn, gpio->port);
    if (0 != ret) {
        return (-1);
    }
    HAL_NVIC_DisableIRQ(pirqn);
    return 0;
}


int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{

    gpio_irq_slop_t * slop = gpio_slop_get(gpio->port);
    if(NULL == slop)return(-1);
    slop->pin_num = -1;
    slop->irq_num = -1;
    slop->handler = NULL;
    slop->priv = NULL;
    return 0;
}


int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{

    return 0;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    gpio_slop_irq(GPIO_Pin);
}

#endif


