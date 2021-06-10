/*
 * uVoice audio hardware adapt layer
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sound_pcm.h"
#include "sound_mixer.h"
#include "ulog/ulog.h"

#include <uvoice_audio.h>

#define LOG_TAG  "[uVoice_a2sa]"
aos_pcm_t *pb_pcm = NULL, *cap_pcm = NULL;

int uvoice_set_volume(snd_device_t device, int volume)
{
    aos_set_master_volume(volume * 10);
    return 0;
}

int uvoice_set_path(struct pcm_device *pcm, snd_device_t device)
{
    return 0;
}

int uvoice_dev_mute(struct pcm_device *pcm, snd_device_t device, int mute)
{
    aos_set_mute_state(mute);
    return 0;
}

void uvoice_pcm_notify(pcm_message_t msg)
{

}

int uvoice_pcm_setup(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_open(struct pcm_device *pcm)
{
    int ret = -1;
    aos_pcm_format_t format = AOSRV_PCM_FORMAT_S16_LE;
    if (!pcm) {
        LOGE(LOG_TAG, "%s:%d: pcm is null ", __func__, __LINE__);
        return -1;
    }

    switch (pcm->config.format) {
        case PCM_FORMAT_S8:
            format = AOSRV_PCM_FORMAT_S8;
            break;
        case PCM_FORMAT_S16_LE:
            format = AOSRV_PCM_FORMAT_S16_LE;
            break;
        case PCM_FORMAT_S24_LE:
        case PCM_FORMAT_S24_3LE:
            format = AOSRV_PCM_FORMAT_S24_LE;
            break;
        case PCM_FORMAT_S32_LE:
            format = AOSRV_PCM_FORMAT_S16_LE;
            break;
        default:
            break;
    }

    if (pcm->dir == PCM_OUT) {
        ret = aos_pcm_open(&pb_pcm, "default", AOS_PCM_STREAM_PLAYBACK, 0);
        if (!ret) {
            aos_pcm_hw_params_alloca(&pb_pcm->hw_params);
            aos_pcm_sw_params_alloca(&pb_pcm->sw_params);
            aos_pcm_sw_params_any(pb_pcm->sw_params);
            aos_pcm_set_params(pb_pcm, format, AOS_PCM_ACCESS_RW_INTERLEAVED, pcm->config.channels, pcm->config.rate, 0, 0);
            aos_pcm_prepare(pb_pcm);
            aos_pcm_start(pb_pcm);
        } else {
            LOGE(LOG_TAG, "%s:%d: open pb stream failed. ", __func__, __LINE__);
        }
    } else if (pcm->dir == PCM_IN) {
        /* open capture stream */
        ret = aos_pcm_open(&cap_pcm, "default", AOS_PCM_STREAM_CAPTURE, 0);
        if (!ret) {
            aos_pcm_hw_params_alloca(&cap_pcm->hw_params);
            aos_pcm_sw_params_alloca(&cap_pcm->sw_params);
            aos_pcm_sw_params_any(cap_pcm->sw_params);
            aos_pcm_set_params(cap_pcm, format, AOS_PCM_ACCESS_RW_INTERLEAVED, pcm->config.channels, pcm->config.rate, 0, 0);
            aos_pcm_prepare(cap_pcm);
            aos_pcm_start(cap_pcm);
        } else {
            LOGE(LOG_TAG, "%s:%d: open cap stream failed. ", __func__, __LINE__);
        }
    }
    return 0;
}

int uvoice_pcm_read(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
    int ret = -1;
    if (pcm->dir == PCM_IN && cap_pcm) {
        ret = aos_pcm_readi(cap_pcm, buffer, aos_pcm_bytes_to_frames(cap_pcm, nbytes));
        ret = aos_pcm_frames_to_bytes(cap_pcm, ret);
    }
    return ret;
}

int uvoice_pcm_write(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
    int ret = -1;
    if (pcm->dir == PCM_OUT && pb_pcm) {
        ret = aos_pcm_writei(pb_pcm, buffer, aos_pcm_bytes_to_frames(pb_pcm, nbytes));
        ret = aos_pcm_frames_to_bytes(pb_pcm, ret);
    }
    return ret;
}

int uvoice_extpa_config(struct external_pa_info *info)
{
    return 0;
}

int uvoice_pcm_silence(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_flush(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_close(struct pcm_device *pcm)
{
    if (pb_pcm) {
        //aos_pcm_drain(pb_pcm);
        aos_pcm_stop(pb_pcm);
        aos_pcm_close(pb_pcm);
        pb_pcm = NULL;
    }
    if (cap_pcm) {
        aos_pcm_stop(cap_pcm);
        aos_pcm_close(cap_pcm);
        cap_pcm = NULL;
    }
    return 0;
}

// extern int audio_install_codec_driver();
int uvoice_pcm_init(void)
{
    // audio_install_codec_driver();
    return 0;
}
