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

#include "uvoice_os.h"
#include "uvoice_config.h"
#include "uvoice_common.h"
#include "uvoice_ringbuffer.h"
#include "uvoice_play.h"
#include "uvoice_format.h"

#include "uvoice_cache.h"


#if (PLAYER_CACHE_TYPE != 0) && (PLAYER_CACHE_TYPE != 1) && \
    (PLAYER_CACHE_TYPE != 2)
#error "cache type unsupport ! please choose from 0, 1, 2"
#endif

#ifdef MUSICBOX_APP
#define NET_CACHE_TASK_PRIORITY        UVOICE_TASK_PRI_HIGHEST
#else
#define NET_CACHE_TASK_PRIORITY        UVOICE_TASK_PRI_HIGHER
#endif

#ifndef PLAYER_CACHE_MEM_SIZE
#define PLAYER_CACHE_MEM_SIZE        40
#endif

#define NET_CACHE_REBUILD_ENABLE        1

#define NET_CACHE_FILE_ENABLE            1


static int net_cache_file(net_cache_t *nc)
{
#if NET_CACHE_FILE_ENABLE
    struct cache_file *cache;
    int load_size = 0;
    int rd_ret = 0;
    int ret;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
    nc->cache_running = 1;
    cache = nc->cache;
    if (!cache) {
        M_LOGE("cache null !\n");
        os_mutex_unlock(nc->lock);
        return -1;
    }

    if (cache->fp) {
        os_fclose(cache->fp);
        cache->fp = NULL;
        M_LOGD("cache file close\n");
    }

    cache->fp = os_fopen(nc->cache_config.file_path, "wb+");
    if (OS_FILE_OPEN_FAIL(cache->fp)) {
        M_LOGE("open %s failed !\n", nc->cache_config.file_path);
        nc->cache_running = 0;
        os_mutex_unlock(nc->lock);
        return -1;
    }
    M_LOGD("cache file open\n");

    cache->wr_pos = 0;
    (void)os_fseek(cache->fp, cache->wr_pos, OS_SEEK_SET);

    if (nc->head_data_size > 0) {
        ret = os_fwrite(nc->buffer, 1, nc->head_data_size, cache->fp);
        if (os_ferror(cache->fp) || ret != nc->head_data_size) {
            M_LOGE("write head data failed %d!\n",
                os_ferror(cache->fp));
            nc->cache_running = 0;
            os_mutex_unlock(nc->lock);
            return -1;
        }
        cache->wr_pos += nc->head_data_size;
        nc->load_length += nc->head_data_size;
        nc->head_data_size = 0;
    }

    load_size = MIN(nc->buffer_size,
        nc->content_length - cache->wr_pos);

    M_LOGD("start\n");

    os_mutex_unlock(nc->lock);

    while (1) {
        rd_ret = nc->cache_load(nc->priv, nc->buffer, load_size);
        if (rd_ret < 0) {
            M_LOGE("load failed !\n");
            nc->cache_cplt = 1;
            break;
        } else if (rd_ret == 0) {
            M_LOGD("load end\n");
            nc->cache_cplt = 1;
            break;
        } else if (rd_ret != load_size) {
            M_LOGW("load %d ret %d\n", load_size, rd_ret);
        }

        os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

        (void)os_fseek(cache->fp, cache->wr_pos, OS_SEEK_SET);
        ret = os_fwrite(nc->buffer, 1, rd_ret, cache->fp);
        if (os_ferror(cache->fp) || ret != rd_ret) {
            M_LOGE("cache wr failed %d!\n", os_ferror(cache->fp));
            nc->cache_cplt = 1;
            os_mutex_unlock(nc->lock);
            break;
        }
        cache->wr_pos += rd_ret;

        if (cache->rd_waiting &&
            cache->wr_pos - cache->rd_pos >= cache->rd_len) {
            os_sem_signal(cache->rd_sem);
            cache->rd_waiting = 0;
        }

        nc->load_length += rd_ret;
        if (nc->cache_stop) {
            if (cache->rd_waiting) {
                os_sem_signal(cache->rd_sem);
                cache->rd_waiting = 0;
            }
            nc->cache_stop = 0;
            nc->cache_cplt = 1;
            os_mutex_unlock(nc->lock);
            M_LOGD("cache stop\n");
            break;
        }

        load_size = MIN(load_size,
            nc->content_length - nc->load_length);

        if (load_size <= 0) {
            if (!nc->sequence) {
                M_LOGD("load end\n");
                nc->cache_cplt = 1;
                os_mutex_unlock(nc->lock);
                break;
            } else {
                M_LOGD("request next segment, wr_pos %u load_length %u\n",
                    cache->wr_pos, nc->load_length);
                os_mutex_unlock(nc->lock);
                if (nc->event(nc->priv, CACHE_EV_CACHE_CPLT, NULL)) {
                    M_LOGD("request next segment failed !\n");
                    nc->cache_cplt = 1;
                    break;
                }
                os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

                if (nc->head_data_size > 0) {
                    ret = os_fwrite(nc->buffer,
                        1, nc->head_data_size, cache->fp);
                    if (os_ferror(cache->fp) || ret != nc->head_data_size) {
                        M_LOGE("write head data failed %d!\n",
                            os_ferror(cache->fp));
                        if (nc->cplt_waiting) {
                            nc->cplt_waiting = 0;
                            os_sem_signal(nc->cplt_sem);
                        }
                        nc->cache_running = 0;
                        os_mutex_unlock(nc->lock);
                        return -1;
                    }
                    cache->wr_pos += nc->head_data_size;
                    nc->load_length += nc->head_data_size;
                    M_LOGD("cache seq head data %u\n",
                        nc->head_data_size);
                    M_LOGD("cache next segment, wr_pos %u load_length %u\n",
                        cache->wr_pos, nc->load_length);
                    nc->head_data_size = 0;
                }

                load_size = MIN(nc->buffer_size,
                    nc->content_length - nc->load_length);
                if (load_size <= 0) {
                    M_LOGD("load end\n");
                    nc->cache_cplt = 1;
                    os_mutex_unlock(nc->lock);
                    break;
                }
            }
        }
        os_mutex_unlock(nc->lock);
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    M_LOGD("complete %d/%d\n",
        nc->load_length, nc->content_length);
    if (!nc->sequence)
        nc->event(nc->priv, CACHE_EV_CACHE_CPLT, NULL);
    if (nc->cplt_waiting) {
        nc->cplt_waiting = 0;
        os_sem_signal(nc->cplt_sem);
    }

    nc->cache_running = 0;
    os_mutex_unlock(nc->lock);
    return 0;

#else
    return -1;
#endif
}

static int net_cache_file_read(void *priv, unsigned char *buffer, int nbytes)
{
#if NET_CACHE_FILE_ENABLE
    net_cache_t *nc = (net_cache_t *)priv;
    struct cache_file *cache;
    int ret = 0;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    cache = nc->cache;
    if (!cache) {
        M_LOGE("cache null !\n");
        os_mutex_unlock(nc->lock);
        return -1;
    }

    if (!cache->rd_start) {
        cache->rd_len = MIN(nbytes * 10, nc->content_length);
        cache->rd_start = 1;
        M_LOGD("read start\n");
    } else {
        cache->rd_len = nbytes;
    }

    if (nc->seek_offset != 0) {
        if (nc->seek_offset > 0) {
            if (nc->seek_offset <= cache->wr_pos - cache->rd_pos) {
                cache->rd_pos += nc->seek_offset;
                nc->event(nc->priv, CACHE_EV_SEEK_DONE,
                    (void *)nc->seek_offset);
                M_LOGD("seek %d bytes\n", nc->seek_offset);
            }
        } else {
            if (abs(nc->seek_offset) <= cache->rd_pos) {
                cache->rd_pos += nc->seek_offset;
                nc->event(nc->priv, CACHE_EV_SEEK_DONE,
                    (void *)nc->seek_offset);
                M_LOGD("seek %d bytes\n", nc->seek_offset);
            }
        }
        nc->seek_offset = 0;
    }

    while (cache->wr_pos <= cache->rd_pos ||
        cache->wr_pos - cache->rd_pos < cache->rd_len ||
        !cache->fp) {
        if (nc->cache_cplt) {
            M_LOGD("cache cplt, stop waiting\n");
            break;
        }

        M_LOGD("wait cache, rd_len %u rd_pos %u wr_pos %u\n",
            cache->rd_len, cache->rd_pos, cache->wr_pos);
        cache->rd_waiting = 1;
        os_mutex_unlock(nc->lock);

        if (os_sem_wait(cache->rd_sem, 30000)) {
            os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
            M_LOGE("wait timeout ! rd_len %u rd_pos %u wr_pos %u\n",
                cache->rd_len, cache->rd_pos, cache->wr_pos);
            cache->rd_waiting = 0;
            goto __exit;
        }
        os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
        cache->rd_waiting = 0;
        M_LOGD("wait exit, rd_len %u rd_pos %u wr_pos %u\n",
            cache->rd_len, cache->rd_pos, cache->wr_pos);
    }

    if (!cache->fp) {
        M_LOGE("cache file not open !\n");
        os_mutex_unlock(nc->lock);
        return -1;
    }

    if (nc->cache_cplt) {
        if (os_feof(cache->fp) || cache->rd_pos >= cache->wr_pos) {
            nbytes = 0;
            M_LOGD("read end\n");
            goto __exit;
        } else if (cache->wr_pos - cache->rd_pos < nbytes) {
            nbytes = cache->wr_pos - cache->rd_pos;
            M_LOGD("read tail %d\n", nbytes);
        }
    }

    (void)os_fseek(cache->fp, cache->rd_pos, OS_SEEK_SET);
    ret = os_fread(buffer, 1, nbytes, cache->fp);
    if (os_ferror(cache->fp) || ret != nbytes) {
        M_LOGE("read failed %d!\n", os_ferror(cache->fp));
        os_mutex_unlock(nc->lock);
        return -1;
    }

    cache->rd_pos += nbytes;
    ret = nbytes;

__exit:
    os_mutex_unlock(nc->lock);
    return ret;

#else
    return -1;
#endif
}

static int net_cache_file_mediainfo(struct cache_file *cache,
        media_info_t *info, media_format_t format)
{
#if NET_CACHE_FILE_ENABLE
    if (!cache) {
        M_LOGW("cache not enable\n");
        return -1;
    }

    if (!cache->fp) {
        M_LOGE("cache file not open !\n");
        return -1;
    }

    if (format == MEDIA_FMT_MP3) {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        (void)os_fseek(cache->fp, -128, OS_SEEK_END);
        int ret = os_fread(buffer, 1, 128, cache->fp);
        if (ret != 128) {
            M_LOGE("read failed %d!\n", ret);
            return -1;
        }

        mp3_id3v1_parse(info, buffer, 128);
    }

    return 0;

#else
    return -1;
#endif
}


static int net_cache_file_reset(struct cache_file *cache)
{
#if NET_CACHE_FILE_ENABLE
    if (!cache) {
        M_LOGE("cache null !\n");
        return -1;
    }

    if (cache->fp) {
        os_fclose(cache->fp);
        cache->fp = NULL;
        M_LOGD("cache file close\n");
    }

    cache->rd_pos = 0;
    cache->wr_pos = 0;
    cache->rd_waiting = 0;
    cache->rd_start = 0;
    cache->rd_len = 0;

    M_LOGD("file cache reset\n");
    return 0;

#else
    return -1;
#endif
}

static int net_cache_file_init(net_cache_t *nc)
{
#if NET_CACHE_FILE_ENABLE
    struct cache_file *cache = snd_zalloc(
            sizeof(struct cache_file), AFM_EXTN);
    if (!cache) {
        M_LOGE("alloc cache file failed !\n");
        return -1;
    }

    cache->rd_sem = os_sem_new(0);
    nc->cache_read = net_cache_file_read;
    nc->cache = cache;

    return 0;

#else
    return -1;
#endif
}

static int net_cache_file_deinit(net_cache_t *nc)
{
#if NET_CACHE_FILE_ENABLE
    struct cache_file *cache;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    cache = nc->cache;
    if (!cache) {
        M_LOGE("cache null !\n");
        return -1;
    }

    net_cache_file_reset(cache);
    os_sem_free(cache->rd_sem);
    snd_free(cache);
    nc->cache_read = NULL;
    nc->cache = NULL;

    return 0;

#else
    return -1;
#endif
}

static int net_cache_mem(net_cache_t *nc)
{
    struct cache_mem *cache;
    int load_size;
    int rd_ret = 0;
    int wr_ret = 0;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    nc->cache_running = 1;

    cache = nc->cache;
    if (!cache) {
        M_LOGE("cache null !\n");
        os_mutex_unlock(nc->lock);
        return -1;
    }

    if (nc->head_data_size > 0) {
        if (uvoice_ringbuff_freesize(&cache->rb) < nc->head_data_size) {
            if (cache->rd_waiting) {
                os_sem_signal(cache->rd_sem);
                cache->rd_waiting = 0;
            }

            cache->wr_len = nc->head_data_size;
            cache->wr_waiting = 1;
            os_mutex_unlock(nc->lock);

            os_sem_wait(cache->wr_sem, OS_WAIT_FOREVER);

            os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
            cache->wr_waiting = 0;

            if (nc->cache_stop) {
                nc->cache_stop = 0;
                nc->cache_cplt = 1;
                M_LOGD("cache stop\n");
                os_mutex_unlock(nc->lock);
                goto __exit;
            }
        }
        wr_ret = uvoice_ringbuff_fill(&cache->rb,
            nc->buffer, nc->head_data_size);
        if (wr_ret < 0) {
            M_LOGE("fill head data %u failed !\n",
                    nc->head_data_size);
            nc->cache_running = 0;
            os_mutex_unlock(nc->lock);
            return -1;
        } else if (wr_ret != nc->head_data_size) {
            M_LOGW("cache head data %u ret %d\n",
                nc->buffer_size, wr_ret);
        }
        nc->load_length += nc->head_data_size;
        nc->head_data_size = 0;
    }

    load_size = MIN(nc->buffer_size,
            nc->content_length - nc->load_length);

    if (nc->load_length >= nc->content_length) {
        nc->cache_cplt = 1;
        goto __exit;
    }

    M_LOGD("start\n");

    os_mutex_unlock(nc->lock);

    while (1) {
        rd_ret = nc->cache_load(nc->priv, nc->buffer + nc->offset,
            load_size);
        if (rd_ret < 0) {
            M_LOGE("load failed !\n");
            nc->cache_cplt = 1;
            nc->load_length -= nc->offset;
            break;
        } else if (rd_ret == 0) {
            M_LOGD("load end\n");
            nc->cache_cplt = 1;
            nc->load_length -= nc->offset;
            break;
        } else if (rd_ret != load_size) {
            M_LOGW("load %d ret %u\n", load_size, rd_ret);
        }

        os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

        if (nc->cache_stop) {
            nc->cache_stop = 0;
            nc->cache_cplt = 1;
            nc->load_length -= nc->offset;
            M_LOGD("cache stop\n");
            os_mutex_unlock(nc->lock);
            goto __exit;
        }

        while (uvoice_ringbuff_freesize(&cache->rb) <
            rd_ret + nc->offset) {
            if (cache->rd_waiting) {
                os_sem_signal(cache->rd_sem);
                cache->rd_waiting = 0;
            }

            cache->wr_len = rd_ret + nc->offset;
            cache->wr_waiting = 1;
            os_mutex_unlock(nc->lock);

            os_sem_wait(cache->wr_sem, OS_WAIT_FOREVER);

            os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
            cache->wr_waiting = 0;

            if (nc->cache_stop) {
                nc->cache_stop = 0;
                nc->cache_cplt = 1;
                nc->load_length -= nc->offset;
                M_LOGD("cache stop\n");
                os_mutex_unlock(nc->lock);
                goto __exit;
            }
        }

        wr_ret = uvoice_ringbuff_fill(&cache->rb,
                nc->buffer, rd_ret + nc->offset);
        if (wr_ret < 0) {
            M_LOGE("fill failed %d!\n", wr_ret);
            nc->cache_cplt = 1;
            nc->load_length -= nc->offset;
            os_mutex_unlock(nc->lock);
            break;
        } else if (wr_ret != rd_ret + nc->offset) {
            M_LOGW("fill %u ret %d\n", rd_ret + nc->offset, wr_ret);
        }

        nc->load_length += rd_ret;

        if (nc->offset > 0)
            nc->offset = 0;

        if (cache->rd_waiting &&
            uvoice_ringbuff_dirtysize(&cache->rb) >= cache->rd_len) {
            cache->rd_waiting = 0;
            os_sem_signal(cache->rd_sem);
        }

        if (nc->cache_stop) {
            nc->cache_stop = 0;
            nc->cache_cplt = 1;
            M_LOGD("stop\n");
            os_mutex_unlock(nc->lock);
            break;
        }

        load_size = MIN(nc->buffer_size,
            nc->content_length - nc->load_length);

        if (load_size <= 0) {
            if (!nc->sequence) {
                M_LOGD("load end\n");
                nc->cache_cplt = 1;
                os_mutex_unlock(nc->lock);
                break;
            } else {
                M_LOGD("request next segment\n");
                os_mutex_unlock(nc->lock);
                if (nc->event(nc->priv, CACHE_EV_CACHE_CPLT, NULL)) {
                    M_LOGD("request next segment failed !\n");
                    nc->cache_cplt = 1;
                    break;
                }
                os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

                load_size = MIN(nc->buffer_size,
                    nc->content_length - nc->load_length);
                if (load_size <= 0) {
                    M_LOGD("load end\n");
                    nc->cache_cplt = 1;
                    os_mutex_unlock(nc->lock);
                    break;
                }

                if (nc->head_data_size > 0) {
                    nc->offset = nc->head_data_size;
                    load_size -= nc->offset;
                    nc->load_length += nc->offset;
                    M_LOGD("cache head data %u\n", nc->offset);
                    nc->head_data_size = 0;
                }
            }
        }

        os_mutex_unlock(nc->lock);
    }

__exit:
    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
    M_LOGD("complete %d/%d\n",
        nc->load_length, nc->content_length);
    if (cache->rd_waiting) {
        M_LOGD("wake rd wait\n");
        cache->rd_waiting = 0;
        os_sem_signal(cache->rd_sem);
    }
    if (nc->cplt_waiting) {
        nc->cplt_waiting = 0;
        os_sem_signal(nc->cplt_sem);
    }

    nc->cache_running = 0;
    os_mutex_unlock(nc->lock);
    return 0;
}

static int net_cache_mem_read(void *priv, unsigned char *buffer,
    int nbytes)
{
    net_cache_t *nc = (net_cache_t *)priv;
    struct cache_mem *cache;
    int ret = 0;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    if (nbytes <= 0)
        goto __exit;

    cache = nc->cache;
    if (!cache) {
        M_LOGE("cache null !\n");
        ret = -1;
        goto __exit;
    }

    if (!cache->rd_start) {
        #ifdef UVOICE_IOTNLP_ENABLE
        cache->rd_len = nbytes;
        #else
        if (nc->cache_cplt)
            cache->rd_len = MIN(cache->pool_size,
                MIN(nbytes, nc->content_length));
        else
            cache->rd_len = MIN(cache->pool_size,
                MIN(nbytes * 4, nc->content_length - nc->load_length));
        if (nbytes > cache->rd_len) {
            nbytes = cache->rd_len;
            M_LOGD("update read bytes %d\n", nbytes);
        }
        #endif
        cache->rd_start = 1;
    } else {
        cache->rd_len = nbytes;
    }

    if (nc->cache_cplt) {
        if (uvoice_ringbuff_dirtysize(&cache->rb) <= 0) {
            M_LOGD("data end\n");
            goto __exit;
        } else if (uvoice_ringbuff_dirtysize(&cache->rb) < nbytes) {
            nbytes = uvoice_ringbuff_dirtysize(&cache->rb);
            cache->rd_len = nbytes;
            M_LOGD("cache cplt, read %d\n", nbytes);
        }
    }

    if (nc->seek_offset != 0) {
        if (nc->seek_offset > 0) {
            if (nc->seek_offset <= uvoice_ringbuff_dirtysize(&cache->rb)) {
                uvoice_ringbuff_drop(&cache->rb, nc->seek_offset);
                nc->event(nc->priv, CACHE_EV_SEEK_DONE,
                    (void *)nc->seek_offset);
            }
        } else {
            if (abs(nc->seek_offset) <= uvoice_ringbuff_freesize(&cache->rb)) {
                uvoice_ringbuff_back(&cache->rb, abs(nc->seek_offset));
                nc->event(nc->priv, CACHE_EV_SEEK_DONE,
                    (void *)nc->seek_offset);
            }
        }
        M_LOGD("seek %d bytes\n", nc->seek_offset);
        nc->seek_offset = 0;
    }

    if (uvoice_ringbuff_dirtysize(&cache->rb) < cache->rd_len) {
        cache->rd_waiting = 1;
        if (cache->wr_waiting) {
            os_sem_signal(cache->wr_sem);
            cache->wr_waiting = 0;
        }

        if (cache->rd_len == nbytes && !nc->sequence)
            cache->rd_len = MIN(cache->pool_size / 2,
                nc->content_length - nc->load_length);

        if (cache->rd_len <= 0) {
            M_LOGD("content end\n");
            cache->rd_waiting = 0;
            goto __exit;
        }
        nbytes = MIN(nbytes, cache->rd_len);
        nc->event(nc->priv, CACHE_EV_READ_BLOCK, (void *)1);

        M_LOGD("wait cache, rd_len %u dirtysize %d freesize %d\n",
            cache->rd_len,
            uvoice_ringbuff_dirtysize(&cache->rb),
            uvoice_ringbuff_freesize(&cache->rb));
        os_mutex_unlock(nc->lock);

        if (os_sem_wait(cache->rd_sem, 60000)) {
            os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
            M_LOGE("wait timeout ! rd_len %u dirtysize %d freesize %d\n",
                cache->rd_len,
                uvoice_ringbuff_dirtysize(&cache->rb),
                uvoice_ringbuff_freesize(&cache->rb));
            cache->rd_waiting = 0;
            goto __exit;
        }
        os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
        M_LOGD("wait exit, rd_len %u dirtysize %d freesize %d\n",
            cache->rd_len,
            uvoice_ringbuff_dirtysize(&cache->rb),
            uvoice_ringbuff_freesize(&cache->rb));
        nc->event(nc->priv, CACHE_EV_READ_BLOCK, 0);
        cache->rd_waiting = 0;
    }

    if (nc->cache_cplt) {
        if (uvoice_ringbuff_dirtysize(&cache->rb) <= 0) {
            M_LOGD("read end\n");
            goto __exit;
        } else if (uvoice_ringbuff_dirtysize(&cache->rb) <
                nbytes) {
            nbytes = uvoice_ringbuff_dirtysize(&cache->rb);
            cache->rd_len = nbytes;
            M_LOGD("cache cplt, read %d\n", nbytes);
        }
    }

    ret = uvoice_ringbuff_read(&cache->rb, buffer, nbytes);
    if (ret < 0) {
        M_LOGE("read %d failed %d!\n", nbytes, ret);
        ret = -1;
        goto __exit;
    } else if (ret != nbytes) {
        M_LOGW("read %d ret %d\n", nbytes, ret);
    }

    if (cache->wr_waiting &&
        uvoice_ringbuff_freesize(&cache->rb) >= cache->wr_len) {
        os_sem_signal(cache->wr_sem);
        cache->wr_waiting = 0;
    }

__exit:
    os_mutex_unlock(nc->lock);
    return ret;
}

static int net_cache_mem_reset(struct cache_mem *cache)
{
    if (!cache) {
        M_LOGE("cache null !\n");
        return -1;
    }

    cache->rd_start = 0;
    cache->wr_waiting = 0;
    cache->rd_waiting = 0;
    cache->rd_len = 0;
    cache->wr_len = 0;
    memset(cache->pool, 0, cache->pool_size);
    uvoice_ringbuff_reset(&cache->rb);

    M_LOGD("mem cache reset\n");
    return 0;
}

static int net_cache_mem_init(net_cache_t *nc)
{
    struct cache_mem *cache;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    cache = snd_zalloc(sizeof(struct cache_mem), AFM_EXTN);
    if (!cache) {
        M_LOGE("alloc cache mem failed !\n");
        return -1;
    }

    cache->pool_size = nc->cache_config.mem_size * 1024;
    if (nc->sequence && nc->cache_config.mem_size > 40)
        cache->pool_size = 40 * 1024;

    cache->pool = snd_zalloc(cache->pool_size + 4, AFM_EXTN);
    if (!cache->pool) {
        M_LOGE("alloc cache pool failed !\n");
        snd_free(cache);
        return -1;
    }

    M_LOGD("alloc cache pool %u\n", cache->pool_size);

    if (uvoice_ringbuff_init(&cache->rb,
            cache->pool, cache->pool_size)) {
        M_LOGE("init ring buffer failed !\n");
        snd_free(cache->pool);
        snd_free(cache);
        return -1;
    }

    cache->wr_sem = os_sem_new(0);
    cache->rd_sem = os_sem_new(0);
    nc->cache_read = net_cache_mem_read;
    nc->cache = cache;

    return 0;
}

static int net_cache_mem_deinit(net_cache_t *nc)
{
    struct cache_mem *cache;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    cache = nc->cache;
    if (!cache) {
        M_LOGE("cache null !\n");
        return -1;
    }

    os_sem_free(cache->wr_sem);
    os_sem_free(cache->rd_sem);
    snd_free(cache->pool);
    snd_free(cache);
    nc->cache_read = NULL;
    nc->cache = NULL;

    return 0;
}

static void net_cache_task(void *arg)
{
    net_cache_t *nc = (net_cache_t *)arg;
    if (!nc) {
        M_LOGE("nc null !\n");
        return;
    }

    if (nc->cache_config.place == CACHE_FILE)
        net_cache_file(nc);
    else if (nc->cache_config.place == CACHE_MEM)
        net_cache_mem(nc);
}


int net_get_cacheinfo(net_cache_t *nc, cache_info_t *info)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (!info) {
        M_LOGE("info null !\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    if (nc->cache_config.place == CACHE_FILE) {
        struct cache_file *cache = nc->cache;
        info->dirty_cache_size = cache->rd_pos;
        info->avail_cache_size = cache->wr_pos - cache->rd_pos;
    } else if (nc->cache_config.place == CACHE_MEM) {
        struct cache_mem *cache = nc->cache;
        info->dirty_cache_size = 0;
        info->avail_cache_size =
            uvoice_ringbuff_dirtysize(&cache->rb);
    }

    os_mutex_unlock(nc->lock);
    return 0;
}

int net_get_mediainfo(net_cache_t *nc,
        media_info_t *info, media_format_t format)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (!info) {
        M_LOGE("info null !\n");
        return -1;
    }

    os_mutex_lock(nc->lock, OS_WAIT_FOREVER);

    switch (nc->cache_config.place) {
        case CACHE_FILE:
        if (!nc->cache_cplt)
            break;

        if (net_cache_file_mediainfo(nc->cache,
                info, format)) {
            M_LOGE("get file mediainfo failed !\n");
            os_mutex_unlock(nc->lock);
            return -1;
        }
        break;
        default:
        break;
    }

    os_mutex_unlock(nc->lock);
    return 0;
}

