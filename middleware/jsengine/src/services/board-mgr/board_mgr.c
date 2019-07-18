/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include "board_mgr.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_port_hal.h"
#include "board_marker.h"
#include "cJSON.h"
#include "default_config.h"

#define DRIVER_DIR BE_FS_ROOT_DIR "/drivers/"

#define DRIVER_NAME "driver.json"

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

static board_mgr_t g_board_mgr = {0, NULL};

static int8_t board_add_new_item(addon_module_m module, char *name_id,
                                 void *node);

static board_mgr_t *board_get_handle(void)
{
    return &g_board_mgr;
}

#ifdef JSE_HW_ADDON_GPIO

static void board_set_gpio_default(gpio_dev_t *gpio_device)
{
    if (NULL == gpio_device) {
        return;
    }
    gpio_device->port   = -1;
    gpio_device->priv   = NULL;
    gpio_device->config = OUTPUT_PUSH_PULL;
}

static int8_t board_parse_gpio(cJSON *gpio, char *id)
{
    int index   = 0;
    int8_t ret  = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *dir  = NULL;
    cJSON *pull = NULL;
    gpio_dev_t device;
    gpio_config_t *config = (gpio_config_t *)&device.config;
    int8_t size           = 1;

    if (size <= 0) {
        return (-1);
    }
    while (index < size) {
        item = gpio;
        index += 1;
        if (NULL == item || NULL == id) {
            continue;
        }

        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }
        board_set_gpio_default(&device);

        dir  = cJSON_GetObjectItem(item, GPIO_DIR);
        pull = cJSON_GetObjectItem(item, GPIO_PULL);

        if (NULL != dir && cJSON_Number == dir->type && NULL != pull &&
            cJSON_Number == pull->type) {
            switch (dir->valueint) {
                case GPIO_DIR_INPUT: {
                    if (GPIO_PULL_DOWN == pull->valueint)
                        *config = INPUT_PULL_DOWN;
                    if (GPIO_PULL_UP == pull->valueint) *config = INPUT_PULL_UP;
                    if (GPIO_PULL_OPEN == pull->valueint)
                        *config = INPUT_HIGH_IMPEDANCE;
                    break;
                }
                case GPIO_DIR_OUTPUT: {
                    if (GPIO_PULL_DOWN == pull->valueint)
                        *config = OUTPUT_PUSH_PULL;
                    if (GPIO_PULL_UP == pull->valueint)
                        *config = OUTPUT_OPEN_DRAIN_PULL_UP;
                    if (GPIO_PULL_OPEN == pull->valueint)
                        *config = OUTPUT_OPEN_DRAIN_NO_PULL;
                    break;
                }
                case GPIO_DIR_IRQ: {
                    *config = IRQ_MODE;
                    break;
                }
                case GPIO_DIR_ANALOG: {
                    *config = ANALOG_MODE;
                    break;
                }
            }
        }
        gpio_dev_t *new_gpio = calloc(1, sizeof(*new_gpio));
        if (NULL == new_gpio) {
            continue;
        }
        device.port = port->valueint;

        char *gpio_id = strdup(id);
        *new_gpio     = device;
        ret           = board_add_new_item(MODULE_GPIO, gpio_id, new_gpio);
        if (0 == ret) {
            continue;
        }
        if (NULL != gpio_id) {
            free(gpio_id);
            gpio_id = NULL;
        }
        if (NULL != new_gpio) {
            free(new_gpio);
            new_gpio = NULL;
        }
    }

    return (0);
}
#endif

#ifdef JSE_HW_ADDON_UART
static void board_set_uart_default(uart_dev_t *uart_device)
{
    if (NULL == uart_device) {
        return;
    }
    uart_device->port                = 0;
    uart_device->priv                = NULL;
    uart_device->config.baud_rate    = 115200;
    uart_device->config.data_width   = DATA_WIDTH_8BIT;
    uart_device->config.flow_control = FLOW_CONTROL_DISABLED;
    uart_device->config.parity       = NO_PARITY;
    uart_device->config.stop_bits    = STOP_BITS_1;
    uart_device->config.mode         = MODE_TX_RX;
}

