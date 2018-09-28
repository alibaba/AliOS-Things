/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stm32f4xx_hal.h"


#define BAR_LED_COUNT		10

#define PIN_STAT_HIGH		1
#define PIN_STAT_LOW		0
#define BAR_CLK_PIN			GPIO_PIN_13	//configure gpio: output, nopull
#define BAR_CLK_PIN_Port	GPIOC
#define BAR_DAT_PIN			GPIO_PIN_4	//configure gpio: output, nopull
#define BAR_DAT_PIN_Port	GPIOA

static unsigned char bar_state[BAR_LED_COUNT];
static int led_bar_data_write(unsigned int data)
{
	unsigned int clk_pin_state = 0;
	unsigned int dat_pin_state = 0;
	int i;
	for (i = 0; i < 16; i++) {
		dat_pin_state = (data & 0x8000) ? PIN_STAT_HIGH : PIN_STAT_LOW;
		HAL_GPIO_WritePin(BAR_DAT_PIN_Port, BAR_DAT_PIN, dat_pin_state);
		clk_pin_state = 1 - clk_pin_state;
		HAL_GPIO_WritePin(BAR_CLK_PIN_Port, BAR_CLK_PIN, clk_pin_state);
		data <<= 1;
	}
}

static int led_bar_data_latch(void)
{
	int i;
	HAL_GPIO_WritePin(BAR_DAT_PIN_Port, BAR_DAT_PIN, PIN_STAT_LOW);
	aos_msleep(10);
	for (i = 0; i < 4; i++) {
		HAL_GPIO_WritePin(BAR_DAT_PIN_Port, BAR_DAT_PIN, PIN_STAT_HIGH);
		HAL_GPIO_WritePin(BAR_DAT_PIN_Port, BAR_DAT_PIN, PIN_STAT_LOW);
	}
	return 0;
}

static int led_bar_data_set(unsigned char *state)
{
	int i;
	led_bar_data_write(0);

	for (i = 0; i < BAR_LED_COUNT; i++)
		led_bar_data_write(state[10-i-1]);

	led_bar_data_write(0x00);
	led_bar_data_write(0x00);
	led_bar_data_latch();
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
	for (i = 0; i < BAR_LED_COUNT; i++) {
		if ((bits % 2) == 1)
			bar_state[i] = 0x20; /* increase the value to get higher brightness */
		else
			bar_state[i] = 0x00;
		bits /= 2;
	}
	led_bar_data_set(bar_state);
	return 0;
}

int drv_led_bar_grove_my9221_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/*Configure GPIO pins : LED Bar Clock */
	GPIO_InitStruct.Pin = BAR_CLK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BAR_CLK_PIN_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LED Bar Data */
	GPIO_InitStruct.Pin = BAR_DAT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BAR_DAT_PIN_Port, &GPIO_InitStruct);

	led_bar_ctrl(0);
	printf("%s: led bar init success\n", __func__);
	return 0;
}