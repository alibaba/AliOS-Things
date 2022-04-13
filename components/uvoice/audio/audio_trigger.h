/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __AUDIO_TRIGGER_H__
#define __AUDIO_TRIGGER_H__


typedef struct {
	int devid;
	char name[32];
} st_device_t;


int audio_trigger_init(void);
int audio_trigger_deinit(void);

#endif /* __AUDIO_TRIGGER_H__ */
