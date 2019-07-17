/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "be_jse_module.h"
#include "be_jse_api.h"
#include "be_jse_global.h"

static int gModulesCount = 0;
static be_jse_node_t gModulesRef[BE_JSE_MAX_MODULES];
static BE_JSE_FUNCTION_EXECUTE_CB gModules[BE_JSE_MAX_MODULES];
static int gModulesInited = 0;

static BE_JSE_FUNCTION_EXECUTE_CB be_jse_find_modules(be_jse_node_t objRef) {
    int i;
    for (i = 0; i < gModulesCount; i++) {
        if (objRef == gModulesRef[i]) return gModules[i];
    }
    return NULL;
}

be_jse_symbol_t *be_jse_module_process(be_jse_vm_ctx_t *execInfo,
                                       be_jse_symbol_t *var, const char *name) {
    be_jse_node_t objRef = get_symbol_node_id(var);

    /* global function */
    if (objRef == 0) return BE_JSE_FUNC_UNHANDLED;

        /* string/array 可以多实例，处理方法不一样 */
#ifdef USE_STRING
    if (symbol_is_string(var)) {
        be_jse_symbol_t *objName = lookup_named_child_symbol(
            bone_engine_get_executor()->root, "String", false);
        if (objName) objRef = objName->first_child;
        symbol_unlock(objName);
    }
#endif

#ifdef USE_ARRAY
    if (symbol_is_array(var)) {
        be_jse_symbol_t *objName = lookup_named_child_symbol(
            bone_engine_get_executor()->root, "Array", false);
        if (objName) objRef = objName->first_child;
        symbol_unlock(objName);
    }
#endif

    BE_JSE_FUNCTION_EXECUTE_CB cb = be_jse_find_modules(objRef);

    return cb ? cb(execInfo, var, name) : BE_JSE_FUNC_UNHANDLED;
}

void be_jse_module_load(const char *moduleName,
                        BE_JSE_FUNCTION_EXECUTE_CB extension) {
    if (!moduleName || strlen(moduleName) == 0) {
        return;
    }

    /* 模块索引初始化 */
    if (!gModulesInited) {
        memset(gModules, 0, sizeof(gModules));
        gModulesInited = 1;
    }

    /* 判断模块是否已加载 */
    be_jse_symbol_t *name = lookup_named_child_symbol(
        bone_engine_get_executor()->root, moduleName, false);
    be_jse_node_t objRef = BE_JSE_MAX_MODULES;
    if (!name) {
        objRef = bone_engine_native_object_register(moduleName);
    } else {
        objRef = name->first_child;
    }

    /* printf("[%s][%s][%d] moduleName=%s objRef = %p \n", __FILE__,
       __FUNCTION__, __LINE__, moduleName, objRef); */

    if (gModulesCount + 1 < BE_JSE_MAX_MODULES) {
        gModules[gModulesCount]    = extension;
        gModulesRef[gModulesCount] = objRef;
        gModulesCount++;
    }

    symbol_unlock(name);

    bone_engine_modules_register(be_jse_module_process);
}
