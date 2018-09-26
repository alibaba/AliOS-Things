/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "lvgl/lvgl.h"
#include <k_api.h>
#include "chat_display.h"
#include <hal/sensor.h>
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

lv_chart_series_t *dl1_1  = NULL;
lv_chart_series_t *dl1_2  = NULL;

aos_timer_t        refresh_timer;


/* acc sensor fd */
static int fd_acc = -1;
/* als sensor fd */
static int fd_als = -1;
/* humi sensor fd */
static int fd_humi = -1;
/* temp sensor fd */
static int fd_temp = -1;

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
static void subscribe_refresh_task(void *arg);
static void  refresh_chart_label(void);
static int   get_acc_data(float *x, float *y, float *z);
static int   get_humi_data(uint32_t *humi);
static int   get_temp_data(uint32_t *temp);
static float acc_val_limit(float val);

/* LED1 gpio config */
static int als_led1_gpio_config(uint8_t led_config)
{
    do {
        switch (led_config) {
            case LED_ON_LOW_DEFAULT:
                hal_gpio_output_low(&brd_gpio_table[GPIO_ALS_LED]);
                break;
            case LED_OFF_HIGH:
                hal_gpio_output_high(&brd_gpio_table[GPIO_ALS_LED]);
                break;
            default:
                hal_gpio_output_high(&brd_gpio_table[GPIO_ALS_LED]);
                break;
        }
    } while (0);
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

extern void user_pub(char *usr_payload);
static void monitor_als_func(void)
{
    uint32_t lux = 0;
    get_als_data(&lux);
    if (lux <= 40) {
        als_led1_gpio_config(LED_ON_LOW_DEFAULT);
        user_pub("LED ON-Line!");
    } else {
        als_led1_gpio_config(LED_OFF_HIGH);
        user_pub("LED is off-line!");
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
    /* open acc sensor */
    fd_acc = aos_open(dev_acc_path, O_RDWR);

    if (fd_acc < 0) {
        printf("acc sensor open failed !\n");
    }

    fd_als = aos_open(dev_als_path, O_RDWR);
    if (fd_als < 0) {
        printf("als sensor open failed !\n");
    }

	fd_humi = aos_open(dev_humi_path, O_RDWR);
    if (fd_humi < 0) {
        printf("humi sensor open failed !\n");
    }

	fd_temp = aos_open(dev_temp_path, O_RDWR);
    if (fd_temp < 0) {
        printf("temp sensor open failed !\n");
    }

    /* create a timer to refresh sensor data */
    aos_timer_new(&refresh_timer, subscribe_refresh_task, NULL, 1000, 1);
}

extern int g_mqtt_enabled_flag;

static void subscribe_refresh_task(void *arg)
{
    static int        task1_count = 0;
    static lv_obj_t * img_src;
    static lv_style_t style1;
    static lv_style_t style2;
    static lv_style_t style3;
    if (g_mqtt_enabled_flag == 0)
		return;
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
    if (task1_count == 8) {
        lv_obj_del(img_src);

        /* create a label to display acc_x */
        label1 = lv_label_create(scr, NULL);
        lv_obj_set_pos(label1, 20, 20);
        lv_style_copy(&style1, &lv_style_plain);
        style1.text.color = LV_COLOR_BLUE;
        lv_label_set_style(label1, &style1);
    }
    /* refresh chart and label data */
    if (task1_count >= 8)
      refresh_chart_label();
	monitor_als_func(); /* monitor env light */
	task1_count++;
}

extern char str_output[256];
void refresh_chart_label(void)
{
    int        i       = 0;
    int        ret     = -1;
    char str[] = "Hello World";

    /* refresh label */
    sprintf(number_buf, "%s", str_output);
    strcpy(msg_buffer, "Msg: ");
    strcat(msg_buffer, number_buf);
    lv_label_set_text(label1, msg_buffer);
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
