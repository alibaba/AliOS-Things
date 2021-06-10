/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_fs.h"
#include "aos_system.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "cJSON.h"
#include "addons/libjs.h"
#include "startup/app_entry.h"

#define MOD_STR "BE"
#define MAX_PATH_SIZE 1024

static duk_context *duk_ctx;
static char node_modules_path[MAX_PATH_SIZE];
static char resolved_path[MAX_PATH_SIZE];
static char *file_data;
int g_repl_config = 0;

#define assert(x) do{ \
    if(!(x)){ \
        amp_error(MOD_STR, "fatal expression: " #x "\r\n"); \
        while(1){ \
            aos_msleep(10); \
        }; \
    } \
}while(0)

static char *resolve_path(const char *parent, const char *module_id)
{
    char base[MAX_PATH_SIZE];
    int i;

#ifdef JSE_HIGHLEVEL_JSAPI
    /* check libjs */
    for(i = 0; i < libjs_num; i++){
        if(strcmp(LIBJS_ENTRIES[i].name, module_id) != 0){
            continue;
        }
        return (char *)module_id;
    }
#endif

    /* path process */
    if (module_id[0] != '.') {
        snprintf(base, sizeof(base), "%s/%s", node_modules_path, module_id);
    } else {
        strcpy(base, parent);
        char *last_slash = strrchr(base, '/');
        if (last_slash) last_slash[0] = '\0';

        int len       = strlen(module_id);
        const char *p = module_id;

        if (len == 1 && p[0] == '.') {
            p += 1;
        } else if (p[0] == '.' && p[1] == '/') {
            p += 2;
        } else {
            const char *end = module_id + len;
            while (p <= end - 3 && p[0] == '.' && p[1] == '.' && p[2] == '/') {
                p += 3;
                char *last_slash = strrchr(base, '/');
                if (last_slash)
                    last_slash[0] = '\0';
                else {
                    amp_warn(MOD_STR, "resolve path fail\n");
                    break;
                }
            }
        }
        strcat(base, "/");
        strcat(base, p);
    }

    struct aos_stat sb;
    int ret = aos_stat(base, &sb);
    amp_debug(MOD_STR, "base: %s, ret: %d\n", base, ret);
    if (!ret && S_ISREG(sb.st_mode)) {
        strcpy(resolved_path, base);
        return resolved_path;
    }

    snprintf(resolved_path, sizeof(resolved_path), "%s.js", base);
    ret = aos_stat(resolved_path, &sb);
    amp_debug(MOD_STR, "resolved_path: %s, ret: %d\n", resolved_path, ret);
    if (!ret && S_ISREG(sb.st_mode)) {
        return resolved_path;
    }

    snprintf(resolved_path, sizeof(resolved_path), "%s/index.js", base);
    ret = aos_stat(resolved_path, &sb);
    amp_debug(MOD_STR, "resolved_path: %s, ret: %d\n", resolved_path, ret);
    if (!ret && S_ISREG(sb.st_mode)) {
        return resolved_path;
    }

    snprintf(resolved_path, sizeof(resolved_path), "%s/package.json", base);
    ret = aos_stat(resolved_path, &sb);
    amp_debug(MOD_STR, "resolved_path: %s, ret: %d\n", resolved_path, ret);
    if (ret || !S_ISREG(sb.st_mode)) {
        amp_warn(MOD_STR, "file %s not exist\n", resolved_path);
        return NULL;
    }
    /* parse package.json */
    char *pkg_data = aos_malloc(sb.st_size + 1);
    if (!pkg_data) {
        amp_warn(MOD_STR, "cannot alloc memory to read package.json for module: %s",
             module_id);
        return NULL;
    }

    int fp = aos_open(resolved_path, O_RDONLY);
    aos_read(fp, pkg_data, sb.st_size);
    aos_close(fp);
    pkg_data[sb.st_size] = '\0';

    cJSON *json      = cJSON_Parse(pkg_data);
    //TODO: cJSON *item_main = cJSON_GetObjectItemCaseSensitive(json, "main");
    cJSON *item_main = cJSON_GetObjectItem(json, "main");
    amp_debug(MOD_STR, "item_main: %p\n", item_main);
    //TODO: if (cJSON_IsString(item_main) && item_main->valuestring) {
    if (item_main->valuestring) {
        snprintf(resolved_path, sizeof(resolved_path), "%s/%s", base,
                 item_main->valuestring);
        if (!aos_stat(resolved_path, &sb) && S_ISREG(sb.st_mode)) {
            cJSON_Delete(json);
            aos_free(pkg_data);
            return resolved_path;
        }
        snprintf(resolved_path, sizeof(resolved_path), "%s/%s.js", base,
                 item_main->valuestring);
        if (!aos_stat(resolved_path, &sb) && S_ISREG(sb.st_mode)) {
            cJSON_Delete(json);
            aos_free(pkg_data);
            return resolved_path;
        }
    }
    cJSON_Delete(json);
    aos_free(pkg_data);
    return NULL;
}

