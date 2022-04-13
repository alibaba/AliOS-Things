/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __UVOICE_WAVE_H__
#define __UVOICE_WAVE_H__


#define ID_RIFF		0x46464952 /* 'F' 'F' 'I' 'R' */
#define ID_WAVE		0x45564157 /* 'E' 'V' 'A' 'W' */
#define ID_FMT		0x20746d66 /* ' ' 't' 'm' 'f' */
#define ID_DATA		0x61746164 /* 'a' 't' 'a' 'd' */

#define WAVE_FMT_PCM			0x0001 /* linear pcm */
#define WAVE_FMT_IEEE_FLOAT		0x0003
#define WAVE_FMT_G711_ALAW		0x0006
#define WAVE_FMT_G711_ULAW		0x0007
#define WAVE_FMT_EXTENSIBLE		0xfffe


struct riff_wave_header {
	uint32_t riff_id;
	uint32_t riff_sz;
	uint32_t wave_id;
};

struct chunk_header {
	uint32_t id;
	uint32_t sz;
};

struct chunk_fmt {
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
};

struct wav_header {
	uint32_t riff_id;
	uint32_t riff_sz;
	uint32_t riff_fmt;
	uint32_t fmt_id;
	uint32_t fmt_sz;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint32_t data_id;
	uint32_t data_sz;
};

/*int wave_encoder_create(media_encoder_t *mencoder);
int wave_encoder_release(media_encoder_t *mencoder);
int wave_decoder_create(media_decoder_t *mdecoder);
int wave_decoder_release(media_decoder_t *mdecoder);*/


#endif /* __UVOICE_WAVE_H__ */

