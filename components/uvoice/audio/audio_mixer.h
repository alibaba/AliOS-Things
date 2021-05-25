/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __VOLUME_H__
#define __VOLUME_H__


int device_select(struct audio_device *adev, snd_device_t device, bool force);
int volume_set(snd_device_t device, int level);
int volume_get(snd_device_t device);

#endif /* __VOLUME_H__ */
