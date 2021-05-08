/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "st7789.h"
#include "aos/vfs.h"
#include "vfsdev/gpio_dev.h"
#include "vfsdev/spi_dev.h"

#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>
#include <stdio.h>
#include <sys/ioctl.h>

#define SPI_MAX_BLOCK 60000

typedef enum {
    CMD = 0,
    DAT = 1,
} st7789_dc_t;

static int gpio_output(int gpio_fd, int gpio_id, bool output)
{
    struct gpio_io_config config = {0};
    config.id                    = gpio_id;
    config.config                = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;
    config.data                  = output > 0;
    int ret = ioctl(gpio_fd, IOC_GPIO_SET, (unsigned long)&config);
    return ret;
}

static inline void spi_write(int spi_fd, const void *data, uint32_t size)
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

static inline void st7789_dc_write_bytes(st7789_dev_t st7789_dev,
                                         st7789_dc_t  st7789_dc,
                                         uint8_t *bytes,
                                         uint32_t     size)
{
    gpio_output(st7789_dev.gpio_fd, st7789_dev.gpio_dc_id, st7789_dc);
    spi_write(st7789_dev.spi_fd, bytes, size);
}

static inline void st7789_dc_write_byte(st7789_dev_t st7789_dev,
                                        st7789_dc_t  st7789_dc,
                                        uint8_t      byte)
{
    gpio_output(st7789_dev.gpio_fd, st7789_dev.gpio_dc_id, st7789_dc);
    spi_write(st7789_dev.spi_fd, &byte, 1);
}

int st7789_hw_init(st7789_dev_t *st7789_dev)
{
    int ret = 0;

    char spi_dev_name[20] = {0};
    sprintf(spi_dev_name, "/dev/spi%d", st7789_dev->spi_port);

    st7789_dev->spi_fd = open(spi_dev_name, 0);

    if (st7789_dev->spi_fd > 0) {
        ret |= ioctl(st7789_dev->spi_fd, IOC_SPI_SET_CFLAG,
                     SPI_MODE_3 | SPI_MSB | SPI_MASTER | SPI_DATA_8BIT);
        ret |=
            ioctl(st7789_dev->spi_fd, IOC_SPI_SET_FREQ, st7789_dev->spi_freq);
        ret |= ioctl(st7789_dev->spi_fd, IOC_SPI_SET_SERIAL_LEN, 8);
    } else {
        printf(
            "st7789 hardware_init fail, open /dev/spi%d fiail, spi_fd is %d\n",
            st7789_dev->spi_port, st7789_dev->spi_fd);
        return 1;
    }

    st7789_dev->gpio_fd = open("/dev/gpio", 0);
    if (st7789_dev->gpio_fd > 0) {
        ret |= gpio_output(st7789_dev->gpio_fd, st7789_dev->gpio_reset_id, 0);
        usleep(50 * 1000);
        ret |= gpio_output(st7789_dev->gpio_fd, st7789_dev->gpio_reset_id, 1);
    } else {
        printf(
            "st7789 hardware_init fail, open /dev/gpio fail, gpio_fd is %d\n",
            st7789_dev->gpio_fd);
        return 1;
    }

    st7789_init_command(*st7789_dev);

    printf("st7789 hardware_init %s\n", ret == 0 ? "success" : "fail");

    return ret;
}