int net_cache_reset(net_cache_t *nc)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (nc->cache_config.place == CACHE_NONE) {
        M_LOGD("cache disabled\n");
        goto __exit;
    }

    if (!nc->cache_cplt && nc->cache_running) {
        M_LOGE("cache not cplt !\n");
        return -1;
    }

    nc->cache_running = 0;
    nc->load_length = 0;
    nc->cache_cplt = 0;
    nc->seek_offset = 0;
    nc->rebuild = 0;

    switch (nc->cache_config.place) {
        case CACHE_FILE:
        if (net_cache_file_reset(nc->cache)) {
            M_LOGE("reset file cache failed !\n");
            return -1;
        }
        break;
        case CACHE_MEM:
        if (net_cache_mem_reset(nc->cache)) {
            M_LOGE("reset mem cache failed !\n");
            return -1;
        }
        break;
        default:
        break;
    }

__exit:
    return 0;
}

int net_cache_seek(net_cache_t *nc, int offset)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (offset != 0) {
        os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
        nc->seek_offset = offset;
        os_mutex_unlock(nc->lock);
    }
    return 0;
}

int net_cache_start(net_cache_t *nc)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (nc->load_length >= nc->content_length) {
        M_LOGD("load cplt, ignore\n");
        nc->cache_cplt = 1;
        goto __exit;
    }

    if (nc->cache_config.place != CACHE_NONE) {
        nc->cache_cplt = 0;
        os_task_create(&nc->cache_task, "uvoice_cache_task",
            net_cache_task, nc,
            2048, NET_CACHE_TASK_PRIORITY);
    }

