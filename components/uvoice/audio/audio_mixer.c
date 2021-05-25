/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "uvoice_types.h"

#include "uvoice_os.h"
#include "uvoice_audio.h"
#include "uvoice_pcm.h"
#include "uvoice_common.h"
#include "audio_common.h"
#include "audio_stream.h"
#include "audio_mixer.h"


#define SPK_VOLUME_ID    "spk_vol"
#define HPH_VOLUME_ID    "hph_vol"
#define REC_VOLUME_ID    "rec_vol"
#define SPK_AND_HPH_VOLUME_ID    "spk_hph_vol"


int device_select(struct audio_device *adev, snd_device_t device,
    bool force)
{
    int volume;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (device >= SND_DEVICE_OUT_BEGIN &&
        device <= SND_DEVICE_OUT_END) {
        if (adev->out_device != device || force) {
            volume = volume_get(device);
            if (volume >= VOLUME_LEVEL_MIN &&
                    volume <= VOLUME_LEVEL_MAX)
                adev->out_volume = volume;
            if (adev->out) {
                uvoice_set_path(&adev->out->pcm, device);
                volume_set(device, adev->out_volume);
            }
            adev->out_device = device;
            snd_debug("out device %d\n", device);
        }
    } else if (device >= SND_DEVICE_IN_BEGIN &&
        device <= SND_DEVICE_IN_END) {
        if (adev->in_device != device || force) {
            if (adev->in)
                uvoice_set_path(&adev->in->pcm, device);
            adev->in_device = device;
            snd_debug("in device %d\n", device);
        }
    } else {
        snd_err("device %d invalid !\n", device);
        return -1;
    }

__exit:
    return 0;
}

int volume_set(snd_device_t device, int volume)
{
    unsigned char curr_vol = VOLUME_LEVEL_MIN;
    int len = sizeof(curr_vol);
    char *key = NULL;
    int ret;

    if (device == SND_DEVICE_OUT_SPEAKER)
        key = SPK_VOLUME_ID;
    else if (device == SND_DEVICE_OUT_HEADPHONE ||
        device == SND_DEVICE_OUT_HEADSET)
        key = HPH_VOLUME_ID;
    else if (device == SND_DEVICE_OUT_RECEIVER)
        key = REC_VOLUME_ID;
    else if (device == SND_DEVICE_OUT_SPEAKER_AND_HEADPHONE)
        key = SPK_AND_HPH_VOLUME_ID;
    else
        return -1;

    ret = uvoice_set_volume(device, volume);
    if (ret) {
        snd_err("set volume failed %d!\n", volume);
        return -1;
    }

    ret = os_kv_get(key, &curr_vol, &len);
    if (ret == -ENOENT) {
        snd_debug("%s not found\n", key);
    } else if (ret) {
        snd_err("get %s failed %d!\n", key, ret);
        return -1;
    }

    if (volume == curr_vol)
        goto __exit;
    curr_vol = volume;

    ret = os_kv_set(key, &curr_vol, len, 0);
    if (ret) {
        snd_err("set %s failed %d!\n", key, ret);
        return -1;
    }

__exit:
    return 0;
}

int volume_get(snd_device_t device)
{
    unsigned char curr_vol = VOLUME_LEVEL_MIN;
    int len = sizeof(curr_vol);
    char *key = NULL;
    int ret;

    if (device == SND_DEVICE_OUT_SPEAKER)
        key = SPK_VOLUME_ID;
    else if (device == SND_DEVICE_OUT_HEADPHONE ||
        device == SND_DEVICE_OUT_HEADSET)
        key = HPH_VOLUME_ID;
    else if (device == SND_DEVICE_OUT_RECEIVER)
        key = REC_VOLUME_ID;
    else if (device == SND_DEVICE_OUT_SPEAKER_AND_HEADPHONE)
        key = SPK_AND_HPH_VOLUME_ID;
    else
        return -1;

    ret = os_kv_get(key, &curr_vol, &len);
    if (ret == -ENOENT) {
        snd_debug("%s not found\n", key);
        return -1;
    } else if (ret) {
        snd_err("get %s failed %d!\n", key, ret);
        return -1;
    }

    return curr_vol;
}

