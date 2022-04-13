/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "uvoice_types.h"
#include "uvoice_player.h"

#include "uvoice_os.h"
#include "uvoice_common.h"
#include "uvoice_play.h"

#include "opensource/libogg/include/ogg/ogg.h"
#include "opensource/libvorbis/include/vorbis/codec.h"


#define OGG_DECODE_INPUT_SIZE			4096

#define OGG_DECODE_OUTPUT_SAMPLES_MAX	1024

enum {
	OGG_FLOW_FIND_IDENTIFY_HEADER = 0,
	OGG_FLOW_FIND_COMMENT_AND_CODEBOOK,
	OGG_FLOW_DECODE_PACKET,
};

typedef struct {
	uint8_t *input_buffer;
	int flow;
	ogg_sync_state state;
	ogg_page page;
	ogg_packet packet;
	ogg_stream_state stream;
	vorbis_info vb_info;
	vorbis_comment vb_comment;
	vorbis_dsp_state vb_dsp_state;
	vorbis_block vb_block;
} ogg_decoder_t;


static int ogg_decoder_reset(void *priv)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	ogg_decoder_t *ogg;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	ogg = mdecoder->decoder;
	if (!ogg) {
		M_LOGE("ogg decoder null !\n");
		return -1;
	}

	ogg_sync_clear(&ogg->state);
	ogg_sync_reset(&ogg->state);
	ogg->flow = OGG_FLOW_FIND_IDENTIFY_HEADER;

	M_LOGD("ogg reset\n");
	return 0;
}

static int ogg_decoder_process(void *priv, const uint8_t *buffer, int nbytes)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	ogg_decoder_t *ogg;
	char *ogg_buffer;
	int serial;
	int out_len;
	static int index = 0;
	static bool stream_end = false;
	int ret;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	ogg = mdecoder->decoder;
	if (!ogg) {
		M_LOGE("ogg null !\n");
		return -1;
	}

	ogg_buffer = ogg_sync_buffer(&ogg->state, nbytes);
	if (!ogg_buffer) {
		M_LOGE("sync buffer failed !\n");
		return -1;
	}

	memcpy(ogg_buffer, buffer, nbytes);

	ret = ogg_sync_wrote(&ogg->state, nbytes);
	if (ret) {
		M_LOGE("ogg sync buffer failed %d!\n", ret);
		return -1;
	}

	if (ogg->flow == OGG_FLOW_FIND_COMMENT_AND_CODEBOOK) {
		//M_LOGD("find comment and codebook headers index %d\n", index);
		goto __find_comment_and_codebook;
	} else if (ogg->flow == OGG_FLOW_DECODE_PACKET) {
		//M_LOGD("find and decode packet\n");
		goto __find_and_decode_packet;
	}

	ret = ogg_sync_pageout(&ogg->state, &ogg->page);
	if (ret != 1) {
		if (nbytes != mdecoder->input_size) {
			M_LOGW("tail buffer %d discard\n", nbytes);
			goto __exit;
		}
		M_LOGE("input not ogg stream !\n");
		return -1;
	}

	serial = ogg_page_serialno(&ogg->page);

	if (ogg_page_bos(&ogg->page)) {
		//M_LOGD("init ogg stream with serial %d\n", serial);
		ret = ogg_stream_init(&ogg->stream, serial);
		if (ret) {
			M_LOGE("ogg stream init failed %d!\n", ret);
			return -1;
		}
	} else {
		M_LOGE("not ogg stream bos !\n");
		return -1;
	}

	vorbis_info_init(&ogg->vb_info);
	vorbis_comment_init(&ogg->vb_comment);

	ret = ogg_stream_pagein(&ogg->stream, &ogg->page);
	if (ret < 0) {
		M_LOGE("read first page failed %d!\n", ret);
		goto __exit_err;
	}

	ret = ogg_stream_packetout(&ogg->stream, &ogg->packet);
	if (ret != 1){
		M_LOGE("read initial header packet failed %d!\n", ret);
		goto __exit_err;
	}

	ret = vorbis_synthesis_headerin(&ogg->vb_info,
		&ogg->vb_comment, &ogg->packet);
	if (ret < 0) {
		M_LOGE("ogg stream not contain vorbis %d!\n", ret);
		goto __exit_err;
	}

	index = 0;
	ogg->flow = OGG_FLOW_FIND_COMMENT_AND_CODEBOOK;

