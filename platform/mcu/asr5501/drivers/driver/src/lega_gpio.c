#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_gpio.h"

lega_gpio_cb_t g_lega_gpio_handler[LEGA_GPIO_TOTAL_NUM];
//gpio pinmux function table
const uint32_t GPIO_PINMUX_FUN[LEGA_GPIO_TOTAL_NUM] =
{
#if (defined LEGA_A0V1)
    0, 0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 4, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
#elif (defined LEGA_A0V2)
    0, 0, 0, 0, 1, 1, 0, 0,
    0, 0, 2, 0, 0, 0, 4, 4,
    0, 0, 0, 0, 0, 0, 0, 0
#endif
};

void GPIO_IRQHandler(void)
{
    lega_intrpt_enter();
    for(int i = 0; i < LEGA_GPIO_TOTAL_NUM; i++)
    {
        if(i < LEGA_GPIO_NUM_PER_GROUP)
        {
            //gpio group0 irq
            if(GPIO_GROUP0->INTSTATUS & (0x0001 << i))
            {
                //clear GPIO GROUP0 interrupt
                GPIO_GROUP0->INTSTATUS = (0x0001 << i);
                if(g_lega_gpio_handler[i].cb)
                {
                    g_lega_gpio_handler[i].cb(g_lega_gpio_handler[i].arg);
                }
            }
        }
        else
        {
            //gpio group1 irq
            if(GPIO_GROUP1->INTSTATUS & (0x0001 << (i-LEGA_GPIO_NUM_PER_GROUP)))
            {
                //clear GPIO GROUP1 interrupt
                GPIO_GROUP1->INTSTATUS = (0x0001 << (i-LEGA_GPIO_NUM_PER_GROUP));
                if(g_lega_gpio_handler[i].cb)
                {
                    g_lega_gpio_handler[i].cb(g_lega_gpio_handler[i].arg);
                }
            }
        }
    }
    lega_intrpt_exit();
}

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
int32_t lega_gpio_init(lega_gpio_dev_t *gpio)
{
    uint32_t reg_value;
    if(NULL == gpio)
    {
        return -1;
    }
    //pinmux setting
    if(gpio->port < 8)
    {
        reg_value = REG_RD(PINMUX_CTRL_REG0) & (~(0x0000000F << (4*gpio->port)));
        REG_WR(PINMUX_CTRL_REG0, (reg_value|(GPIO_PINMUX_FUN[gpio->port] << (4*gpio->port))));
    }
    else if(gpio->port < 16)
    {
        reg_value = REG_RD(PINMUX_CTRL_REG1) & (~(0x0000000F << (4*(gpio->port-8))));
        REG_WR(PINMUX_CTRL_REG1, (reg_value|(GPIO_PINMUX_FUN[gpio->port] << (4*(gpio->port-8)))));
    }
    else if(gpio->port < 24)
    {
        reg_value = REG_RD(PINMUX_CTRL_REG2) & (~(0x0000000F << (4*(gpio->port-16))));
        REG_WR(PINMUX_CTRL_REG2, (reg_value|(GPIO_PINMUX_FUN[gpio->port] << (4*(gpio->port-16)))));
    }
#if (defined LEGA_A0V1)
    else if(gpio->port < 32)
    {
        reg_value = REG_RD(PINMUX_CTRL_REG3) & (~(0x0000000F << (4*(gpio->port-24))));
        REG_WR(PINMUX_CTRL_REG3, (reg_value|(GPIO_PINMUX_FUN[gpio->port] << (4*(gpio->port-24)))));
    }
#endif
    else
    {
        return -1;
    }
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        switch(gpio->config)
        {
            case LEGA_ANALOG_MODE:
                break;
            case LEGA_IRQ_MODE:
            case LEGA_INPUT_PULL_UP:
                if(GPIO_GROUP0->OUTENSET & (1 << gpio->port))
                {
                    GPIO_GROUP0->OUTENCLR = (1 << gpio->port);
                }
            #if (defined LEGA_A0V2)
                reg_value = REG_RD(HW_CTRL_PE_PS);
                REG_WR(HW_CTRL_PE_PS, (reg_value & (~(1 << gpio->port))));
            #endif
                reg_value = REG_RD(PAD_PS_REG);
                REG_WR(PAD_PS_REG, (reg_value | (1 << gpio->port)));
                reg_value = REG_RD(PAD_PE_REG);
                REG_WR(PAD_PE_REG, (reg_value | (1 << gpio->port)));
                break;
            case LEGA_INPUT_PULL_DOWN:
                if(GPIO_GROUP0->OUTENSET & (1 << gpio->port))
                {
                    GPIO_GROUP0->OUTENCLR = (1 << gpio->port);
                }
            #if (defined LEGA_A0V2)
                reg_value = REG_RD(HW_CTRL_PE_PS);
                REG_WR(HW_CTRL_PE_PS, (reg_value & (~(1 << gpio->port))));
            #endif
                reg_value = REG_RD(PAD_PS_REG);
                REG_WR(PAD_PS_REG, (reg_value & (~(1 << gpio->port))));
                reg_value = REG_RD(PAD_PE_REG);
                REG_WR(PAD_PE_REG, (reg_value | (1 << gpio->port)));
                break;
            case LEGA_INPUT_HIGH_IMPEDANCE:
                if(GPIO_GROUP0->OUTENSET & (1 << gpio->port))
                {
                    GPIO_GROUP0->OUTENCLR = (1 << gpio->port);
                }
                break;
            case LEGA_OUTPUT_PUSH_PULL:
            case LEGA_OUTPUT_OPEN_DRAIN_NO_PULL:
            case LEGA_OUTPUT_OPEN_DRAIN_PULL_UP:
                if(!(GPIO_GROUP0->OUTENSET & (1 << gpio->port)))
                {
                    GPIO_GROUP0->OUTENSET = (1 << gpio->port);
                }
                break;
            default:
                return -1;
                //break;
        }
    }
    else //if(gpio->port < 32/24)
    {
        switch(gpio->config)
        {
            case LEGA_ANALOG_MODE:
                break;
            case LEGA_IRQ_MODE:
                break;
            case LEGA_INPUT_PULL_UP:
                if(GPIO_GROUP1->OUTENSET & (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP)))
                {
                    GPIO_GROUP1->OUTENCLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                }
            #if (defined LEGA_A0V2)
                reg_value = REG_RD(HW_CTRL_PE_PS);
                REG_WR(HW_CTRL_PE_PS, (reg_value & (~(1 << gpio->port))));
            #endif
                reg_value = REG_RD(PAD_PS_REG);
                REG_WR(PAD_PS_REG, (reg_value | (1 << gpio->port)));
                reg_value = REG_RD(PAD_PE_REG);
                REG_WR(PAD_PE_REG, (reg_value | (1 << gpio->port)));
                break;
            case LEGA_INPUT_PULL_DOWN:
                if(GPIO_GROUP1->OUTENSET & (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP)))
                {
                    GPIO_GROUP1->OUTENCLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                }
            #if (defined LEGA_A0V2)
                reg_value = REG_RD(HW_CTRL_PE_PS);
                REG_WR(HW_CTRL_PE_PS, (reg_value & (~(1 << gpio->port))));
            #endif
                reg_value = REG_RD(PAD_PS_REG);
                REG_WR(PAD_PS_REG, (reg_value & (~(1 << gpio->port))));
                reg_value = REG_RD(PAD_PE_REG);
                REG_WR(PAD_PE_REG, (reg_value | (1 << gpio->port)));
                break;
            case LEGA_INPUT_HIGH_IMPEDANCE:
                if(GPIO_GROUP1->OUTENSET & (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP)))
                {
                    GPIO_GROUP1->OUTENCLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                }
                break;
            case LEGA_OUTPUT_PUSH_PULL:
            case LEGA_OUTPUT_OPEN_DRAIN_NO_PULL:
            case LEGA_OUTPUT_OPEN_DRAIN_PULL_UP:
                if(!(GPIO_GROUP1->OUTENSET & (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP))))
                {
                    GPIO_GROUP1->OUTENSET = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                }
                break;
            default:
                return -1;
                //break;
        }
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
int32_t lega_gpio_output_high(lega_gpio_dev_t *gpio)
{
    if(NULL == gpio)
    {
        return -1;
    }
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        GPIO_GROUP0->DATAOUT |= (1 << gpio->port);
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        GPIO_GROUP1->DATAOUT |= (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
    }
    else
    {
        return -1;
    }
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
int32_t lega_gpio_output_low(lega_gpio_dev_t *gpio)
{
    if(NULL == gpio)
    {
        return -1;
    }
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        GPIO_GROUP0->DATAOUT &= ~(1 << gpio->port);
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        GPIO_GROUP1->DATAOUT &= ~(1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
    }
    else
    {
        return -1;
    }
    return 0;
}

/**
 * Get the state of an output GPIO pin. Using this function on a
 * gpio pin which is set to input mode will return an undefined value.
 *
 * @param[in]  gpio   the gpio pin which should be read
 * @param[in]  value  gpio value
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 *
 * note: defined by asr
 */
int32_t lega_gpio_output_get(lega_gpio_dev_t *gpio, uint32_t *value)
{
    if(NULL == gpio)
    {
        return -1;
    }
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        *value = (GPIO_GROUP0->DATAOUT >> (gpio->port)) & 1;
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        *value = (GPIO_GROUP1->DATAOUT >> ((gpio->port-LEGA_GPIO_NUM_PER_GROUP))) & 1;
    }
    else
    {
        return -1;
    }
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
int32_t lega_gpio_output_toggle(lega_gpio_dev_t *gpio)
{
    uint32_t value;
    if(NULL == gpio)
    {
        return -1;
    }
    if(0 == lega_gpio_output_get(gpio, &value))
    {
        if(value)
        {
            return lega_gpio_output_low(gpio);
        }
        else
        {
            return lega_gpio_output_high(gpio);
        }
    }
    else
    {
        return -1;
    }
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
int32_t lega_gpio_input_get(lega_gpio_dev_t *gpio, uint32_t *value)
{
    if(NULL == gpio)
    {
        return -1;
    }
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        *value = (GPIO_GROUP0->DATA >> gpio->port) & 1;
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        *value = (GPIO_GROUP1->DATA >> (gpio->port-LEGA_GPIO_NUM_PER_GROUP)) & 1;
    }
    else
    {
        return -1;
    }
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
int32_t lega_gpio_enable_irq(lega_gpio_dev_t *gpio, lega_gpio_irq_trigger_t trigger,
                            lega_gpio_irq_handler_t handler, void *arg)
{
    if(NULL == gpio)
    {
        return -1;
    }
    g_lega_gpio_handler[gpio->port].cb = handler;
    g_lega_gpio_handler[gpio->port].arg = arg;
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        switch(trigger)
        {
            case LEGA_IRQ_TRIGGER_RISING_EDGE:
                GPIO_GROUP0->INTTYPESET = (1 << gpio->port); //edge or level trig
                GPIO_GROUP0->INTPOLSET = (1 << gpio->port); //trig polarity
                break;
            case LEGA_IRQ_TRIGGER_FALLING_EDGE:
                GPIO_GROUP0->INTTYPESET = (1 << gpio->port);
                GPIO_GROUP0->INTPOLCLR = (1 << gpio->port);
                break;
            case LEGA_IRQ_TRIGGER_BOTH_EDGES:
            default:
                return -1;
        }
        GPIO_GROUP0->INTENSET = (1 << gpio->port); //int enable
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        switch(trigger)
        {
            case LEGA_IRQ_TRIGGER_RISING_EDGE:
                GPIO_GROUP1->INTTYPESET = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                GPIO_GROUP1->INTPOLSET = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                break;
            case LEGA_IRQ_TRIGGER_FALLING_EDGE:
                GPIO_GROUP1->INTTYPESET = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                GPIO_GROUP1->INTPOLCLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
                break;
            case LEGA_IRQ_TRIGGER_BOTH_EDGES:
            default:
                return -1;
        }
        GPIO_GROUP1->INTENSET = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
    }
    else
    {
        return -1;
    }
    NVIC_EnableIRQ(GPIO_IRQn); //0x10
    return 0;
}

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref lega_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_disable_irq(lega_gpio_dev_t *gpio)
{
    if(NULL == gpio)
    {
        return -1;
    }
    g_lega_gpio_handler[gpio->port].cb = NULL;
    g_lega_gpio_handler[gpio->port].arg = NULL;
    //NVIC_DisableIRQ(GPIO_IRQn); //common use
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        GPIO_GROUP0->INTENCLR = (1 << gpio->port);
        GPIO_GROUP0->INTTYPECLR = (1 << gpio->port);
        GPIO_GROUP0->INTPOLCLR = (1 << gpio->port);
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        GPIO_GROUP1->INTTYPECLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
        GPIO_GROUP1->INTPOLCLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
        GPIO_GROUP1->INTENCLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
    }
    else
    {
        return -1;
    }
    return 0;
}

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref lega_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_clear_irq(lega_gpio_dev_t *gpio)
{
    if(NULL == gpio)
    {
        return -1;
    }

    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        //GPIO interrupt status
        if(GPIO_GROUP0->INTSTATUS & (1 << gpio->port))
        {
            //clear GPIO interrupt status
            GPIO_GROUP0->INTSTATUS = (1 << gpio->port);
        }
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        if(GPIO_GROUP1->INTSTATUS & (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP)))
        {
            GPIO_GROUP1->INTSTATUS = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

/**
 * Set a GPIO pin in default state.
 *
 * @param[in]  gpio  the gpio pin which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_finalize(lega_gpio_dev_t *gpio)
{
    if(NULL == gpio)
    {
        return -1;
    }
    if(gpio->port < LEGA_GPIO_NUM_PER_GROUP)
    {
        if(GPIO_GROUP0->OUTENSET & (1 << gpio->port))
        {
            GPIO_GROUP0->OUTENCLR = (1 << gpio->port);
        }
    }
    else if(gpio->port < LEGA_GPIO_TOTAL_NUM)
    {
        if(GPIO_GROUP1->OUTENSET & (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP)))
        {
            GPIO_GROUP1->OUTENCLR = (1 << (gpio->port-LEGA_GPIO_NUM_PER_GROUP));
        }
    }
    else
    {
        return -1;
    }
    lega_gpio_disable_irq(gpio);
    lega_gpio_clear_irq(gpio);
    return 0;
}
