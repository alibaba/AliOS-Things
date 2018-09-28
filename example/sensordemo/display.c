/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <math.h>
#include <aos/aos.h>
#include "lvgl/lvgl.h"
#include <k_api.h>
#include <hal/sensor.h>
#include "display.h"
#include "drv/display/st7789h2/st7789h2.h"
#include "drv/display/stm32f413h_discovery_lcd.h"
#include "drv/display/stm32f413h_discovery_ts.h"


LV_IMG_DECLARE(AliOS_Things_logo);

#define MAX_MSG_BYTES 100
#define MAX_NUM_BYTES 20

char msg_buffer[MAX_MSG_BYTES];
char number_buf[MAX_NUM_BYTES];

#define SCREEN_ACTION_UP			1
#define SCREEN_ACTION_DOWN			2
#define SCREEN_ACTION_NEXT			3
#define SCREEN_ACTION_PREVIOUS		4

#define VIEW_LOUDNESS_BAR		0
#define VIEW_SOUND_LOCATION		1
#define VIEW_SENSOR_MONITOR		2
#define VIEW_PLAY_VOLUME		3

//#define AUDIO_LOOPBACK_TEST
#define AUDIO_LOCATION_TEST

/* gui object definition */
lv_obj_t *scr     = NULL;
lv_obj_t *img_src = NULL;

aos_timer_t refresh_timer;

int scr_clean_flag = 0;

/* display driver */
lv_disp_drv_t dis_drv;
lv_indev_drv_t indev_drv;
lv_indev_t *indev = NULL;

static void littlevgl_refresh_task(void *arg);
static void app_init(void);

static void logo_display(void);
static void fb_refresh_task(void *arg);

static void clean_screen(void);

static void lvgl_drv_register(void);
static bool ts_read(lv_indev_data_t * data);
static void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                          const lv_color_t *color_p);
static void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                         lv_color_t color);
static void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                        const lv_color_t *color_p);

static aos_mutex_t display_lock;

void display_init(void)
{
    /* init littlevGL */
    lv_init();

    /* init LCD */
    BSP_LCD_Init();
    BSP_TS_Init(240,240);

    /* register driver for littlevGL */
    lvgl_drv_register();

	aos_mutex_new(&display_lock);

    /* create a task to refresh the LCD */
    aos_task_new("littlevgl_refresh_task", littlevgl_refresh_task, NULL, 2048);

    /* int app */
    app_init();
}

static void littlevgl_refresh_task(void *arg)
{
    while (1) {
        /* this function is used to refresh the LCD */

		aos_mutex_lock(&display_lock, AOS_WAIT_FOREVER);
        lv_task_handler();
		aos_mutex_unlock(&display_lock);

        krhino_task_sleep(35);
    }
}

void app_init(void)
{
    indev = lv_indev_get_act();

    /* create a timer to refresh sensor data */
    aos_timer_new(&refresh_timer, fb_refresh_task, NULL, 40, 1);
}

int8_t mic_loudness_db = 76;
int8_t display_view_index;
extern uint8_t gesture_action;


