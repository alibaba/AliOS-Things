/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "lvgl/lvgl.h"
#include <k_api.h>
#include "sensor_display.h"

LV_IMG_DECLARE(AliOS_Things_logo);

#define MAX_MSG_BYTES 100
#define MAX_NUM_BYTES 20

char msg_buffer[MAX_MSG_BYTES];
char number_buf[MAX_NUM_BYTES];

lv_obj_t *scr = NULL;
lv_obj_t *chart1 = NULL;
lv_obj_t *label = NULL;
lv_obj_t *labe2 = NULL;
lv_obj_t *labe3 = NULL;
lv_chart_series_t *dl1_1 = NULL;
lv_chart_series_t *dl1_2 = NULL;
lv_chart_series_t *dl1_3 = NULL;
aos_timer_t refresh_timer;

static void sensor_refresh_task(void *arg);
static void refresh_chart(void);

void app_init(void)
{
    aos_timer_new(&refresh_timer, sensor_refresh_task, NULL, 50, 1);
}

static void sensor_refresh_task(void *arg)
{
    static int task1_count = 0;
    static lv_obj_t *img_src;

    if (task1_count == 0) {
        scr = lv_scr_act();

        img_src = lv_img_create(scr, NULL);  /*Crate an image object*/
        lv_img_set_src(img_src, &AliOS_Things_logo);  /*Set the created file as image (a red fl  ower)*/
        lv_obj_set_pos(img_src, 60, 60);      /*Set the positions*/
        lv_obj_set_drag(img_src, true);
    }

    if (task1_count == 30) {

        lv_obj_del(img_src);

        /*Use ASCII and Unicode letters*/
        static lv_style_t style1;
        static lv_style_t style2;
        static lv_style_t style3;

        label = lv_label_create(scr, NULL);
        lv_obj_set_pos(label, 20, 20);
        lv_style_copy(&style1, &lv_style_plain);
        style1.text.color = LV_COLOR_BLUE;
        lv_label_set_style(label, &style1);

        labe2 = lv_label_create(scr, NULL);
        lv_obj_set_pos(labe2, 20, 45);
        lv_style_copy(&style2, &lv_style_plain);
        style2.text.color = LV_COLOR_RED;
        lv_label_set_style(labe2, &style2);

        labe3 = lv_label_create(scr, NULL);
        lv_obj_set_pos(labe3, 20, 70);
        lv_style_copy(&style3, &lv_style_plain);
        style3.text.color = LV_COLOR_PURPLE;
        lv_label_set_style(labe3, &style3);

        /* Create a default object*/
        chart1 = lv_chart_create(scr, NULL);
        lv_chart_set_type(chart1, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE);
        lv_obj_set_size(chart1, 200, 120);
        lv_obj_set_pos(chart1, 20, 100);
        lv_chart_set_range(chart1, -100, 100);
        lv_chart_set_point_count(chart1, 30);

        dl1_1 = lv_chart_add_series(chart1, LV_COLOR_BLUE);
        dl1_2 = lv_chart_add_series(chart1, LV_COLOR_RED);
        dl1_3 = lv_chart_add_series(chart1, LV_COLOR_PURPLE);
    }

    if (task1_count >= 30) {
        if ((dl1_1 != NULL) || (dl1_1 != NULL) || (dl1_1 != NULL)) {
            refresh_chart();
        } else {
            lv_label_set_text(label, "chart create failed !\n");
        }
    }

    printf("refresh_task\n");

    task1_count++;
}

static void refresh_chart(void)
{
    int i = 0;
    float arg = 0.0f;
    static float count = 0;

    /* refresh chart */
    for (i = 0; i < 30; i++) {
        arg = ((float)i) / 3.1415926f + count / 10;
        dl1_1->points[i] = 10 * sin(arg);
        dl1_2->points[i] = 10 * sin(arg) + 40;
        dl1_3->points[i] = 10 * sin(arg) - 40;
    }

    lv_chart_refresh(chart1);

    /* refresh chart */
    sprintf(number_buf, "%.4f", sin(arg));
    strcpy(msg_buffer, "acc_x is ");
    strcat(msg_buffer, number_buf);
    strcat(msg_buffer, " m/s2");

    lv_label_set_text(label, msg_buffer);

    sprintf(number_buf, "%.4f", (sin(arg) + 100));
    strcpy(msg_buffer, "acc_y is ");
    strcat(msg_buffer, number_buf);
    strcat(msg_buffer, " m/s2");

    lv_label_set_text(labe2, msg_buffer);

    sprintf(number_buf, "%.4f", (sin(arg) + 200));
    strcpy(msg_buffer, "acc_z is ");
    strcat(msg_buffer, number_buf);
    strcat(msg_buffer, " m/s2");

    lv_label_set_text(labe3, msg_buffer);

    count++;
}
