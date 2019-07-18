/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef CLI_KSYSCALL_H
#define CLI_KSYSCALL_H

#include <stdint.h>

int32_t sys_cli_register_command_stub(void *arg);

int32_t sys_cli_unregister_command_stub(void *arg);

int32_t sys_cli_register_commands_stub(void *arg);

int32_t sys_cli_unregister_commands_stub(void *arg);

#endif /* CLI_KSYSCALL_H */

