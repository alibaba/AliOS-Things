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

#include "pvmp4audiodecoder_api.h"

#define BOX_TYPE_LEN    4
#define DECODER_PREPARED 2
#define ASC_BUFF_LEN    12

#define MP4_ES_DESC_TAG                0x03
#define MP4_DEC_CFG_DESC_TAG           0x04
#define MP4_DEC_SPEC_DESC_TAG          0x05

#define MP4_DECODE_NEXT       0
#define MP4_DECODE_STOP       1
#define MP4_DECODE_END        2

typedef struct {
    uint32_t box_size;
    char box_type[BOX_TYPE_LEN];
}box_header_t;

typedef struct {
    char major_brand[BOX_TYPE_LEN];
    int minor_version;
    char compatible_brands[12];
}box_ftyp_t;

typedef struct {
    uint32_t box_size;
    char box_type[BOX_TYPE_LEN];
    uint8_t version;
    uint8_t flags[3];
    uint32_t creation_time;
    uint32_t modification_time;
    uint32_t time_scale;
    uint32_t duration;
    uint32_t rate;
    uint8_t reserved[10];
    uint8_t matrix[36];
    uint32_t preview_time;
    uint32_t post_time;
    uint32_t selection_time;
    uint32_t selection_duration;
    uint32_t current_time;
    uint32_t next_track_id;
}box_mvhd_t;

typedef struct {
    uint8_t version;
    uint8_t flag[3];
    uint32_t entry_num;
}box_stco_t;

typedef struct {
    uint8_t version;
    uint8_t flag[3];
    uint32_t entry_num;
}box_stsc_t;

typedef struct {
    uint32_t chunk_id;
    uint32_t samples;
    uint32_t id;
}entry_info_t;

typedef struct {
    uint8_t version;
    uint8_t flag[3];
    uint32_t sample_size;
    uint32_t sample_count;
}box_stsz_t;

typedef struct {
    uint32_t sample_len;
    uint32_t sample_off;
}sample_info_t;

typedef struct {
    char decoder_status;
    char decoder_find; /* bit 0 esds, bit 1  stsz, bit 2 mdat*/
    char seek_head;
    char resv;

    uint32_t  audio_spec_config_len;
    uint8_t audio_spec_config_info[ASC_BUFF_LEN];
    uint32_t sample_count;
    uint32_t *sample_size_info;
    uint32_t sample_index;
    uint32_t unproc_size;

    uint32_t sample_rate;
    uint32_t frame_length;
    int channels;
}m4a_decoder_t;

static tPVMP4AudioDecoderExternal g_dec_ext;
static void *dec_mem = NULL;
static uint32_t dec_mem_len = 0;
static media_decoder_t *g_aac_mdecoder = NULL;

static int aac_dec_init(int channels)
{
    int status = 0;

    if (dec_mem == NULL) {
        dec_mem_len = PVMP4AudioDecoderGetMemRequirements();
        dec_mem = malloc(dec_mem_len);
        if (0 == dec_mem) {
            return -1;
        }
        memset(dec_mem, 0, dec_mem_len);
    }
    else {
        memset(dec_mem, 0, dec_mem_len);
    }

    memset(&g_dec_ext, 0, sizeof(tPVMP4AudioDecoderExternal));

    g_dec_ext.inputBufferCurrentLength = 0;
    g_dec_ext.remainderBits = 0;
    g_dec_ext.inputBufferMaxLength = PVMP4AUDIODECODER_INBUFSIZE;
    g_dec_ext.outputFormat = OUTPUTFORMAT_16PCM_INTERLEAVED;
    g_dec_ext.desiredChannels = (int)channels;
    g_dec_ext.aacPlusEnabled = TRUE;

    status = PVMP4AudioDecoderInitLibrary(&g_dec_ext, dec_mem);
    return status;
}

static int aac_dec_deinit()
{
    if (NULL != dec_mem) {
        snd_free(dec_mem);
        dec_mem = NULL;
    }

    dec_mem_len = 0;

    return 0;
}

