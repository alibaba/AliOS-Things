/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_gpio.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "IR"

static JSClassID js_ir_class_id;

typedef struct ir_module {
   item_handle_t sda_handle;
   item_handle_t scl_handle;
   item_handle_t busy_handle;
} ir_module_t;

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
        ret = aos_hal_gpio_input_get(gpio, &value);
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

static JSValue native_ir_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    ir_module_t *ir_handle;
    gpio_dev_t *gpio_device = NULL;
    const char *id;

    if((argc < 1) || (!JS_IsString(argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }

    /* init sda port*/
    id = JS_ToCString(ctx, argv[0]);

    ir_handle = (uint8_t *)aos_malloc(sizeof(ir_handle));
    if (NULL == ir_handle) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }

    ret = board_attach_item(MODULE_GPIO, id, &(ir_handle->sda_handle));
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!\n");
        goto out;
    }
    amp_debug(MOD_STR, "ir handle:%u\n", ir_handle->sda_handle.handle);
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->sda_handle));
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = aos_hal_gpio_init(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_init fail!\n");
        goto out;
    }
    gpio_device->priv = NULL;

    /* init scl port*/
    id = JS_ToCString(ctx, argv[1]);

    ret = board_attach_item(MODULE_GPIO, id, &(ir_handle->scl_handle));
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!\n");
        goto out;
    }
    amp_debug(MOD_STR, "ir handle:%u\n", ir_handle->scl_handle);
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->scl_handle));
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = aos_hal_gpio_init(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_init fail!\n");
        goto out;
    }
    gpio_device->priv = NULL;


    /* init busy port*/
    id = JS_ToCString(ctx, argv[1]);

    ret = board_attach_item(MODULE_GPIO, id, &(ir_handle->busy_handle));
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!\n");
        goto out;
    }
    amp_debug(MOD_STR, "ir handle:%u\n", ir_handle->busy_handle);
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->busy_handle));
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = aos_hal_gpio_init(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_init fail!\n");
        goto out;
    }
    gpio_device->priv = NULL;

out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }

    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_GPIO, &(ir_handle->sda_handle));
        board_disattach_item(MODULE_GPIO, &(ir_handle->scl_handle));
        board_disattach_item(MODULE_GPIO, &(ir_handle->busy_handle));
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_ir_class_id);
        JS_SetOpaque(obj, (void *)ir_handle);
        return obj;
    }
    return JS_NewInt32(ctx, 0);
}


static JSValue native_ir_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t result = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;
    ir_module_t *ir_handle;

    ir_handle = JS_GetOpaque2(ctx, this_val, js_ir_class_id);
    if (!ir_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    /* close sda port*/
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->sda_handle));
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    aos_hal_gpio_disable_irq(gpio_device);
    gpio_device->priv = NULL;
    board_disattach_item(MODULE_GPIO, &(ir_handle->sda_handle));

    /* close scl port*/
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->scl_handle));
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    aos_hal_gpio_disable_irq(gpio_device);
    gpio_device->priv = NULL;
    board_disattach_item(MODULE_GPIO, &(ir_handle->scl_handle));

    /* busy scl port*/
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->busy_handle));
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    aos_hal_gpio_disable_irq(gpio_device);
    gpio_device->priv = NULL;
    board_disattach_item(MODULE_GPIO, &(ir_handle->busy_handle));

    result = 0;
out:
    return JS_NewInt32(ctx, result);
}

struct gpio_irq_notify_param {
    int js_cb_ref;
    int value;
};

static void gpio_irq_notify(void *arg)
{
    struct gpio_irq_notify_param *p = (struct gpio_irq_notify_param *)arg;
    amp_debug(MOD_STR, "value: 0x%x\n", p->value);

    JSContext *ctx = js_get_context();
    JSValue val = JS_Call(ctx, p->js_cb_ref, JS_UNDEFINED, 1, &(p->value));
    JS_FreeValue(ctx, val);
}

/* avoid stdout in irq function */
static void ir_handle(void *arg)
{
    uint32_t value   = 0;
    gpio_dev_t *gpio = (gpio_dev_t *)arg;

    if (NULL == gpio) {
        /* amp_error(MOD_STR, "param error!\n"); */
        return;
    }
    value = ir_nec(gpio);
    if (0x00 == value) {
        return;
    }

    int js_cb_ref = (int)gpio->priv;
    if (js_cb_ref <= 0) {
        /* amp_error(MOD_STR, "js cb ref error, ref: %d\n", js_cb_ref); */
        return;
    }

    struct gpio_irq_notify_param *p =
        (struct gpio_irq_notify_param *)aos_malloc(sizeof(*p));
    p->js_cb_ref = js_cb_ref;
    p->value     = value & 0xFFFF;
    if (amp_task_schedule_call(gpio_irq_notify, p) < 0) {
        /* amp_warn(MOD_STR, "amp_task_schedule_call failed\n"); */
        aos_free(p);
    }
}

