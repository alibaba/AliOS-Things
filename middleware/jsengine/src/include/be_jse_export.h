/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_JSE_EXPORT_H
#define BE_JSE_EXPORT_H

#ifdef MINI_ENGINE
#include "be_jse_api.h"
#endif

/**
 *
 * BoneEngine初始化
 *
 */
void bone_engine_init(void);

/**
 * 运行JS语法块
 * 可以多次调用
 *
 */
void bone_engine_start(const char* js);

/**
 * 运行指定JS文件
 * pathname - 需要执行的文件， 要求全路径
 */
void bone_engine_eval_file(const char* pathname);

/**
 * 退出BoneEngine
 * 释放内存(符号表)
 */
void bone_engine_exit(void);

typedef const char* (*BE_JSE_LOAD_MODULE_CB)(const char* moduleName);

/**
 *
 * 注册load module函数
 * 用于require处理,获取js模块内容
 *
 */
void bone_engine_loadmodule_register(BE_JSE_LOAD_MODULE_CB func_cb);

/**
 * 用于require处理
 * 记录require模块时,保存__dirname
 *
 */
void bone_engine_save_dirname(const char* name);

/**
 *
 * 注册LOG输出重定向函数
 *
 */
typedef void (*BE_JSE_FUNCTION_LOG_CB)(const char* tag, int level,
                                       const char* msg, int len);
void bone_engine_set_log_cb(BE_JSE_FUNCTION_LOG_CB cb);

/**
 *
 * BoneEngine task 初始化
 *
 */
int32_t bone_engine_task_init();

/**
 * BoneEngine task 做工作, 处理异步回调
 *
 */
int32_t bone_engine_task_yield(uint32_t timeout);

int32_t bone_console_get_log_flag(void);

void bone_console_log_enable(void);

void bone_console_log_disable(void);

#endif /* BE_JSE_EXPORT_H */