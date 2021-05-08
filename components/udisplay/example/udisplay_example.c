/*
 * Copyright (C) 2015-2020 manufacturername
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aos/kernel.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <ulog/ulog.h>

#include "udisplay.h"
#include "udisplay_cli.h"


static void set_brightness(char *pbuffer, int outlen, int argc, char **argv)
{
    int32_t brightness;

    brightness = atoi(argv[1]);

    LOGD("udisplay", "set brightness to %d\n", brightness);
    udisplay_set_brightness(brightness);
}

static void get_brightness(char *pbuffer, int outlen, int argc, char **argv)
{
    int32_t brightness;

    brightness = udisplay_get_brightness();

    LOGD("udisplay", "brightness is %d\n", brightness);
}

static void enable_backlight(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;

    ret = udisplay_enable_backlight();

    if (ret < 0)
        LOGD("udisplay", "enable backlight fail\n");
    else
        LOGD("udisplay", "enable backlight ok\n");
}

static void disable_backlight(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;

    ret = udisplay_disable_backlight();

    if (ret < 0)
        LOGD("udisplay", "disable backlight fail\n");
    else
        LOGD("udisplay", "disable backlight ok\n");
}

static char *dump_rotate_str(uint32_t id)
{
    const char *str = NULL;

    switch (id) {
        case FB_ROTATE_UR:
            str = "FB_ROTATE_UR";
            break;
        case FB_ROTATE_UD:
            str = "FB_ROTATE_UD";
            break;
        case FB_ROTATE_CW:
            str = "FB_ROTATE_CW";
            break;
        case FB_ROTATE_CCW:
            str = "FB_ROTATE_CCW";
            break;
    }
    return str;
}

static void udisplay_comp_example(int argc, char **argv)
{
    int32_t ret;
    uint32_t value = 0;
    uint32_t rgb_fmt;
    bool performance = false;
    long long start, end;

    if (argc < 2)
        return;

    start = aos_now_ms();
    if (strncmp(argv[1], "pattern", 7) == 0) {
        rgb_fmt = strtoul(argv[2], NULL, 0);
        value = strtoul(argv[3], NULL, 0);
        if (rgb_fmt == 32)
            udisplay_pattern_rgb32(value, true);
        else if (rgb_fmt == 16)
            udisplay_pattern_rgb16(value, false);
    } else if (strncmp(argv[1], "info", 7) == 0) {
        udisplay_context_t *ctx = udisplay_get_context();
        LOGD("udisplay", \
             "[fb info] xres: %d, yres: %d, bits_per_pixel: %d, rotate: %s", \
             ctx->var.xres, ctx->var.yres, \
             ctx->var.bits_per_pixel, \
             dump_rotate_str(ctx->var.rotate));

    } else if (strncmp(argv[1], "stress_ud", 9) == 0) {
        // normal
        int i = 0;
        rgb_fmt = strtoul(argv[2], NULL, 0);
        value = strtoul(argv[3], NULL, 0);
        LOGD("udisplay", "stress test count=%d\n", value);
        if (rgb_fmt == 32) {
            for (i = 0; i < value; i++)
                udisplay_pattern_rgb32((i * 16) % 0xff, false);

        } else if (rgb_fmt == 16) {
            for (i = 0; i < value; i++) {
                udisplay_pattern_rgb16(0x0000ffff, false);
                aos_msleep(10);
                udisplay_pattern_rgb16((i * 16) % 0xff, false);
                aos_msleep(10);
                udisplay_pattern_rgb16(0xffff0000, false);
            }
        }
        performance = true;
        LOGD("udisplay", "stress test finish count=%d\n", i);
    } else if (strncmp(argv[1], "stress_cw", 9) == 0) {
        // rotate
        int i = 0;
        rgb_fmt = strtoul(argv[2], NULL, 0);
        value = strtoul(argv[3], NULL, 0);
        LOGD("udisplay", "stress test count=%d\n", value);
        if (rgb_fmt == 32) {
            for (i = 0; i < value; i++)
                udisplay_pattern_rgb32((i * 16) % 0xff, true);

        } else if (rgb_fmt == 16) {
            for (i = 0; i < value; i++)
                udisplay_pattern_rgb16((i * 16) % 0xff, true);

        }
        performance = true;
        LOGD("udisplay", \
             "stress test with rotate finish count=%d\n", i);
    }
    if (performance) {
        end = aos_now_ms();
        LOGD("udisplay", \
             "[%s]time gap: %lld ms for %d counts, fps: %2.0f\r\n", \
             __func__, end - start, value, \
             value * 1.f / (end - start) * 1000.f);
    }
}

static void udisplay_comp_init_example(int argc, char **argv)
{
    // init udisplay
    udisplay_init();
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
// display pattern 32/16 0x0000ffff 1/0(rotate); display stress_ud/stress_cw <count>; display info
ALIOS_CLI_CMD_REGISTER(udisplay_comp_init_example, udisplay_init, udisplay component base example)
ALIOS_CLI_CMD_REGISTER(udisplay_comp_example, udisplay, udisplay component base example)
#endif