static JSValue native_ir_on(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    gpio_dev_t *gpio_device = NULL;
    int js_cb_ref;
    ir_module_t *ir_handle;

    if((argc < 1) || (JS_IsNull(argv[0]) || JS_IsUndefined(argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }

    ir_handle = JS_GetOpaque2(ctx, this_val, js_ir_class_id);
    if (!ir_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->sda_handle));
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = aos_hal_gpio_enable_irq(gpio_device, IRQ_TRIGGER_FALLING_EDGE, ir_handle,
                              gpio_device);
    if (ret < 0) {
        amp_error(MOD_STR, "aos_hal_gpio_enable_irq fail!\n");
        goto out;
    }
    js_cb_ref = JS_DupValue(ctx, argv[0]);
    gpio_device->priv = (void *)js_cb_ref;
out:
    return JS_NewInt32(ctx, ret);
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
    aos_hal_gpio_output_low(scl);
    ir_delay(4);
    for (i = 7; i >= 0; i--) {
        ir_delay(4);
        if ((bData >> i) & 0x01) {
            aos_hal_gpio_output_high(sda);
        } else {
            aos_hal_gpio_output_low(sda);
        }
        ir_delay(4);
        aos_hal_gpio_output_high(scl);
        ir_delay(4);
        aos_hal_gpio_output_low(scl);
    }
    aos_hal_gpio_output_high(sda);
    ir_delay(16);
    aos_hal_gpio_output_high(scl);
    ir_delay(16);
    aos_hal_gpio_input_get(sda, &val);
    ir_delay(16);
    aos_hal_gpio_output_low(scl);
    ir_delay(16);
}

static void ir_buff(gpio_dev_t *sda, gpio_dev_t *scl, uint8_t *data,
                    uint32_t count)
{
    uint32_t i = 0;
    aos_hal_gpio_output_high(sda);
    aos_hal_gpio_output_high(scl);
    ir_delay(4);
    aos_hal_gpio_output_low(scl);
    ir_delay(8);
    aos_hal_gpio_output_high(scl);
    jse_osal_delay(20);
    aos_hal_gpio_output_high(scl);
    aos_hal_gpio_output_high(sda);
    ir_delay(8);
    aos_hal_gpio_output_low(sda);
    ir_delay(40);
    aos_hal_gpio_output_low(scl);
    ir_delay(8);
    ir_delay(4);
    for (i = 0; i < count; i++) {
        ir_byte(sda, scl, data[i]);
        ir_delay(4);
    }
    ir_delay(4);
    aos_hal_gpio_output_low(scl);
    aos_hal_gpio_output_low(sda);
    ir_delay(4);
    aos_hal_gpio_output_high(scl);
    ir_delay(4);
    aos_hal_gpio_output_high(sda);
    ir_delay(8);
    aos_hal_gpio_output_low(sda);
    aos_hal_gpio_output_low(scl);
    ir_delay(4);
    aos_hal_gpio_output_high(scl);
    ir_delay(4);
    aos_hal_gpio_output_high(sda);
    ir_delay(8);
}

static JSValue native_ir_send(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    gpio_dev_t *gpio_scl = NULL;
    gpio_dev_t *gpio_sda = NULL;
    int arr_idx;
    int err = -1;
    ir_module_t *ir_handle;

    if(argc < 1)
    {
        amp_warn(MOD_STR, "parameter must be array");
        goto out;
    }

    ir_handle = JS_GetOpaque2(ctx, this_val, js_ir_class_id);
    if (!ir_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_sda = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->sda_handle));
    if (NULL == gpio_sda) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    gpio_scl = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->sda_handle));
    if (NULL == gpio_scl) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    data = JS_GetArrayBuffer(ctx, &len, argv[0]);
    if(!data)
    {
        amp_warn(MOD_STR, "parameter buffer is invalid, size: %d", len);
        goto out;
    }

    ir_buff(gpio_sda, gpio_scl, data, len);
    ir_delay(10);
    ir_buff(gpio_sda, gpio_scl, data, len);
    ir_delay(10);
    /* ir_buff(gpio_sda,gpio_scl,data,len); */
    err = 0;
out:
    aos_free(data);
    return JS_NewInt32(ctx, err);
}

static JSValue native_ir_learn(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    uint32_t i        = 0;
    int32_t ret       = -1;
    uint8_t buff[232] = {0x00};
    gpio_dev_t *gpio_scl  = NULL;
    gpio_dev_t *gpio_sda  = NULL;
    gpio_dev_t *gpio_busy = NULL;
    ir_module_t *ir_handle;
    JSValue response = JS_UNDEFINED;

    ir_handle = JS_GetOpaque2(ctx, this_val, js_ir_class_id);
    if (!ir_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto failed;
    }

    gpio_sda = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->sda_handle));
    if (NULL == gpio_sda) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto failed;
    }

    gpio_scl = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->scl_handle));
    if (NULL == gpio_scl) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto failed;
    }

    gpio_busy = board_get_node_by_handle(MODULE_GPIO, &(ir_handle->busy_handle));
    if (NULL == gpio_busy) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto failed;
    }

    ret = ir_learn_start(gpio_scl->port, gpio_sda->port, gpio_busy->port, buff);
    if (ret <= 0) {
        amp_error(MOD_STR, "ir_learn_start fail!\n");
        goto failed;
    }

    response = JS_NewArrayBufferCopy(ctx, buff, 232);
    
    if (JS_IsException(response))
        goto failed;

failed:
    return response;
}

static JSClassDef js_ir_class = {
    "IR",
};

static const JSCFunctionListEntry js_ir_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_ir_open),
    JS_CFUNC_DEF("on", 1, native_ir_on),
    JS_CFUNC_DEF("send", 2, native_ir_send),
    JS_CFUNC_DEF("learn", 2, native_ir_learn),
    JS_CFUNC_DEF("close", 0, native_ir_close),
};


static int js_ir_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_ir_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_ir_class_id, &js_ir_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_ir_funcs,
                               countof(js_ir_funcs));
    JS_SetClassProto(ctx, js_ir_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_ir_funcs,
                                  countof(js_ir_funcs));
}

JSModuleDef *js_init_module_ir(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_ir_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_ir_funcs, countof(js_ir_funcs));
    return m;
}

void module_ir_register(void)
{
    amp_debug(MOD_STR, "module_ir_register");
    JSContext *ctx = js_get_context();
    aos_printf("module ir register\n");
    js_init_module_ir(ctx, "IR");
}