#define LOUDNESS_BAR_NUM	22
static lv_obj_t *loudness_bar_obj[LOUDNESS_BAR_NUM];
static lv_style_t loudness_bar_style[LOUDNESS_BAR_NUM];
static lv_obj_t *loudness_bar_label1;
static lv_obj_t *loudness_bar_label2;
static lv_style_t loudness_bar_label_style;
static int loudness_bar_init(void)
{
	int i;

	for (i = 0; i < LOUDNESS_BAR_NUM; i++) {
		lv_style_copy(&loudness_bar_style[i], &lv_style_plain_color);
	    loudness_bar_style[i].body.main_color = LV_COLOR_MAKE(138, 142, 133);
	    loudness_bar_style[i].body.grad_color = loudness_bar_style[i].body.main_color;
		loudness_bar_obj[i] = lv_obj_create(scr, NULL);
		lv_obj_set_size(loudness_bar_obj[i], 55, 8);
		lv_obj_set_style(loudness_bar_obj[i], &loudness_bar_style[i]);
		lv_obj_align(loudness_bar_obj[i], NULL, LV_ALIGN_IN_TOP_LEFT, 40, 210 - i * 9);
	}

	loudness_bar_label1 = lv_label_create(scr, NULL);
	lv_label_set_text(loudness_bar_label1, " 0");
	lv_style_copy(&loudness_bar_label_style, &lv_style_pretty_color);
    loudness_bar_label_style.text.color = LV_COLOR_MAKE(142, 146, 138);
	lv_label_set_style(loudness_bar_label1, &loudness_bar_label_style);
	lv_obj_align(loudness_bar_label1, NULL, LV_ALIGN_IN_TOP_LEFT, 125, 190);

	loudness_bar_label2 = lv_label_create(scr, NULL);
	lv_label_set_text(loudness_bar_label2, "dB");
	lv_label_set_style(loudness_bar_label2, &loudness_bar_label_style);
	lv_obj_align(loudness_bar_label2, NULL, LV_ALIGN_IN_TOP_LEFT, 149, 190);

	return 0;
}

static int loudness_bar_refresh(int sel)
{
	int i;
	char label_text[8];
	unsigned int bar_bits = 0;
	static int loudness = 0;

	if (sel == loudness)
		return 0;

	if (sel > LOUDNESS_BAR_NUM)
		sel = LOUDNESS_BAR_NUM;

	snprintf(label_text, sizeof(label_text), "%2d", mic_loudness_db);
	for (i = 0; i < LOUDNESS_BAR_NUM; i++) {
		if (i < sel || i == 0) {
    		//loudness_bar_style[i].body.main_color = LV_COLOR_MAKE(60 + i * 6, 160 + i * 4, 16);
			loudness_bar_style[i].body.main_color = LV_COLOR_MAKE(60 + i * 7, 160 - i * 2, 10);
    		loudness_bar_style[i].body.grad_color = loudness_bar_style[i].body.main_color;
		} else {
    		loudness_bar_style[i].body.main_color = LV_COLOR_MAKE(138, 142, 133);
    		loudness_bar_style[i].body.grad_color = loudness_bar_style[i].body.main_color;
		}

		lv_obj_refresh_style(loudness_bar_obj[i]);
	}
	lv_label_set_text(loudness_bar_label1, (const char *)label_text);
	if (sel > 0)
		loudness_bar_label_style.text.color = loudness_bar_style[sel - 1].body.main_color;
	else
		loudness_bar_label_style.text.color = LV_COLOR_MAKE(60, 160, 10);
	lv_obj_refresh_style(loudness_bar_label2);

	loudness = sel;
	return 0;
}

static int loudness_bar_delete(void)
{
	int i;
	for (i = 0; i < LOUDNESS_BAR_NUM; i++)
		lv_obj_del(loudness_bar_obj[i]);
	lv_obj_del(loudness_bar_label1);
	lv_obj_del(loudness_bar_label2);
	return 0;
}

static int8_t led_bar_auto_index;
static int8_t led_bar_auto_state;
static int led_bar_autorefresh(int sel)
{
	uint32_t bits = 0;
	int i;

	for (i = 0; i < 10; i++) {
		if (i < sel)
			bits |= (1 << i);
	}

	led_bar_ctrl(bits);
	return 0;
}


int8_t sound_location;
static int8_t pre_sound_location = -1;

