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


struct file_loader {
    char filename[256];
    os_file_t stream;
    int file_length;
    int rd_pos;
    long seek_offset;
    os_mutex_t lock;
};

struct file_packer {
    os_file_t stream;
    os_mutex_t lock;
};

static int file_get_format(void *priv, media_format_t *format)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    struct file_loader *loader;
    uint8_t *head_buffer;
    int buffer_size;
    int file_pos;
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

    if (!OS_FILE_OPENING(loader->stream)) {
        M_LOGE("file not open !\n");
        os_mutex_unlock(loader->lock);
        return -1;
    }

    buffer_size = MIN(512, loader->file_length);
    head_buffer = snd_zalloc(buffer_size, AFM_EXTN);
    if (!head_buffer) {
        M_LOGE("alloc buffer failed !\n");
        os_mutex_unlock(loader->lock);
        return -1;
    }

    file_pos = os_ftell(loader->stream);
    if (file_pos < 0)
        file_pos = loader->rd_pos;
    (void)os_fseek(loader->stream, 0, OS_SEEK_SET);
    ret = os_fread(head_buffer, 1, buffer_size, loader->stream);
    if (ret != buffer_size) {
        M_LOGE("read failed %d!\n", ret);
        (void)os_fseek(loader->stream, file_pos, OS_SEEK_SET);
        snd_free(head_buffer);
        os_mutex_unlock(loader->lock);
        return -1;
    }
    (void)os_fseek(loader->stream, file_pos, OS_SEEK_SET);
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

static int file_get_mediainfo(void *priv, media_info_t *info,
    media_format_t format)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    struct file_loader *loader;

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

    if (!OS_FILE_OPENING(loader->stream)) {
        M_LOGE("file not open !\n");
        os_mutex_unlock(loader->lock);
        return -1;
    }

    if (format == MEDIA_FMT_MP3) {
        if (loader->file_length < 128) {
            M_LOGE("file too short !\n");
            os_mutex_unlock(loader->lock);
            return -1;
        }
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));

        int file_pos = os_ftell(loader->stream);
        if (file_pos < 0)
            file_pos = loader->rd_pos;
        (void)os_fseek(loader->stream, -128, OS_SEEK_END);
        int ret = os_fread(buffer, 1, 128, loader->stream);
        if (ret != 128) {
            M_LOGE("read failed %d!\n", ret);
            (void)os_fseek(loader->stream, file_pos, OS_SEEK_SET);
            os_mutex_unlock(loader->lock);
            return -1;
        }
        (void)os_fseek(loader->stream, file_pos, OS_SEEK_SET);

        mp3_id3v1_parse(info, buffer, 128);
    }

    os_mutex_unlock(loader->lock);
    return 0;
}

static int file_loader_reset(void *priv)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    struct file_loader *loader;

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

    if (OS_FILE_OPENING(loader->stream)) {
        M_LOGE("file load not stop !\n");
        os_mutex_unlock(loader->lock);
        return -1;
    }

    loader->rd_pos = 0;
    loader->seek_offset = 0;
    loader->file_length = 0;
    os_mutex_unlock(loader->lock);

    M_LOGD("file load reset\n");
    return 0;
}

static int file_loader_update(struct file_loader *loader, char *sink)
{
    int ret;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (!sink) {
        M_LOGE("sink null!\n");
        return -1;
    }

    if (loader->stream) {
        M_LOGE("prev file not close !\n");
        return -1;
    }

    if (strncmp(sink, "fs:", strlen("fs:"))) {
        M_LOGE("filename invalid !\n");
        return -1;
    }

    sink += strlen("fs:");
    if (strlen(sink) + 1 > sizeof(loader->filename)) {
        M_LOGE("filename length overrange !\n");
        return -1;
    }

    memset(loader->filename, 0, sizeof(loader->filename));
    memcpy(loader->filename, sink, strlen(sink) + 1);
    loader->stream = os_fopen(loader->filename, "rb");
    if (OS_FILE_OPEN_FAIL(loader->stream)) {
        M_LOGE("open %s failed !\n", loader->filename);
        return -1;
    }

    ret = (int)os_fseek(loader->stream, 0, OS_SEEK_END);
    loader->file_length = os_ftell(loader->stream);
    if (loader->file_length < 0)
        loader->file_length = ret;
    (void)os_fseek(loader->stream, 0, OS_SEEK_SET);
    loader->rd_pos = 0;
    loader->seek_offset = 0;

    return 0;
}

