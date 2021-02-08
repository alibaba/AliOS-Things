#include <stdio.h>
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"
#include "hal_lcd.h"

// user define area
#ifdef AOS_APP_GAME_DEMO
#define ILI9341_HEIGHT 320
#define ILI9341_WIDTH 240
#else
#define ILI9341_HEIGHT 240
#define ILI9341_WIDTH 320
#endif
#define ILI9341_DC_PIN 1
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

typedef uint32_t ret_code_t;

static spi_dev_t spi_ili9341 = {0};
static gpio_dev_t gpio_ili9341_dc;
static gpio_dev_t gpio_ili9341_reset;

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
    data[0] = y_0 >> 8;
    data[1] = y_0;
    data[2] = y_1 >> 8;
    data[3] = y_1;
    spi_write(data, 4);
    write_command(ILI9341_RAMWR);
}

static void command_list(void)
{
    write_command(ILI9341_SWRESET);
    aos_msleep(120);
    write_command(ILI9341_DISPOFF);
    aos_msleep(120);
    write_command(ILI9341_PWCTRB);
    write_data(0x00);
    write_data(0XC1);
    write_data(0X30);

    write_command(ILI9341_TIMCTRA);
    write_data(0x85);
    write_data(0x00);
    write_data(0x78);

    write_command(ILI9341_PWCTRSEQ);
    write_data(0x39);
    write_data(0x2C);
    write_data(0x00);
    write_data(0x34);
    write_data(0x02);

    write_command(ILI9341_PUMP);
    write_data(0x20);

    write_command(ILI9341_TIMCTRB);
    write_data(0x00);
    write_data(0x00);

    write_command(ILI9341_PWCTR1);
    write_data(0x23);

    write_command(ILI9341_PWCTR2);
    write_data(0x10);

    write_command(ILI9341_VMCTR1);
    write_data(0x3e);
    write_data(0x28);

    write_command(ILI9341_VMCTR2);
    write_data(0x86);

    write_command(ILI9341_MADCTL);
    write_data(0x48);

    write_command(ILI9341_PIXFMT);
    write_data(0x55);

    write_command(ILI9341_FRMCTR1);
    write_data(0x00);
    write_data(0x18);

    write_command(ILI9341_DFUNCTR);
    write_data(0x08);
    write_data(0x82);
    write_data(0x27);

    write_command(ILI9341_ENGMCTR);
    write_data(0x00);

    write_command(ILI9341_GAMMASET);
    write_data(0x01);

    write_command(ILI9341_GMCTRP1);
    write_data(0x0F);
    write_data(0x31);
    write_data(0x2B);
    write_data(0x0C);
    write_data(0x0E);
    write_data(0x08);
    write_data(0x4E);
    write_data(0xF1);
    write_data(0x37);
    write_data(0x07);
    write_data(0x10);
    write_data(0x03);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x00);

    write_command(ILI9341_GMCTRN1);
    write_data(0x00);
    write_data(0x0E);
    write_data(0x14);
    write_data(0x03);
    write_data(0x11);
    write_data(0x07);
    write_data(0x31);
    write_data(0xC1);
    write_data(0x48);
    write_data(0x08);
    write_data(0x0F);
    write_data(0x0C);
    write_data(0x31);
    write_data(0x36);
    write_data(0x0F);

    write_command(ILI9341_SLPOUT);
    aos_msleep(120);
    write_command(ILI9341_DISPON);
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
    spi_ili9341.config.cs = SPI_CS_EN;

    gpio_ili9341_dc.port = ILI9341_DC_PIN;
    gpio_ili9341_dc.config = OUTPUT_PUSH_PULL;
    gpio_ili9341_dc.priv = NULL;

    gpio_ili9341_reset.port = ILI9341_RESET_PIN;
    gpio_ili9341_reset.config = OUTPUT_PUSH_PULL;
    gpio_ili9341_reset.priv = NULL;

    printf("hardware_init START\r\n");

    ret |= hal_gpio_init(&gpio_ili9341_dc);
    ret |= hal_gpio_init(&gpio_ili9341_reset);

    printf("hal_gpio_init done\r\n");

    ret |= hal_gpio_output_low(&gpio_ili9341_reset);
    aos_msleep(500);
    ret |= hal_gpio_output_high(&gpio_ili9341_reset);
    ret |= hal_spi_init(&spi_ili9341);

    printf("hal_spi_init done\r\n");

    if (ret)
    {
        printf("hardware_init fail\r\n");
        return ret;
    }

    return ret;
}

