/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "uvoice_os.h"
#include "uvoice_audio.h"
#include "uvoice_common.h"
#include "audio_common.h"
#include "audio_stream.h"

#include "opensource/speexdsp/include/speex/speex_resampler.h"


#define SPEEX_RESAMPLE_ENABLE


typedef struct {
	int sbits;
	int channels;
	int src_rate;
	int dst_rate;
	uint8_t *out_buffer;
	float *fin;
	float *fout;
	int out_buffer_size;
	int fin_samples;
	int fout_samples;
	int out_coeff;
	int sample_bytes;
	int quality;
	SpeexResamplerState *speex_state;
	os_mutex_t lock;
} resampler_inst_t;


static void float_to_short_interleave(short *output, float *input,
	int samples_per_channel, int channels)
{
	int channel_index;
	int sample_index;

	for (channel_index = 0; channel_index < channels; channel_index++) {
		input += samples_per_channel * channel_index;
		for (sample_index = 0; sample_index < samples_per_channel; sample_index++)
			output[channels * sample_index + channel_index] = (short)input[sample_index];
				//floor(.5 + input[sample_index]);
	}
}

static void short_to_float_deinterleave(float *output, short *input,
	int samples_per_channel, int channels, int channel_gap)
{
	int channel_index;
	int sample_index;

	for (channel_index = 0; channel_index < channels; channel_index++) {
		for (sample_index = 0; sample_index < samples_per_channel; sample_index++)
			output[sample_index] =
				(float)input[channels * sample_index + channel_index];
		output += channel_gap;
	}
}

int uvoice_resampler_process(void *resampler, uint8_t *input, int input_len,
	uint8_t **output, int *output_len)
{
	resampler_inst_t *inst = (resampler_inst_t *)resampler;
	spx_uint32_t ilen;
	spx_uint32_t olen;
	int input_samples;
	int fin_samples;
	int fout_samples;
	int out_buffer_size;
	spx_uint32_t output_samples;
	int channels;
	int temp;
	int ret;
	int i;

	if (!inst) {
		*output = input;
		*output_len = input_len;
		return 0;
	}

	os_mutex_lock(inst->lock, OS_WAIT_FOREVER);

	if (inst->src_rate == inst->dst_rate) {
		*output = input;
		*output_len = input_len;
		os_mutex_unlock(inst->lock);
		return 0;
	}

	channels = inst->channels;
	input_samples = (input_len / inst->sample_bytes) / channels;
	fin_samples = (input_samples / 100 + 1) * 100;
	fout_samples = fin_samples * inst->out_coeff;

#ifdef SPEEX_RESAMPLE_ENABLE
	if (fin_samples != inst->fin_samples) {
		if (inst->fin) {
			snd_free(inst->fin);
			inst->fin = NULL;
			inst->fin_samples = 0;
		}
		inst->fin = snd_zalloc(
			fin_samples * sizeof(float) * inst->channels,
			AFM_EXTN);
		if (!inst->fin) {
			M_LOGE("alloc fin buffer fail !\n");
			os_mutex_unlock(inst->lock);
			return -1;
		}
		inst->fin_samples = fin_samples;
	}

	if (fout_samples != inst->fout_samples) {
		if (inst->fout) {
			snd_free(inst->fout);
			inst->fout = NULL;
			inst->fout_samples = 0;
		}
		inst->fout = snd_zalloc(
			fout_samples * sizeof(float) * inst->channels,
			AFM_EXTN);
		if (!inst->fout) {
			M_LOGE("alloc fout buffer fail !\n");
			os_mutex_unlock(inst->lock);
			return -1;
		}
		inst->fout_samples = fout_samples;
	}
#endif

	out_buffer_size = fout_samples * channels * inst->sample_bytes;
	if (inst->out_buffer_size != out_buffer_size) {
		if (inst->out_buffer)
			snd_free(inst->out_buffer);

		inst->out_buffer = snd_zalloc(out_buffer_size, AFM_MAIN);
		if (!inst->out_buffer) {
			M_LOGE("alloc out buffer fail !\n");
			os_mutex_unlock(inst->lock);
			return -1;
		}
		inst->out_buffer_size = out_buffer_size;
	}

#ifdef SPEEX_RESAMPLE_ENABLE
	short_to_float_deinterleave(inst->fin,
		(short *)input, input_samples, channels, inst->fin_samples);

	output_samples = 0;

	for (i = 0; i < channels; i++) {
		ilen = (spx_uint32_t)input_samples;
		olen = (spx_uint32_t)inst->fout_samples;
		ret = speex_resampler_process_float(inst->speex_state, i,
			(const float *)(inst->fin + inst->fin_samples * i),
			&ilen,
			inst->fout + output_samples, &olen);
		if (ret != RESAMPLER_ERR_SUCCESS) {
			M_LOGE("resampler error %d\n", ret);
		}

		if (ilen < input_samples) {
			M_LOGW("input %d samples, consum %u samples\n",
				input_samples, ilen);
		}

		output_samples += olen;
	}

	float_to_short_interleave((short *)inst->out_buffer, inst->fout,
		output_samples / channels, channels);

	*output = inst->out_buffer;
	*output_len = output_samples * inst->sample_bytes;

#else
	short *input_ptr = (short *)input;
	short *output_ptr = (short *)inst->out_buffer;
	int sample_value;
	for (i = 0; i < input_samples; i++) {
		output_ptr[i * 2] = input_ptr[i];
		if (i == input_samples - 1)
			sample_value = input_ptr[i];
		else
			sample_value = (input_ptr[i] + input_ptr[i + 1]) / 2;
		output_ptr[i * 2 + 1] = (short)sample_value;
	}
	*output = inst->out_buffer;
	*output_len = input_samples * 2 * inst->sample_bytes;
#endif

	os_mutex_unlock(inst->lock);
	return 0;
}

