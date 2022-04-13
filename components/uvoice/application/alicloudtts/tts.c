/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_types.h"
#include "uvoice_tts.h"

#include "../../../internal/uvoice_os.h"

static tts_aicloud_type_e g_aicloud_type;
static uvoice_tts_t *g_uvoice_tts;

static int uvoice_tts_init(tts_aicloud_type_e aicloud_type, tts_config_t *config)
{
	g_aicloud_type = aicloud_type;
	if (aicloud_type == TTS_AICLOUD_ALIYUN) {
#ifdef ALICLOUD_TTS_SUPPORT
		return uvoice_tts_aliyun_init(config);
#else
		return -1;
#endif
	}

	return -1;
}

static int uvoice_tts_request(char *text, tts_recv_type_e recv_type,
	tts_recv_callback_t *recv_cb)
{
    if(g_aicloud_type == TTS_AICLOUD_ALIYUN) {
#ifdef ALICLOUD_TTS_SUPPORT
        return uvoice_tts_aliyun_request(text, recv_type, recv_cb);
#else
        return -1;
#endif
    }

    return -1;
}

static int uvoice_tts_stop()
{
	if(g_aicloud_type == TTS_AICLOUD_ALIYUN) {
#ifdef ALICLOUD_TTS_SUPPORT
		return uvoice_tts_stop();
#else
		return -1;
#endif
	}

	return -1;
}

uvoice_tts_t *uvoice_tts_create(void)
{
	uvoice_tts_t *uvoice_tts = g_uvoice_tts;

	if (uvoice_tts)
		return uvoice_tts;

	uvoice_tts = snd_zalloc(sizeof(uvoice_tts_t), AFM_EXTN);
	if (!uvoice_tts) {
		M_LOGE("alloc uvoice_tts failed !\n");
		return NULL;
	}

	g_uvoice_tts = uvoice_tts;

	uvoice_tts->tts_init = uvoice_tts_init;
	uvoice_tts->tts_request = uvoice_tts_request;
	uvoice_tts->tts_stop = uvoice_tts_stop;

	M_LOGI("uvoice tts create\n");
	return uvoice_tts;
}

int uvoice_tts_release(uvoice_tts_t *uvoice_tts)
{
	if (uvoice_tts) {
		snd_free(uvoice_tts);
		g_uvoice_tts = NULL;
		M_LOGI("uvoice tts release\n");
	}
	return 0;
}

