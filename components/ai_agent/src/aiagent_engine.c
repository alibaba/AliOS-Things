/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "aiconfig.h"
#include "aiagent_service.h"
#ifdef CONFIG_UCLOUD_AI_ENGINE_ENABLE
#include "engine/ucloud_ai_engine.h"
#endif

#define TAG "AIAGENT_ENGINE"

static aiagent_engine_t *g_ai_engine = NULL;

#ifdef CONFIG_UCLOUD_AI_ENGINE_ENABLE
extern aiagent_context_t ucloud_ai_engine;
#endif

/* Available ai engine context */
static aiagent_context_t *ai_engine_ctx[] = {
#ifdef CONFIG_UCLOUD_AI_ENGINE_ENABLE
    &ucloud_ai_engine,
#endif
    NULL
};

/*
 * Initialize the ai engine
 */
aiagent_engine_t *aiagent_engine_init(const char *dev_name)
{
    aiagent_engine_t *engine;
    int32_t index;
    int32_t i;

    /*Reset ai engine service*/
    if (g_ai_engine != NULL)
        aiagent_engine_uninit();

    index = 0;
    engine = NULL;
    if (dev_name == NULL)
        return NULL;

    if (dev_name != NULL) {
        for (i = 0; ai_engine_ctx[i]; ++i) {
            if (strncasecmp(ai_engine_ctx[i]->name, dev_name, strlen(dev_name)) == 0) {
                if (ai_engine_ctx[i]->available()) {
                    engine = ai_engine_ctx[i]->create(index);
                    break;
                }
            }
        }
    } else {
        for (i = 0; ai_engine_ctx[i]; ++i) {
            if (ai_engine_ctx[i]->available()) {
                engine = ai_engine_ctx[i]->create(index);
                if (engine != NULL)
                    break;

            }
        }
    }
    if (engine == NULL) {
        if (dev_name) {
            LOGE(TAG, "%s not available", dev_name);
            return NULL;
        }
        LOGE(TAG, "No available engine engine");
        return NULL;
    }
    g_ai_engine = engine;
    g_ai_engine->name = ai_engine_ctx[i]->name;

    /* Initialize the ai engine */
    if (g_ai_engine->ai_engine_init(g_ai_engine) < 0) {
        aiagent_engine_uninit();
        return NULL;
    }

    LOG("Initialize ai agent engine successfully\n");
    return g_ai_engine;
}

/*
 * Uninitialize the ai engine
 */
void aiagent_engine_uninit(void)
{
    int32_t i, j;

    if (!g_ai_engine)
        return;

    g_ai_engine->ai_engine_uninit(g_ai_engine);
    g_ai_engine->ai_engine_free(g_ai_engine);
    g_ai_engine = NULL;

    LOG("Uninitialize ai agent engine successfully\n");
    return;
}

const char *aiagent_get_engine_name(void)
{
    if (!g_ai_engine)
        return NULL;

    return g_ai_engine->name;
}

aiagent_engine_t *aiagent_get_engine(void)
{
    return g_ai_engine;
}
