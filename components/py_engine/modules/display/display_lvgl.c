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
#if MICROPY_PY_DISPLAY && MICROPY_PY_DISPLAY_LVGL

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "axp192.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lvgl_i2c/i2c_manager.h"
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

#include "lvgl_helpers.h"
#include "moddisplay.h"

#define TAG               "disp_lvgl"
#define LV_TICK_PERIOD_MS 1
#define I2C_0             i2c_hal(0)

static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void drawDisplay();

SemaphoreHandle_t xGuiSemaphore;
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

int mp_thread_create_c(TaskFunction_t pxTaskCode, const char *const pcName, const uint32_t usStackDepth,
                       void *const pvParameters, UBaseType_t uxPriority, TaskHandle_t *const pxCreatedTask,
                       const BaseType_t xCoreID)
{
    struct mp_task_struct *task_struct;

    task_struct = (struct mp_task_struct *)malloc(sizeof(struct mp_task_struct));

    memset(task_struct, 0, sizeof(struct mp_task_struct));
    task_struct->entry = pxTaskCode;
    task_struct->arg = pvParameters;
    task_struct->dict_locals = mp_locals_get();
    task_struct->dict_globals = mp_globals_get();
    task_struct->stack_size = usStackDepth - 1024;
    xTaskCreatePinnedToCore(mp_thread_entry_c, pcName, usStackDepth, task_struct, uxPriority, pxCreatedTask, xCoreID);

    return 0;
}

STATIC void display_lvgl_printinfo(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "LVGL   (Color mode: 16-bit, Clk=40000000 Hz)\n");
    mp_printf(print, "LVGL  (miso=38, mosi=23, clk=18, cs=5, dc=15, reset by axp192)");
}

STATIC mp_obj_t display_lvgl_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    display_lvgl_obj_t *self = m_new0(display_lvgl_obj_t, 1);
    self->base.type = &display_lvgl_type;
    return MP_OBJ_FROM_PTR(self);
}

void start_display()
{
    mp_thread_create_c(guiTask, "gui", 4096 * 10, NULL, ESP_TASK_PRIO_MIN + 1, NULL, 0);
}

void driver_init()
{
    lvgl_driver_init();
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
    // ESP_LOGE(TAG, "gui task entern");
    xGuiSemaphore = xSemaphoreCreateMutex();
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = { .callback = &lv_tick_task, .name = "periodic_gui" };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            mp_sched_schedule((mp_obj_t)&mp_lv_task_handler_obj, mp_const_none);
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    vTaskDelete(NULL);
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
    disp_drv.flush_cb = disp_driver_flush;

    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    ESP_LOGE(TAG, "register touch driver");
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
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
    // printf("display_flush area: %d %d %d %d\n", area->x1, area->y1, area->x2, area->y2);
    disp_driver_flush(drv, area, color_map);
}
DEFINE_PTR_OBJ(display_flush);

static void touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    // ESP_LOGE(TAG, "touch_read");
    touch_driver_read(drv, data);
}
DEFINE_PTR_OBJ(touch_read);

STATIC mp_obj_t app_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    ESP_LOGE(TAG, "app_init");
    driver_init();
    drawDisplay();
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_display_init_obj, 0, app_init);

STATIC mp_obj_t app_draw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    ESP_LOGE(TAG, "app_draw");
    lv_init();
    drawDisplay();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_display_draw_obj, 0, app_draw);

STATIC const mp_rom_map_elem_t display_lvgl_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_display) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_display_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&PTR_OBJ(display_flush)) },
    { MP_ROM_QSTR(MP_QSTR_touch_read), MP_ROM_PTR(&PTR_OBJ(touch_read)) },
    // for test start
    { MP_ROM_QSTR(MP_QSTR_draw), MP_ROM_PTR(&mp_display_draw_obj) },
    { MP_ROM_QSTR(MP_QSTR_monkey_test), MP_ROM_PTR(&mp_monkey_test) },
    // for test end
};
STATIC MP_DEFINE_CONST_DICT(display_lvgl_locals_dict, display_lvgl_locals_dict_table);

const mp_obj_type_t display_lvgl_type = {
    { &mp_type_type },
    .name = MP_QSTR_LVGL,
    .print = display_lvgl_printinfo,
    .make_new = display_lvgl_make_new,
    .locals_dict = (mp_obj_t)&display_lvgl_locals_dict,
};

#endif  // MICROPY_PY_DISPLAY_LVGL
