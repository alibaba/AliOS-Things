/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS

#include "board_mgr.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/list.h"
#include "aos_hal_adc.h"
#include "aos_hal_gpio.h"
#include "aos_hal_i2c.h"
#include "aos_hal_pwm.h"
#include "aos_hal_spi.h"
#include "aos_hal_uart.h"
#include "aos_hal_wdg.h"
#include "board_config.h"
#include "cJSON.h"
#include "ulog/ulog.h"

#define LOG_TAG     "BOARD_MGR"

#define DRIVER_NAME "driver.json"

// extern int py_repl_config;
int py_repl_config = 0;

typedef struct parse_json {
    char *marker_name;
    addon_module_m module;
    int8_t (*fn)(cJSON *, char *);
} parse_json_t;

typedef struct board_item {
    addon_module_m module;
    item_handle_t handle;
    char *name_id;
    void *node;
    uint8_t status;
} board_item_t;

typedef struct board_mgr {
    uint32_t item_size;
    board_item_t **item;
} board_mgr_t;

typedef struct page_entry {
    char *page;
    dlist_t node;
} page_entry_t;

static board_mgr_t g_board_mgr = { 0, NULL };

static dlist_t g_pages_list;

static int8_t board_add_new_item(addon_module_m module, char *name_id, void *node);

static board_mgr_t *board_get_handle(void)
{
    return &g_board_mgr;
}

static void board_set_gpio_default(gpio_dev_t *gpio_device)
{
    gpio_params_t *priv = (gpio_params_t *)gpio_device->priv;
    if (NULL == gpio_device || NULL == priv) {
        return;
    }

    gpio_device->port = -1;
    gpio_device->config = OUTPUT_PUSH_PULL;
#ifndef ESP_PLATFORM
    gpio_device->gpioc = NULL;
    gpio_device->gpioc_index = 0;
    gpio_device->pin_index = 0;
#endif
    priv->irq_mode = 0;
    priv->js_cb_ref = 0;
    priv->reserved = NULL;
}

static int8_t board_parse_gpio(cJSON *gpio, char *id)
{
    int index = 0;
    int8_t ret = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *dir = NULL;
    cJSON *pull = NULL;
    cJSON *intMode = NULL;
    gpio_dev_t device;
    gpio_params_t *priv = NULL;
    gpio_config_t *config = (gpio_config_t *)&device.config;
    int8_t size = 1;

    if (size <= 0) {
        return BOARD_ERR_SIZE_INVALID;
    }
    while (index < size) {
        if ((priv = (gpio_params_t *)aos_calloc(1, sizeof(gpio_params_t))) == NULL) {
            LOGE(LOG_TAG, "malloc failed");
            return BOARD_ERR_NO_MEM;
        }
        item = gpio;
        index += 1;
        if (NULL == item || NULL == id) {
            continue;
        }
        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }

        device.priv = priv;
        board_set_gpio_default(&device);
        dir = cJSON_GetObjectItem(item, GPIO_DIR);
        pull = cJSON_GetObjectItem(item, GPIO_PULL);
        intMode = cJSON_GetObjectItem(item, GPIO_INTMODE);
        if (NULL != dir && cJSON_String == dir->type && NULL != pull && cJSON_String == pull->type) {
            if (strcmp(dir->valuestring, GPIO_DIR_INPUT) == 0) {
                if (strcmp(GPIO_PULL_DOWN, pull->valuestring) == 0)
                    *config = INPUT_PULL_DOWN;
                else if (strcmp(GPIO_PULL_UP, pull->valuestring) == 0)
                    *config = INPUT_PULL_UP;
                else if (strcmp(GPIO_PULL_OPEN, pull->valuestring) == 0)
                    *config = INPUT_HIGH_IMPEDANCE;
            }

            else if (strcmp(dir->valuestring, GPIO_DIR_OUTPUT) == 0) {
                if (strcmp(GPIO_PULL_DOWN, pull->valuestring) == 0)
                    *config = OUTPUT_PUSH_PULL;
                else if (strcmp(GPIO_PULL_UP, pull->valuestring) == 0)
                    *config = OUTPUT_OPEN_DRAIN_PULL_UP;
                else if (strcmp(GPIO_PUSH_PULL, pull->valuestring) == 0)
                    *config = OUTPUT_PUSH_PULL;
                else if (strcmp(GPIO_PULL_OPEN, pull->valuestring) == 0)
                    *config = OUTPUT_OPEN_DRAIN_NO_PULL;
            } else if (strcmp(dir->valuestring, GPIO_DIR_IRQ) == 0) {
                *config = IRQ_MODE;

                if (strcmp(GPIO_PULL_DOWN, pull->valuestring) == 0)
                    *config = INPUT_PULL_DOWN;
                else if (strcmp(GPIO_PULL_UP, pull->valuestring) == 0)
                    *config = INPUT_PULL_UP;
                else if (strcmp(GPIO_PULL_OPEN, pull->valuestring) == 0)
                    *config = INPUT_HIGH_IMPEDANCE;
                if (strcmp(GPIO_INT_RISING, intMode->valuestring) == 0)
                    priv->irq_mode = IRQ_TRIGGER_RISING_EDGE;
                else if (strcmp(GPIO_INT_FALLING, intMode->valuestring) == 0)
                    priv->irq_mode = IRQ_TRIGGER_FALLING_EDGE;
                else if (strcmp(GPIO_INT_BOTH, intMode->valuestring) == 0)
                    priv->irq_mode = IRQ_TRIGGER_BOTH_EDGES;
                else if (strcmp(GPIO_INT_HIGH_LEVEL, intMode->valuestring) == 0)
                    priv->irq_mode = IRQ_TRIGGER_LEVEL_HIGH;
                else if (strcmp(GPIO_INT_LOW_LEVEL, intMode->valuestring) == 0)
                    priv->irq_mode = IRQ_TRIGGER_LEVEL_LOW;
            }

            else if (strcmp(dir->valuestring, GPIO_DIR_ANALOG) == 0) {
                *config = ANALOG_MODE;
            }
        }
        gpio_dev_t *new_gpio = (gpio_dev_t *)aos_calloc(1, sizeof(*new_gpio));
        if (NULL == new_gpio) {
            continue;
        }
        device.port = port->valueint;
        char *gpio_id = strdup(id);
        memcpy(new_gpio, &device, sizeof(gpio_dev_t));
        ret = board_add_new_item(MODULE_GPIO, gpio_id, new_gpio);
        if (0 == ret) {
            continue;
        }
        if (NULL != gpio_id) {
            aos_free(gpio_id);
            gpio_id = NULL;
        }
        if (NULL != new_gpio) {
            aos_free(new_gpio);
            new_gpio = NULL;
            aos_free(priv);
            priv = NULL;
        }
    }
    return BOARD_ERR_NONE;
}

