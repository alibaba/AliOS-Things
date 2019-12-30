/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UCLI_H
#define UCLI_H

#include "cli_api.h"
#include "task_group.h"

int ucli_proc_cmd(int argc, const char *argv[]);

int ucli_register_cmd(const struct cli_command_st *cmd);

int ucli_unregister_cmd(const struct cli_command_st *cmd);

void ucli_early_init(void);

int ucli_register_default_cmds(void);

void ucli_show_cmds(void);

int ucli_init(task_group_t *group);

void ucli_exit(task_group_t *group);

#endif /* UCLI_H */

