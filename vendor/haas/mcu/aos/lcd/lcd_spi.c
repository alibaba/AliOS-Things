/*
 * drivers/lcd/lcd_spi.c
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <aos/hal/spi.h>
#include <stdio.h>
#include "hal_iomux_haas1000.h"
#include "hal_gpio.h"
#include "test.h"
#include "lcd_spi.h"
#include "cmsis_os.h"
#include "cmsis.h"

extern const uint8_t gImage_test[153600];

#define COL 240
#define ROW 320

struct SpiDevice *lcd_spi = NULL;
static uint8_t *fbbuf_write = NULL;

#define GPIO_DC HAL_IOMUX_PIN_P0_0
#define GPIO_RESET HAL_IOMUX_PIN_P2_2
#define GPIO_BL HAL_IOMUX_PIN_LED1



static void reset_init(void)
{
	hal_gpio_pin_set_dir(GPIO_RESET, HAL_GPIO_DIR_OUT, 1);
}

static void dc_init(void)
{
	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 1);
}

static void bl_en_init(void)
{
	hal_gpio_pin_set_dir(GPIO_BL, HAL_GPIO_DIR_OUT, 1);
}

static uint32_t spi_init(void)
{
	spi_dev_t lcd_spi_cfg = {
		.port = 0,
		.config.mode = 0,
		.config.freq = 26000000,
	};

	return hal_spi_init(&lcd_spi_cfg);

}

void lcd_spi_write(struct SpiDevice *spi, uint8_t *tbuf, uint32_t tlen)
{
	hal_spi_send(NULL, tbuf, tlen, 0);
}

static void WriteComm(struct SpiDevice *spi, u8 command)
{
	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 0);
	aos_msleep(1);
	lcd_spi_write(spi, &command, 1);
}

static void WriteData(struct SpiDevice *spi, uint8_t data)
{
	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 1);
	aos_msleep(1);
	lcd_spi_write(spi, &data, 1);
}

static void BlockWrite(struct SpiDevice *spi, uint32_t Xstart, uint32_t Xend, uint32_t Ystart, uint32_t Yend)
{
	WriteComm(spi, 0x2B); /* Set row address */
	WriteData(spi, (Ystart >> 8) & 0xff);
	WriteData(spi, Ystart & 0xff);
	WriteData(spi, (Yend >> 8) & 0xff);
	WriteData(spi, Yend & 0xff);

	WriteComm(spi, 0x2A); /* Set coloum address */
	WriteData(spi, (Xstart >> 8) & 0xff);
	WriteData(spi, Xstart & 0xff);
	WriteData(spi, (Xend >> 8) & 0xff);
	WriteData(spi, Xend & 0xff);

	WriteComm(spi, 0x2c);
}

static unsigned long long reverse_bytes(unsigned long long value)
{
 return (value & 0x00000000000000FFUL) << 48 |
        (value & 0x000000000000FF00UL) << 48 |
        (value & 0x0000000000FF0000UL) << 16 |
        (value & 0x00000000FF000000UL) << 16 |
        (value & 0x000000FF00000000UL) >> 16 |
        (value & 0x0000FF0000000000UL) >> 16 |
        (value & 0x00FF000000000000UL) >> 48 |
        (value & 0xFF00000000000000UL) >> 48;
}

static uint32_t endian64bit_convert(void *src_data, void *dest_data, uint32_t size)
{
    uint8_t *src = (uint8_t *)src_data;
    uint8_t *dest = (uint8_t *)dest_data;

    if (size % 8) {
        hal_trace_printf("endian64bit_convert size error %d\n", size);
        return -1;
    }
    while (size > 0) {
        *(unsigned long long *)dest =
            reverse_bytes(*(unsigned long long *)src);
        src += 8;
        dest += 8;
        size -= 8;
    }
    return 0;
}

void frame_post(uint8_t *buf, uint32_t col, uint32_t row)
{
	uint32_t size;

    size = col * row * 2;

    //endian64bit_convert(buf, fbbuf_write, size);
	BlockWrite(lcd_spi, 0, col -1, 0, row - 1);
	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 1);
	lcd_spi_write(lcd_spi, buf, size);
}

uint32_t bsp_disp_lcd_write_line(uint32_t disp, uint32_t x, uint32_t y, void *buf, uint32_t len)
{
    uint32_t size;
	BlockWrite(lcd_spi, x, (x + (len / 2) - 1), y, y);
	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 1);
	size = len;
	lcd_spi_write(lcd_spi, buf, size);
}

