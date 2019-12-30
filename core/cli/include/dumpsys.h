/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CLI_DUMPSYS_H
#define CLI_DUMPSYS_H

uint32_t dumpsys_task_func(char *buf, uint32_t len, int32_t detail);
uint32_t dumpsys_func(char *buf, int32_t len, int32_t argc, char **argv);

#endif /* CLI_DUMPSYS_H */
