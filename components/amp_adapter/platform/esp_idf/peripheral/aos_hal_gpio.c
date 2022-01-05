#include <errno.h>
#include <driver/gpio.h>
#include <aos_hal_gpio_internal.h>

int32_t aos_hal_gpio_init(aos_hal_gpio_dev_t *gpio)
{
    gpio_config_t config = { .intr_type = GPIO_INTR_DISABLE, };

    if (!gpio)
        return -EINVAL;

    config.pin_bit_mask = (uint64_t)1 << gpio->port;

    switch (gpio->config) {
    case IRQ_MODE:
        config.mode = GPIO_MODE_INPUT;
        config.pull_up_en = GPIO_PULLUP_ENABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        break;
    case INPUT_PULL_UP:
        config.mode = GPIO_MODE_INPUT;
        config.pull_up_en = GPIO_PULLUP_ENABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        break;
    case INPUT_PULL_DOWN:
        config.mode = GPIO_MODE_INPUT;
        config.pull_up_en = GPIO_PULLUP_DISABLE;
        config.pull_down_en = GPIO_PULLDOWN_ENABLE;
        break;
    case INPUT_HIGH_IMPEDANCE:
        config.mode = GPIO_MODE_INPUT;
        config.pull_up_en = GPIO_PULLUP_DISABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        break;
    case OUTPUT_PUSH_PULL:
        config.mode = GPIO_MODE_INPUT_OUTPUT;
        config.pull_up_en = GPIO_PULLUP_DISABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        break;
    case OUTPUT_OPEN_DRAIN_NO_PULL:
        config.mode = GPIO_MODE_INPUT_OUTPUT_OD;
        config.pull_up_en = GPIO_PULLUP_DISABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        break;
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        config.mode = GPIO_MODE_INPUT_OUTPUT_OD;
        config.pull_up_en = GPIO_PULLUP_ENABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        break;
    default:
        return -EINVAL;
    }

    if (gpio_config(&config) != ESP_OK)
        return -EINVAL;

    return 0;
}

int32_t aos_hal_gpio_finalize(aos_hal_gpio_dev_t *gpio)
{
    return 0;
}

int32_t aos_hal_gpio_get(aos_hal_gpio_dev_t *gpio)
{
    int r;

    if (!gpio)
        return -EINVAL;

    r = gpio_get_level(gpio->port);

    return (r >= 0) ? !!r : -EIO;
}

int32_t aos_hal_gpio_output_high(aos_hal_gpio_dev_t *gpio)
{
    if (!gpio)
        return -EINVAL;

    gpio->level = 1;

    if (gpio_set_level(gpio->port, gpio->level) != ESP_OK)
        return -EIO;

    return 0;
}

int32_t aos_hal_gpio_output_low(aos_hal_gpio_dev_t *gpio)
{
    if (!gpio)
        return -EINVAL;

    gpio->level = 0;

    if (gpio_set_level(gpio->port, gpio->level) != ESP_OK)
        return -EIO;

    return 0;
}

int32_t aos_hal_gpio_output_toggle(aos_hal_gpio_dev_t *gpio)
{
    if (!gpio)
        return -EINVAL;

    gpio->level = gpio->level ? 0 : 1;

    if (gpio_set_level(gpio->port, gpio->level) != ESP_OK)
        return -EIO;

    return 0;
}

static void gpio_irq_handler(void *arg)
{
    aos_hal_gpio_dev_t *gpio = (aos_hal_gpio_dev_t *)arg;

    gpio->irq_handler(0, gpio->irq_arg);
}

int32_t aos_hal_gpio_enable_irq(aos_hal_gpio_dev_t *gpio, aos_hal_gpio_irq_trigger_t trigger,
                                aos_hal_gpio_irq_handler_t handler, void *arg)
{
    gpio_int_type_t intr_type;

    if (!gpio || !handler)
        return -EINVAL;

    switch (trigger) {
    case IRQ_TRIGGER_RISING_EDGE:
        intr_type = GPIO_INTR_POSEDGE;
        break;
    case IRQ_TRIGGER_FALLING_EDGE:
        intr_type = GPIO_INTR_NEGEDGE;
        break;
    case IRQ_TRIGGER_BOTH_EDGES:
        intr_type = GPIO_INTR_ANYEDGE;
        break;
    case IRQ_TRIGGER_LEVEL_HIGH:
        intr_type = GPIO_INTR_HIGH_LEVEL;
        break;
    case IRQ_TRIGGER_LEVEL_LOW:
        intr_type = GPIO_INTR_LOW_LEVEL;
        break;
    default:
        return -EINVAL;
    }

    (void)gpio_intr_disable(gpio->port);
    (void)gpio_isr_handler_remove(gpio->port);
    gpio->irq_handler = handler;
    gpio->irq_arg = arg;

    if (gpio_isr_handler_add(gpio->port, gpio_irq_handler, gpio) != ESP_OK)
        return -EINVAL;

    if (gpio_set_intr_type(gpio->port, intr_type) != ESP_OK)
        return -EINVAL;

    if (gpio_intr_enable(gpio->port) != ESP_OK)
        return -EINVAL;

    return 0;
}

int32_t aos_hal_gpio_disable_irq(aos_hal_gpio_dev_t *gpio)
{
    if (!gpio)
        return -EINVAL;

    if (gpio_intr_disable(gpio->port) != ESP_OK)
        return -EINVAL;

    if (gpio_isr_handler_remove(gpio->port) != ESP_OK)
        return -EINVAL;

    gpio->irq_handler = NULL;
    gpio->irq_arg = NULL;

    return 0;
}
