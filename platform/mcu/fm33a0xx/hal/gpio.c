/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "stdint.h"
#include "aos/errno.h"
#include "fm33a0xx_include_all.h"
#include "board.h"

// ============== Fm33A0X Gpio Define Begin =============== //

/*
 * GPIO interrupt trigger
 */
typedef enum {
    IRQ_TRIGGER_RISING_EDGE  = 0x1, /* Interrupt triggered at input signal's rising edge  */
    IRQ_TRIGGER_FALLING_EDGE = 0x2, /* Interrupt triggered at input signal's falling edge */
    IRQ_TRIGGER_BOTH_EDGES   = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE,
} gpio_irq_trigger_t;

/*
 * GPIO interrupt callback handler
 */
typedef void (*gpio_irq_handler_t)(void *arg);

gpio_irq_handler_t g_gpio_irq_handler = NULL;
void *g_arg = NULL;
uint8_t g_port = 0xFF; // irq enabled gpio

const uint32_t g_gpio_pin[] = { GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, 
                                GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, 
                                GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11, 
                                GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15, 
                                GPIO_Pin_All
                            };

int8_t port_transform_GPIOx(uint8_t port, GPIOx_Type** GPIOx);

void GPIO_IRQHandler(void)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (EIO == port_transform_GPIOx(g_port, &GPIOx))
    {
        return;
    }
    
    if (SET ==  GPIO_EXTI_EXTIxIF_ChkEx(GPIOx, g_gpio_pin[g_port & 0x0F]))
    {
        GPIO_EXTI_EXTIxIF_ClrEx(GPIOx, g_gpio_pin[g_port & 0x0F]);
    
        if ((NULL != g_gpio_irq_handler) &&
            (NULL != g_arg))
        {
            g_gpio_irq_handler(g_arg);
        }
    }
}

// transform port to GPIOx
int8_t port_transform_GPIOx(uint8_t port, GPIOx_Type** GPIOx)
{
    uint8_t local_port, pin;
    *GPIOx = NULL;
    
    local_port = (port >> 4) & 0x0F;
    pin = port & 0x0F;
    if ((local_port > GPIO_G) ||
        (pin > GPIO_PIN_15))
    {
        return EIO;
    }
    
    switch (local_port)
    {
        case GPIO_A:
            *GPIOx = GPIOA;
            break;
        case GPIO_B:
            *GPIOx = GPIOB;
            break;
        case GPIO_C:
            *GPIOx = GPIOC;
            break;
        case GPIO_D:
            *GPIOx = GPIOD;
            break;
        case GPIO_E:
            *GPIOx = GPIOE;
            break;
        case GPIO_F:
            *GPIOx = GPIOF;
            break;
        case GPIO_G:
            *GPIOx = GPIOG;
            break;
        default:
            break;
    }   
    
    return 0;
}

// trigger type transform
GPIOExtiEdge trigger_transform(gpio_irq_trigger_t trigger)
{
    GPIOExtiEdge edge;
    edge = (GPIOExtiEdge)(trigger - IRQ_TRIGGER_RISING_EDGE);
    
    return edge;
}

// check if gpio port support irq mode
uint8_t check_gpio_irq_mode(uint8_t port)
{
    uint8_t local_port, pin, ret = 1;
    
    local_port = (port >> 4) & 0x0F;
    pin = port & 0x0F;
    
    switch (local_port)
    {
        case GPIO_A:
        case GPIO_B:
        case GPIO_C:
            ret = 0;
            break;
        case GPIO_D:
            if (pin <= GPIO_PIN_10)
            {
                ret = 0;
            }
            break;
        case GPIO_E:
            if ((pin >= GPIO_PIN_2) &&
                (pin <= GPIO_PIN_9))
            {
                ret = 0;
            }
            break;
        case GPIO_F:
            if (pin <= GPIO_PIN_11)
            {
                ret = 0;
            }
            break;
        case GPIO_G:
            if ((pin == GPIO_PIN_2) ||
            ((pin >= GPIO_PIN_5) && (pin <= GPIO_PIN_8)))
            {
                ret = 0;
            }
            break;
        default:
            break; 
    }
    
    return ret;
}