static void st7789_init_command(st7789_dev_t st7789_dev)
{
    st7789_dc_write_byte(st7789_dev, CMD, ST7789_MADCTL);
    st7789_dc_write_byte(st7789_dev, DAT, 0x00);

    st7789_dc_write_byte(st7789_dev, CMD, ST7789_COLMOD);
    st7789_dc_write_byte(st7789_dev, DAT, 0x05);

    st7789_dc_write_byte(st7789_dev, CMD, 0xb2);
    st7789_dc_write_byte(st7789_dev, DAT, 0x0c);
    st7789_dc_write_byte(st7789_dev, DAT, 0x0c);
    st7789_dc_write_byte(st7789_dev, DAT, 0x00);
    st7789_dc_write_byte(st7789_dev, DAT, 0x33);
    st7789_dc_write_byte(st7789_dev, DAT, 0x33);

    st7789_dc_write_byte(st7789_dev, CMD, 0xb7);
    st7789_dc_write_byte(st7789_dev, DAT, 0x70);

    st7789_dc_write_byte(st7789_dev, CMD, 0xbb);
    st7789_dc_write_byte(st7789_dev, DAT, 0x21);

    st7789_dc_write_byte(st7789_dev, CMD, 0xc0);
    st7789_dc_write_byte(st7789_dev, DAT, 0x2c);

    st7789_dc_write_byte(st7789_dev, CMD, 0xc2);
    st7789_dc_write_byte(st7789_dev, DAT, 0x01);

    st7789_dc_write_byte(st7789_dev, CMD, 0xc3);
    st7789_dc_write_byte(st7789_dev, DAT, 0x0B);

    st7789_dc_write_byte(st7789_dev, CMD, 0xc4);
    st7789_dc_write_byte(st7789_dev, DAT, 0x27);

    st7789_dc_write_byte(st7789_dev, CMD, 0xc6);
    st7789_dc_write_byte(st7789_dev, DAT, 0x0f);

    st7789_dc_write_byte(st7789_dev, CMD, 0xd0);
    st7789_dc_write_byte(st7789_dev, DAT, 0xa4);
    st7789_dc_write_byte(st7789_dev, DAT, 0xa1);

    st7789_dc_write_byte(st7789_dev, CMD, 0xe0);
    st7789_dc_write_byte(st7789_dev, DAT, 0xD0);
    st7789_dc_write_byte(st7789_dev, DAT, 0x06);
    st7789_dc_write_byte(st7789_dev, DAT, 0x0B);
    st7789_dc_write_byte(st7789_dev, DAT, 0x09);
    st7789_dc_write_byte(st7789_dev, DAT, 0x08);
    st7789_dc_write_byte(st7789_dev, DAT, 0x30);
    st7789_dc_write_byte(st7789_dev, DAT, 0x30);
    st7789_dc_write_byte(st7789_dev, DAT, 0x5B);
    st7789_dc_write_byte(st7789_dev, DAT, 0x4B);
    st7789_dc_write_byte(st7789_dev, DAT, 0x18);
    st7789_dc_write_byte(st7789_dev, DAT, 0x14);
    st7789_dc_write_byte(st7789_dev, DAT, 0x14);
    st7789_dc_write_byte(st7789_dev, DAT, 0x2C);
    st7789_dc_write_byte(st7789_dev, DAT, 0x32);

    st7789_dc_write_byte(st7789_dev, CMD, 0xe1);
    st7789_dc_write_byte(st7789_dev, DAT, 0xD0);
    st7789_dc_write_byte(st7789_dev, DAT, 0x05);
    st7789_dc_write_byte(st7789_dev, DAT, 0x0A);
    st7789_dc_write_byte(st7789_dev, DAT, 0x0A);
    st7789_dc_write_byte(st7789_dev, DAT, 0x07);
    st7789_dc_write_byte(st7789_dev, DAT, 0x28);
    st7789_dc_write_byte(st7789_dev, DAT, 0x32);
    st7789_dc_write_byte(st7789_dev, DAT, 0x2C);
    st7789_dc_write_byte(st7789_dev, DAT, 0x49);
    st7789_dc_write_byte(st7789_dev, DAT, 0x18);
    st7789_dc_write_byte(st7789_dev, DAT, 0x13);
    st7789_dc_write_byte(st7789_dev, DAT, 0x13);
    st7789_dc_write_byte(st7789_dev, DAT, 0x2C);
    st7789_dc_write_byte(st7789_dev, DAT, 0x33);

    st7789_dc_write_byte(st7789_dev, CMD, ST7789_INVON);

    st7789_dc_write_byte(st7789_dev, CMD, ST7789_CASET);
    st7789_dc_write_byte(st7789_dev, DAT, 0x00);
    st7789_dc_write_byte(st7789_dev, DAT, 0x00);
    st7789_dc_write_byte(st7789_dev, DAT, 0x00);
    st7789_dc_write_byte(st7789_dev, DAT, 0xEF);

    st7789_dc_write_byte(st7789_dev, CMD, ST7789_RASET);
    st7789_dc_write_byte(st7789_dev, DAT, 0x00);
    st7789_dc_write_byte(st7789_dev, DAT, 0x00);
    st7789_dc_write_byte(st7789_dev, DAT, 0x01);
    st7789_dc_write_byte(st7789_dev, DAT, 0x3F);

    st7789_dc_write_byte(st7789_dev, CMD, ST7789_SLPOUT);
    usleep(120 * 1000);

    st7789_dc_write_byte(st7789_dev, CMD, ST7789_DISPON);
    st7789_dc_write_byte(st7789_dev, CMD, ST7789_RAMWR);
}

int st7789_hw_uninit(st7789_dev_t *st7789_dev)
{
    int ret = 0;
    ret |= close(st7789_dev->spi_fd);
    ret |= close(st7789_dev->gpio_fd);
    return ret;
}

