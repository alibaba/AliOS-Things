/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIOC_H
#define AOS_GPIOC_H

#include <aos/device.h>
#include <aos/gpio.h>

/**
 * @defgroup gpioc_api GPIO Controller
 * @ingroup driver_api
 * @brief AOS API for GPIO controller.
 * @{
 */

typedef aos_dev_ref_t aos_gpioc_ref_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get a GPIO controller device.
 * @param   ref GPIO controller ref to operate.
 * @param   id  GPIO controller device ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_get(aos_gpioc_ref_t *ref, uint32_t id);
/**
 * @brief   Release a GPIO controller device.
 * @param   ref GPIO controller ref to operate.
 * @return  None.
 */
void aos_gpioc_put(aos_gpioc_ref_t *ref);
/**
 * @brief   Get number of pins on a GPIO controller device.
 * @param   ref GPIO controller ref to operate.
 * @return  >= 0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_get_num_pins(aos_gpioc_ref_t *ref);
/**
 * @brief   Get the parameters and IRQ settings associated with a GPIO pin.
 * @param   ref         GPIO controller ref to operate.
 * @param   pin         Pin to operate.
 * @param   mode        Pin mode.
 * @param   irq_handler IRQ handler.
 * @param   irq_arg     Argument passed to IRQ handler.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_get_mode_irq(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *mode,
                                    aos_gpio_irq_handler_t *irq_handler, void **irq_arg);
/**
 * @brief   Set the parameters and IRQ settings associated with a GPIO pin.
 * @param   ref         GPIO controller ref to operate.
 * @param   pin         Pin to operate.
 * @param   mode        Pin mode.
 * @param   irq_handler IRQ handler.
 * @param   irq_arg     Argument passed to IRQ handler.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_set_mode_irq(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t mode,
                                    aos_gpio_irq_handler_t irq_handler, void *irq_arg);
/**
 * @brief   Get the parameters associated with a GPIO pin.
 * @param   ref     GPIO controller ref to operate.
 * @param   pin     Pin to operate.
 * @param   mode    Pin mode.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_get_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *mode);
/**
 * @brief   Set the parameters associated with a GPIO pin.
 * @param   ref     GPIO controller ref to operate.
 * @param   pin     Pin to operate.
 * @param   mode    Pin mode.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_set_mode(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t mode);
/**
 * @brief   Get the IRQ trigger type of a GPIO pin.
 * @param   ref     GPIO controller ref to operate.
 * @param   pin     Pin to operate.
 * @param   trig    Trigger type.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_get_irq_trigger(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t *trig);
/**
 * @brief   Set the IRQ trigger type of a GPIO pin.
 * @param   ref     GPIO controller ref to operate.
 * @param   pin     Pin to operate.
 * @param   trig    Trigger type.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_set_irq_trigger(aos_gpioc_ref_t *ref, uint32_t pin, uint32_t trig);
/**
 * @brief   Get the input or output level of a GPIO pin.
 * @param   ref GPIO controller ref to operate.
 * @param   pin Pin to operate.
 * @return  0: low level; > 0: high level; < 0: on failure.
 */
aos_status_t aos_gpioc_get_value(aos_gpioc_ref_t *ref, uint32_t pin);
/**
 * @brief   Set the output level of a GPIO pin.
 * @param   ref GPIO controller ref to operate.
 * @param   pin Pin to operate.
 * @param   val Output level.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_set_value(aos_gpioc_ref_t *ref, uint32_t pin, int val);
/**
 * @brief   Toggle the output level of a GPIO pin.
 * @param   ref GPIO controller ref to operate.
 * @param   pin Pin to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpioc_toggle(aos_gpioc_ref_t *ref, uint32_t pin);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* AOS_GPIOC_H */
