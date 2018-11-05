/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAME54 has 14 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7
#define GPIO_PIN_FUNCTION_I 8
#define GPIO_PIN_FUNCTION_J 9
#define GPIO_PIN_FUNCTION_K 10
#define GPIO_PIN_FUNCTION_L 11
#define GPIO_PIN_FUNCTION_M 12
#define GPIO_PIN_FUNCTION_N 13

#define CARD_DETECT_0 GPIO(GPIO_PORTA, 0)
#define CARD_WRITE_PROTECT_0 GPIO(GPIO_PORTA, 1)
#define CARD_RESET_PIN GPIO(GPIO_PORTA, 6)
#define CARD_CONF_WINC_EXT_INT_PIN GPIO(GPIO_PORTC, 23) 
#define CARD_CE_PIN GPIO(GPIO_PORTA, 27)
#define CARD_CS_PIN GPIO(GPIO_PORTB, 28)
#define PA04 GPIO(GPIO_PORTA, 4)
#define PA05 GPIO(GPIO_PORTA, 5)
#define PA20 GPIO(GPIO_PORTA, 20)
#define PA21 GPIO(GPIO_PORTA, 21)
#define PB18 GPIO(GPIO_PORTB, 18)
#define PB19 GPIO(GPIO_PORTB, 19)
#define PB20 GPIO(GPIO_PORTB, 20)
#define PB21 GPIO(GPIO_PORTB, 21)
#define EDBG_COM_RX GPIO(GPIO_PORTB, 24)
#define EDBG_COM_TX GPIO(GPIO_PORTB, 25)
#define PB31 GPIO(GPIO_PORTB, 31)
#define LED0 GPIO(GPIO_PORTC, 18)
#endif // ATMEL_START_PINS_H_INCLUDED