static int aac_decoder_config(const uint8_t *in_buffer, uint32_t in_size, int *samplerate, int *channels, int *framelength)
{
    int status = 0;

    g_dec_ext.pInputBuffer = in_buffer;
    g_dec_ext.inputBufferCurrentLength = in_size;
    g_dec_ext.pOutputBuffer = NULL;

    status = PVMP4AudioDecoderConfig(&g_dec_ext, dec_mem);

    *samplerate = g_dec_ext.samplingRate;
    *channels = g_dec_ext.encodedChannels;

    aac_dec_init(*channels);
    g_dec_ext.pInputBuffer = in_buffer;
    g_dec_ext.inputBufferCurrentLength = in_size;
    g_dec_ext.pOutputBuffer = NULL;

    status = PVMP4AudioDecoderConfig(&g_dec_ext, dec_mem);

    *framelength = g_dec_ext.frameLength;

    return status;
}

static int aac_decoder_frame(const uint8_t *in_buffer, uint32_t *in_size, uint8_t *out_buffer, uint32_t *out_size)
{
    int status = 0;

    g_dec_ext.pInputBuffer = in_buffer;
    g_dec_ext.pOutputBuffer = out_buffer;

    g_dec_ext.inputBufferCurrentLength = *in_size;
    g_dec_ext.inputBufferUsedLength = 0;

    status = PVMP4AudioDecodeFrame(&g_dec_ext, dec_mem);

    if (MP4AUDEC_SUCCESS == status || SUCCESS == status) {
        *in_size -= g_dec_ext.inputBufferUsedLength;
        *out_size = g_dec_ext.frameLength * g_dec_ext.desiredChannels;

        return status;
    } else if (MP4AUDEC_INCOMPLETE_FRAME == status) {

    } else {
        *in_size = 0;
    }

    return status;
}

static int find_box_head(const char *box_type, const uint8_t *in_buf, uint32_t size)
{
    char *f;
    uint32_t i = 0;
    int flag = 1;

    if (size < strlen(box_type)) {
        return -1;
    }

    do {
        f = (char *)in_buf + i;
        i++;
        flag = memcmp(box_type, f, strlen(box_type));
    } while ((i < size) && (flag));

    if (flag != 0) {
        return -1;
    }

    return i - 1 + strlen(box_type);
}

static int m4aac_get_sample_size_info(m4a_decoder_t *m4adec, const uint8_t *buffer, uint32_t buff_size)
{
    int i = 0;
    int offset = 0;
    int count = 0;
    uint32_t *sample_size = NULL;
    int boxtype_offset = 0;
    uint32_t *sample_size_buf = NULL;

    m4adec->sample_index = 0;

    offset = find_box_head("stsz", buffer, buff_size);
    if ((offset < 0) || (offset >= buff_size)) {
        m4adec->unproc_size = buff_size - BOX_TYPE_LEN + 1;
        return 0;
    }

    boxtype_offset = offset;

    if (buff_size - offset < sizeof(box_stsz_t)) {
        m4adec->unproc_size = buff_size - boxtype_offset;
        return 0;
    }

    box_stsz_t *stsz = (box_stsz_t *)(buffer + offset);
    memcpy(&count, &stsz->sample_count, sizeof(int));
    count = UINT32_BIG_2_LIT(count);

    offset += sizeof(box_stsz_t);
    sample_size_buf = (uint32_t *)((uint32_t)buffer + offset);
    sample_size = (uint32_t *)calloc(count * sizeof(uint32_t) + 256, 1);

    m4adec->sample_count = count;

    count = MIN(m4adec->sample_count, (buff_size - offset) / sizeof(int));

    for (i = 0; i < count; i++) {
        memcpy(&sample_size[i], sample_size_buf + i, sizeof(int));
        sample_size[i] = UINT32_BIG_2_LIT(sample_size[i]);
    }

    m4adec->sample_size_info = sample_size;
    m4adec->sample_index = count;

    offset += count * sizeof(int);

    m4adec->unproc_size = buff_size - offset;

    return 0;
}

static int m4aac_get_sample_size_remain(m4a_decoder_t *m4adec, const uint8_t *buffer, uint32_t buff_size)
{
    int i = 0;
    int offset = 0;
    int count = 0;
    uint32_t *sample_size = NULL;
    int boxtype_offset = 0;
    uint32_t *sample_size_buf = NULL;

    sample_size_buf = (uint32_t *)buffer;
    sample_size = m4adec->sample_size_info;

    count = MIN(m4adec->sample_count - m4adec->sample_index, (buff_size) / sizeof(int));

    for (i = 0; i < count; i++) {
        memcpy(&sample_size[i + m4adec->sample_index], sample_size_buf + i, sizeof(int));
        sample_size[i + m4adec->sample_index] = UINT32_BIG_2_LIT(sample_size[i + m4adec->sample_index]);
    }

    m4adec->sample_index += count;

    offset += count * sizeof(int);

    m4adec->unproc_size = buff_size - offset;

    return 0;
}

