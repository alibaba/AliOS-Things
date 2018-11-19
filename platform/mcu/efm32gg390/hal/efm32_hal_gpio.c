/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "efm32_hal_gpio.h"

int32_t get_gpio_group(gpio_dev_t *gpio, GPIO_Port_TypeDef *GPIOx)
{
    uint16_t group = 0;
    int32_t ret = 0;

    if (gpio == NULL) {
        return -1;
    }

    group = gpio->port / PINS_IN_GROUP;

    switch (group) {
    case GROUP_GPIOA:
        *GPIOx = gpioPortA;
        break;
    case GROUP_GPIOB:
        *GPIOx = gpioPortB;
        break;
    case GROUP_GPIOC:
        *GPIOx = gpioPortC;
        break;
    case GROUP_GPIOD:
        *GPIOx = gpioPortD;
        break;
    case GROUP_GPIOE:
        *GPIOx = gpioPortE;
        break;
    case GROUP_GPIOF:
        *GPIOx = gpioPortF;
        break;
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

int32_t gpio_para_transform(gpio_dev_t *gpio, GPIO_Mode_TypeDef * init_mode,unsigned int* out)
{
    int32_t ret = 0;

    switch (gpio->config) {
    case IRQ_MODE:
        *init_mode = gpioModeInput;
        *out= 0;
        break;

    case INPUT_PULL_UP:
        *init_mode = gpioModeInputPull;
        *out= 1;
        break;
    case INPUT_PULL_DOWN:
        *init_mode = gpioModeInputPull;
        *out= 0;
        break;
    case INPUT_HIGH_IMPEDANCE:
        *init_mode = gpioModeInput;
        *out= 0;
        break;
    case OUTPUT_PUSH_PULL:
        *init_mode = gpioModePushPull;
        *out= 0;
        break;
    case OUTPUT_OPEN_DRAIN_NO_PULL:
        *init_mode = gpioModeWiredAnd;
        *out= 0;
        break;
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        *init_mode = gpioModeWiredAndPullUp;
        *out= 1;
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}


int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    int32_t ret = -1;
    GPIO_Port_TypeDef GPIOx;
    GPIO_Mode_TypeDef mode;
    unsigned int out;
    int pin;
    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret){
        return ret;
    }
    CMU_ClockEnable(cmuClock_GPIO, true);


    ret = gpio_para_transform(gpio, &mode,&out);
    if (ret){
        return ret;
    }

    pin = get_gpio_pin(gpio->port);
    GPIO_PinModeSet(GPIOx, pin, mode, out);
    

    return ret;
}



int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    uint16_t pin = 0;
    GPIO_Port_TypeDef GPIOx;
    int32_t ret = 0;
    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
        GPIO_PinOutSet(GPIOx, pin);
    };

    return ret;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    uint16_t pin = 0;
    GPIO_Port_TypeDef GPIOx;
    int32_t ret = 0;
    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
        GPIO_PinOutClear(GPIOx, pin);
    };

    return ret;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    uint16_t pin = 0;
    GPIO_Port_TypeDef GPIOx;
    int32_t ret = 0;
    if (gpio == NULL) {
        return -1;
    }
    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
        GPIO_PinOutToggle(GPIOx, pin);
    };

    return ret;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    uint16_t pin = 0;  
    GPIO_Port_TypeDef GPIOx;
    int32_t ret = 0;

    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = gpio->port % PINS_IN_GROUP;
        *value = GPIO_PinInGet(GPIOx, pin);
    };

    return ret;
}






int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{

    uint16_t pin = 0;  
    GPIO_Port_TypeDef GPIOx;
    int32_t ret = 0;
    bool risingEdge =false;
    bool fallingEdge = false;
    unsigned int Int_Number;
    if (gpio == NULL) {
        return -1;
    }

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
    };

    Board_GPIOInterruptRegisterCallback(pin, handler);
    if(trigger == IRQ_TRIGGER_RISING_EDGE)
    {
        risingEdge = true;
    }
    else if(trigger == IRQ_TRIGGER_FALLING_EDGE)
    {
        fallingEdge = true;
    }
    else if(trigger == IRQ_TRIGGER_BOTH_EDGES)
    {
        fallingEdge = true;
        risingEdge = true;
    }
    else
    {
    }
    if( (pin%2)==0 )
    {
        Int_Number = GPIO_EVEN_IRQn;
    }
    else
    {
        Int_Number = GPIO_ODD_IRQn;
    }
    GPIO_IntConfig(GPIOx, pin, risingEdge, fallingEdge, true);
    GPIO_IntClear(pin);
    GPIO_IntEnable(pin);
    NVIC_SetPriority(Int_Number, 5);
    NVIC_EnableIRQ(Int_Number);

    return ret;
}



int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    uint16_t pin = 0;  
    GPIO_Port_TypeDef GPIOx;
    int32_t ret = -1;
    
    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
    };
    GPIO_IntClear(pin);
    GPIO_IntDisable(pin);
    return ret;
}


int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    uint16_t pin = 0;  
    GPIO_Port_TypeDef GPIOx;
    int32_t ret = -1;
    
    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == 0) {
        pin = get_gpio_pin(gpio->port);
    };
    GPIO_IntClear(pin);
    GPIO_IntDisable(pin);

    return ret;
}


int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{

    return 0;
}






