/*
 * Copyright (C) 2017 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "aos/kernel.h"
#include "aos/hal/spi.h"
#include "aos/hal/gpio.h"

#include "ssd1306.h"

static spi_dev_t oled_spi_dev;
static gpio_dev_t gpio_oled_cs;
static gpio_dev_t gpio_oled_ds;
static gpio_dev_t gpio_oled_reset;

int32_t ssd1306_write(uint8_t data, SSD1306_WR_MODE mode)
{
	if(mode == SSD1306_CMD) {
		hal_gpio_output_low(&gpio_oled_ds);
	} else if(mode == SSD1306_DATA) {
		hal_gpio_output_high(&gpio_oled_ds);
	}
	hal_spi_send(&oled_spi_dev, &data, 1, 0xffffffff);
	return 0;
}

void ssd1306_init(void)
{
	oled_spi_dev.port = 1;
	oled_spi_dev.config.mode = HAL_SPI_MODE_MASTER;
	oled_spi_dev.config.freq = 6000000;
	hal_spi_init(&oled_spi_dev);

	//config reset and ds pin
	gpio_oled_reset.port = 8; //PA8
	gpio_oled_reset.config = OUTPUT_PUSH_PULL;
	hal_gpio_init(&gpio_oled_reset);
	hal_gpio_output_high(&gpio_oled_reset);

	gpio_oled_cs.port = 22; //PA22
	gpio_oled_cs.config = OUTPUT_PUSH_PULL;
	hal_gpio_init(&gpio_oled_cs);
	hal_gpio_output_high(&gpio_oled_cs);
	aos_msleep(1);
	hal_gpio_output_low(&gpio_oled_cs);

	gpio_oled_ds.port = 20; //PA20
	gpio_oled_ds.config = OUTPUT_PUSH_PULL;
	hal_gpio_init(&gpio_oled_ds);
	hal_gpio_output_high(&gpio_oled_ds);

	aos_msleep(1);
	hal_gpio_output_low(&gpio_oled_reset);
	aos_msleep(1);
	hal_gpio_output_high(&gpio_oled_reset);

	//config oled
	ssd1306_write(SSD1306_DISPLAYOFF, SSD1306_CMD);
	ssd1306_write(SSD1306_SETDISPLAYCLOCKDIV, SSD1306_CMD);
	ssd1306_write(80, SSD1306_CMD);
	ssd1306_write(SSD1306_SETMULTIPLEX, SSD1306_CMD);
	ssd1306_write(0x3F, SSD1306_CMD);
	ssd1306_write(SSD1306_SETDISPLAYOFFSET, SSD1306_CMD);
	ssd1306_write(0x00, SSD1306_CMD);
	ssd1306_write(SSD1306_SETSTARTLINE, SSD1306_CMD);
	ssd1306_write(SSD1306_ENABLE_CHARGE_PUMP, SSD1306_CMD);
	ssd1306_write(0x14, SSD1306_CMD);
	ssd1306_write(SSD1306_MEMORYMODE, SSD1306_CMD);
	ssd1306_write(0x02, SSD1306_CMD);
	ssd1306_write(0xA1, SSD1306_CMD);
	ssd1306_write(SSD1306_COMSCANINC, SSD1306_CMD);
	ssd1306_write(SSD1306_SETCOMPINS, SSD1306_CMD);
	ssd1306_write(0X12, SSD1306_CMD);
	ssd1306_write(SSD1306_SETCONTRAST, SSD1306_CMD);
	ssd1306_write(0x1F, SSD1306_CMD); //set brightness
	ssd1306_write(SSD1306_SETPRECHARGE, SSD1306_CMD);
	ssd1306_write(0xF1, SSD1306_CMD);
	ssd1306_write(SSD1306_SETVCOMDETECT, SSD1306_CMD);
	ssd1306_write(0x30, SSD1306_CMD);
	ssd1306_write(SSD1306_DISPLAYALLON_RESUME, SSD1306_CMD);
	ssd1306_write(SSD1306_NORMALDISPLAY, SSD1306_CMD);
	ssd1306_write(SSD1306_DISPLAYON, SSD1306_CMD);

	return 0;
}

void ssd1306_deinit(void)
{
	hal_spi_finalize(&oled_spi_dev);
}

void ssd1306_set_brightness(uint8_t brightness)
{
	ssd1306_write(SSD1306_SETCONTRAST, SSD1306_CMD);
	ssd1306_write(brightness, SSD1306_CMD); //set brightness
}

void ssd1306_reset(void)
{
	hal_gpio_output_low(&gpio_oled_reset);
	aos_msleep(1);
	hal_gpio_output_high(&gpio_oled_reset);
}