__exit:
    return 0;
}

int net_cache_pause(net_cache_t *nc)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (nc->cache_config.place == CACHE_MEM ||
        nc->cache_config.place == CACHE_NONE) {
        M_LOGD("rebuild cache\n");
        nc->rebuild = NET_CACHE_REBUILD_ENABLE;

        /* sequece stream rebuild forcibly */
        if (nc->sequence)
            nc->rebuild = 1;
    }

    return 0;
}

int net_cache_resume(net_cache_t *nc)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    return 0;
}

int net_cache_stop(net_cache_t *nc)
{
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (!nc->cache_running) {
        M_LOGD("cache not running, ignore\n");
        goto __exit;
    }

    if (nc->cache_cplt) {
        M_LOGD("cache cplt already, ignore\n");
        goto __exit;
    }

    if (nc->cache_config.place != CACHE_NONE) {
        os_mutex_lock(nc->lock, OS_WAIT_FOREVER);
        if (!nc->cache_cplt) {
            M_LOGD("wait cache cplt\n");
            if (nc->cache_config.place == CACHE_MEM) {
                struct cache_mem *cache = nc->cache;
                nc->cache_stop = 1;
                if (cache->wr_waiting) {
                    M_LOGD("wake wr waiting\n");
                    os_sem_signal(cache->wr_sem);
                    cache->wr_waiting = 0;
                }
                cache->rd_start = 0;
            } else if (nc->cache_config.place == CACHE_FILE) {
                struct cache_file *cache = nc->cache;
                nc->cache_stop = 1;
                cache->rd_start = 0;
            }
            nc->cplt_waiting = 1;
            os_mutex_unlock(nc->lock);
            if (os_sem_wait(nc->cplt_sem, 15000)) {
                nc->cplt_waiting = 0;
                M_LOGW("wait cache cplt timeout !\n");
            }
            M_LOGD("cache cplt and stop\n");
        } else {
            nc->cache_cplt = 0;
            os_mutex_unlock(nc->lock);
        }
    }

__exit:
    return 0;
}

