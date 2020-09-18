/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

const int16_t RES_AUD_RING_SAMPRATE_8000 [] = {
#include "res/ring/SOUND_RING_8000.txt"
};
#ifdef __BT_WARNING_TONE_MERGE_INTO_STREAM_SBC__

const int16_t RES_AUD_RING_SAMPRATE_16000 [] = {
#include "res/ring/SOUND_RING_16000.txt"
};
const int16_t RES_AUD_RING_SAMPRATE_44100[] = {
#include "res/ring/SOUND_RING_44100.txt"
};

const int16_t RES_AUD_RING_SAMPRATE_48000 [] = {
#include "res/ring/SOUND_RING_48000.txt"
};
#endif
