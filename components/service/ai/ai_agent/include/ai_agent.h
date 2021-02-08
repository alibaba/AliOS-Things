/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _AI_AGENT_H_
#define _AI_AGENT_H_

typedef enum _ai_agent_type_t {
    AI_AGENT_TYPE_UCLOUD_AI,
    AI_AGENT_TYPE_UAI,
    AI_AGENT_TYPE_MNN,
    AI_AGENT_TYPE_ONNX,
    AI_AGENT_TYPE_MAX
} ai_agent_type_t;

void set_ai_agent_type(ai_agent_type_t type);
ai_agent_type_t get_ai_agent_type(void);

#endif //_AI_AGENT_H_