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
#if MICROPY_PY_LVGL
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "axp192.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

#include "esp_task.h"
#include "py/runtime.h"
#include "py/stackctrl.h"

#if MICROPY_PY_THREAD

#include "py/mpthread.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif
#endif

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

#include "moddisplay.h"

#define TAG "demo"
#define LV_TICK_PERIOD_MS 1
#define I2C_0 i2c_hal(0)

static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void drawDisplay();
#if 0
void vPortCleanUpTCB(void *tcb) {
}
#endif

#if MICROPY_PY_TFT
extern const mp_obj_type_t display_tft_type;
#endif

SemaphoreHandle_t xGuiSemaphore;
void driver_init()
{
    // axp192_init(I2C_0);
    // axp192_ioctl(I2C_0, AXP192_GPIO0_SET_LEVEL, AXP192_HIGH);  // M-Bus Power
    // axp192_ioctl(I2C_0, AXP192_GPIO1_SET_LEVEL, AXP192_LOW);   // Enable LED
    // axp192_ioctl(I2C_0, AXP192_GPIO2_SET_LEVEL, AXP192_LOW);   // Disable speaker
    // axp192_ioctl(I2C_0, AXP192_LDO2_SET_VOLTAGE, 3300);        // Set LDO2 LCD&TP voltage
    // axp192_ioctl(I2C_0, AXP192_LDO2_ENABLE);                   // Enable LDO2
    // axp192_ioctl(I2C_0, AXP192_GPIO4_SET_LEVEL, AXP192_LOW);   // LCD&TP Reset
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    // axp192_ioctl(I2C_0, AXP192_GPIO4_SET_LEVEL, AXP192_HIGH);  // LCD&TP Hold
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    lvgl_driver_init();
    start_display();
}
typedef void (*mp_task_entry_t)(void *arg);
struct mp_task_struct {
    mp_task_entry_t entry;
    size_t stack_size;
    void *arg;
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;
};

void mp_thread_entry_c(void *arg)
{
    struct mp_task_struct *task_struct = (struct mp_task_struct *)arg;
    mp_state_thread_t ts;
    mp_thread_set_state(&ts);
    mp_task_entry_t entry = task_struct->entry;
    void *task_param = task_struct->arg;
    mp_stack_set_top(&ts + 1); // need to include ts in root-pointer scan
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

int mp_thread_create_c(TaskFunction_t pxTaskCode,
                    const char *const pcName,
                    const uint32_t usStackDepth,
                    void *const pvParameters,
                    UBaseType_t uxPriority,
                    TaskHandle_t *const pxCreatedTask,
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
}

void start_display()
{
   //ESP_LOGE(TAG, "start_display");
    mp_thread_create_c(guiTask, "gui", 4096 * 10, NULL, ESP_TASK_PRIO_MIN + 1, NULL, 0);
}

STATIC mp_obj_t mp_lv_task_handler(mp_obj_t arg)
{
    lv_task_handler();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_lv_task_handler_obj, mp_lv_task_handler);

static void guiTask(void *pvParameter)
{
    (void) pvParameter;
    //ESP_LOGE(TAG, "gui task entern");
    xGuiSemaphore = xSemaphoreCreateMutex();
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
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
STATIC mp_obj_t app_display_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
//void app_main()
{
    driver_init();
    drawDisplay();
}

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static void drawDisplay()
{

    lv_init();

    //lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_color_t *buf1 = (lv_color_t *)malloc(DISP_BUF_SIZE*sizeof(lv_color_t));
    assert(buf1 != NULL);

    //lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_color_t *buf2 = (lv_color_t *)malloc(DISP_BUF_SIZE*sizeof(lv_color_t));
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

    lv_obj_t * obj;
    obj = lv_btn_create(lv_scr_act());
    lv_obj_set_size(obj, 250, 150);
    obj = lv_label_create(obj);
    lv_label_set_text(obj, "lvgltest\n");
}

static void lv_tick_task(void *arg)
{
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void display_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{
    // printf("display_flush area: %d %d %d %d\n", area->x1, area->y1, area->x2, area->y2);
    disp_driver_flush(drv,area,color_map);
}

static void touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
   // ESP_LOGE(TAG, "touch_read");
    touch_driver_read(drv, data);
}

STATIC mp_obj_t app_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    ESP_LOGE(TAG, "app_init");
    driver_init();
    return mp_obj_new_int(0);
}

STATIC mp_obj_t app_draw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    ESP_LOGE(TAG, "app_draw");
    drawDisplay();
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_display_init_obj, 0, app_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_display_draw_obj, 0, app_draw);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_display_all_obj, 0, app_display_all);

DEFINE_PTR_OBJ(display_flush);
DEFINE_PTR_OBJ(touch_read);

STATIC const mp_rom_map_elem_t display_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_display) },
        { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_display_init_obj) },
        { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&PTR_OBJ(display_flush))},
        { MP_ROM_QSTR(MP_QSTR_touch_read), MP_ROM_PTR(&PTR_OBJ(touch_read))},
        //for test start
        { MP_ROM_QSTR(MP_QSTR_draw), MP_ROM_PTR(&mp_display_draw_obj)},
        { MP_ROM_QSTR(MP_QSTR_test), MP_ROM_PTR(&mp_display_all_obj)},
        { MP_OBJ_NEW_QSTR(MP_QSTR_TFT), MP_ROM_PTR(&display_tft_type)},
        //for test end
};
         

STATIC MP_DEFINE_CONST_DICT(mp_module_display_globals, display_globals_table);

const mp_obj_module_t mp_module_display = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_display_globals
};
#endif
