/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UCLI_H
#define UCLI_H

#include "aos/cli.h"
#include "task_group.h"

int ucli_proc_cmd(int argc, char *argv[], uint32_t pid);

int ucli_register_cmd(const struct cli_command *cmd);

int ucli_unregister_cmd(const struct cli_command *cmd);

int ucli_register_default_cmds(void);

void ucli_show_cmds(void);

int ucli_init(task_group_t *group);

void ucli_exit(task_group_t *group);

uint32_t ucli_attach_pid_get(void);

void ucli_attach_pid_set(uint32_t pid);

#endif /* UCLI_H */

