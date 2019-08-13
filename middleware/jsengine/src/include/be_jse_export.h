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
 * JSEngine initialization
 *
 */
void jsengine_init(void);

/**
 * Running JS code
 *
 */
void jsengine_start(const char* js);

/**
 * Running JS file
 */
void jsengine_eval_file(const char* pathname);

/**
 * JSEngine exit
 * free memory(including symbol table)
 */
void jsengine_exit(void);

typedef const char* (*BE_JSE_LOAD_MODULE_CB)(const char* moduleName);

/**
 *
 * register load module, for require other js file
 *
 */
void jsengine_loadmodule_register(BE_JSE_LOAD_MODULE_CB func_cb);

/**
 * for require js files, saving __dirname
 *
 */
void jsengine_save_dirname(const char* name);

/**
 *
 * JSEngine task initialization
 *
 */
int32_t jsengine_task_init();

/**
 * JSEngine yield task, for asynchronous event process
 *
 */
int32_t jsengine_task_yield(uint32_t timeout);

int32_t bone_console_get_log_flag(void);

void bone_console_log_enable(void);

void bone_console_log_disable(void);

#endif /* BE_JSE_EXPORT_H */