/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef AUDIO_SPECTRUM_H
#define AUDIO_SPECTRUM_H

void audio_spectrum_open(int sample_rate, enum AUD_BITS_T sample_bits);

void audio_spectrum_close(void);

void audio_spectrum_run(const uint8_t *buf, int len);

#endif
