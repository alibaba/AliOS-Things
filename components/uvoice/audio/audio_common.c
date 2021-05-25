/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "uvoice_os.h"

#include "uvoice_audio.h"
#include "audio_common.h"


enum pcm_format bits_to_pcm_format(int bits)
{
    switch (bits) {
        case 32:
        return PCM_FORMAT_S32_LE;
        case 16:
        return PCM_FORMAT_S16_LE;
        case 24:
        return PCM_FORMAT_S24_LE;
        case 8:
        return PCM_FORMAT_S8;
    }
    return PCM_FORMAT_S16_LE;
}

#ifndef UVOICE_ON_XR871
int pcm_format_to_bits(enum pcm_format format)
{
    switch (format) {
        case PCM_FORMAT_S32_LE:
        return 32;
        case PCM_FORMAT_S24_LE:
        return 32;
        case PCM_FORMAT_S24_3LE:
        return 24;
        case PCM_FORMAT_S16_LE:
        return 16;
        case PCM_FORMAT_S8:
        return 8;
    };
    return 0;
}
#endif

int pcm_to_decibel(struct pcm_config *config, uint8_t *buffer, int nbytes)
{
    double ampl_sum = 0.0;
    int decibel;
    int samples;
    int i;
    int bits = pcm_format_to_bits(config->format) >> 3;

    if (bits == 0) {
        return 0;
    }

    samples = nbytes / bits;

    for (i = 0; i < samples; i += config->channels)
        ampl_sum += abs(buffer[i]);

    ampl_sum /= samples;
    decibel = (int)(20.0 * log10(ampl_sum));

    return decibel;
}

