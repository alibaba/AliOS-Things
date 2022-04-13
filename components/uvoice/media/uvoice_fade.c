/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdlib.h>
#include <stdint.h>


int fade_out_proc_8bit(uint8_t *buffer, int nbytes,
	int *pos, int scope, int channels)
{
	char *ptr = (char *)buffer;
	int sample_pos = *pos;
	int samples;
	int sample_index;
	int channel_index;
	double sample_val;

	samples = nbytes / channels;
	for (sample_index = 0; sample_index < samples; sample_index++) {
		for (channel_index = 0; channel_index < channels;
			channel_index++) { 
			sample_val = (double)*ptr;
			sample_val *= (double)sample_pos;
			sample_val /= (double)scope;
			*ptr = (uint8_t)sample_val;
			ptr++;
		}

		if (sample_pos > 0)
			sample_pos--;
	}

	*pos = sample_pos;
	return 0;
}

int fade_in_proc_8bit(uint8_t *buffer, int nbytes,
	int *pos, int scope, int channels)
{
	char *ptr = (char *)buffer;
	int sample_pos = *pos;
	int samples;
	int sample_index;
	int channel_index;
	double sample_val;

	if (sample_pos >= scope)
		goto __exit;

	samples = nbytes / channels;
	for (sample_index = 0; sample_index < samples; sample_index++) {
		for (channel_index = 0; channel_index < channels;
			channel_index++) { 
			sample_val = (double)*ptr;
			sample_val *= (double)sample_pos;
			sample_val /= (double)scope;
			*ptr = (uint8_t)sample_val;
			ptr++;
		}

		if (sample_pos < scope)
			sample_pos++;

		if (sample_pos >= scope)
			break;
	}

	*pos = sample_pos;

__exit:
	return 0;
}

int fade_out_proc_16bit(uint8_t *buffer, int nbytes,
	int *pos, int scope, int channels)
{
	short *ptr = (short *)buffer;
	int sample_pos = *pos;
	int samples;
	int sample_index;
	int channel_index;
	double sample_val;

	samples = nbytes / (channels * 2);
	for (sample_index = 0; sample_index < samples; sample_index++) {
		for (channel_index = 0; channel_index < channels;
			channel_index++) { 
			sample_val = (double)*ptr;
			sample_val *= (double)sample_pos;
			sample_val /= (double)scope;
			*ptr = (short)sample_val;
			ptr++;
		}

		if (sample_pos > 0)
			sample_pos--;
	}

	*pos = sample_pos;
	return 0;
}

int fade_in_proc_16bit(uint8_t *buffer, int nbytes,
	int *pos, int scope, int channels)
{
	short *ptr = (short *)buffer;
	int sample_pos = *pos;
	int samples;
	int sample_index;
	int channel_index;
	double sample_val;

	if (sample_pos >= scope)
		goto __exit;

	samples = nbytes / (channels * 2);
	for (sample_index = 0; sample_index < samples; sample_index++) {
		for (channel_index = 0; channel_index < channels;
			channel_index++) { 
			sample_val = (double)*ptr;
			sample_val *= (double)sample_pos;
			sample_val /= (double)scope;
			*ptr = (short)sample_val;
			ptr++;
		}

		if (sample_pos < scope)
			sample_pos++;

		if (sample_pos >= scope)
			break;
	}

	*pos = sample_pos;

__exit:
	return 0;
}

