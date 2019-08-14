/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "jse_task.h"
#include "jse_port.h"
#include "board-mgr/board_mgr.h"
#include "bone_engine_inl.h"

static void ir_learn_mode(uint32_t scl_pin, uint32_t sda_pin)
{
    gpio_i2c_reset(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);

    gpio_i2c_set_low(scl_pin);
    gpio_i2c_delay_10us(8);
    gpio_i2c_set_high(scl_pin);

    jse_osal_delay(20);
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

static int8_t ir_learn_read(uint32_t scl_pin, uint32_t sda_pin, uint8_t *buff)
{
    uint8_t value    = 0;
    uint8_t i        = 0;
    uint8_t checksum = 0;
    gpio_i2c_reset(scl_pin, sda_pin);
    gpio_i2c_delay_10us(4);
    gpio_i2c_set_low(scl_pin);
    gpio_i2c_delay_10us(8);
    gpio_i2c_set_high(scl_pin);
    jse_osal_delay(20);

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
        return -1;
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
    return 0;
}

static int32_t ir_learn_start(uint32_t scl_pin, uint32_t sda_pin,
                              uint32_t busy_bin, uint8_t buff[232])
{
    uint8_t sumValue = 0;
    int32_t count    = 0;
    int8_t ret       = -1;
    uint8_t i        = 0;
    uint8_t tmp[512] = {0x00};
    gpio_i2c_init(scl_pin, sda_pin);
    gpio_i2c_set_in(busy_bin);
    ir_learn_mode(scl_pin, sda_pin);
    jse_osal_delay(50);
    while (!gpio_i2c_read_pin(busy_bin)) gpio_i2c_delay_10us(10);
    ret = ir_learn_read(scl_pin, sda_pin, tmp);
    if (0 != ret) {
        return -1;
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
    return 232;
}

static uint32_t ir_counts(gpio_dev_t *gpio, uint8_t level)
{
    int8_t ret      = 0;
    uint32_t value  = 0;
    uint32_t counts = 0;
    do {
        ret = hal_gpio_input_get(gpio, &value);
        counts += 1;
        jse_osal_delay10us();
    } while ((0 == ret) && (value == level));
    return counts;
}

static uint32_t ir_nec(gpio_dev_t *gpio)
{
    uint32_t counts = 0;
    uint32_t value  = 0;
    uint8_t i       = 0;
    uint8_t j       = 0;
    /*9ms*/
    counts = ir_counts(gpio, 0);
    if (counts < 850 || counts > 950) {
        return 0;
    }
    /*4.5ms*/
    counts = ir_counts(gpio, 1);
    if (counts < 400 || counts > 500) {
        return 0;
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
    return value;
}

static duk_ret_t native_ir_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t gpio_handle;
    gpio_handle.handle      = 0xFFFFFFFF;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("parameter must be string\n");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_GPIO, id, &gpio_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }
    jse_debug("ir handle:%u\n", gpio_handle.handle);
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_gpio_init(gpio_device);
    if (0 != ret) {
        jse_error("hal_gpio_init fail!\n");
        goto out;
    }
    gpio_device->priv = NULL;
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_GPIO, &gpio_handle);
    } else {
        duk_push_pointer(ctx, (void *)gpio_handle.handle);
    }
    return 1;
}

static duk_ret_t native_ir_close(duk_context *ctx)
{
    int8_t result = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    hal_gpio_disable_irq(gpio_device);
    gpio_device->priv = NULL;
    board_disattach_item(MODULE_GPIO, &gpio_handle);
    result = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

struct gpio_irq_notify_param {
    int js_cb_ref;
    int value;
};

static void gpio_irq_notify(void *arg)
{
    struct gpio_irq_notify_param *p = (struct gpio_irq_notify_param *)arg;
    jse_debug("value: 0x%x\n", p->value);
    duk_context *ctx = bone_engine_get_context();
    bone_engine_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->value);
    duk_pcall(ctx, 1);
    duk_pop(ctx);
    jse_free(p);
}

/* avoid stdout in irq function */
static void ir_handle(void *arg)
{
    uint32_t value   = 0;
    gpio_dev_t *gpio = (gpio_dev_t *)arg;

    if (NULL == gpio) {
        /* jse_error("param error!\n"); */
        return;
    }
    value = ir_nec(gpio);
    if (0x00 == value) {
        return;
    }

    int js_cb_ref = (int)gpio->priv;
    if (js_cb_ref <= 0) {
        /* jse_error("js cb ref error, ref: %d\n", js_cb_ref); */
        return;
    }

    struct gpio_irq_notify_param *p =
        (struct gpio_irq_notify_param *)jse_malloc(sizeof(*p));
    p->js_cb_ref = js_cb_ref;
    p->value     = value & 0xFFFF;
    if (be_jse_task_schedule_call(gpio_irq_notify, p) < 0) {
        /* jse_warn("be_jse_task_schedule_call failed\n"); */
        jse_free(p);
    }
}