// GPIO port transform exti number
int8_t port_transform_exti_number(uint8_t port)
{
    uint8_t local_port;
    
    local_port = (port >> 4) & 0x0F;
    
    if (local_port <= GPIO_B)
    {
        return EXTI_0;
    }
    else if ((GPIO_C == local_port) ||
             (GPIO_D == local_port) ||
             (GPIO_G == local_port))
    {
        return EXTI_1;
    }
    else
    {
        return EXTI_2;
    }
}

// GPIO IRQ mode config, keep disable until irq fun been set
uint8_t port_irq_config(uint8_t port)
{
    int8_t exti_number;
    GPIOx_Type* GPIOx = NULL;
    
    if (EIO == port_transform_GPIOx(port, &GPIOx))
    {
        return EIO;
    }
    
    exti_number = port_transform_exti_number(port);
    if (EIO == exti_number)
    {
        return EIO;
    }
    
    if (EXTI_0 == exti_number)
    {
        RCC_PERCLK_SetableEx(EXTI0CLK, ENABLE);     //perclk enable
    }
    else if (EXTI_1 == exti_number)
    {
        RCC_PERCLK_SetableEx(EXTI1CLK, ENABLE);     //perclk enable
    }
    else
    {
        RCC_PERCLK_SetableEx(EXTI2CLK, ENABLE);     //perclk enable
    }
    
    InputIO( GPIOx, port & 0x0F, IN_NORMAL);      //Input IO
    GPIO_EXTI_Init( GPIOx, port & 0x0F, EXTI_DISABLE);    //disable defualt
    
    
    NVIC_DisableIRQ(GPIO_IRQn);
    NVIC_SetPriority(GPIO_IRQn,2);//irq priority
    NVIC_EnableIRQ(GPIO_IRQn);
    
    return 0;
}

// ============== Fm33A0X Gpio Define End =============== //

/*
 * Pin configuration
 */
typedef enum {
    ANALOG_MODE,               /* Used as a function pin, input and output analog */
    IRQ_MODE,                  /* Used to trigger interrupt */
    INPUT_PULL_UP,             /* Input with an internal pull-up resistor - use with devices
                                    that actively drive the signal low - e.g. button connected to ground */
    INPUT_PULL_DOWN,           /* Input with an internal pull-down resistor - use with devices
                                    that actively drive the signal high - e.g. button connected to a power rail */
    INPUT_HIGH_IMPEDANCE,      /* Input - must always be driven, either actively or by an external pullup resistor */
    OUTPUT_PUSH_PULL,          /* Output actively driven high and actively driven low -
                                    must not be connected to other active outputs - e.g. LED output */
    OUTPUT_OPEN_DRAIN_NO_PULL, /* Output actively driven low but is high-impedance when set high -
                                    can be connected to other open-drain/open-collector outputs.
                                    Needs an external pull-up resistor */
    OUTPUT_OPEN_DRAIN_PULL_UP, /* Output actively driven low and is pulled high
                                    with an internal resistor when set high -
                                    can be connected to other open-drain/open-collector outputs. */
} gpio_config_t;

/*
 * GPIO dev struct
 */
typedef struct {
    uint8_t       port;    /* gpio port */
    gpio_config_t config;  /* gpio config */
    void         *priv;    /* priv data */
} gpio_dev_t;

///*
// * GPIO interrupt trigger
// */
//typedef enum {
//    IRQ_TRIGGER_RISING_EDGE  = 0x1, /* Interrupt triggered at input signal's rising edge  */
//    IRQ_TRIGGER_FALLING_EDGE = 0x2, /* Interrupt triggered at input signal's falling edge */
//    IRQ_TRIGGER_BOTH_EDGES   = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE,
//} gpio_irq_trigger_t;

///*
// * GPIO interrupt callback handler
// */
//typedef void (*gpio_irq_handler_t)(void *arg);

