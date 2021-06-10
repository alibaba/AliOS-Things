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

#include "uvoice_config.h"
#include "uvoice_common.h"
#include "uvoice_play.h"

#include "uvoice_cache.h"


int net_download_work(net_cache_t *nc)
{
    int filepath_len;
    char *filepath;
    os_file_t fp;
    int rd_ret = 0;
    int load_size;
    int i = 0;
    int ret;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    ret = os_access(PLAYER_SOURCE_DLOAD_DIR, OS_F_OK);
    if (ret != 0) {
        ret = os_mkdir(PLAYER_SOURCE_DLOAD_DIR);
        if (ret) {
            M_LOGE("create %s failed %d!\n",
                    PLAYER_SOURCE_DLOAD_DIR, ret);
            os_mutex_unlock(nc->lock);
            return -1;
        }
        M_LOGD("create dir: %s\n", PLAYER_SOURCE_DLOAD_DIR);
    }

    filepath_len = strlen(PLAYER_SOURCE_DLOAD_DIR) +
        strlen(nc->filename) + 4;
    filepath = snd_zalloc(filepath_len, AFM_EXTN);
    if (!filepath) {
        M_LOGE("alloc file path failed !\n");
        os_mutex_unlock(nc->lock);
        return -1;
    }

    snprintf(filepath, filepath_len, "%s/%s",
        PLAYER_SOURCE_DLOAD_DIR, nc->filename);

    M_LOGI("download to %s\n", filepath);
    ret = os_access(filepath, OS_W_OK | OS_R_OK | OS_F_OK);
    if (ret == 0) {
        M_LOGW("file exist\n");
        snd_free(filepath);
        os_mutex_unlock(nc->lock);
        goto __exit;
    }

    fp = os_fopen(filepath, "wb+");
    if (OS_FILE_OPEN_FAIL(fp)) {
        M_LOGE("open %s failed !\n", filepath);
        snd_free(filepath);
        os_mutex_unlock(nc->lock);
        return -1;
    }

    if (nc->head_data_size > 0) {
        ret = os_fwrite(nc->buffer, 1, nc->head_data_size, fp);
        if (os_ferror(fp) || ret != nc->head_data_size) {
            M_LOGE("write head data failed %d!\n", os_ferror(fp));
            os_fclose(fp);
            snd_free(filepath);
            nc->head_data_size = 0;
            os_mutex_unlock(nc->lock);
            return -1;
        }
        M_LOGD("save head data %u\n", nc->head_data_size);
        nc->load_length += nc->head_data_size;
        nc->head_data_size = 0;
    }

    load_size = MIN(nc->buffer_size,
        nc->content_length - nc->load_length);

    nc->download = 1;

    os_mutex_unlock(nc->lock);

    while (load_size > 0) {
        rd_ret = nc->cache_load(nc->priv, nc->buffer, load_size);
        if (rd_ret < 0) {
            M_LOGE("read failed !\n");
            break;
        } else if (rd_ret == 0) {
            M_LOGD("read end\n");
            break;
        }

        os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
        ret = os_fwrite(nc->buffer, 1, rd_ret, fp);
        if (os_ferror(fp) || ret != rd_ret) {
            M_LOGE("write failed %d!\n", os_ferror(fp));
            os_mutex_unlock(nc->lock);
            break;
        }
        nc->load_length += rd_ret;
        load_size = MIN(load_size,
            nc->content_length - nc->load_length);
        if (++i == 50) {
            M_LOGI("download %d/%d\n",
                nc->load_length, nc->content_length);
            i = 0;
        }

        if (!nc->download) {
            M_LOGI("download abort\n");
            os_mutex_unlock(nc->lock);
            break;
        }
        os_mutex_unlock(nc->lock);
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    os_fclose(fp);

    if (!nc->download) {
        ret = os_remove(filepath);
        if (ret)
            M_LOGE("delete failed %d!\n", ret);
        else
            M_LOGD("file delete\n");
    } else {
        M_LOGI("download %d/%d\n",
            nc->load_length, nc->content_length);
        M_LOGI("download complete\n");
    }

    snd_free(filepath);
    if (nc->cplt_waiting) {
        nc->cplt_waiting = 0;
        os_sem_signal(nc->cplt_sem);
    }
    os_mutex_unlock(nc->lock);

__exit:
    return 0;
}

int new_download_abort(net_cache_t *nc)
{
    if (!nc) {
        M_LOGW("nc null\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
    if (nc->download) {
        nc->download = 0;
        nc->cplt_waiting = 1;
        M_LOGD("wait cplt\n");
        os_mutex_unlock(nc->lock);
        if (os_sem_wait(nc->cplt_sem, 15000)) {
            M_LOGE("wait cplt timeout !\n");
            nc->cplt_waiting = 0;
            return -1;
        }

        M_LOGD("download abort\n");
    } else {
        M_LOGW("no download\n");
        os_mutex_unlock(nc->lock);
    }
    return 0;
}

net_cache_t *net_download_create(void)
{
    net_cache_t *nc = snd_zalloc(sizeof(net_cache_t), AFM_EXTN);
    if (!nc) {
        M_LOGE("alloc net cache failed !\n");
        return NULL;
    }

    nc->cache_config.place = CACHE_NONE;
    nc->buffer_size = 2048;
    nc->buffer = snd_zalloc(nc->buffer_size, AFM_MAIN);
    if (!nc->buffer) {
        M_LOGE("alloc buffer failed !\n");
        snd_free(nc);
        return NULL;
    }

    nc->lock = os_mutex_new();
    nc->cplt_sem = os_sem_new(0);
    return nc;
}

int net_download_release(net_cache_t *nc)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    os_sem_free(nc->cplt_sem);
    os_mutex_free(nc->lock);
    snd_free(nc->buffer);
    snd_free(nc);
    return 0;
}

