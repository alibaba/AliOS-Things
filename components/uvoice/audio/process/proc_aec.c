/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_os.h"
#include "uvoice_audio.h"
#include "uvoice_common.h"
#include "audio_common.h"
#include "audio_stream.h"
#include "audio_aec.h"

#include "opensource/webrtc/modules/audio_processing/aec/include/echo_cancellation.h"


typedef struct {
	int16_t *out;
	int16_t *far;
	int16_t *near;
	int samples;
	int band_samples;
	int band_num;
	void *inst;
} webrtc_aec_t;

static int aec_buffer_alloc(webrtc_aec_t *webrtc_aec)
{
	if (!webrtc_aec) {
		snd_err("webrtc_aec null !\n");
		return -1;
	}

	webrtc_aec->out = snd_zalloc(
		webrtc_aec->samples * sizeof(int16_t), AFM_MAIN);
	if (!webrtc_aec->out) {
		snd_err("alloc out buffer failed !\n");
		return -1;
	}

	webrtc_aec->far = snd_zalloc(
		webrtc_aec->samples * sizeof(int16_t), AFM_MAIN);
	if (!webrtc_aec->far) {
		snd_err("alloc far buffer failed !\n");
		return -1;
	}

	webrtc_aec->near = snd_zalloc(
		webrtc_aec->samples * sizeof(int16_t), AFM_MAIN);
	if (!webrtc_aec->near) {
		snd_err("alloc near buffer failed !\n");
		return -1;
	}

	return 0;
}

static int aec_buffer_free(webrtc_aec_t *webrtc_aec)
{
	if (!webrtc_aec) {
		snd_err("webrtc_aec null !\n");
		return -1;
	}

	if (webrtc_aec->near) {
		snd_free(webrtc_aec->near);
		webrtc_aec->near = NULL;
	}

	if (webrtc_aec->far) {
		snd_free(webrtc_aec->far);
		webrtc_aec->far = NULL;
	}

	if (webrtc_aec->out) {
		snd_free(webrtc_aec->out);
		webrtc_aec->out = NULL;
	}

	return 0;
}

int echo_cancellation_process(struct aec_handler *ec,
	uint8_t *near, uint8_t *far, int nbytes)
{
	webrtc_aec_t *webrtc_aec;
	int proc_size;
	int ret;
	int i;

	if (!ec) {
		snd_err("ec null !\n");
		return -1;
	}

	webrtc_aec = ec->aec_inst;
	if (!webrtc_aec) {
		snd_err("webrtc_aec null !\n");
		return -1;
	}

	proc_size = webrtc_aec->samples * sizeof(int16_t);

	if (nbytes % proc_size != 0) {
		snd_err("invalid input size !\n");
		return -1;
	}

	for (i = 0; i < nbytes; i += proc_size) {
		memcpy(webrtc_aec->far, far + i, proc_size);
		WebRtcAec_BufferFarend(webrtc_aec->inst,
			webrtc_aec->far,
			webrtc_aec->samples);

		memcpy(webrtc_aec->near, near + i, proc_size);
		WebRtcAec_Process(webrtc_aec->inst,
			webrtc_aec->near,
			NULL,
			webrtc_aec->out,
			NULL,
			webrtc_aec->samples,
			10,
			0);
		memcpy(near + i, webrtc_aec->out, proc_size);
	}

	return 0;
}

int echo_cancellation_create(struct aec_handler *aec,
	int rate, int bits, int samples)
{
	webrtc_aec_t *webrtc_aec;
	int ret;

	if (!aec) {
		snd_err("aec null !\n");
		return -1;
	}

	if (bits != 16) {
		snd_err("bits %d ns not support !\n", bits);
		return -1;
	}

	if (samples % (rate / 100) != 0) {
		snd_err("samples and rate not mulitple !\n");
		return -1;
	}

	webrtc_aec = snd_zalloc(sizeof(webrtc_aec_t), AFM_EXTN);
	if (!webrtc_aec) {
		snd_err("alloc webrtc ns failed !\n");
		return -1;
	}

	webrtc_aec->samples = rate / 100;

	if (aec_buffer_alloc(webrtc_aec)) {
		snd_err("alloc buffer failed !\n");
		snd_free(webrtc_aec);
		return -1;
	}

	ret = WebRtcAec_Create(&webrtc_aec->inst);
	if (ret) {
		snd_err("create webrtc aec failed %d!\n", ret);
		snd_free(webrtc_aec);
		return -1;
	}

	ret = WebRtcAec_Init(webrtc_aec->inst, rate, rate);
	if (ret) {
		snd_err("init webrtc aec failed !\n");
		WebRtcAec_Free(webrtc_aec->inst);
		aec_buffer_free(webrtc_aec);
		snd_free(webrtc_aec);
		return -1;
	}

	snd_info("echo cancellation create\n");
	aec->aec_inst = webrtc_aec;
	return 0;
}

int echo_cancellation_release(struct aec_handler *aec)
{
	webrtc_aec_t *webrtc_aec;

	if (!aec) {
		snd_err("aec null !\n");
		return -1;
	}

	webrtc_aec = aec->aec_inst;
	if (!webrtc_aec) {
		snd_err("webrtc_aec null !\n");
		return -1;
	}

	aec_buffer_free(webrtc_aec);
	WebRtcAec_Free(webrtc_aec->inst);
	snd_free(webrtc_aec);
	aec->aec_inst = NULL;

	snd_info("echo cancellation release\n");
	return 0;
}

