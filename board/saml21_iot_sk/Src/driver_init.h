/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_ext_irq.h>

#include <hal_flash.h>

#include <hal_i2c_m_sync.h>

#include <hal_usart_sync.h>
#include <hal_usart_async.h>
#include <hal_pwm.h>
#include <hpl_tc_base.h>

#include <hal_pwm.h>
#include <hpl_tcc.h>

#include <hal_pwm.h>
#include <hpl_tcc.h>

extern struct flash_descriptor FLASH_0;

extern struct i2c_m_sync_desc I2C_0;

extern struct usart_sync_descriptor  TARGET_IO;
extern struct usart_async_descriptor USART_AT;

extern struct pwm_descriptor PWM_B;

extern struct pwm_descriptor PWM_R;

extern struct pwm_descriptor PWM_G;

void FLASH_0_init(void);
void FLASH_0_CLOCK_init(void);

void I2C_0_CLOCK_init(void);
void I2C_0_init(void);
void I2C_0_PORT_init(void);

void TARGET_IO_PORT_init(void);
void TARGET_IO_CLOCK_init(void);
void TARGET_IO_init(void);

void USART_AT_PORT_init(void);
void USART_AT_CLOCK_init(void);
void USART_AT_init(void);

void PWM_B_PORT_init(void);
void PWM_B_CLOCK_init(void);
void PWM_B_init(void);

void PWM_R_PORT_init(void);
void PWM_R_CLOCK_init(void);
void PWM_R_init(void);

void PWM_G_PORT_init(void);
void PWM_G_CLOCK_init(void);
void PWM_G_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