static void board_set_uart_default(uart_dev_t *uart_device)
{
    if (NULL == uart_device) {
        return;
    }
    uart_device->port = 0;
    uart_device->priv = NULL;
    uart_device->config.baud_rate = 115200;
    uart_device->config.data_width = DATA_WIDTH_8BIT;
    uart_device->config.flow_control = FLOW_CONTROL_DISABLED;
    uart_device->config.parity = NO_PARITY;
    uart_device->config.stop_bits = STOP_BITS_1;
    uart_device->config.mode = MODE_TX_RX;
}

static int8_t board_parse_uart(cJSON *uart, char *id)
{
    int index = 0;
    int8_t ret = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    uart_dev_t device;
    uart_config_t *config = (uart_config_t *)&device.config;
    int8_t size = 1;

    if (size <= 0) {
        return BOARD_ERR_SIZE_INVALID;
    }
    while (index < size) {
        item = uart;
        index += 1;
        if (NULL == item) {
            continue;
        }
        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }
        board_set_uart_default(&device);
        temp = cJSON_GetObjectItem(item, UART_DATA_WIDTH);
        if (NULL != temp && cJSON_Number == temp->type) {
            int32_t width = temp->valueint;
            switch (width) {
            case 5:
                config->data_width = DATA_WIDTH_5BIT;
                break;
            case 6:
                config->data_width = DATA_WIDTH_6BIT;
                break;
            case 7:
                config->data_width = DATA_WIDTH_7BIT;
                break;
            case 8:
                config->data_width = DATA_WIDTH_8BIT;
                break;

            default:
                break;
            }
        }
        temp = cJSON_GetObjectItem(item, UART_BAUD_RATE);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->baud_rate = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, UART_STOP_BITS);
        if (NULL != temp && cJSON_Number == temp->type) {
            int32_t stopbits = temp->valueint;
            switch (stopbits) {
            case 1:
                config->stop_bits = STOP_BITS_1;
                break;
            case 2:
                config->stop_bits = STOP_BITS_2;

            default:
                break;
            }
        }
        temp = cJSON_GetObjectItem(item, UART_FLOW_CONTROL);
        if (NULL != temp && cJSON_String == temp->type) {
            if (strcmp(temp->valuestring, UART_FC_DISABLE) == 0) {
                config->flow_control = FLOW_CONTROL_DISABLED;
            }

            else if (strcmp(temp->valuestring, UART_FC_CTS) == 0) {
                config->flow_control = FLOW_CONTROL_CTS;
            }

            else if (strcmp(temp->valuestring, UART_FC_RTS) == 0) {
                config->flow_control = FLOW_CONTROL_RTS;
            }

            else if (strcmp(temp->valuestring, UART_FC_RTSCTS) == 0) {
                config->flow_control = FLOW_CONTROL_CTS_RTS;
            }
        }
        temp = cJSON_GetObjectItem(item, UART_PARITY_CONFIG);
        if (NULL != temp && cJSON_String == temp->type) {
            if (strcmp(temp->valuestring, UART_PARITY_NONE) == 0) {
                config->parity = NO_PARITY;
            }

            else if (strcmp(temp->valuestring, UART_PARITY_ODD) == 0) {
                config->parity = ODD_PARITY;
            }

            else if (strcmp(temp->valuestring, UART_PARITY_EVEN) == 0) {
                config->parity = EVEN_PARITY;
            }
        }
        uart_dev_t *new_uart = (uart_dev_t *)aos_calloc(1, sizeof(*new_uart));
        if (NULL == new_uart) {
            continue;
        }
        device.port = port->valueint;
        char *uart_id = strdup(id);
        *new_uart = device;
        ret = board_add_new_item(MODULE_UART, uart_id, new_uart);
        LOGD(LOG_TAG, "*** add item: %s", uart_id);
        if (0 == ret) {
            continue;
        }
        if (NULL != uart_id) {
            aos_free(uart_id);
            uart_id = NULL;
        }
        if (NULL != new_uart) {
            aos_free(new_uart);
            new_uart = NULL;
        }
    }

    return BOARD_ERR_NONE;
}

