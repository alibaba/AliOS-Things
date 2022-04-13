/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "sound_pcm.h"
#include "ulog/ulog.h"

#define LOG_TAG  "[sound_pcm]"
#define PCM_LOCK(pcm) aos_mutex_lock(&pcm->mutex, AOS_WAIT_FOREVER)
#define PCM_UNLOCK(pcm) aos_mutex_unlock(&pcm->mutex)
#define PCM_RETURN_ERR      -1
#define PCM_RETURN_SUCCESS  0


int aos_device_name_hint(int card, void *hints)
{
    int i = 0, j = 0, fd = -1;
    char fn[256];
    hint_list_t *node = NULL;

    if(card == -1) {
        for(i = 0; i < AOS_SNDCARD_NUM_MAX; i++) {
            for(j = 0; j < AOS_SNDCARD_DEVICE_NUM_MAX; j++) {
                memset(fn, 0, sizeof(fn));
                snprintf(fn, sizeof(fn), "/dev/pcmC%uD%u%c", i, j, 'p');
                fd = open(fn, O_RDWR|O_NONBLOCK);
                if (fd > 0) {
                    node = (hint_list_t *)calloc(1, sizeof(hint_list_t));
                    if(!node) {
                        return -ENOMEM;
                    }
                    hints = (void *)node;
                    node->card = i;
                    node->device = j;
                    strncpy(node->name, fn, sizeof(node->name));
                    close(fd);
                    LOGD(LOG_TAG, "%s:%d, found sound card %d device %d %s", __func__, __LINE__, i, j, node->name);
                    return PCM_RETURN_SUCCESS;
                }
            }
        }
    } else {
        for(j = 0; j < AOS_SNDCARD_DEVICE_NUM_MAX; j++) {
            memset(fn, 0, sizeof(fn));
            snprintf(fn, sizeof(fn), "/dev/pcmC%uD%u%c", card, j, 'p');
            fd = open(fn, O_RDWR|O_NONBLOCK);
            if (fd > 0) {
                node = (hint_list_t *)calloc(1, sizeof(hint_list_t));
                if(!node) {
                    return -ENOMEM;
                }
                hints = (void *)node;
                node->card = card;
                node->device = j;
                strncpy(node->name, fn, sizeof(node->name));
                close(fd);
                LOGD(LOG_TAG, "%s:%d, found sound card %d device %d %s", __func__, __LINE__, card, j, node->name);
                return PCM_RETURN_SUCCESS;
            }
        }
    }
    LOGE(LOG_TAG, "%s:%d, found no sound device ", __func__, __LINE__);
    return PCM_RETURN_ERR;
}

