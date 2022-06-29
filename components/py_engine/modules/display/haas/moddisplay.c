/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stackctrl.h"

#if MICROPY_PY_THREAD
#include "py/mpthread.h"
#endif

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "moddisplay.h"
#include "ft6336.h"

#define LV_TICK_PERIOD_MS 1
#define DISP_BUF_SIZE  (240 * 320)
static bool lvgl_display_init = false;
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void drawDisplay();

aos_mutex_t mutex_handle;
aos_timer_t timer_tick;
static void display_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);
static void touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
extern void st7789_frame_draw(uint8_t *frame);
extern void st7789_rect_draw(uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height,
        uint16_t color);
extern void st7789_local_refresh(uint16_t x1,
        uint16_t y1,
        uint16_t x2,
        uint16_t y2,
        uint8_t *frame);
extern uint8_t st7789_init(void);
typedef void (*mp_task_entry_t)(void *arg);
struct mp_task_struct {
    mp_task_entry_t entry;
    size_t stack_size;
    void *arg;
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;
};

typedef struct _display_lvgl_obj_t {
    mp_obj_base_t base;
} display_lvgl_obj_t;

const mp_obj_type_t display_lvgl_type;

void mp_thread_entry_c(void *arg)
{
    struct mp_task_struct *task_struct = (struct mp_task_struct *)arg;
    mp_state_thread_t ts;
    mp_thread_set_state(&ts);
    mp_task_entry_t entry = task_struct->entry;
    void *task_param = task_struct->arg;
    mp_stack_set_top(&ts + 1);  // need to include ts in root-pointer scan
    mp_stack_set_limit(task_struct->stack_size);

#if MICROPY_ENABLE_PYSTACK
    // TODO threading and pystack is not fully supported, for now just make a small stack
    mp_obj_t mini_pystack[128];
    mp_pystack_init(mini_pystack, &mini_pystack[128]);
#endif

    ts.mp_pending_exception = MP_OBJ_NULL;
    // set locals and globals from the calling context
    mp_locals_set(task_struct->dict_locals);
    mp_globals_set(task_struct->dict_globals);
    // signal that we are set up and running
    entry(task_param);
}

int mp_thread_create_c(mp_task_entry_t pxTaskCode, const char *const pcName, const uint32_t usStackDepth,
                       void *const pvParameters, const uint32_t uxPriority)
{
    struct mp_task_struct *task_struct;

    printf("entern mp_thread_create_c\r\n");
    task_struct = (struct mp_task_struct *)malloc(sizeof(struct mp_task_struct));

    memset(task_struct, 0, sizeof(struct mp_task_struct));
    task_struct->entry = pxTaskCode;
    task_struct->arg = pvParameters;
    task_struct->dict_locals = mp_locals_get();
    task_struct->dict_globals = mp_globals_get();
    task_struct->stack_size = usStackDepth - 1024;
    mp_thread_create(mp_thread_entry_c, task_struct, &usStackDepth);
    mp_thread_start();
    printf("out mp_thread_create_c\r\n");

    return 0;
}

void start_display()
{
    printf("start_display\n");
    mp_thread_create_c(guiTask, "gui", 4 * 1024, NULL, AOS_DEFAULT_APP_PRI);
    printf("out_display\n");
}

void driver_init()
{
    FT6336_init();//touch driver init
    st7789_init(); //driver init
    start_display();
}

STATIC mp_obj_t mp_lv_task_handler(mp_obj_t arg)
{
    lv_task_handler();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_lv_task_handler_obj, mp_lv_task_handler);

static void guiTask(void *pvParameter)
{
    (void)pvParameter;
    printf("gui task entern\r\n");
    aos_status_t status;
    printf("mutex create\n");
    status = aos_mutex_create(&mutex_handle, 0);
    if (status != 0) {
        printf("[%s]create mutex error\n", __FUNCTION__);
        return ;
    }
    printf("timer create\n");
    //aos_status_t status;
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    status = aos_timer_create(&timer_tick, lv_tick_task, NULL, (LV_TICK_PERIOD_MS), AOS_TIMER_REPEAT);
    if (status != 0) {
        printf("[%s]create timer error\n", __FUNCTION__);
        return;
    }
    aos_timer_start(&timer_tick);

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        aos_msleep(30);

        /* Try to take the semaphore, call lvgl related function on success */
        status = aos_mutex_lock(&mutex_handle, AOS_WAIT_FOREVER);
        if (status == 0) {
            callback_to_python_LoBo((mp_obj_t)&mp_lv_task_handler_obj, mp_const_none, NULL);
            aos_mutex_unlock(&mutex_handle);
        }
    }

    vTaskDelete(NULL);
    aos_mutex_free(&mutex_handle);
}
/**********************
 *   APPLICATION MAIN
 **********************/
