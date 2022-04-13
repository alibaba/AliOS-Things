/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_types.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_os.h"
#include "uvoice_common.h"
#include "uvoice_play.h"
#include "uvoice_record.h"
#include "uvoice_format.h"


struct partition_loader {
	int part; /* partition id */
	uint32_t part_size;
	uint32_t begin;
	uint32_t rd_offset;
	int32_t bin_size;
	long seek_offset;
	os_mutex_t lock;
};

struct partition_packer {
	int part; /* partition id */
	uint32_t part_size;
	uint32_t begin;
	uint32_t wr_offset;
	int bin_size;
	os_mutex_t lock;
};


static int partition_get_format(void *priv, media_format_t *format)
{
	media_loader_t *mloader = (media_loader_t *)priv;
	struct partition_loader *loader;
	uint8_t *head_buffer;
	int32_t buffer_size;
	int32_t rd_offset;
	int ret;

	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	loader = mloader->loader;
	if (!loader) {
		M_LOGE("loader null x!\n");
		return -1;
	}

	os_mutex_lock(loader->lock, OS_WAIT_FOREVER);

	buffer_size = MIN(512, loader->bin_size);
	head_buffer = snd_zalloc(buffer_size, AFM_EXTN);
	if (!head_buffer) {
		M_LOGE("alloc buffer failed !\n");
		os_mutex_unlock(loader->lock);
		return -1;
	}

	rd_offset = loader->rd_offset;
	loader->rd_offset = 0;
	ret = os_partition_read(loader->part,
		&loader->rd_offset, head_buffer, buffer_size);
	if (ret) {
		M_LOGE("read partition failed %d!\n", ret);
		loader->rd_offset = rd_offset;
		os_mutex_unlock(loader->lock);
		return -1;
	}
	loader->rd_offset = rd_offset;
	os_mutex_unlock(loader->lock);

	if (flac_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_FLAC;
		M_LOGD("format FLAC\n");
	} else if (mp3_format_check(head_buffer, buffer_size)) { 
		*format = MEDIA_FMT_MP3;
		M_LOGD("format MP3\n");
	} else if (wav_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_WAV;
		M_LOGD("format WAV\n");
	} else if (aac_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_AAC;
		M_LOGD("format AAC\n");
	} else if (m4a_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_M4A;
		M_LOGD("format M4A\n");
	} else if (ogg_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_OGG;
		M_LOGD("format OGG\n");
	} else if (wma_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_WMA;
		M_LOGD("format WMA\n");
	} else if (amrwb_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_AMRWB;
		M_LOGD("format AMR-WB\n");
	} else if (amr_format_check(head_buffer, buffer_size)) {
		*format = MEDIA_FMT_AMR;
		M_LOGD("format AMR\n");
	}

	snd_free(head_buffer);
	return 0;
}

static int partition_get_mediainfo(void *priv,
	media_info_t *info, media_format_t format)
{
	media_loader_t *mloader = (media_loader_t *)priv;
	struct partition_loader *loader;
	int ret;

	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	loader = mloader->loader;
	if (!loader) {
		M_LOGE("loader null !\n");
		return -1;
	}

	os_mutex_lock(loader->lock, OS_WAIT_FOREVER);

	if (format == MEDIA_FMT_MP3) {
		if (loader->bin_size < 128) {
			M_LOGE("binary too short !\n");
			os_mutex_unlock(loader->lock);
			return -1;
		}
		char buffer[128];
		memset(buffer, 0, sizeof(buffer));

		int rd_offset = loader->rd_offset;
		loader->rd_offset =
			loader->begin + loader->bin_size - 128;
		ret = os_partition_read(loader->part, &loader->rd_offset,
			buffer, 128);
		if (ret) {
			M_LOGE("read partition failed %d!\n", ret);
			loader->rd_offset = rd_offset;
			os_mutex_unlock(loader->lock);
			return -1;
		}
		loader->rd_offset = rd_offset;
		mp3_id3v1_parse(info, buffer, 128);
	}

	os_mutex_unlock(loader->lock);
	return 0;
}

static int partition_info_parse(char *info,
	unsigned int *part, unsigned int *begin, int *bin_size)
{
	char *ptr;

	if (!info || !part || !begin || !bin_size) {
		M_LOGE("arg null !\n");
		return -1;
	}

	ptr = strstr(info, "pt:");
	if (ptr) {
		ptr += strlen("pt:");
		*part = atoi(ptr);
	} else {
		M_LOGE("part not found !\n");
		return -1;
	}

	ptr = strstr(info, "offset:");
	if (ptr) {
		ptr += strlen("offset:");
		*begin = atoi(ptr);
	} else {
		M_LOGE("offset not found !\n");
		return -1;
	}

	ptr = strstr(info, "len:");
	if (ptr) {
		ptr += strlen("len:");
		*bin_size = atoi(ptr);
	} else {
		M_LOGE("len not found !\n");
		return -1;
	}

	return 0;
}

