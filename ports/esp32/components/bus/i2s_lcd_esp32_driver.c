// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_heap_caps.h"
#include "esp32/rom/lldesc.h"
#include "soc/dport_access.h"
#include "soc/dport_reg.h"
#include "soc/i2s_struct.h"
#include "hal/gpio_ll.h"
#include "esp_log.h"
#include "i2s_lcd_driver.h"

static const char *TAG = "ESP32_I2S_LCD";

#define I2S_CHECK(a, str, ret) if (!(a)) {                                              \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);       \
        return (ret);                                                                   \
    }

#define LCD_CAM_DMA_NODE_BUFFER_MAX_SIZE  (4000) // 4-byte aligned
#define LCD_DATA_MAX_WIDTH (24)  /*!< Maximum width of LCD data bus */

typedef struct {
    uint32_t dma_buffer_size;
    uint32_t dma_half_buffer_size;
    uint32_t dma_node_buffer_size;
    uint32_t dma_node_cnt;
    uint32_t dma_half_node_cnt;
    lldesc_t *dma;
    uint8_t  *dma_buffer;
    QueueHandle_t event_queue;
    uint8_t width;
    bool swap_data;
    intr_handle_t lcd_cam_intr_handle;
    i2s_dev_t *i2s_dev;
} i2s_lcd_obj_t;

typedef struct {
    void (*i2s_write_data_func)(i2s_lcd_obj_t *i2s_lcd_obj, uint8_t *data, size_t len);
    int rs_io_num;
    i2s_lcd_obj_t *i2s_lcd_obj;
    SemaphoreHandle_t mutex;
} i2s_lcd_driver_t;

