/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/hal/gpio.h"
#include "hal_gpio.h"
#include "hal_trace.h"

struct gpio_irq_handler {
    uint8_t             port;
    gpio_irq_handler_t  func;
    void                *arg;
};

static struct gpio_irq_handler bes_gpio_irq_handler[HAL_GPIO_PIN_NUM] = {0};

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
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio;

    if (gpio->port >= HAL_IOMUX_PIN_LED_NUM) {
        return -1;
    }

    cfg_hw_gpio.pin = gpio->port;
    cfg_hw_gpio.function = HAL_IOMUX_FUNC_AS_GPIO;
    cfg_hw_gpio.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
	cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_NOPULL;

    if ((gpio->config == OUTPUT_PUSH_PULL) || (gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP)
        || (gpio->config == INPUT_PULL_UP) || (gpio->config == IRQ_MODE)) {
        cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_PULLUP_ENALBE;
    } else if ((gpio->config == INPUT_PULL_DOWN)) {
        cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_PULLDOWN_ENALBE;
    }

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_hw_gpio, 1);
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
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)gpio->port, HAL_GPIO_DIR_OUT, 1);

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
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)gpio->port, HAL_GPIO_DIR_OUT, 0);

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
    uint8_t level = 0;
    level = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)gpio->port);
    level ^= 1;
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)gpio->port, HAL_GPIO_DIR_OUT, level);

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
    int32_t ret = -1;

    if(NULL != gpio && NULL != value) {
        hal_gpio_pin_set_dir(gpio->port, HAL_GPIO_DIR_IN, 0);
        *value = hal_gpio_pin_get_val(gpio->port);
        ret = 0;
    }

	return ret;
}

int32_t hal_gpio_get(gpio_dev_t *gpio, uint32_t *value)
{
    int32_t ret = -1;

    if(NULL != gpio && NULL != value) {
        *value = hal_gpio_pin_get_val(gpio->port);
        ret = 0;
    }

	return ret;
}

void bes_gpio_irq_hdl(enum HAL_GPIO_PIN_T pin)
{
    struct gpio_irq_handler *hdl_pointer = NULL;

    if (pin >= HAL_GPIO_PIN_NUM) {
        TRACE("%s %d, error pin:%d", __func__, __LINE__, pin);
        return;
    }

    //TRACE("%s %d, pin:%d", __func__, __LINE__, pin);
    hdl_pointer = &bes_gpio_irq_handler[pin];
    if (hdl_pointer->port == pin) {
        if (hdl_pointer->func) {
            hdl_pointer->func(hdl_pointer->arg);
        } else {
            TRACE("%s %d, error hdl_pointer->func == NULL", __func__, __LINE__);
        }
    } else {
        TRACE("%s %d, error pin:%d, hdl_pointer->port:%d", __func__, __LINE__, pin, hdl_pointer->port);
    }
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
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

    if (gpio->port >= HAL_GPIO_PIN_NUM) {
        TRACE("%s %d, error gpio->port:%d", __func__, __LINE__, gpio->port);
        return -1;
    }

    memset(&bes_gpio_irq_handler[gpio->port], 0, sizeof(struct gpio_irq_handler));
    bes_gpio_irq_handler[gpio->port].port = gpio->port;
    bes_gpio_irq_handler[gpio->port].func = handler;
    bes_gpio_irq_handler[gpio->port].arg  = arg;

    hal_gpio_pin_set_dir(gpio->port, HAL_GPIO_DIR_IN, 0);
    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_polarity = (trigger == IRQ_TRIGGER_RISING_EDGE) ? HAL_GPIO_IRQ_POLARITY_HIGH_RISING : HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    gpiocfg.irq_handler = bes_gpio_irq_hdl;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    hal_gpio_setup_irq(gpio->port, &gpiocfg);

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
    const struct HAL_GPIO_IRQ_CFG_T gpiocfg = {
        .irq_enable = false,
        .irq_debounce = false,
        .irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING,
        .irq_handler = NULL,
        .irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE,
    };
    hal_gpio_setup_irq(gpio->port, &gpiocfg);

	return 0;
}

/**
 * Clear an interrupt status for an input GPIO pin.
 * Using this function on a gpio pin which has generated a interrupt.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    const struct HAL_GPIO_IRQ_CFG_T gpiocfg = {
        .irq_enable = false,
        .irq_debounce = false,
        .irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING,
        .irq_handler = NULL,
        .irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE,
    };
    hal_gpio_setup_irq(gpio->port, &gpiocfg);

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
	return 0;
}