static int partition_loader_update(struct partition_loader *loader,
	char *sink)
{
	const char *part_name;
	int part_size;

	if (!loader) {
		M_LOGE("loader null !\n");
		return -1;
	}

	if (!sink) {
		M_LOGE("sink null !\n");
		return -1;
	}

	if (partition_info_parse(sink, &loader->part,
			&loader->begin, &loader->bin_size)) {
		M_LOGE("parse partition info failed !\n");
		return -1;
	}

	M_LOGD("part %d begin %u bin_size %d\n",
		loader->part,
		loader->begin, loader->bin_size);

	part_name = os_partition_name(loader->part);
	if (!part_name) {
		M_LOGE("get partition name failed !\n");
		return -1;
	}

	if (strncmp(part_name, "UVOICE MEDIA", strlen("UVOICE MEDIA"))) {
		M_LOGE("partition name %s not match !\n", part_name);
		return -1;
	}

	part_size = os_partition_size(loader->part);
	if (part_size < 0) {
		M_LOGE("get partition size failed !\n");
		return -1;
	}

	if (loader->bin_size > part_size) {
		M_LOGE("bin size %d overrange !\n", loader->bin_size);
		return -1;
	}

	loader->part_size = part_size;
	loader->rd_offset = loader->begin;
	return 0;
}

static int partition_loader_reset(void *priv)
{
	media_loader_t *mloader = (media_loader_t *)priv;
	struct partition_loader *loader;

	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	loader = mloader->loader;
	if (!loader) {
		M_LOGE("loader null !\n");
		return -1;
	}

	os_mutex_lock(loader->lock, OS_WAIT_FOREVER);

	loader->begin = 0;
	loader->rd_offset = loader->begin;
	loader->part_size = 0;
	loader->part = -1;
	loader->seek_offset = 0;
	os_mutex_unlock(loader->lock);

	M_LOGD("partition load reset\n");
	return 0;
}

static int partition_loader_read(void *priv,
	uint8_t *buffer, int nbytes)
{
	media_loader_t *mloader = (media_loader_t *)priv;
	struct partition_loader *loader;
	int read_size = 0;
	int ret;

	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	loader = mloader->loader;
	if (!loader) {
		M_LOGE("loader null !\n");
		return -1;
	}

	os_mutex_lock(loader->lock, OS_WAIT_FOREVER);

	if (loader->seek_offset != 0) {
		if (loader->seek_offset > 0) {
			if (loader->seek_offset <=
				loader->bin_size - loader->rd_offset) {
				loader->rd_offset += loader->seek_offset;
				if (mloader->message)
					mloader->message(mloader->priv,
						PLAYER_MSG_SEEK_DONE, (void *)loader->seek_offset);
			}
		} else {
			if (abs(loader->seek_offset) <= loader->rd_offset) {
				loader->rd_offset += loader->seek_offset;
				if (mloader->message)
					mloader->message(mloader->priv,
						PLAYER_MSG_SEEK_DONE, (void *)loader->seek_offset);
			}
		}
		loader->seek_offset = 0;
	}

	if (loader->rd_offset >= loader->bin_size) {
		M_LOGD("read end\n");
		goto __exit;
	}

	read_size = MIN(nbytes,
		loader->bin_size - loader->rd_offset);
	ret = os_partition_read(loader->part,
		&loader->rd_offset, buffer, read_size);
	if (ret) {
		M_LOGE("read failed %d!\n", ret);
		os_mutex_unlock(loader->lock);
		return -1;
	}
	if (read_size != nbytes)
		M_LOGD("read %d ret %d\n", nbytes, read_size);

__exit:
	os_mutex_unlock(loader->lock);
	return read_size;
}