void LCD_WriteOneDot(uint16_t color)
{
	uint8_t data[2];

	data[0] = (color >> 8) & 0xff;
	data[1] = color & 0xff;
	lcd_spi_write(lcd_spi, &data, 2);
}

void LCD_Clear(uint16_t Color)
{
	uint32_t i;

	BlockWrite(lcd_spi, 0, COL-1, 0, ROW-1);
	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 1);
	for(i = 0; i < COL * ROW; i++) {
       LCD_WriteOneDot(Color);
	}
}

void LCD_Clear1(uint16_t xStart,uint16_t yStart, uint16_t xlong,uint16_t ylong,uint16_t Color)
{
	uint32_t i,j;
	j = xlong * ylong;
	BlockWrite(lcd_spi, xStart, xlong-1, yStart, ylong-1);
	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 1);
	for (i = 0; i < j; i++) {
		LCD_WriteOneDot(Color);
	}
}

uint32_t image_test(void)
{
	frame_post(gImage_test, COL, ROW);
	return 0;
}

uint32_t lcd_test(void)
{
	uint32_t i = 0, j = 0;
	uint32_t ret = -1;
	uint16_t *pixel = NULL;

	pixel = (uint16_t *)malloc(COL * ROW * 2);
	if (!pixel)
		goto OUT;

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (i < 30) {
               pixel[(i * COL) + j] = (0xf800 << 8) | (0xf800 >> 8);
			   //pixel[(i * COL) + j] = 0xf800;
            } else if (i < 60) {
                pixel[(i * COL) + j] = (0x07e0 << 8) |(0x07e0 >> 8);
				//pixel[(i * COL) + j] = 0x07e0;
            } else if (i < 90) {
                pixel[(i * COL) + j] = (0x001f << 8) | (0x001f >> 8);
				//pixel[(i * COL) + j] = 0x001f;
            } else {
                pixel[(i * COL) + j] = 0xffff;
            }
        }
    }

	//for (i = 0; i < 320 * 240; i++)
		//hal_trace_printf("pixel[%d]: 0x%02x\n", i, pixel[i]);
	frame_post((uint8_t*)pixel, COL, ROW);
	free(pixel);
	pixel = NULL;
	return 0;
OUT:
	free(pixel);
	return ret;
}

void LCD_Single_Color(uint16_t color)
{
#if 1
	uint16_t *pixel = NULL;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t size;

	BlockWrite(lcd_spi, 0, COL-1, 0, ROW-1);

	hal_gpio_pin_set_dir(GPIO_DC, HAL_GPIO_DIR_OUT, 1);

	pixel = (uint16_t*)malloc(COL * ROW * 2);

	if (!pixel)
		goto OUT;
#if 0
	for (i = 0; i < COL * ROW * 2; i += 2) {
		pixel[i] = (color >> 8) & 0xff;
		pixel[i+1] = color & 0xff;
		//lcd_spi_write(lcd_spi, pixel, 2);
	}
#endif
	for (i = 0; i < COL * ROW; i ++) {
		pixel[i] = (color << 8) | (color >> 8);
		lcd_spi_write(lcd_spi, pixel, 2);
	}
	//size = COL * ROW * 2;
	//lcd_spi_write(lcd_spi, pixel, size);
	free(pixel);
	return 0;
#endif

OUT:
	hal_trace_printf("\n");
	return -1;

}

static uint32_t color_screen(uint16_t color)
{
	uint16_t *pixel = NULL;
	uint32_t i = 0;
	uint32_t size;

	pixel = (uint16_t *)malloc(COL * ROW * 2);
	if (!pixel)
		goto OUT;
#if 0
	for (i = 0; i < COL * ROW * 2; i += 2) {
		pixel[i] = 0xff & (color >> 8);
		pixel[i + 1] = 0xff & color;
	}
#endif

	for (i = 0; i < COL * ROW; i++) {
		pixel[i] = (color << 8) | (color >> 8);
	}

	frame_post((uint8_t *)pixel, COL, ROW);

	hal_trace_printf("black screen exit\n");
	free(pixel);
	pixel = NULL;
	return 0;
OUT:
	hal_trace_printf("\n");
	return -1;
}