static int file_loader_read(void *priv, uint8_t *buffer, int nbytes)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    struct file_loader *loader;
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

    if (mloader->rebase_request) {
        mloader->rebase_request = 0;
        loader->seek_offset = loader->rd_pos - mloader->rebase_offset;
        M_LOGI("read pos rebase %d\n", mloader->rebase_offset);
    }

    os_mutex_lock(loader->lock, OS_WAIT_FOREVER);

    if (loader->seek_offset != 0) {
        if (loader->seek_offset > 0) {
            if (loader->seek_offset <=
                loader->file_length - loader->rd_pos) {
                (void)os_fseek(loader->stream, loader->seek_offset,
                    OS_SEEK_CUR);
                ret = os_ftell(loader->stream);
                if (ret < 0)
                    loader->rd_pos += loader->seek_offset;
                else
                    loader->rd_pos = ret;
                if (mloader->message)
                    mloader->message(mloader->priv,
                        PLAYER_MSG_SEEK_DONE, (void *)loader->seek_offset);
            }
        } else {
            if (abs(loader->seek_offset) <= loader->rd_pos) {
                (void)os_fseek(loader->stream, loader->seek_offset,
                    OS_SEEK_CUR);
                ret = os_ftell(loader->stream);
                if (ret < 0)
                    loader->rd_pos += loader->seek_offset;
                else
                    loader->rd_pos = ret;
                if (mloader->message)
                    mloader->message(mloader->priv,
                        PLAYER_MSG_SEEK_DONE, (void *)loader->seek_offset);
            }
        }
        loader->seek_offset = 0;
    }

    if (os_feof(loader->stream) ||
        loader->rd_pos >= loader->file_length) {
        nbytes = 0;
        M_LOGD("read end\n");
        goto __exit;
    } else if (loader->file_length - loader->rd_pos < nbytes) {
        nbytes = loader->file_length - loader->rd_pos;
        M_LOGD("read tail %d\n", nbytes);
    }

    ret = os_fread(buffer, 1, nbytes, loader->stream);
    if (os_ferror(loader->stream) || ret != nbytes) {
        M_LOGE("read failed %d!\n", os_ferror(loader->stream));
        os_mutex_unlock(loader->lock);
        return -1;
    }

    loader->rd_pos += nbytes;

__exit:
    os_mutex_unlock(loader->lock);
    return nbytes;
}

