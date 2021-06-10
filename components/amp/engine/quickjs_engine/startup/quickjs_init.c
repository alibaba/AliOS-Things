#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "quickjs.h"
#include "amp_config.h"
#include "amp_list.h"
#include "quickjs-libc.h"
#include "aos_system.h"
#include "quickjs_addon.h"

#define MOD_STR "QUICKJS_INIT"

static JSRuntime *rt = NULL;
static JSContext *ctx = NULL;
static JSValue val;

extern int g_repl_config;
extern JSModuleDef * quickjs_module_loader(JSContext *ctx,
                                          const char *module_name, void *opaque);

JSContext *js_get_context(void)
{
    return ctx;
}

static void jsengine_register_addons()
{
#if 1
    extern uint32_t jslib_events_size;
    extern uint8_t jslib_events[];
    quickjs_add_prebuild_module("events", jslib_events, jslib_events_size);
#endif
#ifdef JSE_HW_ADDON_GPIO
    module_gpio_register();
    extern uint32_t jslib_gpio_size;
    extern uint8_t jslib_gpio[];
    quickjs_add_prebuild_module("gpio", jslib_gpio, jslib_gpio_size);
#endif

#ifdef JSE_HW_ADDON_IR
    module_ir_register();
#endif

#ifdef JSE_HW_ADDON_PWM
    module_pwm_register();
    extern uint32_t jslib_pwm_size;
    extern uint8_t jslib_pwm[];
    quickjs_add_prebuild_module("pwm", jslib_pwm, jslib_pwm_size);
#endif

#ifdef JSE_HW_ADDON_ONEWIRE
    module_onewire_register();
    extern uint32_t jslib_onewire_size;
    extern uint8_t jslib_onewire[];
    quickjs_add_prebuild_module("onewire", jslib_onewire, jslib_onewire_size);
#endif

#ifdef JSE_HW_ADDON_RTC
    module_rtc_register();
    extern uint32_t jslib_rtc_size;
    extern uint8_t jslib_rtc[];
    quickjs_add_prebuild_module("rtc", jslib_rtc, jslib_rtc_size);
#endif
#ifdef JSE_CORE_ADDON_CRYPTO
    module_crypto_register();
    extern uint32_t jslib_crypto_size;
    extern uint8_t jslib_crypto[];
    quickjs_add_prebuild_module("crypto", jslib_crypto, jslib_crypto_size);
#endif
#ifdef JSE_HW_ADDON_WDG
    module_wdg_register();
    extern uint32_t jslib_wdg_size;
    extern uint8_t jslib_wdg[];
    quickjs_add_prebuild_module("wdg", jslib_wdg, jslib_wdg_size);
#endif

#ifdef JSE_HW_ADDON_UART
    module_uart_register();
    extern uint32_t jslib_uart_size;
    extern uint8_t jslib_uart[];
    quickjs_add_prebuild_module("uart", jslib_uart, jslib_uart_size);
#endif

#ifdef JSE_HW_ADDON_SPI
    module_spi_register();
    extern uint32_t jslib_spi_size;
    extern uint8_t jslib_spi[];
    quickjs_add_prebuild_module("spi", jslib_spi, jslib_spi_size);
#endif

#ifdef JSE_HW_ADDON_ADC
    module_adc_register();
    extern uint32_t jslib_adc_size;
    extern uint8_t jslib_adc[];
    quickjs_add_prebuild_module("adc", jslib_adc, jslib_adc_size);
#endif

#ifdef JSE_HW_ADDON_TIMER
    module_timer_register();
#endif

#ifdef JSE_HW_ADDON_DAC
    module_dac_register();
    extern uint32_t jslib_dac_size;
    extern uint8_t jslib_dac[];
    quickjs_add_prebuild_module("dac", jslib_dac, jslib_dac_size);
#endif

#ifdef JSE_HW_ADDON_DS18B20
    extern uint32_t jslib_ds18b20_size;
    extern uint8_t jslib_ds18b20[];
    quickjs_add_prebuild_module("ds18b20", jslib_ds18b20, jslib_ds18b20_size);
#endif

#ifdef JSE_CORE_ADDON_SYSTIMER
    module_systimer_register();
#endif

#ifdef JSE_NET_ADDON_HTTP
    module_http_register();
#endif

#ifdef JSE_NET_ADDON_NETMGR
    module_netmgr_register();
    extern uint32_t jslib_netmgr_size;
    extern uint8_t jslib_netmgr[];
    quickjs_add_prebuild_module("netmgr", jslib_netmgr, jslib_netmgr_size);
#endif

#ifdef JSE_CORE_ADDON_SYSTEM
    module_system_register();
#endif

#ifdef JSE_ADVANCED_ADDON_OTA
    module_app_ota_register();
    extern uint32_t jslib_appota_size;
    extern uint8_t jslib_appota[];
    quickjs_add_prebuild_module("appota", jslib_appota, jslib_appota_size);
#endif

#ifdef JSE_ADVANCED_ADDON_UI
    page_entry_register();
    app_entry_register();
    //module_vm_register();
    module_ui_register();
#endif

#ifdef JSE_HW_ADDON_LCD
    module_lcd_register();
#endif

#ifdef JSE_HW_ADDON_I2C
    module_i2c_register();
    extern uint32_t jslib_i2c_size;
    extern uint8_t jslib_i2c[];
    quickjs_add_prebuild_module("i2c", jslib_i2c, jslib_i2c_size);
#endif

#ifdef JSE_ADVANCED_ADDON_AIOT_DEVICE
    module_aiot_device_register();
    extern uint32_t jslib_iot_size;
    extern uint8_t jslib_iot[];
    quickjs_add_prebuild_module("iot", jslib_iot, jslib_iot_size);
#endif

#ifdef JSE_ADVANCED_ADDON_AIOT_GATEWAY
    module_aiot_gateway_register();
    extern uint32_t jslib_gateway_size;
    extern uint8_t jslib_gateway[];
    quickjs_add_prebuild_module("gateway", jslib_gateway, jslib_gateway_size);
#endif


#ifdef JSE_CORE_ADDON_LOG
    module_log_register();
#endif

#ifdef JSE_WIRELESS_ADDON_BT_HOST
    module_bt_host_register();
    extern uint32_t jslib_bt_host_size;
    extern uint8_t jslib_bt_host[];
    quickjs_add_prebuild_module("bt_host", jslib_bt_host, jslib_bt_host_size);
#endif

#ifdef JSE_CORE_ADDON_FS
    module_fs_register();
    extern uint32_t jslib_fs_size;
    extern uint8_t jslib_fs[];
    quickjs_add_prebuild_module("fs", jslib_fs, jslib_fs_size);
#endif

#ifdef JSE_CORE_ADDON_KV
    module_kv_register();
#endif

#ifdef JSE_ADVANCED_ADDON_AUDIOPLAYER
    module_audio_register();
    extern uint32_t jslib_audioplayer_size;
    extern uint8_t jslib_audioplayer[];
    quickjs_add_prebuild_module("audioplayer", jslib_audioplayer, jslib_audioplayer_size);
#endif

#ifdef JSE_CORE_ADDON_CHECKSUM
    module_checksum_register();
#endif

#ifdef JSE_ADVANCED_ADDON_BLECFGNET
    module_blecfgnet_register();
#endif
}

