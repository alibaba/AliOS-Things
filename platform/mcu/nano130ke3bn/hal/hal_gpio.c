/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>
#include "aos/hal/gpio.h"
#include "board.h"

#define NU_MAX_PIN_PER_PORT     16

#ifndef OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_ENABLE
#define OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_ENABLE 0
#endif

#ifndef OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_ENABLE_LIST
#define OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_ENABLE_LIST NC
#endif

#ifndef OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_CLOCK_SOURCE
#define OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_CLOCK_SOURCE GPIO_DBCTL_DBCLKSRC_LIRC
#endif

#ifndef OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_SAMPLE_RATE
#define OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_SAMPLE_RATE GPIO_DBCTL_DBCLKSEL_16
#endif

struct nu_gpio_irq_var {
    struct gpio_s * 	obj_arr[NU_MAX_PIN_PER_PORT];
    IRQn_Type       	irq_n;
    uint32_t        	port_index;
};


static void numicro_gpio_irq_enable(struct gpio_s *obj);
static void numicro_gpio_irq(struct nu_gpio_irq_var *var);

//EINT0_IRQn
static struct nu_gpio_irq_var gpio_irq_var_arr[] = {
    {{NULL}, GPA_IRQn, 0},
    {{NULL}, GPB_IRQn, 1},
    {{NULL}, GPC_IRQn, 2},
    {{NULL}, GPD_IRQn, 3},
    {{NULL}, GPE_IRQn, 4},
    {{NULL}, GPF_IRQn, 5},
    {{NULL}, GPG_IRQn, 6},
    {{NULL}, GPH_IRQn, 7},
};
#define NU_MAX_PORT  (sizeof (gpio_irq_var_arr) / sizeof (gpio_irq_var_arr[0]))


void GPA_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[0]); }				 // 16: GPIO Port A
void GPB_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[1]); }        // 17: GPIO Port B
void GPC_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[2]); }        // 18: GPIO Port C
void GPD_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[3]); }        // 19: GPIO Port D
void GPE_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[4]); }        // 20: GPIO Port E
void GPF_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[5]); }        // 21: GPIO Port F
void GPG_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[6]); }        // 72: GPIO Port G
void GPH_IRQHandler(void)	{ numicro_gpio_irq(&gpio_irq_var_arr[7]); }        // 88: GPIO Port H

static PinName gpio_irq_debounce_arr[] = {
    OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_ENABLE_LIST
};

static void numicro_gpio_irq(struct nu_gpio_irq_var *var)
{
    // NOTE: GPA_IRQn, GPB_IRQn, ... are not arranged sequentially, so we cannot calculate out port_index through offset from GPA_IRQn.
    //       Instead, we add port_index into gpio_irq_var_arr table.
    uint32_t port_index = var->port_index;
    GPIO_T *gpio_base = NU_PORT_BASE(port_index);

    uint32_t intsrc = gpio_base->INTSRC;
    uint32_t inten = gpio_base->INTEN;
    while (intsrc) {
        int pin_index = nu_ctz(intsrc);
        struct gpio_s *obj = var->obj_arr[pin_index];
        if (inten & (GPIO_INT_RISING << pin_index)) {
            if (GPIO_PIN_DATA(port_index, pin_index)) {
                if (obj->irq_handler) {
                    ((gpio_irq_handler_t) obj->irq_handler)(obj->arg);
                }
            }
        }

        if (inten & (GPIO_INT_FALLING << pin_index)) {
            if (! GPIO_PIN_DATA(port_index, pin_index)) {
                if (obj->irq_handler) {
                    ((gpio_irq_handler_t) obj->irq_handler)(obj->arg);
                }
            }
        }

        intsrc &= ~(1 << pin_index);
    }
    // Clear all interrupt flags
    gpio_base->INTSRC = gpio_base->INTSRC;
}