static int8_t board_parse_uart(cJSON *uart, char *id)
{
    int index   = 0;
    int8_t ret  = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    uart_dev_t device;
    uart_config_t *config = (uart_config_t *)&device.config;
    int8_t size           = 1;

    if (size <= 0) {
        return (-1);
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
            if (width >= 0 && width < 5) {
                config->data_width = width;
            }
        }
        temp = cJSON_GetObjectItem(item, UART_BAUD_RATE);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->baud_rate = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, UART_STOP_BITS);
        if (NULL != temp && cJSON_Number == temp->type) {
            int32_t stopbits = temp->valueint - 1;
            if (0 == stopbits || 1 == stopbits) {
                config->stop_bits = stopbits;
            }
        }
        temp = cJSON_GetObjectItem(item, UART_FLOW_CONTROL);
        if (NULL != temp && cJSON_Number == temp->type) {
            int32_t flow_control = temp->valueint;
            if (flow_control >= 0 && flow_control < 4) {
                config->flow_control = flow_control;
            }
        }
        temp = cJSON_GetObjectItem(item, UART_PARITY_CONFIG);
        if (NULL != temp && cJSON_Number == temp->type) {
            int32_t parity = temp->valueint;
            if (parity >= 0 && parity < 3) {
                config->flow_control = parity;
            }
        }
        temp = cJSON_GetObjectItem(item, UART_MODE);
        if (NULL != temp && cJSON_Number == temp->type) {
            int32_t mode = temp->valueint;
            if (mode >= 0 && mode < 3) {
                config->mode = mode;
            }
        }
        uart_dev_t *new_uart = calloc(1, sizeof(*new_uart));
        if (NULL == new_uart) {
            continue;
        }
        device.port   = port->valueint;
        char *uart_id = strdup(id);
        *new_uart     = device;
        ret           = board_add_new_item(MODULE_UART, uart_id, new_uart);
        if (0 == ret) {
            continue;
        }
        if (NULL != uart_id) {
            free(uart_id);
            uart_id = NULL;
        }
        if (NULL != new_uart) {
            free(new_uart);
            new_uart = NULL;
        }
    }

    return (0);
}
#endif

#ifdef JSE_HW_ADDON_I2C
static void board_set_i2c_default(i2c_dev_t *i2c_device)
{
    if (NULL == i2c_device) {
        return;
    }
    i2c_device->port                 = 0;
    i2c_device->priv                 = NULL;
    i2c_device->config.address_width = 7;
    i2c_device->config.freq          = 100000;
    i2c_device->config.mode          = I2C_MODE_MASTER;
    i2c_device->config.dev_addr      = 0xFF;
}

static int8_t board_parse_i2c(cJSON *i2c, char *id)
{
    int index   = 0;
    int8_t ret  = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    i2c_dev_t device;
    i2c_config_t *config = (i2c_config_t *)&device.config;
    int8_t size          = 1;

    if (size <= 0) {
        return (-1);
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
            config->address_width = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, I2C_FREQ);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->freq = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, I2C_MODE);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->mode = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, I2C_ADDR_DEV);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->dev_addr = temp->valueint;
        }

        i2c_dev_t *new_i2c = calloc(1, sizeof(*new_i2c));
        if (NULL == new_i2c) {
            continue;
        }
        device.port  = port->valueint;
        char *i2c_id = strdup(id);
        *new_i2c     = device;
        ret          = board_add_new_item(MODULE_I2C, i2c_id, new_i2c);
        if (0 == ret) {
            continue;
        }
        if (NULL != i2c_id) {
            free(i2c_id);
            i2c_id = NULL;
        }
        if (NULL != new_i2c) {
            free(new_i2c);
            new_i2c = NULL;
        }
    }

    return (0);
}
#endif

