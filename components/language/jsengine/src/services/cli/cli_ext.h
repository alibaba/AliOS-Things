/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_CLI_EXT_H
#define BE_CLI_EXT_H

#include "jse_port.h"
#include "jse_task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BE_CLI_REPLY "#be-cli-reply#"
#define BE_PUSH_ERROR_REPLY "#be-cli-push-error-reply#"

void sub_call_start(void *arg);

void cli_cmd_register_file();

#ifdef __cplusplus
}
#endif

#endif /* BE_CLI_EXT_H */
