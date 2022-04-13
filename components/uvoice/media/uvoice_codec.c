/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_config.h"
#include "uvoice_common.h"
#include "uvoice_play.h"
#include "uvoice_record.h"
#include "uvoice_codec.h"


#define MP3_DECODER_BUFF_SIZE	1200
#define AAC_DECODER_BUFF_SIZE	4096
#define M4A_DECODER_BUFF_SIZE	6144


static int mp3_decoder_create(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	mdecoder->input_size = MP3_DECODER_BUFF_SIZE;

#ifdef DECODER_MAD_ENABLE
	if (mdecoder->stere_enable)
		mdecoder->buffer_out_size = 1152 * sizeof(int16_t) * 2;
	else
		mdecoder->buffer_out_size = 1152 * sizeof(int16_t);
#else
	mdecoder->buffer_out_size = 1152 * sizeof(int16_t) * 2;
#endif

	mdecoder->buffer_out = snd_zalloc(
			mdecoder->buffer_out_size, AFM_EXTN);
	if (!mdecoder->buffer_out) {
		M_LOGE("alloc out buffer failed !\n");
		goto __exit;
	}

#ifdef DECODER_MAD_ENABLE
	if (mad_decoder_create(mdecoder)) {
		M_LOGE("create mad decoder failed !\n");
		goto __exit1;
	}
#elif (DECODER_PV_MP3_ENABLE == 1)
	if (pvmp3_decoder_create(mdecoder)) {
		M_LOGE("create pvmp3 decoder failed !\n");
		goto __exit1;
	}
#elif defined(DECODER_HELIX_MP3_ENABLE)
	if (helixmp3_decoder_create(mdecoder)) {
		M_LOGE("create helixmp3 decoder failed !\n");
		goto __exit1;
	}
#else
	M_LOGW("MP3 decoder not enable !\n");
	goto __exit1;
#endif

	return 0;

__exit1:
	mdecoder->buffer_out_size = 0;
	snd_free(mdecoder->buffer_out);
	mdecoder->buffer_out = NULL;
__exit:
	mdecoder->input_size = 0;
	return -1;
}

static int mp3_decoder_release(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

#ifdef DECODER_MAD_ENABLE
	mad_decoder_release(mdecoder);
#elif (DECODER_PV_MP3_ENABLE == 1)
	pvmp3_decoder_release(mdecoder);
#elif defined(DECODER_HELIX_MP3_ENABLE)
	helixmp3_decoder_release(mdecoder);
#endif

	snd_free(mdecoder->buffer_out);
	mdecoder->buffer_out = NULL;
	mdecoder->buffer_out_size = 0;
	mdecoder->input_size = 0;

	return 0;
}

static int aac_decoder_create(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	mdecoder->input_size = AAC_DECODER_BUFF_SIZE;

#ifndef DECODER_FAAD2_AAC_ENABLE
	mdecoder->buffer_out_size = 2048 * sizeof(int16_t);
	mdecoder->buffer_out = snd_zalloc(
			mdecoder->buffer_out_size, AFM_EXTN);
	if (!mdecoder->buffer_out) {
		M_LOGE("alloc out buffer failed !\n");
		mdecoder->input_size = 0;
		goto __exit;
	}
#endif

#ifdef DECODER_FAAD2_AAC_ENABLE
	if (faad2aac_decoder_create(mdecoder)) {
		M_LOGE("create faad2 aac decoder failed !\n");
		goto __exit;
	}
#elif defined(DECODER_HELIX_AAC_ENABLE)
	if (helixaac_decoder_create(mdecoder)) {
		M_LOGE("create helix aac decoder failed !\n");
		goto __exit1;
	}
#else
	M_LOGE("AAC decoder not enable !\n");
	goto __exit1;
#endif

	return 0;

__exit1:
	mdecoder->buffer_out_size = 0;
	snd_free(mdecoder->buffer_out);
	mdecoder->buffer_out = NULL;
__exit:
	mdecoder->input_size = 0;
	return -1;
}

static int aac_decoder_release(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

#ifdef DECODER_FAAD2_AAC_ENABLE
	faad2aac_decoder_release(mdecoder);
#elif defined(DECODER_HELIX_AAC_ENABLE)
	helixaac_decoder_release(mdecoder);
#endif

#ifndef DECODER_FAAD2_AAC_ENABLE
	snd_free(mdecoder->buffer_out);
	mdecoder->buffer_out = NULL;
	mdecoder->buffer_out_size = 0;
#endif
	mdecoder->input_size = 0;

	return 0;
}