static void board_set_i2c_default(i2c_dev_t *i2c_device)
{
    if (NULL == i2c_device) {
        return;
    }
    i2c_device->port = 0;
    i2c_device->priv = NULL;
    i2c_device->config.address_width = 7;
    i2c_device->config.freq = 100000;
    i2c_device->config.mode = I2C_MODE_MASTER;
    i2c_device->config.dev_addr = 0xFF;
}

static int8_t board_parse_i2c(cJSON *i2c, char *id)
{
    int index = 0;
    int8_t ret = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    uint32_t timeout;
    i2c_dev_t device;
    i2c_config_t *config = (i2c_config_t *)&device.config;
    int8_t size = 1;

    if (size <= 0) {
        return BOARD_ERR_SIZE_INVALID;
    }
    while (index < size) {
        item = i2c;
        index += 1;
        if (NULL == item) {
            continue;
        }
        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }
        board_set_i2c_default(&device);

        temp = cJSON_GetObjectItem(item, I2C_ADDR_WIDTH);
        if (NULL != temp && cJSON_Number == temp->type) {
            switch (temp->valueint) {
            case 7:
                config->address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
                break;
            case 10:
                config->address_width = I2C_HAL_ADDRESS_WIDTH_10BIT;
                break;

            default:
                break;
            }
        }
        temp = cJSON_GetObjectItem(item, I2C_FREQ);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->freq = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, I2C_MODE);
        if (NULL != temp && cJSON_String == temp->type) {
            if (strcmp(temp->valuestring, I2C_MASTER) == 0) {
                config->mode = I2C_MODE_MASTER;
            } else if (strcmp(temp->valuestring, I2C_SLAVE) == 0) {
                config->mode = I2C_MODE_SLAVE;
            }
        }
        temp = cJSON_GetObjectItem(item, I2C_TIMEOUT);
        if (NULL != temp && cJSON_Number == temp->type) {
            timeout = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, I2C_ADDR_DEV);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->dev_addr = temp->valueint;
        }

        i2c_dev_t *new_i2c = (i2c_dev_t *)aos_calloc(1, sizeof(*new_i2c));
        if (NULL == new_i2c) {
            continue;
        }
        device.port = port->valueint;
        char *i2c_id = strdup(id);
        *new_i2c = device;
        ret = board_add_new_item(MODULE_I2C, i2c_id, new_i2c);
        if (0 == ret) {
            continue;
        }
        if (NULL != i2c_id) {
            aos_free(i2c_id);
            i2c_id = NULL;
        }
        if (NULL != new_i2c) {
            aos_free(new_i2c);
            new_i2c = NULL;
        }
    }

    return BOARD_ERR_NONE;
}