net_cache_t *net_cache_create(int read_size,
    cache_config_t *config, bool sequence)
{
    net_cache_t *nc;

    nc = snd_zalloc(sizeof(net_cache_t), AFM_EXTN);
    if (!nc) {
        M_LOGE("alloc net cache failed !\n");
        return NULL;
    }

    memcpy(&nc->cache_config, config, sizeof(cache_config_t));
    nc->buffer_size = read_size;
    nc->buffer = snd_zalloc(nc->buffer_size, AFM_MAIN);
    if (!nc->buffer) {
        M_LOGE("alloc buffer failed !\n");
        snd_free(nc);
        return NULL;
    }
    nc->lock = os_mutex_new();
    nc->cplt_sem = os_sem_new(0);
    if (sequence)
        nc->sequence = 1;

    if (nc->cache_config.place == CACHE_FILE) {
        if (net_cache_file_init(nc)) {
            M_LOGE("init file cache failed !\n");
            snd_free(nc->buffer);
            snd_free(nc);
            return NULL;
        }
    } else if (nc->cache_config.place == CACHE_MEM) {
        if (net_cache_mem_init(nc)) {
            M_LOGE("init mem cache failed !\n");
            snd_free(nc->buffer);
            snd_free(nc);
            return NULL;
        }
    }

    M_LOGD("net cache create\n");
    return nc;
}

int net_cache_release(net_cache_t *nc)
{
    int ret = 0;

    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (nc->cache_config.place == CACHE_FILE)
        ret = net_cache_file_deinit(nc);
    else if (nc->cache_config.place == CACHE_MEM)
        ret = net_cache_mem_deinit(nc);
    if (ret) {
        M_LOGE("cache deinit failed !\n");
        return -1;
    }

    os_sem_free(nc->cplt_sem);
    os_mutex_free(nc->lock);
    snd_free(nc->buffer);
    snd_free(nc);

    M_LOGD("net cache release\n");
    return 0;
}

