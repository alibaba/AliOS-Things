/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_common.h"

#include "uvoice_os.h"
#include "uvoice_audio.h"

#include "audio_common.h"
#include "audio_stream.h"
#include "audio_vad.h"

#include "opensource/webrtc/common_audio/vad/include/webrtc_vad.h"


typedef struct {
	short *in[2];
	short *out[2];
	int32_t *filter_state[4];
	int rate;
	int samples;
	int band_samples;
	int band_num;
	VadInst *inst;
} webrtc_vad_t;


static int vad_buffer_alloc(webrtc_vad_t *webrtc_vad)
{
	int i;

	if (!webrtc_vad) {
		snd_err("webrtc_vad null !\n");
		return -1;
	}

	for (i = 0; i < webrtc_vad->band_num; i++) {
		webrtc_vad->in[i] = snd_zalloc(
			webrtc_vad->band_samples * sizeof(short), AFM_MAIN);
		if (!webrtc_vad->in) {
			snd_err("alloc in buffer failed !\n");
			return -1;
		}

		webrtc_vad->out[i] = snd_zalloc(
			webrtc_vad->band_samples * sizeof(short), AFM_MAIN);
		if (!webrtc_vad->out) {
			snd_err("alloc out buffer failed !\n");
			return -1;
		}
	}

	if (webrtc_vad->band_num == 2) {
		for (i = 0; i < 4; i++) {
			webrtc_vad->filter_state[i] = snd_zalloc(
				6 * sizeof(int32_t), AFM_MAIN);
			if (!webrtc_vad->filter_state[i]) {
				snd_err("alloc filter state failed !\n");
				return -1;
			}
		}
	}

	return 0;
}

static int vad_buffer_free(webrtc_vad_t *webrtc_vad)
{
	int i;

	if (!webrtc_vad) {
		snd_err("webrtc_vad null !\n");
		return -1;
	}

	for (i = 0; i < 4; i++) {
		if (webrtc_vad->filter_state[i]) {
			snd_free(webrtc_vad->filter_state[i]);
			webrtc_vad->filter_state[i] = NULL;
		}
	}

	for (i = 0; i < webrtc_vad->band_num; i++) {
		if (webrtc_vad->in[i]) {
			snd_free(webrtc_vad->in[i]);
			webrtc_vad->in[i] = NULL;
		}

		if (webrtc_vad->out[i]) {
			snd_free(webrtc_vad->out[i]);
			webrtc_vad->out[i] = NULL;
		}
	}

	return 0;
}

/*
 * return value: 0 - noise, 1 - speech
 */
int vad_process(struct voice_active_detect *vad,
	const uint8_t *buffer, int nbytes)
{
	webrtc_vad_t *webrtc_vad;
	int proc_size;
	int ret = 0;
	int i;

	if (!vad) {
		snd_err("vad null !\n");
		return -1;
	}

	webrtc_vad = vad->handler;
	if (!webrtc_vad) {
		snd_err("webrtc_vad null !\n");
		return -1;
	}

	proc_size = webrtc_vad->samples * sizeof(short);

	if (nbytes % proc_size != 0) {
		snd_err("invalid input size !\n");
		return -1;
	}

	if (webrtc_vad->band_num == 1) {
		for (i = 0; i < nbytes; i += proc_size) {
			memcpy(webrtc_vad->in[0], buffer + i, proc_size);
			ret = WebRtcVad_Process(webrtc_vad->inst,
				webrtc_vad->rate,
				webrtc_vad->in[0], webrtc_vad->samples);
		}
	} else if (webrtc_vad->band_num == 2) {
		for (i = 0; i < nbytes; i += proc_size) {
			WebRtcSpl_AnalysisQMF(buffer + i,
				webrtc_vad->samples,
				webrtc_vad->in[0],
				webrtc_vad->in[1],
  				webrtc_vad->filter_state[0],
  				webrtc_vad->filter_state[1]);

			ret = WebRtcVad_Process(webrtc_vad->inst,
				webrtc_vad->rate,
				webrtc_vad->in[0], webrtc_vad->samples);
		}
	}

	return ret;
}

int vad_create(struct voice_active_detect *vad,
	int rate, int samples, int mode)
{
	webrtc_vad_t *webrtc_vad;
	int ret;

	if (!vad) {
		snd_err("vad null !\n");
		return -1;
	}

	if (samples % (rate / 100) != 0) {
		snd_err("samples and rate not mulitple !\n");
		return -1;
	}

	webrtc_vad = snd_zalloc(sizeof(webrtc_vad_t), AFM_EXTN);
	if (!webrtc_vad) {
		snd_err("alloc webrtc ns failed !\n");
		return -1;
	}

	webrtc_vad->inst = WebRtcVad_Create();
	if (!webrtc_vad->inst) {
		snd_err("create webrtc vad failed !\n");
		snd_free(webrtc_vad);
		return -1;
	}

	ret = WebRtcVad_Init(webrtc_vad->inst);
	if (ret) {
		snd_err("init webrtc vad failed %d!\n", ret);
		WebRtcVad_Free(webrtc_vad->inst);
		snd_free(webrtc_vad);
		return -1;
	}

	webrtc_vad->samples = samples;
	if (rate == 48000 || rate == 32000) {
		webrtc_vad->band_num = 2;
		webrtc_vad->band_samples = webrtc_vad->samples / 2;
	} else {
		webrtc_vad->band_num = 1;
		webrtc_vad->band_samples = webrtc_vad->samples;
	}
	webrtc_vad->band_num = 1;
	webrtc_vad->band_samples = webrtc_vad->samples;
	webrtc_vad->rate = rate;

	ret = WebRtcVad_set_mode(webrtc_vad->inst, mode);
	if (ret) {
		snd_err("set webrtc vad mode failed %d!\n", ret);
		WebRtcVad_Free(webrtc_vad->inst);
		snd_free(webrtc_vad);
		return -1;
	}

	if (vad_buffer_alloc(webrtc_vad)) {
		snd_err("alloc vad buffer failed !\n");
		vad_buffer_free(webrtc_vad);
		WebRtcVad_Free(webrtc_vad->inst);
		snd_free(webrtc_vad);
		return -1;
	}

	snd_debug("vad create\n");
	vad->handler = webrtc_vad;
	return 0;
}

int vad_release(struct voice_active_detect *vad)
{
	webrtc_vad_t *webrtc_vad;

	if (!vad) {
		snd_err("vad null !\n");
		return -1;
	}

	webrtc_vad = vad->handler;
	if (!webrtc_vad) {
		snd_err("webrtc_vad null !\n");
		return -1;
	}

	vad_buffer_free(webrtc_vad);

	WebRtcVad_Free(webrtc_vad->inst);
	snd_free(webrtc_vad);
	vad->handler = NULL;

	snd_debug("vad release\n");
	return 0;
}