int uvoice_resampler_update(void *resampler, int src_rate, int dst_rate, int channels,
	int sbits)
{
	resampler_inst_t *inst = (resampler_inst_t *)resampler;
	int out_coeff;
	int i;

	if (!inst) {
		M_LOGE("no resampler !\n");
		return -1;
	}

	os_mutex_lock(inst->lock, OS_WAIT_FOREVER);

	if (inst->channels == channels && inst->sbits == sbits &&
		inst->src_rate == src_rate && inst->dst_rate == dst_rate) {
		os_mutex_unlock(inst->lock);
		return 0;
	}

	if (inst->channels != channels)
		inst->channels = channels;

	if (inst->sbits != sbits) {
		inst->sbits = sbits;
		inst->sample_bytes = sbits >> 3;
	}

	if (inst->src_rate != src_rate ||
		inst->dst_rate != dst_rate) {
		inst->src_rate = src_rate;
		inst->dst_rate = dst_rate;
		out_coeff = (inst->dst_rate / inst->src_rate) + 1;
		if (inst->out_coeff != out_coeff) {
			inst->out_coeff = out_coeff;
			M_LOGD("update coeff %d\n", inst->out_coeff);
		}
#ifdef SPEEX_RESAMPLE_ENABLE
		M_LOGI("speex resampler update, %d->%d\n",
			inst->src_rate, inst->dst_rate);
		speex_resampler_set_rate(inst->speex_state,
			(spx_uint32_t)inst->src_rate,
			(spx_uint32_t)inst->dst_rate);
#endif
	}

	os_mutex_unlock(inst->lock);
	return 0;
}

int uvoice_resampler_create(void **resampler, int src_rate, int dst_rate,
	int channels, int sbits)
{
	resampler_inst_t *inst;
	int err = 0;
	int ret;

	if (!resampler)
		return -1;

	inst = snd_zalloc(sizeof(resampler_inst_t), AFM_EXTN);
	if (!inst) {
		M_LOGE("alloc resampler inst fail !\n");
		return -1;
	}

	inst->lock = os_mutex_new();

	inst->src_rate = src_rate;
	inst->dst_rate = dst_rate;
	inst->sbits = sbits;
	inst->channels = channels;
	inst->quality = 0;

	inst->out_coeff = (dst_rate / src_rate) + 1;
	inst->sample_bytes = sbits >> 3;

#ifdef SPEEX_RESAMPLE_ENABLE
	inst->speex_state = speex_resampler_init((spx_uint32_t)channels,
		(spx_uint32_t)src_rate,
		(spx_uint32_t)dst_rate,
		inst->quality, &err);
	if (!inst->speex_state) {
		M_LOGE("init speex resampler fail %d!\n", err);
		os_mutex_free(inst->lock);
		snd_free(inst);
		return -1;
	}

	speex_resampler_skip_zeros(inst->speex_state);
#endif

	*resampler = inst;
	M_LOGI("speex resampler create, %d->%d\n", inst->src_rate, inst->dst_rate);
	return 0;
}

int uvoice_resampler_release(void *resampler)
{
	resampler_inst_t *inst = (resampler_inst_t *)resampler;

	if (!inst) {
		M_LOGI("no resampler\n");
		return 0;
	}

	if (inst->fout)
		snd_free(inst->fout);

	if (inst->fin)
		snd_free(inst->fin);

	if (inst->out_buffer)
		snd_free(inst->out_buffer);

#ifdef SPEEX_RESAMPLE_ENABLE
	speex_resampler_destroy(inst->speex_state);
#endif

	os_mutex_free(inst->lock);
	snd_free(inst);

	M_LOGI("speex resampler release\n");
	return 0;
}

