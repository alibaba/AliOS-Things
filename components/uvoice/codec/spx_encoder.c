/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_recorder.h"

#include "uvoice_common.h"
#include "uvoice_record.h"

#include "opensource/speex/include/speex/speex_callbacks.h"


typedef struct {
	void *speex;
	SpeexBits bits;
	int frame_size;
} spx_encoder_t;

static int spx_encode_process(void *priv, uint8_t *buffer, int nbytes)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	spx_encoder_t *spx = mencoder->encoder;
	if (!spx) {
		M_LOGE("speex null !\n");
		return -1;
	}

	spx_int16_t *ptr;
	int samples = nbytes / sizeof(short);
	int encode_sum = 0;
	int frame_size = 0;
	int ret;
	int i;

	int time_ms = os_current_time();

	for (i = 0; i < samples; i += spx->frame_size) {
		speex_bits_reset(&spx->bits);
		ptr = (spx_int16_t *)buffer + i;
		speex_encode_int(spx->speex, ptr, &spx->bits);
		frame_size = speex_bits_write(&spx->bits, buffer + encode_sum + sizeof(int),
			spx->frame_size * sizeof(spx_int16_t));
		if (frame_size + sizeof(int) >= spx->frame_size * sizeof(spx_int16_t)) {
			M_LOGE("encoded data overflow !\n");
			return -1;
		}
		snd_memcpy(buffer + encode_sum, &frame_size, sizeof(int));
		encode_sum += frame_size + sizeof(int);
	}

	//time_ms = os_current_time() - time_ms;
	//M_LOGD("time %dms ratio %d/%d\n", time_ms, frame_size, nbytes);

	return encode_sum;
}

static int spx_encode_action(void *priv, recorder_action_t action, void *arg)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	spx_encoder_t *spx;
	int ret;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	spx = mencoder->encoder;
	if (!spx) {
		M_LOGE("spx encoder null !\n");
		return -1;
	}

	if (action == RECORDER_START) {
		if ((mencoder->frames * mencoder->channels) % spx->frame_size != 0) {
			M_LOGE("frame size is not integral multiple of %d !\n",
				spx->frame_size);
			return -1;
		}
	}

	return 0;
}

int spx_encoder_create(media_encoder_t *mencoder)
{
	spx_encoder_t *spx;
	spx_int32_t temp;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	spx = snd_zalloc(sizeof(spx_encoder_t), AFM_MAIN);
	if (!spx) {
		M_LOGE("alloc speex encoder failed !\n");
		return -1;
	}

	spx->speex = speex_encoder_init(
		speex_lib_get_mode(SPEEX_MODEID_NB)); //nb_encoder_init()
	if (!spx->speex) {
		M_LOGE("init speex failed !\n");
		snd_free(spx);
		return -1;
	}

	temp = 0;
	speex_encoder_ctl(spx->speex, SPEEX_SET_VBR, &temp); //nb_encoder_ctl()
	temp = 2;
	speex_encoder_ctl(spx->speex, SPEEX_SET_QUALITY, &temp);
	temp = 2;
	speex_encoder_ctl(spx->speex, SPEEX_SET_COMPLEXITY, &temp);

	speex_encoder_ctl(spx->speex, SPEEX_GET_FRAME_SIZE, &spx->frame_size);
	M_LOGD("speex frame size %d\n", spx->frame_size); /* frame size in sizeof(short) */

	speex_bits_init(&spx->bits);

	mencoder->encoder = spx;
	mencoder->encode = spx_encode_process;
	mencoder->action = spx_encode_action;

	M_LOGI("speex encoder create\n");
	return 0;
}

int spx_encoder_release(media_encoder_t *mencoder)
{
	spx_encoder_t *spx;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	spx = mencoder->encoder;
	if (!spx) {
		M_LOGE("speex encoder null !\n");
		return -1;
	}

	speex_bits_destroy(&spx->bits);
	speex_encoder_destroy(spx->speex);
	snd_free(spx);
	mencoder->encoder = NULL;

	M_LOGI("speex encoder release\n");
	return 0;
}