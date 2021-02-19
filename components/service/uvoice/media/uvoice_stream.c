/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_config.h"
#include "uvoice_common.h"
#include "uvoice_play.h"
#include "uvoice_record.h"
#include "uvoice_stream.h"


int media_loader_reset(media_loader_t *mloader)
{
	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	if (mloader->reset && mloader->reset(mloader)) {
		M_LOGE("reset loader failed !\n");
		return -1;
	}

	return 0;
}

media_loader_t *media_loader_create(media_type_t type, char *source)
{
	media_loader_t *mloader;

	if (!source) {
		M_LOGE("source null !\n");
		return NULL;
	}

	mloader = snd_zalloc(sizeof(media_loader_t), AFM_EXTN);
	if (!mloader) {
		M_LOGE("alloc mloader failed !\n");
		return NULL;
	}

	if (type == MEDIA_TYPE_HTTP) {
		#ifdef UVOICE_HTTP_ENABLE
		if (http_loader_create(mloader, source)) {
			M_LOGE("init http loader failed !\n");
			snd_free(mloader);
			return NULL;
		}
		#else
		M_LOGE("http load not enable !\n");
		snd_free(mloader);
		return NULL;
		#endif
	} else if (type == MEDIA_TYPE_FILE) {
		#ifdef UVOICE_FILE_ENABLE
		if (file_loader_create(mloader, source)) {
			M_LOGE("init file loader failed !\n");
			snd_free(mloader);
			return NULL;
		}
		#else
		M_LOGE("file load not enable !\n");
		snd_free(mloader);
		return NULL;
		#endif
	} else if (type == MEDIA_TYPE_FLASH) {
		#ifdef UVOICE_PARTITION_ENABLE
		if (partition_loader_create(mloader, source)) {
			M_LOGE("init flash loader failed !\n");
			snd_free(mloader);
			return NULL;
		}
		#else
		M_LOGE("flash load not enable !\n");
		snd_free(mloader);
		return NULL;
		#endif
	} else {
		M_LOGE("type unsupport !\n");
		snd_free(mloader);
		return NULL;
	}

	mloader->type = type;
	return mloader;
}

int media_loader_release(media_loader_t *mloader)
{
	if (!mloader) {
		M_LOGE("mloader null !\n");
		return -1;
	}

	if (mloader->type == MEDIA_TYPE_HTTP) {
		#ifdef UVOICE_HTTP_ENABLE
		http_loader_release(mloader);
		#endif
	} else if (mloader->type == MEDIA_TYPE_FILE) {
		#ifdef UVOICE_FILE_ENABLE
		file_loader_release(mloader);
		#endif
	} else if (mloader->type == MEDIA_TYPE_FLASH) {
		#ifdef UVOICE_PARTITION_ENABLE
		partition_loader_release(mloader);
		#endif
	}

	snd_free(mloader);
	return 0;
}

media_packer_t *media_packer_create(char *sink, media_type_t type)
{
	media_packer_t *mpacker;

	if (!sink) {
		M_LOGE("sink null !\n");
		return NULL;
	}

	mpacker = snd_zalloc(sizeof(media_packer_t), AFM_EXTN);
	if (!mpacker) {
		M_LOGE("alloc media packer failed !\n");
		return NULL;
	}

	if (type == MEDIA_TYPE_FILE) {
		#ifdef UVOICE_FILE_ENABLE
		if (file_packer_create(mpacker, sink)) {
			M_LOGE("init file packer failed !\n");
			snd_free(mpacker);
			return NULL;
		}
		#else
		M_LOGE("file pack not enable !\n");
		snd_free(mpacker);
		return NULL;
		#endif
	} else if (type == MEDIA_TYPE_FLASH) {
		#ifdef UVOICE_PARTITION_ENABLE
		if (partition_packer_create(mpacker, sink)) {
			M_LOGE("init flash packer failed !\n");
			snd_free(mpacker);
			return NULL;
		}
		#else
		M_LOGE("flash pack not enable !\n");
		snd_free(mpacker);
		return NULL;
		#endif
	} else {
		M_LOGE("type unsupport !\n");
		snd_free(mpacker);
		return NULL;
	}

	mpacker->type = type;
	return mpacker;
}

int media_packer_release(media_packer_t *mpacker)
{
	if (!mpacker)
		return -1;

	if (mpacker->type == MEDIA_TYPE_FILE) {
		#ifdef UVOICE_FILE_ENABLE
		file_packer_release(mpacker);
		#endif
	} else if (mpacker->type == MEDIA_TYPE_FLASH) {
		#ifdef UVOICE_PARTITION_ENABLE
		partition_packer_release(mpacker);
		#endif
	}

	snd_free(mpacker);
	return 0;
}