static void IRAM_ATTR i2s_isr(void *arg)
{
    BaseType_t HPTaskAwoken = pdFALSE;
    i2s_lcd_obj_t *i2s_lcd_obj = (i2s_lcd_obj_t *)arg;
    i2s_dev_t *i2s_dev = i2s_lcd_obj->i2s_dev;

    typeof(i2s_dev->int_st) status = i2s_dev->int_st;
    i2s_dev->int_clr.val = status.val;
    if (status.val == 0) {
        return;
    }

    if (status.out_eof) {
        xQueueSendFromISR(i2s_lcd_obj->event_queue, (void *)&status.val, &HPTaskAwoken);
    }

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void lcd_dma_set_int(i2s_lcd_obj_t *i2s_lcd_obj)
{
    // Generate a data DMA linked list
    for (int x = 0; x < i2s_lcd_obj->dma_node_cnt; x++) {
        i2s_lcd_obj->dma[x].size = i2s_lcd_obj->dma_node_buffer_size;
        i2s_lcd_obj->dma[x].length = i2s_lcd_obj->dma_node_buffer_size;
        i2s_lcd_obj->dma[x].buf = (i2s_lcd_obj->dma_buffer + i2s_lcd_obj->dma_node_buffer_size * x);
        i2s_lcd_obj->dma[x].eof = !((x + 1) % i2s_lcd_obj->dma_half_node_cnt);
        i2s_lcd_obj->dma[x].empty = (uint32_t)&i2s_lcd_obj->dma[(x + 1) % i2s_lcd_obj->dma_node_cnt];
    }
    i2s_lcd_obj->dma[i2s_lcd_obj->dma_half_node_cnt - 1].empty = (uint32_t)NULL;
    i2s_lcd_obj->dma[i2s_lcd_obj->dma_node_cnt - 1].empty = (uint32_t)NULL;
}

static void lcd_dma_set_left(i2s_lcd_obj_t *i2s_lcd_obj, int pos, size_t len)
{
    int end_pos = 0, size = 0;
    // Processing data length is an integer multiple of i2s_lcd_obj->dma_node_buffer_size
    if (len % i2s_lcd_obj->dma_node_buffer_size) {
        end_pos = (pos % 2) * i2s_lcd_obj->dma_half_node_cnt + len / i2s_lcd_obj->dma_node_buffer_size;
        size = len % i2s_lcd_obj->dma_node_buffer_size;
    } else {
        end_pos = (pos % 2) * i2s_lcd_obj->dma_half_node_cnt + len / i2s_lcd_obj->dma_node_buffer_size - 1;
        size = i2s_lcd_obj->dma_node_buffer_size;
    }
    // Process the tail node to make it a DMA tail
    i2s_lcd_obj->dma[end_pos].size = size;
    i2s_lcd_obj->dma[end_pos].length = size;
    i2s_lcd_obj->dma[end_pos].eof = 1;
    i2s_lcd_obj->dma[end_pos].empty = (uint32_t)NULL;
}

static void lcd_i2s_start(i2s_dev_t *i2s_dev, uint8_t fifo_mode, uint32_t addr, size_t len)
{
    while (!i2s_dev->state.tx_idle);
    i2s_dev->fifo_conf.tx_fifo_mod = fifo_mode;
    i2s_dev->conf.tx_start = 0;
    i2s_dev->conf.tx_reset = 1;
    i2s_dev->conf.tx_reset = 0;
    i2s_dev->lc_conf.out_rst = 1;
    i2s_dev->lc_conf.out_rst = 0;
    i2s_dev->conf.tx_fifo_reset = 1;
    i2s_dev->conf.tx_fifo_reset = 0;
    i2s_dev->out_link.addr = addr;
    i2s_dev->out_link.start = 1;
    ets_delay_us(1);
    i2s_dev->conf.tx_start = 1;
}

static void i2s_write_8bit_data(i2s_lcd_obj_t *i2s_lcd_obj, uint8_t *data, size_t len)
{
    int event  = 0;
    int x = 0, y = 0, left = 0, cnt = 0;
    if (len <= 0) {
        ESP_LOGE(TAG, "wrong len!");
        return;
    }
    len = len * 2;
    lcd_dma_set_int(i2s_lcd_obj);
    uint8_t fifo_mode = 1;
    // Start signal
    xQueueSend(i2s_lcd_obj->event_queue, &event, 0);
    cnt = len / i2s_lcd_obj->dma_half_buffer_size;
    // Process a complete piece of data, ping-pong operation
    for (x = 0; x < cnt; x++) {
        uint8_t *out = (uint8_t *)i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt].buf;
        uint8_t *in  = data;
        if (!i2s_lcd_obj->swap_data) { // data will be swapped when fifo_mode=1, so negate the lcd.swap_data
            for (y = 0; y < i2s_lcd_obj->dma_half_buffer_size; y += 4) {
                out[y + 3] = in[(y >> 1) + 0];
                out[y + 1] = in[(y >> 1) + 1];
            }
        } else {
            for (y = 0; y < i2s_lcd_obj->dma_half_buffer_size; y += 4) {
                out[y + 1] = in[(y >> 1) + 0];
                out[y + 3] = in[(y >> 1) + 1];
            }
        }
        data += i2s_lcd_obj->dma_half_buffer_size >> 1;
        xQueueReceive(i2s_lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
        lcd_i2s_start(i2s_lcd_obj->i2s_dev, fifo_mode, ((uint32_t)&i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt]) & 0xfffff, i2s_lcd_obj->dma_half_buffer_size);
    }
    left = len % i2s_lcd_obj->dma_half_buffer_size;
    // Process remaining incomplete segment data
    while (left) {
        uint8_t *out = (uint8_t *)i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt].buf;
        uint8_t *in  = data;
        if (left > 2) {
            cnt = left - left % 4;
            left = left % 4;
            data += cnt >> 1;
            if (!i2s_lcd_obj->swap_data) { // data will be swapped when fifo_mode=1, so negate the lcd.swap_data
                for (y = 0; y < cnt; y += 4) {
                    out[y + 3] = in[(y >> 1) + 0];
                    out[y + 1] = in[(y >> 1) + 1];
                }
            } else {
                for (y = 0; y < cnt; y += 4) {
                    out[y + 1] = in[(y >> 1) + 0];
                    out[y + 3] = in[(y >> 1) + 1];
                }
            }
        } else {
            cnt = 4;
            left = 0;
            fifo_mode = 3;
            out[3] = in[0];
        }
        // printf("[");
        // for (size_t i = 0; i < cnt; i++) {
        //     printf("%02x, ", out[i]);
        // } printf("]\n");
        lcd_dma_set_left(i2s_lcd_obj, x, cnt);
        xQueueReceive(i2s_lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
        lcd_i2s_start(i2s_lcd_obj->i2s_dev, fifo_mode, ((uint32_t)&i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt]) & 0xfffff, cnt);
        x++;
    }
    xQueueReceive(i2s_lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
}

