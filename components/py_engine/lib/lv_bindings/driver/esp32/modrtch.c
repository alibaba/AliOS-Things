
//////////////////////////////////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////////////////////////////////

// Uncomment the following line to see logs!
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include "../include/common.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_task.h"
#include "lvgl/src/hal/lv_hal_indev.h"
#include "lvgl/src/core/lv_disp.h"

#include "esp_log.h"
#include "soc/adc_channel.h"

//////////////////////////////////////////////////////////////////////////////
// Constants 
//////////////////////////////////////////////////////////////////////////////

static const char TAG[] = "[RTCH]";

#define INVALID_MEASUREMENT INT32_MIN

#define RTCH_TASK_STACK_SIZE (4*1024)
#define RTCH_TASK_PRIORITY (ESP_TASK_PRIO_MIN + 2)

#ifndef RTCH_MAX_TOUCH_SAMPLES
#define RTCH_MAX_TOUCH_SAMPLES 16
#endif

#ifndef RTCH_SAMPLE_WAIT_MS
#define RTCH_SAMPLE_WAIT_MS 1
#endif

#ifndef RTCH_TOUCH_WAIT_MS
#define RTCH_TOUCH_WAIT_MS 10
#endif

#ifndef RTCH_INIT_ADC_WAIT_MS
#define RTCH_INIT_ADC_WAIT_MS 20
#endif

#ifndef CONCAT3
#define _CONCAT3(a,b,c) a ## b ## c
#define CONCAT3(a,b,c) _CONCAT3(a,b,c)
#endif

#define GPIO_TO_ADC_ELEMENT(x) [x] = CONCAT3(ADC1_GPIO, x, _CHANNEL)
static const int gpio_to_adc[] = {
        GPIO_TO_ADC_ELEMENT(36),
        GPIO_TO_ADC_ELEMENT(37),
        GPIO_TO_ADC_ELEMENT(38),
        GPIO_TO_ADC_ELEMENT(39),
        GPIO_TO_ADC_ELEMENT(32),
        GPIO_TO_ADC_ELEMENT(33),
        GPIO_TO_ADC_ELEMENT(34),
        GPIO_TO_ADC_ELEMENT(35),
};

//////////////////////////////////////////////////////////////////////////////
// Module definition
//////////////////////////////////////////////////////////////////////////////

typedef struct _rtch_info_t
{
    int x;
    int y;
    bool touched;
} rtch_info_t;

typedef struct _rtch_t
{
    mp_obj_base_t base;

    gpio_num_t xp;  // X+
    gpio_num_t yp;  // Y+
    gpio_num_t xm;  // X-
    gpio_num_t ym;  // Y-
    gpio_num_t touch_rail; // pfet! 0 to enable.
    gpio_num_t touch_sense; // Probably Y+ or Y-, when touch rail is enabled

    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t cal_x0, cal_y0;
    uint32_t cal_x1, cal_y1;
    uint32_t touch_samples; // number of samples to take on every touch measurement
    uint32_t touch_samples_threshold; // max distance between touch sample measurements for a valid touch reading

    rtch_info_t rtch_info;
    xTaskHandle rtch_task_handle;
    SemaphoreHandle_t rtch_info_mutex;
} rtch_t;


// Unfortunately, lvgl doesn't pass user_data to callbacks, so we use this global.
// This means we can have only one active touch driver instance, pointed by this global.
STATIC rtch_t *g_rtch = NULL;

STATIC bool touch_read(lv_indev_drv_t * indev_drv, lv_indev_data_t *data)
{
    rtch_info_t *touch_info = &g_rtch->rtch_info;
    xSemaphoreTake(g_rtch->rtch_info_mutex, portMAX_DELAY);
    data->point = (lv_point_t){touch_info->x, touch_info->y};
    data->state = touch_info->touched? LV_INDEV_STATE_PRESSED: LV_INDEV_STATE_RELEASED;
    xSemaphoreGive(g_rtch->rtch_info_mutex);
    return false;
}

STATIC mp_obj_t mp_activate_rtch(mp_obj_t self_in)
{
    rtch_t *self = MP_OBJ_TO_PTR(self_in);
    g_rtch = self;
    return mp_const_none;
}

STATIC mp_obj_t rtch_make_new(const mp_obj_type_t *type,
                                 size_t n_args,
                                 size_t n_kw,
                                 const mp_obj_t *all_args);

