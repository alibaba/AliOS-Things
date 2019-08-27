#ifndef _LEGA_GPIO_H_
#define _LEGA_GPIO_H_
#include <stdint.h>

#define GPIO0_INDEX 0
#define GPIO1_INDEX 1
#define GPIO2_INDEX 2
#define GPIO3_INDEX 3
#define GPIO4_INDEX 4
#define GPIO5_INDEX 5
#define GPIO6_INDEX 6
#define GPIO7_INDEX 7
#define GPIO8_INDEX 8
#define GPIO9_INDEX 9
#define GPIO10_INDEX 10
#define GPIO11_INDEX 11
#define GPIO12_INDEX 12
#define GPIO13_INDEX 13
#define GPIO14_INDEX 14
#define GPIO15_INDEX 15
#define GPIO16_INDEX 16
#define GPIO17_INDEX 17
#define GPIO18_INDEX 18
#define GPIO19_INDEX 19
#define GPIO20_INDEX 20
#define GPIO21_INDEX 21
#define GPIO22_INDEX 22
#define GPIO23_INDEX 23
#define GPIO24_INDEX 24
#define GPIO25_INDEX 25
#define GPIO26_INDEX 26
#define GPIO27_INDEX 27
#define GPIO28_INDEX 28
#define GPIO29_INDEX 29
#define GPIO30_INDEX 30
#define GPIO31_INDEX 31
#define LEGA_GPIO_NUM_PER_GROUP 16
#if (defined LEGA_A0V1)
#define LEGA_GPIO_TOTAL_NUM     32

#elif (defined LEGA_A0V2)
#define LEGA_GPIO_TOTAL_NUM     24
#endif

/*
 * Pin configuration
 */
typedef enum {
    LEGA_ANALOG_MODE,               /* Used as a function pin, input and output analog */
    LEGA_IRQ_MODE,                  /* Used to trigger interrupt */
    LEGA_INPUT_PULL_UP,             /* Input, with an internal pull-up resistor */
    LEGA_INPUT_PULL_DOWN,           /* Input, with an internal pull-down resistor */
    LEGA_INPUT_HIGH_IMPEDANCE,      /* Input, must always be driven, either actively or by an external pullup resistor */
    LEGA_OUTPUT_PUSH_PULL,          /* Output, actively driven high and actively driven low */
    LEGA_OUTPUT_OPEN_DRAIN_NO_PULL, /* Output, actively driven low. When set high, is high-impedance */
    LEGA_OUTPUT_OPEN_DRAIN_PULL_UP, /* Output, actively driven low. When set high, is pulled high with an internal resistor */
} lega_gpio_config_t;

/*
 * GPIO dev struct
 */
typedef struct {
    uint8_t       port;    /* gpio port */
    lega_gpio_config_t config;  /* gpio config */
    void         *priv;    /* priv data */
} lega_gpio_dev_t;

/*
 * GPIO interrupt trigger
 */
typedef enum {
    LEGA_IRQ_TRIGGER_RISING_EDGE  = 0x1, /* Interrupt triggered at input signal's rising edge  */
    LEGA_IRQ_TRIGGER_FALLING_EDGE = 0x2, /* Interrupt triggered at input signal's falling edge */
    LEGA_IRQ_TRIGGER_BOTH_EDGES   = LEGA_IRQ_TRIGGER_RISING_EDGE | LEGA_IRQ_TRIGGER_FALLING_EDGE,
} lega_gpio_irq_trigger_t;

/*
 * GPIO interrupt callback handler
 */
typedef void (*lega_gpio_irq_handler_t)(void *arg);

typedef struct {
    lega_gpio_irq_handler_t cb;
    void *arg;
} lega_gpio_cb_t;

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
int32_t lega_gpio_init(lega_gpio_dev_t *gpio);

/**
 * Sets an output GPIO pin high
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set high
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_output_high(lega_gpio_dev_t *gpio);

/**
 * Sets an output GPIO pin low
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_output_low(lega_gpio_dev_t *gpio);

/**
 * Trigger an output GPIO pin's output. Using this function on a
 * gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_output_toggle(lega_gpio_dev_t *gpio);

/**
 * Get the state of an input GPIO pin. Using this function on a
 * gpio pin which is set to output mode will return an undefined value.
 *
 * @param[in]  gpio   the gpio pin which should be read
 * @param[in]  value  gpio value
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_input_get(lega_gpio_dev_t *gpio, uint32_t *value);

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
int32_t lega_gpio_enable_irq(lega_gpio_dev_t *gpio, lega_gpio_irq_trigger_t trigger,
                            lega_gpio_irq_handler_t handler, void *arg);

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref lega_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_disable_irq(lega_gpio_dev_t *gpio);

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref lega_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_clear_irq(lega_gpio_dev_t *gpio);

/**
 * Set a GPIO pin in default state.
 *
 * @param[in]  gpio  the gpio pin which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_gpio_finalize(lega_gpio_dev_t *gpio);

#endif //_LEGA_GPIO_H_
