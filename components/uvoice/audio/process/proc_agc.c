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
#include "audio_common.h"
#include "audio_stream.h"

#include "opensource/webrtc/modules/audio_processing/agc/gain_control.h"


typedef struct {
	int16_t *in[2];
	int16_t *out[2];
	int32_t *filter_state[4];
	int samples;
	int band_samples;
	int band_num;
	void *inst;
} webrtc_agc_t;

static int agc_buffer_alloc(webrtc_agc_t *webrtc_agc)
{
	int i;

	if (!webrtc_agc) {
		snd_err("webrtc_agc null !\n");
		return -1;
	}

	for (i = 0; i < webrtc_agc->band_num; i++) {
		webrtc_agc->in[i] = snd_zalloc(
			webrtc_agc->band_samples * sizeof(int16_t), AFM_MAIN);
		if (!webrtc_agc->in[i]) {
			snd_err("alloc in buffer failed !\n");
			return -1;
		}

		webrtc_agc->out[i] = snd_zalloc(
			webrtc_agc->band_samples * sizeof(short), AFM_MAIN);
		if (!webrtc_agc->out) {
			snd_err("alloc out buffer failed !\n");
			return -1;
		}
	}

	if (webrtc_agc->band_num == 2) {
		for (i = 0; i < 4; i++) {
			webrtc_agc->filter_state[i] = snd_zalloc(
				6 * sizeof(int32_t), AFM_MAIN);
			if (!webrtc_agc->filter_state[i]) {
				snd_err("alloc filter state failed !\n");
				return -1;
			}
		}
	}

	return 0;
}

static int agc_buffer_free(webrtc_agc_t *webrtc_agc)
{
	int i;

	if (!webrtc_agc) {
		snd_err("webrtc_agc null !\n");
		return -1;
	}

	for (i = 0; i < 4; i++) {
		if (webrtc_agc->filter_state[i]) {
			snd_free(webrtc_agc->filter_state[i]);
			webrtc_agc->filter_state[i] = NULL;
		}
	}

	for (i = 0; i < webrtc_agc->band_num; i++) {
		if (webrtc_agc->in[i]) {
			snd_free(webrtc_agc->in[i]);
			webrtc_agc->in[i] = NULL;
		}

		if (webrtc_agc->out[i]) {
			snd_free(webrtc_agc->out[i]);
			webrtc_agc->out[i] = NULL;
		}
	}

	return 0;
}

int auto_gain_control_process(struct auto_gain_control *agc,
	uint8_t *buffer, int nbytes)
{
	webrtc_agc_t *webrtc_agc;
	int level_in = 0;
	int level_out = 0;
	uint8_t saturation_warning;
	int proc_size;
	int ret;
	int i;

	if (!agc) {
		snd_err("agc null !\n");
		return -1;
	}

	webrtc_agc = agc->handler;
	if (!webrtc_agc) {
		snd_err("webrtc_agc null !\n");
		return -1;
	}

	proc_size = webrtc_agc->samples * sizeof(int16_t);

	if (nbytes % proc_size != 0) {
		snd_err("invalid input size !\n");
		return -1;
	}

	if (webrtc_agc->band_num == 1) {
		for (i = 0; i < nbytes; i += proc_size) {
			memcpy(webrtc_agc->in[0], buffer + i, proc_size);
			level_in = level_out;
			ret = WebRtcAgc_Process(webrtc_agc->inst,
				webrtc_agc->in,
				1,
				webrtc_agc->samples,
				webrtc_agc->out,
				level_in,
				&level_out,
				0,
				&saturation_warning);
			if (ret) {
				snd_err("agc process failed %d!\n", ret);
				return -1;
			}
			memcpy(buffer + i, webrtc_agc->out[0], proc_size);
		}
	} else if (webrtc_agc->band_num == 2) {
		for (i = 0; i < nbytes; i += proc_size) {
			WebRtcSpl_AnalysisQMF(buffer + i,
				webrtc_agc->samples,
				webrtc_agc->in[0],
				webrtc_agc->in[1],
  				webrtc_agc->filter_state[0],
  				webrtc_agc->filter_state[1]);

			level_in = level_out;
			ret = WebRtcAgc_Process(webrtc_agc->inst,
				webrtc_agc->in,
				2,
				webrtc_agc->band_samples,
				webrtc_agc->out,
				level_in,
				&level_out,
				0,
				&saturation_warning);
			if (ret) {
				snd_err("agc process failed %d!\n", ret);
				return -1;
			}

			WebRtcSpl_SynthesisQMF(webrtc_agc->out[0],
				webrtc_agc->out[1],
				webrtc_agc->band_samples,
				buffer + i,
  				webrtc_agc->filter_state[2],
  				webrtc_agc->filter_state[3]);
		}
	}

	return 0;
}

int auto_gain_control_create(struct auto_gain_control *agc,
	int rate, int bits, int samples)
{
	webrtc_agc_t *webrtc_agc;
	WebRtcAgcConfig config;
	int ret;

	if (!agc) {
		snd_err("agc null !\n");
		return -1;
	}

	if (bits != 16) {
		snd_err("bits %d ns not support !\n", bits);
		return -1;
	}

	webrtc_agc = snd_zalloc(sizeof(webrtc_agc_t), AFM_EXTN);
	if (!webrtc_agc) {
		snd_err("alloc webrtc ns failed !\n");
		return -1;
	}

	if (rate == 32000 || rate == 48000) {
		webrtc_agc->samples = 320;
		webrtc_agc->band_samples = 160;
		webrtc_agc->band_num = 2;
	} else {
		webrtc_agc->samples = rate / 100;
		webrtc_agc->band_samples = webrtc_agc->samples;
		webrtc_agc->band_num = 1;
	}

	if (samples % webrtc_agc->samples != 0) {
		snd_err("samples and rate not mulitple !\n");
		snd_free(webrtc_agc);
		return -1;
	}

	if (agc_buffer_alloc(webrtc_agc)) {
		snd_err("alloc buffer failed !\n");
		snd_free(webrtc_agc);
		return -1;
	}

	webrtc_agc->inst = WebRtcAgc_Create();
	if (!webrtc_agc->inst) {
		snd_err("create webrtc agc failed !\n");
		snd_free(webrtc_agc);
		return -1;
	}

	ret = WebRtcAgc_Init(webrtc_agc->inst, 0, 255, kAgcModeFixedDigital, rate);
	if (ret) {
		snd_err("init webrtc agc failed !\n");
		WebRtcAgc_Free(webrtc_agc->inst);
		agc_buffer_free(webrtc_agc);
		snd_free(webrtc_agc);
		return -1;
	}

	WebRtcAgc_get_config(webrtc_agc->inst, &config);

	config.compressionGaindB = 20;
	config.limiterEnable     = 1;
	config.targetLevelDbfs   = 3;
	WebRtcAgc_set_config(webrtc_agc->inst, config);

	snd_info("auto gain control create\n");
	agc->handler = webrtc_agc;
	return 0;
}

int auto_gain_control_release(struct auto_gain_control *agc)
{
	webrtc_agc_t *webrtc_agc;

	if (!agc) {
		snd_err("agc null !\n");
		return -1;
	}

	webrtc_agc = agc->handler;
	if (!webrtc_agc) {
		snd_err("webrtc_agc null !\n");
		return -1;
	}

	agc_buffer_free(webrtc_agc);
	WebRtcAgc_Free(webrtc_agc->inst);
	snd_free(webrtc_agc);
	agc->handler = NULL;

	snd_info("auto gain control release\n");
	return 0;
}

