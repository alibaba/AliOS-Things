/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_CODEC_H__
#define __UVOICE_CODEC_H__


int wave_encoder_create(media_encoder_t *mencoder);
int wave_encoder_release(media_encoder_t *mencoder);
int wave_decoder_create(media_decoder_t *mdecoder);
int wave_decoder_release(media_decoder_t *mdecoder);

int mad_decoder_create(media_decoder_t *mdecoder);
int mad_decoder_release(media_decoder_t *mdecoder);

int pvmp3_decoder_create(media_decoder_t *mdecoder);
int pvmp3_decoder_release(media_decoder_t *mdecoder);

int helixmp3_decoder_create(media_decoder_t *mdecoder);
int helixmp3_decoder_release(media_decoder_t *mdecoder);

int helixaac_decoder_create(media_decoder_t *mdecoder);
int helixaac_decoder_release(media_decoder_t *mdecoder);

int ogg_decoder_create(media_decoder_t *mdecoder);
int ogg_decoder_release(media_decoder_t *mdecoder);

int opus_encoder_create(media_encoder_t *mencoder);
int opus_encoder_release(media_encoder_t *mencoder);
int opus_decoder_create(media_decoder_t *mdecoder);
int opus_decoder_release(media_decoder_t *mdecoder);

int spx_encoder_create(media_encoder_t *mencoder);
int spx_encoder_release(media_encoder_t *mencoder);
int spx_decoder_create(media_decoder_t *mdecoder);
int spx_decoder_release(media_decoder_t *mdecoder);

int flac_decoder_create(media_decoder_t *mdecoder);
int flac_decoder_release(media_decoder_t *mdecoder);

int amr_encoder_create(media_encoder_t *mencoder);
int amr_encoder_release(media_encoder_t *mencoder);
int amr_decoder_create(media_decoder_t *mdecoder);
int amr_decoder_release(media_decoder_t *mdecoder);

int amrwb_encoder_create(media_encoder_t *mencoder);
int amrwb_encoder_release(media_encoder_t *mencoder);
int amrwb_decoder_create(media_decoder_t *mdecoder);
int amrwb_decoder_release(media_decoder_t *mdecoder);


#endif /* __UVOICE_CODEC_H__ */