static void init_panel_test(struct SpiDevice *spi)
{
	WriteComm(spi, 0x11); /* Sleep Out */
	aos_msleep(60);
	WriteComm(spi, 0xb2);

	WriteData(spi, 0x0c);
	WriteData(spi, 0x0c);
	WriteData(spi, 0x00);
	WriteData(spi, 0x33);
	WriteData(spi, 0x33);
	WriteComm(spi, 0xb7);
	WriteData(spi, 0x75);

	WriteComm(spi, 0x3A); /* Interface Pixel Format */
	WriteData(spi, 0x55);
	WriteComm(spi, 0xb0);
	WriteData(spi, 0x03);
	WriteData(spi, 0xc0); /*change to little endian*/

	WriteComm(spi, 0x36); /* Memory WriteData Access Control */
	//WriteData(spi, 0xa0); /* landscape */
	WriteData(spi, 0x00); /* Portrait */
	//WriteData(spi, 0xC0); /* Portrait */

	WriteComm(spi, 0xbb);
	WriteData(spi, 0x19);

	WriteComm(spi, 0xc0);
	WriteData(spi, 0x2c);

	WriteComm(spi, 0xc2);
	WriteData(spi, 0x01);

	WriteComm(spi, 0xc3);
	WriteData(spi, 0x12);

	WriteComm(spi, 0xc4);
	WriteData(spi, 0x20);

	WriteComm(spi, 0xc6);
	WriteData(spi, 0x0f);

	WriteComm(spi, 0xd0);
	WriteData(spi, 0xa4);
	WriteData(spi, 0xa1);

	WriteComm(spi, 0xe0);
	WriteData(spi, 0xd0);
	WriteData(spi, 0x01);
	WriteData(spi, 0x04);
	WriteData(spi, 0x09);
	WriteData(spi, 0x0b);
	WriteData(spi, 0x07);
	WriteData(spi, 0x2e);
	WriteData(spi, 0x44);
	WriteData(spi, 0x43);
	WriteData(spi, 0x0b);
	WriteData(spi, 0x16);
	WriteData(spi, 0x15);
	WriteData(spi, 0x17);
	WriteData(spi, 0x1D);

	WriteComm(spi, 0xE1);
	WriteData(spi, 0xD0);
	WriteData(spi, 0x01);
	WriteData(spi, 0x05);
	WriteData(spi, 0x0A);
	WriteData(spi, 0x0B);
	WriteData(spi, 0x08);
	WriteData(spi, 0x2F);
	WriteData(spi, 0x44);
	WriteData(spi, 0x41);
	WriteData(spi, 0x0A);
	WriteData(spi, 0x15);
	WriteData(spi, 0x14);
	WriteData(spi, 0x19);
	WriteData(spi, 0x1D);

	BlockWrite(spi, 0, COL - 1, 0, ROW - 1);
	aos_msleep(60);

	WriteComm(spi, 0x29); /* Display ON */
}

