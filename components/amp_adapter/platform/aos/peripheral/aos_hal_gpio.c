/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vfsdev/gpio_dev.h>
#include "aos_hal_gpio.h"

#ifdef CONFIG_GPIO_NUM
#define PLATFORM_GPIO_NUM CONFIG_GPIO_NUM
#else
#define PLATFORM_GPIO_NUM 40
#endif


int32_t aos_hal_gpio_init(gpio_dev_t *gpio)
{
    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    return 0;
}

int32_t aos_hal_gpio_output_high(gpio_dev_t *gpio)
{
    gpio_io_config_t config;
    int fd;
    int r;

    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    fd = open("/dev/gpio", 0);
    if (fd < 0)
        return -1;

    config.id = gpio->port;
    config.config = GPIO_IO_OUTPUT;

    switch (gpio->config) {
        case OUTPUT_PUSH_PULL:
            config.config |= GPIO_IO_OUTPUT_PP;
            break;
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            config.config |= GPIO_IO_OUTPUT_ODPU;
            break;
        case OUTPUT_OPEN_DRAIN_NO_PULL:
            config.config |= GPIO_IO_OUTPUT_ODNP;
        default:
            break;
    }

    config.data = 1;
    r = ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
    close(fd);

    return r < 0 ? -1 : 0;
}

int32_t aos_hal_gpio_output_low(gpio_dev_t *gpio)
{
    gpio_io_config_t config;
    int fd;
    int r;

    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    fd = open("/dev/gpio", 0);
    if (fd < 0)
        return -1;

    config.id = gpio->port;
    config.config = GPIO_IO_OUTPUT;

    switch (gpio->config) {
        case OUTPUT_PUSH_PULL:
            config.config |= GPIO_IO_OUTPUT_PP;
            break;
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            config.config |= GPIO_IO_OUTPUT_ODPU;
            break;
        case OUTPUT_OPEN_DRAIN_NO_PULL:
            config.config |= GPIO_IO_OUTPUT_ODNP;
        default:
            break;
    }

    config.data = 0;
    r = ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
    close(fd);

    return r < 0 ? -1 : 0;
}

int32_t aos_hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    gpio_io_config_t config;
    int fd;
    int r;

    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    fd = open("/dev/gpio", 0);
    if (fd < 0)
        return -1;

    config.id = gpio->port;
    config.config = GPIO_IO_OUTPUT_TOGGLE;
    config.data = 0;
    r = ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
    close(fd);

    return r < 0 ? -1 : 0;
}

int32_t aos_hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    gpio_io_config_t config;
    int fd;
    int r;

    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM || !value)
        return -1;

    fd = open("/dev/gpio", 0);
    if (fd < 0)
        return -1;

    config.id = gpio->port;
    config.config = GPIO_IO_INPUT;

    switch (gpio->config) {
        case INPUT_PULL_UP:
            config.config |= GPIO_IO_INPUT_PU;
            break;
        case INPUT_PULL_DOWN:
            config.config |= GPIO_IO_INPUT_PD;
            break;
        case INPUT_HIGH_IMPEDANCE:
        default:
            config.config |= GPIO_IO_INPUT_HI;
            break;
    }

    config.data = 0;
    r = ioctl(fd, IOC_GPIO_GET, (unsigned long)&config);
    close(fd);
    *value = (r < 0) ? 0 : r;

    return r < 0 ? -1 : 0;
}

int32_t aos_hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                                gpio_irq_handler_t handler, void *arg)
{
    gpio_irq_config_t config;
    int fd;
    int r;

    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    fd = open("/dev/gpio", 0);
    if (fd < 0)
        return -1;

    config.id = gpio->port;
    config.config = GPIO_IRQ_ENABLE;

    switch (trigger) {
        case IRQ_TRIGGER_RISING_EDGE:
            config.config |= GPIO_IRQ_EDGE_RISING;
            break;
        case IRQ_TRIGGER_FALLING_EDGE:
            config.config |= GPIO_IRQ_EDGE_FALLING;
            break;
        case IRQ_TRIGGER_BOTH_EDGES:
        default:
            config.config |= GPIO_IRQ_EDGE_BOTH;
            break;
    }

    config.cb = (io_irq_handler_t)handler;
    config.arg = arg;
    r = ioctl(fd, IOC_GPIO_SET_IRQ, (unsigned long)&config);
    close(fd);

    return r < 0 ? -1 : 0;
}

int32_t aos_hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    gpio_irq_config_t config;
    int fd;
    int r;

    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    fd = open("/dev/gpio", 0);
    if (fd < 0)
        return -1;

    config.id = gpio->port;
    config.config = GPIO_IRQ_DISABLE;
    config.cb = NULL;
    config.arg = NULL;
    r = ioctl(fd, IOC_GPIO_SET_IRQ, (unsigned long)&config);
    close(fd);

    return r < 0 ? -1 : 0;
}

int32_t aos_hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    return aos_hal_gpio_disable_irq(gpio);
}

int32_t aos_hal_gpio_finalize(gpio_dev_t *gpio)
{
    if (!gpio || gpio->port >= PLATFORM_GPIO_NUM)
        return -1;

    return 0;
}
