/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"
#include "hal_iomux_haas1000.h"
#include "hal_lcd.h"

// user define area
#define ILI9341_HEIGHT 240
#define ILI9341_WIDTH 320
#define ILI9341_DC_PIN 4
#define ILI9341_RESET_PIN 0

// Set of commands described in ILI9341 datasheet.
#define ILI9341_NOP 0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID 0x04
#define ILI9341_RDDST 0x09

#define ILI9341_SLPIN 0x10
#define ILI9341_SLPOUT 0x11
#define ILI9341_PTLON 0x12
#define ILI9341_NORON 0x13

#define ILI9341_RDMODE 0x0A
#define ILI9341_RDMADCTL 0x0B
#define ILI9341_RDPIXFMT 0x0C
#define ILI9341_RDIMGFMT 0x0D
#define ILI9341_RDSELFDIAG 0x0F

#define ILI9341_INVOFF 0x20
#define ILI9341_INVON 0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON 0x29

#define ILI9341_CASET 0x2A
#define ILI9341_PASET 0x2B
#define ILI9341_RAMWR 0x2C
#define ILI9341_RAMRD 0x2E

#define ILI9341_PTLAR 0x30
#define ILI9341_MADCTL 0x36
#define ILI9341_PIXFMT 0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR 0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1 0xC0
#define ILI9341_PWCTR2 0xC1
#define ILI9341_PWCTR3 0xC2
#define ILI9341_PWCTR4 0xC3
#define ILI9341_PWCTR5 0xC4
#define ILI9341_VMCTR1 0xC5
#define ILI9341_VMCTR2 0xC7
#define ILI9341_PWCTRSEQ 0xCB
#define ILI9341_PWCTRA 0xCD
#define ILI9341_PWCTRB 0xCF

#define ILI9341_RDID1 0xDA
#define ILI9341_RDID2 0xDB
#define ILI9341_RDID3 0xDC
#define ILI9341_RDID4 0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
#define ILI9341_DGMCTR1 0xE2
#define ILI9341_DGMCTR2 0xE3
#define ILI9341_TIMCTRA 0xE8
#define ILI9341_TIMCTRB 0xEA

#define ILI9341_ENGMCTR 0xF2
#define ILI9341_INCTR 0xF6
#define ILI9341_PUMP 0xF7

#define ILI9341_MADCTL_MY 0x80
#define ILI9341_MADCTL_MX 0x40
#define ILI9341_MADCTL_MV 0x20
#define ILI9341_MADCTL_ML 0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH 0x04

#define false 0
#define true 1

#define SPI_MAX_BLOCK 60000

static spi_dev_t spi_ili9341 = {0};
static gpio_dev_t gpio_ili9341_dc;
static gpio_dev_t gpio_ili9341_reset;
static gpio_dev_t gpio_ili9341_blk;

static inline void spi_write(const void *data, uint32_t size)
{
    if (size > SPI_MAX_BLOCK)
    {
        uint32_t rest = size;
        uint32_t start = 0;
        while (rest > SPI_MAX_BLOCK)
        {
            hal_spi_send(&spi_ili9341, (const void *)(data + start), SPI_MAX_BLOCK, 10);
            start += SPI_MAX_BLOCK;
            rest -= SPI_MAX_BLOCK;
        }
        hal_spi_send(&spi_ili9341, (const void *)(data + start), rest, 10);
    }
    else
    {
        hal_spi_send(&spi_ili9341, (const void *)data, (uint16_t)size, 10);
    }
}

static inline void write_command(uint8_t c)
{
    hal_gpio_output_low(&gpio_ili9341_dc);
    spi_write(&c, sizeof(c));
}

static inline void write_data(uint8_t c)
{
    hal_gpio_output_high(&gpio_ili9341_dc);
    spi_write(&c, sizeof(c));
}

