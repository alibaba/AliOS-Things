/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_gpio.h"
#include "aos_system.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "ONEWIRE"

/*
使用方法：yaml文件中添加JSE_HW_ADDON_ONEWIRE、JSE_HW_ADDON_GPIO、RHINO_CONFIG_HW_COUNT
参考如下：
def_config:                              # 组件的可配置项
    CONFIG_ENGINE_DUKTAPE: 1
    #CONFIG_ENGINE_QUICKJS: 1
    CONFIG_VERSION: '1.0'
    JSE_HW_ADDON_ONEWIRE: 1
    JSE_HW_ADDON_GPIO: 1
    RHINO_CONFIG_HW_COUNT: 1

*/
static int A,B,C,D,E,F,G,H,I,J;
static uint16_t gpio_init_flag = 0;
static JSClassID js_onewire_class_id;

#define RHINO_CONFIG_HW_COUNT 1

#if (RHINO_CONFIG_HW_COUNT > 0)
#define CONFIG_FAST_SYSTICK_HZ      (hal_cmu_get_crystal_freq() / 4)
#define FAST_TICKS_TO_US(tick)      ((uint32_t)(tick) * 10 / (CONFIG_FAST_SYSTICK_HZ / 1000 / 100))

#define RHINO_CPU_INTRPT_DISABLE() do{ cpsr = cpu_intrpt_save(); }while(0)
#define RHINO_CPU_INTRPT_ENABLE()  do{ cpu_intrpt_restore(cpsr); }while(0)

static JSClassID js_onewire_class_id;

void oneWireUdelay(unsigned long x) {
    unsigned long now,t;
    int cpsr;

    RHINO_CPU_INTRPT_DISABLE();

    t = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    now = t;
    while ((now - t) < x) {
        now = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    }

    RHINO_CPU_INTRPT_ENABLE();
}
#endif

static void oneWireGpioSet(gpio_dev_t *gpio_device, unsigned char leve)
{
    int ret = 0;

    if(leve == 1)
    {
        ret = aos_hal_gpio_output_high(gpio_device);
    }
    else if(leve == 0)
    {
         ret = aos_hal_gpio_output_low(gpio_device);

    }
    //amp_warn(MOD_STR, "GPIO:%d, output config:0x%x, leve: %d, ret:%d", gpio_device->port, gpio_device->config, leve, ret);
}

static uint32_t oneWireGpioGet(gpio_dev_t *gpio_device)
{
    int ret = 0;
    unsigned int value = 0;

    ret = aos_hal_gpio_input_get(gpio_device, &value);
    //amp_warn(MOD_STR, "GPIO:%d, input config:0x%x, ret: %d, value:%d\r\n", gpio_device->port, gpio_device->config, ret, value);

    return (ret == 0)? value: ret;
}

//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 1 if no presence detect was found,
// return 0 otherwise.
// (NOTE: Does not handle alarm presence from DS2404/DS1994)
//
int oneWireTouchReset(gpio_dev_t *gpio_device)
{
    int result1;
    int result2;

    oneWireUdelay(G);

    oneWireGpioSet(gpio_device, 0x00); // Drives DQ low

    oneWireUdelay(H);

    oneWireGpioSet(gpio_device, 0x01); // Releases the bus

    oneWireUdelay(I);

    result1 = oneWireGpioGet(gpio_device) ^ 0x01; // Sample for presence pulse from slave

    oneWireUdelay(J); // Complete the reset sequence recovery

    result2 = oneWireGpioGet(gpio_device) ^ 0x01; // Sample for presence pulse from slave

    amp_warn(MOD_STR, "oneWireTouchReset[%d] result1 = %d, result2 = %d", __LINE__, result1, result2);

    return !((result1 == 1) && (result2 == 0)); // Return sample presence pulse result
}

