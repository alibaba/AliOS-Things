/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdarg.h>

#include "jse_common.h"
#include "be.h"
#include "be_addon.h"
#include "be_api.h"
#include "be_module.h"

static be_jse_executor_ctx_t beJseExecutor;
static BE_JSE_FUNCTION_EXECUTE_CB be_user_ext_function_cb = 0;
static BE_JSE_FUNCTION_EXECUTE_CB be_jse_module_cb        = 0;

#ifdef SUPPORT_NODE_MODELES

static struct be_list_head be_jse_dirname_head;

struct be_list_head *bone_engine_dirname_get_head()
{
    return &be_jse_dirname_head;
}

/* push 一个dirname */
void bone_engine_dirname_push(BE_JSE_DIRNAME_s *dirPtr)
{
    be_list_add(&dirPtr->lst, &be_jse_dirname_head);
}

/* get dirname */
BE_JSE_DIRNAME_s *bone_engine_dirname_get()
{
    if (be_list_empty(&be_jse_dirname_head)) {
        return NULL;
    }

    BE_JSE_DIRNAME_s *dirPtr;
    dirPtr = be_list_first_entry(&be_jse_dirname_head, BE_JSE_DIRNAME_s, lst);

    return dirPtr;
}

/* pop and delete dirname */
void bone_engine_dirname_pop_del()
{
    if (be_list_empty(&be_jse_dirname_head)) {
        return;
    }

    BE_JSE_DIRNAME_s *dirPtr;
    dirPtr = be_list_first_entry(&be_jse_dirname_head, BE_JSE_DIRNAME_s, lst);

    be_list_del(&dirPtr->lst);

    jse_free(dirPtr->dirname);
    jse_free(dirPtr);
}

#endif

be_jse_symbol_t *handle_function_call(be_jse_vm_ctx_t *execInfo,
                                      be_jse_symbol_t *var, const char *name)
{
    be_jse_symbol_t *value = BE_JSE_FUNC_UNHANDLED;

    /* search in addons */
    if (be_jse_module_cb) {
        value = be_jse_module_cb(execInfo, var, name);
        if (value != BE_JSE_FUNC_UNHANDLED) {
            return value;
        }
    }

    /* search in global module */
    if (be_user_ext_function_cb && (var == NULL)) {
        value = be_user_ext_function_cb(execInfo, var, name);
        if (value != BE_JSE_FUNC_UNHANDLED) {
            return value;
        }
    }

    return value;
}

be_jse_executor_ctx_t *jsengine_get_executor()
{
    return &beJseExecutor;
}

void jsengine_native_function_register(BE_JSE_FUNCTION_EXECUTE_CB extension)
{
    be_user_ext_function_cb = extension;
}

void jsengine_modules_register(BE_JSE_FUNCTION_EXECUTE_CB extension)
{
    be_jse_module_cb = extension;
}

void jsengine_register_addons()
{
#ifdef JSE_CORE_ADDON_BUILDIN
    module_builtin_load();
#endif

#ifdef JSE_CORE_ADDON_PROCESS
    module_process_load();
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

#ifdef JSE_NET_ADDON_HTTP
    module_http_register();
#endif

#ifdef JSE_NET_ADDON_NET
    module_net_register();
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
    be_jse_symbol_table_init();

    be_jse_executor_init(&beJseExecutor);
#ifdef SUPPORT_NODE_MODELES
    BE_INIT_LIST_HEAD(&be_jse_dirname_head);
#endif
    /* register all addons */
    jsengine_register_addons();
}

/*
    call after jsengine_init()
*/
be_jse_node_t jsengine_native_object_register(const char *objectName)
{
    be_jse_node_t objRef = 0;
    be_jse_symbol_t *name;

    name = lookup_named_child_symbol(beJseExecutor.root, objectName, true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child)
        name->first_child =
            symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    objRef = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    return objRef;
}