static int file_loader_action(void *priv, player_action_t action, void *arg)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    struct file_loader *loader;
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

    if (action == PLAYER_START) {
        os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
        if (!OS_FILE_OPENING(loader->stream)) {
            M_LOGD("open %s\n", loader->filename);
            loader->stream = os_fopen(loader->filename, "rb");
            if (OS_FILE_OPEN_FAIL(loader->stream)) {
                M_LOGE("open %s failed !\n", loader->filename);
                os_mutex_unlock(loader->lock);
                return -1;
            }
            ret = (int)os_fseek(loader->stream, 0, OS_SEEK_END);
            loader->file_length = os_ftell(loader->stream);
            if (loader->file_length < 0)
                loader->file_length = ret;
            (void)os_fseek(loader->stream, 0, OS_SEEK_SET);
            loader->rd_pos = 0;
            loader->seek_offset = 0;
            M_LOGI("file_length %d\n", loader->file_length);
        }
        if (mloader->message) {
            media_info_t media_info;
            memset(&media_info, 0, sizeof(media_info_t));
            media_info.media_size = loader->file_length;
            mloader->message(mloader->priv,
                PLAYER_MSG_MEDIA_INFO, &media_info);
        }
        os_mutex_unlock(loader->lock);
    } else if (action == PLAYER_STOP) {
        os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
        if (OS_FILE_OPENING(loader->stream)) {
            M_LOGD("file close\n");
            os_fclose(loader->stream);
            loader->stream = NULL;
        }
        os_mutex_unlock(loader->lock);
    } else if (action == PLAYER_RELOAD) {
        os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
        if (!OS_FILE_OPENING(loader->stream)) {
            M_LOGD("file not open\n");
            os_mutex_unlock(loader->lock);
            return -1;
        }
        M_LOGD("reload from %x\n", (uint32_t)arg);
        loader->seek_offset = 0;
        loader->rd_pos = (int)arg;
        (void)os_fseek(loader->stream, (long)arg, OS_SEEK_SET);
        os_mutex_unlock(loader->lock);
    } else if (action == PLAYER_SEEK) {
        os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
        loader->seek_offset = (long)arg;
        os_mutex_unlock(loader->lock);
    } else if (action == PLAYER_COMPLETE) {
        os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
        if (OS_FILE_OPENING(loader->stream)) {
            M_LOGD("file close\n");
            os_fclose(loader->stream);
            loader->stream = OS_FILE_CLOSED;
        } else {
            M_LOGW("file not open !\n");
        }
        os_mutex_unlock(loader->lock);
    } else if (action == PLAYER_NEXT) {
        os_mutex_lock(loader->lock, OS_WAIT_FOREVER);
        if (file_loader_update(loader, (char *)arg)) {
            M_LOGE("update failed !\n");
            os_mutex_unlock(loader->lock);
            return -1;
        }
        os_mutex_unlock(loader->lock);
    }

    return 0;
}

int file_loader_create(media_loader_t *mloader, char *source)
{
    struct file_loader *loader;
    int ret;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    if (!source) {
        M_LOGE("source null !\n");
        return -1;
    }

    if (strncmp(source, "fs:", strlen("fs:"))) {
        M_LOGE("filename invalid !\n");
        return -1;
    }

    source += strlen("fs:");

    loader = snd_zalloc(sizeof(struct file_loader), AFM_EXTN);
    if (!loader) {
        M_LOGE("alloc file loader failed !\n");
        return -1;
    }

    if (strlen(source) + 1 > sizeof(loader->filename)) {
        M_LOGE("filename length overrange !\n");
        snd_free(loader);
        return -1;
    }

    memcpy(loader->filename, source, strlen(source) + 1);

    loader->stream = os_fopen(loader->filename, "rb");
    if (OS_FILE_OPEN_FAIL(loader->stream)) {
        M_LOGE("open %s failed !\n", loader->filename);
        snd_free(loader);
        return -1;
    }
    ret = os_fseek(loader->stream, 0, OS_SEEK_END);
    loader->file_length = os_ftell(loader->stream);
    if (loader->file_length < 0)
        loader->file_length = ret;
    (void)os_fseek(loader->stream, 0, OS_SEEK_SET);

    if (loader->file_length <= 0) {
        M_LOGE("file lenth %u invalid !\n", loader->file_length);
        os_fclose(loader->stream);
        snd_free(loader);
        return -1;
    }

    loader->lock = os_mutex_new();
    loader->seek_offset = 0;
    loader->rd_pos = 0;

    mloader->read = file_loader_read;
    mloader->action = file_loader_action;
    mloader->get_format = file_get_format;
    mloader->get_mediainfo = file_get_mediainfo;
    mloader->get_cacheinfo = NULL;
    mloader->reset = file_loader_reset;
    mloader->loader = loader;

    M_LOGD("file loader create\n");
    return 0;
}

int file_loader_release(media_loader_t *mloader)
{
    struct file_loader *loader;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (OS_FILE_OPENING(loader->stream)) {
        M_LOGD("file close\n");
        os_fclose(loader->stream);
        loader->stream = OS_FILE_CLOSED;
    }

    os_mutex_free(loader->lock);
    snd_free(loader);
    mloader->loader = NULL;

    M_LOGD("file loader release\n");
    return 0;
}

