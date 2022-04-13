/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <errno.h>
#include <ulog/ulog.h>
#include "audio_rtos.h"
#include "audio_card.h"
#include "pcm_dev.h"
#include "pb_task.h"
#include "cap_task.h"

#define LOG_TAG    "[audio_rtos]"

static native_pcm_device_t native_device[2];

static int pcm_open(void *dev)
{
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(pcm_dev->isOpenState) {
        //return -EBUSY;
        return 0; /* WAR in case app not close pcm stream before open. */
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    pcm_dev->isOpenState = true;
    return 0;
}

static int pcm_hw_params(void *dev, audio_hw_params_t *params)
{
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!params) {
        LOGE(LOG_TAG, "%s:%d, params is null", __func__, __LINE__);
        return -EINVAL;
    }
    pcm_dev->params.block = params->block;
    pcm_dev->params.interleave = params->interleave;
    pcm_dev->params.rate = params->rate;
    pcm_dev->params.channels = params->channels;
    pcm_dev->params.sample_bits = params->sample_bits;
    return 0;
}

static int pcm_sw_params(void *dev, audio_sw_params_t *params)
{
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!params) {
        LOGE(LOG_TAG, "%s:%d, params is null", __func__, __LINE__);
        return -EINVAL;
    }
    pcm_dev->swParams.hdl = params->hdl;
    pcm_dev->swParams.period = params->period;
    return 0;
}

static int pcm_hw_prepare(void *dev)
{
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->open) {
        LOGE(LOG_TAG, "%s:%d, dev ops open is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->hdl) {
        /* if 1st time open this pcm stream */
        pcm_dev->hdl = pcm_dev->ops->open(pcm_dev->mode, pcm_dev->params.rate, pcm_dev->params.channels, pcm_dev->params.sample_bits, pcm_dev->swParams.hdl);
        if(!pcm_dev->hdl) {
            LOGE(LOG_TAG, "%s:%d, pcm_dev open failed", __func__, __LINE__);
            return -1;
        }
    } else {
        /* if not 1st time open this stream, it means recovering this stream */
        pcm_dev->ops->close(pcm_dev->hdl);
        /* if 1st time open this pcm stream */
        pcm_dev->hdl = pcm_dev->ops->open(pcm_dev->mode, pcm_dev->params.rate, pcm_dev->params.channels, pcm_dev->params.sample_bits, pcm_dev->swParams.hdl);
        if(!pcm_dev->hdl) {
            LOGE(LOG_TAG, "%s:%d, pcm_dev open failed", __func__, __LINE__);
            return -1;
        }
    }
    return 0;
}

static int pcm_start(void *dev)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->start) {
        LOGE(LOG_TAG, "%s:%d, dev ops start is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm_dev->ops->start(pcm_dev->hdl);
    if(pcm_dev->mode == PCM_STREAM_OUT) {
        /* start playback thread */
        playback_task_start();
    } else {
        /* start playback thread */
        capture_task_start(dev);
    }
    return ret;
}

static int pcm_readi(void *dev, audio_xferi_t *xbuf)
{
    int ret = -1;
    unsigned int byte_len = 0, frame_size = 0;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    frame_size = (pcm_dev->params.channels) * (pcm_dev->params.sample_bits) / 8;
    byte_len = xbuf->frames * frame_size;
    ret = capture_read_data(pcm_dev, xbuf->buf, byte_len, pcm_dev->params.block);
    xbuf->result = (ret > 0) ? ret / frame_size : 0;
    return 0;
}

static int pcm_writei(void *dev, audio_xferi_t *xbuf)
{
    int ret = 0;
    int byte_len = 0, frame_size = 0;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->write) {
        LOGE(LOG_TAG, "%s:%d, dev ops write is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!xbuf || !(xbuf->buf)) {
        LOGE(LOG_TAG, "%s:%d, invalid xbuf", __func__, __LINE__);
        return -EINVAL;
    }
    frame_size = (pcm_dev->params.channels) * (pcm_dev->params.sample_bits) / 8;
    byte_len = (xbuf->frames) * frame_size;
    ret = playback_push_data(dev, xbuf->buf, byte_len, pcm_dev->params.block);
    xbuf->result = (ret > 0) ? ret / frame_size : 0;
    return 0;
}

static int pcm_readn(void *dev, audio_xfern_t *xbuf)
{
    int ret = 0;
    LOGE(LOG_TAG, "%s:%d, unsupported by now", __func__, __LINE__);
    return ret;
}

static int pcm_writen(void *dev, audio_xfern_t *xbuf)
{
    int ret = 0;
    LOGE(LOG_TAG, "%s:%d, unsupported by now", __func__, __LINE__);
    return ret;
}

int pcm_drain(void *dev)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(pcm_dev->mode == PCM_STREAM_OUT) {
        /* stop playback task */
        ret = playback_task_drain();
    }
    LOGE(LOG_TAG, "%s:%d", __func__, __LINE__);
    return ret;
}

