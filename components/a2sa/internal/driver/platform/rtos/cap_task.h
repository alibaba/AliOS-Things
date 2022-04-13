/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CAP_TASK_H
#define CAP_TASK_H

#include <stdio.h>
#include <stdbool.h>
#include "audio_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

void capture_task_start(void *dev);
void capture_task_stop();
int capture_read_data(void *dev, char *buf, unsigned int len, int blockMode);

#ifdef __cplusplus
}
#endif

#endif /* PB_TASK_H */