__find_comment_and_codebook:
	while (index < 2) {
		while (index < 2) {
			ret = ogg_sync_pageout(&ogg->state, &ogg->page);
			if (ret == 0) {
				//M_LOGD("need more input\n");
				break;
			}
			if (ret == 1) {
				ogg_stream_pagein(&ogg->stream, &ogg->page);
				while (index < 2) {
					ret = ogg_stream_packetout(&ogg->stream, &ogg->packet);
					if (ret == 0)
						break;
					if (ret < 0) {
						M_LOGE("second header corrupt %d!\n", ret);
						goto __exit_err;
					}

					ret = vorbis_synthesis_headerin(&ogg->vb_info,
						&ogg->vb_comment, &ogg->packet);
					if (ret < 0) {
						M_LOGE("secondary header corrupt %d!\n", ret);
						goto __exit_err;
					}
					index++;
				}
			}
		}
		//M_LOGD("need more input, fill %d returned %d storage %d\n",
		//	ogg->state.fill, ogg->state.returned, ogg->state.storage);
		goto __exit;
    }

	//M_LOGD("rate %d channels %d\n",
	//	ogg->vb_info.rate, ogg->vb_info.channels);

	if (!mdecoder->running) {
		media_pcminfo_t pcm_info;
		memset(&pcm_info, 0, sizeof(pcm_info));
		pcm_info.rate = ogg->vb_info.rate;
		pcm_info.frames = 1024;
		if (mdecoder->stere_enable)
			pcm_info.channels = ogg->vb_info.channels;
		else
			pcm_info.channels = 1;

		mdecoder->buffer_out_size = OGG_DECODE_OUTPUT_SAMPLES_MAX * pcm_info.channels * 2;
		M_LOGD("alloc out buffer %u\n", mdecoder->buffer_out_size);
		mdecoder->buffer_out = snd_zalloc(mdecoder->buffer_out_size, AFM_EXTN);
		if (!mdecoder->buffer_out) {
			M_LOGE("alloc mdecoder failed !\n");
			goto __exit_err;
		}

		pcm_info.bits = 16;
		mdecoder->message(mdecoder->priv,
			PLAYER_MSG_PCM_INFO, &pcm_info);
		mdecoder->running = 1;
	}

	ogg->flow = OGG_FLOW_DECODE_PACKET;
	ret = vorbis_synthesis_init(&ogg->vb_dsp_state, &ogg->vb_info);
	if (ret != 0) {
		M_LOGW("vorbis synthesis init failed %d!\n", ret);
		goto __exit2;
	}

	vorbis_block_init(&ogg->vb_dsp_state, &ogg->vb_block);

	stream_end = false;
	//M_LOGD("decode packet, returned %d fill %d\n",
	//	ogg->state.fill, ogg->state.returned);

