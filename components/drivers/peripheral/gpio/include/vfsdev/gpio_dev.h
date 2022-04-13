/*
 * Copyright ( C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _IO_GPIO_H_
#define _IO_GPIO_H_

#define IOC_GPIO_BASE 'G'
#define IOC_GPIO_SET IOC_GPIO_BASE + 1
#define IOC_GPIO_GET IOC_GPIO_BASE + 2
#define IOC_GPIO_SET_IRQ IOC_GPIO_BASE + 3

enum GPIO_IO_BITS {
    GPIO_IO_INPUT_BIT = 0, // Input
    GPIO_IO_OUTPUT_BIT,    // Output
    GPIO_IO_INPUT_PU_BIT,  // Pull up
    GPIO_IO_INPUT_PD_BIT,  // Pull down
    GPIO_IO_INPUT_HI_BIT,  // High impedance
    GPIO_IO_OUTPUT_PP_BIT, // Push high and pull low actively
    GPIO_IO_OUTPUT_ODNP_BIT, // Open Drain No Pull
    GPIO_IO_OUTPUT_ODPU_BIT, // Open Drain Pull Up
    GPIO_IO_OUTPUT_TOGGLE_BIT, // get GPIO output level reverted
    GPIO_IO_INPUT_OUTPUT_MAX_BIT,
};

#define GPIO_IO_INPUT          (1 << GPIO_IO_INPUT_BIT)
#define GPIO_IO_OUTPUT         (1 << GPIO_IO_OUTPUT_BIT)
#define GPIO_IO_INPUT_PU       (1 << GPIO_IO_INPUT_PU_BIT)
#define GPIO_IO_INPUT_PD       (1 << GPIO_IO_INPUT_PD_BIT)
#define GPIO_IO_INPUT_HI       (1 << GPIO_IO_INPUT_HI_BIT)
#define GPIO_IO_OUTPUT_PP      (1 << GPIO_IO_OUTPUT_PP_BIT)
#define GPIO_IO_OUTPUT_ODNP    (1 << GPIO_IO_OUTPUT_ODNP_BIT)
#define GPIO_IO_OUTPUT_ODPU    (1 << GPIO_IO_OUTPUT_ODPU_BIT)
#define GPIO_IO_OUTPUT_TOGGLE    (1 << GPIO_IO_OUTPUT_TOGGLE_BIT)

#define GPIO_IO_INPUT_MASK (GPIO_IO_INPUT_PU | GPIO_IO_INPUT_PD | GPIO_IO_INPUT_HI)
#define GPIO_IO_OUTPUT_MASK (GPIO_IO_OUTPUT_PP | GPIO_IO_OUTPUT_ODNP | GPIO_IO_OUTPUT_ODPU | GPIO_IO_OUTPUT_TOGGLE)

enum GPIO_IRQ_BITS {
    GPIO_IRQ_ENABLE_BIT = 0,
    GPIO_IRQ_DISABLE_BIT,
    GPIO_IRQ_CLEAR_BIT,
    GPIO_IRQ_LEVEL_LOW_BIT,
    GPIO_IRQ_LEVEL_HIGH_BIT,
    GPIO_IRQ_EDGE_FALLING_BIT,
    GPIO_IRQ_EDGE_RISING_BIT,
    GPIO_IRQ_EDGE_BOTH_BIT,
};

#define GPIO_IRQ_ENABLE       (1 << GPIO_IRQ_ENABLE_BIT)
#define GPIO_IRQ_DISABLE      (1 << GPIO_IRQ_DISABLE_BIT)
#define GPIO_IRQ_CLEAR        (1 << GPIO_IRQ_CLEAR_BIT)
#define GPIO_IRQ_LEVEL_LOW    (1 << GPIO_IRQ_LEVEL_LOW_BIT)
#define GPIO_IRQ_LEVEL_HIGH   (1 << GPIO_IRQ_LEVEL_HIGH_BIT)
#define GPIO_IRQ_EDGE_FALLING (1 << GPIO_IRQ_EDGE_FALLING_BIT)
#define GPIO_IRQ_EDGE_RISING  (1 << GPIO_IRQ_EDGE_RISING_BIT)
#define GPIO_IRQ_EDGE_BOTH    (1 << GPIO_IRQ_EDGE_BOTH_BIT)
#define GPIO_IRQ_MODE_MASK (GPIO_IRQ_LEVEL_LOW | GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_EDGE_FALLING | GPIO_IRQ_EDGE_RISING | GPIO_IRQ_EDGE_BOTH)

typedef struct gpio_io_config {
    unsigned int id:8;        // GPIO id
    unsigned int config:16;   // GPIO config (directin/pull up/down, etc.)
    unsigned int data:1;      // output data
} gpio_io_config_t;

typedef int (*io_irq_handler_t)(void *arg);

typedef struct gpio_irq_config {
    unsigned int id:8;  // GPIO id
    unsigned int config:16; // IRQ config (enable/disable, trigger type, clear, etc.)
    unsigned char *arg; // argument used to parsed to irq callback
    io_irq_handler_t cb;
} gpio_irq_config_t;

int vfs_gpio_drv_init (void);

#endif //_IO_GPIO_H_
