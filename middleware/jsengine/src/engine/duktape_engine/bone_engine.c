/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <assert.h>

#include "jse_port.h"
#include "bone_engine_inl.h"
#include "cJSON.h"
#ifdef JSE_IDE_DEBUG
#include "websocket.h"
#endif

#define MAX_PATH_SIZE 1024

static duk_context *duk_ctx;
static char *node_modules_path;
static char resolved_path[MAX_PATH_SIZE];

static char *resolve_path(const char *parent, const char *module_id)
{
    char base[MAX_PATH_SIZE];

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
                    jse_warn("resolve path fail\n");
                    break;
                }
            }
        }
        strcat(base, "/");
        strcat(base, p);
    }

    struct stat sb;
    int ret = jse_stat(base, &sb);
    jse_debug("base: %s, ret: %d\n", base, ret);
    if (!ret && S_ISREG(sb.st_mode)) {
        strcpy(resolved_path, base);
        return resolved_path;
    }

    snprintf(resolved_path, sizeof(resolved_path), "%s.js", base);
    ret = jse_stat(resolved_path, &sb);
    jse_debug("resolved_path: %s, ret: %d\n", resolved_path, ret);
    if (!ret && S_ISREG(sb.st_mode)) {
        return resolved_path;
    }

    snprintf(resolved_path, sizeof(resolved_path), "%s/index.js", base);
    ret = jse_stat(resolved_path, &sb);
    jse_debug("resolved_path: %s, ret: %d\n", resolved_path, ret);
    if (!ret && S_ISREG(sb.st_mode)) {
        return resolved_path;
    }

    snprintf(resolved_path, sizeof(resolved_path), "%s/package.json", base);
    ret = jse_stat(resolved_path, &sb);
    jse_debug("resolved_path: %s, ret: %d\n", resolved_path, ret);
    if (ret || !S_ISREG(sb.st_mode)) {
        jse_warn("file %s not exist\n", resolved_path);
        return NULL;
    }
    /* parse package.json */
    char *pkg_data = jse_malloc(sb.st_size + 1);
    if (!pkg_data) {
        jse_warn("cannot alloc memory to read package.json for module: %s",
             module_id);
        return NULL;
    }

    int fd = jse_open(resolved_path, O_RDONLY);
    jse_read(fd, pkg_data, sb.st_size);
    jse_close(fd);
    pkg_data[sb.st_size] = '\0';

    cJSON *json      = cJSON_Parse(pkg_data);
    cJSON *item_main = cJSON_GetObjectItemCaseSensitive(json, "main");
    jse_debug("item_main: %p\n", item_main);
    if (cJSON_IsString(item_main) && item_main->valuestring) {
        snprintf(resolved_path, sizeof(resolved_path), "%s/%s", base,
                 item_main->valuestring);
        if (!jse_stat(resolved_path, &sb) && S_ISREG(sb.st_mode)) {
            cJSON_Delete(json);
            jse_free(pkg_data);
            return resolved_path;
        }
        snprintf(resolved_path, sizeof(resolved_path), "%s/%s.js", base,
                 item_main->valuestring);
        if (!jse_stat(resolved_path, &sb) && S_ISREG(sb.st_mode)) {
            cJSON_Delete(json);
            jse_free(pkg_data);
            return resolved_path;
        }
    }
    cJSON_Delete(json);
    jse_free(pkg_data);
    return NULL;
}

static duk_ret_t cb_resolve_module(duk_context *ctx)
{
    const char *module_id;
    const char *parent_id;

    module_id = duk_require_string(ctx, 0);
    parent_id = duk_require_string(ctx, 1);

    jse_debug("module_id: %s, parent_id: %s\n", module_id, parent_id);

    char *path = resolve_path(parent_id, module_id);
    if (!path) return duk_type_error(ctx, "cannot find module: %s", module_id);

    duk_push_string(ctx, path);
    jse_debug("resolve_cb: id:'%s', parent-id:'%s', resolve-to:'%s'\n", module_id,
          parent_id, duk_get_string(ctx, -1));

    return 1;
}

static duk_ret_t cb_load_module(duk_context *ctx)
{
    const char *filename;
    const char *module_id;

    module_id = duk_require_string(ctx, 0);
    duk_get_prop_string(ctx, 2, "filename");
    filename = duk_require_string(ctx, -1);

    jse_debug("id:'%s', filename:'%s'\n", module_id, filename);

    int fd = jse_open(filename, O_RDONLY);
    assert(fd > 0);
    int len = be_lseek(fd, 0, SEEK_END);
    jse_debug("file: %s, size: %d\n", filename, len);
    be_lseek(fd, 0, SEEK_SET);
    char *data = (char *)jse_malloc(len);
    if (!data) {
        jse_close(fd);
        jse_warn("cannot alloc memory to read module: %s", module_id);
        return duk_type_error(ctx, "cannot alloc memory to read module: %s",
                              module_id);
    }

    jse_read(fd, data, len);
    duk_push_lstring(ctx, data, len);
    jse_free(data);
    jse_close(fd);

    return 1;
}

