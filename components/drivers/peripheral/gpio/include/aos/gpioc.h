/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIOC_H
#define AOS_GPIOC_H

#include <aos/device_core.h>

#define AOS_GPIO_DIR_MASK                       ((uint32_t)0xF << 0)
#define AOS_GPIO_DIR_NONE                       ((uint32_t)0x0 << 0)
#define AOS_GPIO_DIR_INPUT                      ((uint32_t)0x1 << 0)
#define AOS_GPIO_DIR_OUTPUT                     ((uint32_t)0x2 << 0)

#define AOS_GPIO_INPUT_CFG_MASK                 ((uint32_t)0xF << 4)
#define AOS_GPIO_INPUT_CFG_DEFAULT              ((uint32_t)0x0 << 4)
#define AOS_GPIO_INPUT_CFG_HI                   ((uint32_t)0x1 << 4)
#define AOS_GPIO_INPUT_CFG_PU                   ((uint32_t)0x2 << 4)
#define AOS_GPIO_INPUT_CFG_PD                   ((uint32_t)0x3 << 4)

#define AOS_GPIO_IRQ_MASK                       ((uint32_t)0xF << 8)
#define AOS_GPIO_IRQ_NONE                       ((uint32_t)0x0 << 8)
#define AOS_GPIO_IRQ_EDGE_RISING                ((uint32_t)0x1 << 8)
#define AOS_GPIO_IRQ_EDGE_FALLING               ((uint32_t)0x2 << 8)
#define AOS_GPIO_IRQ_EDGE_BOTH                  ((uint32_t)0x3 << 8)
#define AOS_GPIO_IRQ_LEVEL_HIGH                 ((uint32_t)0x4 << 8)
#define AOS_GPIO_IRQ_LEVEL_LOW                  ((uint32_t)0x5 << 8)

#define AOS_GPIO_OUTPUT_CFG_MASK                ((uint32_t)0xF << 4)
#define AOS_GPIO_OUTPUT_CFG_DEFAULT             ((uint32_t)0x0 << 4)
#define AOS_GPIO_OUTPUT_CFG_PP                  ((uint32_t)0x1 << 4)
#define AOS_GPIO_OUTPUT_CFG_ODNP                ((uint32_t)0x2 << 4)
#define AOS_GPIO_OUTPUT_CFG_ODPU                ((uint32_t)0x3 << 4)

#define AOS_GPIO_OUTPUT_INIT_MASK               ((uint32_t)0x1 << 8)
#define AOS_GPIO_OUTPUT_INIT_LOW                ((uint32_t)0x0 << 8)
#define AOS_GPIO_OUTPUT_INIT_HIGH               ((uint32_t)0x1 << 8)

struct aos_gpioc;

typedef struct {
    void (*unregister)(struct aos_gpioc *);
    aos_status_t (*set_mode)(struct aos_gpioc *, uint32_t);
    int (*get_value)(struct aos_gpioc *, uint32_t);
    void (*set_value)(struct aos_gpioc *, uint32_t);
} aos_gpioc_ops_t;

typedef void (*aos_gpio_irq_handler_t)(int trig, void *arg);

typedef struct {
    uint32_t mode;
    aos_gpio_irq_handler_t irq_handler;
    void *irq_arg;
    int value;
} aos_gpioc_pin_t;

typedef struct aos_gpioc {
    aos_dev_t dev;
    const aos_gpioc_ops_t *ops;
    uint32_t num_pins;
    aos_gpioc_pin_t pins[0];
} aos_gpioc_t;

typedef aos_dev_ref_t aos_gpioc_ref_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_gpioc_register(aos_gpioc_t *gpioc);
aos_status_t aos_gpioc_unregister(uint32_t id);
/**
 * @brief       Get a GPIO controller device.
 * @param[out]  ref     GPIO controller ref to operate
 * @param[in]   id      GPIO controller device ID
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_get(aos_gpioc_ref_t *ref, uint32_t id);
/**
 * @brief       Release a GPIO controller device.
 * @param[in]   ref     GPIO controller ref to operate
 * @return      None
 */
void aos_gpioc_put(aos_gpioc_ref_t *ref);
/**
 * @brief       Get number of pins on a GPIO controller device.
 * @param[in]   ref     GPIO controller ref to operate
 * @return      >= 0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_get_num_pins(aos_gpioc_ref_t *ref);
/**
 * @brief       Get the parameters associated with a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @param[out]  mode    pin mode
 * @return      0: on success; < 0: on failure
 */
aos_status_t
aos_gpioc_get_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *mode);
/**
 * @brief       Set the parameters associated with a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @param[in]   mode    pin mode
 * @return      0: on success; < 0: on failure
 */
aos_status_t
aos_gpioc_set_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t mode);
/**
 * @brief       Set the parameters and IRQ settings associated with a GPIO pin.
 * @param[in]   ref             GPIO controller ref to operate
 * @param[in]   pin             pin to operate
 * @param[in]   mode            pin mode
 * @param[in]   irq_handler     IRQ handler
 * @param[in]   irq_arg         argument passed to IRQ handler
 * @param[in]   irq_prio        priority of IRQ handler task
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_set_mode_irq(aos_gpioc_ref_t *ref,
                                    uint32_t pin, uint32_t mode,
                                    aos_gpio_irq_handler_t irq_handler,
                                    void *irq_arg, int32_t irq_prio);
/**
 * @brief       Get the input level of a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @return      0: low level; > 0: high level; < 0: on failure
 */
aos_status_t aos_gpioc_get_value(aos_gpioc_ref_t *ref, uint32_t pin);
/**
 * @brief       Set the output level of a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @param[in]   val     output level
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_set_value(aos_gpioc_ref_t *ref, uint32_t pin, int val);
/**
 * @brief       Toggle the output level of a GPIO pin.
 * @param[in]   ref     GPIO controller ref to operate
 * @param[in]   pin     pin to operate
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_gpioc_toggle(aos_gpioc_ref_t *ref, uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif /* AOS_GPIOC_H */