#ifdef JSE_HW_ADDON_PWM
static void board_set_pwm_default(pwm_dev_t *pwm_device)
{
    if (NULL == pwm_device) {
        return;
    }
    pwm_device->port              = 0;
    pwm_device->priv              = NULL;
    pwm_device->config.freq       = 0;
    pwm_device->config.duty_cycle = 100;
}

static int8_t board_parse_pwm(cJSON *pwm, char *id)
{
    int index   = 0;
    int8_t ret  = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    pwm_dev_t device;
    pwm_config_t *config = (pwm_config_t *)&device.config;
    int8_t size          = 1;

    if (size <= 0) {
        return (-1);
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
        temp = cJSON_GetObjectItem(item, PWM_FREQ);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->freq = temp->valueint;
        }

        temp = cJSON_GetObjectItem(item, PWM_DUTY);
        if (NULL != temp && cJSON_Number == temp->type) {
            config->duty_cycle = temp->valueint / 100.0;
        }
        pwm_dev_t *new_pwm = calloc(1, sizeof(*new_pwm));
        if (NULL == new_pwm) {
            continue;
        }
        device.port  = port->valueint;
        char *pwm_id = strdup(id);
        *new_pwm     = device;
        ret          = board_add_new_item(MODULE_PWM, pwm_id, new_pwm);
        if (0 == ret) {
            continue;
        }

        if (NULL != pwm_id) {
            free(pwm_id);
            pwm_id = NULL;
        }
        if (NULL != new_pwm) {
            free(new_pwm);
            new_pwm = NULL;
        }
    }

    return (0);
}
#endif

#ifdef JSE_HW_ADDON_ADC
static void board_set_adc_default(adc_dev_t *adc_device)
{
    if (NULL == adc_device) {
        return;
    }
    adc_device->port = 0;
    /* adc_device->chan                  = 0; */
    adc_device->priv                  = NULL;
    adc_device->config.sampling_cycle = 12000000;
}

static int8_t board_parse_adc(cJSON *adc, char *id)
{
    int index   = 0;
    int8_t ret  = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    adc_dev_t device;
    adc_config_t *config = (adc_config_t *)&device.config;
    int8_t size          = 1;

    if (size <= 0) {
        return (-1);
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

        /* temp = cJSON_GetObjectItem(item, ADC_CHAN);
        if (NULL != temp && cJSON_Number == temp->type) {
            device.chan = temp->valueint;
        } */

        adc_dev_t *new_adc = calloc(1, sizeof(*new_adc));
        if (NULL == new_adc) {
            continue;
        }
        device.port  = port->valueint;
        char *adc_id = strdup(id);
        *new_adc     = device;
        ret          = board_add_new_item(MODULE_ADC, adc_id, new_adc);
        if (0 == ret) {
            continue;
        }

        if (NULL != adc_id) {
            free(adc_id);
            adc_id = NULL;
        }
        if (NULL != new_adc) {
            free(new_adc);
            new_adc = NULL;
        }
    }

    return (0);
}
#endif

#ifdef JSE_HW_ADDON_DAC
static void board_set_dac_default(dac_dev_t *dac_device)
{
    if (NULL == dac_device) {
        return;
    }

    dac_device->port = 0;
    dac_device->priv = NULL;
}