static void set_addr_window(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1)
{
    uint8_t data[4];

    if (x_0 > x_1)
    {
        x_0 = x_0 ^ x_1;
        x_1 = x_0 ^ x_1;
        x_0 = x_0 ^ x_1;
    }
    if (y_0 > y_1)
    {
        y_0 = y_0 ^ y_1;
        y_1 = y_0 ^ y_1;
        y_0 = y_0 ^ y_1;
    }

    write_command(ILI9341_CASET);
    hal_gpio_output_high(&gpio_ili9341_dc);
    data[0] = x_0 >> 8;
    data[1] = x_0;
    data[2] = x_1 >> 8;
    data[3] = x_1;
    spi_write(data, 4);
    write_command(ILI9341_PASET);
	hal_gpio_output_high(&gpio_ili9341_dc);
    data[0] = y_0 >> 8;
    data[1] = y_0;
    data[2] = y_1 >> 8;
    data[3] = y_1;
    spi_write(data, 4);
    write_command(ILI9341_RAMWR);
}

static void command_list(void)
{
    hal_gpio_output_high(&gpio_ili9341_reset);
    aos_msleep(1);
    hal_gpio_output_low(&gpio_ili9341_reset);
    aos_msleep(10);
    hal_gpio_output_high(&gpio_ili9341_reset);
    aos_msleep(120);

    write_command(0x36);
    write_data(0x00);

    write_command(0x3a);
    write_data(0x05); //65k
    //--------------------------------ST7789V Frame rate setting----------------------------------//
    write_command(0xb2);
    write_data(0x0c);
    write_data(0x0c);
    write_data(0x00);
    write_data(0x33);
    write_data(0x33);

    write_command(0xb7);
    write_data(0x70);

    //---------------------------------ST7789V Power setting--------------------------------------//
    write_command(0xbb);
    write_data(0x21); //VCOM

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
    //--------------------------------ST7789V gamma setting---------------------------------------//
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

    write_command(0x21); //反显

    write_command(0x2A); //Frame rate control
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0xEF);

    write_command(0x2B); //Display function control
    write_data(0x00);
    write_data(0x00);
    write_data(0x01);
    write_data(0x3F);

    write_command(0x11);
    aos_msleep(120); //aos_msleep 120ms

    write_command(0x29); //display on
    write_command(0x2c);
}

static ret_code_t hardware_init(void)
{
    int32_t ret;

    spi_ili9341.port = 0;
    spi_ili9341.config.mode = SPI_WORK_MODE_3; // CPOL = 1; CPHA = 1
    spi_ili9341.config.freq = 26000000;
    spi_ili9341.config.role = SPI_ROLE_MASTER;
    spi_ili9341.config.firstbit = SPI_FIRSTBIT_MSB;
    spi_ili9341.config.t_mode = SPI_TRANSFER_NORMAL;
    spi_ili9341.config.serial_len = 8;
    spi_ili9341.config.data_size = SPI_DATA_SIZE_8BIT;
    spi_ili9341.config.cs = SPI_CS_DIS;

    gpio_ili9341_dc.port = ILI9341_DC_PIN;
    gpio_ili9341_dc.config = OUTPUT_PUSH_PULL;
    gpio_ili9341_dc.priv = NULL;

    gpio_ili9341_reset.port = ILI9341_RESET_PIN;
    gpio_ili9341_reset.config = OUTPUT_PUSH_PULL;
    gpio_ili9341_reset.priv = NULL;

    gpio_ili9341_blk.port = HAL_IOMUX_PIN_P0_6;
    gpio_ili9341_blk.config = OUTPUT_PUSH_PULL;
    hal_gpio_output_high(&gpio_ili9341_blk);

    printf("hardware_init START\r\n");

    ret |= hal_spi_init(&spi_ili9341);

    ret |= hal_gpio_init(&gpio_ili9341_dc);
    ret |= hal_gpio_init(&gpio_ili9341_reset);

    printf("hal_gpio_init done\r\n");

    ret |= hal_gpio_output_low(&gpio_ili9341_reset);
    aos_msleep(500);
    ret |= hal_gpio_output_high(&gpio_ili9341_reset);


    printf("hal_spi_init done\r\n");

    if (ret)
    {
        printf("hardware_init fail\r\n");
        return ret;
    }

    return ret;
}

 ret_code_t ili9341_init(void)
{
    ret_code_t err_code;

    err_code = hardware_init();
    if (err_code != 0)
    {
        return err_code;
    }

    command_list();

    ili9341_rotation_set(90);

    return err_code;
}

