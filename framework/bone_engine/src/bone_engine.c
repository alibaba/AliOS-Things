/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "be_jse.h"
#include "be_jse_api.h"

#ifdef USE_AOS
#include "aos/aos.h"   // LOGW LOGD
#endif

#include "be_bi_module.h"
#include "be_iot_module.h"


static be_jse_executor_ctx_t beJseExecutor;
static BE_JSE_FUNCTION_EXECUTE_CB be_user_ext_function_cb = 0;

be_jse_executor_ctx_t* bone_engine_get_executor()
{
    return &beJseExecutor;
}

void bone_engine_native_function_register(BE_JSE_FUNCTION_EXECUTE_CB extension)
{
    be_user_ext_function_cb = extension;
}

be_jse_symbol_t *be_native_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    be_jse_symbol_t* value = BE_JSE_FUNC_UNHANDLED;

    // modules 对象扩展
    value = be_iot_module_handle_cb(execInfo, var, name);
    if( value != BE_JSE_FUNC_UNHANDLED )
        return value;

    // 用户注册的对扩展
    if (be_user_ext_function_cb) {
        value = be_user_ext_function_cb(execInfo, var, name);
        if (value != BE_JSE_FUNC_UNHANDLED)
            return value;
    }

    return value;
}

void bone_engine_init()
{
    be_jse_symbol_table_init();

    be_jse_executor_init(&beJseExecutor);
    be_module_init(&beJseExecutor);

    be_jse_register_function_cb(be_native_module_handle_cb);
}

/*
    必须在bone_engine_init之后调用
*/
be_jse_node_t bone_engine_native_object_register(const char* objectName)
{
    be_jse_node_t objRef = 0;
    be_jse_symbol_t *name;

    name = lookup_named_child_symbol(beJseExecutor.root, objectName, true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    objRef = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    return objRef;
}

void bone_engine_exit()
{

    be_module_exit();

#ifdef TRACE_JSE_INFO
    trace_symbol_info(beJseExecutor.root, 0);
#endif

#ifdef DUMP_SYMBL_USAGE
    LOGW("bone_engine","befor: %d Memory Records Used\n", be_jse_get_memory_usage());
#endif
    be_jse_executor_deinit(&beJseExecutor);

#ifdef DUMP_SYMBL_USAGE
    LOGW("bone_engine","after: %d Memory Records Used (should be 0!)\n", be_jse_get_memory_usage());

    be_jse_show_symbol_table_used();
#endif


}

void bone_engine_start(const char* js)
{
    be_jse_symbol_t* value;

    value = be_jse_eval_string(&beJseExecutor, js);

    symbol_unlock(value);
}

void bone_engine_restart(const char* js)
{
    be_jse_symbol_t* value;

    bone_engine_exit();

    bone_engine_init();

    value = be_jse_eval_string(&beJseExecutor, js);

    symbol_unlock(value);

}



