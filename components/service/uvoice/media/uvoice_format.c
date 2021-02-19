/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_common.h"
#include "uvoice_play.h"
#include "uvoice_record.h"
#include "uvoice_wave.h"


#define MP3_HEADER_MASK		0xfffe0c00
#define MP3_HEADER_SIZE		4
#define MP3_HEADER_PACK(ptr)		\
	((ptr)[0] << 24 | (ptr)[1] << 16 | (ptr)[2] << 8 | (ptr)[3])

struct mp3_id3v2_desc {
	char label[3];
	char version;
	char revision;
	char flag;
	char size[4];
};

struct mp3_id3v2_frame_header {
	char id[4];
	char size[4];
	unsigned short flag;
};

int mp3_id3v1_parse(media_info_t *info, unsigned char *data, int size)
{
	unsigned char *ptr;

	if (!data || !info) {
		M_LOGE("arg null !\n");
		return -1;
	}

	ptr = data;
	if (memcmp(ptr, "TAG", 3)) {
		M_LOGD("no tag\n");
		return -1;
	}

	ptr += 3;
	memcpy(info->name, ptr, 30);
	ptr += 30;
	memcpy(info->author, ptr, 30);
	ptr += 30;
	memcpy(info->album, ptr, 30);
	ptr += 30;
	memcpy(info->year, ptr, 4);
	ptr += 4;
	ptr += 30;
	memcpy(&info->type, ptr, 1);
	info->valid = 1;
	M_LOGD("name: %s, author: %s, album: %s, year: %s, type: %u\n",
		info->name,
		info->author,
		info->album, info->year, info->type);
	return 0;
}