__find_and_decode_packet:
	/* The rest is just a straight decode loop until end of stream */
	while (!stream_end) {
		while (!stream_end) {
			ret = ogg_sync_pageout(&ogg->state, &ogg->page);
			if (ret == 0) {
				//M_LOGD("require more input\n");
				break;
			}
			if (ret < 0) { /* missing or corrupt data at this page position */
				M_LOGW("corrupt or missing data in bitstream, continue\n");
			} else {
				ogg_stream_pagein(&ogg->stream, &ogg->page);
				/* can safely ignore errors at this point */
				while (1) {
					ret = ogg_stream_packetout(&ogg->stream, &ogg->packet);
					if (ret == 0) {
						//M_LOGD("require more data\n");
						break;
					}
					if (ret < 0) {
						/* missing or corrupt data at this page position */
						/* no reason to complain; already complained above */
						M_LOGW("corrupt or missing data in bitstream, continue\n");
					} else {
						/* we have a packet.  Decode it */
						float **pcm;
						int samples;

						if (vorbis_synthesis(&ogg->vb_block, &ogg->packet) == 0)
							vorbis_synthesis_blockin(&ogg->vb_dsp_state, &ogg->vb_block);
						/* pcm is a multichannel float vector.  In stereo, for
						 * example, pcm[0] is left, and pcm[1] is right.  samples is
						 * the size of each channel.  Convert the float values
						 * (-1.<=range<=1.) to whatever PCM format and write it out
						 */

						while ((samples = vorbis_synthesis_pcmout(&ogg->vb_dsp_state,
								&pcm)) > 0) {
							int i, j;
							int clip = 0;
							int bout = samples <= OGG_DECODE_OUTPUT_SAMPLES_MAX ?
								samples : OGG_DECODE_OUTPUT_SAMPLES_MAX;
							int channels = mdecoder->stere_enable ? ogg->vb_info.channels : 1;
							ogg_int16_t *convbuffer = (ogg_int16_t *)mdecoder->buffer_out;

							/* convert floats to 16 bit signed ints (host order) and
							 * interleave
							 */
							for (i = 0; i < channels; i++) {
								ogg_int16_t *ptr = convbuffer + i;
								float *mono = pcm[i];
								for (j = 0; j < bout; j++) {
									#if 1
									int val = floor(mono[j] * 32767.f + .5f);
									#else /* optional dither */
									int val=mono[j]*32767.f+drand48()-0.5f;
									#endif
									/* might as well guard against clipping */
									if (val > 32767) {
										val = 32767;
										clip = 1;
									}
									if (val < -32768){
										val = -32768;
										clip = 1;
									}
									*ptr = val;
									ptr += channels;
								}
							}

							if (clip)
								M_LOGW("clipping in frame %ld\n",
									(long)(ogg->vb_dsp_state.sequence));

							out_len = bout * channels * 2;

							//M_LOGD("bout %d out_len %d channels %d\n",
							//	bout, out_len, ogg->vb_info.channels);
							if (out_len > mdecoder->buffer_out_size) {
								M_LOGE("output size %d overrange !\n", out_len);
								goto __exit_err;
							}

							if (mdecoder->output(mdecoder->priv, mdecoder->buffer_out, out_len)) {
								M_LOGE("output failed !\n");
								goto __exit_err;
							}

							/* tell libvorbis how many samples we actually consumed */
							vorbis_synthesis_read(&ogg->vb_dsp_state, bout); 
						}
					}
				}

				if (ogg_page_eos(&ogg->page)) {
					M_LOGD("stream end\n");
					stream_end = true;
				}
			}
		}

		if (!stream_end) {
	  		if (nbytes != mdecoder->input_size) {
				M_LOGD("tail buffer %d\n", nbytes);
				stream_end = true;
				continue;
	  		}
			//M_LOGD("require more input, fill %d returned %d storage %d\n",
			//	ogg->state.fill, ogg->state.returned, ogg->state.storage);

			goto __exit;
		}
	}

	ogg->flow = OGG_FLOW_FIND_IDENTIFY_HEADER;

	vorbis_block_clear(&ogg->vb_block);
	vorbis_dsp_clear(&ogg->vb_dsp_state);

__exit2:
	ogg_stream_clear(&ogg->stream);
	vorbis_comment_clear(&ogg->vb_comment);
	vorbis_info_clear(&ogg->vb_info);

__exit:
	return 0;

__exit_err:
	ogg_stream_clear(&ogg->state);
	vorbis_comment_clear(&ogg->vb_comment);
	vorbis_info_clear(&ogg->vb_info);
	return -1;
}

static int ogg_decoder_action(void *priv, player_action_t action, void *arg)
{
	return 0;
}

int ogg_decoder_create(media_decoder_t *mdecoder)
{
	ogg_decoder_t *ogg;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	ogg = snd_zalloc(sizeof(ogg_decoder_t), AFM_MAIN);
	if (!ogg) {
		M_LOGE("alloc ogg decoder failed !\n");
		return -1;
	}

	ogg_sync_init(&ogg->state);

	ogg->flow = OGG_FLOW_FIND_IDENTIFY_HEADER;
	mdecoder->input_size = OGG_DECODE_INPUT_SIZE;

	mdecoder->decode = ogg_decoder_process;
	mdecoder->action = ogg_decoder_action;
    mdecoder->reset = ogg_decoder_reset;
    mdecoder->decoder = ogg;

	M_LOGD("ogg create\n");
	return 0;
}

int ogg_decoder_release(media_decoder_t *mdecoder)
{
	ogg_decoder_t *ogg;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	if (mdecoder->buffer_out) {
		snd_free(mdecoder->buffer_out);
		mdecoder->buffer_out = NULL;
		mdecoder->buffer_out_size = 0;
	}

    ogg = mdecoder->decoder;
	if (!ogg) {
		M_LOGE("ogg null !\n");
		return -1;
	}

	ogg_sync_clear(&ogg->state);

	snd_free(ogg);
	M_LOGD("ogg release\n");
	return 0;
}