static void init_panel(struct SpiDevice *spi)
{
	WriteComm(spi, 0x11); /* Sleep Out */
	aos_msleep(60);
	WriteComm(spi, 0xb2);

	WriteData(spi, 0x0c);
	WriteData(spi, 0x0c);
	WriteData(spi, 0x00);
	WriteData(spi, 0x33);
	WriteData(spi, 0x33);
	WriteComm(spi, 0xb7);
	WriteData(spi, 0x75);


	WriteComm(spi, 0x3A); /* Interface Pixel Format */
	WriteData(spi, 0x55);
	WriteComm(spi, 0xb0);
	WriteData(spi, 0x03);
	WriteData(spi, 0xc0); /*change to little endian*/

	WriteComm(spi, 0x36); /* Memory WriteData Access Control */

	//WriteData(spi, 0xa0); /* landscape */
	WriteData(spi, 0x00); /* Portrait */

	WriteComm(spi, 0xbb);
	WriteData(spi, 0x2b);
	WriteComm(spi, 0xc0);
	WriteData(spi, 0x2c);
	WriteComm(spi, 0xc2);
	WriteData(spi, 0x01);
	WriteComm(spi, 0xc3);
	WriteData(spi, 0x0b);
	WriteComm(spi, 0xc4);
	WriteData(spi, 0x20);
	WriteComm(spi, 0xc6);
	WriteData(spi, 0x0f);
	WriteComm(spi, 0xd0);
	WriteData(spi, 0xa4);
	WriteData(spi, 0xa1);

	WriteComm(spi, 0xe0);
	WriteData(spi, 0xd0);
	WriteData(spi, 0x01);
	WriteData(spi, 0x04);
	WriteData(spi, 0x09);
	WriteData(spi, 0x0b);
	WriteData(spi, 0x07);
	WriteData(spi, 0x2e);
	WriteData(spi, 0x44);
	WriteData(spi, 0x43);
	WriteData(spi, 0x0b);
	WriteData(spi, 0x16);
	WriteData(spi, 0x15);
	WriteData(spi, 0x17);
	WriteData(spi, 0x1D);

	WriteComm(spi, 0xE1);
	WriteData(spi, 0xD0);
	WriteData(spi, 0x01);
	WriteData(spi, 0x05);
	WriteData(spi, 0x0A);
	WriteData(spi, 0x0B);
	WriteData(spi, 0x08);
	WriteData(spi, 0x2F);
	WriteData(spi, 0x44);
	WriteData(spi, 0x41);
	WriteData(spi, 0x0A);
	WriteData(spi, 0x15);
	WriteData(spi, 0x14);
	WriteData(spi, 0x19);
	WriteData(spi, 0x1D);

	BlockWrite(spi, 0, COL - 1, 0, ROW - 1);
	aos_msleep(60);

	WriteComm(spi, 0x29); /* Display ON */
}

static void exit_panel(struct SpiDevice *spi)
{
	WriteComm(spi, 0x28);
	aos_msleep(20);
	WriteComm(spi, 0x10);
	aos_msleep(20);

	hal_gpio_pin_set_dir(GPIO_RESET, HAL_GPIO_DIR_OUT, 0);
	aos_msleep(10);
}

static uint32_t panel_blank(struct SpiDevice *spi, uint32_t en)
{
	if (en)
		WriteComm(spi, 0x28);
	else
		WriteComm(spi, 0x29);
	return 0;
}

static void gpio_init(void)
{
	hal_trace_printf("gpio_init start\n");
	reset_init();
	dc_init();
	bl_en_init();
	hal_trace_printf("gpio_init ok\n");
}

void lcd_power_on(void)
{
	/* config lcd_power pin to open lcd power0 */
	uint32_t ret = 0;
	hal_trace_printf("lcd_power_on star\n");
	ret = spi_init();
	if (ret) {
		hal_trace_printf("Init spi fail!\n");
		return;
	}

	gpio_init();
	hal_gpio_pin_set_dir(GPIO_RESET, HAL_GPIO_DIR_OUT, 1);
	aos_msleep(50);
	hal_gpio_pin_set_dir(GPIO_RESET, HAL_GPIO_DIR_OUT, 0);
	aos_msleep(50);
	hal_gpio_pin_set_dir(GPIO_RESET, HAL_GPIO_DIR_OUT, 1);
	aos_msleep(20);

	init_panel(lcd_spi);
	aos_msleep(20);
	hal_gpio_pin_set_dir(GPIO_BL, HAL_GPIO_DIR_OUT, 1);
#if 0
	hal_trace_printf("init ok,show color\n");
	color_screen(0xf800);
	aos_msleep(1000);
	color_screen(0xffe0);
	aos_msleep(1000);
	color_screen(0x07e0);
	aos_msleep(1000);
	color_screen(0x07ff);
	aos_msleep(1000);
	color_screen(0x001f);
	aos_msleep(1000);
	color_screen(0xf81f);
	aos_msleep(1000);

	LCD_Clear1(0, 0, 240, 100, 0xf800);
	LCD_Clear1(0, 100, 240, 200, 0xffe0);
	LCD_Clear1(0, 200, 80, 300, 0xf800);
	LCD_Clear1(80, 200, 160, 300, 0xffe0);

	lcd_test();
	aos_msleep(1000);
#endif
	image_test();
}

void lcd_power_off(void)
{
	hal_gpio_pin_set_dir(GPIO_BL, HAL_GPIO_DIR_OUT, 0);
	panel_blank(lcd_spi, 1);
	if (lcd_spi) {
		exit_panel(lcd_spi);
		free(lcd_spi);
		lcd_spi = NULL;
	} else
		hal_trace_printf("lcd power off fail\n");
	return;
}
