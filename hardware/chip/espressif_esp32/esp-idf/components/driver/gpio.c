// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_types.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif

#include "soc/gpio_caps.h"
#include "soc/gpio_periph.h"
#include "esp_log.h"
#include "hal/gpio_hal.h"

static const char *GPIO_TAG = "gpio";
#define GPIO_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(GPIO_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }
#define GPIO_ISR_CORE_ID_UNINIT    (3)

typedef struct {
    gpio_isr_t fn;   /*!< isr function */
    void *args;      /*!< isr function args */
} gpio_isr_func_t;

// Used by the IPC call to register the interrupt service routine.
typedef struct {
    int source;               /*!< ISR source */
    int intr_alloc_flags;     /*!< ISR alloc flag */
    void (*fn)(void*);        /*!< ISR function */
    void *arg;                /*!< ISR function args*/
    void *handle;             /*!< ISR handle */
    esp_err_t ret;
} gpio_isr_alloc_t;

typedef struct {
    gpio_hal_context_t *gpio_hal;
    portMUX_TYPE gpio_spinlock;
    uint32_t isr_core_id;
    gpio_isr_func_t *gpio_isr_func;
    gpio_isr_handle_t gpio_isr_handle;
} gpio_context_t;

static gpio_hal_context_t _gpio_hal = {
    .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
};

static gpio_context_t gpio_context = {
    .gpio_hal = &_gpio_hal,
    .gpio_spinlock = portMUX_INITIALIZER_UNLOCKED,
    .isr_core_id = GPIO_ISR_CORE_ID_UNINIT,
    .gpio_isr_func = NULL,
};

esp_err_t gpio_pullup_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || GPIO_SUPPORTS_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pullup_en(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        rtc_gpio_pullup_en(gpio_num);
    }

    return ESP_OK;
}

esp_err_t gpio_pullup_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || GPIO_SUPPORTS_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pullup_dis(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        rtc_gpio_pullup_dis(gpio_num);
    }

    return ESP_OK;
}

esp_err_t gpio_pulldown_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || GPIO_SUPPORTS_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pulldown_en(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        rtc_gpio_pulldown_en(gpio_num);
    }

    return ESP_OK;
}

esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || GPIO_SUPPORTS_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pulldown_dis(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        rtc_gpio_pulldown_dis(gpio_num);
    }

    return ESP_OK;
}

esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(intr_type < GPIO_INTR_MAX, "GPIO interrupt type error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_set_intr_type(gpio_context.gpio_hal, gpio_num, intr_type);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

static esp_err_t gpio_intr_enable_on_core(gpio_num_t gpio_num, uint32_t core_id)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_intr_enable_on_core(gpio_context.gpio_hal, gpio_num, core_id);
    return ESP_OK;
}

esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    if(gpio_context.isr_core_id == GPIO_ISR_CORE_ID_UNINIT) {
        gpio_context.isr_core_id = xPortGetCoreID();
    }
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return gpio_intr_enable_on_core (gpio_num, gpio_context.isr_core_id);
}

esp_err_t gpio_intr_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_intr_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_input_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_input_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_input_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_input_enable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_output_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_output_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_output_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG);
    gpio_hal_output_enable(gpio_context.gpio_hal, gpio_num);
    gpio_matrix_out(gpio_num, SIG_GPIO_OUT_IDX, false, false);
    return ESP_OK;
}

static esp_err_t gpio_od_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_od_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_od_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_od_enable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG);
    gpio_hal_set_level(gpio_context.gpio_hal, gpio_num, level);
    return ESP_OK;
}

int gpio_get_level(gpio_num_t gpio_num)
{
    return gpio_hal_get_level(gpio_context.gpio_hal, gpio_num);
}

esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(pull <= GPIO_FLOATING, "GPIO pull mode error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    switch (pull) {
        case GPIO_PULLUP_ONLY:
            gpio_pulldown_dis(gpio_num);
            gpio_pullup_en(gpio_num);
            break;

        case GPIO_PULLDOWN_ONLY:
            gpio_pulldown_en(gpio_num);
            gpio_pullup_dis(gpio_num);
            break;

        case GPIO_PULLUP_PULLDOWN:
            gpio_pulldown_en(gpio_num);
            gpio_pullup_en(gpio_num);
            break;

        case GPIO_FLOATING:
            gpio_pulldown_dis(gpio_num);
            gpio_pullup_dis(gpio_num);
            break;

        default:
            ESP_LOGE(GPIO_TAG, "Unknown pull up/down mode,gpio_num=%u,pull=%u", gpio_num, pull);
            ret = ESP_ERR_INVALID_ARG;
            break;
    }

    return ret;
}

esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if ((GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) != true) && (mode & GPIO_MODE_DEF_OUTPUT)) {
        ESP_LOGE(GPIO_TAG, "io_num=%d can only be input", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    if (mode & GPIO_MODE_DEF_INPUT) {
        gpio_input_enable(gpio_num);
    } else {
        gpio_input_disable(gpio_num);
    }

    if (mode & GPIO_MODE_DEF_OUTPUT) {
        gpio_output_enable(gpio_num);
    } else {
        gpio_output_disable(gpio_num);
    }

    if (mode & GPIO_MODE_DEF_OD) {
        gpio_od_enable(gpio_num);
    } else {
        gpio_od_disable(gpio_num);
    }

    return ret;
}

esp_err_t gpio_config(const gpio_config_t *pGPIOConfig)
{
    uint64_t gpio_pin_mask = (pGPIOConfig->pin_bit_mask);
    uint32_t io_reg = 0;
    uint32_t io_num = 0;
    uint8_t input_en = 0;
    uint8_t output_en = 0;
    uint8_t od_en = 0;
    uint8_t pu_en = 0;
    uint8_t pd_en = 0;

    if (pGPIOConfig->pin_bit_mask == 0 || pGPIOConfig->pin_bit_mask >= (((uint64_t) 1) << GPIO_PIN_COUNT)) {
        ESP_LOGE(GPIO_TAG, "GPIO_PIN mask error ");
        return ESP_ERR_INVALID_ARG;
    }

    if ((pGPIOConfig->mode) & (GPIO_MODE_DEF_OUTPUT)) {
        if(GPIO_MASK_CONTAIN_INPUT_GPIO(gpio_pin_mask)) {
            ESP_LOGE(GPIO_TAG, "GPIO can only be used as input mode");
            return ESP_ERR_INVALID_ARG;
        }
    }

    do {
        io_reg = GPIO_PIN_MUX_REG[io_num];

        if (((gpio_pin_mask >> io_num) & BIT(0))) {
            if (!io_reg) {
                ESP_LOGE(GPIO_TAG, "IO%d is not a valid GPIO", io_num);
                return ESP_ERR_INVALID_ARG;
            }

            if (rtc_gpio_is_valid_gpio(io_num)) {
                rtc_gpio_deinit(io_num);
            }

            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_INPUT) {
                input_en = 1;
                gpio_input_enable(io_num);
            } else {
                gpio_input_disable(io_num);
            }

            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_OD) {
                od_en = 1;
                gpio_od_enable(io_num);
            } else {
                gpio_od_disable(io_num);
            }

            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_OUTPUT) {
                output_en = 1;
                gpio_output_enable(io_num);
            } else {
                gpio_output_disable(io_num);
            }

            if (pGPIOConfig->pull_up_en) {
                pu_en = 1;
                gpio_pullup_en(io_num);
            } else {
                gpio_pullup_dis(io_num);
            }

            if (pGPIOConfig->pull_down_en) {
                pd_en = 1;
                gpio_pulldown_en(io_num);
            } else {
                gpio_pulldown_dis(io_num);
            }

            ESP_LOGI(GPIO_TAG, "GPIO[%d]| InputEn: %d| OutputEn: %d| OpenDrain: %d| Pullup: %d| Pulldown: %d| Intr:%d ", io_num, input_en, output_en, od_en, pu_en, pd_en, pGPIOConfig->intr_type);
            gpio_set_intr_type(io_num, pGPIOConfig->intr_type);

            if (pGPIOConfig->intr_type) {
                gpio_intr_enable(io_num);
            } else {
                gpio_intr_disable(io_num);
            }

            PIN_FUNC_SELECT(io_reg, PIN_FUNC_GPIO); /*function number 2 is GPIO_FUNC for each pin */
        }

        io_num++;
    } while (io_num < GPIO_PIN_COUNT);

    return ESP_OK;
}