STATIC mp_obj_t app_monkey_test(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    lv_monkey_init();

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_monkey_test, 0, app_monkey_test);

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

static void drawDisplay()
{
    // lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_color_t *buf1 = (lv_color_t *)malloc(DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf1 != NULL);

    // lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_color_t *buf2 = (lv_color_t *)malloc(DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf2 != NULL);

    uint32_t size_in_px = DISP_BUF_SIZE;

    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = display_flush;

    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    printf("register touch driver\r\n");
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
}

static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void display_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    printf("display_flush area: %d %d %d %d ;%p\n", area->x1, area->y1, area->x2, area->y2, color_map);
    //st7789_frame_draw((uint8_t*)color_map);
    uint16_t x = area->x1;
    uint16_t y = area->y1;
    uint16_t w = area->x2 - area->x1 + 1;
    uint16_t h = area->y2 - area->y1 + 1;
    //printf("display_flush area: %d %d %d %d ;%p\n", x, y, w, h, (uint16_t*)color_map);
    //st7789_rect_draw(x, y, w, h, (uint16_t*)color_map);
    //st7789_frame_draw((uint8_t*)color_map);
    st7789_local_refresh(area->x1, area->y1, area->x2, area->y2,(uint8_t*)color_map);
    lv_disp_flush_ready(drv);
}
DEFINE_PTR_OBJ(display_flush);

static void touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    //printf("touch_read\n");
    //touch_driver_read(drv, data);
    if (data == NULL)
        return;

    TouchPointEvent event;
    event.status = -1;
    event.x = 0;
    event.y = 0;
    event = FT6336_read_point();
    //data->continue_reading = false;
    if (event.status == 0)
    {
        data->point.x = event.x;
        data->point.y = event.y;
        data->state = LV_INDEV_STATE_REL;
    }
    else if (event.status == 1)
    {
        data->point.x = event.x;
        data->point.y = event.y;
        data->state = LV_INDEV_STATE_PR;
    }
    //printf("touch_read x = %d;y = %d;state= %d;\n", data->point.x,data->point.y,data->state);
}
DEFINE_PTR_OBJ(touch_read);

STATIC mp_obj_t is_initialized(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    bool _res = (bool)lvgl_display_init;
    printf("check init = %d;_res = %d;\r\n", lvgl_display_init, _res);
    return _res ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_is_initialized_obj, 0, is_initialized);

STATIC mp_obj_t app_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    printf("app_init\r\n");
    lvgl_display_init = true;
    driver_init();
    drawDisplay();
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_display_init_obj, 0, app_init);

STATIC mp_obj_t app_draw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    printf("app_draw\r\n");
    lv_init();
    drawDisplay();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_display_draw_obj, 0, app_draw);

STATIC const mp_rom_map_elem_t display_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_lvgl_display) },
    { MP_ROM_QSTR(MP_QSTR_is_initialized), MP_ROM_PTR(&mp_is_initialized_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_display_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&PTR_OBJ(display_flush)) },
    { MP_ROM_QSTR(MP_QSTR_touch_read), MP_ROM_PTR(&PTR_OBJ(touch_read)) },
    // for test start
    { MP_ROM_QSTR(MP_QSTR_draw), MP_ROM_PTR(&mp_display_draw_obj) },
    { MP_ROM_QSTR(MP_QSTR_monkey_test), MP_ROM_PTR(&mp_monkey_test) },
    // for test end
    };
STATIC MP_DEFINE_CONST_DICT(mp_module_display_globals, display_globals_table);

const mp_obj_module_t mp_module_lvgl_display = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_display_globals
};
MP_REGISTER_MODULE(MP_QSTR_lvgl_display, mp_module_lvgl_display, MICROPY_PY_LVGL);
