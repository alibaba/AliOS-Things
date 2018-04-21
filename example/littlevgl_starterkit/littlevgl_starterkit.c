/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "lvgl/lvgl.h"

lv_disp_drv_t dis_drv;

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);
void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);

extern void ST7789H2_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);
extern int st7789_init(void);

static void app_delayed_action(void *arg)
{
    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);

    while(1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        krhino_task_sleep(10);

    }

    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);

    /*Initialize LittlevGL*/
    lv_init();

    st7789_init();

    lvgl_drv_register();

    aos_loop_run();

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

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
   int32_t x = 0;
   int32_t y = 0;

   for (y = y1; y <= y2; y++)                          /*Pick the next row*/
   {
        for (x = x1; x <= x2; x++)                     /*Pick the a pixel in the row*/
        {
            ST7789H2_WritePixel(x,y, color_p->full);
            color_p++;
        }
   }

   lv_flush_ready();
}

void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    int32_t i =0;
    int32_t j =0;
    
    for (i = x1; i <= x2; i++)
    {
        for (j = y1; j <= y2; j++)
        {
            ST7789H2_WritePixel(i,j, color.full);
        }
    }
}

void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    int32_t i =0;
    int32_t j =0;
    
    for (i = x1; i <= x2; i++)
    {
        for (j = y1; j <= y2; j++)
        {
            ST7789H2_WritePixel(i,j, color_p->full);
            color_p++;
        }
    }
}
