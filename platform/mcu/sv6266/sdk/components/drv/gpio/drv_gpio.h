#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include "hal_gpio.h"

/**
 * @file drv_gpio.h
 *
 *  Configure gpio settings.
 *
 */
 

/**
  * @brief setup gpio mode.
  *
  * setup gpio mode.
  *
  * @param pin pin number.
  * @param mode set the pin mode.
  *
  */
int8_t drv_gpio_set_mode(gpio_pin_t pin, pin_mode_t mode);


/**
 * @brief   Set GPIO direction.
 *
 * @param   pin                             GPIO pin number.
 * @param   dir                             The GPIO direction(in or out).
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_gpio_set_dir(gpio_pin_t pin, gpio_dir_t dir);


/**
 * @brief   Get GPIO direction.
 *
 * @param   pin                             GPIO pin number.
 *
 * @return  dir                              The GPIO direction(in or out).
 */
gpio_dir_t drv_gpio_get_dir(gpio_pin_t pin);


/**
 * @brief   Setup GPIO input pull down or pull up or none.
 *
 * @param   pin                             GPIO pin number.
 * @param   pull                            GPIO pull up or down.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_gpio_set_pull(gpio_pin_t pin, gpio_pull_t pull);


/**
 * @brief   Get GPIO input pull down or pull up or none.
 *
 * @param   pin                             GPIO pin number.
 *
 * @return  pull                            GPIO pull up or down.
 */
gpio_pull_t drv_gpio_get_pull(gpio_pin_t pin);


/**
 * @brief   Setup GPIO output logic.
 *
 * @param   pin                             GPIO pin number.
 * @param   logic                           GPIO logic high or low.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_gpio_set_logic(gpio_pin_t pin, gpio_logic_t logic);


/**
 * @brief   Setup GPIO pull driving strengh.
 *
 * @param   pin                             GPIO pin number.
 * @param   driving_streght                 GPIO driving strength.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_gpio_set_driving_strengh(gpio_pin_t pin, gpio_driving_strengh_t driving_streght);



/**
 * @brief   Get GPIO logic status.
 *
 * @param   pin                             GPIO pin number.
 *
 * @return   0                              GPIO logic state LOW.
 *           1                              GPIO logic state HIGH.              
 */
gpio_logic_t drv_gpio_get_logic(gpio_pin_t pin);


/**
 * @brief   Setting gpio interrupt trigger mode
 *
 * @param   pin                             GPIO pin number.
 * @param   mode                            gpio input interrupt trigger mode.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.         
 */
int8_t drv_gpio_intc_trigger_mode(gpio_pin_t pin, gpio_intc_trigger_mode_t mode);


/**
 * @brief   Setting an interrupt handler that interrupt occurs.
 *
 * @param   pin                             GPIO pin number.
 * @param   handler                         A pointer to an interrupt handler that interrupt occurs.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.         
 */
int8_t drv_gpio_register_isr(gpio_pin_t pin, isr_func handler);


/**
 * @brief   clear the interrupt trigger.
 *
 * @param   pin                             GPIO pin number.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.         
 */
int8_t drv_gpio_intc_clear(gpio_pin_t pin);

#endif  // __DRV_GPIO_H__
