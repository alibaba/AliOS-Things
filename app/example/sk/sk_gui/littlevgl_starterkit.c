/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "lvgl/lvgl.h"
#include <k_api.h>
#include "st7789.h"

LV_IMG_DECLARE(AliOS_Things_logo);

lv_disp_drv_t dis_drv;
lv_obj_t *scr = NULL;
aos_timer_t refresh_timer;

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);

static void littlevgl_refresh_task(void *arg)
{
    while (1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        printf("littlevgl_refresh_task\n");
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 100);
    }
}

static void sensor_refresh_task(void *arg)
{
    static int task1_count = 0;
    static lv_obj_t *img_src;
    static lv_obj_t *label1;
    static lv_style_t style;

    if (task1_count == 0) {

        scr = lv_scr_act();

        img_src = lv_img_create(scr, NULL);  /*Crate an image object*/
        lv_img_set_src(img_src, &AliOS_Things_logo);  /*Set the created file as image (a red fl  ower)*/
        lv_obj_set_pos(img_src, 60, 60);      /*Set the positions*/
        lv_obj_set_drag(img_src, true);

        label1 = lv_label_create(scr, NULL);
        lv_style_copy(&style, &lv_style_plain);
        style.text.color = LV_COLOR_PURPLE;
        lv_label_set_style(label1, &style);
        lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_pos(label1, 65, 200);
    }

    task1_count++;
}

int application_start(int argc, char *argv[])
{
    printf("application_start1\n");

    /*Initialize LittlevGL*/
    lv_init();

    st7789_init();

    lvgl_drv_register();

    aos_task_new("littlevgl_refresh_task", littlevgl_refresh_task, NULL, 2048);

    aos_timer_new(&refresh_timer, sensor_refresh_task, NULL, 50, 1);

    return 0;
}

void lvgl_drv_register(void)
{
    lv_disp_drv_init(&dis_drv);

    dis_drv.disp_flush = my_disp_flush;
    dis_drv.disp_fill = my_disp_fill;
    dis_drv.disp_map = my_disp_map;
    lv_disp_drv_register(&dis_drv);
}

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    int32_t x = 0;
    int32_t y = 0;

    for (y = y1; y <= y2; y++) {                        /*Pick the next row*/
        for (x = x1; x <= x2; x++) {                   /*Pick the a pixel in the row*/
            ST7789H2_WritePixel(x, y, color_p->full);
            color_p++;
        }
    }

    lv_flush_ready();
}

void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    int32_t i = 0;
    int32_t j = 0;

    for (i = x1; i <= x2; i++) {
        for (j = y1; j <= y2; j++) {
            ST7789H2_WritePixel(i, j, color.full);
        }
    }
}

void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    int32_t i = 0;
    int32_t j = 0;

    for (i = x1; i <= x2; i++) {
        for (j = y1; j <= y2; j++) {
            ST7789H2_WritePixel(i, j, color_p->full);
            color_p++;
        }
    }
}
