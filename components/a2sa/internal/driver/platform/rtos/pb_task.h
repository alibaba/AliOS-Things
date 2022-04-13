/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PB_TASK_H
#define PB_TASK_H

#include <stdio.h>
#include <stdbool.h>
#include "audio_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

void playback_task_start();
void playback_task_stop();
int playback_push_data(void *dev, const char *buf, unsigned int len, int blockMode);
int playback_task_drain();

#ifdef __cplusplus
}
#endif

#endif /* PB_TASK_H */