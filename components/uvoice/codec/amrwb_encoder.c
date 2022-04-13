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

#include "opensource/amrwb/enc_if.h"


#define AMRWB_MAGIC_NUMBER		"#!AMR-WB\n"

typedef struct {
	void *amrwbenc;
	uint8_t *enc_buffer;
} amrwb_encoder_t;

static int amrwb_encode_process(void *priv, uint8_t *buffer, int nbytes)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	amrwb_encoder_t *amrwb = mencoder->encoder;
	if (!amrwb) {
		M_LOGE("speex null !\n");
		return -1;
	}

	int16_t mode = 4;
	int16_t allow_dtx = 0;
	int rem_size = nbytes;
	uint8_t *ptr = buffer;
	uint8_t *enc_ptr = buffer;
	int enc_sum = 0;
	int enc_ret;
	int i;

	if (rem_size < 640) { /* 640: L_FRAME16k * sizeof(short) */
		M_LOGE("samples too less !\n");
		return -1;
	}

	while (1) {
		enc_ret = E_IF_encode(amrwb->amrwbenc, mode, ptr, amrwb->enc_buffer, allow_dtx);
		snd_memcpy(enc_ptr, amrwb->enc_buffer, enc_ret);
		enc_ptr += enc_ret;
		enc_sum += enc_ret;
		rem_size -= 640;
		//M_LOGD("enc_ret %d rem_size %d\n", enc_ret, rem_size);
		if (rem_size < 640) {
			if (rem_size > 0)
				M_LOGW("discard unproc data %d\n", rem_size);
			break;
		}
		ptr += 640;
	}

	return enc_sum;
}

static int amrwb_encode_header_gen(void *priv, void *arg)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	mencoder->header_size = strlen(AMRWB_MAGIC_NUMBER);

	char *header = snd_zalloc(mencoder->header_size, AFM_EXTN);
	if (!header) {
		M_LOGE("alloc header failed !\n");
		return -1;
	}

	memcpy(header, AMRWB_MAGIC_NUMBER, strlen(AMRWB_MAGIC_NUMBER));

	mencoder->header = header;
	mencoder->header_cplt = 1;
	return 0;
}

static int amrwb_encode_action(void *priv, recorder_action_t action, void *arg)
{
	return 0;
}

int amrwb_encoder_create(media_encoder_t *mencoder)
{
	amrwb_encoder_t *amrwb;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	amrwb = snd_zalloc(sizeof(amrwb_encoder_t), AFM_MAIN);
	if (!amrwb) {
		M_LOGE("alloc speex encoder failed !\n");
		return -1;
	}

	amrwb->amrwbenc = E_IF_init();
	if (!amrwb->amrwbenc) {
		M_LOGE("init amrwb encoder failed !\n");
		snd_free(amrwb);
		return -1;
	}

	amrwb->enc_buffer = snd_zalloc(L_FRAME16k * sizeof(short), AFM_MAIN);
	if (!amrwb->enc_buffer) {
		M_LOGE("alloc enc buffer failed !\n");
		E_IF_exit(amrwb->amrwbenc);
		snd_free(amrwb);
		return -1;
	}

	mencoder->encoder = amrwb;
	mencoder->encode = amrwb_encode_process;
	mencoder->action = amrwb_encode_action;
	mencoder->header_gen = amrwb_encode_header_gen;

	M_LOGD("amrwb encoder create\n");
	return 0;
}

int amrwb_encoder_release(media_encoder_t *mencoder)
{
	amrwb_encoder_t *amrwb;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	amrwb = mencoder->encoder;
	if (!amrwb) {
		M_LOGE("amrwb encoder null !\n");
		return -1;
	}

	snd_free(amrwb->enc_buffer);
	E_IF_exit(amrwb->amrwbenc);
	snd_free(amrwb);
	mencoder->encoder = NULL;

	M_LOGD("amrwb encoder release\n");
	return 0;
}