static void i2s_write_16bit_data(i2s_lcd_obj_t *i2s_lcd_obj, uint8_t *data, size_t len)
{
    int event  = 0;
    int x = 0, y = 0, left = 0, cnt = 0;
    if (len <= 0 || len % 2 != 0) {
        ESP_LOGE(TAG, "wrong len!");
        return;
    }
    lcd_dma_set_int(i2s_lcd_obj);
    uint8_t fifo_mode = 1;
    // Start signal
    xQueueSend(i2s_lcd_obj->event_queue, &event, 0);
    cnt = len / i2s_lcd_obj->dma_half_buffer_size;
    // Process a complete piece of data, ping-pong operation
    for (x = 0; x < cnt; x++) {
        uint8_t *out = (uint8_t *)i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt].buf;
        uint8_t *in  = data;
        if (i2s_lcd_obj->swap_data) {
            for (y = 0; y < i2s_lcd_obj->dma_half_buffer_size; y += 4) {
                out[y + 3] = in[y + 0];
                out[y + 2] = in[y + 1];
                out[y + 1] = in[y + 2];
                out[y + 0] = in[y + 3];
            }
        } else {
            for (y = 0; y < i2s_lcd_obj->dma_half_buffer_size; y += 4) {
                out[y + 2] = in[y + 0];
                out[y + 3] = in[y + 1];
                out[y + 0] = in[y + 2];
                out[y + 1] = in[y + 3];
            }
        }
        data += i2s_lcd_obj->dma_half_buffer_size;
        xQueueReceive(i2s_lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
        lcd_i2s_start(i2s_lcd_obj->i2s_dev, fifo_mode, ((uint32_t)&i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt]) & 0xfffff, i2s_lcd_obj->dma_half_buffer_size);
    }
    left = len % i2s_lcd_obj->dma_half_buffer_size;
    // Process remaining incomplete segment data
    while (left) {
        uint8_t *out = (uint8_t *)i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt].buf;
        uint8_t *in = data;
        if (left > 2) {
            cnt = left - left % 4;
            left = left % 4;
            data += cnt;
            if (i2s_lcd_obj->swap_data) {
                for (y = 0; y < cnt; y += 4) {
                    out[y + 3] = in[y + 0];
                    out[y + 2] = in[y + 1];
                    out[y + 1] = in[y + 2];
                    out[y + 0] = in[y + 3];
                }
            } else {
                for (y = 0; y < cnt; y += 4) {
                    out[y + 2] = in[y + 0];
                    out[y + 3] = in[y + 1];
                    out[y + 0] = in[y + 2];
                    out[y + 1] = in[y + 3];
                }
            }
        } else {
            cnt = 4;
            left = 0;
            fifo_mode = 3;
            if (i2s_lcd_obj->swap_data) {
                out[3] = in[0];
                out[2] = in[1];
            } else {
                out[2] = in[0];
                out[3] = in[1];
            }
        }
        lcd_dma_set_left(i2s_lcd_obj, x, cnt);
        xQueueReceive(i2s_lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
        lcd_i2s_start(i2s_lcd_obj->i2s_dev, fifo_mode, ((uint32_t)&i2s_lcd_obj->dma[(x % 2) * i2s_lcd_obj->dma_half_node_cnt]) & 0xfffff, cnt);
        x++;
    }
    xQueueReceive(i2s_lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
}

