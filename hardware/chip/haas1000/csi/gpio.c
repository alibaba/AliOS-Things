/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <drv/gpio.h>
#include <hal_gpio.h>
#include <aos/gpioc_csi.h>

#define GPIO_PINS_PER_PORT      (sizeof(uint32_t) * 8)
#define GPIO_NUM_PORTS          ((HAL_GPIO_PIN_LED_NUM + GPIO_PINS_PER_PORT - 1) / GPIO_PINS_PER_PORT)

static aos_gpioc_csi_t gpioc_csi[GPIO_NUM_PORTS];
static csi_gpio_irq_mode_t irq_modes[GPIO_NUM_PORTS][GPIO_PINS_PER_PORT];

csi_error_t csi_gpio_init(csi_gpio_t *gpio, uint32_t port_idx)
{
    if (!gpio)
        return CSI_ERROR;

    if (port_idx >= GPIO_NUM_PORTS)
        return CSI_ERROR;

    gpio->dev.idx = port_idx;
    gpio->callback = NULL;
    gpio->arg = NULL;

    return CSI_OK;
}

void csi_gpio_uninit(csi_gpio_t *gpio)
{
}

csi_error_t csi_gpio_dir(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_dir_t dir)
{
    if (!gpio)
        return CSI_ERROR;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return CSI_ERROR;

    for (uint32_t i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if (pin_mask & ((uint32_t)1 << i)) {
            struct HAL_IOMUX_PIN_FUNCTION_MAP cfg;
            enum HAL_GPIO_DIR_T d;
            cfg.pin = gpio->dev.idx * GPIO_PINS_PER_PORT + i;
            cfg.function = HAL_IOMUX_FUNC_AS_GPIO;
            cfg.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
            cfg.pull_sel = HAL_IOMUX_PIN_PULLUP_ENALBE;
            hal_iomux_init(&cfg, 1);
            d = (dir == GPIO_DIRECTION_INPUT) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
            hal_gpio_pin_set_dir(cfg.pin, d, 0);
        }
    }

    return CSI_OK;
}

csi_error_t csi_gpio_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_mode_t mode)
{
    if (!gpio)
        return CSI_ERROR;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return CSI_ERROR;

    for (uint32_t i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if (pin_mask & ((uint32_t)1 << i)) {
            struct HAL_IOMUX_PIN_FUNCTION_MAP cfg;
            enum HAL_GPIO_DIR_T d;
            cfg.pin = gpio->dev.idx * GPIO_PINS_PER_PORT + i;
            cfg.function = HAL_IOMUX_FUNC_AS_GPIO;
            cfg.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
            switch (mode) {
            case GPIO_MODE_PULLNONE:
                cfg.pull_sel = HAL_IOMUX_PIN_NOPULL;
                d = HAL_GPIO_DIR_IN;
                break;
            case GPIO_MODE_PULLUP:
                cfg.pull_sel = HAL_IOMUX_PIN_PULLUP_ENALBE;
                d = HAL_GPIO_DIR_IN;
                break;
            case GPIO_MODE_PULLDOWN:
                cfg.pull_sel = HAL_IOMUX_PIN_PULLDOWN_ENALBE;
                d = HAL_GPIO_DIR_IN;
                break;
            case GPIO_MODE_OPEN_DRAIN:
                cfg.pull_sel = HAL_IOMUX_PIN_NOPULL;
                d = HAL_GPIO_DIR_OUT;
                break;
            case GPIO_MODE_PUSH_PULL:
                cfg.pull_sel = HAL_IOMUX_PIN_PULLDOWN_ENALBE;
                d = HAL_GPIO_DIR_OUT;
                break;
            default:
                cfg.pull_sel = HAL_IOMUX_PIN_NOPULL;
                d = HAL_GPIO_DIR_IN;
                break;
            }
            hal_iomux_init(&cfg, 1);
            hal_gpio_pin_set_dir(cfg.pin, d, 0);
        }
    }

    return CSI_OK;
}

csi_error_t csi_gpio_irq_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_irq_mode_t mode)
{
    if (!gpio)
        return CSI_ERROR;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return CSI_ERROR;

    switch (mode) {
    case GPIO_IRQ_MODE_RISING_EDGE:
    case GPIO_IRQ_MODE_FALLING_EDGE:
        break;
    default:
        return CSI_ERROR;
    }

    for (uint32_t i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if (pin_mask & ((uint32_t)1 << i))
            irq_modes[gpio->dev.idx][i] = mode;
    }

    return CSI_OK;
}

