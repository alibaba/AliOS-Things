/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CONTROL_DEV_H
#define CONTROL_DEV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aos/list.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int id;
    char name[32];
	struct dlist_s kcontrol_list;      /* audio_kcontrol list */
	int kcontrols_count;
	int last_numid;
	bool ctrl_dev_state;
	void *parent_data;
} ctrl_device_t;


ctrl_device_t *audio_ctrl_device_new(int id);
int audio_ctrl_device_free(ctrl_device_t *dev);


#ifdef __cplusplus
}
#endif
#endif /* CONTROL_DEV_H */