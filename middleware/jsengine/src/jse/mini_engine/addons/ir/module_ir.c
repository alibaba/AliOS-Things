/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "be_jse_module.h"
#include "be_jse_task.h"
#include "be_port_hal.h"
#include "be_port_osal.h"
#include "board-mgr/board_mgr.h"

typedef struct ir_notify {
    be_jse_symbol_t *fun;
    uint32_t value;
} ir_notify_t;

static void ir_learn_mode(uint32_t scl_pin, uint32_t sda_pin) {
    gpio_i2c_reset(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);

    gpio_i2c_set_low(scl_pin);
    gpio_i2c_delay_10us(8);
    gpio_i2c_set_high(scl_pin);

    be_osal_delay(20);
    gpio_i2c_start(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);

    gpio_i2c_write_byte(scl_pin, sda_pin, 0x30);
    gpio_i2c_delay_10us(4);

    gpio_i2c_write_byte(scl_pin, sda_pin, 0x20);
    gpio_i2c_delay_10us(4);

    gpio_i2c_write_byte(scl_pin, sda_pin, 0x50);
    gpio_i2c_delay_10us(8);

    gpio_i2c_stop(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);

    gpio_i2c_reset(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
}

static int8_t ir_learn_read(uint32_t scl_pin, uint32_t sda_pin, uint8_t *buff) {
    uint8_t value    = 0;
    uint8_t i        = 0;
    uint8_t checksum = 0;
    gpio_i2c_reset(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    gpio_i2c_set_low(scl_pin);
    gpio_i2c_delay_10us(8);
    gpio_i2c_set_high(scl_pin);
    be_osal_delay(20);

    gpio_i2c_start(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    gpio_i2c_write_byte(scl_pin, sda_pin, 0x30);
    gpio_i2c_delay_10us(4);
    gpio_i2c_write_byte(scl_pin, sda_pin, 0x62);
    gpio_i2c_delay_10us(4);

    gpio_i2c_start(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    gpio_i2c_write_byte(scl_pin, sda_pin, 0x31);
    gpio_i2c_delay_10us(4);
    value = gpio_i2c_read_byte(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    if (0x00 != value) {
        gpio_i2c_stop(scl_pin, sda_pin);
        gpio_i2c_delay_10us(4);
        gpio_i2c_reset(scl_pin, sda_pin);
        gpio_i2c_delay_10us(4);
        return (-1);
    }
    buff[i]  = value;
    checksum = 0xc3;
    for (i = 1; i < 230; i++) {
        value = gpio_i2c_read_byte(scl_pin, sda_pin);
        gpio_i2c_delay_10us(4);
        buff[i] = value;
        checksum += value;
    }
    value = gpio_i2c_read_byte(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    gpio_i2c_stop(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    gpio_i2c_reset(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    return (0);
}

static int32_t ir_learn_start(uint32_t scl_pin, uint32_t sda_pin,
                              uint32_t busy_bin, uint8_t buff[232]) {
    uint8_t sumValue = 0;
    int32_t count    = 0;
    int8_t ret       = -1;
    uint8_t i        = 0;
    uint8_t tmp[512] = {0x00};
    gpio_i2c_init(scl_pin, sda_pin);
    gpio_i2c_set_in(busy_bin);
    ir_learn_mode(scl_pin, sda_pin);
    be_osal_delay(50);
    while (!gpio_i2c_read_pin(busy_bin)) {
        gpio_i2c_delay_10us(10);
    }
    ret = ir_learn_read(scl_pin, sda_pin, tmp);
    if (0 != ret) {
        return (-1);
    }
    buff[0] = 0x30;
    sumValue += buff[0];
    buff[1] = 0x03;
    sumValue += buff[1];
    for (i = 1; i < 231; i++) {
        buff[i + 1] = tmp[i];
        sumValue += tmp[i];
    }
    buff[231] = sumValue;
    return (232);
}

static uint32_t ir_counts(gpio_dev_t *gpio, uint8_t level) {
    int8_t ret      = 0;
    uint32_t value  = 0;
    uint32_t counts = 0;
    do {
        ret = hal_gpio_input_get(gpio, &value);
        counts += 1;
        be_osal_delay10us();
    } while ((0 == ret) && (value == level));

    return (counts);
}

static uint32_t ir_nec(gpio_dev_t *gpio) {
    uint32_t counts = 0;
    uint32_t value  = 0;
    uint8_t i       = 0;
    uint8_t j       = 0;
    /*9ms*/
    counts = ir_counts(gpio, 0);
    if (counts < 850 || counts > 950) {
        return (0);
    }
    /*4.5ms*/
    counts = ir_counts(gpio, 1);
    if (counts < 400 || counts > 500) {
        return (0);
    }
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 8; ++j) {
            value <<= 1;
            counts = ir_counts(gpio, 0);
            if (counts < 30 || counts > 100) {
                return 0;
            }
            counts = ir_counts(gpio, 1);
            if (counts > 130 && counts < 200) {
                value |= 1;
            } else if (counts < 30 || counts > 100) {
                return 0;
            }
        }
    }
    return (value);
}

static be_jse_symbol_t *ir_open(void) {
    int32_t len   = -1;
    char *data    = NULL;
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t gpio_handle;
    gpio_handle.handle      = 0xFFFFFFFF;
    be_jse_symbol_t *arg0   = NULL;
    gpio_dev_t *gpio_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_string(arg0)) {
        goto out;
    }

    len  = symbol_str_len(arg0);
    data = calloc(1, sizeof(char) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    symbol_to_str(arg0, data, len);

    ret = board_attach_item(MODULE_GPIO, data, &gpio_handle);
    if (0 != ret) {
        be_error("ir", "board_attach_item fail!\n");
        goto out;
    }
    be_debug("ir", "ir handle:%u\n", gpio_handle.handle);

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_gpio_init(gpio_device);
    if (0 != ret) {
        be_error("ir", "hal_gpio_init fail!\n");
        goto out;
    }
    symbol_unlock(gpio_device->priv);
    gpio_device->priv = NULL;
    result            = 0;

out:

    if (NULL != data) {
        free(data);
        data = NULL;
    }
    symbol_unlock(arg0);
    if (0 != result) {
        board_disattach_item(MODULE_GPIO, &gpio_handle);
        return new_int_symbol(-1);
    }
    return new_int_symbol(gpio_handle.handle);
}

static be_jse_symbol_t *ir_close(void) {
    int8_t result = -1;
    item_handle_t gpio_handle;
    be_jse_symbol_t *arg0   = NULL;
    gpio_dev_t *gpio_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    gpio_handle.handle = get_symbol_value_int(arg0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }
    hal_gpio_disable_irq(gpio_device);
    symbol_unlock(gpio_device->priv);
    gpio_device->priv = NULL;
    board_disattach_item(MODULE_GPIO, &gpio_handle);
    result = 0;
out:
    symbol_unlock(arg0);
    return new_int_symbol(result);
}

static void ir_notify(void *arg) {
    ir_notify_t *msg = (ir_notify_t *)arg;
    if (NULL == msg) {
        return;
    }
    be_jse_symbol_t *argv[1];
    argv[0] = new_int_symbol(msg->value);
    be_jse_execute_func(bone_engine_get_executor(), msg->fun, 1, argv);
    free(msg);
    msg = NULL;
}

static void ir_handle(void *arg) {
    be_jse_symbol_t *fun_symbol = NULL;
    uint32_t value              = 0;
    gpio_dev_t *gpio            = (gpio_dev_t *)arg;

    if (NULL == gpio) {
        be_error("ir", "param error!\n");
        return;
    }
    value = ir_nec(gpio);
    if (0x00 == value) {
        return;
    }
    fun_symbol = (be_jse_symbol_t *)gpio->priv;
    if (!fun_symbol || !symbol_is_function(fun_symbol)) {
        be_error("ir", "fun_symbol error!\n");
        return;
    }
    ir_notify_t *msg = malloc(sizeof(*msg));
    if (NULL == msg) {
        return;
    }
    msg->value = value & 0xFFFF;
    msg->fun   = fun_symbol;
    be_jse_task_schedule_call(ir_notify, msg);
}

static be_jse_symbol_t *ir_on(void) {
    int32_t ret   = -1;
    int32_t len   = -1;
    int8_t result = -1;
    item_handle_t gpio_handle;
    gpio_handle.handle      = 0xFFFFFFFF;
    be_jse_symbol_t *arg0   = NULL;
    be_jse_symbol_t *arg1   = NULL;
    gpio_dev_t *gpio_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    gpio_handle.handle = get_symbol_value_int(arg0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }
    if (!arg1 || !symbol_is_function(arg1)) {
        goto out;
    }

    ret = hal_gpio_enable_irq(gpio_device, IRQ_TRIGGER_FALLING_EDGE, ir_handle,
                              gpio_device);
    if (ret < 0) {
        be_error("ir", "hal_gpio_enable_irq fail!\n");
        goto out;
    }
    symbol_unlock(gpio_device->priv);
    gpio_device->priv = arg1;

    result = 0;
out:
    symbol_unlock(arg0);
    return new_int_symbol(result);
}

static void ir_delay(uint32_t counts) {
    uint32_t i = 0;
    for (i = 0; i < counts; i++) {
        be_osal_delay10us();
    }
}

static void ir_byte(gpio_dev_t *sda, gpio_dev_t *scl, unsigned char bData) {
    int8_t i     = 0;
    uint32_t val = 0;
    hal_gpio_output_low(scl);
    ir_delay(4);
    for (i = 7; i >= 0; i--) {
        ir_delay(4);
        if ((bData >> i) & 0x01) {
            hal_gpio_output_high(sda);
        } else {
            hal_gpio_output_low(sda);
        }
        ir_delay(4);
        hal_gpio_output_high(scl);
        ir_delay(4);
        hal_gpio_output_low(scl);
    }
    hal_gpio_output_high(sda);
    ir_delay(16);
    hal_gpio_output_high(scl);
    ir_delay(16);
    hal_gpio_input_get(sda, &val);
    ir_delay(16);
    hal_gpio_output_low(scl);
    ir_delay(16);
}

static void ir_buff(gpio_dev_t *sda, gpio_dev_t *scl, uint8_t *data,
                    uint32_t count) {
    uint32_t i = 0;
    hal_gpio_output_high(sda);
    hal_gpio_output_high(scl);
    ir_delay(4);
    hal_gpio_output_low(scl);
    ir_delay(8);
    hal_gpio_output_high(scl);
    be_osal_delay(20);
    hal_gpio_output_high(scl);
    hal_gpio_output_high(sda);
    ir_delay(8);
    hal_gpio_output_low(sda);
    ir_delay(40);
    hal_gpio_output_low(scl);
    ir_delay(8);
    ir_delay(4);
    for (i = 0; i < count; i++) {
        ir_byte(sda, scl, data[i]);
        ir_delay(4);
    }
    ir_delay(4);
    hal_gpio_output_low(scl);
    hal_gpio_output_low(sda);
    ir_delay(4);
    hal_gpio_output_high(scl);
    ir_delay(4);
    hal_gpio_output_high(sda);
    ir_delay(8);
    hal_gpio_output_low(sda);
    hal_gpio_output_low(scl);
    ir_delay(4);
    hal_gpio_output_high(scl);
    ir_delay(4);
    hal_gpio_output_high(sda);
    ir_delay(8);
}

static be_jse_symbol_t *ir_send(void) {
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t gpio_handle;
    be_jse_symbol_t *item = NULL;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    gpio_dev_t *gpio_scl  = NULL;
    gpio_dev_t *gpio_sda  = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    gpio_handle.handle = get_symbol_value_int(arg0);
    gpio_sda           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_sda) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }
    gpio_handle.handle = get_symbol_value_int(arg1);
    gpio_scl           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_scl) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }

    if (!arg2 || !symbol_is_array(arg2)) {
        goto out;
    }
    len  = get_symbol_array_length(arg2);
    data = calloc(1, sizeof(uint8_t) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    for (i = 0; i < len; ++i) {
        item = get_symbol_array_item(arg2, i);
        if (!item || !symbol_is_int(item)) {
            goto out;
        }
        data[i] = (uint8_t)get_symbol_value_int(item);
        symbol_unlock(item);
    }
    ir_buff(gpio_sda, gpio_scl, data, len);
    ir_delay(10);
    ir_buff(gpio_sda, gpio_scl, data, len);
    ir_delay(10);
    /* ir_buff(gpio_sda,gpio_scl,data,len); */

out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    if (NULL != data) {
        free(data);
        data = NULL;
    }
    return new_int_symbol(len);
}

static be_jse_symbol_t *ir_learn(void) {
    uint32_t i        = 0;
    int32_t ret       = -1;
    int8_t result     = -1;
    uint8_t buff[232] = {0x00};
    item_handle_t gpio_handle;
    be_jse_symbol_t *arr  = NULL;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    gpio_dev_t *gpio_scl  = NULL;
    gpio_dev_t *gpio_sda  = NULL;
    gpio_dev_t *gpio_busy = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    gpio_handle.handle = get_symbol_value_int(arg0);
    gpio_sda           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_sda) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }

    gpio_handle.handle = get_symbol_value_int(arg1);
    gpio_scl           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_scl) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }

    gpio_handle.handle = get_symbol_value_int(arg2);
    gpio_busy          = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_busy) {
        be_error("ir", "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = ir_learn_start(gpio_scl->port, gpio_sda->port, gpio_busy->port, buff);
    if (ret <= 0) {
        be_error("ir", "ir_learn_start fail!\n");
        goto out;
    }
    arr = new_symbol(BE_SYM_ARRAY);
    for (i = 0; i < 232; ++i) {
        /* printf("0x%x ",buff[i]);
           if(0 == i%16)printf("\n"); */
        be_jse_symbol_t *idx = new_int_symbol(buff[i]);
        symbol_array_push(arr, idx);
        symbol_unlock(idx);
    }
    result = 0;
out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    return (0 == result) ? arr : new_symbol(BE_SYM_NULL);
}

static be_jse_symbol_t *ir_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                            be_jse_symbol_t *var,
                                            const char *name) {
    if (0 == strcmp(name, "open")) {
        return ir_open();
    }
    if (0 == strcmp(name, "on")) {
        return ir_on();
    }
    if (0 == strcmp(name, "send")) {
        return ir_send();
    }
    if (0 == strcmp(name, "close")) {
        return ir_close();
    }
    if (0 == strcmp(name, "learn")) {
        return ir_learn();
    }

    return (BE_JSE_FUNC_UNHANDLED);
}

void module_ir_register(void) { be_jse_module_load("IR", ir_module_handle_cb); }
