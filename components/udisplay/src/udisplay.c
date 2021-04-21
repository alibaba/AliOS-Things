/*
 * Copyright (C) 2015-2020 manufacturername
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <fcntl.h>
// #include <sys/mman.h>
#include "udisplay.h"
#include "udisplay_cli.h"

static udisplay_context_t g_udisplay_ctx;


udisplay_context_t *udisplay_get_context(void)
{
    return &g_udisplay_ctx;
}

static uint8_t *get_display_buffer_addr(int32_t num)
{
    udisplay_context_t *ctx = udisplay_get_context();

    if (num == 0)
        return (uint8_t *)ctx->framebuffer;
    else if (num == 1)
        return (uint8_t *)(ctx->framebuffer + ctx->framebuffer_size / 2);

    return NULL;
}

static void display_draw_rect(uint32_t x1, uint32_t y1,
                              uint32_t h, uint32_t w,
                              uint8_t *buf)
{
    udisplay_context_t *ctx = udisplay_get_context();
    int32_t line_length = 0;
    uint32_t location = 0;
    uint32_t byte_location = 0;
    uint8_t bit_location = 0;
    uint8_t *fbaddr = NULL;

    fbaddr = get_display_buffer_addr(ctx->fb_id);
    line_length = w * ctx->var.bits_per_pixel / 8;

    if ((x1 > ctx->var.xres) || (y1 > ctx->var.yres) || \
        ((x1 + w) > ctx->var.xres) || ((y1 + h) > ctx->var.yres)) {
        LOGE("udisplay", "[%s]x1, y1, h, or w is not correct : \
              %d, %d, %d, %d\n", __func__, x1, y1, h, w);
        return;
    }

    /*32 or 24 bit per pixel*/
    if (ctx->var.bits_per_pixel == 32 || ctx->var.bits_per_pixel == 24) {
        uint32_t * fbp32 = (uint32_t *)fbaddr;
        uint32_t *pbuf = buf;
        int32_t y;

        for (y = y1; y <= h + y1; y++) {
            location = (x1 + ctx->var.xoffset) + \
                       (y + ctx->var.yoffset) * line_length / 4;
            memcpy(&fbp32[location], (uint32_t *)pbuf, w * 4);
            pbuf += w;
        }
    }
    /*16 bit per pixel*/
    else if (ctx->var.bits_per_pixel == 16) {
        uint16_t * fbp16 = (uint16_t *)fbaddr;
        uint16_t *pbuf = buf;
        int32_t y;
        for (y = y1; y <= h + y1; y++) {
            location = (x1 + ctx->var.xoffset) + \
                       (y + ctx->var.yoffset) * line_length / 2;
            memcpy(&fbp16[location], (uint16_t *)pbuf, w * 2);
            pbuf += w;
        }
    }
    /*8 bit per pixel*/
    else if (ctx->var.bits_per_pixel == 8) {
        uint8_t * fbp8 = (uint8_t *)fbaddr;
        uint8_t *pbuf = buf;
        int32_t y;
        for (y = y1; y <= h + y1; y++) {
            location = (x1 + ctx->var.xoffset) + \
                       (y + ctx->var.yoffset) * line_length;
            memcpy(&fbp8[location], (uint32_t *)pbuf, w);
            pbuf += w;
        }
    }
    /*1 bit per pixel*/
    else if (ctx->var.bits_per_pixel == 1) {
        uint8_t * fbp8 = (uint8_t *)fbaddr;
        uint8_t *pbuf = buf;
        int32_t x;
        int32_t y;
        for (y = y1; y <= y1 + h; y++) {
            for (x = x1; x <= x1 + w; x++) {
                location = (x + ctx->var.xoffset) + \
                           (y + ctx->var.yoffset) * ctx->var.xres;
                /* find the byte we need to change */
                byte_location = location / 8;
                /* inside the byte found, find the bit we need to change */
                bit_location = location % 8;
                fbp8[byte_location] &= ~(((uint8_t)(1)) << bit_location);
                fbp8[byte_location] |= ((uint8_t)pbuf) << bit_location;
                pbuf++;
            }

            pbuf += w;
        }
    } else {
        /*Not supported bit per pixel*/
    }
}