static int mp3_id3v2_parse(media_info_t *info, char *data, int size)
{
	struct mp3_id3v2_desc ID3;
	struct mp3_id3v2_frame_header header;
	int id3v2_frame_size;

	if (!info || !data || size < sizeof(ID3)) {
		M_LOGE("param invalid !\n");
		return -1;
	}

	memcpy(&ID3, data, sizeof(ID3));
	if (ID3.version != 3) {
		M_LOGW("ID3 not version 3\n");
		return -1;
	}

	//M_LOGD("label %s\n", ID3.label);
	//M_LOGD("version %d\n", ID3.version);
	//M_LOGD("revision %d\n", ID3.revision);
	//M_LOGD("flag %d\n", ID3.flag);
	id3v2_frame_size = ((ID3.size[0] & 0x7f) << 21) |
		((ID3.size[1] & 0x7f) << 14) |
		((ID3.size[2] & 0x7f) << 7) |
		(ID3.size[3] & 0x7f);
	//M_LOGD("size %d\n", id3v2_frame_size);

	unsigned int pos = sizeof(struct mp3_id3v2_desc);
	unsigned int frame_size = 0;
	unsigned int frame_pos = 0;
	unsigned int copy_size = 0;
	unsigned int end = MIN(id3v2_frame_size, size - pos);
	int i, j;

	while (pos <= end) {
		memcpy(&header, data + pos ,
			sizeof(struct mp3_id3v2_frame_header));
		frame_size = header.size[0] << 24 |
			header.size[1] << 16 |
			header.size[2] << 8 |
			header.size[3];

		if (frame_size == 0) {
			pos++;
			continue;
		}

		if (!memcmp(header.id, "TIT2", 4)) {
			pos += frame_size + sizeof(struct mp3_id3v2_frame_header);
			if (pos > end)
				break;
			frame_pos = pos - frame_size;
			copy_size = MIN(sizeof(info->name) - 1, frame_size);
			for (i = 0, j = 0; i < copy_size; i++) {
				if (*(data + frame_pos + i) == '\0')
					continue;
				info->name[j++] = *(data + frame_pos + i);
			}
			info->name[j] = '\0';
			//M_LOGD("name: %s\n", info->name);
		} else if (!memcmp(header.id, "TPE1", 4)) {
			pos += frame_size + sizeof(struct mp3_id3v2_frame_header);
			if (pos > end)
				break;
			frame_pos = pos - frame_size;
			copy_size = MIN(sizeof(info->author) - 1, frame_size);
			for (i = 0, j = 0; i < copy_size; i++) {
				if (*(data + frame_pos + i) == '\0')
					continue;
				info->author[j++] = *(data + frame_pos + i);
			}
			info->author[j] = '\0';
			//M_LOGD("author: %s\n", info->author);
		} else if (!memcmp(header.id, "TALB", 4)) {
			pos += frame_size + sizeof(struct mp3_id3v2_frame_header);
			if (pos > end)
				break;
			frame_pos = pos - frame_size;
			copy_size = MIN(sizeof(info->album) - 1, frame_size);
			for (i = 0, j = 0; i < copy_size; i++) {
				if (*(data + frame_pos + i) == '\0')
					continue;
				info->album[j++] = *(data + frame_pos + i);
			}
			info->album[j] = '\0';
			//M_LOGD("album: %s\n", info->album);
		} else if (!memcmp(header.id, "TYER", 4)) {
			pos += frame_size + sizeof(struct mp3_id3v2_frame_header);
			if (pos > end)
				break;
			frame_pos = pos - frame_size;
			copy_size = MIN(sizeof(info->year) - 1, frame_size);
			for (i = 0, j = 0; i < copy_size; i++) {
				if (*(data + frame_pos + i) == '\0')
					continue;
				info->year[j++] = *(data + frame_pos + i);
			}
			info->year[j] = '\0';
			//M_LOGD("year: %s\n", info->year);
		} else if (!memcmp(header.id, "TCON", 4)) {
			pos += frame_size + sizeof(struct mp3_id3v2_frame_header);
			if (pos > end)
				break;
			frame_pos = pos - frame_size;
			char type[32];
			memset(type, 0, sizeof(type));
			copy_size = MIN(sizeof(type) - 1, frame_size);
			for (i = 0, j = 0; i < copy_size; i++) {
				if (*(data + frame_pos + i) == '\0')
					continue;
				type[j++] = *(data + frame_pos + i);
			}
			type[j] = '\0';
			//M_LOGD("type: %s\n", type);
		} else if (!memcmp(header.id, "TSSE", 4)) {
			pos += frame_size + sizeof(struct mp3_id3v2_frame_header);
			if (pos > end)
				break;
			frame_pos = pos - frame_size;
			char tsse[32];
			memset(tsse, 0, sizeof(tsse));
			copy_size = MIN(sizeof(tsse) - 1, frame_size);
			for (i = 0, j = 0; i < copy_size; i++) {
				if (*(data + frame_pos + i) == '\0')
					continue;
				tsse[j++] = *(data + frame_pos + i);
			}
			tsse[j] = '\0';
			//M_LOGD("tsse: %s\n", tsse);
		} else {
			pos++;
			continue;
		}

		//M_LOGD("frame id %s frame_size %u frame_pos %d\n",
		//	header.id, frame_size, frame_pos);
	}
	return 0;
}

bool wav_format_check(char *buffer, int size)
{
	struct riff_wave_header riff_wave_header;
	if (buffer && size >= sizeof(riff_wave_header)) {
		memcpy(&riff_wave_header, buffer,
			sizeof(riff_wave_header));
		if (riff_wave_header.riff_id == ID_RIFF &&
			riff_wave_header.wave_id == ID_WAVE)
			return true;
	}
	return false;
}

bool mp3_format_check(char *data, int size)
{
	if (!data || size < strlen("ID3")) {
		M_LOGE("arg invalid !\n");
		return false;
	}

	if (!memcmp(data, "ID3", strlen("ID3"))) {
		struct mp3_id3v2_desc ID3;
		memcpy(&ID3, data, sizeof(struct mp3_id3v2_desc));
		M_LOGD("ID3 found, version %d\n", ID3.version);
		if (ID3.version != 3 && ID3.version != 4)
			return false;
		media_info_t info;
		memset(&info, 0, sizeof(info));

		mp3_id3v2_parse(&info, data, size);
		return true;
	}

	return false;
}