static esp_err_t i2s_lcd_reg_config(i2s_dev_t *i2s_dev, uint16_t data_width, uint32_t clk_freq)
{
    // Configure the clock
    i2s_dev->clkm_conf.clkm_div_num = 2; // 160MHz / 2 = 80MHz
    i2s_dev->clkm_conf.clkm_div_b = 0;
    i2s_dev->clkm_conf.clkm_div_a = 10;
    i2s_dev->clkm_conf.clk_en = 1;

    i2s_dev->conf.val = 0;
    i2s_dev->fifo_conf.val = 0;
    i2s_dev->fifo_conf.dscr_en = 1;

    i2s_dev->conf2.lcd_en = 1;
    i2s_dev->conf2.camera_en = 1;

    i2s_dev->lc_conf.ahbm_fifo_rst = 1;
    i2s_dev->lc_conf.ahbm_fifo_rst = 0;
    i2s_dev->lc_conf.ahbm_rst = 1;
    i2s_dev->lc_conf.ahbm_rst = 0;
    i2s_dev->lc_conf.check_owner = 0;
    i2s_dev->lc_conf.out_loop_test = 0;
    i2s_dev->lc_conf.out_auto_wrback = 0;
    i2s_dev->lc_conf.out_data_burst_en = 1;
    i2s_dev->lc_conf.out_no_restart_clr = 0;
    i2s_dev->lc_conf.indscr_burst_en = 0;
    i2s_dev->lc_conf.out_eof_mode = 1;

    i2s_dev->timing.val = 0;

    i2s_dev->int_ena.val = 0;
    i2s_dev->int_clr.val = ~0;

    // Configure sampling rate
    i2s_dev->sample_rate_conf.tx_bck_div_num = 40000000 / clk_freq; // Fws = Fbck / 2
    i2s_dev->sample_rate_conf.tx_bits_mod = (data_width == 8) ? 0 : 1;
    // Configuration data format
    i2s_dev->conf.tx_start = 0;
    i2s_dev->conf.tx_reset = 1;
    i2s_dev->conf.tx_reset = 0;
    i2s_dev->conf.tx_fifo_reset = 1;
    i2s_dev->conf.tx_fifo_reset = 0;
    i2s_dev->conf.tx_slave_mod = 0;
    i2s_dev->conf.tx_right_first = 1; // Must be set to 1, otherwise the clock line will change during reset
    i2s_dev->conf.tx_msb_right = 0;
    i2s_dev->conf.tx_short_sync = 0;
    i2s_dev->conf.tx_mono = 0;
    i2s_dev->conf.tx_msb_shift = 0;

    i2s_dev->conf1.tx_pcm_bypass = 1;
    i2s_dev->conf1.tx_stop_en = 1;

    i2s_dev->conf_chan.tx_chan_mod = 1;

    i2s_dev->fifo_conf.tx_fifo_mod_force_en = 1;
    i2s_dev->fifo_conf.tx_data_num = 32;
    i2s_dev->fifo_conf.tx_fifo_mod = 1;

    i2s_dev->lc_conf.out_rst  = 1;
    i2s_dev->lc_conf.out_rst  = 0;

    i2s_dev->int_ena.out_eof = 1;

    return ESP_OK;
}

static esp_err_t lcd_set_pin(const i2s_lcd_config_t *config)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin_num_wr], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin_num_wr, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(config->pin_num_wr, GPIO_FLOATING);
    gpio_matrix_out(config->pin_num_wr, I2S0O_WS_OUT_IDX, true, false);

    for (int i = 0; i < config->data_width; i++) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin_data_num[i]], PIN_FUNC_GPIO);
        gpio_set_direction(config->pin_data_num[i], GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(config->pin_data_num[i], GPIO_FLOATING);
        // High bit aligned, OUT23 is always the highest bit
        gpio_matrix_out(config->pin_data_num[i], I2S0O_DATA_OUT0_IDX + (LCD_DATA_MAX_WIDTH - config->data_width) + i, false, false);
    }

    return ESP_OK;
}