static void ili9341_uninit(void)
{
    //去初始化spi
}

 void ili9341_pixel_draw(uint16_t x, uint16_t y, uint16_t color)
{
    set_addr_window(x, y, x, y);

    const uint8_t data[2] = {color >> 8, color};

    hal_gpio_output_high(&gpio_ili9341_dc);

    spi_write(data, sizeof(data));

    hal_gpio_output_low(&gpio_ili9341_dc);
}

 void ili9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    set_addr_window(x, y, x + width - 1, y + height - 1);
    uint32_t bufferSize = width * height;
    hal_gpio_output_high(&gpio_ili9341_dc);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);
    for (uint32_t i = 0; i < bufferSize; i++)
    {
        burst_buffer[2 * i] = color >> 8;
        burst_buffer[2 * i + 1] = color;
    }
    spi_write(burst_buffer, bufferSize * 2);
    free(burst_buffer);
}

 void ili9341_line_draw(uint16_t x, uint16_t y, uint16_t length, uint8_t dir, uint16_t color)
{
    if (dir == 0) // vir
    {
        ili9341_rect_draw(x, y, length, 1, color);
    }
    else
    {
        ili9341_rect_draw(x, y, 1, length, color);
    }
}

 void ili9341_area_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *frame, uint32_t areaSize)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;

    if (width * height != areaSize)
    {
        printf("error parm width * height != areaSize \n");
        return;
    }

    set_addr_window(x, y, x + width - 1, y + height - 1);
    uint32_t bufferSize = width * height;
    hal_gpio_output_high(&gpio_ili9341_dc);
    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);
    for (uint32_t i = 0; i < bufferSize; i++)
    {
        burst_buffer[2 * i] = rgb565_frame[i] >> 8;
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }
    spi_write(burst_buffer, bufferSize * 2);
    free(burst_buffer);
}

 void ili9341_frame_draw(uint8_t *frame)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;
    uint32_t bufferSize = ILI9341_HEIGHT * ILI9341_WIDTH;

    set_addr_window(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
    hal_gpio_output_high(&gpio_ili9341_dc);

    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);

    for (uint32_t i = 0; i < bufferSize; i++)
    {
        burst_buffer[2 * i] = rgb565_frame[i] >> 8;
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }

    spi_write(burst_buffer, bufferSize * 2);

    free(burst_buffer);
}

 void ili9341_dummy_display(void)
{
    /* No implementation needed. */
}

void ili9341_rotation_set(uint16_t rotation)
{
    write_command(ILI9341_MADCTL);
    switch (rotation)
    {
    case 0:
        write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
        break;
    case 90:
        write_data(ILI9341_MADCTL_MV | ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
        break;
    case 180:
        write_data(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
        break;
    case 270:
        write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
        break;
    default:
        break;
    }
}

static void ili9341_display_invert(uint8_t invert)
{
    write_command(invert ? ILI9341_INVON : ILI9341_INVOFF);
}


//NOTE:VCC=3.3V
//************************指令*****************************
// void write_command(unsigned char y) // (uchar y,uchar x)
// {
//     unsigned char i;
//     cs = 0;
//     rs = 0;
//     for (i = 0; i < 8; i++)
//     {
//         scl = 0;
//         if (y & 0x80)
//         {
//             sdi = 1;
//         }
//         else
//         {
//             sdi = 0;
//         }
//         scl = 1;
//         y = y << 1;
//     }
//     cs = 1;
// }
//************************************数据***************************
// void write_data(unsigned char w) // (uchar w, uchar v)
// {
//     unsigned char i;
//     cs = 0;
//     rs = 1;
//     for (i = 0; i < 8; i++)
//     {
//         scl = 0;
//         if (w & 0x80)
//         {
//             sdi = 1;
//         }
//         else
//         {
//             sdi = 0;
//         }
//         scl = 1;
//         w = w << 1;
//     }
//     cs = 1;
// }
//*********************************送显示数据

void EnterSleep(void)
{
    write_command(0x28);
    aos_msleep(20);
    write_command(0x10);
}

//*********************************************************
void ExitSleep(void)

{
    write_command(0x11);
    aos_msleep(120);
    write_command(0x29);
}