static int m4a_decoder_create(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	mdecoder->input_size = M4A_DECODER_BUFF_SIZE;

#ifndef DECODER_FAAD2_M4A_ENABLE
	if (mdecoder->stere_enable)
		mdecoder->buffer_out_size = 1024 * sizeof(int16_t) * 2;
	else
		mdecoder->buffer_out_size = 1024 * sizeof(int16_t);

	mdecoder->buffer_out = snd_zalloc(
			mdecoder->buffer_out_size, AFM_EXTN);
	if (!mdecoder->buffer_out) {
		M_LOGE("alloc out buffer failed !\n");
		mdecoder->input_size = 0;
		goto __exit;
	}
#endif

#ifdef DECODER_FAAD2_M4A_ENABLE
	if (faad2m4a_decoder_create(mdecoder)) {
		M_LOGE("create faad2m4a decoder failed !\n");
		goto __exit;
	}
#elif defined(DECODER_HELIX_M4A_ENABLE)
	if (helixm4a_decoder_create(mdecoder)) {
		M_LOGE("create helixm4a decoder failed !\n");
		goto __exit1;
	}
#elif (DECODER_PV_M4A_ENABLE == 1)
	if (pvm4a_decoder_create(mdecoder)) {
		M_LOGE("create pvm4a decoder failed !\n");
		goto __exit1;
	}
    printf("func %s, line %d: mdecoder->action is 0x%p\n", __FUNCTION__, __LINE__, mdecoder->action);
#else
	M_LOGE("M4A decoder not enable !\n");
	goto __exit1;
#endif

	M_LOGD("M4A decoder create\n");
	return 0;

__exit1:
	mdecoder->buffer_out_size = 0;
	snd_free(mdecoder->buffer_out);
	mdecoder->buffer_out = NULL;
__exit:
	mdecoder->input_size = 0;
	return -1;
}

static int m4a_decoder_release(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

#ifdef DECODER_FAAD2_M4A_ENABLE
	faad2m4a_decoder_release(mdecoder);
#elif defined(DECODER_HELIX_M4A_ENABLE)
	helixm4a_decoder_release(mdecoder);
#elif (DECODER_PV_M4A_ENABLE == 1)
	pvm4a_decoder_release(mdecoder);
#endif

	snd_free(mdecoder->buffer_out);
#ifndef DECODER_FAAD2_M4A_ENABLE
	mdecoder->buffer_out = NULL;
	mdecoder->buffer_out_size = 0;
#endif
	mdecoder->input_size = 0;

	M_LOGD("M4A decoder release\n");
	return 0;
}

int media_decoder_reset(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	if (mdecoder->reset && mdecoder->reset(mdecoder))
		M_LOGW("reset decoder failed\n");

	mdecoder->unproc_size = 0;
	mdecoder->running = 0;

	return 0;
}

