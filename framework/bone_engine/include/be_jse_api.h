/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#ifndef BE_JSE_API_H_
#define BE_JSE_API_H_

#include "be_jse.h"

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



#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_API_H_ */


