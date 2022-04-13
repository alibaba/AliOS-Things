/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __AUDIO_PROCESS_H__
#define __AUDIO_PROCESS_H__


int audio_out_conserve(struct audio_device *adev, uint8_t *buffer, int nbytes);
int audio_in_process(struct in_stream *in, uint8_t *buffer, int nbytes);
int audio_in_process_init(struct audio_device *adev, struct in_stream *in);
int audio_in_process_deinit(struct in_stream *in);


#endif /* __AUDIO_PROCESS_H__ */