int aos_pcm_open(aos_pcm_t **pcm, const char *name, aos_pcm_stream_t stream, int mode)
{
    int card = 0, device = 0, fd = -1;
    char fn[256];
    if(!name) {
        LOGE(LOG_TAG, "%s:%d, param name is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    *pcm = (aos_pcm_t *)calloc(1, sizeof(aos_pcm_t));
    if(NULL == *pcm) {
        LOGE(LOG_TAG, "%s:%d, calloc aos_pcm_t failed", __func__, __LINE__);
        return -ENOMEM;
    }
    memset(*pcm, 0, sizeof(aos_pcm_t));
    if(!strcmp(name, "default")) {
        for(card = 0; card < AOS_SNDCARD_NUM_MAX; card++) {
            for(device = 0; device < AOS_SNDCARD_DEVICE_NUM_MAX; device++) {
                memset(fn, 0, sizeof(fn));
                snprintf(fn, sizeof(fn), "/dev/pcmC%uD%u%c", card, device, (stream == AOS_PCM_STREAM_PLAYBACK) ? 'p' : 'c');
                fd = open(fn, O_RDWR|O_NONBLOCK);
                if (fd > 0) {
                    (*pcm)->fd = fd;
                    (*pcm)->card = card;
                    (*pcm)->device = device;
                    (*pcm)->name = strdup(fn);
                    (*pcm)->stream = stream;
                    (*pcm)->mode = mode;
                    (*pcm)->state = AOS_PCM_STATE_OPEN;
                    aos_mutex_new(&((*pcm)->mutex));
                    aos_event_new(&((*pcm)->evt), 0);
                    LOGD(LOG_TAG, "%s:%d, open card %d device %d successfully", __func__, __LINE__, card, device);
                    return PCM_RETURN_SUCCESS;
                }
            }
        }
    } else {
        for(card = 0; card < AOS_SNDCARD_NUM_MAX; card++) {
            for(device = 0; device < AOS_SNDCARD_DEVICE_NUM_MAX; device++) {
                memset(fn, 0, sizeof(fn));
                snprintf(fn, sizeof(fn), "/dev/pcmC%uD%u%c", card, device, (stream == AOS_PCM_STREAM_PLAYBACK) ? 'p' : 'c');
                if(!strcmp(name, fn)) {
                    fd = open(fn, O_RDWR|O_NONBLOCK);
                    if (fd > 0) {
                        (*pcm)->fd = fd;
                        (*pcm)->card = card;
                        (*pcm)->device = device;
                        (*pcm)->name = strdup(fn);
                        (*pcm)->stream = stream;
                        (*pcm)->mode = mode;
                        (*pcm)->state = AOS_PCM_STATE_OPEN;
                        aos_mutex_new(&((*pcm)->mutex));
                        aos_event_new(&((*pcm)->evt), 0);
                        LOGD(LOG_TAG, "%s:%d, open card %d device %d successfully", __func__, __LINE__, card, device);
                        return PCM_RETURN_SUCCESS;
                    }
                }
            }
        }
    }
    LOGE(LOG_TAG, "%s:%d, open %s device failed", __func__, __LINE__, (stream == AOS_PCM_STREAM_PLAYBACK) ? "playback" : "capture");
    free(*pcm);
    *pcm = NULL;
    return PCM_RETURN_ERR;
}

int aos_pcm_prepare(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_PREPARE)) {
        LOGE(LOG_TAG, "%s:%d, ioctl prepare failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    } else {
        pcm->state = AOS_PCM_STATE_PREPARED;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_start(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_START)) {
        LOGE(LOG_TAG, "%s:%d, ioctl start failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    } else {
        pcm->state = AOS_PCM_STATE_RUNNING;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_wait(aos_pcm_t *pcm, int timeout)
{
    int ret = PCM_RETURN_SUCCESS;
    unsigned int actl_flags = 0;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    aos_event_get(&pcm->evt, AOS_PCM_EVT_READ | AOS_PCM_EVT_XRUN, AOS_EVENT_OR_CLEAR, &actl_flags, timeout);
    PCM_UNLOCK(pcm);

    if (actl_flags & AOS_EVENT_OR_CLEAR) {
        LOGE(LOG_TAG,"PCM_EVT_XRUN");
        return -EPIPE;
    }

    return ret; // TBD 2x : ret value + evt to driver
}

int aos_pcm_stop(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_DROP)) {
        LOGE(LOG_TAG, "%s:%d, ioctl drop failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    } else {
        pcm->state = AOS_PCM_STATE_PAUSED;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_drain(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_DRAIN)) {
        LOGE(LOG_TAG, "%s:%d, ioctl drain failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_pause(aos_pcm_t *pcm, int enable)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_PAUSE, enable)) {
        LOGE(LOG_TAG, "%s:%d, ioctl pause failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_close(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(close(pcm->fd)) {
        LOGE(LOG_TAG, "%s:%d, close pcm failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    if(pcm->name) {
        free(pcm->name);
    }
    if(pcm->open_func) {
        free(pcm->open_func);
    }
    if(pcm->private_data) {
        free(pcm->private_data);
    }
    if(pcm->hw_params) {
        free(pcm->hw_params);
    }
    if(pcm->sw_params) {
        free(pcm->sw_params);
    }
    PCM_UNLOCK(pcm);
    free(pcm);
    return ret;
}

int aos_pcm_recover(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_RECOVER)) {
        LOGE(LOG_TAG, "%s:%d, ioctl recover failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_suspend(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_SUSPEND)) {
        LOGE(LOG_TAG, "%s:%d, ioctl suspend failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_resume(aos_pcm_t *pcm)
{
    int ret = PCM_RETURN_SUCCESS;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_RESUME)) {
        LOGE(LOG_TAG, "%s:%d, ioctl resume failed", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_hw_params_alloca(aos_pcm_hw_params_t **p)
{
    *p = (aos_pcm_hw_params_t *)calloc(1, sizeof(aos_pcm_hw_params_t));
    if(NULL == *p) {
        LOGE(LOG_TAG, "%s:%d, calloc aos_pcm_hw_params_t failed", __func__, __LINE__);
        return -ENOMEM;
    }
    return PCM_RETURN_SUCCESS;
}

int aos_pcm_hw_params_any(aos_pcm_hw_params_t *params)
{
    if(!params) {
        return PCM_RETURN_ERR;
    }
    params->access = AOS_PCM_ACCESS_RW_INTERLEAVED;
    params->format = AOSRV_PCM_FORMAT_S16_LE;
    params->channels = 2;
    params->rate = 16000;
    params->sample_bits = (params->format * 8);
    params->frame_bits = (params->format * params->channels * 8);
    return PCM_RETURN_SUCCESS;
}

int aos_pcm_hw_params(aos_pcm_t *pcm, aos_pcm_hw_params_t *p)
{
    int ret = PCM_RETURN_SUCCESS;
    audio_hw_params_t params;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(p && pcm->hw_params) {
        memcpy(pcm->hw_params, p, sizeof(aos_pcm_hw_params_t));
    }
    PCM_LOCK(pcm);
    params.block = (pcm->mode & AOS_PCM_NONBLOCK) ? 0 : 1;
    params.interleave = (p->access == AOS_PCM_ACCESS_RW_NONINTERLEAVED) ? 0 : 1;
    params.channels = p->channels;
    params.rate = p->rate;
    params.sample_bits = p->format * 8;
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_HW_PARAMS, &params)) {
        LOGE(LOG_TAG, "%s:%d, cannot set hw params", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    LOGD(LOG_TAG, "%s:%d, block %d, interleave %d, rate %d channels %d bits %d", __func__, __LINE__, params.block,
         params.interleave, params.rate, params.channels, params.sample_bits);
    return ret;
}

int aos_pcm_sw_params_any(aos_pcm_sw_params_t *params)
{
    if(!params) {
        return PCM_RETURN_ERR;
    }
    params->period_step = 20; // 20ms period by default
    return PCM_RETURN_SUCCESS;
}

int aos_pcm_sw_params_alloca(aos_pcm_sw_params_t **p)
{
    *p = (aos_pcm_sw_params_t *)calloc(1, sizeof(aos_pcm_sw_params_t));
    if(NULL == *p) {
        LOGE(LOG_TAG, "%s:%d, calloc aos_pcm_sw_params_t failed", __func__, __LINE__);
        return -ENOMEM;
    }
    return PCM_RETURN_SUCCESS;
}

int aos_pcm_sw_params(aos_pcm_t *pcm, aos_pcm_sw_params_t *p)
{
    int ret = PCM_RETURN_SUCCESS;
    audio_sw_params_t params;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->sw_params && p) {
        pcm->sw_params->period_step = p->period_step;
    } else {
        LOGE(LOG_TAG, "%s:%d, invalid pcm->sw_params or p", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    params.hdl = &pcm->evt;
    params.period = p->period_step;
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_SW_PARAMS, &params)) {
        LOGE(LOG_TAG, "%s:%d, cannot set sw params", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    return ret;
}

int aos_pcm_set_params(aos_pcm_t *pcm, aos_pcm_format_t format, aos_pcm_access_t access, unsigned int channels,
                       unsigned int rate, int soft_resample, unsigned int latency)
{
    int ret = PCM_RETURN_SUCCESS;
    audio_hw_params_t params;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if (!pcm->hw_params) {
        pcm->hw_params = aos_zalloc(sizeof(aos_pcm_hw_params_t));

        if (pcm->hw_params == NULL) {
            return PCM_RETURN_ERR;
        }
    }
    pcm->hw_params->access = access;
    pcm->hw_params->channels = channels;
    pcm->hw_params->format = format;
    pcm->hw_params->rate = rate;
    pcm->hw_params->sample_bits = format * 8;
    pcm->hw_params->frame_bits = format * 8 * channels;

    params.block = (pcm->mode & AOS_PCM_NONBLOCK) ? 0 : 1;
    params.interleave = (access == AOS_PCM_ACCESS_RW_NONINTERLEAVED) ? 0 : 1;
    params.channels = channels;
    params.rate = rate;
    switch(format) {
        case AOSRV_PCM_FORMAT_S8:
            params.sample_bits = 8;
            break;
        case AOSRV_PCM_FORMAT_S16_LE:
            params.sample_bits = 16;
            break;
        case AOSRV_PCM_FORMAT_S24_LE:
            params.sample_bits = 24;
            break;
        case AOSRV_PCM_FORMAT_S32_LE:
            params.sample_bits = 32;
            break;
        default:
            return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_HW_PARAMS, &params)) {
        LOGE(LOG_TAG, "%s:%d, cannot set hw params", __func__, __LINE__);
        ret = PCM_RETURN_ERR;
    }
    PCM_UNLOCK(pcm);
    LOGD(LOG_TAG, "%s:%d, block %d, interleave %d, rate %d channels %d bits %d", __func__, __LINE__, params.block,
         params.interleave, params.rate, params.channels, params.sample_bits);
    return ret;
}

aos_pcm_sframes_t aos_pcm_writei(aos_pcm_t *pcm, const void *buffer, aos_pcm_uframes_t size)
{
    audio_xferi_t x;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->state != AOS_PCM_STATE_RUNNING) {
        LOGE(LOG_TAG, "%s:%d, unexpected pcm state %d", __func__, __LINE__, pcm->state);
        return PCM_RETURN_ERR;
    }
    if(pcm->stream != AOS_PCM_STREAM_PLAYBACK) {
        LOGE(LOG_TAG, "%s:%d, not playback device", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    x.buf = (void*)buffer;
    x.frames = size;
    x.result = 0;
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_WRITEI_FRAMES, &x)) {
        LOGE(LOG_TAG, "%s:%d, ioctl writei failed", __func__, __LINE__);
    }
    PCM_UNLOCK(pcm);
    //LOGE(LOG_TAG, "%s:%d, frames %d result %d", __func__, __LINE__, x.frames, x.result);
    return x.result;
}

aos_pcm_sframes_t aos_pcm_writen(aos_pcm_t *pcm, void **bufs, aos_pcm_uframes_t size)
{
    audio_xfern_t x;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->state != AOS_PCM_STATE_RUNNING) {
        LOGE(LOG_TAG, "%s:%d, unexpected pcm state %d", __func__, __LINE__, pcm->state);
        return PCM_RETURN_ERR;
    }
    if(pcm->stream != AOS_PCM_STREAM_PLAYBACK) {
        LOGE(LOG_TAG, "%s:%d, not playback device", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    x.bufs = bufs;
    x.frames = size;
    x.result = 0;
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_WRITEN_FRAMES, &x)) {
        LOGE(LOG_TAG, "%s:%d, ioctl writen failed", __func__, __LINE__);
    }
    PCM_UNLOCK(pcm);
    //LOGE(LOG_TAG, "%s:%d, frames %d result %d", __func__, __LINE__, x.frames, x.result);
    return x.result;
}

aos_pcm_sframes_t aos_pcm_readi(aos_pcm_t *pcm, void *buffer, aos_pcm_uframes_t size)
{
    audio_xferi_t x;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->state != AOS_PCM_STATE_RUNNING) {
        LOGE(LOG_TAG, "%s:%d, unexpected pcm state %d", __func__, __LINE__, pcm->state);
        return PCM_RETURN_ERR;
    }
    if(pcm->stream != AOS_PCM_STREAM_CAPTURE) {
        LOGE(LOG_TAG, "%s:%d, not capture device", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    x.buf = (void*)buffer;
    x.frames = size;
    x.result = 0;
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_READI_FRAMES, &x)) {
        LOGE(LOG_TAG, "%s:%d, ioctl readi failed", __func__, __LINE__);
    }
    PCM_UNLOCK(pcm);
    return x.result;
}

aos_pcm_sframes_t aos_pcm_readn(aos_pcm_t *pcm, void **bufs, aos_pcm_uframes_t size)
{
    audio_xfern_t x;
    if(NULL == pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->fd < 0) {
        LOGE(LOG_TAG, "%s:%d, param fd is not opened", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    if(pcm->state != AOS_PCM_STATE_RUNNING) {
        LOGE(LOG_TAG, "%s:%d, unexpected pcm state %d", __func__, __LINE__, pcm->state);
        return PCM_RETURN_ERR;
    }
    if(pcm->stream != AOS_PCM_STREAM_CAPTURE) {
        LOGE(LOG_TAG, "%s:%d, not capture device", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    PCM_LOCK(pcm);
    x.bufs = bufs;
    x.frames = size;
    x.result = 0;
    if(ioctl(pcm->fd, AUDIO_PCM_IOCTL_READN_FRAMES, &x)) {
        LOGE(LOG_TAG, "%s:%d, ioctl readn failed", __func__, __LINE__);
    }
    PCM_UNLOCK(pcm);
    return x.result;
}

aos_pcm_sframes_t aos_pcm_bytes_to_frames(aos_pcm_t *pcm, int bytes)
{
    if(!pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }

    return (bytes / (pcm->hw_params->frame_bits / 8));
}

int aos_pcm_frames_to_bytes(aos_pcm_t *pcm, aos_pcm_sframes_t frames)
{
    if(!pcm) {
        LOGE(LOG_TAG, "%s:%d, param pcm is null", __func__, __LINE__);
        return PCM_RETURN_ERR;
    }
    return (frames * (pcm->hw_params->frame_bits / 8));
}