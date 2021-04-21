/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "aiconfig.h"
#include "aiagent_service.h"
#include "aiagent_engine.h"

#define TAG "AIAGENT_SERVICE"

/*
 * Initialize the ai agent service
 */
int32_t aiagent_service_init(const char *engine_name, ai_model_t model)
{
    aiagent_engine_t *eng = aiagent_engine_init(engine_name);
    if (!eng) {
        LOGE(TAG, "ai agent service init fail\n");
        return -1;
    }

    eng->model = model;
    return 0;
}

/*
 * Uninitialize the ai agent service
 */
int32_t aiagent_service_uninit(void)
{
    return aiagent_engine_uninit();
}

/*
 * Do ai model inference
 */
int32_t aiagent_service_model_infer(char *src1, char *src2, ai_engine_cb_t cb)
{
    int32_t ret;
    aiagent_engine_t *eng = aiagent_get_engine();
    if (!eng) {
        LOGE(TAG, "aiagent get engine name fail\n");
        return -1;
    }

    eng->callback = cb;
    eng->src1 = src1;
    eng->src2 = src2;
    ret = eng->ai_engine_model_infer(eng);

    LOG("aiagent_service_model_infer done\n");
    return ret;
}

/*
 * Get current ai agent model
 */
ai_model_t aiagent_service_get_cur_model(void)
{
    aiagent_engine_t *eng = aiagent_get_engine();
    if (!eng) {
        LOGE(TAG, "aiagent get engine name fail\n");
        return -1;
    }
    return eng->model;
}