STATIC mp_obj_t mp_rtch_init(mp_obj_t self_in);
STATIC mp_obj_t mp_rtch_deinit(mp_obj_t self_in);
STATIC mp_obj_t calibrate(mp_uint_t n_args, const mp_obj_t *args);

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_init_rtch_obj, mp_rtch_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_deinit_rtch_obj, mp_rtch_deinit);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_activate_rtch_obj, mp_activate_rtch);
DEFINE_PTR_OBJ(touch_read);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(calibrate_obj, 5, 5, calibrate);

STATIC const mp_rom_map_elem_t rtch_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_init_rtch_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_deinit_rtch_obj) },
    { MP_ROM_QSTR(MP_QSTR_activate), MP_ROM_PTR(&mp_activate_rtch_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&PTR_OBJ(touch_read)) },
    { MP_ROM_QSTR(MP_QSTR_calibrate), MP_ROM_PTR(&calibrate_obj) },
};

STATIC MP_DEFINE_CONST_DICT(rtch_locals_dict, rtch_locals_dict_table);

STATIC const mp_obj_type_t rtch_type = {
    { &mp_type_type },
    .name = MP_QSTR_rtch,
    //.print = rtch_print,
    .make_new = rtch_make_new,
    .locals_dict = (mp_obj_dict_t*)&rtch_locals_dict,
};

STATIC const mp_rom_map_elem_t rtch_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rtch) },
        { MP_ROM_QSTR(MP_QSTR_touch), (mp_obj_t)&rtch_type},
};
         

STATIC MP_DEFINE_CONST_DICT (
    mp_module_rtch_globals,
    rtch_globals_table
);

const mp_obj_module_t mp_module_rtch = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_rtch_globals
};


STATIC mp_obj_t rtch_make_new(const mp_obj_type_t *type,
                                 size_t n_args,
                                 size_t n_kw,
                                 const mp_obj_t *all_args)
{
    enum{
        ARG_xp,  // X+
        ARG_yp,  // Y+
        ARG_xm,  // X-
        ARG_ym,  // Y-
        ARG_touch_rail, // pfet! 0 to enable.
        ARG_touch_sense, // Probably Y+ or Y-, when touch rail is enabled

        ARG_screen_width,
        ARG_screen_height,
        ARG_cal_x0,
        ARG_cal_y0,
        ARG_cal_x1,
        ARG_cal_y1,
        ARG_touch_samples, // number of samples to take on every touch measurement
        ARG_touch_samples_threshold, // max distance between touch sample measurements for a valid touch reading
     };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_xp, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},  
        { MP_QSTR_yp, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},  
        { MP_QSTR_xm, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},  
        { MP_QSTR_ym, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},  
        { MP_QSTR_touch_rail, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}}, 
        { MP_QSTR_touch_sense, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}}, 

        { MP_QSTR_screen_width, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
        { MP_QSTR_screen_height, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
        { MP_QSTR_cal_x0, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=400}},
        { MP_QSTR_cal_y0, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=200}},
        { MP_QSTR_cal_x1, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=3500}},
        { MP_QSTR_cal_y1, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=3470}},
        { MP_QSTR_touch_samples, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=9}}, 
        { MP_QSTR_touch_samples_threshold, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=500}}, 
   };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    rtch_t *self = m_new_obj(rtch_t);
    self->base.type = type;
    self->xp = args[ARG_xp].u_int;
    self->yp = args[ARG_yp].u_int;
    self->xm = args[ARG_xm].u_int;
    self->ym = args[ARG_ym].u_int;
    self->touch_rail = args[ARG_touch_rail].u_int;
    self->touch_sense = args[ARG_touch_sense].u_int;

    self->screen_width = args[ARG_screen_width].u_int;
    if (self->screen_width == -1) self->screen_width = LV_HOR_RES;
    self->screen_height = args[ARG_screen_height].u_int;
    if (self->screen_height == -1) self->screen_height = LV_VER_RES;
    self->cal_x0 = args[ARG_cal_x0].u_int;
    self->cal_y0 = args[ARG_cal_y0].u_int;
    self->cal_x1 = args[ARG_cal_x1].u_int;
    self->cal_y1 = args[ARG_cal_y1].u_int;
    self->touch_samples = args[ARG_touch_samples].u_int;
    self->touch_samples_threshold = args[ARG_touch_samples_threshold].u_int;

    self->rtch_info = (rtch_info_t){0};
    self->rtch_task_handle = NULL;
    self->rtch_info_mutex = NULL;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t calibrate(mp_uint_t n_args, const mp_obj_t *args)
{
    (void)n_args; // unused, we know it's 5
    rtch_t *self = MP_OBJ_TO_PTR(args[0]);
    self->cal_x0 = mp_obj_get_int(args[1]);
    self->cal_y0 = mp_obj_get_int(args[2]);
    self->cal_x1 = mp_obj_get_int(args[3]);
    self->cal_y1 = mp_obj_get_int(args[4]);
    return mp_const_none;
}