static duk_ret_t cb_resolve_module(duk_context *ctx)
{
    const char *module_id;
    const char *parent_id;

    module_id = duk_require_string(ctx, 0);
    parent_id = duk_require_string(ctx, 1);

    amp_debug(MOD_STR, "module_id: %s, parent_id: %s\n", module_id, parent_id);

    char *path = resolve_path(parent_id, module_id);
    if (!path) return duk_type_error(ctx, "cannot find module: %s", module_id);

    duk_push_string(ctx, path);
    amp_debug(MOD_STR, "resolve_cb: id:'%s', parent-id:'%s', resolve-to:'%s'\n", module_id,
          parent_id, duk_get_string(ctx, -1));

    return 1;
}

static duk_ret_t cb_load_module(duk_context *ctx)
{
    const char *filename;
    const char *module_id;
    int len;
    int i;

    module_id = duk_require_string(ctx, 0);
    duk_get_prop_string(ctx, 2, "filename");
    filename = duk_require_string(ctx, -1);

    amp_debug(MOD_STR, "id:'%s', filename:'%s'\n", module_id, filename);

#ifdef JSE_HIGHLEVEL_JSAPI
    /* find libjs entries */
    for(i = 0; i < libjs_num; i++){
        if(strcmp(LIBJS_ENTRIES[i].name, module_id) != 0){
            continue;
        }
        amp_debug(MOD_STR, "find libjs entry: %s", module_id);
        duk_push_lstring(ctx, LIBJS_ENTRIES[i].content, strlen(LIBJS_ENTRIES[i].content));
        return 1;
    }
#endif

    int fp = aos_open(filename, O_RDONLY);
    assert(fp >= 0);
    len = aos_lseek(fp, 0, HAL_SEEK_END);
    amp_debug(MOD_STR, "file: %s, size: %d\n", filename, len);
    aos_lseek(fp, 0, HAL_SEEK_SET);
    char *data = (char *)aos_malloc(len);
    if (!data) {
        aos_close(fp);
        amp_warn(MOD_STR, "cannot alloc memory to read module: %s", module_id);
        return duk_type_error(ctx, "cannot alloc memory to read module: %s",
                              module_id);
    }

    aos_read(fp, data, len);
    duk_push_lstring(ctx, data, len);
    aos_free(data);
    aos_close(fp);

    return 1;
}

static duk_ret_t handle_assert(duk_context *ctx)
{
    if (duk_to_boolean(ctx, 0)) return 0;

    const char *msg = duk_safe_to_string(ctx, 1);
    // fprintf(stderr, "assertion failed: %s\n", msg);
    // fflush(stderr);
    amp_error(MOD_STR,"assertion failed: %s", msg);

    duk_push_string(ctx, "");
    return duk_throw(ctx);
}

