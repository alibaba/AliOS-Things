/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>

#include "aos/kernel.h"
#include "lvgl/lvgl.h"
#include <k_api.h>
#include "sensor_display.h"
#include "sensor/sensor.h"
#include "st7789.h"


#include "soc_init.h"

LV_IMG_DECLARE(AliOS_Things_logo);

#define MAX_MSG_BYTES 100
#define MAX_NUM_BYTES 20

char msg_buffer[MAX_MSG_BYTES];
char number_buf[MAX_NUM_BYTES];

/* gui object definition */
lv_obj_t *         scr    = NULL;
lv_obj_t *         chart1 = NULL;
lv_obj_t *         label1 = NULL;
lv_obj_t *         label2 = NULL;
lv_obj_t *         label3 = NULL;
lv_chart_series_t *dl1_1  = NULL;
lv_chart_series_t *dl1_2  = NULL;
lv_chart_series_t *dl1_3  = NULL;
aos_timer_t        refresh_timer;

/* acc sensor fd */
int fd_acc = -1;
/* als sensor fd */
static int fd_als = -1;

/* display driver */
lv_disp_drv_t dis_drv;

enum led_config
{
    LED_ON_LOW_DEFAULT = 0,
    LED_OFF_HIGH
};

static void  littlevgl_refresh_task(void *arg);
static void  app_init(void);
static void  lvgl_drv_register(void);
static void  my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                           const lv_color_t *color_p);
static void  my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                          lv_color_t color);
static void  my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                         const lv_color_t *color_p);
static void  sensor_refresh_task(void *arg);
static void  refresh_chart_label(void);
static int   get_acc_data(float *x, float *y, float *z);
static float acc_val_limit(float val);

/* LED1 gpio config */
static int als_led1_gpio_config(uint8_t led_config)
{
    int ret = 0;
    do {
        switch (led_config) {
            case LED_ON_LOW_DEFAULT:
                hal_gpio_output_low(&brd_gpio_table[GPIO_LED_1]);
                break;
            case LED_OFF_HIGH:
                hal_gpio_output_high(&brd_gpio_table[GPIO_LED_1]);
                break;
            default:
                hal_gpio_output_high(&brd_gpio_table[GPIO_LED_1]);
                break;
        }
    } while (0);
    return ret;
}

static int get_als_data(uint32_t *lux)
{
    als_data_t data = { 0 };
    ssize_t    size = 0;
    size            = aos_read(fd_als, &data, sizeof(data));
    if (size != sizeof(data)) {
        printf("aos_read return error.\n");
        return -1;
    }
    *lux = data.lux;
    return 0;
}

static void monitor_als_func(void)
{
    uint32_t lux = 0;
    get_als_data(&lux);
    if (lux <= 40) {
        als_led1_gpio_config(LED_ON_LOW_DEFAULT);
    } else {
        als_led1_gpio_config(LED_OFF_HIGH);
    }
}

void sensor_display_init(void)
{
    printf("application_start\n");

    /* init littlevGL */
    lv_init();

    /* init LCD */
    st7789_init();

    /* als led turn off as default */
    als_led1_gpio_config(LED_OFF_HIGH);

    /* register driver for littlevGL */
    lvgl_drv_register();

    /* create a task to refresh the LCD */
    aos_task_new("littlevgl_refresh_task", littlevgl_refresh_task, NULL, 8192);

    /* int app */
    app_init();
}

static void littlevgl_refresh_task(void *arg)
{
    while (1) {
        /* this function is used to refresh the LCD */
        lv_task_handler();

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 1);
    }
}

void app_init(void)
{
    char name[SENSOR_NAME_LEN];
    int index = 0;

    /* open acc sensor */
    index = 0;
    snprintf(name,SENSOR_NAME_LEN,"%s/%d",dev_acc_path,index);
    fd_acc = aos_open(name, O_RDWR);

    if (fd_acc < 0) {
        printf("acc sensor open failed !\n");
    }

    index = 0;
    snprintf(name,SENSOR_NAME_LEN,"%s/%d",dev_als_path,index);
    fd_als = aos_open(name, O_RDWR);
    if (fd_als < 0) {
        printf("als sensor open failed !\n");
    }

    /* create a timer to refresh sensor data */
    aos_timer_new(&refresh_timer, sensor_refresh_task, NULL, 1000, 1);
}

static void sensor_refresh_task(void *arg)
{
    static int        task1_count = 0;
    static lv_obj_t * img_src;
    static lv_style_t style1;
    static lv_style_t style2;
    static lv_style_t style3;

    /* disaply alios logo */
    if (task1_count == 0) {
        scr = lv_scr_act();

        img_src = lv_img_create(scr, NULL);          /*Crate an image object*/
        lv_img_set_src(img_src, &AliOS_Things_logo); /*Set the created file as
                                                        image (a red fl  ower)*/
        lv_obj_set_pos(img_src, 60, 60);             /*Set the positions*/
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
    monitor_als_func(); /* monitor env light */
    task1_count++;
}


static void refresh_chart_label(void)
{
    int        i       = 0;
    static int count   = 0;
    float      acc_x_f = 0;
    float      acc_y_f = 0;
    float      acc_z_f = 0;
    int        ret     = -1;

    /* get acc sensor data */
    ret = get_acc_data(&acc_x_f, &acc_y_f, &acc_z_f);
    if (ret != 0) {
        printf("acc sensor read failed !\n");
        return;
    }

    acc_x_f = acc_val_limit(acc_x_f);
    acc_y_f = acc_val_limit(acc_y_f);
    acc_z_f = acc_val_limit(acc_z_f);

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
    accel_data_t data = { 0 };
    ssize_t      size = 0;

    size = aos_read(fd_acc, &data, sizeof(data));
    if (size != sizeof(data)) {
        return -1;
    }

    *x = ((float)data.data[0] / 1000.0f) * 9.8f;
    *y = ((float)data.data[1] / 1000.0f) * 9.8f;
    *z = ((float)data.data[2] / 1000.0f) * 9.8f;

    return 0;
}

float acc_val_limit(float val)
{
    float ret = 0.0f;

    if (val > 80.0f) {
        ret = 80.0f;
    } else if (val < -20.0f) {
        ret = -20.0f;
    } else {
        ret = val;
    }

    return ret;
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