static void board_set_spi_default(spi_dev_t *spi_device)
{
    if (NULL == spi_device) {
        return;
    }
    spi_device->port = 0;
    spi_device->priv = NULL;
    spi_device->config.data_size = SPI_DATA_SIZE_8BIT;
    spi_device->config.mode = SPI_WORK_MODE_3;
    spi_device->config.cs = SPI_CS_DIS;
    spi_device->config.role = SPI_ROLE_MASTER;
    spi_device->config.serial_len = 0;
    spi_device->config.firstbit = SPI_FIRSTBIT_MSB;
    spi_device->config.t_mode = SPI_TRANSFER_NORMAL;
    spi_device->config.freq = 2000000;
}

static int8_t board_parse_spi(cJSON *spi, char *id)
{
    int index = 0;
    int8_t ret = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    spi_dev_t device;
    spi_config_t *config = (spi_config_t *)&device.config;
    int8_t size = 1;

    if (size <= 0) {
        return BOARD_ERR_SIZE_INVALID;
    }
    while (index < size) {
        item = spi;
        index += 1;
        if (NULL == item) {
            continue;
        }
        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }
        board_set_spi_default(&device);

        temp = cJSON_GetObjectItem(item, SPI_MODE);
        if (NULL != temp && cJSON_String == temp->type) {
            if (strcmp(temp->valuestring, SPI_MODE_MASTER) == 0) {
                config->role = HAL_SPI_MODE_MASTER;
            }

            else if (strcmp(temp->valuestring, SPI_MODE_SLAVE) == 0) {
                config->role = HAL_SPI_MODE_SLAVE;
            }
        }

        temp = cJSON_GetObjectItem(item, SPI_FREQ);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->freq = temp->valueint;
        }

        temp = cJSON_GetObjectItem(item, SPI_CPOL_CPHA);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->mode = temp->valueint;
        }

        spi_dev_t *new_spi = (spi_dev_t *)aos_calloc(1, sizeof(*new_spi));
        if (NULL == new_spi) {
            continue;
        }
        device.port = port->valueint;
        char *spi_id = strdup(id);
        *new_spi = device;
        ret = board_add_new_item(MODULE_SPI, spi_id, new_spi);
        if (0 == ret) {
            continue;
        }
        if (NULL != spi_id) {
            aos_free(spi_id);
            spi_id = NULL;
        }
        if (NULL != new_spi) {
            aos_free(new_spi);
            new_spi = NULL;
        }
    }

    return BOARD_ERR_NONE;
}

static void board_set_pwm_default(pwm_dev_t *pwm_device)
{
    if (NULL == pwm_device) {
        return;
    }
    pwm_device->port = 0;
    pwm_device->priv = NULL;
    pwm_device->config.freq = 1;
    pwm_device->config.duty_cycle = 100;
}

static int8_t board_parse_pwm(cJSON *pwm, char *id)
{
    int index = 0;
    int8_t ret = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    pwm_dev_t device;
    pwm_config_t *config = (pwm_config_t *)&device.config;
    int8_t size = 1;

    if (size <= 0) {
        return BOARD_ERR_SIZE_INVALID;
    }
    while (index < size) {
        item = pwm;
        index += 1;
        if (NULL == item) {
            continue;
        }
        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }
        board_set_pwm_default(&device);
        pwm_dev_t *new_pwm = (pwm_dev_t *)aos_calloc(1, sizeof(*new_pwm));
        if (NULL == new_pwm) {
            continue;
        }
        device.port = port->valueint;
        char *pwm_id = strdup(id);
        *new_pwm = device;
        ret = board_add_new_item(MODULE_PWM, pwm_id, new_pwm);
        if (0 == ret) {
            continue;
        }

        if (NULL != pwm_id) {
            aos_free(pwm_id);
            pwm_id = NULL;
        }
        if (NULL != new_pwm) {
            aos_free(new_pwm);
            new_pwm = NULL;
        }
    }

    return BOARD_ERR_NONE;
}

static void board_set_adc_default(adc_dev_t *adc_device)
{
    if (NULL == adc_device) {
        return;
    }
    adc_device->port = 0;
    adc_device->priv = NULL;
    adc_device->config.sampling_cycle = 12000000;
    adc_device->config.adc_atten = 0;
    adc_device->config.adc_width = 3;
}