static int file_packer_write(void *priv, uint8_t *buffer, int nbytes)
{
    media_packer_t *mpacker = (media_packer_t *)priv;
    struct file_packer *packer;
    int ret;

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
    ret = os_fwrite(buffer, 1, nbytes, packer->stream);
    if (os_ferror(packer->stream) || ret != nbytes) {
        M_LOGE("write failed %d!\n", os_ferror(packer->stream));
        os_mutex_unlock(packer->lock);
        return -1;
    }
    mpacker->size += nbytes;

    os_mutex_unlock(packer->lock);
    return nbytes;
}

static int file_packer_update(void *priv, uint8_t *buffer,
    int nbytes, int pos)
{
    media_packer_t *mpacker = (media_packer_t *)priv;
    struct file_packer *packer;
    int file_pos;
    int ret;

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
    file_pos = os_ftell(packer->stream);
    if (file_pos < 0)
        file_pos = mpacker->size;
    (void)os_fseek(packer->stream, pos, OS_SEEK_SET);

    ret = os_fwrite(buffer, 1, nbytes, packer->stream);
    if (os_ferror(packer->stream) || ret != nbytes) {
        M_LOGE("write failed %d!\n", os_ferror(packer->stream));
        os_mutex_unlock(packer->lock);
        return -1;
    }
    (void)os_fseek(packer->stream, file_pos, OS_SEEK_SET);

    os_mutex_unlock(packer->lock);
    return 0;
}

static int file_packer_action(void *priv, recorder_action_t action, void *arg)
{
    media_packer_t *mpacker = (media_packer_t *)priv;
    struct file_packer *packer;

    if (!mpacker) {
        M_LOGE("mpacker null !\n");
        return -1;
    }

    packer = mpacker->packer;
    if (!packer) {
        M_LOGE("packer null !\n");
        return -1;
    }

    if (action == RECORDER_STOP)
        M_LOGD("pack size %d\n", mpacker->size);

    return 0;
}

int file_packer_create(media_packer_t *mpacker, char *sink)
{
    struct file_packer *packer;

    if (!mpacker) {
        M_LOGE("mpacker null !\n");
        return -1;
    }

    if (!sink) {
        M_LOGE("sink null !\n");
        return -1;
    }

    if (strncmp(sink, "fs:", strlen("fs:"))) {
        M_LOGE("filename invalid !\n");
        return -1;
    }

    sink += strlen("fs:");

    packer = snd_zalloc(sizeof(struct file_packer), AFM_EXTN);
    if (!packer) {
        M_LOGE("alloc file packer failed !\n");
        return -1;
    }

    packer->stream = os_fopen(sink, "wb+");
    if (OS_FILE_OPEN_FAIL(packer->stream)) {
        M_LOGE("open %s failed !\n", sink);
        snd_free(packer);
        return -1;
    }
    (void)os_fseek(packer->stream, 0, OS_SEEK_SET);

    packer->lock = os_mutex_new();
    mpacker->packer = packer;
    mpacker->pack = file_packer_write;
    mpacker->update = file_packer_update;
    mpacker->action = file_packer_action;

    M_LOGD("file packer create\n");
    return 0;
}

int file_packer_release(media_packer_t *mpacker)
{
    struct file_packer *packer;

    if (!mpacker) {
        M_LOGE("mpacker null !\n");
        return -1;
    }

    packer = mpacker->packer;
    if (!packer) {
        M_LOGE("packer null !\n");
        return -1;
    }

    if (OS_FILE_OPENING(packer->stream)) {
        M_LOGD("file close\n");
        os_fclose(packer->stream);
    }

    os_mutex_free(packer->lock);
    snd_free(packer);
    mpacker->packer = NULL;

    M_LOGD("file packer release\n");
    return 0;
}

