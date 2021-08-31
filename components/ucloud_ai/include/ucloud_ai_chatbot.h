/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_NLP_H_
#define _UCLOUD_AI_NLP_H_
#include "ucloud_ai_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup ucloud_ai_aos_api
 * @{
 */

/**
 * Ucloud ai recognizing identity card face side api.
 *
 * @param[in]  InstanceId          chatbot's instanceId.
 * @param[in]  text                user's input text for interaction.
 * @return     response text result.
 *
 */
char *ucloud_ai_chatbot(char *instance_id, char *session_id, char *text);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _UCLOUD_AI_NLP_H_