media_decoder_t *media_decoder_create(media_format_t format)
{
	media_decoder_t *mdecoder = snd_zalloc(
			sizeof(media_decoder_t), AFM_EXTN);
	if (!mdecoder) {
		M_LOGE("alloc mdecoder failed !\n");
		return NULL;
	}

	mdecoder->stere_enable = PLAYER_STERE_ENABLE;

	switch (format) {
	case MEDIA_FMT_MP3:
		if (mp3_decoder_create(mdecoder)) {
			M_LOGE("create mp3 decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
	case MEDIA_FMT_WAV:
		if (wave_decoder_create(mdecoder)) {
			M_LOGE("create wav decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
	case MEDIA_FMT_AAC:
		if (aac_decoder_create(mdecoder)) {
			M_LOGE("create aac decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
	case MEDIA_FMT_M4A:
		if (m4a_decoder_create(mdecoder)) {
			M_LOGE("create m4a decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
	case MEDIA_FMT_SPX:
#ifdef DECODER_SPEEX_ENABLE
		if (spx_decoder_create(mdecoder)) {
			M_LOGE("create spx decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
#else
		M_LOGE("spx decoder not enable !\n");
		snd_free(mdecoder);
		return NULL;
#endif
	case MEDIA_FMT_OGG:
#ifdef DECODER_OGG_ENABLE
		if (ogg_decoder_create(mdecoder)) {
			M_LOGE("create ogg decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
#else
		M_LOGE("ogg decoder not enable !\n");
		snd_free(mdecoder);
		return NULL;
#endif
	case MEDIA_FMT_WMA:
#ifdef DECODER_WMA_ENABLE
		if (wma_decoder_create(mdecoder)) {
			M_LOGE("create wma decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
#else
		M_LOGE("wma decoder not enable!\n");
		snd_free(mdecoder);
		return NULL;
#endif
	case MEDIA_FMT_OPS:
#ifdef DECODER_OPUS_ENABLE
		if (opus_decode_create(mdecoder)) {
			M_LOGE("create opus decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
#else
		M_LOGE("opus decoder not enable !\n");
		snd_free(mdecoder);
		return NULL;
#endif
	case MEDIA_FMT_FLAC:
#ifdef DECODER_FLAC_ENABLE
		if (flac_decoder_create(mdecoder)) {
			M_LOGE("create flac decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
#else
		M_LOGE("flac decoder not enable !\n");
		snd_free(mdecoder);
		return NULL;
#endif
	case MEDIA_FMT_AMR:
#ifdef DECODER_AMR_ENABLE
		if (amr_decoder_create(mdecoder)) {
			M_LOGE("create amr decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
#else
		M_LOGE("amr decoder not enable !\n");
		snd_free(mdecoder);
		return NULL;
#endif
	case MEDIA_FMT_AMRWB:
#ifdef DECODER_AMRWB_ENABLE
		if (amrwb_decoder_create(mdecoder)) {
			M_LOGE("create amrwb decoder failed !\n");
			snd_free(mdecoder);
			return NULL;
		}
		mdecoder->initialized = 1;
		break;
#else
		M_LOGE("amrwb decoder not enable !\n");
		snd_free(mdecoder);
		return NULL;
#endif
	case MEDIA_FMT_PCM:
		mdecoder->input_size = 2048;
		break;
	default:
		M_LOGE("unknown format %d\n", format);
		snd_free(mdecoder);
		return NULL;
	}

	mdecoder->format = format;
	return mdecoder;
}

int media_decoder_release(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	switch (mdecoder->format) {
	case MEDIA_FMT_MP3:
		mp3_decoder_release(mdecoder);
		break;
	case MEDIA_FMT_WAV:
		wave_decoder_release(mdecoder);
		break;
	case MEDIA_FMT_AAC:
		aac_decoder_release(mdecoder);
		break;
	case MEDIA_FMT_M4A:
		m4a_decoder_release(mdecoder);
		break;
	case MEDIA_FMT_SPX:
#ifdef DECODER_SPEEX_ENABLE
		spx_decoder_release(mdecoder);
#endif
		break;

	case MEDIA_FMT_OGG:
#ifdef DECODER_OGG_ENABLE
		ogg_decoder_release(mdecoder);
#endif
		break;

	case MEDIA_FMT_WMA:
#ifdef DECODER_WMA_ENABLE
		wma_decoder_release(mdecoder);
#endif
		break;

	case MEDIA_FMT_OPS:
#ifdef DECODER_OPUS_ENABLE
		opus_decode_release(mdecoder);
#endif
		break;

	case MEDIA_FMT_FLAC:
#ifdef DECODER_FLAC_ENABLE
		flac_decoder_release(mdecoder);
#endif
		break;

	case MEDIA_FMT_AMR:
#ifdef DECODER_AMR_ENABLE
		amr_decoder_release(mdecoder);
#endif
		break;

	case MEDIA_FMT_AMRWB:
#ifdef DECODER_AMRWB_ENABLE
		amrwb_decoder_release(mdecoder);
#endif
		break;
	default:
		break;
	}

	snd_free(mdecoder);
	return 0;
}

int media_encoder_header_gen(media_encoder_t *mencoder,
		media_pcminfo_t *pcminfo)
{
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	if (!pcminfo) {
		M_LOGE("pcminfo null !\n");
		return -1;
	}

	if (mencoder->header_gen &&
			mencoder->header_gen(mencoder, pcminfo)) {
		M_LOGE("generate header failed !\n");
		return -1;
	}

	return 0;
}

media_encoder_t *media_encoder_create(media_format_t format)
{
	media_encoder_t *mencoder = snd_zalloc(
			sizeof(media_encoder_t), AFM_EXTN);
	if (!mencoder) {
		M_LOGE("alloc media encoder failed !\n");
		return NULL;
	}

	switch (format) {
	case MEDIA_FMT_WAV:
		if (wave_encoder_create(mencoder)) {
			M_LOGE("create wave encoder failed !\n");
			snd_free(mencoder);
			return NULL;
		}
		mencoder->initialized = 1;
		break;

	case MEDIA_FMT_SPX:
#ifdef ENCODER_SPEEX_ENABLE
		if (spx_encoder_create(mencoder)) {
			M_LOGE("create speex encoder failed !\n");
			snd_free(mencoder);
			return NULL;
		}
		mencoder->initialized = 1;
		break;
#else
		M_LOGE("speex encoder not enable !\n");
		snd_free(mencoder);
		return NULL;
#endif

    case MEDIA_FMT_OPS:
#ifdef ENCODER_OPUS_ENABLE
		if (opus_encode_create(mencoder)) {
			M_LOGE("create opus encoder failed !\n");
			snd_free(mencoder);
			return NULL;
		}
		mencoder->initialized = 1;
		break;
#else
		M_LOGE("opus encoder not enable !\n");
		snd_free(mencoder);
		return NULL;
#endif

	case MEDIA_FMT_AMR:
#ifdef ENCODER_AMR_ENABLE
		if (amr_encoder_create(mencoder)) {
			M_LOGE("create amr encoder failed !\n");
			snd_free(mencoder);
			return NULL;
		}
		mencoder->initialized = 1;
		break;
#else
		M_LOGE("amr encoder not enable !\n");
		snd_free(mencoder);
		return NULL;
#endif

	case MEDIA_FMT_AMRWB:
#ifdef ENCODER_AMRWB_ENABLE
		if (amrwb_encoder_create(mencoder)) {
			M_LOGE("create amrwb encoder failed !\n");
			snd_free(mencoder);
			return NULL;
		}
		mencoder->initialized = 1;
		break;
#else
		M_LOGE("amrwb encoder not enable !\n");
		snd_free(mencoder);
		return NULL;
#endif

	case MEDIA_FMT_AAC:
#ifdef ENCODER_FAAC_ENABLE
		if (faac_encoder_create(mencoder)) {
			M_LOGE("create faac encoder failed !\n");
			snd_free(mencoder);
			return NULL;
		}
		mencoder->initialized = 1;
		break;
#else
		M_LOGE("faac encoder not enable !\n");
		snd_free(mencoder);
		return NULL;
#endif

	case MEDIA_FMT_MP3:
#ifdef ENCODER_LAME_ENABLE
		if (lame_encoder_create(mencoder)) {
			M_LOGE("create lame encoder failed !\n");
			snd_free(mencoder->buffer_out);
			snd_free(mencoder);
			return NULL;
		}
		mencoder->initialized = 1;
		break;
#else
		M_LOGE("MP3 encoder not enable !\n");
		snd_free(mencoder);
		return NULL;
#endif
	default:
		break;
	}

	mencoder->format = format;

	return mencoder;
}

int media_encoder_release(media_encoder_t *mencoder)
{
	if (!mencoder) {
		M_LOGW("mencoder null\n");
		return -1;
	}

	switch (mencoder->format) {
	case MEDIA_FMT_WAV:
		wave_encoder_release(mencoder);
		mencoder->initialized = 0;
		break;

	case MEDIA_FMT_SPX:
#ifdef ENCODER_SPEEX_ENABLE
		spx_encoder_release(mencoder);
		mencoder->initialized = 0;
#endif
		break;

	case MEDIA_FMT_MP3:
#ifdef ENCODER_LAME_ENABLE
		lame_encoder_release(mencoder);
		mencoder->initialized = 0;
#endif
		break;

	case MEDIA_FMT_AAC:
#ifdef ENCODER_FAAC_ENABLE
		faac_encoder_release(mencoder);
		mencoder->initialized = 0;
#endif
		break;

    case MEDIA_FMT_AMR:
#ifdef ENCODER_AMR_ENABLE
		amr_encoder_release(mencoder);
		mencoder->initialized = 0;
#endif
		break;

    case MEDIA_FMT_AMRWB:
#ifdef ENCODER_AMRWB_ENABLE
		amrwb_encoder_release(mencoder);
		mencoder->initialized = 0;
#endif
		break;

    case MEDIA_FMT_OPS:
#ifdef ENCODER_OPUS_ENABLE
		opus_encode_release(mencoder);
		mencoder->initialized = 0;
#endif
		break;

	default:
		break;
	}

	snd_free(mencoder);
	return 0;
}