static int m4aac_get_mdat(m4a_decoder_t *m4adec, const uint8_t *buffer, uint32_t buff_size)
{
    int offset = 0;

    offset = find_box_head("mdat", buffer, buff_size);
    if (offset < 0) {
        m4adec->unproc_size = buff_size - BOX_TYPE_LEN + 1;
        return -1;
    }

    m4adec->unproc_size = buff_size - offset;
    return 0;
}

static void convert_to_local_endian(short *buffer, uint32_t samples, char *out_buffer)
{
    int i;

    for (i = 0; i < samples; i++)
    {
        out_buffer[i * 2] = (char)(buffer[i] & 0xFF);
        out_buffer[i * 2 + 1] = (char)((buffer[i] >> 8) & 0xFF);
    }
}

static void pcm_output(m4a_decoder_t *m4adec, uint8_t *in_buf, uint32_t samples)
{
	media_decoder_t *mdecoder = g_aac_mdecoder;
    media_pcminfo_t pcm_info;
	memset(&pcm_info, 0, sizeof(pcm_info));

	if (!mdecoder->running) {
		M_LOGD("samplerate %d, channels %d, samples %d\n",
				m4adec->sample_rate,
				m4adec->channels,
				samples);

		pcm_info.rate = m4adec->sample_rate;
		if (mdecoder->stere_enable) {
			pcm_info.frames = samples * m4adec->channels;
			pcm_info.channels = m4adec->channels;
		} else {
			pcm_info.channels = 1;
			pcm_info.frames = samples;
		}
		pcm_info.bits = 16;
		mdecoder->message(mdecoder->priv, PLAYER_MSG_PCM_INFO, &pcm_info);

		media_info_t media_info;
		memset(&media_info, 0, sizeof(media_info_t));
		media_info.bitrate = 0;
		mdecoder->message(mdecoder->priv, PLAYER_MSG_MEDIA_INFO, &media_info);
		mdecoder->running = 1;
	}

	if (mdecoder->output &&
		mdecoder->output(mdecoder->priv, in_buf, samples * 2)) {
		M_LOGE("output failed !\n");
		return -1;
	}
}

static int m4aac_decode_sample(m4a_decoder_t *m4adec, const uint8_t *buffer, uint32_t buff_size)
{
    int ret = 0;
    int offset = 0;
    int i = 0;

    uint32_t  start_sample = m4adec->sample_index;
    uint32_t  sample_count = m4adec->sample_count;
    uint32_t *sample_size_info = m4adec->sample_size_info;

    uint8_t *readPtr = NULL;
    uint8_t *sample_buffer = NULL;
    uint8_t *sample_addr = (uint8_t *)buffer;
    uint32_t dec_size = 0;
    uint32_t out_size = 0;

    offset = 0;
    sample_buffer = malloc(m4adec->frame_length * 4);
    for (i = start_sample; i < sample_count; i++) {
        m4adec->sample_index = i;
        offset += sample_size_info[i];
        if (buff_size < offset) {
            m4adec->unproc_size = buff_size - offset + sample_size_info[i];
            free(sample_buffer);
            return 0;
        }

        dec_size = sample_size_info[i];
        out_size = 0;
        memset(sample_buffer, 0, m4adec->frame_length * 4);

        aac_decoder_frame(sample_addr, &dec_size, sample_buffer, &out_size);

        if (out_size != 0) {
            pcm_output(m4adec, sample_buffer, out_size);
        }

        sample_addr += sample_size_info[i];
    }

    m4adec->sample_index = sample_count;
    m4adec->unproc_size = 0;
    free(sample_buffer);

    return 0;
}