static void udisplay_draw_rgb32(uint8_t *in, uint32_t x0, uint32_t y0,
                                        uint32_t width, uint32_t height,
                                        int32_t color)
{
    int32_t x, y;
    udisplay_context_t *ctx = udisplay_get_context();
    const int32_t bytesPerPixel = ctx->var.bits_per_pixel / 8;
    const int32_t line_length = width * ctx->var.bits_per_pixel / 8;
    const int32_t stride = line_length / bytesPerPixel;
    uint32_t *dest = (uint32_t *)in;

    for (y = y0; y < height; ++y) {
        for (x = x0; x < width; ++x)
            dest[x] = color;

        dest += stride;
    }
}

static void udisplay_draw_rgb16(uint8_t *in, uint32_t x0, uint32_t y0,
                                      uint32_t width, uint32_t height,
                                      int32_t color)
{
    int32_t x, y;
    udisplay_context_t *ctx = udisplay_get_context();
    const int32_t bytesPerPixel = ctx->var.bits_per_pixel / 8;
    const int32_t line_length = width * ctx->var.bits_per_pixel / 8;
    const int32_t stride = line_length / bytesPerPixel;
    // const int32_t red = (color & 0xff0000) >> (16 + 3);
    // const int32_t green = (color & 0xff00) >> (8 + 2);
    // const int32_t blue = (color & 0xff) >> 3;
    // const short color16 = blue | (green << 5) | (red << (5 + 6));

    short *dest = (short *) (in);

    for (y = y0; y < height; ++y) {
        for (x = x0; x < width; ++x)
            dest[x] = color;

        dest += stride;
    }
}

static int32_t udisplay_set_prefb_addr(uint8_t *buf)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();

    ret = ioctl(ctx->fd, FBIOPUT_PREFB_ADDR, (unsigned long)buf);
    if (ret  < 0) {
        LOGE("udisplay", "[%s]ioctl fail, ret = %d\n", __func__, ret);
        return -1;
    }
    return ret;
}

int32_t udisplay_init(void)
{
    int32_t fd;
    int32_t ret;
    int32_t fb_size;
    uint8_t *fbaddr = NULL;
    fb_var_screeninfo_t fb_var;
    udisplay_context_t *ctx = udisplay_get_context();

    memset(ctx, 0 , sizeof(udisplay_context_t));

    if (aos_mutex_new(&ctx->mutex) != 0) {
        LOGE("udisplay", "[%s]create mutex error\n", __func__);
        return -1;
    }

    /*open framebuffer device*/
    fd = open(FB_PATH, O_RDWR);
    if (fd < 0) {
        LOGE("udisplay", "[%s]open %s fail, fd : %d\n", __func__, FB_PATH, fd);
        return -1;
    }

    /*get framebuffer size*/
    ret = ioctl(fd, FBIOGET_VSCREENINFO, (unsigned long)&fb_var);
    if (ret < 0) {
        close(fd);
        LOGE("udisplay", "[%s]VSCREENINFO fail, ret : %d\n", __func__, ret);
        return -1;
    }

    fb_size = fb_var.xres_virtual * fb_var.yres_virtual
                                  * fb_var.bits_per_pixel / 8;

    // need set fd first becaseof udisplay_set_prefb_addr function used
    ctx->fd = fd;

#ifdef ENABLE_MICRO_KERNEL
    fbaddr = (uint8_t *)mmap(NULL, fb_size, 0, 0, fd, 0);
    if (!fbaddr) {
        close(fd);
        return -1;
    }
    LOGE("udisplay", "[%s]mmap addr : %p\r\n", __func__, fbaddr);
    ctx->framebuffer = fbaddr;
    ctx->framebuffer_size = fb_size;
#endif

    ctx->swap = ((fb_var.xres_virtual * fb_var.yres_virtual) /
                (fb_var.xres * fb_var.yres) == 2) ?
                true : false;
    ctx->fb_id = 0;

    memcpy(&ctx->var, &fb_var, sizeof(fb_var_screeninfo_t));

    return 0;
}

static int32_t wait_vsync(void)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();

    ret = ioctl(ctx->fd, FBIO_WAITFORVSYNC, (unsigned long)&ctx->var);
    if (ret < 0)
        LOGE("udisplay", "[%s]ioctl fail, ret : %d\n", __func__, ret);

    return ret;
}

static int32_t udisplay_pan_display(uint8_t fb_id)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();
    fb_var_screeninfo_t *var = &ctx->var;
