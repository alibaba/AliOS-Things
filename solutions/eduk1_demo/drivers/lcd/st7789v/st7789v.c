/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "st7789v.h"
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"
#include "hal_iomux_haas1000.h"
#include <stdio.h>

#define ST7789_HEIGHT    240
#define ST7789_WIDTH     320
#define ST7789_DC_PIN    HAL_GPIO_PIN_P0_4
#define ST7789_RESET_PIN HAL_GPIO_PIN_P0_0

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_IDMOFF  0x38
#define ST7789_IDMON   0x39

#define ST7789_CASET 0x2A
#define ST7789_RASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_RAMRD 0x2E

#define ST7789_COLMOD 0x3A
#define ST7789_MADCTL 0x36

#define ST7789_PTLAR    0x30
#define ST7789_VSCRDEF  0x33
#define ST7789_VSCRSADD 0x37

#define ST7789_WRDISBV  0x51
#define ST7789_WRCTRLD  0x53
#define ST7789_WRCACE   0x55
#define ST7789_WRCABCMB 0x5e

#define ST7789_POWSAVE    0xbc
#define ST7789_DLPOFFSAVE 0xbd

#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00

#define ST_CMD_DELAY 0x80

#define SPI_MAX_BLOCK 60000

static spi_dev_t  spi_st7789 = {0};
static gpio_dev_t gpio_st7789_dc;
static gpio_dev_t gpio_st7789_reset;
static gpio_dev_t gpio_st7789_blk;

static inline void spi_write(const void *data, uint32_t size)
{
    if (size > SPI_MAX_BLOCK) {
        uint32_t rest  = size;
        uint32_t start = 0;
        while (rest > SPI_MAX_BLOCK) {
            hal_spi_send(&spi_st7789, (const void *)(data + start),
                         SPI_MAX_BLOCK, 10);
            start += SPI_MAX_BLOCK;
            rest -= SPI_MAX_BLOCK;
        }
        hal_spi_send(&spi_st7789, (const void *)(data + start), rest, 10);
    } else {
        hal_spi_send(&spi_st7789, (const void *)data, (uint16_t)size, 10);
    }
}

static inline void write_command(uint8_t c)
{
    hal_gpio_output_low(&gpio_st7789_dc);
    spi_write(&c, sizeof(c));
}

static inline void write_data(uint8_t c)
{
    hal_gpio_output_high(&gpio_st7789_dc);
    spi_write(&c, sizeof(c));
}

static void
set_addr_window(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1)
{
    uint8_t data[4];

    if (x_0 > x_1) {
        x_0 = x_0 ^ x_1;
        x_1 = x_0 ^ x_1;
        x_0 = x_0 ^ x_1;
    }
    if (y_0 > y_1) {
        y_0 = y_0 ^ y_1;
        y_1 = y_0 ^ y_1;
        y_0 = y_0 ^ y_1;
    }

    write_command(ST7789_CASET);
    hal_gpio_output_high(&gpio_st7789_dc);
    data[0] = x_0 >> 8;
    data[1] = x_0;
    data[2] = x_1 >> 8;
    data[3] = x_1;
    spi_write(data, 4);
    write_command(ST7789_RASET);
    hal_gpio_output_high(&gpio_st7789_dc);
    data[0] = y_0 >> 8;
    data[1] = y_0;
    data[2] = y_1 >> 8;
    data[3] = y_1;
    spi_write(data, 4);
    write_command(ST7789_RAMWR);
}

static void command_list(void)
{
    hal_gpio_output_high(&gpio_st7789_reset);
    aos_msleep(1);
    hal_gpio_output_low(&gpio_st7789_reset);
    aos_msleep(10);
    hal_gpio_output_high(&gpio_st7789_reset);
    aos_msleep(120);

    write_command(0x36);
    write_data(0x00);

    write_command(0x3a);
    write_data(0x05);

    write_command(0xb2);
    write_data(0x0c);
    write_data(0x0c);
    write_data(0x00);
    write_data(0x33);
    write_data(0x33);

    write_command(0xb7);
    write_data(0x70);

    write_command(0xbb);
    write_data(0x21);

    write_command(0xc0);
    write_data(0x2c);

    write_command(0xc2);
    write_data(0x01);

    write_command(0xc3);
    write_data(0x0B);

    write_command(0xc4);
    write_data(0x27);

    write_command(0xc6);
    write_data(0x0f);

    write_command(0xd0);
    write_data(0xa4);
    write_data(0xA1);

    write_command(0xe0);
    write_data(0xD0);
    write_data(0x06);
    write_data(0x0B);
    write_data(0x09);
    write_data(0x08);
    write_data(0x30);
    write_data(0x30);
    write_data(0x5B);
    write_data(0x4B);
    write_data(0x18);
    write_data(0x14);
    write_data(0x14);
    write_data(0x2C);
    write_data(0x32);

    write_command(0xe1);
    write_data(0xD0);
    write_data(0x05);
    write_data(0x0A);
    write_data(0x0A);
    write_data(0x07);
    write_data(0x28);
    write_data(0x32);
    write_data(0x2C);
    write_data(0x49);
    write_data(0x18);
    write_data(0x13);
    write_data(0x13);
    write_data(0x2C);
    write_data(0x33);

    write_command(0x21);

    write_command(0x2A);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0xEF);

    write_command(0x2B);
    write_data(0x00);
    write_data(0x00);
    write_data(0x01);
    write_data(0x3F);

    write_command(0x11);
    aos_msleep(120);

    write_command(0x29);
    write_command(0x2c);
}

