/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "ili9341_fb.h"
#include "hal_iomux_haas1000.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <fb.h>
#include <devicevfs/devicevfs.h>

#define ILI9341_DEVICE_NAME "ili9341_fb"

#define CLCD_WIDTH  (ILI9341_WIDTH)
#define CLCD_HEIGHT (ILI9341_HEIGHT)

ili9341_dev_t dev_ili9341 = {0};

struct drv_clcd_device {
    uint32_t width;
    uint32_t height;

    uint8_t *fb;
};
struct drv_clcd_device _lcd;

static int ili9341_open(fb_info_t *info, int user);
static int ili9341_release(fb_info_t *info, int user);
static ssize_t ili9341_read(fb_info_t *info, char *buf, size_t count);
static ssize_t ili9341_write(fb_info_t *info, const char *buf, size_t count);
static int ili9341_blank(int blank, fb_info_t *info);
static int ili9341_pan_display(fb_var_screeninfo_t *var, fb_info_t *info);
static ili9341_ioctl(fb_info_t *info, unsigned int cmd, unsigned long arg);
static int ili9341_mmap(fb_info_t *info, size_t size);

static const fb_ops_t ili9341_ops = {
    .fb_open    = ili9341_open,
    .fb_release    = ili9341_release,
    .fb_read        = ili9341_read,
    .fb_write       = ili9341_write,
    .fb_pan_display    = ili9341_pan_display,
    .fb_mmap    = ili9341_mmap,
    .fb_sync    = NULL,
    .fb_blank    = ili9341_blank,
    .fb_ioctl    = ili9341_ioctl,
};


static const fb_fix_screeninfo_t ili9341_fix = {
    .id         = ILI9341_DEVICE_NAME,
    .xpanstep    = 0,
    .ypanstep    = 0,
    .ywrapstep    = 0,
    .line_length    = 0,
    .mmio_start    = 0,    /* Not implemented for char. special, so */
    .mmio_len    = 0    /* these are set to 0 */
};



static int ili9341_open(fb_info_t *info, int user)
{
    return 0;
}

static int ili9341_release(fb_info_t *info, int user)
{
    return 0;
}

static ssize_t ili9341_read(fb_info_t *info, char *buf, size_t count)
{
    return 0;
}

static ssize_t ili9341_write(fb_info_t *info, const char *buf, size_t count)
{
    return 0;
}

static int ili9341_blank(int blank, fb_info_t *info)
{
    return 0;
}

static int ili9341_pan_display(fb_var_screeninfo_t *var, fb_info_t *info)
{
    printf("ili9341 pan display function:0xp\n", info->screen_buffer);

    ili9341_draw_frame(dev_ili9341, info->screen_buffer);
    printf("ili9341_draw_frame done\n");

    return 0;
}

static int ili9341_mmap(fb_info_t *info, size_t size)
{
    // AOS3.3 no need mmap buffer
    return 0;
}

static ili9341_ioctl(fb_info_t *info, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case FBIO_WAITFORVSYNC:
            // ret = aos_sem_wait((aos_sem_t *)&vsync_sem, AOS_WAIT_FOREVER);
            // if (unlikely(ret)) {
            //     printf("input semaphore wait err\n");
            // }
            break;
        case FBIOPUT_PREFB_ADDR:
            printf("fbioput buffer setting\n");
            info->screen_buffer = (uint8_t *)arg;
            printf("ili9341 ili9341_ioctl function:0xp\n", info->screen_buffer);
            if (info->screen_buffer == NULL)
                return -ENOMEM;

            break;
    }
    return ret;
}


int ili9341_fb_init(void)
{
    int ret = 0;

    // set fb params to init
    dev_ili9341.spi_port = 0;
    dev_ili9341.spi_freq = 26000000;
    dev_ili9341.gpio_bgl_id = HAL_GPIO_PIN_P0_6;
    dev_ili9341.gpio_dc_id = HAL_GPIO_PIN_P0_1;
    dev_ili9341.gpio_reset_id = HAL_GPIO_PIN_P0_0;

    ili9341_hw_init(&dev_ili9341);

    // set rotation
    ili9341_set_rotation(dev_ili9341, 90);

    // create fbdev params
    fb_info_t *info = NULL;

    /* memset _lcd to zero */
    memset(&_lcd, 0x0, sizeof(_lcd));

    _lcd.width  = CLCD_WIDTH;
    _lcd.height = CLCD_HEIGHT;

    info = framebuffer_alloc(0);

    /*set var screeninfo*/
    memset(&info->var, 0, sizeof(fb_var_screeninfo_t));
    info->var.xres = _lcd.width;
    info->var.yres = _lcd.height;
    info->var.xres_virtual = _lcd.width;
    info->var.yres_virtual = _lcd.height;
    info->var.width = _lcd.width;
    info->var.height = _lcd.height;
    info->var.rotate = FB_ROTATE_UD; // rotate 0 degrees wiseclock
#ifdef RGB_888_FORMAT
    info->var.bits_per_pixel = 4 * 8;
#endif
#ifdef RGB_565_FORMAT
    info->var.bits_per_pixel = 2 * 8;
#endif
    info->screen_size = info->var.xres * info->var.yres * info->var.bits_per_pixel / 8;
    info->screen_base = _lcd.fb;
    /*set fb ops function*/
    info->fbops = &ili9341_ops;

    /*set fix screeninfo*/
    info->fix = ili9341_fix;
    info->fix.line_length = info->var.xres * info->var.bits_per_pixel / 8;

    ret = register_framebuffer(info);

    printf("ili9341 hardware_init %s\n", ret == 0 ? "success" : "fail");

    return ret;
}

LEVEL1_DRIVER_ENTRY(ili9341_fb_init)