static void set_addr_window(st7789_dev_t st7789_dev,
                            uint16_t     x_0,
                            uint16_t     y_0,
                            uint16_t     x_1,
                            uint16_t     y_1)
{
    uint8_t data[4] = {0};
    if (x_0 < x_1) {
        data[0] = x_0 >> 8;
        data[1] = x_0;
        data[2] = x_1 >> 8;
        data[3] = x_1;
    } else {
        data[0] = x_1 >> 8;
        data[1] = x_1;
        data[2] = x_0 >> 8;
        data[3] = x_0;
    }
    st7789_dc_write_byte(st7789_dev, CMD, ST7789_CASET);
    st7789_dc_write_bytes(st7789_dev, DAT, data, 4);
    if (y_0 < y_1) {
        data[0] = y_0 >> 8;
        data[1] = y_0;
        data[2] = y_1 >> 8;
        data[3] = y_1;
    } else {
        data[0] = y_1 >> 8;
        data[1] = y_1;
        data[2] = y_0 >> 8;
        data[3] = y_0;
    }
    st7789_dc_write_byte(st7789_dev, CMD, ST7789_RASET);
    st7789_dc_write_bytes(st7789_dev, DAT, data, 4);

    st7789_dc_write_byte(st7789_dev, CMD, ST7789_RAMWR);
}

void st7789_draw_pixel(st7789_dev_t st7789_dev,
                       uint16_t     x,
                       uint16_t     y,
                       uint16_t     color)
{
    set_addr_window(st7789_dev, x, y, x, y);
    uint8_t data[2] = {color >> 8, color};
    st7789_dc_write_bytes(st7789_dev, DAT, data, 2);
}

void st7789_draw_area(st7789_dev_t st7789_dev,
                      uint16_t     x,
                      uint16_t     y,
                      uint16_t     width,
                      uint16_t     height,
                      uint8_t *frame)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;

    set_addr_window(st7789_dev, x, y, x + width - 1, y + height - 1);
    uint32_t       bufferSize   = width * height * sizeof(uint16_t);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize);
    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[i] =
            (bufferSize % 2) ? rgb565_frame[i] : rgb565_frame[i] >> 8;
    }
    st7789_dc_write_bytes(st7789_dev, DAT, burst_buffer, bufferSize);
    free(burst_buffer);
}

void st7789_draw_rect(st7789_dev_t st7789_dev,
                      uint16_t     x,
                      uint16_t     y,
                      uint16_t     width,
                      uint16_t     height,
                      uint16_t     color)
{
    set_addr_window(st7789_dev, x, y, x + width - 1, y + height - 1);
    uint32_t       bufferSize   = width * height * sizeof(uint16_t);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize);
    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[i] = (bufferSize % 2) ? color : color >> 8;
    }
    st7789_dc_write_bytes(st7789_dev, DAT, burst_buffer, bufferSize);
    free(burst_buffer);
}

void st7789_draw_frame(st7789_dev_t st7789_dev, uint8_t *frame)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;
    uint32_t  bufferSize   = ST7789_HEIGHT * ST7789_WIDTH * sizeof(uint16_t);
    set_addr_window(st7789_dev, 0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize);
    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[i] =
            (bufferSize % 2) ? rgb565_frame[i] : rgb565_frame[i] >> 8;
    }
    st7789_dc_write_bytes(st7789_dev, DAT, burst_buffer, bufferSize);
    free(burst_buffer);
}

void st7789_set_rotation(st7789_dev_t st7789_dev, uint16_t rotation)
{
    st7789_dc_write_byte(st7789_dev, CMD, ST7789_MADCTL);
    switch (rotation) {
        case 0:
            st7789_dc_write_byte(st7789_dev, DAT,
                                 ST7789_MADCTL_MX | ST7789_MADCTL_RGB);
            break;
        case 90:
            st7789_dc_write_byte(st7789_dev, DAT,
                                 ST7789_MADCTL_MV | ST7789_MADCTL_MX |
                                     ST7789_MADCTL_RGB);
            break;
        case 180:
            st7789_dc_write_byte(st7789_dev, DAT,
                                 ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
            break;
        case 270:
            st7789_dc_write_byte(st7789_dev, DAT,
                                 ST7789_MADCTL_MX | ST7789_MADCTL_MY |
                                     ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            break;
        default:
            break;
    }
}

void st7789_set_invert(st7789_dev_t st7789_dev, uint8_t invert)
{
    st7789_dc_write_byte(st7789_dev, CMD,
                         invert ? ST7789_INVON : ST7789_INVOFF);
}

void st7789_enter_sleep(st7789_dev_t st7789_dev)
{
    st7789_dc_write_byte(st7789_dev, CMD, 0x28);
    usleep(20 * 1000);
    st7789_dc_write_byte(st7789_dev, CMD, 0x10);
}

void st7789_exit_sleep(st7789_dev_t st7789_dev)
{
    st7789_dc_write_byte(st7789_dev, CMD, 0x11);
    usleep(120 * 1000);
    st7789_dc_write_byte(st7789_dev, CMD, 0x29);
}