int jsengine_init(void)
{
    rt = JS_NewRuntime();
    if (rt == NULL) {
        fprintf(stderr, "JS_NewRuntime failure");
        aos_task_exit(1);
    }
    js_std_init_handlers(rt);

    ctx = JS_NewContext(rt);
    if (ctx == NULL) {
        fprintf(stderr, "JS_NewContext failure");
    }
    //JS_AddIntrinsicBaseObjects(ctx);
    JS_SetCanBlock(rt, 1);

    /* loader for ES6 modules */
    JS_SetModuleLoaderFunc(rt, NULL, quickjs_module_loader, NULL);

    js_std_add_helpers(ctx, -1, NULL);

    /* system modules */
    js_init_module_std(ctx, "std");
    js_init_module_os(ctx, "os");

    jsengine_register_addons();

    if (g_repl_config) {
        module_repl_register();
    }

    aos_printf("quickjs engine started !\n");

    return 0;
}

void qjs_dump_obj(JSContext* ctx, JSValueConst val)
{
    const char* str;

    str = JS_ToCString(ctx, val);
    if (str) {
        aos_printf("%s", str);
        JS_FreeCString(ctx, str);
    } else {
        aos_printf("[exception]\n");
    }
}

void qjs_std_dump_error1(JSContext* ctx, JSValueConst exception_val)
{
    JSValue val;
    bool is_error;

    is_error = JS_IsError(ctx, exception_val);
    qjs_dump_obj(ctx, exception_val);
    if (is_error) {
        val = JS_GetPropertyStr(ctx, exception_val, "stack");
        if (!JS_IsUndefined(val)) {
            qjs_dump_obj(ctx, val);
        }
        JS_FreeValue(ctx, val);
    }
}

void qjs_std_dump_error(JSContext* ctx)
{
    JSValue exception_val;

    exception_val = JS_GetException(ctx);

    qjs_std_dump_error1(ctx, exception_val);
    JS_FreeValue(ctx, exception_val);
}

void jsengine_eval_file(const char *filename)
{
    uint8_t *buf = NULL;
    size_t buf_len = 0;
    int eval_flags = JS_EVAL_TYPE_MODULE;

    aos_printf("begin eval file: %s\n", filename);

    buf = js_load_file(ctx, &buf_len, filename);
    if(filename[strlen(filename) - 1] == 's') {
        val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
        if (JS_IsException(val)) {
            aos_printf("global.createInstance eval jsbundle exec error\n");
            qjs_std_dump_error(ctx);
        }
        JS_FreeValue(ctx, val);
    } else {
        js_std_eval_binary(ctx, buf, buf_len, 0);
    }

    js_free(ctx, buf);
    if (g_repl_config) {
        extern uint32_t jslib_repl_size;
        extern uint8_t jslib_repl[];
        js_std_eval_binary(ctx, jslib_repl, jslib_repl_size, 0);
    }
}

void jsengine_exit()
{
    JS_FreeValue(ctx, val);
    js_std_loop(ctx);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

void dump_quickjs_memory()
{
    JS_RunGC(rt);
    JSMemoryUsage stats;
    JS_ComputeMemoryUsage(rt, &stats);
    JS_DumpMemoryUsage(stdout, &stats, rt);
}