static lv_obj_t *sound_loc_obj[4];
static lv_style_t sound_loc_style[4];
static int sound_loc_init(void)
{
	int i;
	sound_location = -1;

	for (i = 0; i < 4; i++) {
		lv_style_copy(&sound_loc_style[i], &lv_style_pretty_color);
		sound_loc_style[i].body.radius = LV_RADIUS_CIRCLE;
	    sound_loc_style[i].body.main_color = LV_COLOR_MAKE(164, 211, 238);
	    sound_loc_style[i].body.grad_color = LV_COLOR_MAKE(164, 211, 238);
		sound_loc_style[i].body.border.color = LV_COLOR_MAKE(178, 234, 103);
		sound_loc_style[i].body.border.width = 0;
		sound_loc_style[i].body.shadow.color = LV_COLOR_MAKE(133, 215, 22);
		sound_loc_style[i].body.shadow.width = 0;
		sound_loc_obj[i] = lv_led_create(scr, NULL);
		lv_obj_set_size(sound_loc_obj[i], 45, 45);
		lv_obj_set_style(sound_loc_obj[i], &sound_loc_style[i]);
		if (i == 0)
			lv_obj_align(sound_loc_obj[i], NULL, LV_ALIGN_IN_TOP_LEFT, 45, 45);
		else if (i == 1)
			lv_obj_align(sound_loc_obj[i], NULL, LV_ALIGN_IN_TOP_LEFT, 150, 45);
		else if (i == 2)
			lv_obj_align(sound_loc_obj[i], NULL, LV_ALIGN_IN_TOP_LEFT, 45, 150);
		else
			lv_obj_align(sound_loc_obj[i], NULL, LV_ALIGN_IN_TOP_LEFT, 150, 150);
		lv_led_set_bright(sound_loc_obj[i], 255);
	}

	return 0;
}

static int sound_loc_refresh(int8_t sel)
{
	uint8_t i;

	if (sel == pre_sound_location)
		return 0;

	for (i = 0; i < 4; i++) {
		if (i == sel) {
			sound_loc_style[i].body.main_color = LV_COLOR_MAKE(161, 213, 91);
			sound_loc_style[i].body.grad_color = sound_loc_style[i].body.main_color;
			sound_loc_style[i].body.border.width = 1;
			sound_loc_style[i].body.shadow.width = 10;
		} else {
			sound_loc_style[i].body.main_color = LV_COLOR_MAKE(164, 211, 238);
			sound_loc_style[i].body.grad_color = sound_loc_style[i].body.main_color;
			sound_loc_style[i].body.shadow.width = 0;
			sound_loc_style[i].body.border.width = 0;
		}
		lv_obj_refresh_style(sound_loc_obj[i]);
	}

	pre_sound_location = sel;
	return 0;
}

static int sound_loc_delete(void)
{
	int i;
	for (i = 0; i < 4; i++)
		lv_obj_del(sound_loc_obj[i]);
	return 0;
}

static int fd_temp = -1;
static int fd_humi = -1;
static void hts221_get_temp(uint32_t *value)
{
	temperature_data_t data;

	memset(&data, 0, sizeof(temperature_data_t));
	if (fd_temp < 0) {
		fd_temp = aos_open(dev_temp_path, O_RDWR);
		if (fd_temp < 0)
			printf("%s: open %s failed !\n", __func__, dev_temp_path);
	} else {
		if (aos_read(fd_temp, &data, sizeof(data)) != sizeof(temperature_data_t))
			return;
		*value = data.t;
	}
}

static void hts221_get_humi(uint32_t *value)
{
	humidity_data_t data;

	memset(&data, 0, sizeof(humidity_data_t));
	if (fd_humi < 0) {
		fd_humi = aos_open(dev_humi_path, O_RDWR);
		if (fd_humi < 0)
			printf("%s: open %s failed !\n", __func__, dev_humi_path);
	} else {
		if (aos_read(fd_humi, &data, sizeof(data)) != sizeof(humidity_data_t))
			return;
		*value = data.h;
	}
}

static lv_obj_t *sensor_temp_obj;
static lv_obj_t *sensor_temp_label;
static lv_obj_t *sensor_temp_label2;
static lv_style_t sensor_temp_style;
static lv_style_t sensor_temp_label_style;

static lv_obj_t *sensor_humi_obj;
static lv_obj_t *sensor_humi_label;
static lv_obj_t *sensor_humi_label2;
static lv_style_t sensor_humi_style;
static lv_style_t sensor_humi_label_style;

