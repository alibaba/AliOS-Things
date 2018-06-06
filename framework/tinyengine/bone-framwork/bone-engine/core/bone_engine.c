/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include "be_jse.h"
#include "be_jse_api.h"

static be_jse_executor_ctx_t beJseExecutor;
static BE_JSE_FUNCTION_EXECUTE_CB be_user_ext_function_cb = 0;
static BE_JSE_FUNCTION_EXECUTE_CB be_jse_module_cb = 0;
static BE_JSE_FUNCTION_LOG_CB jse_log = 0;

be_jse_symbol_t *handle_function_call(be_jse_vm_ctx_t *execInfo,be_jse_symbol_t *var, const char *name) 
{
    be_jse_symbol_t *value = BE_JSE_FUNC_UNHANDLED;

    // 在addons中查找
    if (be_jse_module_cb) {
        value = be_jse_module_cb(execInfo, var, name);
        if (value != BE_JSE_FUNC_UNHANDLED) {
            return value;
        }
    }

    // 全局扩展对象查找
    if (be_user_ext_function_cb && (var == NULL)) {
        value = be_user_ext_function_cb(execInfo, var, name);
        if (value != BE_JSE_FUNC_UNHANDLED) {
            return value;
        }
    }

    return value;
}

be_jse_executor_ctx_t *bone_engine_get_executor() { return &beJseExecutor; }

void bone_engine_native_function_register(BE_JSE_FUNCTION_EXECUTE_CB extension) 
{
    be_user_ext_function_cb = extension;
}

void bone_engine_modules_register(BE_JSE_FUNCTION_EXECUTE_CB extension) 
{
    be_jse_module_cb = extension;
}

void bone_engine_init() 
{
    be_jse_symbol_table_init();

    be_jse_executor_init(&beJseExecutor);
}

/*
    必须在bone_engine_init之后调用
*/
be_jse_node_t bone_engine_native_object_register(const char *objectName) 
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

void bone_engine_exit() 
{
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
#endif

    be_jse_symbol_table_deinit();
}

void bone_engine_start(const char *js) 
{
    be_jse_symbol_t *value;

    value = be_jse_eval_string(&beJseExecutor, js);
    symbol_unlock(value);
}

void bone_engine_restart(const char *js) 
{
    be_jse_symbol_t *value;

    bone_engine_exit();
    bone_engine_init();
    value = be_jse_eval_string(&beJseExecutor, js);
    symbol_unlock(value);
}

void bone_engine_log(const char *file, const char *tag, int level, int line,
                     const char *func, const char *format, ...)
{
    if (jse_log) {
        char body[512] = {0};
        va_list args;
        va_start(args, format);
        vsnprintf(body, sizeof(body), format, args);
        va_end(args);
        jse_log(file, tag, level, line, func, body);
    }
}

void bone_engine_set_log(BE_JSE_FUNCTION_LOG_CB cb) 
{
    jse_log = cb; 
}