static int8_t board_parse_dac(cJSON *dac, char *id)
{
    int index   = 0;
    int8_t ret  = -1;
    cJSON *port = NULL;
    cJSON *item = NULL;
    cJSON *temp = NULL;
    dac_dev_t device;
    int8_t size = 1;

    if (size <= 0) {
        return (-1);
    }
    while (index < size) {
        item = dac;
        index += 1;
        if (NULL == item) {
            continue;
        }
        port = cJSON_GetObjectItem(item, MARKER_PORT);
        if (NULL == port || cJSON_Number != port->type) {
            continue;
        }
        board_set_dac_default(&device);
        temp = cJSON_GetObjectItem(item, DAC_VOLTAGE);
        if (NULL != temp && cJSON_Number == temp->type) {
            /* config->output_voltage = temp->valueint; */
        }
        dac_dev_t *new_dac = calloc(1, sizeof(*new_dac));
        if (NULL == new_dac) {
            continue;
        }
        device.port  = port->valueint;
        char *dac_id = strdup(id);
        *new_dac     = device;
        ret          = board_add_new_item(MODULE_DAC, dac_id, new_dac);
        if (0 == ret) {
            continue;
        }

        if (NULL != dac_id) {
            free(dac_id);
            dac_id = NULL;
        }
        if (NULL != new_dac) {
            free(new_dac);
            new_dac = NULL;
        }
    }

    return (0);
}
#endif

static char *board_get_json_buff(const char *json_path)
{
    void *json_data = NULL;
    uint32_t len    = 0;
    int32_t json_fd = -1;
    if (NULL == json_path) {
        return (NULL);
    }
    if ((json_fd = be_open(json_path, O_RDONLY)) < 0) {
        return (NULL);
    }
    len       = be_lseek(json_fd, 0, SEEK_END);
    json_data = calloc(1, sizeof(char) * (len + 1));
    if (NULL == json_data) {
        be_close(json_fd);
        json_fd = -1;
        return (NULL);
    }
    be_lseek(json_fd, 0, SEEK_SET);
    be_read(json_fd, json_data, len);
    be_close(json_fd);
    return json_data;
}

static parse_json_t g_parse_json[] = {
#ifdef JSE_HW_ADDON_UART
    {MARKER_UART, MODULE_UART, board_parse_uart},
#endif

#ifdef JSE_HW_ADDON_GPIO
    {MARKER_GPIO, MODULE_GPIO, board_parse_gpio},
#endif

#ifdef JSE_HW_ADDON_PWM
    {MARKER_PWM, MODULE_PWM, board_parse_pwm},
#endif

#ifdef JSE_HW_ADDON_I2C
    {MARKER_I2C, MODULE_I2C, board_parse_i2c},
#endif

#ifdef JSE_HW_ADDON_ADC
    {MARKER_ADC, MODULE_ADC, board_parse_adc},
#endif

#ifdef JSE_HW_ADDON_DAC
    {MARKER_DAC, MODULE_DAC, board_parse_dac},
#endif
    {NULL, MODULE_NUMS, NULL},
};

static const int8_t board_parse_json_buff(const char *json_buff)
{
    cJSON *root                 = NULL;
    cJSON *item                 = NULL;
    cJSON *child                = NULL;
    parse_json_t *parser_handle = NULL;

    if (NULL == json_buff) {
        return (-1);
    }
    root = cJSON_Parse(json_buff);
    if (NULL == root) {
        return (-1);
    }

    child = root->child;
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

    cJSON_Delete(root);

    return (0);
}