static void irq_handler(enum HAL_GPIO_PIN_T pin)
{
    uint32_t port = pin / GPIO_PINS_PER_PORT;
    csi_gpio_t *gpio;

    if (port >= GPIO_NUM_PORTS || pin % GPIO_PINS_PER_PORT >= gpioc_csi[port].gpioc.num_pins)
        return;

    gpio = &gpioc_csi[port].csi_gpio;
    gpio->callback(gpio, (uint32_t)1 << (pin % GPIO_PINS_PER_PORT), gpio->arg);
}

csi_error_t csi_gpio_irq_enable(csi_gpio_t *gpio, uint32_t pin_mask, bool enable)
{
    if (!gpio)
        return CSI_ERROR;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return CSI_ERROR;

    for (uint32_t i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if (pin_mask & ((uint32_t)1 << i)) {
            uint32_t pin = gpio->dev.idx * GPIO_PINS_PER_PORT + i;
            struct HAL_GPIO_IRQ_CFG_T cfg;
            if (enable) {
                csi_gpio_irq_mode_t mode;
                cfg.irq_enable = true;
                cfg.irq_debounce = true;
                mode = irq_modes[gpio->dev.idx][i];
                cfg.irq_polarity = (mode == GPIO_IRQ_MODE_RISING_EDGE) ? HAL_GPIO_IRQ_POLARITY_HIGH_RISING :
                                                                         HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
                cfg.irq_handler = irq_handler;
                cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
            } else {
                cfg.irq_enable = false;
                cfg.irq_debounce = false;
                cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
                cfg.irq_handler = NULL;
                cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
            }
            hal_gpio_setup_irq(pin, &cfg);
        }
    }

    return CSI_OK;
}

void csi_gpio_write(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_pin_state_t value)
{
    if (!gpio)
        return;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return;

    for (uint32_t i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if (pin_mask & ((uint32_t)1 << i)) {
            uint32_t pin = gpio->dev.idx * GPIO_PINS_PER_PORT + i;
            hal_gpio_pin_set_dir(pin, HAL_GPIO_DIR_OUT, !!value);
        }
    }
}

uint32_t csi_gpio_read(csi_gpio_t *gpio, uint32_t pin_mask)
{
    uint32_t ret = 0;

    if (!gpio)
        return CSI_ERROR;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return CSI_ERROR;

    for (uint32_t i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if (pin_mask & ((uint32_t)1 << i)) {
            uint32_t pin = gpio->dev.idx * GPIO_PINS_PER_PORT + i;
            ret |= (uint32_t)!!hal_gpio_pin_get_val(pin) << i;
        }
    }

    return ret;
}

csi_error_t csi_gpio_attach_callback(csi_gpio_t *gpio, void *callback, void *arg)
{
    if (!gpio)
        return CSI_ERROR;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return CSI_ERROR;

    gpio->callback = callback;
    gpio->arg = arg;

    return CSI_OK;
}

void csi_gpio_detach_callback(csi_gpio_t *gpio)
{
    if (!gpio)
        return;

    if (gpio->dev.idx >= GPIO_NUM_PORTS)
        return;

    gpio->callback = NULL;
    gpio->arg = NULL;
}

static int gpioc_csi_init(void)
{
    for (uint32_t i = 0; i < GPIO_NUM_PORTS; i++) {
        uint32_t num_pins;
        int ret;

        gpioc_csi[i].gpioc.dev.id = i;
        num_pins = HAL_GPIO_PIN_LED_NUM - GPIO_PINS_PER_PORT * i;
        if (num_pins > GPIO_PINS_PER_PORT)
            num_pins = GPIO_PINS_PER_PORT;
        gpioc_csi[i].gpioc.num_pins = num_pins;
        gpioc_csi[i].default_input_cfg = AOS_GPIO_INPUT_CFG_PU;
        gpioc_csi[i].default_output_cfg = AOS_GPIO_OUTPUT_CFG_PP;
        ret = (int)aos_gpioc_csi_register(&gpioc_csi[i]);
        if (ret) {
            for (uint32_t j = 0; j < i; j++)
                (void)aos_gpioc_csi_unregister(j);
            return ret;
        }
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(gpioc_csi_init)