#ifdef CONFIG_UDISPLAY_TIME_MEASURE
    long long start, end;
    start = aos_now_ms();
#endif
    var->yoffset = (fb_id > 0) ? fb_id * var->yres : 0;
    ret = ioctl(ctx->fd, FBIOPAN_DISPLAY, (unsigned long)var);
    if (ret < 0)
        LOGE("udisplay", "[%s]aos_ioctl fail, ret : %d\n", __func__, ret);

#ifdef CONFIG_UDISPLAY_TIME_MEASURE
    end = aos_now_ms();
    LOGD("udisplay", "[%s]ioctl time gap: %lld ms\r\n", __func__, end - start);
#endif
#ifdef CONFIG_UDISPLAY_TIME_MEASURE
    start = aos_now_ms();
#endif
    /*check if enable swap buffer*/
    if (ctx->swap) {
        wait_vsync();
        ctx->fb_id = !fb_id;
    }
#ifdef CONFIG_UDISPLAY_TIME_MEASURE
    end = aos_now_ms();
    LOGD("udisplay", "[%s]wait vsync time gap: %lld ms\r\n", \
        __func__, end - start);
#endif
    return ret;
}

int32_t udisplay_show_rect(uint8_t *buf, uint32_t x, uint32_t y, uint32_t w, \
                                        uint32_t h, bool rotate)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();
#ifdef CONFIG_UDISPLAY_TIME_MEASURE
    long long start, end;
    start = aos_now_ms();
#endif

    if (!buf)
        return -1;

    if (aos_mutex_lock(&ctx->mutex, AOS_WAIT_FOREVER) != 0)
        return -1;

    if (rotate) {
        ret = udisplay_set_prefb_addr(buf);
        if (ret < 0) {
            LOGE("udisplay", "[%s]set prefb addr fail\n", __func__);
            aos_mutex_unlock(&ctx->mutex);
            return -1;
        }
    } else {
#ifdef ENABLE_MICRO_KERNEL
        if ((x == 0) && (y == 0)) {
            memcpy(get_display_buffer_addr(ctx->fb_id), buf, \
                    ctx->var.xres * \
                    ctx->var.yres * \
                    ctx->var.bits_per_pixel / 8);
        } else {
            display_draw_rect(x, y, w, h, buf);
        }
#else
        ret = udisplay_set_prefb_addr(buf);
        if (ret < 0) {
            LOGE("udisplay", "[%s]set prefb addr fail\n", __func__);
            aos_mutex_unlock(&ctx->mutex);
            return -1;
        }
#endif
    }

    ret = udisplay_pan_display(ctx->fb_id);
#ifdef CONFIG_UDISPLAY_TIME_MEASURE
    end = aos_now_ms();
    LOGD("udisplay", "[%s]time gap: %lld ms\r\n", __func__, end - start);
#endif
    aos_mutex_unlock(&ctx->mutex);
    return ret;
}

int32_t udisplay_show(void)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();

    if (aos_mutex_lock(&ctx->mutex, AOS_WAIT_FOREVER) != 0)
        return -1;
    ret = udisplay_pan_display(ctx->fb_id);
    aos_mutex_unlock(&ctx->mutex);

    return ret;
}

uint8_t *udisplay_get_framebuffer(void)
{
    uint8_t *fb = NULL;
    udisplay_context_t *ctx = udisplay_get_context();

    if (aos_mutex_lock(&ctx->mutex, AOS_WAIT_FOREVER) != 0)
        return NULL;

    fb = get_display_buffer_addr(ctx->fb_id);
    aos_mutex_unlock(&ctx->mutex);
    return fb;
}

