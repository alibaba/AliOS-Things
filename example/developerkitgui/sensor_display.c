/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "lvgl/lvgl.h"
#include <k_api.h>
#include "sensor_display.h"
#include <hal/sensor.h>

LV_IMG_DECLARE(AliOS_Things_logo);

#define MAX_MSG_BYTES 100
#define MAX_NUM_BYTES 20

char msg_buffer[MAX_MSG_BYTES];
char number_buf[MAX_NUM_BYTES];

/* gui object definition */
lv_obj_t *scr = NULL;
lv_obj_t *chart1 = NULL;
lv_obj_t *label1 = NULL;
lv_obj_t *label2 = NULL;
lv_obj_t *label3 = NULL;
lv_chart_series_t *dl1_1 = NULL;
lv_chart_series_t *dl1_2 = NULL;
lv_chart_series_t *dl1_3 = NULL;
aos_timer_t refresh_timer;

/* acc sensor fd */
int fd_acc = -1;

static void sensor_refresh_task(void *arg);
static void refresh_chart_label(void);
static int get_acc_data(float *x, float *y, float *z);

void app_init(void)
{
    /* open acc sensor */
    fd_acc = aos_open(dev_acc_path, O_RDWR);

    if (fd_acc < 0) {
        printf("acc sensor open failed !\n");
    }

    /* create a timer to refresh sensor data */
    aos_timer_new(&refresh_timer, sensor_refresh_task, NULL, 300, 1);
}

static void sensor_refresh_task(void *arg)
{
    static int task1_count = 0;
    static lv_obj_t *img_src;
    static lv_style_t style1;
    static lv_style_t style2;
    static lv_style_t style3;

    /* disaply alios logo */
    if (task1_count == 0) {
        scr = lv_scr_act();

        img_src = lv_img_create(scr, NULL);  /*Crate an image object*/
        lv_img_set_src(img_src, &AliOS_Things_logo);  /*Set the created file as image (a red fl  ower)*/
        lv_obj_set_pos(img_src, 60, 60);      /*Set the positions*/
        lv_obj_set_drag(img_src, true);
    }

    /* hide alios logo and display sensor data */
    if (task1_count == 5) {
        lv_obj_del(img_src);

        /* create a label to display acc_x */
        label1 = lv_label_create(scr, NULL);
        lv_obj_set_pos(label1, 20, 20);
        lv_style_copy(&style1, &lv_style_plain);
        style1.text.color = LV_COLOR_BLUE;
        lv_label_set_style(label1, &style1);

        /* create a label to display acc_y */
        label2 = lv_label_create(scr, NULL);
        lv_obj_set_pos(label2, 20, 45);
        lv_style_copy(&style2, &lv_style_plain);
        style2.text.color = LV_COLOR_RED;
        lv_label_set_style(label2, &style2);

        /* create a label to display acc_z */
        label3 = lv_label_create(scr, NULL);
        lv_obj_set_pos(label3, 20, 70);
        lv_style_copy(&style3, &lv_style_plain);
        style3.text.color = LV_COLOR_BLACK;
        lv_label_set_style(label3, &style3);

        /* create a chart to display all data */
        chart1 = lv_chart_create(scr, NULL);
        lv_chart_set_type(chart1, LV_CHART_TYPE_LINE);
        lv_obj_set_size(chart1, 200, 120);
        lv_obj_set_pos(chart1, 20, 100);
        lv_chart_set_range(chart1, 0, 40);
        lv_chart_set_point_count(chart1, 30);

        /* add acc_x acc_y acc_z to the chart */
        dl1_1 = lv_chart_add_series(chart1, LV_COLOR_BLUE);
        dl1_2 = lv_chart_add_series(chart1, LV_COLOR_RED);
        dl1_3 = lv_chart_add_series(chart1, LV_COLOR_BLACK);
    }

    /* refresh chart and label data */
    if (task1_count >= 5) {
        if ((dl1_1 != NULL) || (dl1_1 != NULL) || (dl1_1 != NULL)) {
            refresh_chart_label();
        } else {
            printf("chart create failed !\n");
        }
    }

    printf("sensor_refresh_task\n");

    task1_count++;
}

static void refresh_chart_label(void)
{
    int i = 0;
    static int count = 0;
    float acc_x_f = 0;
    float acc_y_f = 0;
    float acc_z_f = 0;
    int ret = -1;

    /* get acc sensor data */
    ret = get_acc_data(&acc_x_f, &acc_y_f, &acc_z_f);
    if (ret != 0) {
        printf("acc sensor read failed !\n");
        return;
    }

    /* refresh chart data */
    if (count == 0) {
        /* to prevent sensor data from dithering at the first time */
        for (i = 0; i <= 29; i++) {
            dl1_1->points[i] = acc_x_f + 20;
            dl1_2->points[i] = acc_y_f + 20;
            dl1_3->points[i] = acc_z_f + 20;
        }
    } else {
        for (i = 28; i >= 0; i--) {
            dl1_1->points[i + 1] = dl1_1->points[i];
            dl1_2->points[i + 1] = dl1_2->points[i];
            dl1_3->points[i + 1] = dl1_3->points[i];
        }

        dl1_1->points[0] = acc_x_f + 20;
        dl1_2->points[0] = acc_y_f + 20;
        dl1_3->points[0] = acc_z_f + 20;
    }

    /* refresh chart */
    lv_chart_refresh(chart1);

    /* refresh label */
    sprintf(number_buf, "%.4f", acc_x_f);
    strcpy(msg_buffer, "acc_x is ");
    strcat(msg_buffer, number_buf);
    strcat(msg_buffer, " m/s2");

    lv_label_set_text(label1, msg_buffer);

    sprintf(number_buf, "%.4f", acc_y_f);
    strcpy(msg_buffer, "acc_y is ");
    strcat(msg_buffer, number_buf);
    strcat(msg_buffer, " m/s2");

    lv_label_set_text(label2, msg_buffer);

    sprintf(number_buf, "%.4f", acc_z_f);
    strcpy(msg_buffer, "acc_z is ");
    strcat(msg_buffer, number_buf);
    strcat(msg_buffer, " m/s2");

    lv_label_set_text(label3, msg_buffer);

    count++;
}

static int get_acc_data(float *x, float *y, float *z)
{
    accel_data_t data = {0};
    ssize_t size = 0;

    size = aos_read(fd_acc, &data, sizeof(data));
    if (size != sizeof(data)) {
        return -1;
    }

    *x = ((float)data.data[0] / 1000.0f) * 9.8f;
    *y = ((float)data.data[1] / 1000.0f) * 9.8f;
    *z = ((float)data.data[2] / 1000.0f) * 9.8f;

    return 0;
}