static int partition_loader_action(void *priv,
		player_action_t action, void *arg)
{
	media_loader_t *mloader = (media_loader_t *)priv;
	struct partition_loader *loader;

	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	loader = mloader->loader;
	if (!loader) {
		M_LOGE("loader null !\n");
		return -1;
	}

	if (action == PLAYER_START) {
		if (mloader->message) {
			media_info_t media_info;
			memset(&media_info, 0, sizeof(media_info_t));
			os_mutex_lock(loader->lock, OS_WAIT_FOREVER);

			media_info.media_size = loader->bin_size;
			os_mutex_unlock(loader->lock);
			mloader->message(mloader->priv,
				PLAYER_MSG_MEDIA_INFO, &media_info);
		}
	} else if (action == PLAYER_STOP) {
		os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
		loader->rd_offset = loader->begin;
		loader->seek_offset = 0;
		os_mutex_unlock(loader->lock);
	} else if (action == PLAYER_SEEK) {
		os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
		loader->seek_offset = (long)arg;
		os_mutex_unlock(loader->lock);
	} else if (action == PLAYER_COMPLETE) {
	} else if (action == PLAYER_NEXT) {
		os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
		if (partition_loader_update(loader, (char *)arg)) {
			M_LOGE("update failed !\n");
			os_mutex_unlock(loader->lock);
			return -1;
		}
		os_mutex_unlock(loader->lock);
	}

	return 0;
}

int partition_loader_create(media_loader_t *mloader, char *source)
{
	struct partition_loader *loader;
	const char *part_name;
	int part_size;

	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	if (!source) {
		M_LOGE("info null !\n");
		return -1;
	}

	loader = snd_zalloc(sizeof(struct partition_loader), AFM_EXTN);
	if (!loader) {
		M_LOGE("alloc partition loader failed !\n");
		return -1;
	}

	if (partition_info_parse(source, &loader->part,
			&loader->begin, &loader->bin_size)) {
		M_LOGE("parse partition info failed !\n");
		snd_free(loader);
		return -1;
	}

	M_LOGD("part %d begin %u bin_size %d\n",
		loader->part, loader->begin, loader->bin_size);

	part_name = os_partition_name(loader->part);
	if (!part_name) {
		M_LOGE("get partition name failed !\n");
		snd_free(loader);
		return -1;
	}

	if (strncmp(part_name, "UVOICE MEDIA", strlen("UVOICE MEDIA"))) {
		M_LOGE("partition %s not media !\n", part_name);
		snd_free(loader);
		return -1;
	}

	part_size = os_partition_size(loader->part);
	if (part_size < 0) {
		M_LOGE("get partition size failed !\n");
		snd_free(loader);
		return -1;
	}

	if (loader->bin_size > part_size) {
		M_LOGE("bin size %d overrange !\n", loader->bin_size);
		snd_free(loader);
		return -1;
	}

	loader->part_size = part_size;
	loader->rd_offset = loader->begin;

	loader->lock = os_mutex_new();
	loader->seek_offset = 0;

	mloader->read = partition_loader_read;
	mloader->action = partition_loader_action;
	mloader->get_format = partition_get_format;
	mloader->get_mediainfo = partition_get_mediainfo;
	mloader->get_cacheinfo = NULL;
	mloader->reset = partition_loader_reset;
	mloader->loader = loader;

	M_LOGD("partition loader create\n");
	return 0;
}

int partition_loader_release(media_loader_t *mloader)
{
	struct partition_loader *loader;

	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	loader = mloader->loader;
	if (!loader) {
		M_LOGE("loader null !\n");
		return -1;
	}

	os_mutex_free(loader->lock);
	snd_free(loader);
	mloader->loader = NULL;

	M_LOGD("partition loader release\n");
	return 0;
}

static int partition_packer_write(void *priv,
	uint8_t *buffer, int nbytes)
{
	media_packer_t *mpacker = (media_packer_t *)priv;
	struct partition_packer *packer;
	int wr_size = 0;
	int ret = 0;

	if (!mpacker) {
		M_LOGE("mpacker null !\n");
		return -1;
	}

	packer = mpacker->packer;
	if (!packer) {
		M_LOGE("packer null !\n");
		return -1;
	}

	os_mutex_lock(packer->lock, OS_WAIT_FOREVER);
	wr_size = MIN(nbytes,
		packer->part_size - packer->wr_offset);

	if (wr_size <= 0) {
		M_LOGE("write end, bin size %u\n",
			packer->wr_offset);
		goto __exit;
	}

	ret = os_partition_write(packer->part,
		&packer->wr_offset, buffer, wr_size);
	if (ret) {
		M_LOGE("write failed %d!\n", ret);
		os_mutex_unlock(packer->lock);
		return -1;
	}

	mpacker->size += wr_size;
	ret = wr_size;

__exit:
	os_mutex_unlock(packer->lock);
	return ret;
}

