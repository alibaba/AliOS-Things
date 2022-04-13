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

#include "opensource/amr/sp_enc.h"


#define AMR_MAGIC_NUMBER		"#!AMR\n"

typedef struct {
	void *amrenc;
	uint8_t *enc_buffer;
} amr_encoder_t;

static int amr_encode_process(void *priv, uint8_t *buffer, int nbytes)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	amr_encoder_t *amr = mencoder->encoder;
	if (!amr) {
		M_LOGE("speex null !\n");
		return -1;
	}

	enum Mode req_mode = MR515;
	int rem_size = nbytes;
	uint8_t *ptr = buffer;
	uint8_t *enc_ptr = buffer;
	int enc_sum = 0;
	int enc_ret;
	int i;

	if (rem_size < 320) {
		M_LOGE("samples too less !\n");
		return -1;
	}

	while (1) {
		enc_ret = Encoder_Interface_Encode(amr->amrenc, req_mode, ptr, amr->enc_buffer, 0);
		snd_memcpy(enc_ptr, amr->enc_buffer, enc_ret);
		enc_ptr += enc_ret;
		enc_sum += enc_ret;
		rem_size -= 320;
		//M_LOGD("enc_ret %d rem_size %d\n", enc_ret, rem_size);
		if (rem_size < 320) {
			if (rem_size > 0)
				M_LOGW("discard unproc data %d\n", rem_size);
			break;
		}
		ptr += 320;
	}

	return enc_sum;
}

static int amr_encode_header_gen(void *priv, void *arg)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	mencoder->header_size = strlen(AMR_MAGIC_NUMBER);

	char *header = snd_zalloc(mencoder->header_size, AFM_EXTN);
	if (!header) {
		M_LOGE("alloc header failed !\n");
		return -1;
	}

	memcpy(header, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER));

	mencoder->header = header;
	mencoder->header_cplt = 1;
	return 0;
}

static int amr_encode_action(void *priv, recorder_action_t action, void *arg)
{
	return 0;
}

int amr_encoder_create(media_encoder_t *mencoder)
{
	amr_encoder_t *amr;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	amr = snd_zalloc(sizeof(amr_encoder_t), AFM_MAIN);
	if (!amr) {
		M_LOGE("alloc speex encoder failed !\n");
		return -1;
	}

	amr->amrenc = Encoder_Interface_init(1);
	if (!amr->amrenc) {
		M_LOGE("init amrenc failed !\n");
		snd_free(amr);
		return -1;
	}

	amr->enc_buffer = snd_zalloc(160 * sizeof(short), AFM_MAIN);
	if (!amr->enc_buffer) {
		M_LOGE("alloc enc buffer failed !\n");
		Encoder_Interface_exit(amr->amrenc);
		snd_free(amr);
		return -1;
	}

	mencoder->encoder = amr;
	mencoder->encode = amr_encode_process;
	mencoder->action = amr_encode_action;
	mencoder->header_gen = amr_encode_header_gen;

	M_LOGD("amr encoder create\n");
	return 0;
}

int amr_encoder_release(media_encoder_t *mencoder)
{
	amr_encoder_t *amr;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	amr = mencoder->encoder;
	if (!amr) {
		M_LOGE("amr encoder null !\n");
		return -1;
	}

	snd_free(amr->enc_buffer);
	Encoder_Interface_exit(amr->amrenc);
	snd_free(amr);
	mencoder->encoder = NULL;

	M_LOGD("amr encoder release\n");
	return 0;
}