void jsengine_exit()
{
#if 0
#ifdef TRACE_JSE_INFO
        trace_symbol_info(beJseExecutor.root, 0);
#endif

#ifdef DUMP_SYMBL_USAGE
        jse_warn("befor: %d Memory Records Used\n",
        be_jse_get_memory_usage());
#endif
       be_jse_executor_deinit(&beJseExecutor);

#ifdef DUMP_SYMBL_USAGE
        jse_warn("after: %d Memory Records Used (should be0!)\n", be_jse_get_memory_usage());
    
        be_jse_show_symbol_table_used();
#endif
#else
    be_jse_executor_deinit(&beJseExecutor);
#endif

    be_jse_symbol_table_deinit();

#ifdef SUPPORT_NODE_MODELES
    bone_engine_dirname_pop_del();
#endif
}

void jsengine_start(const char *js)
{
    be_jse_symbol_t *value;

    value = be_jse_eval_string(&beJseExecutor, js);
    symbol_unlock(value);
}

#if 0
void bone_engine_restart(const char *js)
{
    be_jse_symbol_t *value;

    jsengine_exit();
    jsengine_init();
    value = be_jse_eval_string(&beJseExecutor, js);
    symbol_unlock(value);
}
#endif

void jsengine_save_dirname(const char *name)
{
#ifdef SUPPORT_NODE_MODELES
    BE_JSE_DIRNAME_s *firstDir = jse_calloc(1, sizeof(BE_JSE_DIRNAME_s));

    /* add "./" */
    char *tmpPath = jse_calloc(1, 3 + strlen(name));
    sprintf(tmpPath, "./%s", name);
    char *clearPath   = getClearPath(tmpPath);
    firstDir->dirname = getFilePath(clearPath);
    bone_engine_dirname_push(firstDir);
    jse_free(tmpPath);
    jse_free(clearPath);
#endif
}

/**
 *
 * get JS file content
 *
 */
const char *load_js_module(const char *moduleName)
{
    int fd = 0;
    int len;
    int size;
    char path[128] = {0};
    char *data     = NULL;

    if (moduleName == NULL) {
        return NULL;
    }

    jse_debug("moduleName =  %s \n", moduleName);

    if (moduleName[0] == '.') {
        if (moduleName[1] == '/') {
            snprintf(path, sizeof(path), "%s", moduleName + 2);
        }
    } else {
        snprintf(path, sizeof(path), "%s", moduleName);
    }

    jse_debug("path =  %s \n", path);

    /* board_mgr */
    /* board_load_drivers(path); */

    fd = jse_open(path, O_RDONLY);
    if (fd == -1) {
        strcat(path, ".js");
        fd = jse_open(path, O_RDONLY);
    }

    if (fd == -1) {
        jse_error("load %s fail \r\n", moduleName);
        return NULL;
    }

    
    if((size = jse_lseek(fd, 0L, SEEK_END)) <= 0){
        jse_error("be_lseek error:%d\r\n", size);
        return NULL;
    }
    jse_lseek(fd, 0L, SEEK_SET);
    jse_debug("%s file size = %d \r\n", path, size);

    data    = jse_calloc(1, size + 1);
    data[0] = 0;
    len     = jse_read(fd, data, size);
    jse_debug("read, len = %d \r\n", len);
    if (len > 0) {
        data[len] = 0;
    } else {
        jse_debug("read, errno = %d\n", errno);
        jse_debug("%s\n", strerror(errno));
    }

    jse_close(fd);
    return data;
}

void jsengine_eval_file(const char *filename)
{
    char *data;

    jsengine_loadmodule_register(load_js_module);

    if (filename) {
        /* record __dirname */
        jsengine_save_dirname(filename);

        jse_debug("load_js_module: %s \r\n", filename);

        data = (char *)load_js_module(filename);
        if (data) {
            jse_debug("Run Js File: %s \r\n", filename);
            jse_debug("date length = %d \r\n", strlen(data));

            /* execute JS code */
            jsengine_start(data);
            jse_free(data);
        }
    }
}