static int m4aac_get_audio_config(m4a_decoder_t *m4adec, const uint8_t *buffer, uint32_t buff_size)
{
    int i = 0;
    int flag = 0;
    int offset = 0;
    int esds_size = 0;
    int boxtype_offset = 0;
    int next_offset = 0;
    int asc_size = 0;

    offset = find_box_head("esds", buffer, buff_size);
    if ((offset < 0) || (offset >= buff_size)) {
        m4adec->unproc_size = BOX_TYPE_LEN - 1;
        return -1;
    }

    boxtype_offset = offset;

    if (buff_size - offset < 10) {
        goto config_exit;
    }

    offset += 4;
    if (*(buffer + offset) != MP4_ES_DESC_TAG) {
        goto config_exit;
    }
    offset += 1;
    esds_size = *(buffer + offset);
    if (esds_size == 0x80) {
        offset += 3;
        esds_size = *(buffer + offset);
    }
    if (buff_size - offset < esds_size) {
        goto config_exit;
    }
    offset += 1;
    for (i = 0; i < esds_size; i++) {
        if (*(buffer + offset + i) == MP4_DEC_CFG_DESC_TAG) {
            flag = MP4_DEC_CFG_DESC_TAG;
        }
        else if ((*(buffer + offset + i) == MP4_DEC_SPEC_DESC_TAG) && (flag == MP4_DEC_CFG_DESC_TAG)) {
            asc_size = *(buffer + offset + i + 1);
            if (asc_size == 0x80) {
                next_offset = 3;
                asc_size = *(buffer + offset + i + 1 + next_offset);
            }
            if ((asc_size > ASC_BUFF_LEN) || (i + 1 + next_offset + asc_size > esds_size)) {
                continue;
            }
            memcpy(m4adec->audio_spec_config_info, buffer + offset + i + 2 + next_offset, asc_size);

            offset += i + 2 + next_offset + asc_size;
            m4adec->audio_spec_config_len = asc_size;
            m4adec->unproc_size = buff_size - offset;
            return 0;
        }
    }
config_exit:
    m4adec->unproc_size = buff_size - boxtype_offset;
    return -1;
}

/* esds stsz mdat  */
static int m4aac_get_audio_info(m4a_decoder_t *m4adec, const uint8_t *buffer, uint32_t buff_size)
{
    const char *f;
    int   i = 0;
    int   j = 0;
    int   flag = 1;
    const char *box_type[3] = { "esds", "stsz", "mdat" };

    if (buff_size < strlen(box_type[0])) {
        return -1;
    }

    do {
        f = (const char *)buffer + i;
        i++;
        for (j = 0; j < 3; j++) {
            if ((m4adec->decoder_find & (1 << j)) != 0) {
                continue;
            }
            flag = memcmp(box_type[j], f, strlen(box_type[j]));
            if (flag == 0) {
                if (j != 2) {
                    m4adec->decoder_status = j;
                }

                m4adec->decoder_find |= 1 << j;

                m4adec->unproc_size = buff_size - (i - 1);
                return 0;
            }
        }

    } while (i < buff_size);

    return -1;
}