static int partition_packer_update(void *priv,
	uint8_t *buffer, int nbytes, int pos)
{
	media_packer_t *mpacker = (media_packer_t *)priv;
	struct partition_packer *packer;
	int wr_offset = 0;
	int ret = 0;

	if (!mpacker) {
		M_LOGE("mpacker null !\n");
		return -1;
	}

	packer = mpacker->packer;
	if (!packer) {
		M_LOGE("packer null !\n");
		return -1;
	}

	os_mutex_lock(packer->lock, OS_WAIT_FOREVER);

	if (pos + nbytes > packer->part_size) {
		M_LOGE("pack size %d overrange !\n", nbytes);
		os_mutex_unlock(packer->lock);
		return -1;
	}

	wr_offset = packer->begin + pos;

	ret = os_partition_erase(packer->part,
		wr_offset, nbytes);
	if (ret) {
		M_LOGE("erase partition failed %d!\n", ret);
		os_mutex_unlock(packer->lock);
		return -1;
	}

	ret = os_partition_write(packer->part,
		&wr_offset, buffer, nbytes);
	if (ret) {
		M_LOGE("write partition failed %d!\n", ret);
		os_mutex_unlock(packer->lock);
		return -1;
	}

__exit:
	os_mutex_unlock(packer->lock);
	return ret;
}

static int partition_packer_action(void *priv,
		recorder_action_t action, void *arg)
{
	media_packer_t *mpacker = (media_packer_t *)priv;
	struct partition_packer *packer;

	if (!mpacker) {
		M_LOGE("mpacker null !\n");
		return -1;
	}

	packer = mpacker->packer;
	if (!packer) {
		M_LOGE("packer null !\n");
		return -1;
	}

	return 0;
}

int partition_packer_create(media_packer_t *mpacker, char *sink)
{
	struct partition_packer *packer;
	const char *part_name;
	int part_size;
	int ret;

	if (!mpacker) {
		M_LOGE("mpacker null !\n");
		return -1;
	}

	if (!sink) {
		M_LOGE("sink null !\n");
		return -1;
	}

	packer = snd_zalloc(sizeof(struct partition_packer), AFM_EXTN);
	if (!packer) {
		M_LOGE("alloc partition packer failed !\n");
		return -1;
	}

	if (partition_info_parse(sink, &packer->part,
			&packer->begin, &packer->bin_size)) {
		M_LOGE("parse partition info failed !\n");
		snd_free(packer);
		return -1;
	}

	M_LOGD("part %d begin %u bin_size %d\n",
		packer->part,
		packer->begin, packer->bin_size);

	part_name = os_partition_name(packer->part);
	if (!part_name) {
		M_LOGE("get partition name failed !\n");
		snd_free(packer);
		return -1;
	}

	if (strncmp(part_name, "UVOICE MEDIA", strlen("UVOICE MEDIA"))) {
		M_LOGE("partition name %s not match !\n", part_name);
		snd_free(packer);
		return -1;
	}

	part_size = os_partition_size(packer->part);
	if (part_size < 0) {
		M_LOGE("get partition size failed !\n");
		snd_free(packer);
		return -1;
	}

	if (packer->bin_size > part_size) {
		M_LOGE("bin size %d overrange !\n", packer->bin_size);
		snd_free(packer);
		return -1;
	}

	M_LOGD("partition partition erasing...\n");
	ret = os_partition_erase(packer->part, packer->wr_offset,
		part_size);
	if (ret) {
		M_LOGE("erase partition failed %d!\n", ret);
		snd_free(packer);
		return -1;
	}

	packer->part_size = part_size;
	if (packer->part_size < packer->bin_size) {
		M_LOGE("binary size %d overrange !\n", packer->bin_size);
		snd_free(packer);
		return -1;
	}

	packer->lock = os_mutex_new();
	mpacker->packer = packer;
	mpacker->pack = partition_packer_write;
	mpacker->update = partition_packer_update;
	mpacker->action = partition_packer_action;

	M_LOGD("partition packer create\n");
	return 0;
}

int partition_packer_release(media_packer_t *mpacker)
{
	struct partition_packer *packer;

	if (!mpacker) {
		M_LOGE("mpacker null !\n");
		return -1;
	}

	packer = mpacker->packer;
	if (!packer) {
		M_LOGE("packer null !\n");
		return -1;
	}

	os_mutex_free(packer->lock);
	snd_free(packer);
	mpacker->packer = NULL;

	M_LOGD("partition packer release\n");
	return 0;
}

