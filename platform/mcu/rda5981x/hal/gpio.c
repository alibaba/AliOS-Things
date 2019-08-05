#include "aos/hal/gpio.h"
#include <stdio.h>
#include "pinnames.h"
#include "gpio_api.h"

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    if (gpio->port >= GPIO_NUM)
        return -1;

    gpio->priv = &gpio_obj[gpio->port];
    gpio_init(gpio->priv, gpio_map[gpio->port]);

    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    gpio_dir(gpio->priv, PIN_OUTPUT);
    gpio_write(gpio->priv, 1);

    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    gpio_dir(gpio->priv, PIN_OUTPUT);
    gpio_write(gpio->priv, 0);

    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    int32_t val;

    val = ((gpio_t *)gpio->priv)->reg_out;
    val ^= 1;
    gpio_dir(gpio->priv, PIN_OUTPUT);
    gpio_write(gpio->priv, val);

    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    int32_t rc = -1;

    if(NULL != gpio && NULL != value) {
        gpio_dir(gpio->priv, PIN_INPUT);
        *value = gpio_read(gpio->priv);
        rc = 0;
    }

    return rc;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    gpio_dir(gpio->priv, PIN_INPUT);
    gpio_irq_init(gpio->priv, (unsigned int)handler, (unsigned int)arg);
    if(trigger == IRQ_TRIGGER_RISING_EDGE){
        gpio_irq_set(gpio->priv, IRQ_RISE, 1);
    }else if(trigger == IRQ_TRIGGER_FALLING_EDGE){
        gpio_irq_set(gpio->priv, IRQ_FALL, 1);
    }else{
        gpio_irq_set(gpio->priv, IRQ_RISE, 1);
        gpio_irq_set(gpio->priv, IRQ_FALL, 1);
    }
    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    gpio_irq_free(gpio->priv);
    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    gpio_irq_free(gpio->priv);
    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    return 0;
}

