/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_types.h"
#include "uvoice_player.h"
#include "uvoice_tts.h"

#include "../../../internal/uvoice_os.h"
#include "../../../internal/uvoice_common.h"
#include "../../../internal/uvoice_play.h"

#include "alicloudtts_intf.h"
#include "alicloudtts.h"

#define ALIYUN_TTS_HTTP_URL "https://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/tts?"

/* https://help.aliyun.com/document_detail/84435.html?spm=a2c4g.11186623.6.581.16a75275MJHPrH */
static const voice_spec_t voice_spec[] = {
    {"xiaoyun", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
    {"xiaogang", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
    {"ruoxi", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"xiaomeng", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
    {"xiaowei", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
    {"amei", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
    {"xiaoxue", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
    {"siqi", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"sijia", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"sicheng", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"siyue", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"xiaomei", {VOICE_CHINESE, VOICE_CN_EN_MIX, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"sitong", {VOICE_CHINESE, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"ninger", {VOICE_CHINESE, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"xiaobei", {VOICE_CHINESE, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"yina", {VOICE_CHINESE, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"sijing", {VOICE_CHINESE, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"wendy", {VOICE_ENGLISH, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"william", {VOICE_ENGLISH, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, TTS_ALIYUN_SAMPLE_RATE_24K}},
    {"halen", {VOICE_ENGLISH, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
    {"harry", {VOICE_ENGLISH, VOICE_NULL, VOICE_NULL}, {TTS_ALIYUN_SAMPLE_RATE_8K, TTS_ALIYUN_SAMPLE_RATE_16K, 0}},
};

static aliyun_tts_config_t g_tts_config;
static int tts_state = ALIYUN_TTS_STATE_NULL;
static char *tts_format[4] = {"null", "pcm", "wav", "mp3"};

int uvoice_tts_aliyun_init(tts_config_t *config)
{
    int i = 0;
    int j = 0;
    int flag = 0;

    if (!config) {
        M_LOGE("config is null!");
        return -1;
    }

    if (!config->app_key) {
        M_LOGE("app_key is null!");
        return -1;
    }

    if (!config->token) {
        M_LOGE("token is null!");
        return -1;
    }

    if ((config->format != MEDIA_FMT_PCM) && (config->format != MEDIA_FMT_WAV) &&
		(config->format != MEDIA_FMT_MP3)) {
        M_LOGE("format %d is not supported !", config->format);
        return -1;
    }

    if ((config->sample_rate != TTS_ALIYUN_SAMPLE_RATE_8K)
        && (config->sample_rate != TTS_ALIYUN_SAMPLE_RATE_16K)
        && (config->sample_rate != TTS_ALIYUN_SAMPLE_RATE_24K)) {
        M_LOGE("sample rate %d is not supported !", config->sample_rate);
        return -1;
    }

    if ((config->speech_rate < TTS_SPEECH_RATE_MIN) ||
		(config->speech_rate > TTS_SPEECH_RATE_MAX)) {
        M_LOGE("speech rate %d is not supported !", config->speech_rate);
        return -1;
    }

    if ((config->pitch_rate < TTS_PITCH_RATE_MIN) ||
		(config->pitch_rate > TTS_PITCH_RATE_MAX)) {
        M_LOGE("pitch rate %d is not supported !", config->pitch_rate);
        return -1;
    }

    if ((config->volume < TTS_VOLUME_MIN) || (config->volume > TTS_VOLUME_MAX)) {
        M_LOGE("volume %d is not supported !", config->volume);
        return -1;
    }

    for (i = 0; i < sizeof(voice_spec) / sizeof(voice_spec_t); i ++) {
        if (!strcmp(voice_spec[i].voice_people, config->voice)) {
            for (j = 0; j < 3; j++) {
                if ((voice_spec[i].sample_rate[j] == config->sample_rate) &&
					(voice_spec[i].sample_rate[j] != 0)) {
                    flag = 1;
                    break;
                }
            }
        }

        if (flag == 1)
            break;
    }

    if (flag == 0) {
        M_LOGE("sample rate %d not match %s !", config->sample_rate, config->voice);
        return -1;
    }

    memset(&g_tts_config, 0, sizeof(aliyun_tts_config_t));
    //strncpy(g_tts_config.app_key, config->app_key, strlen(config->app_key));
    //strncpy(g_tts_config.token, config->token, strlen(config->token));
	snprintf(g_tts_config.app_key, sizeof(g_tts_config.app_key), "%s", config->app_key);
	snprintf(g_tts_config.token, sizeof(g_tts_config.token), "%s", config->token);
    g_tts_config.format = config->format;      /* tts output format, now only support wav, pcm, mp3 */
    g_tts_config.sample_rate = config->sample_rate; /* support 8000Hz��16000Hz */
    //strncpy(g_tts_config.voice, config->voice, strlen(config->voice));     /* voice people */
    snprintf(g_tts_config.voice, sizeof(g_tts_config.voice), "%s", config->voice);
    g_tts_config.volume = config->volume;      /* 0 ~ 100 */
    g_tts_config.speech_rate = config->speech_rate; /* -500 ~ 500 */
    g_tts_config.pitch_rate = config->pitch_rate;  /* -500 ~ 500 */
    g_tts_config.text_encode_type = config->text_encode_type;

    tts_state = ALIYUN_TTS_STATE_INITED;
    return 0;
}

//by: alicloud_tts_download_task()
int uvoice_tts_aliyun_request(char *text, tts_recv_type_e recv_type,
	tts_recv_callback_t *recv_cb)
{
    int i = 0;
    int ret = 0;
    char *tts_text = NULL;
    char *utf8_text = NULL;
    int tts_text_len;
    char *tts_get_url = NULL;

    if (tts_state != ALIYUN_TTS_STATE_INITED) {
        M_LOGE("aliyun tts: not initialized\n");
        return -1;
    }

    if (!recv_cb) {
        M_LOGE("aliyun tts: recv callback is null\n");
        return -1;
    }

    tts_text_len = strlen(text);
    if(tts_text_len > UVOICE_TTS_MAX_TEXT_LEN) {
        tts_text_len = UVOICE_TTS_MAX_TEXT_LEN;
        if (recv_cb->event) {
            recv_cb->event(TTS_WARNING, "text length too long, cut to "UVOICE_TTS_MAX_TEXT_LEN_STR);
        }
    }

    tts_text = snd_zalloc(tts_text_len * 3 + 1, AFM_EXTN);
	if (!tts_text) {
		M_LOGE("alloc tts text buffer fail !\n");
		return -1;
	}

	snprintf(tts_text, tts_text_len * 3 + 1, "%s", text);
#ifdef UVOICE_TTS_SUPPORT_GBK_ENCODE
    if (g_tts_config.text_encode_type == TTS_ENCODE_GBK) {
        utf8_text = snd_zalloc(tts_text_len * 2 * 3 + 1, AFM_EXTN);
		if (!utf8_text) {
			M_LOGE("alloc utf8 text buffer fail !\n");
			goto exit_free;
		}
        uvoice_gbk2utf8(tts_text, utf8_text, tts_text_len*2);
		M_LOGI("text encode type GBK\n");
    }
    else
#endif
    {
        utf8_text = tts_text;
		M_LOGI("text encode type UTF-8\n");

    }

    uvoice_urlencode(utf8_text);

    tts_get_url = snd_zalloc(UVOICE_TTS_HTTPGET_URL_LENGTH, AFM_MAIN);
	if (!tts_get_url) {
		M_LOGE("alloc url buffer fail !\n");
		goto exit_free;
	}

    snprintf(tts_get_url, UVOICE_TTS_HTTPGET_URL_LENGTH,
		"%sappkey=%s&token=%s&text=%s&format=%s&sample_rate=%u&voice=%s&volume=%d&speech_rate=%d&pitch_rate=%d",
		ALIYUN_TTS_HTTP_URL,
		g_tts_config.app_key,
		g_tts_config.token,
		utf8_text,
		tts_format[g_tts_config.format],
		g_tts_config.sample_rate,
		g_tts_config.voice,
		g_tts_config.volume,
		g_tts_config.speech_rate, g_tts_config.pitch_rate );

    if (recv_type == TTS_RECV_URL) {
        if(recv_cb->recv_url) {
            printf("tts url is %s\n", tts_get_url);
            recv_cb->recv_url(tts_get_url);
        }
        if (recv_cb->event)
            recv_cb->event(TTS_TRANS_COMPLETE, "OK");
        ret = 0;
        goto exit_free;
    }

    if (recv_type == TTS_RECV_DATA) {

	    media_loader_t *mloader;
	    uint8_t *buffer = NULL;
	    int size        = 2048;
	    int ret_size    = 0;
	    int index       = 0;

        if(recv_cb->recv_data == NULL) {
            M_LOGE("recv_cb->recv_data is null !\n");
            ret = -1;
            goto exit_free;
        }

        mloader = snd_zalloc(sizeof(media_loader_t), AFM_EXTN);
        if (!mloader) {
            M_LOGE("alloc mloader failed !\n");
            ret = -1;
            goto exit_free;
        }

		M_LOGD("url %s\n", tts_get_url);
        ret = http_loader_create(mloader, tts_get_url);
        if (ret) {
            M_LOGE("alloc mloader failed !\n");
            ret = -1;
            goto exit_free;
        }

        buffer = snd_zalloc(size, AFM_EXTN);
		if (!buffer) {
			M_LOGE("alloc buffer fail !\n");
			ret = -1;
			http_loader_release(mloader);
			goto exit_free;
		}
        if (recv_cb->event)
            recv_cb->event(TTS_RECV_START, "");

		if (mloader->action(mloader, PLAYER_START, NULL)) {
			M_LOGE("start http load failed !\n");
			ret = -1;
	        snd_free(buffer);
	        http_loader_release(mloader);
	        snd_free(mloader);
			goto exit_free;
        }

        while (1) {
            ret_size = mloader->read(mloader, buffer, size);
            if (ret_size > 0) {
                recv_cb->recv_data(buffer, ret_size, index);
                index++;
            } else {
                break;
            }
        }

        mloader->action(mloader, PLAYER_STOP, NULL);
        if (recv_cb->event)
            recv_cb->event(TTS_RECV_COMPLETE, NULL);
        snd_free(buffer);
        http_loader_release(mloader);
        snd_free(mloader);
    }

exit_free:
    if (tts_text)
        snd_free(tts_text);
	
#ifdef UVOICE_TTS_SUPPORT_GBK_ENCODE    
	if (utf8_text)
		snd_free(utf8_text);
#endif
	if (tts_get_url)
		snd_free(tts_get_url);
    return ret;
}

int uvoice_tts_aliyun_stop()
{
	return 0;
}