#if defined(JSE_HIGHLEVEL_JSAPI) && defined(JSE_CORE_ADDON_INITJS)
static void module_initjs_register(void)
{
    amp_debug(MOD_STR, "module_initjs_register");
    // amp_debug("%s\r\n", LIBJS_ENTRIES[libjs_num - 1].content);
    duk_context *ctx = be_get_context();
    duk_eval_string(ctx, LIBJS_ENTRIES[libjs_num - 1].content);
    duk_pop(ctx);
}
#endif

/*
 * register addons
 */
static void jsengine_register_addons()
{
    duk_context *ctx = be_get_context();

/** global Object */
#ifdef JSE_CORE_ADDON_BUILDIN
    module_builtin_register();
#endif
#ifdef JSE_CORE_ADDON_SYSTEM
    module_system_register();
#endif
#ifdef JSE_CORE_ADDON_SYSTIMER
    module_systimer_register();
#endif
#ifdef JSE_ADVANCED_ADDON_UND
    module_und_register();
#endif

    duk_push_object(ctx);

/** core component */
#ifdef JSE_CORE_ADDON_FS
    module_fs_register();
#endif
#ifdef JSE_CORE_ADDON_KV
    module_kv_register();
#endif
#ifdef JSE_CORE_ADDON_PM
    module_pm_register();
#endif
#ifdef JSE_CORE_ADDON_BATTERY
    module_battery_register();
#endif
#ifdef JSE_CORE_ADDON_CHARGER
    module_charger_register();
#endif
#ifdef JSE_CORE_ADDON_CHECKSUM
    module_checksum_register();
#endif
#ifdef JSE_CORE_ADDON_AT
    module_at_register();
#endif
#ifdef JSE_CORE_ADDON_LOG
    module_log_register();
#endif

#ifdef JSE_ADVANCED_ADDON_BLECFGNET
    module_blecfgnet_register();
#endif

#ifdef JSE_ADVANCED_ADDON_UI
    //module_vm_register();
    module_ui_register();
#endif

#ifdef JSE_ADVANCED_ADDON_KEYPAD
    module_keypad_register();
#endif

#ifdef JSE_CORE_ADDON_CRYPTO
    module_crypto_register();
#endif

/** network component */
#ifdef JSE_NET_ADDON_MQTT
    module_mqtt_register();
#endif
#ifdef JSE_NET_ADDON_NETMGR
    module_netmgr_register();
#endif
#ifdef JSE_NET_ADDON_WIFI
    module_wifi_register();
#endif
#ifdef JSE_NET_ADDON_CELLULAR
    module_cellular_register();
#endif
#ifdef JSE_NET_ADDON_UDP
    module_udp_register();
#endif
#ifdef JSE_NET_ADDON_TCP
    module_tcp_register();
#endif
#ifdef JSE_NET_ADDON_HTTP
    module_http_register();
#endif
#ifdef JSE_NET_ADDON_MIIO
    module_miio_register();
#endif

/** hardware component */
#ifdef JSE_HW_ADDON_ADC
    module_adc_register();
#endif
#ifdef JSE_HW_ADDON_DAC
    module_dac_register();
#endif
#ifdef JSE_HW_ADDON_CAN
    module_can_register();
#endif
#ifdef JSE_HW_ADDON_GPIO
    module_gpio_register();
#endif
#ifdef JSE_HW_ADDON_I2C
    module_i2c_register();
#endif
#ifdef JSE_HW_ADDON_SPI
    module_spi_register();
#endif
#ifdef JSE_HW_ADDON_TIMER
    module_timer_register();
#endif
#ifdef JSE_HW_ADDON_IR
    module_ir_register();
#endif
#ifdef JSE_HW_ADDON_LCD
    module_lcd_register();
#endif
#ifdef JSE_HW_ADDON_PWM
    module_pwm_register();
#endif
#ifdef JSE_HW_ADDON_ONEWIRE
    module_onewire_register();
#endif
#ifdef JSE_HW_ADDON_RTC
    module_rtc_register();
#endif
#ifdef JSE_HW_ADDON_UART
    module_uart_register();
#endif
#ifdef JSE_HW_ADDON_WDG
    module_wdg_register();
#endif

/** advanced component */
#ifdef JSE_ADVANCED_ADDON_AIOT_DEVICE
    module_aiot_device_register();
#endif
#ifdef JSE_ADVANCED_ADDON_AIOT_GATEWAY
    module_aiot_gateway_register();
#endif
#ifdef JSE_ADVANCED_ADDON_OTA
    module_app_ota_register();
#endif
#ifdef JSE_ADVANCED_ADDON_AUDIOPLAYER
    module_audioplayer_register();
#endif
#ifdef JSE_ADVANCED_ADDON_TTS
    module_tts_register();
#endif
#ifdef JSE_ADVANCED_ADDON_LOCATION
    module_location_register();
#endif
#ifdef JSE_ADVANCED_ADDON_PAYBOX
    module_paybox_register();
#endif
#ifdef JSE_ADVANCED_ADDON_SMARTCARD
    module_smartcard_register();
#endif

/** wireless component */
#ifdef JSE_WIRELESS_ADDON_BT_HOST
    module_bt_host_register();
#endif

/* init.js */
#if defined(JSE_HIGHLEVEL_JSAPI) && defined(JSE_CORE_ADDON_INITJS)
    module_initjs_register();
#endif

    duk_put_global_string(ctx, "__native");
}