static void *board_get_items(addon_module_m module, item_handle_t *handle,
                             const char *name_id)
{
    board_mgr_t *mgr_handle = board_get_handle();
    board_item_t *item      = NULL;
    if (NULL == handle && NULL == name_id) {
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

static int8_t board_add_new_item(addon_module_m module, char *name_id,
                                 void *node)
{
    board_item_t *item      = NULL;
    board_mgr_t *mgr_handle = board_get_handle();
    if (NULL == name_id || NULL == node) return (-1);
    if (NULL != board_get_items(module, NULL, name_id)) {
        return (-1);
    }
    board_item_t *new_item = calloc(1, sizeof(*new_item));
    if (NULL == new_item) {
        return (-1);
    }
    void *addr = realloc(mgr_handle->item,
                         sizeof(board_item_t *) * (mgr_handle->item_size + 1));
    if (NULL == addr) {
        goto out;
    }

    new_item->module                        = module;
    new_item->name_id                       = name_id;
    new_item->handle.handle                 = (uint32_t)new_item;
    new_item->node                          = node;
    new_item->status                        = 0;
    mgr_handle->item                        = addr;
    mgr_handle->item[mgr_handle->item_size] = new_item;
    mgr_handle->item_size += 1;

    return (0);
out:
    if (NULL != new_item) {
        free(new_item);
        new_item = NULL;
    }
    return (-1);
}

int8_t board_attach_item(addon_module_m module, const char *name_id,
                         item_handle_t *out)
{
    board_item_t *item = NULL;
    if (NULL == name_id) {
        return (-1);
    }
    item = board_get_items(module, NULL, name_id);
    if (NULL == item || 1 == item->status) {
        return (-1);
    }
    item->status = 1;
    *out         = item->handle;

    return (0);
}

int8_t board_disattach_item(addon_module_m module, item_handle_t *handle)
{
    board_item_t *item = NULL;
    if (NULL == handle) {
        return (-1);
    }
    item = board_get_items(module, handle, NULL);
    if (NULL == item) {
        return (-1);
    }
    item->status = 0;

    return (0);
}

int8_t board_check_attach_status(addon_module_m module, item_handle_t *handle)
{
    board_item_t *item = NULL;
    if (NULL == handle) {
        return (0);
    }
    item = board_get_items(module, handle, NULL);
    if (NULL == item) {
        return (0);
    }

    return (item->status);
}

void *board_get_node_by_name(addon_module_m module, const char *name_id)
{
    board_item_t *item = NULL;
    if (NULL == name_id) {
        return (NULL);
    }
    item = board_get_items(module, NULL, name_id);
    if (NULL == item || 0 == item->status) {
        return (NULL);
    }

    return (item->node);
}

void *board_get_node_by_handle(addon_module_m module, item_handle_t *handle)
{
    board_item_t *item = NULL;
    if (NULL == handle) return (NULL);
    item = board_get_items(module, handle, NULL);
    if (NULL == item || 0 == item->status) {
        return (NULL);
    }

    return (item->node);
}

int8_t board_mgr_init(const char *json_path)
{
    int8_t ret = -1;
    char *json = NULL;

    memset(&g_board_mgr, 0x00, sizeof(g_board_mgr));
    json = board_get_json_buff(json_path);
    if (NULL == json) {
        return (-1);
    }
    ret = board_parse_json_buff(json);
    free(json);
    json = NULL;

    return (ret);
}

int8_t board_load_drivers(const char *driver)
{
    char *p          = driver;
    char *index      = NULL;
    char *json       = NULL;
    char *new_driver = NULL;
    int32_t len      = -1;
    int8_t ret       = -1;
    if (NULL == driver) {
        return (-1);
    }
    len = strlen(driver);
    if (len < 8) {
        return (-1);
    }
    if (0 != strncmp(driver + len - 3, ".js", 3)) {
        return (-1);
    }
    p     = p + strlen("/spiffs/");
    index = driver + len - 1;
    while (index > p) {
        if (*index == '/') {
            break;
        }
        index -= 1;
    }
    if (index <= p) {
        return (-1);
    }
    new_driver = calloc(1, sizeof(char) * (index - driver + 16));
    if (NULL == new_driver) {
        return (-1);
    }
    memmove(new_driver, driver, index - driver + 1);
    memmove(new_driver + (index - driver + 1), DRIVER_NAME,
            strlen(DRIVER_NAME));

    printf("%s%d, new_driver = %s \n", __FUNCTION__, __LINE__, new_driver);

    json = board_get_json_buff(new_driver);
    if (NULL == json) {
        goto out;
    }
    ret = board_parse_json_buff(json);
out:
    if (NULL != new_driver) {
        free(new_driver);
        new_driver = NULL;
    }
    if (NULL != json) {
        free(json);
        json = NULL;
    }
    return (ret);
}
