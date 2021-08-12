/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "st7789v_vfs.h"
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"
#include <stdio.h>

#include <epoll.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "aos/vfs.h"
#include "hal2vfs/io_gpio.h"
#include "hal2vfs/io_spi.h"
#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>

#include "aos/hal/gpio.h"

static int        spi_fd     = -1;
static spi_dev_t  spi_st7789 = {0};
static gpio_dev_t gpio_st7789_dc;
static gpio_dev_t gpio_st7789_reset;
static gpio_dev_t gpio_st7789_blk;

int                   gpio_fd = -1;
int                   gpio_st7789_reset_fd;
int                   gpio_st7789_blink_fd;
struct gpio_io_config gpio_st7789_dc_config;
struct gpio_io_config gpio_st7789_reset_config;
struct gpio_io_config gpio_st7789_blink_config;

static int gpio_set(int gpio_id)
{
    struct gpio_io_config config = {0};
    config.id                    = gpio_id;
    config.config                = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;
    config.data                  = 1;
    int ret = ioctl(gpio_fd, IOC_GPIO_SET, (unsigned long)&config);

    return ret;
}

static int gpio_rst(int gpio_id)
{
    struct gpio_io_config config = {0};
    config.id                    = gpio_id;
    config.config                = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;
    config.data                  = 0;
    int ret = ioctl(gpio_fd, IOC_GPIO_SET, (unsigned long)&config);
    printf("gpio write %d return %d\r\n", gpio_id, ret);

    return ret;
}

static uint8_t hardware_init(void)
{
    int ret = 0;

    spi_fd = open("/dev/spi%d", ST7789_SPI_PORT);
    printf("open /dev/spi%d %s, fd:%d\r\n", ST7789_SPI_PORT,
           spi_fd >= 0 ? "success" : "fail", spi_fd);

    if (spi_fd >= 0) {
        ret |= ioctl(spi_fd, IOC_SPI_SET_CFLAG,
                     SPI_MODE_3 | SPI_MSB | SPI_MASTER | SPI_DATA_8BIT);
        ret |= ioctl(spi_fd, IOC_SPI_SET_FREQ, 26000000);
        ret |= ioctl(spi_fd, IOC_SPI_SET_SERIAL_LEN, 8);
    }

    gpio_fd = open("/dev/gpio", 0);
    printf("open /dev/gpio %s, fd:%d\r\n", gpio_fd >= 0 ? "success" : "fail",
           gpio_fd);

    gpio_st7789_dc.port   = ST7789_DC_PIN;
    gpio_st7789_dc.config = OUTPUT_PUSH_PULL;
    gpio_st7789_dc.priv   = NULL;

    gpio_st7789_reset.port   = ST7789_RESET_PIN;
    gpio_st7789_reset.config = OUTPUT_PUSH_PULL;
    gpio_st7789_reset.priv   = NULL;

    ret |= hal_gpio_init(&gpio_st7789_dc);
    ret |= hal_gpio_init(&gpio_st7789_reset);

    ret |= gpio_rst(ST7789_RESET_PIN);
    aos_msleep(500);
    ret |= gpio_set(ST7789_RESET_PIN);

    if (ret) {
        printf("hardware_init fail\r\n");
        return ret;
    }

    printf("hardware_init done\r\n");

    return ret;
}

static inline void spi_write(const void *data, uint32_t size)
{
    if (size > SPI_MAX_BLOCK) {
        uint32_t rest  = size;
        uint32_t start = 0;
        while (rest > SPI_MAX_BLOCK) {
            write(spi_fd, (const void *)(data + start), SPI_MAX_BLOCK);
            start += SPI_MAX_BLOCK;
            rest -= SPI_MAX_BLOCK;
        }
        write(spi_fd, (const void *)(data + start), rest);
    } else {
        write(spi_fd, (const void *)data, (uint16_t)size);
    }
}

static inline void write_command(uint8_t c)
{
    gpio_rst(ST7789_DC_PIN);
    spi_write(&c, sizeof(c));
}

static inline void write_data(uint8_t c)
{
    gpio_set(ST7789_DC_PIN);
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
    gpio_set(ST7789_DC_PIN);
    data[0] = x_0 >> 8;
    data[1] = x_0;
    data[2] = x_1 >> 8;
    data[3] = x_1;
    spi_write(data, 4);
    write_command(ST7789_RASET);
    gpio_set(ST7789_DC_PIN);
    data[0] = y_0 >> 8;
    data[1] = y_0;
    data[2] = y_1 >> 8;
    data[3] = y_1;
    spi_write(data, 4);
    write_command(ST7789_RAMWR);
}

static void command_list(void)
{
    gpio_set(ST7789_RESET_PIN);
    aos_msleep(1);
    gpio_rst(ST7789_RESET_PIN);
    aos_msleep(10);
    gpio_set(ST7789_RESET_PIN);
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
    // 去初始化spi
}

void st7789_pixel_draw(uint16_t x, uint16_t y, uint16_t color)
{
    set_addr_window(x, y, x, y);

    const uint8_t data[2] = {color >> 8, color};

    gpio_set(ST7789_DC_PIN);

    spi_write(data, sizeof(data));

    gpio_rst(ST7789_DC_PIN);
}

void st7789_rect_draw(uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint16_t color)
{
    set_addr_window(x, y, x + width - 1, y + height - 1);
    uint32_t bufferSize = width * height;
    gpio_set(ST7789_DC_PIN);
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
    gpio_set(ST7789_DC_PIN);
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
    gpio_set(ST7789_DC_PIN);

    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);

    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[2 * i]     = rgb565_frame[i] >> 8;
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }

    spi_write(burst_buffer, bufferSize * 2);

    free(burst_buffer);
}

void st7789_dummy_display(void) { ; }

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
