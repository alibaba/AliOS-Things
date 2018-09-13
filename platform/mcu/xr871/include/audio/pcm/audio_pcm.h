/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AUDIO_PCM_H_H
#define AUDIO_PCM_H_H

#define PCM_OUT                        0
#define PCM_IN                         1

enum pcm_format {
        PCM_FORMAT_S16_LE = 0,
        PCM_FORMAT_S32_LE,
        PCM_FORMAT_S8,
        PCM_FORMAT_S24_LE,
        PCM_FORMAT_MAX,
};

enum audio_card {
        SOUND_CARD_EXTERNAL_AUDIOCODEC = 0,   /* PLAY CAP*/
        SOUND_CARD_INTERNAL_DMIC,             /* CAP */
        SOUND_CARD_NULL                       /* NONE */
};

/* Configuration*/
struct pcm_config {
        unsigned int    channels;
        unsigned int    rate;
        unsigned int    period_size;
        unsigned int    period_count;
        enum pcm_format format;
};

#define AUDIO_CARD0                SOUND_CARD_EXTERNAL_AUDIOCODEC
#define AUDIO_CARD1                SOUND_CARD_INTERNAL_DMIC

int snd_pcm_init();
int snd_pcm_deinit();
int snd_pcm_write(struct pcm_config *config, unsigned int card, void *data, unsigned int count);
int snd_pcm_read(struct pcm_config *config, unsigned int card, void *data, unsigned int count);
int snd_pcm_flush(struct pcm_config *config, unsigned int card);
int snd_pcm_open(struct pcm_config *config, unsigned int card, unsigned int flags);
int snd_pcm_close(unsigned int card, unsigned int flags);

#endif