int32_t udisplay_pattern_rgb32(uint32_t color, bool rotate)
{
    int32_t ret;
    uint8_t *buf = NULL;
    udisplay_context_t *ctx = udisplay_get_context();
    fb_var_screeninfo_t *var = &ctx->var;

    if (aos_mutex_lock(&ctx->mutex, AOS_WAIT_FOREVER) != 0)
        return -1;

    if (rotate) {
#ifdef CONFIG_UDISPLAY_TIME_MEASURE
        long long start, end;
        start = aos_now_ms();
#endif
        buf = (uint8_t *)aos_malloc(var->xres * var->yres * \
                                    var->bits_per_pixel / 8);
        if (!buf) {
            LOGE("udisplay", "[%s]no memory allocated\n", __func__);
            aos_mutex_unlock(&ctx->mutex);
            return -1;
        }
        udisplay_draw_rgb32(buf, 0, 0, var->yres, var->xres, color);
        ret = udisplay_set_prefb_addr(buf);
        if (ret < 0) {
            aos_free(buf);
            LOGE("udisplay", "[%s]set prefb addr fail\n", __func__);
            aos_mutex_unlock(&ctx->mutex);
            return -1;
        }
        aos_free(buf);
#ifdef CONFIG_UDISPLAY_TIME_MEASURE
        end = aos_now_ms();
        LOGD("udisplay", "[%s]from malloc and free time gap: %lld ms\r\n", \
            __func__, end - start);
#endif
    } else {
        buf = get_display_buffer_addr(ctx->fb_id);
        udisplay_draw_rgb32(buf, 0, 0, var->xres, var->yres, color);
    }
    ret = udisplay_pan_display(ctx->fb_id);
    aos_mutex_unlock(&ctx->mutex);
    return ret;
}

int32_t udisplay_pattern_rgb16(uint32_t color, bool rotate)
{
    int32_t ret;
    uint8_t *buf = NULL;
    udisplay_context_t *ctx = udisplay_get_context();
    fb_var_screeninfo_t *var = &ctx->var;

    if (aos_mutex_lock(&ctx->mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE("udisplay", "[%s]aos_mutex_lock failed\n", __func__);
        return -1;
    }

#ifdef ENABLE_MICRO_KERNEL
    buf = get_display_buffer_addr(g_udisplay_ctx.fb_id);
#else
    buf = (uint8_t *)aos_malloc(var->xres * var->yres *
                                var->bits_per_pixel / 8);
    if (!buf) {
        LOGE("udisplay", "[%s]no memory allocated\n", __func__);
        aos_mutex_unlock(&ctx->mutex);
        return -1;
    }
#endif

    ret = udisplay_set_prefb_addr(buf);
    if (ret < 0) {
#ifndef ENABLE_MICRO_KERNEL
        aos_free(buf);
#endif
        LOGE("udisplay", "[%s]set prefb addr fail\n", __func__);
        aos_mutex_unlock(&ctx->mutex);
        return -1;
    }
    udisplay_draw_rgb16(buf, 0, 0, var->yres, var->xres, color);
    ret = udisplay_pan_display(ctx->fb_id);
    if (ret < 0) {
#ifndef ENABLE_MICRO_KERNEL
        aos_free(buf);
#endif
        LOGE("udisplay", "[%s]udisplay_pan_display fail\n", __func__);
        aos_mutex_unlock(&ctx->mutex);
        return -1;
    }
#ifndef ENABLE_MICRO_KERNEL
    aos_free(buf);
#endif
    aos_mutex_unlock(&ctx->mutex);
    LOGE("udisplay", "[%s]udisplay_pattern_rgb16 finish\n", __func__);
    return ret;
}

int32_t udisplay_set_brightness(int32_t brightness)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();

    ret = ioctl(ctx->fd, FBIOPUT_BRIGHTNESS, (unsigned long)&brightness);
    if (ret < 0)
        LOGE("udisplay", "[%s]aos_ioctl fail, ret : %d\n", __func__, ret);

    return ret;
}

int32_t udisplay_get_brightness(void)
{
    int32_t ret;
    int32_t brightness;
    udisplay_context_t *ctx = udisplay_get_context();

    ret = ioctl(ctx->fd, FBIOGET_BRIGHTNESS, (unsigned long)&brightness);
    if (ret < 0) {
        LOGE("udisplay", "[%s]aos_ioctl fail, ret : %d\n", __func__, ret);
        return ret;
    }

    return brightness;
}

int32_t udisplay_enable_backlight(void)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();

    ret = ioctl(ctx->fd, FBIOENABLE_BACKLIGHT, NULL);
    if (ret < 0)
        LOGE("udisplay", "[%s]aos_ioctl fail, ret : %d\n", __func__, ret);

    return ret;
}

int32_t udisplay_disable_backlight(void)
{
    int32_t ret;
    udisplay_context_t *ctx = udisplay_get_context();

    ret = ioctl(ctx->fd, FBIODISABLE_BACKLIGHT, NULL);
    if (ret < 0)
        LOGE("udisplay", "[%s]aos_ioctl fail, ret : %d\n", __func__, ret);

    return ret;
}