/**
 * Initialises a GPIO pin
 *
 * @note  Prepares a GPIO pin for use.
 *
 * @param[in]  gpio           the gpio pin which should be initialised
 * @param[in]  configuration  A structure containing the required gpio configuration
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    uint8_t pin;
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }   
    pin = gpio->port & 0x0F;
    
    switch (gpio->config)
    {
        case ANALOG_MODE:               /* Used as a function pin, input and output analog */
            AnalogIO(GPIOx, g_gpio_pin[pin]);
            break;
        case IRQ_MODE:                  /* Used to trigger interrupt */
            if (0 != check_gpio_irq_mode(gpio->port))
            {
                return EIO;
            }
            if (EIO == port_irq_config(gpio->port))
            {
                return EIO;
            }
            break;
        case INPUT_PULL_UP:             /* Input with an internal pull-up resistor - use with devices
                                            that actively drive the signal low - e.g. button connected to ground */
            InputIO(GPIOx, g_gpio_pin[pin], IN_PULLUP);
            break;
        case INPUT_PULL_DOWN:           /* Input with an internal pull-down resistor - use with devices
                                            that actively drive the signal high - e.g. button connected to a power rail */
            return EIO;
            //break;
        case INPUT_HIGH_IMPEDANCE:      /* Input - must always be driven, either actively or by an external pullup resistor */
            InputIO(GPIOx, g_gpio_pin[pin], IN_NORMAL);//TODO
            break;
        case OUTPUT_PUSH_PULL:          /* Output actively driven high and actively driven low -
                                            must not be connected to other active outputs - e.g. LED output */
            OutputIO(GPIOx, g_gpio_pin[pin], OUT_PUSHPULL);
            break;
        case OUTPUT_OPEN_DRAIN_NO_PULL: /* Output actively driven low but is high-impedance when set high -
                                            can be connected to other open-drain/open-collector outputs.
                                            Needs an external pull-up resistor */
            OutputIO(GPIOx, g_gpio_pin[pin], OUT_OPENDRAIN);
            break;
        case OUTPUT_OPEN_DRAIN_PULL_UP: /* Output actively driven low and is pulled high
                                            with an internal resistor when set high -
                                            can be connected to other open-drain/open-collector outputs. */
            return EIO;
        //break;
    }
    
    return 0;
}

/**
 * Sets an output GPIO pin high
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set high
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }
    
    GPIO_SetBits(GPIOx, g_gpio_pin[gpio->port & 0x0F]);
    
    return 0;
}

/**
 * Sets an output GPIO pin low
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }
    
    GPIO_ResetBits(GPIOx, g_gpio_pin[gpio->port & 0x0F]);
    
    return 0;
}

/**
 * Trigger an output GPIO pin's output. Using this function on a
 * gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }
    
    GPIO_ToggleBits(GPIOx, g_gpio_pin[gpio->port & 0x0F]);
    
    return 0;
}

/**
 * Get the state of an input GPIO pin. Using this function on a
 * gpio pin which is set to output mode will return an undefined value.
 *
 * @param[in]  gpio   the gpio pin which should be read
 * @param[in]  value  gpio value
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }
    
    *value = GPIO_ReadInputDataBit(GPIOx, g_gpio_pin[gpio->port & 0x0F]);
    
    return 0;   
}

/**
 * Enables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which is set to
 * output mode is undefined.
 *
 * @param[in]  gpio     the gpio pin which will provide the interrupt trigger
 * @param[in]  trigger  the type of trigger (rising/falling edge)
 * @param[in]  handler  a function pointer to the interrupt handler
 * @param[in]  arg      an argument that will be passed to the interrupt handler
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    GPIOx_Type* GPIOx = NULL;
    
    if ((NULL == gpio) ||
        (NULL == handler) ||
        (NULL == arg))
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }
    
    g_gpio_irq_handler = handler;
    g_arg = arg;
    g_port = gpio->port;
    GPIO_EXTI_Init( GPIOx, g_gpio_pin[gpio->port & 0x0F], trigger_transform(trigger));    //input irq enable
    
    return 0;
}

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }

    GPIO_EXTI_Init( GPIOx, g_gpio_pin[gpio->port & 0x0F], EXTI_DISABLE);  //disable EXTI
    
    return 0;
}

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }

    GPIO_EXTI_Init( GPIOx, g_gpio_pin[gpio->port & 0x0F], EXTI_DISABLE);  //disable EXTI
    
    return 0;
}

/**
 * Set a GPIO pin in default state.
 *
 * @param[in]  gpio  the gpio pin which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    GPIOx_Type* GPIOx = NULL;
    
    if (NULL == gpio)
    {
        return EIO;
    }
    
    if (EIO == port_transform_GPIOx(gpio->port, &GPIOx))
    {
        return EIO;
    }
    
    CloseIO(GPIOx, g_gpio_pin[gpio->port & 0x0F]);
    
    return 0;
}

#endif /* HAL_GPIO_H */