static int8_t board_parse_adc(cJSON *adc, char *id)
{
    int index = 0;
    int8_t ret = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    adc_dev_t device;
    adc_config_t *config = (adc_config_t *)&device.config;
    int8_t size = 1;

    if (size <= 0) {
        return BOARD_ERR_SIZE_INVALID;
    }
    while (index < size) {
        item = adc;
        index += 1;
        if (NULL == item) {
            continue;
        }
        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }
        board_set_adc_default(&device);

        temp = cJSON_GetObjectItem(item, ADC_SAMPLING);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->sampling_cycle = temp->valueint;
        }

        temp = cJSON_GetObjectItem(item, ADC_ATTEN);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->adc_atten = temp->valueint;
        }

        temp = cJSON_GetObjectItem(item, ADC_WIDTH);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->adc_width = temp->valueint;
        }

        adc_dev_t *new_adc = (adc_dev_t *)aos_calloc(1, sizeof(*new_adc));
        if (NULL == new_adc) {
            continue;
        }
        device.port = port->valueint;
        char *adc_id = strdup(id);
        *new_adc = device;
        ret = board_add_new_item(MODULE_ADC, adc_id, new_adc);
        if (0 == ret) {
            continue;
        }

        if (NULL != adc_id) {
            aos_free(adc_id);
            adc_id = NULL;
        }
        if (NULL != new_adc) {
            aos_free(new_adc);
            new_adc = NULL;
        }
    }

    return BOARD_ERR_NONE;
}

static void board_set_modbus_default(modbus_dev_t *mdobus_device)
{
    if (NULL == mdobus_device) {
        return;
    }
    mdobus_device->mode = MBMODE_SERIAL;
    mdobus_device->port = MODBUS_SERIAL_PORT;
    mdobus_device->baudrate = 9600;
    mdobus_device->parity = 0;
    mdobus_device->timeout = 200;
}

static int8_t board_parse_modbus(cJSON *modbus, char *id)
{
    int32_t index = 0;
    int8_t ret = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    modbus_dev_t device;
    int8_t size = 1;

    if (size <= 0) {
        return BOARD_ERR_SIZE_INVALID;
    }
    while (index < size) {
        item = modbus;
        index += 1;
        if (NULL == item) {
            continue;
        }

        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }

        board_set_modbus_default(&device);

        temp = cJSON_GetObjectItem(item, MODBUS_MODE);
        if (NULL != temp && cJSON_Number == temp->type) {
            device.mode = temp->valueint;
        }

        if (device.mode == MBMODE_SERIAL) {
            temp = cJSON_GetObjectItem(item, MODBUS_PORT);
            if (NULL != temp && cJSON_Number == temp->type) {
                device.port = temp->valueint;
            }

            temp = cJSON_GetObjectItem(item, MODBUS_BAUDRATE);
            if (NULL != temp && cJSON_Number == temp->type) {
                device.baudrate = temp->valueint;
            }

            temp = cJSON_GetObjectItem(item, MODBUS_PARITY);
            if (NULL != temp && cJSON_Number == temp->type) {
                device.parity = temp->valueint;
            }

            temp = cJSON_GetObjectItem(item, MODBUS_TIMEOUT);
            if (NULL != temp && cJSON_Number == temp->type) {
                device.timeout = temp->valueint;
            }
        }

        modbus_dev_t *new_modbus = (modbus_dev_t *)aos_calloc(1, sizeof(*new_modbus));
        if (NULL == new_modbus) {
            continue;
        }

        const char *modbus_id = strdup(id);
        *new_modbus = device;
        ret = board_add_new_item(MODULE_MODBUS, modbus_id, new_modbus);
        if (0 == ret) {
            continue;
        }

        if (NULL != modbus_id) {
            aos_free(modbus_id);
            modbus_id = NULL;
        }

        if (NULL != new_modbus) {
            aos_free(new_modbus);
            new_modbus = NULL;
        }
    }

    return BOARD_ERR_NONE;
}

static void board_set_wdt_default(wdg_dev_t *wdt_device)
{
    if (NULL == wdt_device) {
        return;
    }
    wdt_device->config.timeout = 3;
}