esp_err_t gpio_reset_pin(gpio_num_t gpio_num)
{
    assert(gpio_num >= 0 && GPIO_IS_VALID_GPIO(gpio_num));
    gpio_config_t cfg = {
        .pin_bit_mask = BIT64(gpio_num),
        .mode = GPIO_MODE_DISABLE,
        //for powersave reasons, the GPIO should not be floating, select pullup
        .pull_up_en = true,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
    return ESP_OK;
}

static inline void IRAM_ATTR gpio_isr_loop(uint32_t status, const uint32_t gpio_num_start)
{
    while (status) {
        int nbit = __builtin_ffs(status) - 1;
        status &= ~(1 << nbit);
        int gpio_num = gpio_num_start + nbit;

        if (gpio_context.gpio_isr_func[gpio_num].fn != NULL) {
            gpio_context.gpio_isr_func[gpio_num].fn(gpio_context.gpio_isr_func[gpio_num].args);
        }
    }
}

static void IRAM_ATTR gpio_intr_service(void *arg)
{
    //GPIO intr process
    if (gpio_context.gpio_isr_func == NULL) {
        return;
    }

    //read status to get interrupt status for GPIO0-31
    uint32_t gpio_intr_status;
    gpio_hal_get_intr_status(gpio_context.gpio_hal, gpio_context.isr_core_id, &gpio_intr_status);

    if (gpio_intr_status) {
        gpio_isr_loop(gpio_intr_status, 0);
        gpio_hal_clear_intr_status(gpio_context.gpio_hal, gpio_intr_status);
    }

    //read status1 to get interrupt status for GPIO32-39
    uint32_t gpio_intr_status_h;
    gpio_hal_get_intr_status_high(gpio_context.gpio_hal, gpio_context.isr_core_id, &gpio_intr_status_h);

    if (gpio_intr_status_h) {
        gpio_isr_loop(gpio_intr_status_h, 32);
        gpio_hal_clear_intr_status_high(gpio_context.gpio_hal, gpio_intr_status_h);
    }
}

esp_err_t gpio_install_isr_service(int intr_alloc_flags)
{
    GPIO_CHECK(gpio_context.gpio_isr_func == NULL, "GPIO isr service already installed", ESP_ERR_INVALID_STATE);
    esp_err_t ret;
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_context.gpio_isr_func = (gpio_isr_func_t *) calloc(GPIO_NUM_MAX, sizeof(gpio_isr_func_t));
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    if (gpio_context.gpio_isr_func == NULL) {
        ret = ESP_ERR_NO_MEM;
    } else {
        ret = gpio_isr_register(gpio_intr_service, NULL, intr_alloc_flags, &gpio_context.gpio_isr_handle);
    }

    return ret;
}

esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
{
    GPIO_CHECK(gpio_context.gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", ESP_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_context.gpio_isr_func) {
        gpio_context.gpio_isr_func[gpio_num].fn = isr_handler;
        gpio_context.gpio_isr_func[gpio_num].args = args;
    }
    gpio_intr_enable_on_core (gpio_num, esp_intr_get_cpu(gpio_context.gpio_isr_handle));
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)
{
    GPIO_CHECK(gpio_context.gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", ESP_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_context.gpio_isr_func) {
        gpio_context.gpio_isr_func[gpio_num].fn = NULL;
        gpio_context.gpio_isr_func[gpio_num].args = NULL;
    }
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

void gpio_uninstall_isr_service(void)
{
    if (gpio_context.gpio_isr_func == NULL) {
        return;
    }
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    esp_intr_free(gpio_context.gpio_isr_handle);
    free(gpio_context.gpio_isr_func);
    gpio_context.gpio_isr_func = NULL;
    gpio_context.isr_core_id = GPIO_ISR_CORE_ID_UNINIT;
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return;
}


static void gpio_isr_register_on_core_static(void *param)
{
    gpio_isr_alloc_t *p = (gpio_isr_alloc_t *)param;
    //We need to check the return value.
    p->ret = esp_intr_alloc(p->source, p->intr_alloc_flags, p->fn, p->arg, p->handle);
}

esp_err_t gpio_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle)
{
    GPIO_CHECK(fn, "GPIO ISR null", ESP_ERR_INVALID_ARG);
    gpio_isr_alloc_t p;
    p.source = ETS_GPIO_INTR_SOURCE;
    p.intr_alloc_flags = intr_alloc_flags;
    p.fn = fn;
    p.arg = arg;
    p.handle = handle;
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    if(gpio_context.isr_core_id == GPIO_ISR_CORE_ID_UNINIT) {
        gpio_context.isr_core_id = xPortGetCoreID();
    }
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    esp_err_t ret;
#if CONFIG_FREERTOS_UNICORE
    gpio_isr_register_on_core_static(&p);
    ret = ESP_OK;
#else /* CONFIG_FREERTOS_UNICORE */
    ret = esp_ipc_call_blocking(gpio_context.isr_core_id, gpio_isr_register_on_core_static, (void *)&p);
#endif /* !CONFIG_FREERTOS_UNICORE */
    if(ret != ESP_OK || p.ret != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    if ((intr_type == GPIO_INTR_LOW_LEVEL) || (intr_type == GPIO_INTR_HIGH_LEVEL)) {
        if (rtc_gpio_is_valid_gpio(gpio_num)) {
            ret = rtc_gpio_wakeup_enable(gpio_num, intr_type);
        }
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_wakeup_enable(gpio_context.gpio_hal, gpio_num, intr_type);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ESP_LOGE(GPIO_TAG, "GPIO wakeup only supports level mode, but edge mode set. gpio_num:%u", gpio_num);
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}

esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    if (rtc_gpio_is_valid_gpio(gpio_num)) {
        ret = rtc_gpio_wakeup_disable(gpio_num);
    }
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_wakeup_disable(gpio_context.gpio_hal, gpio_num);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ret;
}

esp_err_t gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(strength < GPIO_DRIVE_CAP_MAX, "GPIO drive capability error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    if (!rtc_gpio_is_valid_gpio(gpio_num) || GPIO_SUPPORTS_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_set_drive_capability(gpio_context.gpio_hal, gpio_num, strength);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ret = rtc_gpio_set_drive_capability(gpio_num, strength);
    }

    return ret;
}

esp_err_t gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t *strength)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(strength != NULL, "GPIO drive capability pointer error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    if (!rtc_gpio_is_valid_gpio(gpio_num) || GPIO_SUPPORTS_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_get_drive_capability(gpio_context.gpio_hal, gpio_num, strength);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ret = rtc_gpio_get_drive_capability(gpio_num, strength);
    }

    return ret;
}