bool aac_format_check(char *buffer, int size)
{
	return false;
}

bool m4a_format_check(char *buffer, int size)
{
	if (!buffer || size < 8) {
		M_LOGE("arg invalid !\n");
		return false;
	}

	return !memcmp(buffer + 4, "ftyp", strlen("ftyp"));
}

bool ogg_format_check(char *buffer, int size)
{
	if (!buffer || size < 4) {
		M_LOGE("arg invalid !\n");
		return false;
	}

	return !memcmp(buffer, "OggS", strlen("OggS"));
}

bool wma_format_check(char *buffer, int size)
{
	if (!buffer || size < 4) {
		M_LOGE("arg invalid !\n");
		return false;
	}

	return !memcmp(buffer, "MAC ", strlen("MAC "));
}

bool amr_format_check(char *buffer, int size)
{
	if (!buffer || size < 6) {
		M_LOGE("arg invalid !\n");
		return false;
	}

	return !memcmp(buffer, "#!AMR\n", strlen("#!AMR\n"));
}

bool amrwb_format_check(char *buffer, int size)
{
	if (!buffer || size < 8) {
		M_LOGE("arg invalid !\n");
		return false;
	}

	return !memcmp(buffer, "#!AMR-WB\n", strlen("#!AMR-WB\n"));
}

bool ape_format_check(char *buffer, int size)
{
	return false;
}

bool flac_format_check(char *buffer, int size)
{
	if (!buffer || size < 4) {
		M_LOGE("arg invalid !\n");
		return false;
	}

	return !memcmp(buffer, "fLaC", strlen("fLaC"));
}

static bool suffix_assert(char *str, char *suffix)
{
	bool ret = false;

	if (!str || !suffix || strlen(str) < strlen(suffix))
		return ret;

	ret = !memcmp((str + strlen(str) - strlen(suffix)),
		suffix, strlen(suffix));

	if (!ret) {
		if (!strncmp(str, "http://", strlen("http://")) ||
			!strncmp(str, "https://", strlen("https://")))
			ret = !!strstr(str, suffix);
	}

	return ret;
}

int format_parse_byname(char *name, media_format_t *format)
{
	if (!name || !format) {
		M_LOGE("arg null !\n");
		return -1;
	}

	if (suffix_assert(name, ".mp3") || suffix_assert(name, ".MP3"))
		*format = MEDIA_FMT_MP3;
	else if (suffix_assert(name, ".wav") || suffix_assert(name, ".WAV"))
		*format = MEDIA_FMT_WAV;
	else if (suffix_assert(name, ".aac") || suffix_assert(name, ".AAC"))
		*format = MEDIA_FMT_AAC;
	else if (suffix_assert(name, ".m4a") || suffix_assert(name, ".M4A"))
		*format = MEDIA_FMT_M4A;
	else if (suffix_assert(name, ".pcm") || suffix_assert(name, ".PCM"))
		*format = MEDIA_FMT_PCM;
	else if (suffix_assert(name, ".spx") || suffix_assert(name, ".SPX"))
		*format = MEDIA_FMT_SPX;
	else if (suffix_assert(name, ".ogg") || suffix_assert(name, ".OGG"))
		*format = MEDIA_FMT_OGG;
	else if (suffix_assert(name, ".wma") || suffix_assert(name, ".WMA"))
		*format = MEDIA_FMT_WMA;
	else if (suffix_assert(name, ".amrwb") || suffix_assert(name, ".AMRWB"))
		*format = MEDIA_FMT_AMRWB;
	else if (suffix_assert(name, ".amr") || suffix_assert(name, ".AMR"))
		*format = MEDIA_FMT_AMR;
	else if (suffix_assert(name, ".opus") || suffix_assert(name, ".OPUS"))
		*format = MEDIA_FMT_OPS;
	else if (suffix_assert(name, ".flac") || suffix_assert(name, ".FLAC"))
		*format = MEDIA_FMT_FLAC;

	return 0;
}