static int m4a_decode_buffer(m4a_decoder_t *m4adec, const uint8_t *buffer, uint32_t buff_size)
{
    uint32_t len = 0;
    int ret = 0;

    unsigned long samplerate = 0;
    uint8_t channels = 0;

    static int decode_status = -1;
    static uint32_t  sample_index = 0;
    static uint32_t *sample_size_info = NULL;
    static uint32_t  sample_count = 0;

    m4adec->unproc_size = 0;

    switch (m4adec->decoder_status)
    {
    case -1:
        m4aac_get_audio_info(m4adec, buffer, buff_size);
        return MP4_DECODE_NEXT;
        break;

    case 0:
        ret = m4aac_get_audio_config(m4adec, buffer, buff_size);
        if (ret < 0) {
            return MP4_DECODE_NEXT;
        }

        ret = aac_decoder_config(m4adec->audio_spec_config_info, m4adec->audio_spec_config_len, &m4adec->sample_rate, &m4adec->channels, &m4adec->frame_length);
        if (ret != 0) {
            M_LOGE("aac_decoder_config fail, ret %d\n", ret);
            return MP4_DECODE_STOP;
        }

        m4adec->decoder_find |= 1;

        if (m4adec->decoder_find == 3) {
            m4adec->decoder_status = 3;
        }
        else if (m4adec->decoder_find == 7) {
            m4adec->decoder_status = 3;
            m4adec->seek_head = 1;
        }
        else {
            m4adec->decoder_status = 1;
        }
        return MP4_DECODE_NEXT;

    case 1:
        ret = m4aac_get_sample_size_info(m4adec, buffer, buff_size);
        if (m4adec->sample_count == 0) {
            return MP4_DECODE_NEXT;
        }
        if (m4adec->sample_count == m4adec->sample_index) {
            m4adec->sample_index = 0;
            m4adec->decoder_find |= 2;

            if (m4adec->decoder_find == 3) {
                m4adec->decoder_status = 3;
            }
            else if (m4adec->decoder_find == 7) {
                m4adec->decoder_status = 3;
                m4adec->seek_head = 1;
            }
            else {
                m4adec->decoder_status = 0;
            }
            return MP4_DECODE_NEXT;
        }

        m4adec->decoder_status = 2;
        return MP4_DECODE_NEXT;

    case 2:
        ret = m4aac_get_sample_size_remain(m4adec, buffer, buff_size);
        if (m4adec->sample_count == m4adec->sample_index) {
            m4adec->sample_index = 0;

            m4adec->decoder_find |= 2;

            if (m4adec->decoder_find == 3) {
                m4adec->decoder_status = 3;
            }
            else if (m4adec->decoder_find == 7) {
                m4adec->decoder_status = 3;
                m4adec->seek_head = 1;
            }
            else {
                m4adec->decoder_status = 0;
            }
        }

        return MP4_DECODE_NEXT;

    case 3:
        ret = m4aac_get_mdat(m4adec, buffer, buff_size);
        if (ret == 0) {
            m4adec->decoder_status = 4;
        }

        return MP4_DECODE_NEXT;

    case 4:
        ret = m4aac_decode_sample(m4adec, buffer, buff_size);
        if (m4adec->sample_count == m4adec->sample_index) {
            free(m4adec->sample_size_info);
            m4adec->sample_size_info = NULL;
            return MP4_DECODE_END;
        }

        return MP4_DECODE_NEXT;

    default:
        break;
    }

    return MP4_DECODE_STOP;
}

static int m4a_decoder_process(void *priv, const uint8_t *buffer, int nbytes)
{
    media_decoder_t *mdecoder = priv;
    m4a_decoder_t *m4adec;
    int ret = 0;
    int unproc_size = 0;

    if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

    g_aac_mdecoder = mdecoder;

	m4adec = mdecoder->decoder;
	if (!m4adec) {
		M_LOGE("pvm4adec null !\n");
		return -1;
	}

    ret = m4a_decode_buffer(m4adec, buffer, nbytes);
    mdecoder->unproc_size = m4adec->unproc_size;

    if (ret == MP4_DECODE_END) {
        ret = 0;
    }

    return ret;
}

static int m4a_decoder_action(void *priv, player_action_t action, void *arg)
{
    return 0;
}

int pvm4a_decoder_create(media_decoder_t *mdecoder)
{
    m4a_decoder_t *m4adec;
    int a1 = 0;

    if (!mdecoder) {
        M_LOGE("mdecoder null !\n");
        return -1;
    }

    m4adec = snd_zalloc(sizeof(m4a_decoder_t), AFM_MAIN);
    if (!m4adec) {
        M_LOGE("alloc pvm4a decoder failed !\n");
        return -1;
    }

    g_aac_mdecoder = mdecoder;

    mdecoder->decode = m4a_decoder_process;
    mdecoder->action = m4a_decoder_action;
    mdecoder->decoder = m4adec;

    aac_dec_init(1);

    M_LOGD("pvm4a decoder create\n");
    return 0;
}

int pvm4a_decoder_release(media_decoder_t *mdecoder)
{
    m4a_decoder_t *m4adec;
    if (!mdecoder) {
        M_LOGE("mdecoder null !\n");
        return -1;
    }
    m4adec = mdecoder->decoder;
    if (!m4adec) {
        M_LOGE("pvm4a_decoder null !\n");
        return -1;
    }

    aac_dec_deinit();

    snd_free(m4adec);
    mdecoder->decode = NULL;
    mdecoder->action = NULL;
    g_aac_mdecoder   = NULL;

    M_LOGD("pvm4a decoder release\n");
    return 0;
}