static esp_err_t lcd_dma_config(i2s_lcd_obj_t *i2s_lcd_obj, uint32_t max_dma_buffer_size)
{
    int cnt = 0;
    if (LCD_CAM_DMA_NODE_BUFFER_MAX_SIZE % 2 != 0) {
        ESP_LOGE(TAG, "ESP32 only supports 2-byte aligned data length");
        return ESP_FAIL;
    }
    if (max_dma_buffer_size >= LCD_CAM_DMA_NODE_BUFFER_MAX_SIZE * 2) {
        i2s_lcd_obj->dma_node_buffer_size = LCD_CAM_DMA_NODE_BUFFER_MAX_SIZE;
        for (cnt = 0; cnt < max_dma_buffer_size - 8; cnt++) { // Find a buffer size that can divide dma_size
            if ((max_dma_buffer_size - cnt) % (i2s_lcd_obj->dma_node_buffer_size * 2) == 0) {
                break;
            }
        }
        i2s_lcd_obj->dma_buffer_size = max_dma_buffer_size - cnt;
    } else {
        i2s_lcd_obj->dma_node_buffer_size = max_dma_buffer_size / 2;
        i2s_lcd_obj->dma_buffer_size = i2s_lcd_obj->dma_node_buffer_size * 2;
    }

    i2s_lcd_obj->dma_half_buffer_size = i2s_lcd_obj->dma_buffer_size / 2;
    i2s_lcd_obj->dma_node_cnt = (i2s_lcd_obj->dma_buffer_size) / i2s_lcd_obj->dma_node_buffer_size; // Number of DMA nodes
    i2s_lcd_obj->dma_half_node_cnt = i2s_lcd_obj->dma_node_cnt / 2;

    ESP_LOGI(TAG, "lcd_buffer_size: %d, lcd_dma_size: %d, lcd_dma_node_cnt: %d", i2s_lcd_obj->dma_buffer_size, i2s_lcd_obj->dma_node_buffer_size, i2s_lcd_obj->dma_node_cnt);

    i2s_lcd_obj->dma    = (lldesc_t *)heap_caps_calloc(i2s_lcd_obj->dma_node_cnt, sizeof(lldesc_t), MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    i2s_lcd_obj->dma_buffer = (uint8_t *)heap_caps_calloc(i2s_lcd_obj->dma_buffer_size, sizeof(uint8_t), MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    return ESP_OK;
}

esp_err_t lcd_cam_deinit(i2s_lcd_driver_t *drv)
{
    if (!drv->i2s_lcd_obj) {
        return ESP_FAIL;
    }

    if (drv->i2s_lcd_obj->event_queue) {
        vQueueDelete(drv->i2s_lcd_obj->event_queue);
    }
    if (drv->i2s_lcd_obj->dma) {
        heap_caps_free(drv->i2s_lcd_obj->dma);
    }
    if (drv->i2s_lcd_obj->dma_buffer) {
        heap_caps_free(drv->i2s_lcd_obj->dma_buffer);
    }

    if (drv->i2s_lcd_obj->lcd_cam_intr_handle) {
        esp_intr_free(drv->i2s_lcd_obj->lcd_cam_intr_handle);
    }

    heap_caps_free(drv->i2s_lcd_obj);
    drv->i2s_lcd_obj = NULL;
    return ESP_OK;
}

static esp_err_t lcd_cam_init(i2s_lcd_driver_t *drv, const i2s_lcd_config_t *config)
{
    esp_err_t ret = ESP_OK;

    i2s_lcd_obj_t *i2s_lcd_obj = (i2s_lcd_obj_t *)heap_caps_calloc(1, sizeof(i2s_lcd_obj_t), MALLOC_CAP_DMA);
    if (i2s_lcd_obj == NULL) {
        ESP_LOGE(TAG, "lcd_cam object malloc failed");
        return ESP_ERR_NO_MEM;
    }
    drv->i2s_lcd_obj = i2s_lcd_obj;

    if (I2S_NUM_0 == config->i2s_port) {
        i2s_lcd_obj->i2s_dev = &I2S0;
        periph_module_enable(PERIPH_I2S0_MODULE);
        ESP_LOGI(TAG, "Enable I2S0");
    } else if (I2S_NUM_1 == config->i2s_port) {
        i2s_lcd_obj->i2s_dev = &I2S1;
        periph_module_enable(PERIPH_I2S1_MODULE);
        ESP_LOGI(TAG, "Enable I2S1");
    } else {
        ESP_LOGE(TAG, "Designated I2S peripheral not found");
    }

    do {
        ret |= i2s_lcd_reg_config(i2s_lcd_obj->i2s_dev, config->data_width, config->clk_freq);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "lcd_cam config fail!");
            break;
        }

        ret |= lcd_set_pin(config);
        ret |= lcd_dma_config(i2s_lcd_obj, config->buffer_size);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "lcd config fail!");
            break;
        }

        i2s_lcd_obj->event_queue = xQueueCreate(1, sizeof(int));
        i2s_lcd_obj->width = config->data_width;
        i2s_lcd_obj->swap_data = config->swap_data;;

        if (i2s_lcd_obj->event_queue == NULL) {
            ESP_LOGE(TAG, "lcd config fail!");
            break;
        }

        if (I2S_NUM_0 == config->i2s_port) {
            ret |= esp_intr_alloc(ETS_I2S0_INTR_SOURCE, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM, i2s_isr, i2s_lcd_obj, &i2s_lcd_obj->lcd_cam_intr_handle);
        } else if (I2S_NUM_1 == config->i2s_port) {
            ret |= esp_intr_alloc(ETS_I2S1_INTR_SOURCE, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM, i2s_isr, i2s_lcd_obj, &i2s_lcd_obj->lcd_cam_intr_handle);
        }
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "lcd_cam intr alloc fail!");
            break;
        }
        ESP_LOGI(TAG, "i2s lcd driver init ok");
        return ESP_OK;
    } while (0);

    lcd_cam_deinit(drv);
    return ESP_FAIL;
}