int pcm_pause(void *dev, int enable)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->pause) {
        LOGE(LOG_TAG, "%s:%d, dev ops close is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm_dev->ops->pause(pcm_dev->hdl, enable);
    return ret;
}

int pcm_suspend(void *dev)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->suspend) {
        LOGE(LOG_TAG, "%s:%d, dev ops close is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm_dev->ops->suspend(pcm_dev->hdl);
    return ret;
}

int pcm_resume(void *dev)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->resume) {
        LOGE(LOG_TAG, "%s:%d, dev ops close is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm_dev->ops->resume(pcm_dev->hdl);
    return ret;
}

int pcm_stop(void *dev)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->stop) {
        LOGE(LOG_TAG, "%s:%d, dev ops stop is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(pcm_dev->mode == PCM_STREAM_OUT) {
        /* stop playback task */
        playback_task_stop();
    } else {
        /* stop capture task */
        capture_task_stop();
    }
    ret = pcm_dev->ops->stop(pcm_dev->hdl);
    LOGE(LOG_TAG, "%s:%d", __func__, __LINE__);
    return ret;
}

int pcm_close(void *dev)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->close) {
        LOGE(LOG_TAG, "%s:%d, dev ops close is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm_dev->ops->close(pcm_dev->hdl);
    pcm_dev->isOpenState = false;
    return ret;
}

int pcm_recover(void *dev)
{
    int ret = -1;
    native_pcm_device_t *pcm_dev = (native_pcm_device_t *)dev;
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops) {
        LOGE(LOG_TAG, "%s:%d, dev ops is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!pcm_dev->ops->recover) {
        LOGE(LOG_TAG, "%s:%d, dev ops close is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm_dev->ops->recover(pcm_dev->hdl);
    return ret;
}

static pcm_device_ops_t pcm_dev_ops = {
    .open = pcm_open,
    .hw_params = pcm_hw_params,
    .sw_params = pcm_sw_params,
    .hw_prepare = pcm_hw_prepare,
    .start = pcm_start,
    .readi = pcm_readi,
    .readn = pcm_readn,
    .writei = pcm_writei,
    .writen = pcm_writen,
    .drain = pcm_drain,
    .pause = pcm_pause,
    .resume = pcm_resume,
    .stop = pcm_stop,
    .close = pcm_close,
    .recover = pcm_recover,
};

/**
 * audio_native_card_register - create an audio card
 * @rec_num: recoder number
 * @spk_num: speaker number
 * @controls: audio_kcontrol_new properties belongs to audio card
 * @controls_num: audio_kcontrol_new number
 */
int audio_native_card_register(int rec_num, int spk_num, pcm_stream_ops_t *ops, const struct audio_kcontrol_new *controls, int controls_num)
{
    int ret = -1;
    audio_card_t *card = NULL;
    pcm_device_t *pcm_dev = NULL;
    char name[20] = {0};

    card = audio_card_new();
    if(!card) {
        LOGE(LOG_TAG, "%s:%d, new card failed", __func__, __LINE__);
        return ret;
    }

    if(rec_num > 0) {
        snprintf(name, sizeof(name), "/dev/pcmC%dD%dc", card->id, card->pcm_str_num);
        pcm_dev = audio_pcm_device_new(AUDIO_DEVICE_TYPE_PCM_CAPTURE, name, &pcm_dev_ops, (void*)&native_device[0]);
        if(!pcm_dev) {
            LOGE(LOG_TAG, "%s:%d, new capture device failed", __func__, __LINE__);
            return -ENOMEM;
        }
        native_device[0].mode = PCM_STREAM_IN;
        native_device[0].ops = ops;
        native_device[0].parent_data = (void *)card;
        ret = audio_card_add_pcm_dev(card, pcm_dev);
        if(ret < 0) {
            LOGE(LOG_TAG, "%s:%d, add capture device failed", __func__, __LINE__);
            return ret;
        }
    }

    if(spk_num > 0) {
        snprintf(name, sizeof(name), "/dev/pcmC%dD%dp", card->id, card->pcm_str_num);
        pcm_dev = audio_pcm_device_new(AUDIO_DEVICE_TYPE_PCM_PLAYBACK, name, &pcm_dev_ops, (void*)&native_device[1]);
        if(!pcm_dev) {
            LOGE(LOG_TAG, "%s:%d, new playback device failed", __func__, __LINE__);
            return -ENOMEM;
        }
        native_device[1].mode = PCM_STREAM_OUT;
        native_device[1].ops = ops;
        native_device[1].parent_data = (void *)card;
        ret = audio_card_add_pcm_dev(card, pcm_dev);
        if(ret < 0) {
            LOGE(LOG_TAG, "%s:%d, add playback device failed", __func__, __LINE__);
            return ret;
        }
    }
    card->pcm_str_num++;

    if(controls && controls_num > 0) {
        audio_add_controls(card->ctrl_dev, controls, controls_num, NULL);
    }
    return 0;
}

