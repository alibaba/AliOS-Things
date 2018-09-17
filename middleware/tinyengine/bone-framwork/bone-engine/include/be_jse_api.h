/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#ifndef BE_JSE_API_H_
#define BE_JSE_API_H_

#include "be_jse.h"

enum BE_JSE_LOG_LEVEL {
    BE_JSE_LOG_LEVEL_DEBUG = (1 << 0),
    BE_JSE_LOG_LEVEL_INFO  = (1 << 1),
    BE_JSE_LOG_LEVEL_WARN  = (1 << 2),
    BE_JSE_LOG_LEVEL_ERROR = (1 << 3),
};

typedef void (*BE_JSE_FUNCTION_LOG_CB)(const char* file, const char* tag,
                                       int level, int line, const char* func,
                                       const char* format, ...);

#define jse_debug(tag, fmt, agrs...) bone_engine_log(__FILE__, tag, BE_JSE_LOG_LEVEL_DEBUG, __LINE__, __func__, fmt, ##agrs)
#define jse_info(tag, fmt, agrs...)  bone_engine_log(__FILE__, tag, BE_JSE_LOG_LEVEL_INFO,  __LINE__, __func__, fmt, ##agrs)
#define jse_warn(tag, fmt, agrs...)  bone_engine_log(__FILE__, tag, BE_JSE_LOG_LEVEL_WARN,  __LINE__, __func__, fmt, ##agrs)
#define jse_error(tag, fmt, agrs...) bone_engine_log(__FILE__, tag, BE_JSE_LOG_LEVEL_ERROR, __LINE__, __func__, fmt, ##agrs)

typedef be_jse_symbol_t *(*BE_JSE_FUNCTION_EXECUTE_CB)(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *a, const char *name);

be_jse_symbol_t *handle_function_call(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *a, const char *name);

/*
    获取当前BoneEngine引擎的executor对象
*/
be_jse_executor_ctx_t* bone_engine_get_executor();

/*
    BoneEngine初始化
*/
void bone_engine_init();

/*
    用户注册自定义的JS对象, 返回该对象的引用值
    必须在bone_engine_init之后调用
*/
be_jse_node_t bone_engine_native_object_register(const char* objectName);

/*
    注册自定义JS对象Native扩展方法及属性,JS全局函数的实现函数
*/
void bone_engine_native_function_register(BE_JSE_FUNCTION_EXECUTE_CB extension);


void bone_engine_modules_register(BE_JSE_FUNCTION_EXECUTE_CB extension);

/*
    运行JS程序
    可以多次调用, BoneEngine符号表共享
*/
void bone_engine_start(const char* js);

/*
    BoneEngine复位之后重新运行JS程序
*/
void bone_engine_restart(const char* js);

/*
    退出BoneEngine
*/
void bone_engine_exit();


void bone_engine_log(const char* file, const char* tag,
                     int level, int line, const char* func,
                     const char* format, ...);

void bone_engine_set_log(BE_JSE_FUNCTION_LOG_CB cb);


typedef const char* (*BE_JSE_LOAD_MODULE_CB)(const char* moduleName);

/*
    注册load module函数
*/
void bone_engine_loadmodule_register(BE_JSE_LOAD_MODULE_CB func_cb);


#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_API_H_ */


