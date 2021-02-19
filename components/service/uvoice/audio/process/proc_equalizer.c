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

#include "opensource/libsox/src/sox.h"

typedef struct {
	sox_effect_t *sox;
	uint8_t *proc_buffer;
	int proc_samples;
	int channels;
} equalizer_effect_t;

char equalizer_opt_fc[16] = "1000";
char equalizer_opt_width[16] = "200q";
char equalizer_opt_gain[16] = "3";


static void interleave(short *output, short *input, int samples, int channels)
{
	int channel_index;
	int sample_index;
	int samples_per_channel = samples / channels;

	for (channel_index = 0; channel_index < channels; channel_index++) {
		input += samples_per_channel * channel_index;
		for (sample_index = 0; sample_index < samples_per_channel; sample_index++)
			output[channels * sample_index + channel_index] = input[sample_index];
	}
}

static void deinterleave(short *output, short *input, int samples, int channels)
{
	int channel_index;
	int sample_index;
	int samples_per_channel = samples / channels;

	for (channel_index = 0; channel_index < channels; channel_index++) {
		output += samples_per_channel * channel_index;
		for (sample_index = 0; sample_index < samples_per_channel; sample_index++)
			output[sample_index] = input[channels * sample_index + channel_index];
	}
}
static equalizer_effect_t *g_equalizer;
int equalizer_process(equalizer_effect_t *equalizer, uint8_t *buffer, int nbytes)
{
	sox_effect_handler_t *handler;
	int proc_samples;
	int in_samples;
	int out_samples;
	int ret;

	equalizer = g_equalizer;
	if (!equalizer) {
		//snd_err("equalizer null !\n");
		return -1;
	}

	in_samples = MIN(equalizer->proc_samples, nbytes / 2);
	out_samples = in_samples;

	if (equalizer->channels > 1)
		deinterleave(equalizer->proc_buffer, buffer, in_samples, equalizer->channels);
	else
		memcpy(equalizer->proc_buffer, buffer, in_samples * 2);

	handler = &equalizer->sox->handler;

	ret = handler->flow(equalizer->sox, equalizer->proc_buffer, buffer, &in_samples, &out_samples);
	if (ret) {
		snd_err("equalizer process failed %d!\n", ret);
		return -1;
	}

	if (equalizer->channels > 1) {
		memcpy(equalizer->proc_buffer, buffer, in_samples * 2);
		interleave(buffer, equalizer->proc_buffer, in_samples, equalizer->channels);
	}

	return 0;
}

void *equalizer_create(int proc_size, int rate, int bits, int channels)
{
	equalizer_effect_t *equalizer;
	sox_effect_handler_t *handler;
	sox_signalinfo_t siginfo;
	char *options[] = {equalizer_opt_fc, equalizer_opt_width, equalizer_opt_gain};
	int ret;

	handler = sox_find_effect("equalizer");
	if (!handler) {
		snd_err("effect not found !\n");
		return NULL;
	}

	equalizer = snd_zalloc(sizeof(equalizer_effect_t), AFM_EXTN);
	if (!equalizer) {
		snd_err("alloc equalizer_effect_t failed !\n");
		return NULL;
	}

	equalizer->channels = channels;
	equalizer->proc_samples = proc_size / 2;
	equalizer->proc_buffer = snd_zalloc(proc_size, AFM_EXTN);
	if (!equalizer->proc_buffer) {
		goto __err_exit;
	}

	equalizer->sox = sox_create_effect(handler);
	if (!equalizer->sox) {
		snd_err("create effect failed !\n");
		goto __err_exit1;
	}

	equalizer->sox->in_signal.rate = rate; /* in_signal type: sox_signalinfo_t */
	equalizer->sox->in_signal.channels = channels;
	equalizer->sox->in_signal.length = proc_size;
	equalizer->sox->in_signal.precision = bits;

	ret = sox_effect_options(equalizer->sox, 3, options);
	if (ret) {
		snd_err("set equalizer options failed %d!\n", ret);
		goto __err_exit2;
	}

	ret = handler->start(equalizer->sox);
	if (ret) {
		snd_err("start equalizer failed %d!\n", ret);
		goto __err_exit2;
	}
g_equalizer= equalizer;
	snd_debug("equalizer create %p\n", equalizer);
	return equalizer;

__err_exit2:
	sox_delete_effect(equalizer->sox);
__err_exit1:
	snd_free(equalizer->proc_buffer);
__err_exit:
	snd_free(equalizer);
	return NULL;
}

int equalizer_release(equalizer_effect_t *equalizer)
{
	if (!equalizer) {
		snd_err("equalizer null !\n");
		return -1;
	}

	sox_delete_effect(equalizer->sox);
	snd_free(equalizer->proc_buffer);
	snd_free(equalizer);
	snd_debug("equalizer release\n");
	return 0;
}