static lv_style_t sensor_temp_humi_label2_style;

static int sensor_monitor_init(void)
{
	lv_style_copy(&sensor_temp_style, &lv_style_pretty_color);
	sensor_temp_style.line.width = 2;
	sensor_temp_style.line.color = LV_COLOR_SILVER;
	sensor_temp_style.body.main_color = LV_COLOR_MAKE(49, 211, 14);
	sensor_temp_style.body.grad_color = LV_COLOR_ORANGE;

	sensor_temp_obj = lv_lmeter_create(scr, NULL);
	lv_lmeter_set_scale(sensor_temp_obj, 240, 25);
	lv_lmeter_set_range(sensor_temp_obj, 0, 50);
	lv_lmeter_set_value(sensor_temp_obj, 20);
	lv_lmeter_set_style(sensor_temp_obj, &sensor_temp_style);
	lv_obj_set_size(sensor_temp_obj, 85, 85);
	lv_obj_align(sensor_temp_obj, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 60);

	lv_style_copy(&sensor_temp_label_style, &lv_style_pretty_color);
	sensor_temp_label_style.text.color = LV_COLOR_MAKE(49, 211, 14);

	lv_style_copy(&sensor_humi_label_style, &lv_style_pretty_color);
	sensor_humi_label_style.text.color = LV_COLOR_MAKE(49, 211, 14);

	sensor_temp_label = lv_label_create(sensor_temp_obj, NULL);
	lv_label_set_text(sensor_temp_label, "0");
	lv_label_set_style(sensor_temp_label, &sensor_temp_label_style);
	lv_obj_align(sensor_temp_label, NULL, LV_ALIGN_CENTER, -6, 0);

	lv_style_copy(&sensor_temp_humi_label2_style, &lv_style_pretty_color);
	sensor_temp_humi_label2_style.text.color = LV_COLOR_MAKE(33, 53, 234);
	sensor_temp_label2 = lv_label_create(scr, NULL);
	lv_label_set_text(sensor_temp_label2, "Temp");
	lv_label_set_style(sensor_temp_label2, &sensor_temp_humi_label2_style);
	lv_obj_align(sensor_temp_label2, NULL, LV_ALIGN_IN_TOP_LEFT, 37, 140);

	lv_style_copy(&sensor_humi_style, &lv_style_pretty_color);
	sensor_humi_style.line.width = 2;
	sensor_humi_style.line.color = LV_COLOR_SILVER;
	sensor_humi_style.body.main_color = LV_COLOR_MAKE(49, 211, 14);
	sensor_humi_style.body.grad_color = LV_COLOR_ORANGE;

	sensor_humi_obj = lv_lmeter_create(scr, NULL);
	lv_lmeter_set_scale(sensor_humi_obj, 240, 25);
	lv_lmeter_set_range(sensor_humi_obj, 0, 100);                   /*Set the range*/
	lv_lmeter_set_value(sensor_humi_obj, 30);                       /*Set the current value*/
	lv_lmeter_set_style(sensor_humi_obj, &sensor_humi_style);           /*Apply the new style*/
	lv_obj_set_size(sensor_humi_obj, 85, 85);
	lv_obj_align(sensor_humi_obj, NULL, LV_ALIGN_IN_TOP_LEFT, 140, 60);

	sensor_humi_label = lv_label_create(sensor_humi_obj, NULL);
	lv_label_set_text(sensor_humi_label, "0%");
	lv_label_set_style(sensor_humi_label, &sensor_humi_label_style);
	lv_obj_align(sensor_humi_label, NULL, LV_ALIGN_CENTER, -2, 0);

	sensor_humi_label2 = lv_label_create(scr, NULL);
	lv_label_set_text(sensor_humi_label2, "Humi");
	lv_label_set_style(sensor_humi_label2, &sensor_temp_humi_label2_style);
	lv_obj_align(sensor_humi_label2, NULL, LV_ALIGN_IN_TOP_LEFT, 163, 140);

	return 0;
}