/**< Public functions */

i2s_lcd_handle_t i2s_lcd_driver_init(const i2s_lcd_config_t *config)
{
    I2S_CHECK(NULL != config, "config pointer invalid", NULL);
    I2S_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(config->pin_num_wr), "GPIO WR invalid", NULL);
    I2S_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(config->pin_num_rs), "GPIO RS invalid", NULL);
    I2S_CHECK(config->data_width > 0 && config->data_width <= 16, "Bit width out of range", NULL);
    I2S_CHECK(0 == (config->data_width % 8), "Bit width must be a multiple of 8", NULL);
    uint64_t pin_mask = 0;
    for (size_t i = 0; i < config->data_width; i++) {
        uint64_t mask = 1ULL << config->pin_data_num[i];
        I2S_CHECK(!(pin_mask & mask), "Data bus GPIO has a duplicate", NULL);
        I2S_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(config->pin_data_num[i]), "Data bus gpio invalid", NULL);
        pin_mask |= mask;
    }

    i2s_lcd_driver_t *i2s_lcd_drv = (i2s_lcd_driver_t *)heap_caps_malloc(sizeof(i2s_lcd_driver_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    I2S_CHECK(NULL != i2s_lcd_drv, "Error malloc handle of i2s lcd driver", NULL);

    esp_err_t ret = lcd_cam_init(i2s_lcd_drv, config);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, "i2s lcd driver initialize failed");
        heap_caps_free(i2s_lcd_drv);
        return NULL;
    }

    i2s_lcd_drv->mutex = xSemaphoreCreateMutex();
    if (i2s_lcd_drv->mutex == NULL) {
        ESP_LOGE(TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, "lcd create mutex failed");
        lcd_cam_deinit(i2s_lcd_drv);
        heap_caps_free(i2s_lcd_drv);
        return NULL;
    }

    if (8 == config->data_width) {
        i2s_lcd_drv->i2s_write_data_func = i2s_write_8bit_data;
    } else if (16 == config->data_width) {
        i2s_lcd_drv->i2s_write_data_func = i2s_write_16bit_data;
    }

    if (config->pin_num_cs >= 0) {
        gpio_pad_select_gpio(config->pin_num_cs);
        gpio_set_direction(config->pin_num_cs, GPIO_MODE_OUTPUT);
        gpio_set_level(config->pin_num_cs, 0);
    }

    gpio_pad_select_gpio(config->pin_num_rs);
    gpio_set_direction(config->pin_num_rs, GPIO_MODE_OUTPUT);
    i2s_lcd_drv->rs_io_num = config->pin_num_rs;
    return (i2s_lcd_handle_t)i2s_lcd_drv;
}

