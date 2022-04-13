/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __AUDIO_COMMON_H__
#define __AUDIO_COMMON_H__


enum {
	PARAM_KEY_INVALID = 0,
	PARAM_KEY_NOISE_SUPPRESSION,
	PARAM_KEY_ECHO_CANCELLATION,
	PARAM_KEY_AUTO_GAIN_CONTROL,
	PARAM_KEY_VOICE_ACTIVE_DETECT,
	PARAM_KEY_EFFECT_EQUALIZER,
	PARAM_KEY_OUT_PCM_DUMP,
};

#define PARAM_VAL_ENABLE	1
#define PARAM_VAL_DISABLE	0

static inline bool pcm_rate_valid(int rate)
{
	switch (rate) {
	case 8000:
	case 12000:
	case 16000:
	case 11025:
	case 22050:
	case 24000:
	case 32000:
	case 44100:
	case 48000:
	case 96000:
	case 192000:
		return true;
	}
	return false;
}

static inline bool pcm_channel_valid(int channels)
{
	if (channels > 0 && channels <= 8)
		return true;
	return false;
}

static inline bool pcm_bits_valid(int bits)
{
	if (bits == 8 || bits == 16 || bits == 24 || bits == 32)
		return true;
	return false;
}

enum pcm_format bits_to_pcm_format(int bits);
#ifndef UVOICE_ON_XR871
int pcm_format_to_bits(enum pcm_format format);
#endif

int pcm_to_decibel(struct pcm_config *config, uint8_t *buffer, int nbytes);

static inline int period_samples_to_bytes(struct pcm_config *config)
{
	return (config->period_size * config->channels *
		(pcm_format_to_bits(config->format) >> 3));
}


#endif /* __AUDIO_COMMON_H__ */
