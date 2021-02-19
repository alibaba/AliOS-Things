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

#include "opensource/webrtc/modules/audio_processing/ns/noise_suppression_x.h"


typedef struct {
	short *in[2];
	short *out[2];
	int32_t *filter_state[4];
	int samples;
	int band_samples;
	int band_num;
	NsxHandle *inst;
} webrtc_ns_t;


static int ns_buffer_alloc(webrtc_ns_t *webrtc_ns)
{
	int i;

	if (!webrtc_ns) {
		snd_err("webrtc_ns null !\n");
		return -1;
	}

	for (i = 0; i < webrtc_ns->band_num; i++) {
		webrtc_ns->in[i] = snd_zalloc(
			webrtc_ns->band_samples * sizeof(short), AFM_MAIN);
		if (!webrtc_ns->in) {
			snd_err("alloc in buffer failed !\n");
			return -1;
		}

		webrtc_ns->out[i] = snd_zalloc(
			webrtc_ns->band_samples * sizeof(short), AFM_MAIN);
		if (!webrtc_ns->out) {
			snd_err("alloc out buffer failed !\n");
			return -1;
		}
	}

	if (webrtc_ns->band_num == 2) {
		for (i = 0; i < 4; i++) {
			webrtc_ns->filter_state[i] = snd_zalloc(
				6 * sizeof(int32_t), AFM_MAIN);
			if (!webrtc_ns->filter_state[i]) {
				snd_err("alloc filter state failed !\n");
				return -1;
			}
		}
	}

	return 0;
}

static int ns_buffer_free(webrtc_ns_t *webrtc_ns)
{
	int i;

	if (!webrtc_ns) {
		snd_err("webrtc_ns null !\n");
		return -1;
	}

	for (i = 0; i < 4; i++) {
		if (webrtc_ns->filter_state[i]) {
			snd_free(webrtc_ns->filter_state[i]);
			webrtc_ns->filter_state[i] = NULL;
		}
	}

	for (i = 0; i < webrtc_ns->band_num; i++) {
		if (webrtc_ns->in[i]) {
			snd_free(webrtc_ns->in[i]);
			webrtc_ns->in[i] = NULL;
		}

		if (webrtc_ns->out[i]) {
			snd_free(webrtc_ns->out[i]);
			webrtc_ns->out[i] = NULL;
		}
	}

	return 0;
}

int noise_suppression_process(struct noise_suppression *ns,
	uint8_t *buffer, int nbytes)
{
	webrtc_ns_t *webrtc_ns;
	int proc_size;
	int ret;
	int i;

	if (!ns) {
		snd_err("ns null !\n");
		return -1;
	}

	webrtc_ns = ns->handler;
	if (!webrtc_ns) {
		snd_err("webrtc_ns null !\n");
		return -1;
	}

	proc_size = webrtc_ns->samples * sizeof(short);

	if (nbytes % proc_size != 0) {
		snd_err("invalid input size !\n");
		return -1;
	}

	if (webrtc_ns->band_num == 1) {
		for (i = 0; i < nbytes; i += proc_size) {
			memcpy(webrtc_ns->in[0], buffer + i, proc_size);
			WebRtcNsx_Process(webrtc_ns->inst,
				webrtc_ns->in, 1, webrtc_ns->out);
			memcpy(buffer + i, webrtc_ns->out[0], proc_size);
		}
	} else if (webrtc_ns->band_num == 2) {
		for (i = 0; i < nbytes; i += proc_size) {
			WebRtcSpl_AnalysisQMF(buffer + i,
				webrtc_ns->samples,
				webrtc_ns->in[0],
				webrtc_ns->in[1],
  				webrtc_ns->filter_state[0],
  				webrtc_ns->filter_state[1]);

			WebRtcNsx_Process(webrtc_ns->inst,
				webrtc_ns->in, 2, webrtc_ns->out);

			WebRtcSpl_SynthesisQMF(webrtc_ns->out[0],
				webrtc_ns->out[1],
				webrtc_ns->band_samples,
				buffer + i,
  				webrtc_ns->filter_state[2],
  				webrtc_ns->filter_state[3]);
		}
	}

	return 0;
}

int noise_suppression_create(struct noise_suppression *ns,
	int rate, int bits, int samples, int mode)
{
	webrtc_ns_t *webrtc_ns;
	int ret;

	if (!ns) {
		snd_err("ns null !\n");
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

	webrtc_ns = snd_zalloc(sizeof(webrtc_ns_t), AFM_EXTN);
	if (!webrtc_ns) {
		snd_err("alloc webrtc ns failed !\n");
		return -1;
	}

	webrtc_ns->inst = WebRtcNsx_Create();
	if (!webrtc_ns->inst) {
		snd_err("create webrtc nsx failed !\n");
		snd_free(webrtc_ns);
		return -1;
	}

	ret = WebRtcNsx_Init(webrtc_ns->inst, rate);
	if (ret) {
		snd_err("init webrtc nsx failed !\n");
		WebRtcNsx_Free(webrtc_ns->inst);
		snd_free(webrtc_ns);
		return -1;
	}

	webrtc_ns->samples = rate / 100;
	if (rate == 48000 || rate == 32000) {
		webrtc_ns->band_num = 2;
		webrtc_ns->band_samples = webrtc_ns->samples / 2;
	} else {
		webrtc_ns->band_num = 1;
		webrtc_ns->band_samples = webrtc_ns->samples;
	}

	ret = WebRtcNsx_set_policy(webrtc_ns->inst, mode);
	if (ret) {
		snd_err("set webrtc policy failed %d!\n", ret);
		WebRtcNsx_Free(webrtc_ns->inst);
		snd_free(webrtc_ns);
		return -1;
	}

	if (ns_buffer_alloc(webrtc_ns)) {
		snd_err("alloc ns buffer failed !\n");
		ns_buffer_free(webrtc_ns);
		WebRtcNsx_Free(webrtc_ns->inst);
		snd_free(webrtc_ns);
		return -1;
	}

	snd_info("noise suppression create\n");
	ns->handler = webrtc_ns;
	return 0;
}

int noise_suppression_release(struct noise_suppression *ns)
{
	webrtc_ns_t *webrtc_ns;

	if (!ns) {
		snd_err("ns null !\n");
		return -1;
	}

	webrtc_ns = ns->handler;
	if (!webrtc_ns) {
		snd_err("webrtc_ns null !\n");
		return -1;
	}

	ns_buffer_free(webrtc_ns);

	WebRtcNsx_Free(webrtc_ns->inst);
	snd_free(webrtc_ns);
	ns->handler = NULL;

	snd_info("noise suppression release\n");
	return 0;
}