static int sensor_monitor_refresh(void)
{
	char text[8];
	static uint8_t count = 0;
	uint32_t curr_humi = 0;
	uint32_t curr_temp = 0;
	static uint32_t humi = 0;
	static uint32_t temp = 0;
	static int8_t temp_warn = 0;
	static int8_t humi_warn = 0;
	if (count++ >= 25) {
		count = 0;
		hts221_get_humi(&curr_humi);
		hts221_get_temp(&curr_temp);
		drv_gesture_paj7620_reset();
		printf("%s: humi %u temp %u\n", __func__, curr_humi, curr_temp);
		temp = curr_temp;
		humi = curr_humi;
	}

	snprintf(text, sizeof(text), "%u.%u", temp/10, temp%10);
	lv_label_set_text(sensor_temp_label, text);
	lv_lmeter_set_value(sensor_temp_obj, temp/10);
	if (temp >= 300 && temp_warn == 0) {
		sensor_temp_label_style.text.color = LV_COLOR_MAKE(255, 0, 0);
		sensor_temp_style.body.main_color = LV_COLOR_MAKE(255, 0, 0);
		lv_obj_refresh_style(sensor_temp_obj);
		lv_obj_refresh_style(sensor_temp_label);
		temp_warn = 1;
	} else if (temp < 300 && temp_warn == 1) {
		temp_warn = 0;
		sensor_temp_label_style.text.color = LV_COLOR_MAKE(49, 211, 14);
		sensor_temp_style.body.main_color = LV_COLOR_MAKE(49, 211, 14);
		lv_obj_refresh_style(sensor_temp_obj);
		lv_obj_refresh_style(sensor_temp_label);
	}
		

	snprintf(text, sizeof(text), "%u%%", humi);
	lv_label_set_text(sensor_humi_label, text);
	lv_lmeter_set_value(sensor_humi_obj, humi);
	if (humi >= 80 && humi_warn == 0) {
		sensor_humi_label_style.text.color = LV_COLOR_MAKE(255, 0, 0);
		sensor_humi_style.body.main_color = LV_COLOR_MAKE(255, 0, 0);
		lv_obj_refresh_style(sensor_humi_obj);
		lv_obj_refresh_style(sensor_humi_label);
		lv_obj_refresh_style(sensor_humi_label2);
		humi_warn = 1;
	} else if (humi < 80 && humi_warn == 1) {
		humi_warn = 0;
		sensor_humi_label_style.text.color = LV_COLOR_MAKE(49, 211, 14);
		sensor_humi_style.body.main_color = LV_COLOR_MAKE(49, 211, 14);
		lv_obj_refresh_style(sensor_humi_obj);
		lv_obj_refresh_style(sensor_humi_label);
		lv_obj_refresh_style(sensor_humi_label2);
	}

	return 0;
}

static int sensor_monitor_delete(void)
{
	lv_obj_del(sensor_temp_obj);
	lv_obj_del(sensor_temp_label2);
	lv_obj_del(sensor_humi_obj);
	lv_obj_del(sensor_humi_label2);
}

