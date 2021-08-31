/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _CHATBOT_H_
#define _CHATBOT_H_

#include "model/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

char *chatbot(char *pInstanceId, char *pSessionId, char *text);

#ifdef __cplusplus
}
#endif
#endif // _CHATBOT_H_
