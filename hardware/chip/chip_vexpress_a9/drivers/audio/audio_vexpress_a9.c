/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <string.h>
#include <aos/list.h>
#include "audio_rtos.h"
#include "control.h"
#include "drv_pl041.h"
#include "drv_ac97.h"
#include "aos/cli.h"

#define LOG_TAG    "[audio_genie]"

static int playback_stream_hdl = 0;
static int capture_stream_hdl = 1;
static char *in_buf = NULL, *out_buf = NULL;
static unsigned int in_size = 0, out_size = 0;
const static char dummyData[1024] = {0};
static int ac97_hw_gain_vol_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
static int ac97_hw_gain_vol_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
static int ac97_hw_bias_level_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
static int ac97_hw_bias_level_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);

static const struct snd_kcontrol_new ac97_codec_controls[] = {
	SOC_SINGLE_EXT("Codec Gain Volume Adjuct",
			    0,
			    0,
			    63,
			    0,
			    ac97_hw_gain_vol_get,
			    ac97_hw_gain_vol_put),
    SOC_SINGLE_EXT("Codec Bias Level Adjuct",
			    0,
			    0,
			    0x80000,
			    0,
			    ac97_hw_bias_level_get,
			    ac97_hw_bias_level_put),
};

static int ac97_hw_bias_level_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
    LOGD(LOG_TAG, "%s:%d: get bias level %d \r\n", __func__, __LINE__, 20);
    ucontrol->value.integer.value[0] = 20;
    return 0;
}

static int ac97_hw_bias_level_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
    LOGD(LOG_TAG, "%s:%d: set bias level to %d \r\n", __func__, __LINE__, ucontrol->value.integer.value[0]);
    return 0;
}

static int ac97_hw_gain_vol_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
    int volume = 0;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: kcontrol or ucontrol is NULL. \r\n", __func__, __LINE__);
        return -1;
    }
    volume = ac97_get_vol();
    LOGD(LOG_TAG, "%s:%d: volume %d \r\n", __func__, __LINE__, volume);
    ucontrol->value.integer.value[0] = volume;
    return 0;
}

static int ac97_hw_gain_vol_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
    int volume = 0;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: kcontrol or ucontrol is NULL. \r\n", __func__, __LINE__);
        return -1;
    }
    volume = ucontrol->value.integer.value[0];
    LOGD(LOG_TAG, "%s:%d: set volume to %d \r\n", __func__, __LINE__, volume);
    ac97_set_vol(volume);
    return 0;
}

static void rt_hw_aaci_isr(unsigned int status, void *user_data)
{
    if (status & AACI_SR_TXHE)
    {
        LOGE(LOG_TAG, "%s:%d", __func__, __LINE__);
    }
    return;
}

static pcm_stream_handler_t pcm_stream_open(int mode, int sampleRate, int channels, pcm_stream_format_t format)
{
    pcm_stream_handler_t hdl = NULL;
    struct pl041_cfg _cfg;
    LOGD(LOG_TAG, "%s:%d, dir %d sampleRate %d channels %d format %d", __func__, __LINE__, mode, sampleRate, channels, (int)format);
    if(mode == PCM_STREAM_IN) {
        hdl = (pcm_stream_handler_t)&capture_stream_hdl;
    } else if (mode == PCM_STREAM_OUT) {
        hdl = (pcm_stream_handler_t)&playback_stream_hdl;

        /* init ac97 and pl041 */
        _cfg.itype = PL041_CHANNEL_LEFT_ADC | PL041_CHANNEL_RIGHT_ADC;
        _cfg.otype = PL041_CHANNEL_LEFT_DAC | PL041_CHANNEL_RIGHT_DAC;
        _cfg.vol   = 100;
        _cfg.rate  = sampleRate;
        aaci_pl041_init();
        ac97_reset();
        aaci_pl041_channel_cfg(0, &_cfg);
        aaci_pl041_irq_register(0, rt_hw_aaci_isr, NULL);

        ac97_set_vol(100);
        ac97_set_rate(sampleRate);
    }
    return hdl;
}

