/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_API_H
#define BE_API_H

#include "jse_common.h"
#include "be.h"

typedef be_jse_symbol_t *(*BE_JSE_FUNCTION_EXECUTE_CB)(
    be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *a, const char *name);

be_jse_symbol_t *handle_function_call(be_jse_vm_ctx_t *execInfo,
                                      be_jse_symbol_t *a, const char *name);

/*
 *
 * get JSEngine executor object
 *
 */
be_jse_executor_ctx_t *jsengine_get_executor(void);

/*
    register custom JS Object, call after jsengine_init().
*/
be_jse_node_t jsengine_native_object_register(const char *objectName);

/*
    register custom JS native function.
*/
void jsengine_native_function_register(BE_JSE_FUNCTION_EXECUTE_CB extension);

void jsengine_modules_register(BE_JSE_FUNCTION_EXECUTE_CB extension);

/*
void bone_engine_log(const char* file, const char* tag,
                     int level, int line, const char* func,
                     const char* format, ...);

void bone_engine_set_log(BE_JSE_FUNCTION_LOG_CB cb); */

#ifdef __cplusplus
}
#endif

#endif /* BE_API_H */
