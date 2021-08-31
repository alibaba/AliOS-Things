/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ucloud_ai_common.h"
#define TAG "UCLOUD_AI_CHATBOT"

char *ucloud_ai_chatbot(char *instance_id, char *session_id, char *text)
{
    if (!instance_id || !text) {
        return NULL;
    }
    return chatbot(instance_id, session_id, text);
}