//////////////////////////////////////////////////////////////////////////////
// RTCH implemenation
//////////////////////////////////////////////////////////////////////////////


STATIC void rtch_task(void* arg);

STATIC void enable_touch_sense(rtch_t *self)
{
    // Configure all touch pins to high impedance (input)

    gpio_config(&(gpio_config_t){
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask =
            (1ULL<<self->xp) |
            (1ULL<<self->yp) |
            (1ULL<<self->xm) |
            (1ULL<<self->ym)
    });

    // Enable touch rail

    gpio_config(&(gpio_config_t){
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<self->touch_rail),
    });
    gpio_set_level(self->touch_rail, 0);

    // Configure touch sense and configure interrupt

    gpio_config(&(gpio_config_t){
        .intr_type = GPIO_PIN_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = 1,
        .pin_bit_mask = (1ULL<<self->touch_sense),
    });

    // Wait for touch rail to stabilize

    vTaskDelay(RTCH_TOUCH_WAIT_MS / portTICK_RATE_MS);
}

STATIC mp_obj_t mp_rtch_init(mp_obj_t self_in)
{
    rtch_t *self = MP_OBJ_TO_PTR(self_in);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    esp_log_level_set("gpio", ESP_LOG_DEBUG);
    esp_log_level_set("RTC_MODULE", ESP_LOG_DEBUG);

    self->rtch_info_mutex = xSemaphoreCreateMutex();

    adc_power_on();
    enable_touch_sense(self);

    // Install Interrupt

    gpio_install_isr_service(0);

    mp_activate_rtch(self_in);

    BaseType_t xReturned = xTaskCreate(rtch_task, "RTCH Task", RTCH_TASK_STACK_SIZE, self, RTCH_TASK_PRIORITY, &self->rtch_task_handle);
    if (xReturned != pdPASS){
        ESP_LOGE(TAG, "Failed createing RTCH task!");
        vTaskDelete(self->rtch_task_handle);
        nlr_raise(
                mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed creating RTCH task")));
    }

    ESP_LOGD(TAG, "RTCH Initialized");

    return mp_const_none;
}

STATIC mp_obj_t mp_rtch_deinit(mp_obj_t self_in)
{
    rtch_t *self = MP_OBJ_TO_PTR(self_in);
    vTaskDelete(self->rtch_task_handle);
    adc_power_off();
    gpio_isr_handler_remove(self->touch_sense);

    // Configure all touch pins to high impedance (input)

    gpio_config(&(gpio_config_t){
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask =
            (1ULL<<self->xp) |
            (1ULL<<self->yp) |
            (1ULL<<self->xm) |
            (1ULL<<self->ym) | 
            (1ULL<<self->touch_rail) |
            (1ULL<<self->touch_sense)
    });

    return mp_const_none;
}

STATIC int compare_int(const void *_a, const void *_b)
{
    int *a = (int*)_a;
    int *b = (int*)_b;
    return *a - *b;
}

STATIC int measure_axis(
        rtch_t *self,
        gpio_num_t plus,
        gpio_num_t minus,
        gpio_num_t measure,
        gpio_num_t ignore)
{
    // Set GPIOs:

    // - Disable touch rail
    gpio_set_level(self->touch_rail, 1);

    // - Float "ignore" and "measure"
    gpio_pad_select_gpio(ignore);
    gpio_set_direction(ignore, GPIO_MODE_DISABLE);
    gpio_set_pull_mode(ignore, GPIO_FLOATING);

    gpio_pad_select_gpio(measure);
    gpio_set_direction(measure, GPIO_MODE_DISABLE);
    gpio_set_pull_mode(measure, GPIO_FLOATING);

    // - Set "plus" to 1, "minus" to 0
    gpio_config(&(gpio_config_t){
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<plus) | (1ULL<<minus)
    });
    gpio_set_level(plus, 1);
    gpio_set_level(minus, 0);

    // Init ADC

    adc1_channel_t adc_channel = gpio_to_adc[measure];

    adc_gpio_init(ADC_UNIT_1, adc_channel);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(adc_channel,ADC_ATTEN_DB_11);

    vTaskDelay(RTCH_INIT_ADC_WAIT_MS / portTICK_RATE_MS);

    // Collect ADC samples and sort them

    static int samples[RTCH_MAX_TOUCH_SAMPLES];
    int sample_count = self->touch_samples;
    for (int i=0; i<sample_count; i++)
    {
        //vTaskDelay(RTCH_SAMPLE_WAIT_MS / portTICK_RATE_MS);
        samples[i] = adc1_get_raw(adc_channel);
    }
    qsort(samples, sample_count, sizeof(samples[0]), compare_int);

    // Make sure samples are close to each other

    int prevSample = INVALID_MEASUREMENT;
    for (int i=0; i<sample_count; i++)
    {
        //ESP_LOGD(TAG, "RAW Sample %d: [%d]", i, samples[i]);
        int sample = samples[i];
        if (prevSample != INVALID_MEASUREMENT &&
                abs(sample - prevSample) > self->touch_samples_threshold) {
            return INVALID_MEASUREMENT;
        }
        prevSample = sample;
    }

    // return median

    return samples[sample_count / 2];
}

