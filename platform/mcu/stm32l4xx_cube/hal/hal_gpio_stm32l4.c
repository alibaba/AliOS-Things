/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "stm32l4xx_hal.h"
#include "hal_gpio_stm32l4.h"
#ifdef HAL_GPIO_MODULE_ENABLED

static int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str);
static int32_t get_gpio_group(gpio_dev_t *gpio, GPIO_TypeDef **GPIOx);
static uint32_t get_gpio_pin(uint8_t pin);
static int32_t gpio_get_int_num(IRQn_Type *pirqn, uint8_t port);
int32_t gpio_has_priv(gpio_dev_t *gpio, GPIO_InitTypeDef * init_str);

#define GPIO_INT_NUM                    16

typedef struct
{
    gpio_irq_handler_t handler;
    void *arg;
} gpio_int_hdl;

static gpio_int_hdl g_hdl_table[GPIO_INT_NUM] = {{NULL, NULL}};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
    case GPIO_PIN_0:
        if (g_hdl_table[0].handler) {
            g_hdl_table[0].handler(g_hdl_table[0].arg);
        }
        break;
    case GPIO_PIN_1:
        if (g_hdl_table[1].handler) {
            g_hdl_table[1].handler(g_hdl_table[1].arg);
        }
        break;
    case GPIO_PIN_2:
        if (g_hdl_table[2].handler) {
            g_hdl_table[2].handler(g_hdl_table[2].arg);
        }
        break;
    case GPIO_PIN_3:
        if (g_hdl_table[3].handler) {
            g_hdl_table[3].handler(g_hdl_table[3].arg);
        }
        break;
    case GPIO_PIN_4:
        if (g_hdl_table[4].handler) {
            g_hdl_table[4].handler(g_hdl_table[4].arg);
        }
        break;
    case GPIO_PIN_5:
        if (g_hdl_table[5].handler) {
            g_hdl_table[5].handler(g_hdl_table[5].arg);
        }
        break;
    case GPIO_PIN_6:
        if (g_hdl_table[6].handler) {
            g_hdl_table[6].handler(g_hdl_table[6].arg);
        }
        break;
    case GPIO_PIN_7:
        if (g_hdl_table[7].handler) {
            g_hdl_table[7].handler(g_hdl_table[7].arg);
        }
        break;
    case GPIO_PIN_8:
        if (g_hdl_table[8].handler) {
            g_hdl_table[8].handler(g_hdl_table[8].arg);
        }
        break;
    case GPIO_PIN_9:
        if (g_hdl_table[9].handler) {
            g_hdl_table[9].handler(g_hdl_table[9].arg);
        }
        break;
    case GPIO_PIN_10:
        if (g_hdl_table[10].handler) {
            g_hdl_table[10].handler(g_hdl_table[10].arg);
        }
        break;
    case GPIO_PIN_11:
        if (g_hdl_table[11].handler) {
            g_hdl_table[11].handler(g_hdl_table[11].arg);
        }
        break;
    case GPIO_PIN_12:
        if (g_hdl_table[12].handler) {
            g_hdl_table[12].handler(g_hdl_table[12].arg);
        }
        break;
    case GPIO_PIN_13:
        if (g_hdl_table[13].handler) {
            g_hdl_table[13].handler(g_hdl_table[13].arg);
        }
        break;
    case GPIO_PIN_14:
        if (g_hdl_table[14].handler) {
            g_hdl_table[14].handler(g_hdl_table[14].arg);
        }
        break;
    case GPIO_PIN_15:
        if (g_hdl_table[15].handler) {
            g_hdl_table[15].handler(g_hdl_table[15].arg);
        }
        break;
    default:
        return;
    }
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
#if !defined(STM32L432xx)
    else if (GPIOx == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    } else if (GPIOx == GPIOE) {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    } else if (GPIOx == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    } else if (GPIOx == GPIOG) {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    } else if (GPIOx == GPIOH) {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif
    else {
        return -1;
    }
    
    ret = gpio_para_transform(gpio, &GPIO_InitStruct);
    if (ret){
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

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    int32_t ret = 0;
    int int_pin = gpio->port % PINS_IN_GROUP;

    if (gpio->config != IRQ_MODE) {
        return -1;
    }
    if (*(gpio_irq_trigger_t *)gpio->priv != trigger) {
        gpio->priv = (void *)&trigger;
        ret = hal_gpio_init(gpio);
        if (ret != 0) {
            return -1;
        }
    }
    g_hdl_table[int_pin].handler = handler;
    g_hdl_table[int_pin].arg = arg;

    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    int32_t ret = 0;
    int int_pin = gpio->port % PINS_IN_GROUP;
    IRQn_Type pirqn = 0;
    IRQn_Type pirqn_temp = 0;
    int i;

    if (gpio->config != IRQ_MODE) {
        return -1;
    }
    g_hdl_table[int_pin].handler = NULL;
    g_hdl_table[int_pin].arg = NULL;
    ret = gpio_get_int_num(&pirqn, gpio->port);
    if (ret != 0) {
        return -1;
    }
    for (i = 0; i < GPIO_INT_NUM; ++i) {
        if (g_hdl_table[i].handler == NULL) {
            continue;
        }
        ret = gpio_get_int_num(&pirqn_temp, i);
        if (ret != 0) {
            return -1;
        }
        if (pirqn_temp == pirqn) {
            break;
        }
    }
    if (i == GPIO_INT_NUM) {
        HAL_NVIC_DisableIRQ(pirqn);
    }

    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    return 0;
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
        pin = gpio->port % PINS_IN_GROUP;
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
        return -1;
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
            init_str->Pull      = GPIO_NOPULL;
            init_str->Speed     = GPIO_SPEED_FREQ_LOW;
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
#if !defined(STM32L432xx)
        case GROUP_GPIOD:
            *GPIOx = GPIOD;
            break;
        case GROUP_GPIOE:
            *GPIOx = GPIOE;
            break;
        case GROUP_GPIOF:
            *GPIOx = GPIOF;
            break;
        case GROUP_GPIOG:
            *GPIOx = GPIOG;
            break;
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

    result = 1u << pin_t;

    return result;
}
#endif
