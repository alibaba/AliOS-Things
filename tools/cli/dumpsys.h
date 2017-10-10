/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef DUMPSYS_H
#define DUMPSYS_H

uint32_t dumpsys_task_func(char *buf, uint32_t len, int detail);
uint32_t dumpsys_func(char *buf, int len, int argc, char **argv);

#endif