static int pcm_stream_start(pcm_stream_handler_t hdl)
{
    int ret = 0;
    LOGD(LOG_TAG, "%s:%d", __func__, __LINE__);
    if(hdl == &playback_stream_hdl) {
        aaci_pl041_channel_enable(0);
        //aaci_pl041_irq_enable(0, AACI_IE_UR | AACI_IE_TX | AACI_IE_TXC);
    }
    return ret;
}

static int pcm_stream_write(pcm_stream_handler_t hdl, void *buf, unsigned int len)
{
#if 0
    audio_data_buf_t *databuf = malloc(sizeof(audio_data_buf_t));

    if(!databuf) {
        LOGE(LOG_TAG, "%s:%d, malloc audio_data_buf_t failed", __func__, __LINE__);
        return -1;
    }
    databuf->data = malloc(len);
    if(!databuf->data) {
        LOGE(LOG_TAG, "%s:%d, malloc databuf->data failed", __func__, __LINE__);
        return -1;
    }
    databuf->size = len;
    memcpy(databuf->data, buf, len);
    dlist_add_tail(&databuf->list, &audio_data_list);
    LOGE(LOG_TAG, "%s:%d, store %d bytes successfully", __func__, __LINE__, len);
#endif
    aaci_pl041_channel_write(0, (unsigned short *)buf, len >> 1);
    //LOGE(LOG_TAG, "%s:%d, feed %d bytes to hw", __func__, __LINE__, len);
    return len;
}

static int pcm_stream_read(pcm_stream_handler_t hdl, void *buf, unsigned int len)
{
    int ret = 0;
#if 0
    if(!out_buf) {
        LOGE(LOG_TAG, "%s:%d, no readable buffer, pls wait...", __func__, __LINE__, len);
        return -1;
    }
    if(len > out_size) {
        LOGE(LOG_TAG, "%s:%d, no enough data available %d/%d", __func__, __LINE__, out_size, len);
        return -1;
    }
    in_buf = malloc(len);
    if(!in_buf) {
        LOGE(LOG_TAG, "%s:%d, malloc %d bytes failed", __func__, __LINE__, len);
        return -1;
    }
    memcpy(in_buf, out_buf, len);
    ret = len;
#endif
    LOGE(LOG_TAG, "%s:%d, fetch %d bytes successfully", __func__, __LINE__, len);
    return ret;
}

static int pcm_stream_stop(pcm_stream_handler_t hdl)
{
    int ret = 0;
    LOGD(LOG_TAG, "%s:%d", __func__, __LINE__);
    if(hdl == &playback_stream_hdl) {
        /* wait codec free */
        usleep(100*1000);
        /* disable irq and channels 0 */
        aaci_pl041_irq_disable(0, AACI_IE_UR | AACI_IE_TX | AACI_IE_TXC);
        aaci_pl041_channel_disable(0);
    }
    return ret;
}

static int pcm_stream_close(pcm_stream_handler_t hdl)
{
    int ret = 0;
    LOGD(LOG_TAG, "%s:%d", __func__, __LINE__);
    return ret;
}

pcm_stream_ops_t native_pcm_ops = {
    .open = pcm_stream_open,
    .start = pcm_stream_start,
    .read = pcm_stream_read,
    .write = pcm_stream_write,
    .stop = pcm_stream_stop,
    .close = pcm_stream_close,
};

int audio_native_card_instantiate()
{
    LOGD(LOG_TAG, "%s:%d, RTOS audio driver test entry!!", __func__, __LINE__);
    return audio_native_card_register(1, 1, &native_pcm_ops, ac97_codec_controls, sizeof(ac97_codec_controls)/sizeof(ac97_codec_controls[0]));
}
ALIOS_CLI_CMD_REGISTER(audio_native_card_instantiate, insmod_audio_drv,  RTOS Audio Driver Test)
