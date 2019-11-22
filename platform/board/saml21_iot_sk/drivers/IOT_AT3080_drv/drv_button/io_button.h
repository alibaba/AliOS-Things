/**
 ******************************************************************************
 * @file    io_button.h
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   GPIO button driver header file
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
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
 *
 ******************************************************************************
 */

#ifndef _IO_BUTTON_H_
#define _IO_BUTTON_H_

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup drivers */
/** @{*/

/** \addtogroup IO_Button */
/** @{*/

/******************************************************************************
 *                                 Enumerations
 ******************************************************************************/

enum _button_idle_state_e{
    IOBUTTON_IDLE_STATE_LOW = 0,  /**< GPIO level is low when button is unpressed. */
    IOBUTTON_IDLE_STATE_HIGH,     /**< GPIO level is high when button is unpressed. */
};
typedef uint8_t btn_idle_state_t;

/******************************************************************************
 *                               Type Definitions
 ******************************************************************************/



typedef void (*button_pressed_cb)(void) ;
typedef void (*button_long_pressed_cb)(void) ;

typedef struct {
  /* Change if migrate to other MCU platform */
  const uint8_t port;           /**< MCU GPIO port define by ATMEL peripheral library. */
  const uint8_t pin;            /**< MCU GPIO pin define by ATMEL peripheral library. */
  void (*io_irq)(void);         /**< MCU external interrupt ISR . */

  /* IO button defines */
  btn_idle_state_t idle;                      /**< GPIO idle state, #_button_idle_state_e  */
  uint32_t long_pressed_timeout;              /**< Button long pressed timeout  */
  button_pressed_cb pressed_func;             /**< Button pressed handler  */
  button_long_pressed_cb long_pressed_func;   /**< Button long pressed handler  */

  /* Use by driver, do not initialize */
  volatile bool clicked;
  volatile bool timer_enabled;
  volatile uint32_t start_time;
} btn_instance_t;

/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/


/**
 * @brief Initialize button device.
 *
 * @param[in] btn: button driver context data, should be persist at button's life time
 *
 * @return    none
 */
void button_init(btn_instance_t * const btn);

/**
 * @brief Button driver service, should be called in main loop.
 *
 * @param[in] btn: button driver context data, should be persist at button's life time
 *
 * @return none
 */
void button_srv(btn_instance_t * const btn);

/**
 * @brief This function should be called by IRQ routine.
 *
 * @param[in] btn: button driver context data, should be persist at button's life time
 *
 * @return none
 */
void button_irq_handler(btn_instance_t * const btn);

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

#endif  // _IO_BUTTON_H_