static void fb_refresh_task(void *arg)
{
    static uint32_t task_count = 0;
	static uint8_t volume_bar_dirty = 3;

	aos_mutex_lock(&display_lock, AOS_WAIT_FOREVER);

    if (task_count == 0) {
        scr = lv_scr_act();
        logo_display();
		sound_loc_init();
		display_view_index = 1;
    }

	if (gesture_action == SCREEN_ACTION_NEXT) {
		if (display_view_index == 1) {
			sound_loc_delete();
			loudness_bar_init();
			display_view_index = 2;
		} else if (display_view_index == 2) {
			loudness_bar_delete();
			sensor_monitor_init();
			display_view_index = 3;
		}
		gesture_action = 0;
	} else if (gesture_action == SCREEN_ACTION_PREVIOUS) {
		if (display_view_index == 2) {
			loudness_bar_delete();
			sound_loc_init();
			display_view_index = 1;
		} else if (display_view_index == 3) {
			sensor_monitor_delete();
			loudness_bar_init();
			display_view_index = 2;
		}
		gesture_action = 0;
	} else if (gesture_action == SCREEN_ACTION_UP) {
		if (led_bar_auto_state == 2)
			led_bar_auto_state = 0;
		else
			led_bar_auto_state = 1;
		gesture_action = 0;
	} else if (gesture_action == SCREEN_ACTION_DOWN) {
		if (led_bar_auto_state == 1)
			led_bar_auto_state = 0;
		else
			led_bar_auto_state = 2;
		gesture_action = 0;
	}

	if (display_view_index == 1) {
		sound_loc_refresh(sound_location);
	} else if (display_view_index == 2) {
		loudness_bar_refresh((mic_loudness_db - 28)/2);
	} else if (display_view_index == 3) {
		sensor_monitor_refresh();
	}

	task_count++;

	if (task_count % 3 == 0) {
		if (led_bar_auto_state == 1) {
			if (led_bar_auto_index == 10) {
				led_bar_auto_state = 0;
			} else {
				led_bar_auto_index++;
				led_bar_autorefresh(led_bar_auto_index);
				if (led_bar_auto_index == 10)
					led_bar_auto_state = 0;
			}
		} else if (led_bar_auto_state == 2) {
			if (led_bar_auto_index == 0) {
				led_bar_auto_state = 0;
			} else {
				led_bar_auto_index--;
				led_bar_autorefresh(led_bar_auto_index);
				if (led_bar_auto_index == 0)
					led_bar_auto_state = 0;
			}
		}
	}

	aos_mutex_unlock(&display_lock);
}

static lv_res_t btn_click_action(lv_obj_t *scr)
{
    scr_clean_flag = 1;
    printf("btn_click_action\n");
}

static void logo_display(void)
{
    img_src = lv_img_create(scr, NULL); /*Crate an image object*/
    lv_img_set_src(img_src, &AliOS_Things_logo);
    lv_obj_set_pos(img_src, 0, 0); /*Set the positions*/
    lv_obj_set_drag(img_src, true);
}

static void clean_screen(void)
{
    lv_obj_clean(scr);
}

void lvgl_drv_register(void)
{
    lv_disp_drv_init(&dis_drv);

    dis_drv.disp_flush = my_disp_flush;
    dis_drv.disp_fill  = my_disp_fill;
    dis_drv.disp_map   = my_disp_map;
    lv_disp_drv_register(&dis_drv);

    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read = ts_read;         
    lv_indev_drv_register(&indev_drv);
}

bool ts_read(lv_indev_data_t * data)
{
    TS_StateTypeDef TS_State;

    BSP_TS_GetState(&TS_State);

    /*Store the collected data*/
    data->point.x = TS_State.touchX[0];
    data->point.y = TS_State.touchY[0];
    data->state = TS_State.touchDetected ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    return false;
}

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                   const lv_color_t *color_p)
{
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t *pRGB = (uint16_t *)color_p;

    for (y = y1; y <= y2; y++) {
        ST7789H2_WriteLine(x1, y, (x2 - x1 + 1), pRGB);
        pRGB += (x2 - x1 + 1);
    }

    lv_flush_ready();
}

void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                  lv_color_t color)
{
    uint16_t x = 0;
    uint16_t y = 0;

    for (y = y1; y <= y2; y++) {
        for (x = x1; x <= x2; x++) {
            BSP_LCD_DrawPixel(x,y,(uint16_t)color.full);
        }
    }
}

void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                 const lv_color_t *color_p)
{
    int32_t x = 0;
    int32_t y = 0;
    uint16_t *pRGB = (uint16_t *)color_p;

    for (y = y1; y <= y2; y++) {
        ST7789H2_WriteLine(x1, y, (x2 - x1 + 1), pRGB);
        color_p += (x2 - x1 + 1);
    }
}

