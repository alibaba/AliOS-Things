/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "ili9341.h"
#include "aos/vfs.h"
#include "vfsdev/gpio_dev.h"
#include "vfsdev/spi_dev.h"

#include <stdio.h>
#include <sys/ioctl.h>

#define SPI_MAX_BLOCK 60000

typedef enum {
    CMD = 0,
    DAT = 1,
} ili9341_dc_t;

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

static inline void ili9341_dc_write_bytes(ili9341_dev_t ili9341_dev,
                                          ili9341_dc_t  ili9341_dc,
                                          uint8_t *bytes,
                                          uint32_t      size)
{
    gpio_output(ili9341_dev.gpio_fd, ili9341_dev.gpio_dc_id, ili9341_dc);
    spi_write(ili9341_dev.spi_fd, bytes, size);
}

static inline void ili9341_dc_write_byte(ili9341_dev_t ili9341_dev,
                                         ili9341_dc_t  ili9341_dc,
                                         uint8_t       byte)
{
    gpio_output(ili9341_dev.gpio_fd, ili9341_dev.gpio_dc_id, ili9341_dc);
    spi_write(ili9341_dev.spi_fd, &byte, 1);
}

int ili9341_hw_init(ili9341_dev_t *ili9341_dev)
{
    int ret = 0;

    char spi_dev_name[20] = {0};
    sprintf(spi_dev_name, "/dev/spi%d", ili9341_dev->spi_port);

    ili9341_dev->spi_fd = open(spi_dev_name, 0);

    if (ili9341_dev->spi_fd > 0) {
        ret |= ioctl(ili9341_dev->spi_fd, IOC_SPI_SET_CFLAG,
                     SPI_MODE_3 | SPI_MSB | SPI_MASTER | SPI_DATA_8BIT);
        ret |=
            ioctl(ili9341_dev->spi_fd, IOC_SPI_SET_FREQ, ili9341_dev->spi_freq);
        ret |= ioctl(ili9341_dev->spi_fd, IOC_SPI_SET_SERIAL_LEN, 8);
    } else {
        printf(
            "ili9341 hardware_init fail, open /dev/spi%d fiail, spi_fd is %d\n",
            ili9341_dev->spi_port, ili9341_dev->spi_fd);
        return 1;
    }

    ili9341_dev->gpio_fd = open("/dev/gpio", 0);
    if (ili9341_dev->gpio_fd > 0) {
        ret |= gpio_output(ili9341_dev->gpio_fd, ili9341_dev->gpio_reset_id, 0);
        usleep(50 * 1000);
        ret |= gpio_output(ili9341_dev->gpio_fd, ili9341_dev->gpio_reset_id, 1);
    } else {
        printf(
            "ili9341 hardware_init fail, open /dev/gpio fail, gpio_fd is %d\n",
            ili9341_dev->gpio_fd);
        return 1;
    }

    ili9341_init_command(*ili9341_dev);

    printf("ili9341 hardware_init %s\n", ret == 0 ? "success" : "fail");

    return ret;
}

static void ili9341_init_command(ili9341_dev_t ili9341_dev)
{
    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_SWRESET);
    usleep(1000 * 120);
    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_DISPOFF);
    usleep(1000 * 120);
    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_PWCTRB);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0XC1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0X30);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_TIMCTRA);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x85);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x78);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_PWCTRSEQ);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x39);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x2C);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x34);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x02);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_PUMP);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x20);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_TIMCTRB);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_PWCTR1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x23);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_PWCTR2);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x10);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_VMCTR1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x3e);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x28);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_VMCTR2);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x86);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_MADCTL);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x48);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_PIXFMT);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x55);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_FRMCTR1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x18);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_DFUNCTR);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x08);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x82);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x27);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_ENGMCTR);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_GAMMASET);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x01);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_GMCTRP1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0F);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x31);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x2B);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0C);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0E);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x08);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x4E);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0xF1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x37);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x07);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x10);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x03);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0E);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x09);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_GMCTRN1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x00);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0E);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x14);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x03);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x11);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x07);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x31);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0xC1);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x48);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x08);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0F);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0C);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x31);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x36);
    ili9341_dc_write_byte(ili9341_dev, DAT, 0x0F);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_SLPOUT);
    usleep(1000 * 120);
    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_DISPON);
}

int ili9341_hw_uninit(ili9341_dev_t *ili9341_dev)
{
    int ret = 0;
    ret |= close(ili9341_dev->spi_fd);
    ret |= close(ili9341_dev->gpio_fd);
    return ret;
}

