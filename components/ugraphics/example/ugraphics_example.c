/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include "ugraphics.h"

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define TAG "ugraphics_example"
#define LOG printf

#define WIDTH  SCREEN_W
#define HEIGHT SCREEN_H

static void ugraphics_comp_example(int argc, char **argv)
{
    int ret;

    if (argc < 2)
        LOGE(TAG, "wrong parameter number\n");

    if (!strncmp(argv[1], "init", 4)) {
        /*Initialize ugraphics window*/
        ret = ugraphics_init(WIDTH, HEIGHT);
        if (ret < 0) {
            LOGE(TAG, "ugraphics init fail, ret: %d\n", ret);
            return;
        }
        LOG("ugraphics init ok!\n");
    } else if (!strncmp(argv[1], "draw", 4)) {
        /*Set background color*/
        ret = ugraphics_set_color(COLOR_BLACK);
        if (ret < 0) {
            LOGE(TAG, "ugraphics set color fail, ret: %d\n", ret);
            return;
        }
        /*Draw background rectangle*/
        ret = ugraphics_fill_rect(0, 0, WIDTH, HEIGHT);
        if (ret < 0) {
            LOGE(TAG, "ugraphics fill rect fail, ret: %d\n", ret);
            return;
        }
        if (!strncmp(argv[2], "rect", 4)) {
            /*Set rect boarder color*/
            ret = ugraphics_set_color(COLOR_RED);
            if (ret < 0) {
                LOGE(TAG, "ugraphics set color fail, ret: %d\n", ret);
                return;
            }

            /*Draw empty rectangle*/
            ret = ugraphics_draw_rect(0, 0, WIDTH / 2, HEIGHT / 2);
            if (ret < 0) {
                LOGE(TAG, "ugraphics draw rect fail, ret: %d\n", ret);
                return;
            }
            LOG("ugraphics draw rectangle ok!\n");
        } else if (!strncmp(argv[2], "line", 4)) {
            /*Set line color*/
            ret = ugraphics_set_color(COLOR_RED);
            if (ret < 0) {
                LOGE(TAG, "ugraphics set color fail, ret: %d\n", ret);
                return;
            }

            /*Draw line*/
            ret = ugraphics_draw_line(0, HEIGHT / 4, WIDTH, HEIGHT / 4);
            if (ret < 0) {
                LOGE(TAG, "ugraphics draw line fail, ret: %d\n", ret);
                return;
            }

            LOG("ugraphics draw line ok!\n");
        } else if (!strncmp(argv[2], "jpg", 3)) {
            /*Draw image*/
            ret = ugraphics_draw_image("/data/ugraphics_image/object.jpg", 0, 0);
            if (ret < 0) {
                LOGE(TAG, "ugraphics draw jpg image fail, ret: %d\n", ret);
                return;
            }
            LOG("ugraphics draw jpg ok!\n");
        } else if (!strncmp(argv[2], "png", 3)) {
            /*Draw image*/
            ret = ugraphics_draw_image("/data/ugraphics_image/anime.png", 0, 0);
            if (ret < 0) {
                LOGE(TAG, "ugraphics draw png image fail, ret: %d\n", ret);
                return;
            }
            LOG("ugraphics draw png ok!\n");
        } else if (!strncmp(argv[2], "string", 6)) {
            /*Set string color*/
            ret = ugraphics_set_color(COLOR_RED);
            if (ret < 0) {
                LOGE(TAG, "ugraphics set color fail, ret: %d\n", ret);
                return;
            }

            /*Load default ttf font*/
            ret = ugraphics_load_font("/data/font/Alibaba-PuHuiTi-Heavy.ttf", 18);
            if (ret < 0) {
                LOGE(TAG, "ugraphics load font fail, ret: %d\n", ret);
                return;
            }

            /*Set font style*/
            ugraphics_set_font_style(UGRAPHICS_FONT_STYLE);

            /*Draw string*/
            ugraphics_draw_string("Welcome to AliOS Things!", WIDTH / 8, HEIGHT / 2);
            LOG("ugraphics draw string ok!\n");
        }

        /*Show graphics on screen*/
        ugraphics_flip();
    } else if (!strncmp(argv[1], "fill", 4)) {
        /*Set background color*/
        ret = ugraphics_set_color(COLOR_BLACK);
        if (ret < 0) {
            LOGE(TAG, "ugraphics set color fail, ret: %d\n", ret);
            return;
        }
        /*Draw background rectangle*/
        ret = ugraphics_fill_rect(0, 0, WIDTH, HEIGHT);
        if (ret < 0) {
            LOGE(TAG, "ugraphics fill rect fail, ret: %d\n", ret);
            return;
        }
        if (!strncmp(argv[2], "rect", 4)) {
            /*Set background color*/
            ret = ugraphics_set_color(COLOR_RED);
            if (ret < 0) {
                LOGE(TAG, "ugraphics set color fail, ret: %d\n", ret);
                return;
            }

            /*Fill full rectangle*/
            ret = ugraphics_fill_rect(0, 0, WIDTH / 2, HEIGHT / 2);
            if (ret < 0) {
                LOGE(TAG, "ugraphics fill rect fail, ret: %d\n", ret);
                return;
            }
            LOG("ugraphics fill rectangle ok!\n");
        } else {
            LOG("unkown command\n");
        }

        /*Show graphics on screen*/
        ugraphics_flip();
    } else if (!strncmp(argv[1], "clear", 5)) {
        /*Clear buffer on screen*/
        ret = ugraphics_clear();
        if (ret < 0) {
            LOGE(TAG, "ugraphics clear fail, ret: %d\n", ret);
            return;
        }

        /*Set background color*/
        ret = ugraphics_set_color(COLOR_BLACK);
        if (ret < 0) {
            LOGE(TAG, "ugraphics set color fail, ret: %d\n", ret);
            return;
        }

        /*Draw background rectangle*/
        ret = ugraphics_fill_rect(0, 0, WIDTH, HEIGHT);
        if (ret < 0) {
            LOGE(TAG, "ugraphics fill rect fail, ret: %d\n", ret);
            return;
        }

        /*Show graphics on screen*/
        ugraphics_flip();
        LOG("ugraphics clear screen ok!\n");
    } else if (!strncmp(argv[1], "quit", 4)) {
        /*Quit ugraphics component*/
        ugraphics_quit();
        LOG("ugraphics quit ok!\n");
    }

    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ugraphics_comp_example, ugraphics, ugraphics component base example)
#endif