STATIC void IRAM_ATTR rtch_isr_handler(void* arg)
{
    rtch_t *self = (rtch_t*)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Disable gpio interrupt

    gpio_intr_disable(self->touch_sense);

    // Notify the task

    xTaskNotifyFromISR(
            self->rtch_task_handle,
            0,
            eNoAction,
            &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) portYIELD_FROM_ISR();
}


STATIC void rtch_task(void* arg)
{
    rtch_t *self = (rtch_t*)arg;

    ESP_LOGD(TAG, "rtch_task started.");

    for( ;; )
    {
        // Enable Interrupt

        //gpio_intr_enable(self->touch_sense);
        gpio_isr_handler_add(self->touch_sense, rtch_isr_handler, self);

        // Wait for interrupt

        ESP_LOGD(TAG, "Waiting for interrupt...");

        xTaskNotifyWait(
                0x00,      /* Don't clear any notification bits on entry. */
                0x00,      /* Don't clear any notification bits on exit. */
                NULL,      /* Notified value ignored. */
                portMAX_DELAY );  /* Block indefinitely. */

        ESP_LOGD(TAG, "Touched!");

        // Touch detected. Loop until untouched

        while(gpio_get_level(self->touch_sense))
        {
            ESP_LOGD(TAG, "Measuring...");

            // measure X and Y
            int x = measure_axis(
                    self,
                    self->xp,
                    self->xm,
                    self->yp,
                    self->ym);

            int y = measure_axis(
                    self,
                    self->yp,
                    self->ym,
                    self->xp,
                    self->xm);

            ESP_LOGD(TAG, "RAW: [%d, %d]", x, y);

            // If measurements valid, calculate calibrated X and Y
            if (x != INVALID_MEASUREMENT && y != INVALID_MEASUREMENT)
            {
                x = ((x - self->cal_x0) * self->screen_width) / (self->cal_x1 - self->cal_x0);
                y = ((y - self->cal_y0) * self->screen_height) / (self->cal_y1 - self->cal_y0);
                if (1) // (x >= 0 && y >= 0 && x < self->screen_width && y < self->screen_height)
                {
                    ESP_LOGD(TAG, "[%d, %d]", x, y);

                    // Update touch info
                    xSemaphoreTake(self->rtch_info_mutex, portMAX_DELAY);
                    self->rtch_info.touched = true;
                    self->rtch_info.x = x;
                    self->rtch_info.y = y;
                    xSemaphoreGive(self->rtch_info_mutex);
                } else {
                    ESP_LOGD(TAG, "Overflow measurement [%d, %d]", x, y);
                }
            } else {
                ESP_LOGD(TAG, "Invalid measurement");
            }

            enable_touch_sense(self);
        }

        ESP_LOGD(TAG, "Untouched!");

        // Untouched. Update touch info
        xSemaphoreTake(self->rtch_info_mutex, portMAX_DELAY);
        self->rtch_info.touched = false;
        xSemaphoreGive(self->rtch_info_mutex);
    }
}


