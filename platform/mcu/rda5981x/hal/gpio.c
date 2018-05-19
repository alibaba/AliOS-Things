#include "hal/soc/soc.h"
#include "pinnames.h"
#include "gpio_api.h"

#define GPIO_NUM 28

static gpio_t gpio_obj[GPIO_NUM];
static const uint16_t gpio_map[GPIO_NUM] = {
    [0] = GPIO_PIN0,
    [1] = GPIO_PIN1,
    [2] = GPIO_PIN2,
    [3] = GPIO_PIN3,
    [4] = GPIO_PIN4,
    [5] = GPIO_PIN5,
    [6] = GPIO_PIN6,
    [7] = GPIO_PIN7,
    [8] = GPIO_PIN8,
    [9] = GPIO_PIN9,
    [10] = GPIO_PIN10,
    [11] = GPIO_PIN11,
    [12] = GPIO_PIN12,
    [13] = GPIO_PIN13,
    [14] = GPIO_PIN14,
    [15] = GPIO_PIN15,
    [16] = GPIO_PIN16,
    [17] = GPIO_PIN17,
    [18] = GPIO_PIN18,
    [19] = GPIO_PIN19,
    [20] = GPIO_PIN20,
    [21] = GPIO_PIN21,
    [22] = GPIO_PIN22,
    [23] = GPIO_PIN23,
    [24] = GPIO_PIN24,
    [25] = GPIO_PIN25,
    [26] = GPIO_PIN26,
    [27] = GPIO_PIN27,
};

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
    int32_t val;

    gpio_dir(gpio->priv, PIN_INPUT);
    val = gpio_read(gpio->priv);

    return val;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    return 0;
}

