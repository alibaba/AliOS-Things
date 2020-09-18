/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __AUDIO_DUMP_H__
#define __AUDIO_DUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AUDIO_DEBUG
#define AUDIO_DUMP
#endif

void audio_dump_init(int frame_len, int sample_bytes, int channel_num);
void audio_dump_clear_up(void);
void audio_dump_add_channel_data(int channel_id, void *pcm_buf, int pcm_len);
void audio_dump_run(void);

void data_dump_init(void);
void data_dump_run(const char *str, void *data_buf, uint32_t data_len);

#ifdef __cplusplus
}
#endif

#endif