static duk_ret_t handle_assert(duk_context *ctx)
{
    if (duk_to_boolean(ctx, 0)) return 0;

    const char *msg = duk_safe_to_string(ctx, 1);
    fprintf(stderr, "assertion failed: %s\n", msg);
    fflush(stderr);

    duk_push_string(ctx, "");
    return duk_throw(ctx);
}

/*
 * register addons
 */
static void jsengine_register_addons()
{
#ifdef JSE_CORE_ADDON_BUILDIN
    module_builtin_register();
#endif
#ifdef JSE_CORE_ADDON_PROCESS
    module_process_register();
#endif
#ifdef JSE_CORE_ADDON_TIMER
    module_timer_register();
#endif
#ifdef JSE_NET_ADDON_MQTT
    module_mqtt_register();
#endif
#ifdef JSE_HW_ADDON_WIFI
    module_wifi_register();
#endif
#ifdef JSE_HW_ADDON_ADC
    module_adc_register();
#endif
#ifdef JSE_HW_ADDON_DAC
    module_dac_register();
#endif
#ifdef JSE_HW_ADDON_GPIO
    module_gpio_register();
#endif
#ifdef JSE_HW_ADDON_I2C
    module_i2c_register();
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
#ifdef JSE_HW_ADDON_RTC
    module_rtc_register();
#endif
#ifdef JSE_HW_ADDON_UART
    module_uart_register();
#endif
#ifdef JSE_HW_ADDON_WDG
    module_wdg_register();
#endif
#ifdef JSE_NET_ADDON_UDP
    module_udp_register();
#endif
#ifdef JSE_CORE_ADDON_FS
    module_fs_register();
#endif
#ifdef JSE_NET_ADDON_MIIO
    module_miio_register();
#endif
}

void jsengine_init()
{
    if (duk_ctx) {
        jse_warn("bone engine has been initialized\n");
        return;
    }
    duk_ctx = duk_create_heap_default();

    bone_engine_ref_setup(duk_ctx);

    duk_push_c_function(duk_ctx, handle_assert, 2);
    duk_put_global_string(duk_ctx, "assert");

    duk_push_object(duk_ctx);
    duk_push_c_function(duk_ctx, cb_resolve_module, DUK_VARARGS);
    duk_put_prop_string(duk_ctx, -2, "resolve");
    duk_push_c_function(duk_ctx, cb_load_module, DUK_VARARGS);
    duk_put_prop_string(duk_ctx, -2, "load");
    be_module_node_init(duk_ctx);

    char tmp[MAX_PATH_SIZE];
    snprintf(tmp, sizeof(tmp), "%s/node_modules", JSE_FS_ROOT_DIR);
    node_modules_path = strdup(tmp);

    /* register all addons */
    jsengine_register_addons();
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
    const char *msg = duk_safe_to_string(ctx, -1);
    fprintf(f, "%s\n", msg);
    fflush(f);

#ifdef JSE_IDE_DEBUG
    /* send to IDE */
    char *buf = (char *)jse_malloc(sizeof(BonePrefix) + strlen(msg));
    sprintf(buf, BonePrefix "%s", msg);
    bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_ERROR, (char *)buf);
    jse_free(buf);
#endif

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
        jse_warn("js is null\n");
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
    assert(filename && filename[0] == '/');

    jse_debug("eval file: %s\n", filename);

    /* set entry */
    be_module_node_set_entry(duk_ctx, filename);

    /* read & run js file */
    struct aos_stat sb;
    if (jse_stat(filename, &sb) || !S_ISREG(sb.st_mode)) {
        jse_warn("file: %s not exist\n", filename);
        return;
    }
    jse_debug("file size: %d\n", sb.st_size);
    char *data = (char *)jse_malloc(sb.st_size);
    if (!data) {
        jse_warn("cannot alloc memory\n");
        return;
    }

    int fd = jse_open(filename, O_RDONLY);
    jse_read(fd, data, sb.st_size);
    jse_close(fd);

    duk_push_pointer(duk_ctx, (void *)data);
    duk_push_uint(duk_ctx, (duk_uint_t)sb.st_size);
    duk_push_string(duk_ctx, filename);

    int ret = duk_safe_call(duk_ctx, wrapped_compile_execute, NULL /*udata*/,
                            3 /*nargs*/, 1 /*nret*/);
    if (ret != DUK_EXEC_SUCCESS) {
        print_pop_error(duk_ctx, stderr);
    } else {
        duk_pop(duk_ctx);
    }

    jse_free(data);
}

void jsengine_exit()
{
    if (!duk_ctx) {
        jse_warn("jsengine has not been initialized\n");
        return;
    }
    duk_destroy_heap(duk_ctx);
    jse_free(node_modules_path);
    duk_ctx = NULL;
}

duk_context *bone_engine_get_context()
{
    return duk_ctx;
}