static ret_code_t ili9341_init(void)
{
    ret_code_t err_code;

    err_code = hardware_init();
    if (err_code != 0)
    {
        return err_code;
    }

    command_list();

    return err_code;
}

static void ili9341_uninit(void)
{
    //去初始化spi
}

static void ili9341_pixel_draw(uint16_t x, uint16_t y, uint32_t color)
{
    set_addr_window(x, y, x, y);

    const uint8_t data[2] = {color >> 8, color};

    hal_gpio_output_high(&gpio_ili9341_dc);

    spi_write(data, sizeof(data));

    hal_gpio_output_low(&gpio_ili9341_dc);
}

// static void ili9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
// {
//     set_addr_window(x, y, x + width - 1, y + height - 1);

//     const uint8_t data[2] = {color >> 8, color};

//     hal_gpio_output_high(&gpio_ili9341_dc);

//     // Duff's device algorithm for optimizing loop.
//     uint32_t i = (height * width + 7) / 8;

//     /*lint -save -e525 -e616 -e646 */
//     switch ((height * width) % 8)
//     {
//     case 0:
//         do
//         {
//             spi_write(data, sizeof(data));
//         case 7:
//             spi_write(data, sizeof(data));
//         case 6:
//             spi_write(data, sizeof(data));
//         case 5:
//             spi_write(data, sizeof(data));
//         case 4:
//             spi_write(data, sizeof(data));
//         case 3:
//             spi_write(data, sizeof(data));
//         case 2:
//             spi_write(data, sizeof(data));
//         case 1:
//             spi_write(data, sizeof(data));
//         } while (--i > 0);
//     default:
//         break;
//     }
//     /*lint -restore */

//     hal_gpio_output_low(&gpio_ili9341_dc);
// }

static void ili9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
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

static void ili9341_line_draw(uint16_t x, uint16_t y, uint16_t length, uint8_t dir, uint32_t color)
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

static void ili9341_area_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *frame, uint32_t areaSize)
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
    uint8_t *burst_buffer = (unsigned char *)malloc(bufferSize * 2);
    for (uint32_t i = 0; i < bufferSize; i++)
    {
        burst_buffer[2 * i] = (rgb565_frame[i] >> 8);
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }
    spi_write(burst_buffer, bufferSize * 2);
    free(burst_buffer);
}

static void ili9341_frame_draw(uint8_t *frame)
{
    uint16_t *rgb565_frame = (uint16_t *)frame;
    set_addr_window(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);

    uint32_t bufferSize = ILI9341_HEIGHT * ILI9341_WIDTH;

    hal_gpio_output_high(&gpio_ili9341_dc);

    unsigned char *burst_buffer = (unsigned char *)malloc(bufferSize * 2);

    //printf("burst_buffer\n");

    for (uint32_t i = 0; i < bufferSize; i++)
    {
        burst_buffer[2 * i] = rgb565_frame[i] >> 8;
        burst_buffer[2 * i + 1] = rgb565_frame[i];
    }

    //printf("fill burst_buffer\n");

    spi_write(burst_buffer, bufferSize * 2);

    free(burst_buffer);
}

static void ili9341_dummy_display(void)
{
    /* No implementation needed. */
}

static void ili9341_rotation_set(hal_lcd_rotation_t rotation)
{
    write_command(ILI9341_MADCTL);
    switch (rotation)
    {
    case HAL_LCD_ROTATE_0:
        write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
        break;
    case HAL_LCD_ROTATE_90:
        write_data(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
        break;
    case HAL_LCD_ROTATE_180:
        write_data(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
        break;
    case HAL_LCD_ROTATE_270:
        write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
        break;
    default:
        break;
    }
}

static void ili9341_display_invert(bool invert)
{
    write_command(invert ? ILI9341_INVON : ILI9341_INVOFF);
}

static lcd_cb_t ili9341_cb = {
    .height = ILI9341_HEIGHT,
    .width = ILI9341_WIDTH};

const hal_lcd_t hal_lcd_ili9341 = {
    .lcd_init = ili9341_init,
    .lcd_uninit = ili9341_uninit,
    .lcd_pixel_draw = ili9341_pixel_draw,
    .lcd_rect_draw = ili9341_rect_draw,
    .lcd_line_draw = ili9341_line_draw,
    .lcd_frame_draw = ili9341_frame_draw,
    .lcd_area_draw = ili9341_area_draw,
    .lcd_display = ili9341_dummy_display,
    .lcd_rotation_set = ili9341_rotation_set,
    .lcd_display_invert = ili9341_display_invert,
    .p_lcd_cb = &ili9341_cb};

hal_lcd_t *get_hal_lcd(void)
{
    return &hal_lcd_ili9341;
}