void duk_debug_write_cb(long arg_level, const char *arg_file, long arg_line, const char *arg_func, const char *arg_msg){
    amp_debug(MOD_STR, "%s\r\n", arg_msg);
}
void jsengine_init()
{
    amp_debug(MOD_STR, "duktape jsengine_init\r\n");
    if (duk_ctx) {
        amp_warn(MOD_STR, "bone engine has been initialized\n");
        return;
    }
    duk_ctx = duk_create_heap_default();
    be_ref_setup(duk_ctx);
    duk_push_c_function(duk_ctx, handle_assert, 2);
    duk_put_global_string(duk_ctx, "assert");
    duk_push_object(duk_ctx);
    duk_push_c_function(duk_ctx, cb_resolve_module, DUK_VARARGS);
    duk_put_prop_string(duk_ctx, -2, "resolve");
    duk_push_c_function(duk_ctx, cb_load_module, DUK_VARARGS);
    duk_put_prop_string(duk_ctx, -2, "load");
    be_module_node_init(duk_ctx);
    amp_debug(MOD_STR, "duktape be_module_node_init\r\n");

    snprintf(node_modules_path, sizeof(node_modules_path), "/node_modules");
    // node_modules_path = JSE_FS_ROOT_DIR"/node_modules";
    /* register all addons */
    jsengine_register_addons();

#ifdef JSE_ADVANCED_ADDON_UI
    page_entry_register();
#endif

    /* register App() */
    app_entry_register();
    amp_debug(MOD_STR, "duktape jsengine_register_addons\r\n");

    if (g_repl_config) {
        repl_init();
    }
}

static duk_ret_t get_stack_raw(duk_context *ctx, void *udata)
{
    (void)udata;

    if (!duk_is_object(ctx, -1)) {
        return 1;
    }
    if (!duk_has_prop_string(ctx, -1, "stack")) {
        return 1;
    }
    if (!duk_is_error(ctx, -1)) {
        /* Not an Error instance, don't read "stack". */
        return 1;
    }

    duk_get_prop_string(ctx, -1, "stack"); /* caller coerces */
    duk_remove(ctx, -2);
    return 1;
}

static void print_pop_error(duk_context *ctx, FILE *f)
{
    /* Print error objects with a stack trace specially.
     * Note that getting the stack trace may throw an error
     * so this also needs to be safe call wrapped.
     */
    (void)duk_safe_call(ctx, get_stack_raw, NULL /*udata*/, 1 /*nargs*/,
                        1 /*nrets*/);
    amp_console("%s\n", duk_safe_to_stacktrace(ctx, -1));

    duk_pop(ctx);
}

