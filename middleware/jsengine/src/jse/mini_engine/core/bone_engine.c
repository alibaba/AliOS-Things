/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <errno.h>
#include "be_jse.h"
#include "be_jse_addon.h"
#include "be_jse_api.h"
#include "be_jse_module.h"
#include "be_utils.h"
#include "hal/system.h"
#include "board_mgr.h"

#define TAG "bone_engine"
static be_jse_executor_ctx_t beJseExecutor;
static BE_JSE_FUNCTION_EXECUTE_CB be_user_ext_function_cb = 0;
static BE_JSE_FUNCTION_EXECUTE_CB be_jse_module_cb = 0;

#ifdef SUPPORT_NODE_MODELES

static struct be_list_head be_jse_dirname_head;

struct be_list_head *bone_engine_dirname_get_head() {
    return &be_jse_dirname_head;
}

/* push 一个dirname */
void bone_engine_dirname_push(BE_JSE_DIRNAME_s *dirPtr) {
    be_list_add(&dirPtr->lst, &be_jse_dirname_head);
}

/* 获取最新dirname */
BE_JSE_DIRNAME_s *bone_engine_dirname_get() {
    if (be_list_empty(&be_jse_dirname_head)) {
        return NULL;
    }

    BE_JSE_DIRNAME_s *dirPtr;
    dirPtr = be_list_first_entry(&be_jse_dirname_head, BE_JSE_DIRNAME_s, lst);

    return dirPtr;
}

/* pop并del 最新的dirname */
void bone_engine_dirname_pop_del() {
    if (be_list_empty(&be_jse_dirname_head)) {
        return;
    }

    BE_JSE_DIRNAME_s *dirPtr;
    dirPtr = be_list_first_entry(&be_jse_dirname_head, BE_JSE_DIRNAME_s, lst);

    be_list_del(&dirPtr->lst);

    free(dirPtr->dirname);
    free(dirPtr);
}

#endif

be_jse_symbol_t *handle_function_call(be_jse_vm_ctx_t *execInfo,
                                      be_jse_symbol_t *var, const char *name) {
    be_jse_symbol_t *value = BE_JSE_FUNC_UNHANDLED;

    /* 在addons中查找 */
    if (be_jse_module_cb) {
        value = be_jse_module_cb(execInfo, var, name);
        if (value != BE_JSE_FUNC_UNHANDLED) {
            return value;
        }
    }

    /* 全局扩展对象查找 */
    if (be_user_ext_function_cb && (var == NULL)) {
        value = be_user_ext_function_cb(execInfo, var, name);
        if (value != BE_JSE_FUNC_UNHANDLED) {
            return value;
        }
    }

    return value;
}

be_jse_executor_ctx_t *bone_engine_get_executor() { return &beJseExecutor; }

void bone_engine_native_function_register(BE_JSE_FUNCTION_EXECUTE_CB extension) {
    be_user_ext_function_cb = extension;
}

void bone_engine_modules_register(BE_JSE_FUNCTION_EXECUTE_CB extension) {
    be_jse_module_cb = extension;
}

/**
 *
 * 根据Kconfig配置加载所有的addon
 *
 */
void bone_engine_load_addon() {

#ifdef JSE_CORE_ADDON_BUILDIN
	module_builtin_load();
#endif

#ifdef JSE_CORE_ADDON_PROCESS
	module_process_load();
#endif

#ifdef JSE_HW_ADDON_MQTT
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

#ifdef JSE_HW_ADDON_HTTP
	module_http_register();
#endif

#ifdef JSE_HW_ADDON_NET
	module_net_register();
#endif

#ifdef JSE_HW_ADDON_UDP
	module_udp_register();
#endif

#ifdef JSE_HW_ADDON_FS
	module_fs_register();
#endif

#ifdef JSE_HW_ADDON_MIIO
	module_miio_register();
#endif

}

void bone_engine_init() {
    be_jse_symbol_table_init();

    be_jse_executor_init(&beJseExecutor);
#ifdef SUPPORT_NODE_MODELES
    BE_INIT_LIST_HEAD(&be_jse_dirname_head);
#endif
    /* 注册所有addon */
    bone_engine_load_addon();
}

