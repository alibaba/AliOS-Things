/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SOUND_CONTROL_H
#define SOUND_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <aos/list.h>
#include <aos/kernel.h>
#ifndef HAAS_AUDIO_DEMO
#include <sys/ioctl.h>
#endif
#include "audio_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AOS_MASTER_VOLUME_TX_STRING    "Master Volume TX"
#define AOS_MASTER_MUTE_STATE_STRING   "Master Mute State"

typedef struct {
    int fd;
    int card;
    char *name;
    struct audio_ctl_card_info card_info;
    struct audio_ctl_elem_info *elem_info;
    unsigned int count;
} aos_mixer_t;

int aos_mixer_open(aos_mixer_t **mixer, int card);
int aos_mixer_print_info(aos_mixer_t *mixer);
int aos_mixer_set_int_value(aos_mixer_t *mixer, char *name, int value);
int aos_mixer_get_int_value(aos_mixer_t *mixer, char *name, int *value);
int aos_mixer_close(aos_mixer_t *mixer);
int aos_set_master_volume(int volume);
int aos_get_master_volume(int *volume);
int aos_set_mute_state(int mute);
int aos_get_mute_state(int *mute);


#ifdef __cplusplus
}
#endif
#endif /* SOUND_CONTROL_H */