static duk_ret_t wrapped_compile_execute(duk_context *ctx, void *udata)
{
    const char *src_data;
    duk_size_t src_len;
    duk_uint_t comp_flags;

    (void)udata;

    /* Use duk_compile_lstring_filename() variant which avoids interning
     * the source code.  This only really matters for low memory environments.
     */

    /* [ ... src_data src_len filename ] */
    src_data = (const char *)duk_require_pointer(ctx, -3);
    src_len  = (duk_size_t)duk_require_uint(ctx, -2);

    comp_flags = DUK_COMPILE_SHEBANG;
    duk_compile_lstring_filename(ctx, comp_flags, src_data, src_len);

    /* [ ... src_data src_len function ] */
    duk_push_global_object(ctx); /* 'this' binding */
    duk_call_method(ctx, 0);

    return 0; /* duk_safe_call() cleans up */
}

void jsengine_start(const char *js)
{
    assert(duk_ctx);
    if (!js) {
        amp_warn(MOD_STR, "js is null\n");
        return;
    }
    duk_push_pointer(duk_ctx, (void *)js);
    duk_push_uint(duk_ctx, (duk_uint_t)strlen(js));
    duk_push_string(duk_ctx, "eval");

    int ret = duk_safe_call(duk_ctx, wrapped_compile_execute, NULL /*udata*/,
                            3 /*nargs*/, 1 /*nret*/);
    if (ret != DUK_EXEC_SUCCESS) {
        print_pop_error(duk_ctx, stderr);
    } else {
        duk_pop(duk_ctx);
    }
}

void jsengine_eval_file(const char *filename)
{
    amp_debug(MOD_STR, "jsengine_eval_file entry");
    assert(filename);

    amp_debug(MOD_STR, "%s %s", "eval file: ", filename);

    /* set entry */
    be_module_node_set_entry(duk_ctx, filename);

    /* read & run js file */
    struct aos_stat sb;
    if (aos_stat(filename, &sb) || !S_ISREG(sb.st_mode)) {
        amp_warn(MOD_STR, "%s %s", "file not exist", filename);
        return;
    }
    amp_debug(MOD_STR, "%s %d", "file size: ", sb.st_size);
    file_data = (char *)aos_malloc(sb.st_size + 1);
    if (!file_data) {
        amp_warn(MOD_STR, "cannot alloc memory");
        return;
    }
    file_data[sb.st_size] = 0;

    int fp = aos_open(filename, O_RDONLY);
    aos_read(fp, file_data, sb.st_size);
    aos_close(fp);
    duk_push_pointer(duk_ctx, (void *)file_data);
    duk_push_uint(duk_ctx, (duk_uint_t)sb.st_size);
    duk_push_string(duk_ctx, filename);

    int ret = duk_safe_call(duk_ctx, wrapped_compile_execute, NULL /*udata*/,
                            3 /*nargs*/, 1 /*nret*/);
    if (ret != DUK_EXEC_SUCCESS) {
        print_pop_error(duk_ctx, stderr);
    } else {
        duk_pop(duk_ctx);
    }
}

void jsengine_exit()
{
    if (!duk_ctx) {
        amp_warn(MOD_STR, "jsengine has not been initialized");
        return;
    }
    if (file_data) {
        aos_free(file_data);
        file_data = NULL;
    }
    duk_destroy_heap(duk_ctx);
    // aos_free(node_modules_path);
    duk_ctx = NULL;
}

duk_context *be_get_context()
{
    return duk_ctx;
}

extern int64_t g_ntp_time;
extern int64_t g_up_time;
duk_double_t amp_date_get_now()
{
    int64_t uptime = aos_now_ms();
    g_ntp_time = g_ntp_time + (uptime - g_up_time);

    duk_double_t time = floor(g_ntp_time);

    g_ntp_time = g_ntp_time - (uptime - g_up_time);
    return time;
}
