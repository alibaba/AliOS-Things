/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sound_pcm.h"
#include "sound_mixer.h"
#include "ulog/ulog.h"

#define LOG_TAG  "[sound_example_vol]"

void sound_example_setvol_entry(int argc, char **argv)
{
    int volume = 0;
    if (argc < 2) {
        LOGE(LOG_TAG, "%s:%d: Usage: %s vaue, e.g. >> setvol 90 ", __func__, __LINE__, argv[0]);
        return;
    }
    volume = atoi(argv[1]);
    aos_set_master_volume(volume);
}

void sound_example_getvol_entry(int argc, char **argv)
{
    int volume = 0;
    aos_get_master_volume(&volume);
    LOGD(LOG_TAG, "%s:%d: get volume %d", __func__, __LINE__, volume);
}