static int8_t board_parse_wdt(cJSON *wdt, char *id)
{
    int32_t ret = -1;
    cJSON *temp = NULL;
    wdg_dev_t *new_wdt = NULL;
    const char *wdt_id = NULL;

    if ((NULL == wdt) || (NULL == id))
        return BOARD_ERR_INVALID_ARG;

    new_wdt = aos_malloc(sizeof(*new_wdt));
    if (NULL == new_wdt)
        return BOARD_ERR_NO_MEM;

    board_set_wdt_default(new_wdt);

    temp = cJSON_GetObjectItem(wdt, WDT_TIMEOUT);
    if (NULL != temp && cJSON_Number == temp->type)
        new_wdt->config.timeout = temp->valueint;

    wdt_id = strdup(id);
    ret = board_add_new_item(MODULE_WDT, wdt_id, new_wdt);
    if (0 == ret) {
        return BOARD_ERR_NONE;
    }

    if (NULL != wdt_id) {
        aos_free(wdt_id);
        wdt_id = NULL;
    }

    if (NULL != new_wdt) {
        aos_free(new_wdt);
        new_wdt = NULL;
    }

    return ret;
}

char *board_get_json_buff(const char *json_path)
{
    void *json_data = NULL;
    int len = 0;
    int json_fd = -1;

    if (NULL == json_path) {
        LOGE(LOG_TAG, "json_path NULL\n");
        return (NULL);
    }

    json_fd = open(json_path, O_RDONLY);
    if (json_fd < 0) {
        LOGE(LOG_TAG, "failed to open json file:%s\n", json_path);
        return (NULL);
    }

    len = lseek(json_fd, 0, SEEK_END);
    if (len < 0) {
        LOGE(LOG_TAG, "failed to seek file\n");
        return (NULL);
    }

    json_data = aos_calloc(len + 1, sizeof(char));
    if (NULL == json_data) {
        close(json_fd);
        LOGE(LOG_TAG, "failed to calloc data for json_data\n");
        return (NULL);
    }
    lseek(json_fd, 0, SEEK_SET);
    read(json_fd, json_data, len);
    close(json_fd);
    return json_data;
}

static parse_json_t g_parse_json[] = {
    { MARKER_UART, MODULE_UART, board_parse_uart },

    { MARKER_GPIO, MODULE_GPIO, board_parse_gpio },

    { MARKER_PWM, MODULE_PWM, board_parse_pwm },

    { MARKER_I2C, MODULE_I2C, board_parse_i2c },

    { MARKER_SPI, MODULE_SPI, board_parse_spi },

    { MARKER_ADC, MODULE_ADC, board_parse_adc },

    { MARKER_MODBUS, MODULE_MODBUS, board_parse_modbus },

    { MARKER_WDT, MODULE_WDT, board_parse_wdt },

    { NULL, MODULE_NUMS, NULL },
};

