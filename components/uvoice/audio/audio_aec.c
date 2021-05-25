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

#include "uvoice_types.h"
#include "uvoice_os.h"

#include "uvoice_common.h"
#include "uvoice_audio.h"

#include "audio_common.h"
#include "audio_stream.h"
#include "audio_aec.h"


#define AEC_REFER_DELAY_MS        10


int audio_aec_refer_conserve(struct aec_handler *aec, uint8_t *buffer, int nbytes)
{
    if (!aec) {
        snd_err("aec handler null !\n");
        return -1;
    }

    os_mutex_lock(aec->lock, OS_WAIT_FOREVER);

    if (uvoice_ringbuff_freesize(&aec->rb) >= nbytes)
        uvoice_ringbuff_fill(&aec->rb, buffer, nbytes);

    os_mutex_unlock(aec->lock);

__exit:
    return 0;
}

int audio_aec_process(struct aec_handler *aec, uint8_t *buffer, int nbytes)
{
    if (!aec) {
        snd_err("aec handler null !\n");
        return -1;
    }

    os_mutex_lock(aec->lock, OS_WAIT_FOREVER);
    if (!aec->start)
        goto __exit;

    if (uvoice_ringbuff_dirtysize(&aec->rb) >= nbytes) {
        uvoice_ringbuff_read(&aec->rb, aec->echo_buffer, nbytes);

        if (echo_cancellation_process(aec, buffer, aec->echo_buffer, nbytes)) {
            snd_err("aec process failed !\n");
            os_mutex_unlock(aec->lock);
            return -1;
        }
    }

__exit:
    os_mutex_unlock(aec->lock);
    return 0;
}

int audio_aec_init(struct in_stream *in)
{
    struct aec_handler *aec;
    struct pcm_config *config;
    int bits;

    if (!in) {
        snd_err("in stream null !\n");
        return -1;
    }

    config = &in->pcm.config;
    bits = pcm_format_to_bits(config->format);

    aec = snd_zalloc(sizeof(struct aec_handler), AFM_EXTN);
    if (!aec) {
        snd_err("alloc echo cancellation struct failed !\n");
        return -1;
    }

    aec->echo_buffer_size = period_samples_to_bytes(&in->pcm.config);
    aec->echo_buffer = snd_zalloc(aec->echo_buffer_size, AFM_MAIN);
    if (!aec->echo_buffer) {
        snd_err("alloc echo buffer failed !\n");
        snd_free(aec);
        return -1;
    }

     aec->refer_pool_size = aec->echo_buffer_size;
    aec->refer_pool = snd_zalloc(aec->refer_pool_size, AFM_EXTN);
    if (!aec->refer_pool) {
        snd_err("alloc refer pool failed !\n");
        snd_free(aec->echo_buffer);
        snd_free(aec);
        return -1;
    }

    if (echo_cancellation_create(aec,
            config->rate, bits, config->period_size)) {
        snd_err("create aec failed !\n");
        snd_free(aec->refer_pool);
        snd_free(aec->echo_buffer);
        snd_free(aec);
        return -1;
    }

    uvoice_ringbuff_init(&aec->rb, aec->refer_pool, aec->refer_pool_size);
    aec->lock = os_mutex_new();
    aec->rd_sem = os_sem_new(0);
    aec->wr_sem = os_sem_new(0);

    in->aec = aec;
    snd_debug("aec init\n");
    return 0;
}

int audio_aec_deinit(struct in_stream *in)
{
    struct aec_handler *aec;

    aec = in->aec;
    if (!aec) {
        snd_err("aec handler null !\n");
        return -1;
    }

    echo_cancellation_release(aec);
    os_sem_free(aec->wr_sem);
    os_sem_free(aec->rd_sem);
    os_mutex_free(aec->lock);
    snd_free(aec->refer_pool);
    snd_free(aec->echo_buffer);
    snd_free(aec);
    in->aec = NULL;

    snd_debug("aec free\n");
    return 0;
}

