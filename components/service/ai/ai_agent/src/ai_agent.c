/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "aos/kernel.h"
#include "graphics.h"
#include "sdkconfig.h"
#include "ai_agent.h"

#define TAG "AI_AGENT"

static ai_agent_type_t ai_agent;

void set_ai_agent_type(ai_agent_type_t type)
{
    ai_agent = type;
}

ai_agent_type_t get_ai_agent_type(void)
{
    return ai_agent;
}

int32_t ai_agent_init(ai_agent_type_t type)
{
    int32_t ret = 0;

    set_ai_agent_type(type);
    return ret;
}

int32_t ai_agent_uninit(void)
{
    return 0;
}