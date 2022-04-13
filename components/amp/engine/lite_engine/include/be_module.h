/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_MODULE_H
#define BE_MODULE_H

#include "amp_defines.h"
#include "be.h"
#include "be_api.h"

#ifdef SUPPORT_NODE_MODELES


typedef struct {
    char* dirname;
    struct be_list_head lst;
} BE_JSE_DIRNAME_s;

struct be_list_head* bone_engine_dirname_get_head(void);
/* get dirname */
BE_JSE_DIRNAME_s* bone_engine_dirname_get(void);
/* push dirname */
void bone_engine_dirname_push(BE_JSE_DIRNAME_s* dirPtr);
/* pop and delete dirname */
void bone_engine_dirname_pop_del(void);
#endif

void be_jse_module_load(const char* moduleName,
                        BE_JSE_FUNCTION_EXECUTE_CB extension);

#endif /* BE_MODULE_H */
