/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/hal/gpio.h"

#define PIN_STAT_HIGH		1
#define PIN_STAT_LOW		0

#ifdef SOUNDAPP_ON_XR871
#include "driver/chip/hal_gpio.h"
#define BAR_DAT_PIN			GPIO_PIN_21	//configure gpio: output, nopull
#define BAR_CLK_PIN			GPIO_PIN_22	//configure gpio: output, nopull

#else
#define BAR_DAT_PIN			-1
#define BAR_CLK_PIN			-1
#endif

#define BAR_LED_COUNT		10

static gpio_dev_t led_bar_dat_pin;
static gpio_dev_t led_bar_clk_pin;
static uint8_t led_bar_initialized;
static uint8_t led_bar_brightness = 55;

static unsigned char led_bar_state[BAR_LED_COUNT];

static int led_bar_data_write(unsigned int data)
{
	unsigned int clk_pin_state = 0;
	unsigned int dat_pin_state = 0;
	int i;

	for (i = 0; i < 16; i++) {
		dat_pin_state = (data & 0x8000) ? PIN_STAT_HIGH : PIN_STAT_LOW;
		if (dat_pin_state == PIN_STAT_HIGH)
			hal_gpio_output_high(&led_bar_dat_pin);
		else
			hal_gpio_output_low(&led_bar_dat_pin);

		clk_pin_state = 1 - clk_pin_state;
		if (clk_pin_state == PIN_STAT_HIGH)
			hal_gpio_output_high(&led_bar_clk_pin);
		else
			hal_gpio_output_low(&led_bar_clk_pin);
		data <<= 1;
	}
}

static int led_bar_data_latch(void)
{
	int i;
	hal_gpio_output_low(&led_bar_dat_pin);
	aos_msleep(10);
	for (i = 0; i < 4; i++) {
		hal_gpio_output_high(&led_bar_dat_pin);
		hal_gpio_output_low(&led_bar_dat_pin);
	}
	return 0;
}

static int led_bar_data_set(unsigned char *state)
{
	int i;
	led_bar_data_write(0);
	for (i = 0; i < BAR_LED_COUNT; i++) {
		led_bar_data_write(state[10-i-1]); //green to red
		//led_bar_data_write(state[i]); //red to green
	}
	/* Two extra empty bits for padding the command to the correct length */
	led_bar_data_write(0x00);
	led_bar_data_write(0x00);
	led_bar_data_latch();
	return 0;
}

int led_bar_init(void)
{
	if (BAR_CLK_PIN < 0 || BAR_DAT_PIN < 0)
		return -1;

	led_bar_clk_pin.port = BAR_CLK_PIN;
	led_bar_clk_pin.config = OUTPUT_OPEN_DRAIN_NO_PULL;

	led_bar_dat_pin.port = BAR_DAT_PIN;
	led_bar_dat_pin.config = OUTPUT_OPEN_DRAIN_NO_PULL;

	hal_gpio_init(&led_bar_clk_pin);
	hal_gpio_init(&led_bar_dat_pin);

	led_bar_initialized = 1;
	//led_bar_ctrl(0);
	led_blue_ctrl(0);
	led_green_ctrl(0);
	printf("%s: led bar init\n", __func__);
	return 0;
}


int led_bar_brightness_set(unsigned char level)
{
	led_bar_brightness = level;
	return 0;
}

/*
 * parameter bits:
 * eg.
 * 0x5 -- Turn on LEDs 1 and 3
 * 0x155 -- Turn on LEDs 1, 3, 5, 7, 9
 */
int led_bar_ctrl(unsigned int bits)
{
	int i;

	if (!led_bar_initialized)
		return -1;

	for (i = 0; i < BAR_LED_COUNT; i++) {
		if ((bits >> i) & 0x01) {
			led_bar_state[i] = led_bar_brightness; /* increase the value to get higher brightness */
			if (i == 8)
				led_bar_state[i] = led_bar_brightness;//0xa;
			else if (i == 9)
				led_bar_state[i] = 0x5;
		} else {
			led_bar_state[i] = 0x00;
		}
	}

	led_bar_data_set(led_bar_state);
	return 0;
}
