/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include "driver_init.h"


/*! The buffer size for USART */
#define USART_AT_BUFFER_SIZE 2048

struct usart_async_descriptor USART_AT;

static uint8_t USART_AT_buffer[USART_AT_BUFFER_SIZE];

struct flash_descriptor FLASH_0;

struct i2c_m_sync_desc I2C_0;

struct usart_sync_descriptor TARGET_IO;

struct pwm_descriptor PWM_B;

struct pwm_descriptor PWM_R;

struct pwm_descriptor PWM_G;

void EXTERNAL_IRQ_BTN_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, CONF_GCLK_EIC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_mclk_set_APBAMASK_EIC_bit(MCLK);

	// Set pin direction to input
	gpio_set_pin_direction(USR, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(USR,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_UP);

	gpio_set_pin_function(USR, PINMUX_PA02A_EIC_EXTINT2);

	ext_irq_init();
}

void FLASH_0_CLOCK_init(void)
{

    hri_mclk_set_AHBMASK_NVMCTRL_bit(MCLK);
}

void FLASH_0_init(void)
{
    FLASH_0_CLOCK_init();
    flash_init(&FLASH_0, NVMCTRL);
}

void I2C_0_PORT_init(void)
{

	gpio_set_pin_pull_mode(PA08,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA08, PINMUX_PA08C_SERCOM0_PAD0);

	gpio_set_pin_pull_mode(PA09,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA09, PINMUX_PA09C_SERCOM0_PAD1);
}

void I2C_0_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_SLOW, CONF_GCLK_SERCOM0_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM0_bit(MCLK);
}

void I2C_0_init(void)
{
	I2C_0_CLOCK_init();
	i2c_m_sync_init(&I2C_0, SERCOM0);
	I2C_0_PORT_init();
}

void TARGET_IO_PORT_init(void)
{

	gpio_set_pin_function(PA22, PINMUX_PA22C_SERCOM3_PAD0);

	gpio_set_pin_function(PA23, PINMUX_PA23C_SERCOM3_PAD1);
}

void TARGET_IO_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_SLOW, CONF_GCLK_SERCOM3_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM3_bit(MCLK);
}

void TARGET_IO_init(void)
{
	TARGET_IO_CLOCK_init();
	usart_sync_init(&TARGET_IO, SERCOM3, (void *)NULL);
	TARGET_IO_PORT_init();
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void USART_AT_CLOCK_init()
{

	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_SLOW, CONF_GCLK_SERCOM4_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM4_bit(MCLK);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void USART_AT_PORT_init()
{

	gpio_set_pin_function(PB08, PINMUX_PB08D_SERCOM4_PAD0);

	gpio_set_pin_function(PB09, PINMUX_PB09D_SERCOM4_PAD1);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void USART_AT_init(void)
{
	USART_AT_CLOCK_init();
	usart_async_init(&USART_AT, SERCOM4, USART_AT_buffer, USART_AT_BUFFER_SIZE, (void *)NULL);
	USART_AT_PORT_init();
}

void PWM_B_PORT_init(void)
{

	gpio_set_pin_function(PB13, PINMUX_PB13E_TC0_WO1);
}

void PWM_B_CLOCK_init(void)
{
	hri_mclk_set_APBCMASK_TC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC0_GCLK_ID, CONF_GCLK_TC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void PWM_B_init(void)
{
	PWM_B_CLOCK_init();
	PWM_B_PORT_init();
	pwm_init(&PWM_B, TC0, _tc_get_pwm());
}

void PWM_R_PORT_init(void)
{

	gpio_set_pin_function(PB10, PINMUX_PB10F_TCC0_WO4);
}

void PWM_R_CLOCK_init(void)
{
	hri_mclk_set_APBCMASK_TCC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC0_GCLK_ID, CONF_GCLK_TCC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void PWM_R_init(void)
{
	PWM_R_CLOCK_init();
	PWM_R_PORT_init();
	pwm_init(&PWM_R, TCC0, _tcc_get_pwm());
}

void PWM_G_PORT_init(void)
{

	gpio_set_pin_function(PA12, PINMUX_PA12E_TCC2_WO0);
}

void PWM_G_CLOCK_init(void)
{
	hri_mclk_set_APBCMASK_TCC2_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC2_GCLK_ID, CONF_GCLK_TCC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void PWM_G_init(void)
{
	PWM_G_CLOCK_init();
	PWM_G_PORT_init();
	pwm_init(&PWM_G, TCC2, _tcc_get_pwm());
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA17

	// Set pin direction to output
	gpio_set_pin_direction(LCD_RST, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LCD_RST,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LCD_RST, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA20

	// Set pin direction to input
	gpio_set_pin_direction(SW_1, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(SW_1,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SW_1, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA21

	// Set pin direction to input
	gpio_set_pin_direction(SW_2, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(SW_2,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SW_2, GPIO_PIN_FUNCTION_OFF);

	EXTERNAL_IRQ_BTN_init();

    FLASH_0_init();

	I2C_0_init();

	TARGET_IO_init();
	USART_AT_init();

	PWM_B_init();

	PWM_R_init();

	PWM_G_init();
}
