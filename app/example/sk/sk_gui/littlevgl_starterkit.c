/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "lvgl.h"
#include <k_api.h>
#include "st7789.h"

LV_IMG_DECLARE(AliOS_Things_logo);

lv_disp_drv_t disp_drv;
lv_obj_t *scr = NULL;
aos_timer_t refresh_timer;

static void lvgl_disp_drv_init(void);
void my_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

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
        lv_label_set_style(label1, 0, &style);
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

    lvgl_disp_drv_init();

    aos_task_new("littlevgl_refresh_task", littlevgl_refresh_task, NULL, 2048);

    aos_timer_new(&refresh_timer, sensor_refresh_task, NULL, 50, 1);

    return 0;
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