static void set_addr_window(ili9341_dev_t ili9341_dev,
                            uint16_t      x_0,
                            uint16_t      y_0,
                            uint16_t      x_1,
                            uint16_t      y_1)
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
    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_CASET);
    ili9341_dc_write_bytes(ili9341_dev, DAT, data, 4);
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
    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_PASET);
    ili9341_dc_write_bytes(ili9341_dev, DAT, data, 4);

    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_RAMWR);
}

void ili9341_draw_pixel(ili9341_dev_t ili9341_dev,
                        uint16_t      x,
                        uint16_t      y,
                        uint16_t      color)
{
    set_addr_window(ili9341_dev, x, y, x, y);
    uint8_t data[2] = {color >> 8, color};
    ili9341_dc_write_bytes(ili9341_dev, DAT, data, 2);
}

void ili9341_draw_area(ili9341_dev_t ili9341_dev,
                       uint16_t      x,
                       uint16_t      y,
                       uint16_t      width,
                       uint16_t      height,
                       uint8_t *frame)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;

    set_addr_window(ili9341_dev, x, y, x + width - 1, y + height - 1);
    uint32_t       bufferSize   = width * height * sizeof(uint16_t);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize);
    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[i] =
            (bufferSize % 2) ? rgb565_frame[i] : rgb565_frame[i] >> 8;
    }
    ili9341_dc_write_bytes(ili9341_dev, DAT, burst_buffer, bufferSize);
    free(burst_buffer);
}

void ili9341_draw_rect(ili9341_dev_t ili9341_dev,
                       uint16_t      x,
                       uint16_t      y,
                       uint16_t      width,
                       uint16_t      height,
                       uint32_t      color)
{
    set_addr_window(ili9341_dev, x, y, x + width - 1, y + height - 1);
    uint32_t       bufferSize   = width * height * sizeof(uint16_t);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize);
    for (uint32_t i = 0; i < bufferSize; i++) {
        burst_buffer[i] = (bufferSize % 2) ? color : color >> 8;
    }
    ili9341_dc_write_bytes(ili9341_dev, DAT, burst_buffer, bufferSize);
    free(burst_buffer);
}

void ili9341_draw_frame(ili9341_dev_t ili9341_dev, uint8_t *frame)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;
    uint32_t  bufferSize   = ILI9341_HEIGHT * ILI9341_WIDTH * sizeof(uint16_t);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize);

    set_addr_window(ili9341_dev, 0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);

    for (uint32_t i = 0; i < bufferSize / 2; i++) {
        burst_buffer[2 * i] = rgb565_frame[i] >> 8;
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }

    ili9341_dc_write_bytes(ili9341_dev, DAT, burst_buffer, bufferSize);
    free(burst_buffer);
}

void ili9341_set_rotation(ili9341_dev_t ili9341_dev, uint16_t rotation)
{
    ili9341_dc_write_byte(ili9341_dev, CMD, ILI9341_MADCTL);
    switch (rotation) {
        case 0:
            ili9341_dc_write_byte(ili9341_dev, DAT,
                                  ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
            break;
        case 90:
            ili9341_dc_write_byte(ili9341_dev, DAT,
                                  ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            break;
        case 180:
            ili9341_dc_write_byte(ili9341_dev, DAT,
                                  ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
            break;
        case 270:
            ili9341_dc_write_byte(ili9341_dev, DAT,
                                  ILI9341_MADCTL_MX | ILI9341_MADCTL_MY |
                                      ILI9341_MADCTL_BGR);
            break;
        default:
            break;
    }
}

void ili9341_set_invert(ili9341_dev_t ili9341_dev, uint8_t invert)
{
    ili9341_dc_write_byte(ili9341_dev, CMD,
                          invert ? ILI9341_INVON : ILI9341_INVOFF);
}

void ili9341_enter_sleep(ili9341_dev_t ili9341_dev)
{
    ili9341_dc_write_byte(ili9341_dev, CMD, 0x28);
    usleep(20 * 1000);
    ili9341_dc_write_byte(ili9341_dev, CMD, 0x10);
}

void ili9341_exit_sleep(ili9341_dev_t ili9341_dev)
{
    ili9341_dc_write_byte(ili9341_dev, CMD, 0x11);
    usleep(120 * 1000);
    ili9341_dc_write_byte(ili9341_dev, CMD, 0x29);
}