static int32_t board_parse_json_buff(const char *json_buff)
{
    cJSON *root = NULL;
    cJSON *page = NULL, *pages = NULL;
    cJSON *io = NULL;
    cJSON *debug = NULL;
    cJSON *repl = NULL;
    cJSON *item = NULL;
    cJSON *child = NULL;
    parse_json_t *parser_handle = NULL;

    if (NULL == json_buff) {
        LOGE(LOG_TAG, "json_buff NULL\n");
        return BOARD_ERR_INVALID_ARG;
    }
    root = cJSON_Parse(json_buff);
    if (NULL == root) {
        LOGE(LOG_TAG, "json parse fail\n");
        return BOARD_ERR_JSON_PARSE;
    }

    /* debugLevel configuration */
    if ((debug = cJSON_GetObjectItem(root, APP_CONFIG_DEBUG)) != NULL) {
        /* parsing debugLevel configuration */
        if (!cJSON_IsString(debug)) {
            LOGE(LOG_TAG, "debugLevel not string");
            goto parse_end;
        }

        LOGD(LOG_TAG, "get debugLevel:%s\n", debug->valuestring);
        if (strcmp(debug->valuestring, "DEBUG") == 0) {
            aos_set_log_level(LOG_DEBUG);
        } else if (strcmp(debug->valuestring, "INFO") == 0) {
            aos_set_log_level(LOG_INFO);
        } else if (strcmp(debug->valuestring, "WARN") == 0) {
            aos_set_log_level(LOG_WARNING);
        } else if (strcmp(debug->valuestring, "ERROR") == 0) {
            aos_set_log_level(LOG_ERR);
        } else if (strcmp(debug->valuestring, "FATAL") == 0) {
            aos_set_log_level(LOG_CRIT);
        } else {
            LOGD(LOG_TAG, "debugLevel error, set to default: 'ERROR'");
            aos_set_log_level(LOG_ERR);
        }
    } else {
        LOGD(LOG_TAG, "No debugLevel configuration in app.json, set to default: 'ERROR'");
    }

    /* page configuration */
    if ((pages = cJSON_GetObjectItem(root, APP_CONFIG_PAGES)) != NULL) {
        /* parsing io configuration */
        if (!cJSON_IsArray(pages)) {
            LOGE(LOG_TAG, "Pages entries need array");
            goto parse_end;
        }

        dlist_init(&g_pages_list);
        cJSON_ArrayForEach(page, pages)
        {
            if (!cJSON_IsString(page)) {
                LOGE(LOG_TAG, "page not string");
                goto parse_end;
            }

            LOGD(LOG_TAG, "get page:%s", page->valuestring);

            /* add page to dlink */
            page_entry_t *page_entry = (page_entry_t *)aos_malloc(sizeof(page_entry_t));
            page_entry->page = strdup(page->valuestring); /* don't forget to free */
            dlist_add_tail(&page_entry->node, &g_pages_list);
        }
    } else {
        LOGD(LOG_TAG, "No pages configuration in app.json");
    }

    /* repl configuration */
    if ((repl = cJSON_GetObjectItem(root, APP_CONFIG_REPL)) != NULL) {
        /* parsing debugLevel configuration */
        if (!cJSON_IsString(repl)) {
            LOGE(LOG_TAG, "repl not string");
            goto parse_end;
        }

        LOGD(LOG_TAG, "get app repl config is:%s", repl->valuestring);
        if (strcmp(repl->valuestring, "disable") == 0) {
            py_repl_config = 0;
        } else if (strcmp(repl->valuestring, "enable") == 0) {
            py_repl_config = 1;
        } else {
            LOGD(LOG_TAG, "repl configuration is wrong, set to default: 'enable'");
            py_repl_config = 1;
        }
    } else {
        LOGD(LOG_TAG, "No repl configuration in app.json, set to default: 'enable'");
    }

    /* net configuration */
    /* TODO */

    /* io configuration */
    if ((io = cJSON_GetObjectItem(root, APP_CONFIG_IO)) != NULL) {
        /* parsing io configuration */
        child = io->child;
        while (NULL != child) {
            item = cJSON_GetObjectItem(child, MARKER_ID);
            if (NULL == item || cJSON_String != item->type) {
                child = child->next;
                continue;
            }
            parser_handle = &g_parse_json[0];
            while (NULL != parser_handle->marker_name) {
                if (0 == strcmp(item->valuestring, parser_handle->marker_name)) {
                    parser_handle->fn(child, child->string);
                }
                parser_handle += 1;
            }
            child = child->next;
        }
    } else {
        LOGE(LOG_TAG, "No io configuration in app.json");
    }

    cJSON_Delete(root);
    return BOARD_ERR_NONE;

parse_end:
    cJSON_Delete(root);
    return BOARD_ERR_JSON_PARSE;
}

static void *board_get_items(addon_module_m module, item_handle_t *handle, const char *name_id)
{
    board_mgr_t *mgr_handle = board_get_handle();
    board_item_t *item = NULL;
    if (NULL == handle && NULL == name_id) {
        LOGE(LOG_TAG, "handle or name_id null\n");
        return (NULL);
    }
    uint32_t i = 0;
    for (i = 0; i < mgr_handle->item_size; ++i) {
        item = mgr_handle->item[i];
        if (module != item->module) {
            continue;
        }
        if (NULL != handle && item->handle.handle != handle->handle) {
            continue;
        }
        if (NULL != name_id && 0 != strcmp(item->name_id, name_id)) {
            continue;
        }
        return (item);
    }

    return (NULL);
}

static int8_t board_add_new_item(addon_module_m module, char *name_id, void *node)
{
    board_mgr_t *mgr_handle = board_get_handle();
    if (NULL == name_id || NULL == node) {
        return BOARD_ERR_INVALID_ARG;
    }
    if (NULL != board_get_items(module, NULL, name_id)) {
        LOGE(LOG_TAG, "board_get_items failed, name_id = %s\n", name_id);
        return BOARD_ERR_NODE_NOT_EXIST;
    }
    board_item_t *new_item = (board_item_t *)aos_calloc(1, sizeof(*new_item));
    if (NULL == new_item) {
        return BOARD_ERR_NO_MEM;
    }
    void *addr = aos_realloc(mgr_handle->item, sizeof(board_item_t *) * (mgr_handle->item_size + 1));
    if (NULL == addr) {
        goto out;
    }

    new_item->module = module;
    new_item->name_id = name_id;
    new_item->handle.handle = (void *)new_item;
    new_item->node = node;
    new_item->status = 0;
    mgr_handle->item = addr;
    mgr_handle->item[mgr_handle->item_size] = new_item;
    mgr_handle->item_size += 1;

    return BOARD_ERR_NONE;
out:
    if (NULL != new_item) {
        aos_free(new_item);
        new_item = NULL;
    }
    return BOARD_ERR_NO_MEM;
}