static duk_ret_t native_ir_on(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t gpio_handle;
    gpio_handle.handle      = 0xFFFFFFFF;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        jse_warn("parameter must be handle and function\n");
        goto out;
    }
    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_gpio_enable_irq(gpio_device, IRQ_TRIGGER_FALLING_EDGE, ir_handle,
                              gpio_device);
    if (ret < 0) {
        jse_error("hal_gpio_enable_irq fail!\n");
        goto out;
    }
    duk_dup(ctx, 1);
    int js_cb_ref     = bone_engine_ref(ctx);
    gpio_device->priv = (void *)js_cb_ref;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static void ir_delay(uint32_t counts)
{
    uint32_t i = 0;
    for (i = 0; i < counts; i++) jse_osal_delay10us();
}

static void ir_byte(gpio_dev_t *sda, gpio_dev_t *scl, unsigned char bData)
{
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
                    uint32_t count)
{
    uint32_t i = 0;
    hal_gpio_output_high(sda);
    hal_gpio_output_high(scl);
    ir_delay(4);
    hal_gpio_output_low(scl);
    ir_delay(8);
    hal_gpio_output_high(scl);
    jse_osal_delay(20);
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

static duk_ret_t native_ir_send(duk_context *ctx)
{
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_scl = NULL;
    gpio_dev_t *gpio_sda = NULL;
    int arr_idx;
    int err = -1;

    if (!duk_is_pointer(ctx, 0) || !duk_is_pointer(ctx, 1) ||
        !duk_is_array(ctx, 2)) {
        jse_warn("parameter must be handle, handle and array\n");
        goto out;
    }

    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_sda           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_sda) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }

    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 1);
    gpio_scl           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_scl) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }

    arr_idx = duk_normalize_index(ctx, 2);
    len     = duk_get_length(ctx, arr_idx);
    data    = (uint8_t *)jse_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        jse_warn("allocate memory failed\n");
        goto out;
    }
    for (i = 0; i < len; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_number(ctx, -1)) {
            jse_warn("data is not number, index: %d\n", i);
            duk_pop(ctx);
            goto out;
        }
        data[i] = (uint8_t)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }
    ir_buff(gpio_sda, gpio_scl, data, len);
    ir_delay(10);
    ir_buff(gpio_sda, gpio_scl, data, len);
    ir_delay(10);
    /* ir_buff(gpio_sda,gpio_scl,data,len); */
    err = 0;
out:
    jse_free(data);
    duk_push_int(ctx, err);
    return 1;
}

static duk_ret_t native_ir_learn(duk_context *ctx)
{
    uint32_t i        = 0;
    int32_t ret       = -1;
    uint8_t buff[232] = {0x00};
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_scl  = NULL;
    gpio_dev_t *gpio_sda  = NULL;
    gpio_dev_t *gpio_busy = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_pointer(ctx, 1) ||
        !duk_is_pointer(ctx, 2)) {
        jse_warn("parameter must be handle, handle and handle\n");
        goto failed;
    }

    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_sda           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_sda) {
        jse_error("board_get_node_by_handle fail!\n");
        goto failed;
    }

    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 1);
    gpio_scl           = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_scl) {
        jse_error("board_get_node_by_handle fail!\n");
        goto failed;
    }

    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 2);
    gpio_busy          = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_busy) {
        jse_error("board_get_node_by_handle fail!\n");
        goto failed;
    }

    ret = ir_learn_start(gpio_scl->port, gpio_sda->port, gpio_busy->port, buff);
    if (ret <= 0) {
        jse_error("ir_learn_start fail!\n");
        goto failed;
    }
    int arr_idx = duk_push_array(ctx);
    for (i = 0; i < 232; ++i) {
        duk_push_int(ctx, buff[i]);
        duk_put_prop_index(ctx, arr_idx, i);
    }
    return 1;

failed:
    duk_push_null(ctx);
    return 1;
}

void module_ir_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_ir_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_ir_on, 2);
    duk_put_prop_string(ctx, -2, "on");

    duk_push_c_function(ctx, native_ir_send, 3);
    duk_put_prop_string(ctx, -2, "send");

    duk_push_c_function(ctx, native_ir_learn, 3);
    duk_put_prop_string(ctx, -2, "learn");

    duk_push_c_function(ctx, native_ir_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "IR");
}
