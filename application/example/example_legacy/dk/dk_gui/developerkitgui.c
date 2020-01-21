/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "lvgl.h"
#include "aos/kernel.h"
#include "soc_init.h"
#include "sensor_display.h"
#include "freetype_display.h"

#define FREETYPE_DISPLAY 0

static void gui_init(void);
static void littlevgl_refresh_task(void *arg);
static void lvgl_disp_drv_init(void);
static void my_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

extern int freetype_init(void);

/* display driver */
lv_disp_drv_t disp_drv;

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

#if FREETYPE_DISPLAY == 1
    freetype_init();
#endif

    /* init LCD */
    st7789_init();

    /* register driver for littlevGL */
    lvgl_disp_drv_init();

    /* create a task to refresh the LCD */
    aos_task_new("littlevgl_refresh_task", littlevgl_refresh_task, NULL, 2048);

    /* start app */
#if FREETYPE_DISPLAY == 0
    sensor_display();
#else
    freetype_display();
#endif
}

static void littlevgl_refresh_task(void *arg)
{
    while (1) {

        printf("littlevgl_refresh_task !\n");

        /* this function is used to refresh the LCD */
        lv_task_handler();

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 10);
    }
}

void lvgl_disp_drv_init(void)
{
    static lv_disp_buf_t disp_buf_2;
    static lv_color_t buf2_1[LV_HOR_RES_MAX * 10];                        /*A buffer for 10 rows*/
    static lv_color_t buf2_2[LV_HOR_RES_MAX * 10];                        /*An other buffer for 10 rows*/
    lv_disp_buf_init(&disp_buf_2, buf2_1, buf2_2, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LittlevGL
     *----------------------------------*/

    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = my_disp_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &disp_buf_2;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

void my_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x = 0;
    int32_t y = 0;

    for (y = area->y1; y <= area->y2; y++) {
        ST7789H2_WriteLine(area->x1, y, (uint8_t *)color_p, (area->x2 - area->x1 + 1));
        color_p += (area->x2 - area->x1 + 1);
    }

    lv_disp_flush_ready(&disp_drv);
}
