//
// Created by liujuncheng on 2022/1/18.
//

#include <stdbool.h>
#include <stdlib.h>

#include "../../../lvgl.h"

static int last_monkey_x = 0;
static int last_monkey_y = 0;

static bool press = false;

static void monkey_touch_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    (void)indev_drv; /*Unused*/

    if (press) {
        press = false;
    } else {
        last_monkey_x = rand() % 320;
        last_monkey_y = rand() % 240;
        press = true;
    }
    /*Store the collected data*/
    data->point.x = last_monkey_x;
    data->point.y = last_monkey_y;
    data->state = press ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void lv_monkey_init() {
    static lv_indev_drv_t indev_mouse_drv;
    lv_indev_drv_init(&indev_mouse_drv);
    indev_mouse_drv.type = LV_INDEV_TYPE_POINTER;
    indev_mouse_drv.read_cb = monkey_touch_read;
    lv_indev_drv_register(&indev_mouse_drv);
}