static uint8_t hardware_init(void)
{
    int32_t ret = 0;

    spi_st7789.port              = 0;
    spi_st7789.config.mode       = SPI_WORK_MODE_3; // CPOL = 1; CPHA = 1
    spi_st7789.config.freq       = 26000000;
    spi_st7789.config.role       = SPI_ROLE_MASTER;
    spi_st7789.config.firstbit   = SPI_FIRSTBIT_MSB;
    spi_st7789.config.t_mode     = SPI_TRANSFER_NORMAL;
    spi_st7789.config.serial_len = 8;
    spi_st7789.config.data_size  = SPI_DATA_SIZE_8BIT;
    spi_st7789.config.cs         = SPI_CS_DIS;

    gpio_st7789_dc.port   = ST7789_DC_PIN;
    gpio_st7789_dc.config = OUTPUT_PUSH_PULL;
    gpio_st7789_dc.priv   = NULL;

    gpio_st7789_reset.port   = ST7789_RESET_PIN;
    gpio_st7789_reset.config = OUTPUT_PUSH_PULL;
    gpio_st7789_reset.priv   = NULL;

    gpio_st7789_blk.port   = HAL_IOMUX_PIN_P0_6;
    gpio_st7789_blk.config = OUTPUT_PUSH_PULL;
    hal_gpio_output_high(&gpio_st7789_blk);

    printf("hardware_init START\r\n");

    ret |= hal_spi_init(&spi_st7789);

    ret |= hal_gpio_init(&gpio_st7789_dc);
    ret |= hal_gpio_init(&gpio_st7789_reset);

    printf("hal_gpio_init done\r\n");

    ret |= hal_gpio_output_low(&gpio_st7789_reset);
    aos_msleep(500);
    ret |= hal_gpio_output_high(&gpio_st7789_reset);

    printf("hal_spi_init done\r\n");

    if (ret) {
        printf("hardware_init fail\r\n");
        return ret;
    }

    return ret;
}

uint8_t st7789_init(void)
{
    uint8_t err_code;

    err_code = hardware_init();
    if (err_code != 0) {
        return err_code;
    }

    command_list();

    st7789_rotation_set(90);

    return err_code;
}

static void st7789_uninit(void)
{
    ;
}

void st7789_pixel_draw(uint16_t x, uint16_t y, uint16_t color)
{
    set_addr_window(x, y, x, y);

    const uint8_t data[2] = {color >> 8, color};

    hal_gpio_output_high(&gpio_st7789_dc);

    spi_write(data, sizeof(data));

    hal_gpio_output_low(&gpio_st7789_dc);
}

void st7789_rect_draw(uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint16_t color)
{
    set_addr_window(x, y, x + width - 1, y + height - 1);
    uint32_t bufferSize = width * height;
    hal_gpio_output_high(&gpio_st7789_dc);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);
    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[2 * i]     = color >> 8;
        burst_buffer[2 * i + 1] = color;
    }
    spi_write(burst_buffer, bufferSize * 2);
    free(burst_buffer);
}

void st7789_line_draw(uint16_t x,
                      uint16_t y,
                      uint16_t length,
                      uint8_t  dir,
                      uint16_t color)
{
    if (dir == 0) {
        st7789_rect_draw(x, y, length, 1, color);
    } else {
        st7789_rect_draw(x, y, 1, length, color);
    }
}

void st7789_area_draw(uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint8_t *frame,
                      uint32_t areaSize)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;

    if (width * height != areaSize) {
        printf("error parm width * height != areaSize\n");
        return;
    }

    set_addr_window(x, y, x + width - 1, y + height - 1);
    uint32_t bufferSize = width * height;
    hal_gpio_output_high(&gpio_st7789_dc);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);
    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[2 * i]     = rgb565_frame[i] >> 8;
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }
    spi_write(burst_buffer, bufferSize * 2);
    free(burst_buffer);
}

void st7789_frame_draw(uint8_t *frame)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;
    uint32_t  bufferSize   = ST7789_HEIGHT * ST7789_WIDTH;

    set_addr_window(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
    hal_gpio_output_high(&gpio_st7789_dc);

    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);

    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[2 * i]     = rgb565_frame[i] >> 8;
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }

    spi_write(burst_buffer, bufferSize * 2);

    free(burst_buffer);
}

void st7789_dummy_display(void) { /* No implementation needed. */ }

void st7789_rotation_set(uint16_t rotation)
{
    write_command(ST7789_MADCTL);
    switch (rotation) {
        case 0:
            write_data(ST7789_MADCTL_MX | ST7789_MADCTL_RGB);
            break;
        case 90:
            write_data(ST7789_MADCTL_MV | ST7789_MADCTL_MX | ST7789_MADCTL_RGB);
            break;
        case 180:
            write_data(ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
            break;
        case 270:
            write_data(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_MV |
                       ST7789_MADCTL_RGB);
            break;
        default:
            break;
    }
}

static void st7789_display_invert(uint8_t invert)
{
    write_command(invert ? ST7789_INVON : ST7789_INVOFF);
}

void EnterSleep(void)
{
    write_command(0x28);
    aos_msleep(20);
    write_command(0x10);
}

void ExitSleep(void)

{
    write_command(0x11);
    aos_msleep(120);
    write_command(0x29);
}
