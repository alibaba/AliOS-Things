/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AUDIO_CARD_H
#define AUDIO_CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aos/list.h>
#include "control_dev.h"
#include "pcm_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int id;
	struct dlist_s pcm_dev_list;   /* pcm device list */
	int pcm_dev_num;               /* pcm device num */
	int pcm_str_num;
	ctrl_device_t *ctrl_dev;       /* ctrl device list */
} audio_card_t;

audio_card_t *audio_card_new();
pcm_device_t * audio_card_get_pcm_device(audio_card_t *card, void *private_data);
int audio_card_add_pcm_dev(audio_card_t *card, pcm_device_t *pcm_dev);
int audio_card_remove_pcm_dev(audio_card_t *card, pcm_device_t *pcm_dev);
int audio_card_remove(audio_card_t *card);


#ifdef __cplusplus
}
#endif
#endif /* AUDIO_CARD_H */