esp_err_t i2s_lcd_driver_deinit(i2s_lcd_handle_t handle)
{
    i2s_lcd_driver_t *i2s_lcd_drv = (i2s_lcd_driver_t *)handle;
    I2S_CHECK(NULL != i2s_lcd_drv, "handle pointer invalid", ESP_ERR_INVALID_ARG);
    lcd_cam_deinit(i2s_lcd_drv);
    vSemaphoreDelete(i2s_lcd_drv->mutex);
    heap_caps_free(handle);
    return ESP_OK;
}

esp_err_t i2s_lcd_write_data(i2s_lcd_handle_t handle, uint16_t data)
{
    i2s_lcd_driver_t *i2s_lcd_drv = (i2s_lcd_driver_t *)handle;
    I2S_CHECK(NULL != i2s_lcd_drv, "handle pointer invalid", ESP_ERR_INVALID_ARG);
    i2s_lcd_drv->i2s_write_data_func(i2s_lcd_drv->i2s_lcd_obj, (uint8_t *)&data, i2s_lcd_drv->i2s_lcd_obj->width == 16 ? 2 : 1);
    return ESP_OK;
}

esp_err_t i2s_lcd_write_cmd(i2s_lcd_handle_t handle, uint16_t cmd)
{
    i2s_lcd_driver_t *i2s_lcd_drv = (i2s_lcd_driver_t *)handle;
    I2S_CHECK(NULL != i2s_lcd_drv, "handle pointer invalid", ESP_ERR_INVALID_ARG);
    gpio_set_level(i2s_lcd_drv->rs_io_num, LCD_CMD_LEV);
    i2s_lcd_drv->i2s_write_data_func(i2s_lcd_drv->i2s_lcd_obj, (uint8_t *)&cmd, i2s_lcd_drv->i2s_lcd_obj->width == 16 ? 2 : 1);
    gpio_set_level(i2s_lcd_drv->rs_io_num, LCD_DATA_LEV);
    return ESP_OK;
}

esp_err_t i2s_lcd_write(i2s_lcd_handle_t handle, const uint8_t *data, uint32_t length)
{
    i2s_lcd_driver_t *i2s_lcd_drv = (i2s_lcd_driver_t *)handle;
    I2S_CHECK(NULL != i2s_lcd_drv, "handle pointer invalid", ESP_ERR_INVALID_ARG);
    i2s_lcd_drv->i2s_write_data_func(i2s_lcd_drv->i2s_lcd_obj, (uint8_t *)data, length);

    return ESP_OK;
}

esp_err_t i2s_lcd_acquire(i2s_lcd_handle_t handle)
{
    i2s_lcd_driver_t *i2s_lcd_drv = (i2s_lcd_driver_t *)handle;
    I2S_CHECK(NULL != i2s_lcd_drv, "handle pointer invalid", ESP_ERR_INVALID_ARG);
    BaseType_t ret = xSemaphoreTake(i2s_lcd_drv->mutex, portMAX_DELAY);
    I2S_CHECK(pdTRUE == ret, "Take semaphore failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t i2s_lcd_release(i2s_lcd_handle_t handle)
{
    i2s_lcd_driver_t *i2s_lcd_drv = (i2s_lcd_driver_t *)handle;
    I2S_CHECK(NULL != i2s_lcd_drv, "handle pointer invalid", ESP_ERR_INVALID_ARG);
    BaseType_t ret = xSemaphoreGive(i2s_lcd_drv->mutex);
    I2S_CHECK(pdTRUE == ret, "Give semaphore failed", ESP_FAIL);
    return ESP_OK;
}

#endif // CONFIG_IDF_TARGET_ESP32