/*
    必须在bone_engine_init之后调用
*/
be_jse_node_t bone_engine_native_object_register(const char *objectName) {
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

void bone_engine_exit() {
#if 0
#ifdef TRACE_JSE_INFO
        trace_symbol_info(beJseExecutor.root, 0);
#endif

#ifdef DUMP_SYMBL_USAGE
        be_warn("bone_engine","befor: %d Memory Records Used\n",
        be_jse_get_memory_usage());
#endif
       be_jse_executor_deinit(&beJseExecutor);

#ifdef DUMP_SYMBL_USAGE
        be_warn("bone_engine","after: %d Memory Records Used (should be0!)\n", be_jse_get_memory_usage());
    
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

void bone_engine_start(const char *js) {
    be_jse_symbol_t *value;

    value = be_jse_eval_string(&beJseExecutor, js);
    symbol_unlock(value);
}

#if 0
void bone_engine_restart(const char *js)
{
    be_jse_symbol_t *value;

    bone_engine_exit();
    bone_engine_init();
    value = be_jse_eval_string(&beJseExecutor, js);
    symbol_unlock(value);
}
#endif

void bone_engine_save_dirname(const char *name) {
#ifdef SUPPORT_NODE_MODELES
    BE_JSE_DIRNAME_s *firstDir = calloc(1, sizeof(BE_JSE_DIRNAME_s));

    /* 加上"./" */
    char *tmpPath = calloc(1, 3 + strlen(name));
    sprintf(tmpPath, "./%s", name);
    char *clearPath = getClearPath(tmpPath);
    firstDir->dirname = getFilePath(clearPath);
    bone_engine_dirname_push(firstDir);
    free(tmpPath);
    free(clearPath);
#endif
}

void bone_engine_set_log_cb(BE_JSE_FUNCTION_LOG_CB cb) { be_set_log(cb); }

/**
 *
 * 获取JS文件的内容
 *
 */
const char *load_js_module(const char *moduleName) {
    int fd = 0;
    int len;
    int size;
    char path[128] = {0};
    char *data = NULL;

    if (moduleName == NULL) {
        return NULL;
    }

    be_debug(TAG, "moduleName =  %s \n", moduleName);

    if (moduleName[0] == '.') {
        if (moduleName[1] == '/') {
            snprintf(path, sizeof(path), "%s", moduleName + 2);
        }
    } else {
        snprintf(path, sizeof(path), "%s", moduleName);
    }

    be_debug(TAG, "path =  %s \n", path);

    /* board_mgr */
    /* board_load_drivers(path); */
    
    fd = be_open(path, O_RDONLY);
    if (fd == -1) {
        strcat(path, ".js");
        fd = be_open(path, O_RDONLY);
    }

    if (fd == -1) {
        printf("load %s fail \r\n", moduleName);
        return NULL;
    }

    size = be_lseek(fd, 0L, SEEK_END);
    be_lseek(fd, 0L, SEEK_SET);
    printf("%s file size = %d \r\n", path, size);

    data = calloc(1, size + 1);
    data[0] = 0;
    len = be_read(fd, data, size);
    printf("read, len = %d \r\n", len);
    if (len > 0) {
        data[len] = 0;
    } else {
        printf("read, errno = %d\n", errno);
        printf("%s\n", strerror(errno));
    }

    be_close(fd);
    return data;
}

void bone_engine_eval_file(const char *filename) {
    char *data;

    bone_engine_loadmodule_register(load_js_module);

    if (filename) {
        /* 记录 __dirname */
        bone_engine_save_dirname(filename);

        printf("load_js_module: %s \r\n", filename);

        data = (char *)load_js_module(filename);
        if (data) {
            printf("Run Js File: %s \r\n", filename);
            printf("date length = %d \r\n", strlen(data));

            /* 运行JS语法块 */
            bone_engine_start(data);
            free(data);
        }
    }
}