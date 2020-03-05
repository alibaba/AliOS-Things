/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CLI_UAGENT_H
#define CLI_UAGENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "uagent/uagent.h"

#define MOD_VER "1.0.0"
#define CLI_RESPONE   (UAGENT_FUNC_USER_BASE)
#define CLI_CMD_INPUT (UAGENT_FUNC_USER_BASE+1)

extern unsigned char g_cmd_from_cloud;

void cli_uagent_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CLI_UAGENT_H */