esp_err_t gpio_hold_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Only output-capable GPIO support this function", ESP_ERR_NOT_SUPPORTED);
    int ret = ESP_OK;

    if (rtc_gpio_is_valid_gpio(gpio_num)) {
        ret = rtc_gpio_hold_en(gpio_num);
    } else if (GPIO_HOLD_MASK[gpio_num]) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_hold_en(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ret = ESP_ERR_NOT_SUPPORTED;
    }

    return ret;
}

esp_err_t gpio_hold_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Only output-capable GPIO support this function", ESP_ERR_NOT_SUPPORTED);
    int ret = ESP_OK;

    if (rtc_gpio_is_valid_gpio(gpio_num)) {
        ret = rtc_gpio_hold_dis(gpio_num);
    }else if (GPIO_HOLD_MASK[gpio_num]) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_hold_dis(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ret = ESP_ERR_NOT_SUPPORTED;
    }

    return ret;
}

void gpio_deep_sleep_hold_en(void)
{
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_deep_sleep_hold_en(gpio_context.gpio_hal);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
}

void gpio_deep_sleep_hold_dis(void)
{
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_deep_sleep_hold_dis(gpio_context.gpio_hal);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
}

#if GPIO_SUPPORTS_FORCE_HOLD

esp_err_t gpio_force_hold_all()
{
    rtc_gpio_force_hold_all();
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_force_hold_all(gpio_context.gpio_hal);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

esp_err_t gpio_force_unhold_all()
{
    rtc_gpio_force_hold_dis_all();
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_force_unhold_all(gpio_context.gpio_hal);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}
#endif

void gpio_iomux_in(uint32_t gpio, uint32_t signal_idx)
{
    gpio_hal_iomux_in(gpio_context.gpio_hal, gpio, signal_idx);
}

void gpio_iomux_out(uint8_t gpio_num, int func, bool oen_inv)
{
    gpio_hal_iomux_out(gpio_context.gpio_hal, gpio_num, func, (uint32_t)oen_inv);
}