static int numicro_gpio_irq_init(struct gpio_s *obj, PinName pin, gpio_irq_handler_t handler, void* arg)
{
    if (pin == NC) {
        return -1;
    }

    uint32_t pin_index = NU_PINNAME_TO_PIN(pin);
    uint32_t port_index = NU_PINNAME_TO_PORT(pin);
    if (pin_index >= NU_MAX_PIN_PER_PORT || port_index >= NU_MAX_PORT) {
        return -1;
    }

    obj->pin = pin;
    obj->irq_handler = (uint32_t) handler;
    obj->arg = arg;

    GPIO_T *gpio_base = NU_PORT_BASE(port_index);
    // NOTE: In InterruptIn constructor, gpio_irq_init() is called with gpio_init_in() which is responsible for multi-function pin setting.
    //       There is no need to call gpio_set() redundantly.

    {
#if OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_ENABLE
        // Suppress compiler warning
        (void) gpio_irq_debounce_arr;

        // Configure de-bounce clock source and sampling cycle time
        GPIO_SET_DEBOUNCE_TIME(OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_CLOCK_SOURCE, OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_SAMPLE_RATE);
        GPIO_ENABLE_DEBOUNCE(gpio_base, 1 << pin_index);
#else
        // Enable de-bounce if the pin is in the de-bounce enable list

        // De-bounce defaults to disabled.
        GPIO_DISABLE_DEBOUNCE(gpio_base, 1 << pin_index);

        PinName *debounce_pos = gpio_irq_debounce_arr;
        PinName *debounce_end = gpio_irq_debounce_arr + sizeof (gpio_irq_debounce_arr) / sizeof (gpio_irq_debounce_arr[0]);
        for (; debounce_pos != debounce_end && *debounce_pos != NC; debounce_pos ++) {
            uint32_t pin_index_debunce = NU_PINNAME_TO_PIN(*debounce_pos);
            uint32_t port_index_debounce = NU_PINNAME_TO_PORT(*debounce_pos);

            if (pin_index == pin_index_debunce &&
                    port_index == port_index_debounce) {
                // Configure de-bounce clock source and sampling cycle time
                GPIO_SET_DEBOUNCE_TIME(OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_CLOCK_SOURCE, OS_CONF_TARGET_GPIO_IRQ_DEBOUNCE_SAMPLE_RATE);
                GPIO_ENABLE_DEBOUNCE(gpio_base, 1 << pin_index);
                break;
            }
        }
#endif
    }

    struct nu_gpio_irq_var *var = gpio_irq_var_arr + port_index;

    var->obj_arr[pin_index] = obj;

    // NOTE: InterruptIn requires IRQ enabled by default.
    numicro_gpio_irq_enable(obj);

    return 0;
}

static void numicro_gpio_irq_free(struct gpio_s *obj)
{
    uint32_t pin_index = NU_PINNAME_TO_PIN(obj->pin);
    uint32_t port_index = NU_PINNAME_TO_PORT(obj->pin);
    struct nu_gpio_irq_var *var = gpio_irq_var_arr + port_index;

    NVIC_DisableIRQ(var->irq_n);
    NU_PORT_BASE(port_index)->INTEN = 0;

    var->obj_arr[pin_index] = NULL;
}

static void numicro_gpio_set(struct gpio_s *obj, gpio_irq_trigger_t event, uint32_t enable)
{
    uint32_t pin_index = NU_PINNAME_TO_PIN(obj->pin);
    uint32_t port_index = NU_PINNAME_TO_PORT(obj->pin);
    GPIO_T *gpio_base = NU_PORT_BASE(port_index);

    switch (event) {
    case IRQ_TRIGGER_FALLING_EDGE:
        if (enable) {
            GPIO_EnableInt(gpio_base, pin_index, GPIO_INT_FALLING);
        } else {
            GPIO_DisableInt(gpio_base, pin_index);
					  //gpio_base->INTEN &= ~(GPIO_INT_RISING << pin_index);
        }
        break;

    case IRQ_TRIGGER_RISING_EDGE:
        if (enable) {
            GPIO_EnableInt(gpio_base, pin_index, GPIO_INT_RISING);
        } else {
            GPIO_DisableInt(gpio_base, pin_index);
            //gpio_base->INTEN &= ~(GPIO_INT_FALLING << pin_index);
        }
        break;

		case IRQ_TRIGGER_BOTH_EDGES:
        if (enable) {
            GPIO_EnableInt(gpio_base, pin_index, GPIO_INT_BOTH_EDGE);
        } else {
            GPIO_DisableInt(gpio_base, pin_index);					
        }
				break;
				
    default:
        break;
    }
}

static void numicro_gpio_irq_enable(struct gpio_s *obj)
{
    uint32_t port_index = NU_PINNAME_TO_PORT(obj->pin);
    struct nu_gpio_irq_var *var = gpio_irq_var_arr + port_index;

//    NVIC_SetVector(var->irq_n, (uint32_t) var->vec);
    NVIC_EnableIRQ(var->irq_n);
}

static void numicro_gpio_irq_disable(struct gpio_s *obj)
{
    uint32_t port_index = NU_PINNAME_TO_PORT(obj->pin);
    struct nu_gpio_irq_var *var = gpio_irq_var_arr + port_index;

    NVIC_DisableIRQ(var->irq_n);
}


/*AOS HAL*/
static struct gpio_s* hal_get_gpio_s ( gpio_dev_t *gpio )
{
	if ( !(gpio) || (gpio->port >= i32BoardMaxGPIONum) )
		goto exit_hal_get_gpio_s;

	// Get UART Private configuration, these setting are defined in board/xxxxx.
	return (struct gpio_s*)&board_gpio[gpio->port];

exit_hal_get_gpio_s:
	return NULL;
}

static uint32_t gpio_set(PinName pin)
{
    if (pin == (PinName) NC)
        return 0;

    uint32_t pin_index = NU_PINNAME_TO_PIN(pin);

    pin_function(pin, 0 << NU_MFP_POS(pin_index));

    return (uint32_t)(1 << pin_index);    // Return the pin mask
}

static int gpio_is_connected(const struct gpio_s *obj)
{
    return (obj->pin != (PinName) NC);
}