//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void OneWireWriteBit(gpio_dev_t *gpio_device, int bit)
{
    //amp_warn(MOD_STR, "OneWireWriteBit[%d] bit = %d", __LINE__, bit);

    if (bit)
    {
            // Write '1' bit
            oneWireGpioSet(gpio_device, 0x00); // Drives DQ low

            oneWireUdelay(1);

            oneWireGpioSet(gpio_device, 0x01); // Releases the bus

            oneWireUdelay(30); // Complete the time slot and 10us recovery
    }
    else
    {
            // Write '0' bit
            oneWireGpioSet(gpio_device, 0x00); // Drives DQ low

            oneWireUdelay(30);

            oneWireGpioSet(gpio_device, 0x01); // Releases the bus

            oneWireUdelay(1);
    }
}

//-----------------------------------------------------------------------------
// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
//
int OneWireReadBit(gpio_dev_t *gpio_device)
{
        int result;

        oneWireGpioSet(gpio_device, 0x00); // Drives DQ low

        oneWireUdelay(1);

        oneWireGpioSet(gpio_device, 0x01); // Releases the bus

        oneWireUdelay(2);

        result = oneWireGpioGet(gpio_device) & 0x01; // Sample the bit value from the slave

        oneWireUdelay(40); // Complete the time slot and 10us recovery

        //amp_warn(MOD_STR, "OneWireReadBit[%d] result = %d", __LINE__, result);

        return result;
}

//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void OneWireWriteByte(gpio_dev_t *gpio_device, int data)
{
        int loop;

        // Loop to write each bit in the byte, LS-bit first
        for (loop = 0; loop < 8; loop++)
        {
                OneWireWriteBit(gpio_device, data & 0x01);

                // shift the data byte for the next bit
                data >>= 1;
        }
}

//-----------------------------------------------------------------------------
// Read 1-Wire data byte and return it
//
int OneWireReadByte(gpio_dev_t *gpio_device)
{
    int loop, result=0;

    for (loop = 0; loop < 8; loop++)
    {
            // shift the result to get it ready for the next bit
            result >>= 1;

            // if result is one, then set MS bit
            if (OneWireReadBit(gpio_device))
            {
                result |= 0x80;
            }

    }
    //amp_warn(MOD_STR, "OneWireReadByte[%d] data = 0x%x", __LINE__, result);

    return result;
}

void delayus(unsigned int z)
{
        while(z--);
}
//-----------------------------------------------------------------------------
// Set the 1-Wire timing to 'standard' (standard=1) or 'overdrive' (standard=0).
//
void oneWireSetSpeed(int standard)
{
    unsigned long now,t;

    // Adjust tick values depending on speed
    if (standard)
    {
            // Standard Speed
            // A = 6 * 4;
            // B = 64 * 4;
            // C = 60 * 4;
            // D = 10 * 4;
            // E = 9 * 4;
            // F = 55 * 4;
            // G = 0;
            // H = 480 * 4;
            // I = 70 * 4;
            // J = 410 * 4;

            A = 6;
            B = 64;
            C = 60;
            D = 10;
            E = 9;
            F = 55;
            G = 0;
            H = 480;
            I = 70;
            J = 410;
    }
    else
    {
            // Overdrive Speed
            A = 1.5 * 4;
            B = 7.5 * 4;
            C = 7.5 * 4;
            D = 2.5 * 4;
            E = 0.75 * 4;
            F = 7 * 4;
            G = 2.5 * 4;
            H = 70 * 4;
            I = 8.5 * 4;
            J = 40 * 4;
    }

    now = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    oneWireUdelay(A);
    t = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    amp_warn(MOD_STR, "[%d]hal_cmu_get_crystal_freq[%d] delta = %d", A, hal_cmu_get_crystal_freq(), (t-now));

    now = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    oneWireUdelay(B);
    t = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    amp_warn(MOD_STR, "[%d]hal_cmu_get_crystal_freq[%d] delta = %d", B, hal_cmu_get_crystal_freq(), (t-now));

    now = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    oneWireUdelay(C);
    t = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    amp_warn(MOD_STR, "[%d]hal_cmu_get_crystal_freq[%d] delta = %d", C, hal_cmu_get_crystal_freq(), (t-now));


    now = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    oneWireUdelay(D);
    t = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    amp_warn(MOD_STR, "[%d]hal_cmu_get_crystal_freq[%d] delta = %d", D, hal_cmu_get_crystal_freq(), (t-now));

}

