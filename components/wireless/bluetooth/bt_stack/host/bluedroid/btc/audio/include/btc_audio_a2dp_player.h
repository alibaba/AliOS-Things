/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BTC_AUDIO_A2DP_PLAYER_H_
#define _BTC_AUDIO_A2DP_PLAYER_H_

#include <stdint.h>

void * btc_audio_create_player(int32_t sample_rate, int32_t bits_per_sample, int32_t channel_type);
void btc_audio_destroy_player(void *handle);
void btc_audio_start_player(void *handle);
void btc_audio_stop_player(void *handle);
void btc_audio_player_flushBuffer(void);

#endif
