/**
 * @file gpio.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_gpio GPIO
 *  gpio hal API.
 *
 *  @{
 */

#include <stdint.h>

/*
 * Pin configuration
 */
typedef enum {
    ANALOG_MODE,               /**< Used as a function pin, input and output analog */
    IRQ_MODE,                  /**< Used to trigger interrupt */
    INPUT_PULL_UP,             /**< Input with an internal pull-up resistor - use with devices
                                  that actively drive the signal low - e.g. button connected to ground */
    INPUT_PULL_DOWN,           /**< Input with an internal pull-down resistor - use with devices
                                  that actively drive the signal high - e.g. button connected to a power rail */
    INPUT_HIGH_IMPEDANCE,      /**< Input - must always be driven, either actively or by an external pullup resistor */
    OUTPUT_PUSH_PULL,          /**< Output actively driven high and actively driven low -
                                  must not be connected to other active outputs - e.g. LED output */
    OUTPUT_OPEN_DRAIN_NO_PULL, /**< Output actively driven low but is high-impedance when set high -
                                  can be connected to other open-drain/open-collector outputs.
                                  Needs an external pull-up resistor */
    OUTPUT_OPEN_DRAIN_PULL_UP, /**< Output actively driven low and is pulled high
                                  with an internal resistor when set high -
                                  can be connected to other open-drain/open-collector outputs. */
    OUTPUT_OPEN_DRAIN_AF,      /**< Alternate Function Open Drain Mode. */
    OUTPUT_PUSH_PULL_AF,       /**< Alternate Function Push Pull Mode. */
} gpio_config_t;

/*
 * GPIO dev struct
 */
typedef struct {
    uint8_t        port;   /**< gpio port */
    gpio_config_t  config; /**< gpio config */
    void          *priv;   /**< priv data */
} gpio_dev_t;

/*
 * GPIO interrupt trigger
 */
typedef enum {
    IRQ_TRIGGER_RISING_EDGE  = 0x1, /**< Interrupt triggered at input signal's rising edge  */
    IRQ_TRIGGER_FALLING_EDGE = 0x2, /**< Interrupt triggered at input signal's falling edge */
    IRQ_TRIGGER_BOTH_EDGES   = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE,
} gpio_irq_trigger_t;


typedef enum {
    GPIO_INPUT     = 0x0000U, /**< Input Floating Mode */
    GPIO_OUTPUT_PP = 0x0001U, /**< Output Push Pull Mode */
    GPIO_OUTPUT_OD = 0x0011U, /**< Output Open Drain Mode */
} hal_gpio_mode_t;

typedef enum {
    GPIO_PinState_Reset = 0,                    /**< Pin state 0 */
    GPIO_PinState_Set   = !GPIO_PinState_Reset, /**< Pin state 1 */
} gpio_pinstate_t;

/*
 * GPIO interrupt callback handler
 */
typedef void (*gpio_irq_handler_t)(void *arg);

/**
 * Initialises a GPIO pin
 *
 * @note  Prepares a GPIO pin for use.
 *
 * @param[in]  gpio           the gpio pin which should be initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_init(gpio_dev_t *gpio);

/**
 * Sets an output GPIO pin high
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set high
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_output_high(gpio_dev_t *gpio);

/**
 * Sets an output GPIO pin low
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_output_low(gpio_dev_t *gpio);

/**
 * Trigger an output GPIO pin's output. Using this function on a
 * gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be toggled
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_output_toggle(gpio_dev_t *gpio);

/**
 * Get the state of an input GPIO pin. Using this function on a
 * gpio pin which is set to output mode will return an undefined value.
 *
 * @param[in]  gpio   the gpio pin which should be read
 * @param[out] value  gpio value
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value);

/**
 * Enables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which is set to
 * output mode is undefined.
 *
 * @param[in]  gpio     the gpio pin which will provide the interrupt trigger
 * @param[in]  trigger  the type of trigger (rising/falling edge or both)
 * @param[in]  handler  a function pointer to the interrupt handler
 * @param[in]  arg      an argument that will be passed to the interrupt handler
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg);

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been setted up using
 * @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_disable_irq(gpio_dev_t *gpio);

/**
 * Clear an interrupt status for an input GPIO pin.
 * Using this function on a gpio pin which has generated a interrupt.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio);

/**
 * Set a GPIO pin in default state.
 *
 * @param[in]  gpio  the gpio pin which should be deinitialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_gpio_finalize(gpio_dev_t *gpio);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_H */

