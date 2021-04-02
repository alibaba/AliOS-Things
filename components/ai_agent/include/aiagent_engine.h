/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _AI_AGENT_ENGINE_H_
#define _AI_AGENT_ENGINE_H_
#include <stdint.h>
#include <stdbool.h>
#include "aiagent_common.h"
#ifdef CONFIG_UCLOUD_AI_ENGINE_ENABLE
#include "engine/ucloud_ai_engine.h"
#endif

typedef struct _aiagent_engine_t {
    /* * * */
    /* The name of this ucloud ai engine */
    const char *name;

    bool is_dummy; /*if no engine, use dummy*/

    ai_model_t model;
    char *src1; /*source data1*/
    char *src2; /*source data2, some cases need two image to compare*/

    ai_engine_cb_t callback;
    /*init ai engine*/
    int32_t (*ai_engine_init) (struct _aiagent_engine_t *eng);

    /*uninit ai engine*/
    void (*ai_engine_uninit) (struct _aiagent_engine_t *eng);

    /*run ai engine model*/
    int (*ai_engine_model_infer) (struct _aiagent_engine_t *eng);

    /*free ai engine*/
    void (*ai_engine_free) (struct _aiagent_engine_t *eng);
} aiagent_engine_t;

#endif // _AI_AGENT_ENGINE_H_