//-----------------------------------------------------------------------------
// Set the 1-Wire timing to 'standard' (standard=1) or 'overdrive' (standard=0).
//
static JSValue native_onewire_gpio_setspeed(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int standard  = 0;
    int8_t result = -1;

    if((argc < 1) || (0 != JS_ToInt32(ctx, &standard, argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid\n");
        goto out;
    }
    printf("[%s][%d]standard = %d\n\r", __FUNCTION__, __LINE__, standard);
    //oneWireSetSpeed(standard);
    result = 0;
out:
    return JS_NewInt32(ctx, result);
}

static JSValue native_onewire_gpio_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;

    item_handle_t gpio_handle;
    gpio_handle.handle      = NULL;
    gpio_dev_t *gpio_device = NULL;
    const char *id;

    if((argc < 1) || (!JS_IsString(argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }

    id = JS_ToCString(ctx, argv[0]);

    ret = board_attach_item(MODULE_GPIO, id, &gpio_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!, id %s", id);
        goto out;
    }
    amp_debug(MOD_STR, "gpio handle:%p\n", gpio_handle.handle);
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    ret = aos_hal_gpio_init(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_init fail!");
        goto out;
    }
    // gpio_device->priv = NULL;
    gpio_init_flag |= (1 << gpio_device->port);

    amp_error(MOD_STR, "aos_hal_gpio_init ret = %d!", ret);
out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }
    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_GPIO, &gpio_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_onewire_class_id);
        JS_SetOpaque(obj, (void *)gpio_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, 0);
}

static JSValue native_onewire_gpio_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_onewire_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_gpio_finalize(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_GPIO, &gpio_handle);

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_onewire_gpio_reset(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    item_handle_t gpio_handle;
    int result = -1;
    gpio_dev_t *gpio_device = NULL;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_onewire_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    //result = oneWireTouchReset(gpio_device);
    result = 0;

out:
    return JS_NewInt32(ctx, result);
}

static JSValue native_onewire_gpio_readbyte(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    item_handle_t gpio_handle;
    int result;
    gpio_dev_t *gpio_device = NULL;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_onewire_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    //result = OneWireReadByte(gpio_device);
    result = 0;

out:
    return JS_NewInt32(ctx, result);
}

static JSValue native_onewire_gpio_writebyte(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    item_handle_t gpio_handle;
    int result = 0;
    gpio_dev_t *gpio_device = NULL;
    int32_t level  = 0;

    if((argc < 1) || (0 != JS_ToInt32(ctx, &level, argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid\n");
        goto out;
    }

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_onewire_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);

    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    //OneWireWriteByte(gpio_device, level);

    result = 1;

out:
    return JS_NewInt32(ctx, result);
}

static JSClassDef js_onewire_class = {
    "ONEWIRE",
};

static const JSCFunctionListEntry js_onewire_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_onewire_gpio_open ),
    JS_CFUNC_DEF("setspeed", 1, native_onewire_gpio_setspeed ),
    JS_CFUNC_DEF("reset", 0, native_onewire_gpio_reset ),
    JS_CFUNC_DEF("readByte", 0, native_onewire_gpio_readbyte),
    JS_CFUNC_DEF("writeByte", 1, native_onewire_gpio_writebyte),
    JS_CFUNC_DEF("close", 0, native_onewire_gpio_close),
};

static int js_onewire_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_onewire_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_onewire_class_id, &js_onewire_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_onewire_funcs,
                               countof(js_onewire_funcs));
    JS_SetClassProto(ctx, js_onewire_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_onewire_funcs,
                                  countof(js_onewire_funcs));
}

JSModuleDef *js_init_module_onewire(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_onewire_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_onewire_funcs, countof(js_onewire_funcs));
    return m;
}

void module_onewire_register(void)
{
    amp_debug(MOD_STR, "module_onewire_register");
    JSContext *ctx = js_get_context();
    aos_printf("module onewire register\n");
    js_init_module_onewire(ctx, "ONEWIRE");
}
