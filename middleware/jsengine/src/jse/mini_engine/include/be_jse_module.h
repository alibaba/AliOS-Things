/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_JSE_MODULE_H
#define BE_JSE_MODULE_H

#include "be_jse.h"
#include "be_jse_api.h"

#ifdef SUPPORT_NODE_MODELES

#include "be_list.h"

typedef struct {
    char* dirname;
    struct be_list_head lst;
} BE_JSE_DIRNAME_s;

struct be_list_head* bone_engine_dirname_get_head();
/* 获取最新dirname */
BE_JSE_DIRNAME_s* bone_engine_dirname_get();
/* push 一个dirname */
void bone_engine_dirname_push(BE_JSE_DIRNAME_s* dirPtr);
/* pop并del 最新的dirname */
void bone_engine_dirname_pop_del();
#endif

void be_jse_module_load(const char* moduleName,
                        BE_JSE_FUNCTION_EXECUTE_CB extension);

#endif /* BE_JSE_MODULE_H */
