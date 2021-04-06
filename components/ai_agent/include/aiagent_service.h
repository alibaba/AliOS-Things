/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _AI_AGENT_H_
#define _AI_AGENT_H_
#include "aiagent_engine.h"
#include "aiagent_common.h"


typedef struct _aiagent_context_t {
    const char *name;
    const char *desc;
    int (*available) (void);
    aiagent_engine_t *(*create) (int engineid);
} aiagent_context_t;


/** @defgroup aiagent_aos_api aiagent
 * @{
 */

/**
 * Initialize aiagent service.
 * @param[in]  engine_name    engine name.
 * @param[in]  model    ai model.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t aiagent_service_init(const char *engine_name, ai_model_t model);

/**
 * Uninitialize aiagent service.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t aiagent_service_uninit(void);

/**
 * Init the aiagent service.
 * @param[in]  src1    source data1(image) you want to detect.
 * @param[in]  src2    source data2(image), some cases need to compare with original data.
 * @param[in]  cb      callback function to deal with result.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t aiagent_service_model_infer(char *src1, char *src2, ai_engine_cb_t cb);

/**
 * Init the aiagent service.
 *
 * @return  ai model get frome aiagent service.
 */
ai_model_t aiagent_service_get_cur_model(void);

/**
 * @}
 */

#endif // _AI_AGENT_H_
