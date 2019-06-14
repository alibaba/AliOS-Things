/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "lvgl/lvgl.h"
#include "aos/kernel.h"
#include "soc_init.h"
#include "sensor_display.h"
#include "freetype_display.h"

#define FREETYPE_DISPLAY 0

static void gui_init(void);
static void littlevgl_refresh_task(void *arg);
static void lvgl_drv_register(void);
static void lvgl_drv_register(void);
static void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
static void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
static void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);

/* display driver */
lv_disp_drv_t dis_drv;

int application_start(int argc, char *argv[])
{
    printf("application_start\n");

    gui_init();

    return 0;
}

void gui_init(void)
{
    /* init littlevGL */
    lv_init();

    /* init LCD */
    st7789_init();

    /* register driver for littlevGL */
    lvgl_drv_register();

    /* create a task to refresh the LCD */
    aos_task_new("littlevgl_refresh_task", littlevgl_refresh_task, NULL, 2048);

    /* int app */
#if FREETYPE_DISPLAY == 0
    sensor_display();
#else
    freetype_display();
#endif
}

static void littlevgl_refresh_task(void *arg)
{
    while (1) {
        /* this function is used to refresh the LCD */
        lv_task_handler();

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 10);
    }
}


void lvgl_drv_register(void)
{
    lv_disp_drv_init(&dis_drv);

    dis_drv.disp_flush = my_disp_flush;
    dis_drv.disp_fill  = my_disp_fill;
    dis_drv.disp_map   = my_disp_map;
    lv_disp_drv_register(&dis_drv);
}

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                   const lv_color_t *color_p)
{
    int32_t x = 0;
    int32_t y = 0;

    for (y = y1; y <= y2; y++) {
        ST7789H2_WriteLine(x1, y, (uint8_t *)color_p, (x2 - x1 + 1));
        color_p += (x2 - x1 + 1);
    }

    lv_flush_ready();
}

void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                  lv_color_t color)
{
    int32_t i = 0;
    int32_t j = 0;

    for (i = x1; i <= x2; i++) {
        for (j = y1; j <= y2; j++) {
            ST7789H2_WritePixel(i, j, color.full);
        }
    }
}

void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                 const lv_color_t *color_p)
{
    int32_t x = 0;
    int32_t y = 0;

    for (y = y1; y <= y2; y++) {
        ST7789H2_WriteLine(x1, y, (int16_t *)color_p, (x2 - x1 + 1));
        color_p += (x2 - x1 + 1);
    }
}