static void gpio_init(struct gpio_s *obj, PinName pin)
{
    obj->pin = pin;

    if (obj->pin == (PinName) NC) {
        return;
    }

    obj->mask = gpio_set(pin);
}

static void gpio_mode(struct gpio_s *obj, PinMode mode)
{
    if (obj->pin == (PinName) NC) {
        return;
    }

    pin_mode(obj->pin, mode);
}

static void gpio_pullctrl(struct gpio_s *obj, PinMode mode)
{
    if (obj->pin == (PinName) NC) {
        return;
    }

    pin_mode(obj->pin, mode);
}

static void gpio_dir(struct gpio_s *obj, PinDirection direction)
{
    if (obj->pin == (PinName) NC) {
        return;
    }

    uint32_t pin_index = NU_PINNAME_TO_PIN(obj->pin);
    uint32_t port_index = NU_PINNAME_TO_PORT(obj->pin);
    GPIO_T *gpio_base = NU_PORT_BASE(port_index);

    uint32_t mode_intern = GPIO_MODE_INPUT;

    switch (direction) {
			case PIN_INPUT:
					mode_intern = GPIO_MODE_INPUT;
					break;

			case PIN_OUTPUT:
					mode_intern = GPIO_MODE_OUTPUT;
					break;

			default:
        return;
    }

    GPIO_SetMode(gpio_base, 1 << pin_index, mode_intern);
}


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
int32_t hal_gpio_init(gpio_dev_t *gpio)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		goto exit_hal_gpio_init;
	
	obj->mask = gpio_set(obj->pin);

	switch (gpio->config) {

    case ANALOG_MODE:
        break;
		
    case IRQ_MODE:
				gpio_dir ( obj, PIN_INPUT );
        break;

    case INPUT_PULL_UP:
				gpio_dir ( obj, PIN_INPUT );
				gpio_pullctrl ( obj, PullUp );
        break;

    case INPUT_PULL_DOWN:
				gpio_dir ( obj, PIN_INPUT );
				gpio_pullctrl ( obj, PullDown );
        break;

    case INPUT_HIGH_IMPEDANCE:
				gpio_dir ( obj, PIN_INPUT );
				gpio_pullctrl ( obj, PullNone );
        break;

    case OUTPUT_PUSH_PULL:
//				gpio_dir ( obj, PIN_OUTPUT );
				gpio_mode ( obj, PushPull );
        break;

    case OUTPUT_OPEN_DRAIN_NO_PULL:
//				gpio_dir ( obj, PIN_OUTPUT );		
				gpio_mode ( obj, OpenDrain );
				gpio_pullctrl ( obj, PullNone );		
        break;

    case OUTPUT_OPEN_DRAIN_PULL_UP:
//				gpio_dir ( obj, PIN_OUTPUT );			
				gpio_mode ( obj, OpenDrain );
				gpio_pullctrl ( obj, PullUp );		
        break;

    default:
				goto exit_hal_gpio_init;
				break;
  }	//switch
	
	return HAL_OK;

exit_hal_gpio_init:
	
	return HAL_ERROR;
}

/**
 * Sets an output GPIO pin high
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set high
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		return HAL_ERROR;
	
	gpio_write(obj, 1);	
	
	return HAL_OK;
}

/**
 * Sets an output GPIO pin low
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		return HAL_ERROR;

	gpio_write(obj, 0);	

	return HAL_OK;
}

/**
 * Trigger an output GPIO pin's output. Using this function on a
 * gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		return HAL_ERROR;
	
	(gpio_read(obj)==1)? gpio_write (obj, 0) : gpio_write(obj, 1) ;

	return HAL_OK;
}

/**
 * Get the state of an input GPIO pin. Using this function on a
 * gpio pin which is set to output mode will return an undefined value.
 *
 * @param[in]  gpio   the gpio pin which should be read
 * @param[in]  value  gpio value
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) || !value )
		return HAL_ERROR;

	if (value)
		*value = gpio_read(obj);

	return HAL_OK;
}

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
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		goto exit_hal_gpio_enable_irq;
	
	if ( numicro_gpio_irq_init ( obj, obj->pin, handler, arg) < 0 )
		goto exit_hal_gpio_enable_irq;

	numicro_gpio_set(obj, trigger, 1);
	
	numicro_gpio_irq_enable(obj);

	return HAL_OK;

exit_hal_gpio_enable_irq:

	return HAL_ERROR;
}

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		return HAL_ERROR;

	numicro_gpio_set(obj, IRQ_TRIGGER_BOTH_EDGES, 0);
	
	numicro_gpio_irq_disable(obj);
	
	return HAL_OK;
}

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		return HAL_ERROR;
	
	numicro_gpio_irq_free(obj);

	return HAL_OK;
}

/**
 * Set a GPIO pin in default state.
 *
 * @param[in]  gpio  the gpio pin which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
	struct gpio_s * obj;

	if ( !(obj= hal_get_gpio_s ( gpio )) )
		return HAL_ERROR;

	return HAL_OK;
}
