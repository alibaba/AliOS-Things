// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
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

#include "sdkconfig.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "scr_interface_driver.h"
#include "driver/gpio.h"

static const char *TAG = "screen interface";

#define LCD_IFACE_CHECK(a, str, ret)  if(!(a)) {                           \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }

/**--------------------- I2S interface driver ----------------------*/
typedef struct {
    i2s_lcd_handle_t i2s_lcd_handle;
    scr_interface_driver_t interface_drv;
} interface_i2s_handle_t;

static esp_err_t _i2s_lcd_write_data(void *handle, uint16_t data)
{
    interface_i2s_handle_t *interface_i2s = __containerof(handle, interface_i2s_handle_t, interface_drv);
#ifndef CONFIG_IDF_TARGET_ESP32S3
    return i2s_lcd_write_data(interface_i2s->i2s_lcd_handle, data);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t _i2s_lcd_write_cmd(void *handle, uint16_t cmd)
{
    interface_i2s_handle_t *interface_i2s = __containerof(handle, interface_i2s_handle_t, interface_drv);
#ifndef CONFIG_IDF_TARGET_ESP32S3
    return i2s_lcd_write_cmd(interface_i2s->i2s_lcd_handle, cmd);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t _i2s_lcd_write(void *handle, const uint8_t *data, uint32_t length)
{
    interface_i2s_handle_t *interface_i2s = __containerof(handle, interface_i2s_handle_t, interface_drv);
#ifndef CONFIG_IDF_TARGET_ESP32S3
    return i2s_lcd_write(interface_i2s->i2s_lcd_handle, data, length);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t _i2s_lcd_read(void *handle, uint8_t *data, uint32_t length)
{
#ifndef CONFIG_IDF_TARGET_ESP32S3
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t _i2s_lcd_acquire(void *handle)
{
    interface_i2s_handle_t *interface_i2s = __containerof(handle, interface_i2s_handle_t, interface_drv);
#ifndef CONFIG_IDF_TARGET_ESP32S3
    return i2s_lcd_acquire(interface_i2s->i2s_lcd_handle);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t _i2s_lcd_release(void *handle)
{
    interface_i2s_handle_t *interface_i2s = __containerof(handle, interface_i2s_handle_t, interface_drv);
#ifndef CONFIG_IDF_TARGET_ESP32S3
    return i2s_lcd_release(interface_i2s->i2s_lcd_handle);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

/**--------------------- I2C interface driver ----------------------*/
#define SSD1306_WRITE_CMD           0x00
#define SSD1306_WRITE_DAT           0x40

#define ACK_CHECK_EN                1                   /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS               0                   /*!< I2C master will not check ack from slave */

typedef struct {
    i2c_bus_device_handle_t i2c_dev;
    scr_interface_driver_t interface_drv;
} interface_i2c_handle_t;

static esp_err_t i2c_lcd_driver_init(const scr_interface_i2c_config_t *cfg, interface_i2c_handle_t *out_interface_i2c)
{
    i2c_bus_device_handle_t i2c_dev = i2c_bus_device_create(cfg->i2c_bus, cfg->slave_addr, cfg->clk_speed);
    LCD_IFACE_CHECK(NULL != i2c_dev, "I2C bus initial failed", ESP_FAIL);
    out_interface_i2c->i2c_dev = i2c_dev;
    return ESP_OK;
}

static esp_err_t i2c_lcd_driver_deinit(interface_i2c_handle_t *interface_i2c)
{
    i2c_bus_device_delete(&interface_i2c->i2c_dev);
    return ESP_OK;
}

static esp_err_t i2c_lcd_write_byte(i2c_bus_device_handle_t i2c_dev, uint8_t ctrl, uint8_t data)
{
    esp_err_t ret;

    uint8_t buffer[2];
    buffer[0] = ctrl;
    buffer[1] = data;
    ret = i2c_bus_write_bytes(i2c_dev, NULL_I2C_MEM_ADDR, 2, buffer);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "i2c send failed [%s]", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t i2c_lcd_write_cmd(void *handle, uint16_t cmd)
{
    interface_i2c_handle_t *interface_i2c = __containerof(handle, interface_i2c_handle_t, interface_drv);
    uint8_t v = cmd;
    return i2c_lcd_write_byte(interface_i2c->i2c_dev, SSD1306_WRITE_CMD, v);
}

static esp_err_t i2c_lcd_write_data(void *handle, uint16_t data)
{
    interface_i2c_handle_t *interface_i2c = __containerof(handle, interface_i2c_handle_t, interface_drv);
    uint8_t v = data;
    return i2c_lcd_write_byte(interface_i2c->i2c_dev, SSD1306_WRITE_DAT, v);
}

static esp_err_t i2c_lcd_write(void *handle, const uint8_t *data, uint32_t length)
{
    interface_i2c_handle_t *interface_i2c = __containerof(handle, interface_i2c_handle_t, interface_drv);
    esp_err_t ret;
    ret = i2c_bus_write_bytes(interface_i2c->i2c_dev, SSD1306_WRITE_DAT, length, (uint8_t *)data);
    LCD_IFACE_CHECK(ESP_OK == ret, "i2C send failed", ESP_FAIL);
    return ESP_OK;
}

static esp_err_t i2c_lcd_read(void *handle, uint8_t *data, uint32_t length)
{
    ESP_LOGW(TAG, "lcd i2c unsupport read");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t i2c_lcd_acquire(void *handle)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t i2c_lcd_release(void *handle)
{
    return ESP_ERR_NOT_SUPPORTED;
}

/**--------------------- SPI interface driver ----------------------*/
#define LCD_CMD_LEV   (0)
#define LCD_DATA_LEV  (1)

typedef struct {
    spi_bus_device_handle_t spi_wr_dev;
    int8_t pin_num_dc;
    uint8_t swap_data;
    scr_interface_driver_t interface_drv;
} interface_spi_handle_t;

static esp_err_t spi_lcd_driver_init(const scr_interface_spi_config_t *cfg, interface_spi_handle_t *out_interface_spi)
{
    LCD_IFACE_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(cfg->pin_num_cs), "gpio cs invalid", ESP_ERR_INVALID_ARG);
    LCD_IFACE_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(cfg->pin_num_dc), "gpio dc invalid", ESP_ERR_INVALID_ARG);

    //Initialize non-SPI GPIOs
    gpio_pad_select_gpio(cfg->pin_num_dc);
    gpio_set_direction(cfg->pin_num_dc, GPIO_MODE_OUTPUT);
    out_interface_spi->pin_num_dc = cfg->pin_num_dc;
    out_interface_spi->swap_data = cfg->swap_data;

    spi_device_config_t devcfg = {
        .clock_speed_hz = cfg->clk_freq,     //Clock out frequency
        .mode = 0,                           //SPI mode 0
        .cs_io_num = cfg->pin_num_cs,        //CS pin
    };
    out_interface_spi->spi_wr_dev = spi_bus_device_create(cfg->spi_bus, &devcfg);
    LCD_IFACE_CHECK(NULL != out_interface_spi->spi_wr_dev, "spi device initialize failed", ESP_FAIL);

    return ESP_OK;
}

static esp_err_t spi_lcd_driver_deinit(interface_spi_handle_t *interface_spi)
{
    spi_bus_device_delete(&interface_spi->spi_wr_dev);
    return ESP_OK;
}

static esp_err_t spi_lcd_driver_acquire(void *handle)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t spi_lcd_driver_release(void *handle)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t _lcd_spi_rw(spi_bus_device_handle_t spi, const uint8_t *output, uint8_t *input, uint32_t length)
{
    LCD_IFACE_CHECK(0 != length, "Length should not be 0", ESP_ERR_INVALID_ARG);
    return spi_bus_transfer_bytes(spi, output, input, length);
}

static esp_err_t spi_lcd_driver_write_cmd(void *handle, uint16_t value)
{
    interface_spi_handle_t *interface_spi = __containerof(handle, interface_spi_handle_t, interface_drv);
    esp_err_t ret;
    gpio_set_level(interface_spi->pin_num_dc, LCD_CMD_LEV);
    uint8_t data = value;
    ret = _lcd_spi_rw(interface_spi->spi_wr_dev, &data, NULL, 1);
    gpio_set_level(interface_spi->pin_num_dc, LCD_DATA_LEV);
    LCD_IFACE_CHECK(ESP_OK == ret, "Send cmd failed", ESP_FAIL);
    return ESP_OK;
}

static esp_err_t spi_lcd_driver_write_data(void *handle, uint16_t value)
{
    interface_spi_handle_t *interface_spi = __containerof(handle, interface_spi_handle_t, interface_drv);
    esp_err_t ret;
    uint8_t data = value;
    ret = _lcd_spi_rw(interface_spi->spi_wr_dev, &data, NULL, 1);
    LCD_IFACE_CHECK(ESP_OK == ret, "Send cmd failed", ESP_FAIL);
    return ESP_OK;
}

static esp_err_t spi_lcd_driver_read(void *handle, uint8_t *data, uint32_t length)
{
    interface_spi_handle_t *interface_spi = __containerof(handle, interface_spi_handle_t, interface_drv);
    esp_err_t ret;
    ret = _lcd_spi_rw(interface_spi->spi_wr_dev, NULL, data, length);
    LCD_IFACE_CHECK(ESP_OK == ret, "Read data failed", ESP_FAIL);
    return ESP_OK;
}

static esp_err_t spi_lcd_driver_write(void *handle, const uint8_t *data, uint32_t length)
{
    interface_spi_handle_t *interface_spi = __containerof(handle, interface_spi_handle_t, interface_drv);
    esp_err_t ret;

    /**< Swap the high and low byte of the data */
    uint32_t l = length / 2;
    uint16_t t;
    if (interface_spi->swap_data) {
        uint16_t *p = (uint16_t *)data;
        for (size_t i = 0; i < l; i++) {
            t = *p;
            *p = t >> 8 | t << 8;
            p++;
        }
    }
    ret = _lcd_spi_rw(interface_spi->spi_wr_dev, data, NULL, length);

    /**
     * @brief swap data to restore the order of data
     *
     * TODO: how to avoid swap data here
     *
     */
    if (interface_spi->swap_data) {
        uint16_t *_p = (uint16_t *)data;
        for (size_t i = 0; i < l; i++) {
            t = *_p;
            *_p = t >> 8 | t << 8;
            _p++;
        }
    }
    LCD_IFACE_CHECK(ESP_OK == ret, "Write data failed", ESP_FAIL);
    return ESP_OK;
}

/*********************************************************/
esp_err_t scr_interface_create(scr_interface_type_t type, void *config, scr_interface_driver_t **out_driver)
{
    LCD_IFACE_CHECK(NULL != config, "Pointer of config is invalid", ESP_ERR_INVALID_ARG);
    LCD_IFACE_CHECK(NULL != out_driver, "Pointer of driver is invalid", ESP_ERR_INVALID_ARG);

    switch (type) {
    case SCREEN_IFACE_8080: {
        interface_i2s_handle_t *interface_i2s = heap_caps_malloc(sizeof(interface_i2s_handle_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        LCD_IFACE_CHECK(NULL != interface_i2s, "memory of iface i2s is not enough", ESP_ERR_NO_MEM);
#ifndef CONFIG_IDF_TARGET_ESP32S3
        interface_i2s->i2s_lcd_handle = i2s_lcd_driver_init((i2s_lcd_config_t *)config);
#endif
        if (NULL == interface_i2s->i2s_lcd_handle) {
            ESP_LOGE(TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, "screen 8080 interface create failed");
            heap_caps_free(interface_i2s);
            return ESP_FAIL;
        }

        interface_i2s->interface_drv.type        = type;
        interface_i2s->interface_drv.write_cmd   = _i2s_lcd_write_cmd;
        interface_i2s->interface_drv.write_data  = _i2s_lcd_write_data;
        interface_i2s->interface_drv.write       = _i2s_lcd_write;
        interface_i2s->interface_drv.read        = _i2s_lcd_read;
        interface_i2s->interface_drv.bus_acquire = _i2s_lcd_acquire;
        interface_i2s->interface_drv.bus_release = _i2s_lcd_release;

        *out_driver = &interface_i2s->interface_drv;
    } break;
    case SCREEN_IFACE_SPI: {
        interface_spi_handle_t *interface_spi = heap_caps_malloc(sizeof(interface_spi_handle_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        LCD_IFACE_CHECK(NULL != interface_spi, "memory of iface spi is not enough", ESP_ERR_NO_MEM);
        esp_err_t ret = spi_lcd_driver_init((scr_interface_spi_config_t *)config, interface_spi);
        if (ESP_OK != ret) {
            ESP_LOGE(TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, "screen spi interface create failed");
            heap_caps_free(interface_spi);
            return ESP_FAIL;
        }

        interface_spi->interface_drv.type        = type;
        interface_spi->interface_drv.write_cmd   = spi_lcd_driver_write_cmd;
        interface_spi->interface_drv.write_data  = spi_lcd_driver_write_data;
        interface_spi->interface_drv.write       = spi_lcd_driver_write;
        interface_spi->interface_drv.read        = spi_lcd_driver_read;
        interface_spi->interface_drv.bus_acquire = spi_lcd_driver_acquire;
        interface_spi->interface_drv.bus_release = spi_lcd_driver_release;

        *out_driver = &interface_spi->interface_drv;

    } break;
    case SCREEN_IFACE_I2C: {
        interface_i2c_handle_t *interface_i2c = heap_caps_malloc(sizeof(interface_i2c_handle_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        LCD_IFACE_CHECK(NULL != interface_i2c, "memory of iface i2c is not enough", ESP_ERR_NO_MEM);
        esp_err_t ret = i2c_lcd_driver_init((scr_interface_i2c_config_t *)config, interface_i2c);
        if (ESP_OK != ret) {
            ESP_LOGE(TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, "screen i2c interface create failed");
            heap_caps_free(interface_i2c);
            return ESP_FAIL;
        }

        interface_i2c->interface_drv.type        = type;
        interface_i2c->interface_drv.write_cmd   = i2c_lcd_write_cmd;
        interface_i2c->interface_drv.write_data  = i2c_lcd_write_data;
        interface_i2c->interface_drv.write       = i2c_lcd_write;
        interface_i2c->interface_drv.read        = i2c_lcd_read;
        interface_i2c->interface_drv.bus_acquire = i2c_lcd_acquire;
        interface_i2c->interface_drv.bus_release = i2c_lcd_release;

        *out_driver = &interface_i2c->interface_drv;
    }

    break;
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t scr_interface_delete(const scr_interface_driver_t *driver)
{
    LCD_IFACE_CHECK(NULL != driver, "Pointer of driver is invalid", ESP_ERR_INVALID_ARG);

    switch (driver->type) {
    case SCREEN_IFACE_8080: {
        interface_i2s_handle_t *interface_i2s = __containerof(driver, interface_i2s_handle_t, interface_drv);
        i2s_lcd_driver_deinit(interface_i2s->i2s_lcd_handle);
        heap_caps_free(interface_i2s);
    } break;
    case SCREEN_IFACE_SPI: {
        interface_spi_handle_t *interface_spi = __containerof(driver, interface_spi_handle_t, interface_drv);
        spi_lcd_driver_deinit(interface_spi);
        heap_caps_free(interface_spi);
    } break;
    case SCREEN_IFACE_I2C: {
        interface_i2c_handle_t *interface_i2c = __containerof(driver, interface_i2c_handle_t, interface_drv);
        i2c_lcd_driver_deinit(interface_i2c);
        heap_caps_free(interface_i2c);
    }
    break;
    default:
        break;
    }
    return ESP_OK;
}

