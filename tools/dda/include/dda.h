/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once

int dda_enable(int agent_id);
void dda_disable(void);
void dda_log(char *buf, ...);
void dda_cli_log(char *buf);
int dda_service_init(void);
int dda_service_start(void);
int ddm_run(int argc, char **argv);