int8_t py_board_attach_item(addon_module_m module, const char *name_id, item_handle_t *out)
{
    board_item_t *item = NULL;
    if (NULL == name_id) {
        return BOARD_ERR_INVALID_ARG;
    }
    item = board_get_items(module, NULL, name_id);
    if (NULL == item) {
        LOGE(LOG_TAG, "board_get_items failed, name_id = %s\n", name_id);
        return BOARD_ERR_NODE_NOT_EXIST;
    }
    item->status = 1;
    *out = item->handle;

    return BOARD_ERR_NONE;
}

int8_t py_board_disattach_item(addon_module_m module, item_handle_t *handle)
{
    board_item_t *item = NULL;
    if (NULL == handle) {
        return BOARD_ERR_INVALID_ARG;
    }
    item = board_get_items(module, handle, NULL);
    if (NULL == item) {
        return BOARD_ERR_NODE_NOT_EXIST;
    }
    item->status = 0;

    return BOARD_ERR_NONE;
}

int8_t py_board_check_attach_status(addon_module_m module, item_handle_t *handle)
{
    board_item_t *item = NULL;
    if (NULL == handle) {
        return BOARD_ERR_NONE;
    }
    item = board_get_items(module, handle, NULL);
    if (NULL == item) {
        return BOARD_ERR_NONE;
    }

    return item->status;
}

void *py_board_get_node_by_name(addon_module_m module, const char *name_id)
{
    board_item_t *item = NULL;
    if (NULL == name_id) {
        return NULL;
    }
    item = board_get_items(module, NULL, name_id);
    if (NULL == item || 0 == item->status) {
        LOGE(LOG_TAG, "board_get_items failed, name_id = %s\n", name_id);
        return NULL;
    }

    return item->node;
}

void *py_board_get_node_by_handle(addon_module_m module, item_handle_t *handle)
{
    board_item_t *item = NULL;
    if (NULL == handle)
        return NULL;
    item = board_get_items(module, handle, NULL);
    if (NULL == item || 0 == item->status) {
        return NULL;
    }

    return item->node;
}

static bool board_init_flag = false;
int32_t py_board_mgr_init()
{
    int32_t ret = -1;
    char *json = NULL;
    int json_fd = -1;
    char *board_json_path = NULL;
    char *data_root_path = AMP_FS_ROOT_DIR "/board.json";
    char *sdcard_root_path = AMP_FS_EXT_ROOT_DIR "/board.json";
    char *data_board_json_path = MP_FS_ROOT_DIR "/lib/board.json";
    char *sdcard_board_json_path = MP_FS_EXT_ROOT_DIR "/lib/board.json";

    if (board_init_flag != false) {
        LOGD(LOG_TAG, "board config haas been loaded\n");
        return BOARD_ERR_NONE;
    }

    memset(&g_board_mgr, 0x00, sizeof(g_board_mgr));
    if (board_json_path == NULL) {
        json_fd = open(data_root_path, O_RDONLY);
        if (json_fd >= 0) {
            close(json_fd);
            board_json_path = data_root_path;
        }
    }

    json_fd = open(sdcard_root_path, O_RDONLY);
    if (json_fd >= 0) {
        close(json_fd);
        board_json_path = sdcard_root_path;
    }

    if (board_json_path == NULL) {
        json_fd = open(data_board_json_path, O_RDONLY);
        if (json_fd >= 0) {
            close(json_fd);
            board_json_path = data_board_json_path;
        }
    }

    if (board_json_path == NULL) {
        board_json_path = sdcard_board_json_path;
    }

    LOGD(LOG_TAG, "board_json_path = %s;\n", board_json_path);
    json = board_get_json_buff(board_json_path);

    if (NULL == json) {
        LOGD(LOG_TAG, "default board config is null");
        return ret;
    }
    // return BOARD_ERR_NONE;
    ret = board_parse_json_buff(json);
    aos_free(json);
    json = NULL;

    if (0 == ret) {
        board_init_flag = true;
    }
    return ret;
}
