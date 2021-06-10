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
#include "uvoice_common.h"
#include "uvoice_play.h"

#include "uvoice_cache.h"
#include "uvoice_http.h"
#include "uvoice_hls.h"


//#define HTTP_LIST_FILE_LOAD

#define HTTP_LIVE_STREAM_ENABLE        1


int http_loader_start(http_loader_t *loader);
int http_loader_stop(http_loader_t *loader);

static int hls_index_get(char *desc)
{
    int len;
    char *ptr;
    bool found = false;

    if (!desc) {
        M_LOGE("desc null !\n");
        return -1;
    }

    len = strlen(desc);
    if (len <= 0) {
        M_LOGE("desc invalid !\n");
        return -1;
    }

    ptr = strstr(desc, ".aac");
    if (!ptr) {
        ptr = strstr(desc, ".AAC");
        if (!ptr) {
            M_LOGE("seg info not found !\n");
            return -1;
        }
    }

    for (ptr--; ptr > desc; ptr--) {
        if (*ptr < '0' || *ptr > '9') {
            found = true;
            ptr++;
            break;
        }
    }

    if (!found) {
        M_LOGE("seg index not found !\n");
        return -1;
    }

    return atoi(ptr);
}

int uvoice_hls_index_update(http_loader_t *loader, int index)
{
    hls_t *hls;
    int target_index;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    hls = loader->hls;
    if (!hls) {
        M_LOGE("hls null !\n");
        return -1;
    }

    os_mutex_lock(hls->lock, OS_WAIT_FOREVER);

    if (index < 0 || index >= hls->seg_count) {
        M_LOGE("index %d invalid !\n", index);
        os_mutex_unlock(hls->lock);
        return -1;
    }

    if (index == hls->seg_index) {
        M_LOGD("seg index not change\n");
        goto __exit;
    }

    hls->seg_index = index;

    target_index = index + hls->seg_begin;

#ifdef HTTP_LIST_FILE_LOAD
    if (!hls->fp) {
        M_LOGE("no hls file !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }
    fseek(hls->fp, 0, SEEK_SET);
#else
    if (!hls->content) {
        M_LOGE("no hls content !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }
    hls->pos = hls->content;
#endif

    char item_desc[128];
    char *ptr;
    char *ptr_temp;
    int i = 0;

    while (1) {
#ifdef HTTP_LIST_FILE_LOAD
        if (os_feof(hls->fp)) {
            M_LOGD("hls file end\n");
            break;
        }

        memset(item_desc, 0, sizeof(item_desc));
        os_fgets(item_desc, sizeof(item_desc), hls->fp);
        if (item_desc[strlen(item_desc) - 1] == '\n')
            item_desc[strlen(item_desc) - 1] = '\0';

#else
        if (hls->pos - hls->content >= hls->len) {
            M_LOGD("hls content end\n");
            break;
        }

        ptr = strchr(hls->pos, '\n');
        if (!ptr) {
            M_LOGD("hls end\n");
            break;
        }
        memset(item_desc, 0, sizeof(item_desc));
        memcpy(item_desc, hls->pos, ptr - hls->pos);
        item_desc[ptr - hls->pos] = '\0';
        ptr++;
        hls->pos = ptr;
#endif

        ptr = (char *)item_desc;

        if (!strncmp(ptr, "#EXTINF:", strlen("#EXTINF:"))) {
#ifdef HTTP_LIST_FILE_LOAD
            memset(item_desc, 0, sizeof(item_desc));
            ptr = os_fgets(item_desc, sizeof(item_desc), hls->fp);
            if (!ptr) {
                M_LOGE("read list file failed !\n");
                os_mutex_unlock(hls->lock);
                return -1;
            }
            if (item_desc[strlen(item_desc) - 1] == '\n')
                item_desc[strlen(item_desc) - 1] = '\0';
#else
            ptr = strchr(hls->pos, '\n');
            if (!ptr) {
                M_LOGD("hls end\n");
                break;
            }
            memset(item_desc, 0, sizeof(item_desc));
            memcpy(item_desc, hls->pos, ptr - hls->pos);
            item_desc[ptr - hls->pos] = '\0';
            ptr++;
            hls->pos = ptr;
#endif

            if (i++ < target_index)
                continue;

            ptr = (char *)item_desc;
            // while (*ptr == '/')
            //    ptr++;

            ptr_temp = ptr + strlen(ptr) - 1;
            while (*ptr_temp == '\\') {
                *ptr_temp = '\0';
                ptr_temp--;
            }

            memset(hls->seg_desc, 0, sizeof(hls->seg_desc));
            memcpy(hls->seg_desc, ptr, strlen(ptr) + 1);

            break;
        } else if (!strncmp(item_desc, "#EXT-X-ENDLIST",
                strlen("#EXT-X-ENDLIST"))) {
            M_LOGD("hls end\n");
            break;
        }
    }

    if (strlen(hls->seg_desc) + 1 >
        HTTP_URL_LEN - (hls->sub - loader->url)) {
        M_LOGE("seg desc length overrange !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }
    memset(hls->sub, 0,
        HTTP_URL_LEN - (hls->sub - loader->url));
    memcpy(hls->sub, hls->seg_desc,
        strlen(hls->seg_desc) + 1);
    M_LOGI("seg url %s\n", loader->url);

__exit:
    os_mutex_unlock(hls->lock);
    return 0;
}

int uvoice_hls_file_next(http_loader_t *loader)
{
    hls_t *hls;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    hls = loader->hls;
    if (!hls) {
        M_LOGE("hls null !\n");
        return -1;
    }

    os_mutex_lock(hls->lock, OS_WAIT_FOREVER);

#ifdef HTTP_LIST_FILE_LOAD
    if (!hls->fp) {
        M_LOGE("no hls file !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }

#else
    if (!hls->content) {
        M_LOGE("no hls content !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }
#endif

__repeat:
    hls->seg_index++;

    if (hls->seg_index >= hls->seg_count) {
        M_LOGD("hls file end\n");
        // goto __exit;
        if (strlen(hls->base) > 0) {
            hls->seg_current = hls_index_get(hls->seg_desc);
            M_LOGD("current seg %d\n", hls->seg_current);

            if (http_loader_stop(loader)) {
                M_LOGE("stop http load failed !\n");
                os_mutex_unlock(hls->lock);
                return -1;
            }

            memset(loader->url, 0, sizeof(loader->url));
            memcpy(loader->url, hls->base,
                strlen(hls->base) + 1);

            if (uvoice_hls_build(loader)) {
                M_LOGE("build hls failed !\n");
                os_mutex_unlock(hls->lock);
                return -1;
            }

            goto __skip;
        } else {
            goto __exit;
        }
    }

    char item_desc[128];
    char *ptr;
    char *ptr_temp;

    while (1) {
#ifdef HTTP_LIST_FILE_LOAD
        if (os_feof(hls->fp)) {
            M_LOGD("hls file end\n");
            break;
        }

        memset(item_desc, 0, sizeof(item_desc));
        os_fgets(item_desc, sizeof(item_desc), hls->fp);
        if (item_desc[strlen(item_desc) - 1] == '\n')
            item_desc[strlen(item_desc) - 1] = '\0';

#else
        if (hls->pos - hls->content >= hls->len) {
            M_LOGD("hls content end\n");
            goto __repeat;
        }

        ptr = strchr(hls->pos, '\n');
        if (!ptr) {
            M_LOGD("hls end\n");
            goto __repeat;
        }
        memset(item_desc, 0, sizeof(item_desc));
        memcpy(item_desc, hls->pos, ptr - hls->pos);
        item_desc[ptr - hls->pos] = '\0';
        ptr++;
        hls->pos = ptr;
#endif

        ptr = (char *)item_desc;

        if (!strncmp(ptr, "#EXTINF:", strlen("#EXTINF:"))) {
#ifdef HTTP_LIST_FILE_LOAD
            memset(item_desc, 0, sizeof(item_desc));
            os_fgets(item_desc, sizeof(item_desc), hls->fp);
            if (item_desc[strlen(item_desc) - 1] == '\n')
                item_desc[strlen(item_desc) - 1] = '\0';
#else
            memset(item_desc, 0, sizeof(item_desc));
            ptr = strchr(hls->pos, '\n');
            if (ptr) {
                memcpy(item_desc, hls->pos, ptr - hls->pos);
                item_desc[ptr - hls->pos] = '\0';
            } else {
                memcpy(item_desc, hls->pos,
                    hls->len - (hls->pos - hls->content));
                item_desc[hls->len - (hls->pos - hls->content)] = '\0';
            }

            ptr++;
            hls->pos = ptr;
#endif

            ptr = (char *)item_desc;

            if (!strncmp(hls->seg_desc, ptr, strlen(ptr) + 1)) {
                M_LOGW("seg desc no change\n");
                goto __repeat;
            }

            // while (*ptr == '/')
            //    ptr++;

            ptr_temp = ptr + strlen(ptr) - 1;
            while (*ptr_temp == '\\') {
                *ptr_temp = '\0';
                ptr_temp--;
            }

            memset(hls->seg_desc, 0, sizeof(hls->seg_desc));
            memcpy(hls->seg_desc, ptr, strlen(ptr) + 1);

            break;
        } else if (!strncmp(item_desc, "#EXT-X-ENDLIST",
                strlen("#EXT-X-ENDLIST"))) {
            M_LOGD("hls end\n");
            goto __repeat;
        }
    }

    if (strlen(hls->seg_desc) + 1 >
        HTTP_URL_LEN - (hls->sub - loader->url)) {
        M_LOGE("seg desc length overrange !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }
    memset(hls->sub, 0,
        HTTP_URL_LEN - (hls->sub - loader->url));
    memcpy(hls->sub, hls->seg_desc,
        strlen(hls->seg_desc) + 1);

    M_LOGD("seg url %s\n", loader->url);

    if (hls->seg_offset) {
        hls->seg_offset[hls->seg_index] =
            hls->seg_offset[hls->seg_index - 1] +
            loader->content_length;
        M_LOGD("set seg_offset[%d] %d\n",
            hls->seg_index,
            hls->seg_offset[loader->hls->seg_index]);
    }

    if (http_loader_stop(loader)) {
        M_LOGE("stop http load failed !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }

__skip:
    loader->breakpoint_pos = 0;
    loader->redirect = 0;
    memset(loader->host_name, 0, sizeof(loader->host_name));
    memset(loader->uri, 0, sizeof(loader->uri));
    memset(loader->redirect_url, 0, sizeof(loader->redirect_url));

    if (http_loader_start(loader)) {
        M_LOGE("start http load failed !\n");
        os_mutex_unlock(hls->lock);
        return -1;
    }

__exit:
    os_mutex_unlock(hls->lock);
    return 0;
}

int uvoice_hls_build(http_loader_t *loader)
{
    net_cache_t *nc;
    char *ptr;
    char *ptr_temp;
    int ret;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    nc = loader->nc;
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    ptr = strstr(loader->url, ".m3u8");
    if (!ptr) {
        if (loader->hls) {
            if (loader->hls->fp)
                os_fclose(loader->hls->fp);
            if (loader->hls->content)
                snd_free(loader->hls->content);
            if (loader->hls->seg_offset)
                snd_free(loader->hls->seg_offset);
            os_mutex_free(loader->hls->lock);
            snd_free(loader->hls);
            loader->hls = NULL;
            nc->sequence = 0;
            M_LOGD("hls free\n");
        }
        goto __exit;
    }

    hls_t *hls = loader->hls;
    if (hls) {
        if (hls->fp) {
            os_fclose(hls->fp);
            hls->fp = NULL;
        }
        if (hls->content) {
            snd_free(hls->content);
            hls->content = NULL;
        }
        if (hls->seg_offset) {
            snd_free(hls->seg_offset);
            hls->seg_offset = NULL;
        }
        os_mutex_free(hls->lock);
        hls->len = 0;
        hls->pos = NULL;
        hls->seg_begin = 0;
        hls->seg_count = 0;
        hls->seg_index = 0;
        M_LOGD("hls reset\n");
    } else {
        hls = snd_zalloc(sizeof(hls_t), AFM_EXTN);
        if (!hls) {
            M_LOGE("alloc hls failed !\n");
            return -1;
        }
        hls->live_stream = HTTP_LIVE_STREAM_ENABLE;
        M_LOGD("alloc hls\n");
    }

    char *ptr_name;
    for (ptr_name = ptr - 1;
            ptr_name >= loader->url; ptr_name--) {
        if (*ptr_name == '/') {
            ptr_name++;
            break;
        }
    }

    if ((ptr_name - loader->url) <= strlen("http://") ||
        ptr_name == ptr) {
        M_LOGE("find hls name failed !\n");
        snd_free(hls);
        if (loader->hls)
            loader->hls = NULL;
        return -1;
    }

    if (http_loader_start(loader)) {
        M_LOGE("start http load failed !\n");
        snd_free(hls);
        if (loader->hls)
            loader->hls = NULL;
        return -1;
    }

#ifdef HTTP_LIST_FILE_LOAD
    if (access(PLAYER_LOCAL_TEMP_DIR, F_OK)) {
        if (mkdir(PLAYER_LOCAL_TEMP_DIR, S_IRWXU)) {
            M_LOGE("create %s failed %d!\n",
                    PLAYER_LOCAL_TEMP_DIR);
            http_loader_stop(loader);
            snd_free(hls);
            if (loader->hls)
                loader->hls = NULL;
            return -1;
        }
        M_LOGD("create dir: %s\n", PLAYER_LOCAL_TEMP_DIR);
    }

    int temp_name_len = strlen(PLAYER_LOCAL_TEMP_DIR) +
        strlen("temp.m3u8") + 2;
    char *temp_name = snd_zalloc(temp_name_len, AFM_EXTN);
    if (!temp_name) {
        M_LOGE("alloc temp name failed !\n");
        http_loader_stop(loader);
        snd_free(hls);
        if (loader->hls)
            loader->hls = NULL;
        return -1;
    }

    memcpy(temp_name, PLAYER_LOCAL_TEMP_DIR,
        strlen(PLAYER_LOCAL_TEMP_DIR) + 1);
    if (*(temp_name + strlen(PLAYER_LOCAL_TEMP_DIR) - 1) == '/')
        snprintf(temp_name + strlen(PLAYER_LOCAL_TEMP_DIR),
            temp_name_len - strlen(PLAYER_LOCAL_TEMP_DIR),
            "temp.m3u8");
    else
        snprintf(temp_name + strlen(PLAYER_LOCAL_TEMP_DIR),
            temp_name_len - strlen(PLAYER_LOCAL_TEMP_DIR),
            "/temp.m3u8");

    M_LOGD("open %s\n", temp_name);
    hls->fp = os_fopen(temp_name, "w+");
    if (!hls->fp) {
        M_LOGE("open %s failed !\n", temp_name);
        http_loader_stop(loader);
        snd_free(temp_name);
        snd_free(hls);
        if (loader->hls)
            loader->hls = NULL;
        return -1;
    }
    snd_free(temp_name);

#else
    if (loader->content_length > 4096) {
        M_LOGE("hls length %d overrange !\n",
            loader->content_length);
        http_loader_stop(loader);
        snd_free(hls);
        if (loader->hls)
            loader->hls = NULL;
        return -1;
    }

    hls->len = loader->content_length;
    hls->content = snd_zalloc(hls->len, AFM_EXTN);
    if (!hls->content) {
        M_LOGE("alloc hls content failed !\n");
        http_loader_stop(loader);
        snd_free(hls);
        if (loader->hls)
            loader->hls = NULL;
        return -1;
    }
    hls->pos = hls->content;
    M_LOGD("hls content len %d\n", hls->len);
#endif

    if (nc->head_data_size > 0) {
#ifdef HTTP_LIST_FILE_LOAD
        if (os_fwrite(nc->buffer, 1, nc->head_data_size, hls->fp) !=
            nc->head_data_size) {
            M_LOGE("write head data failed %d!\n");
            os_fclose(hls->fp);
            snd_free(hls);
            if (loader->hls)
                loader->hls = NULL;
            nc->head_data_size = 0;
            return -1;
        }
#else
        memcpy(hls->pos, nc->buffer, nc->head_data_size);
        hls->pos += nc->head_data_size;
#endif
        M_LOGD("save head data %u\n", nc->head_data_size);
        nc->load_length += nc->head_data_size;
        nc->head_data_size = 0;
    }

    int load_size = MIN(nc->buffer_size,
        nc->content_length - nc->load_length);
    int rd_ret;

    while (load_size > 0) {
        rd_ret = nc->cache_load(nc->priv,
            nc->buffer, load_size);
        if (rd_ret < 0) {
            M_LOGE("read failed !\n");
            break;
        } else if (rd_ret == 0) {
            M_LOGD("read end\n");
            break;
        }

#ifdef HTTP_LIST_FILE_LOAD
        ret = os_fwrite(nc->buffer, 1, rd_ret, hls->fp);
        if (ret != rd_ret) {
            M_LOGE("write failed %d!\n", os_ferror(hls->fp));
            break;
        }
#else
        if (hls->pos + rd_ret - hls->content > hls->len) {
            M_LOGE("content length overrange !\n");
            break;
        }
        memcpy(hls->pos, nc->buffer, rd_ret);
#endif

        hls->pos += rd_ret;
        nc->load_length += rd_ret;
        load_size = MIN(load_size,
            nc->content_length - nc->load_length);
    }

    nc->sequence = 1;
    nc->load_length = 0;
    http_loader_stop(loader);

    char item_desc[128];
    float duration_max;
    float duration = 0.001;
    float duration_sum = 0.0;
    bool seg_found = false;
    int pos_bak;
    int seg_current;
    int seg_index = 0;

#ifdef HTTP_LIST_FILE_LOAD
    os_fseek(hls->fp, 0, OS_SEEK_SET);
#else
    hls->pos = hls->content;
#endif

    hls->seg_begin = -1;

    while (1) {
#ifdef HTTP_LIST_FILE_LOAD
        if (os_feof(hls->fp)) {
            M_LOGD("hls file end\n");
            break;
        }

        memset(item_desc, 0, sizeof(item_desc));
        os_fgets(item_desc, sizeof(item_desc), hls->fp);
        if (item_desc[strlen(item_desc) - 1] == '\n')
            item_desc[strlen(item_desc) - 1] = '\0';
#else
        if (hls->pos - hls->content >= hls->len) {
            M_LOGD("hls content end\n");
            break;
        }

        ptr = strchr(hls->pos, '\n');
        if (!ptr) {
            M_LOGD("hls end\n");
            break;
        }
        memset(item_desc, 0, sizeof(item_desc));
        memcpy(item_desc, hls->pos, ptr - hls->pos);
        item_desc[ptr - hls->pos] = '\0';
        ptr++;
        hls->pos = ptr;
#endif

        ptr = (char *)item_desc;
        if (!strncmp(ptr, "#EXTINF:", strlen("#EXTINF:"))) {
            ptr += strlen("#EXTINF:");
            duration = atof(ptr);
            if (duration_max > 0.01 && duration > duration_max) {
                M_LOGE("duration invalid !\n");
                continue;
            }

            if (!seg_found) {
                #ifdef HTTP_LIST_FILE_LOAD
                memset(item_desc, 0, sizeof(item_desc));
                os_fgets(item_desc, sizeof(item_desc), hls->fp);
                ptr = (char *)item_desc;
                if (*(ptr + strlen(ptr) - 1) == '\n')
                    *(ptr + strlen(ptr) - 1) = '\0';

                pos_bak = os_ftell(hls->fp);

                #else
                ptr = strchr(hls->pos, '\n');
                if (!ptr) {
                    M_LOGD("seg desc not found\n");
                    continue;
                }

                if (ptr - hls->pos >= sizeof(item_desc)) {
                    M_LOGW("seg desc overrange\n");
                    continue;
                }

                memset(item_desc, 0, sizeof(item_desc));
                memcpy(item_desc, hls->pos, ptr - hls->pos);
                item_desc[ptr - hls->pos] = '\0';

                ptr++;
                hls->pos = ptr;
                pos_bak = (int)hls->pos;

                ptr = (char *)item_desc;
                #endif

                ptr_temp = ptr + strlen(ptr) - 1;
                while (*ptr_temp == '\\') {
                    *ptr_temp = '\0';
                    ptr_temp--;
                }

                memset(hls->seg_desc, 0, sizeof(hls->seg_desc));
                memcpy(hls->seg_desc, ptr, strlen(ptr) + 1);
                if (hls->seg_current > 0) {
                    seg_current = hls_index_get(hls->seg_desc);
                    M_LOGD("index %d current %d\n",
                        seg_current, hls->seg_current);
                    if (seg_current <= hls->seg_current) {
                        seg_index++;
                        continue;
                    }
                }
                hls->seg_index = seg_index;
                seg_found = true;
            }

            duration_sum += duration;
            hls->seg_count++;
        } else if (!strncmp(ptr, "#EXT-X-MEDIA-SEQUENCE:",
                strlen("#EXT-X-MEDIA-SEQUENCE:"))) {
            ptr += strlen("#EXT-X-MEDIA-SEQUENCE:");
            hls->seg_begin = atoi(ptr);
            if (hls->seg_begin < 0)
                M_LOGW("media sequence invalid !\n");
            else
                M_LOGD("media sequence %d\n", hls->seg_begin);

        } else if (!strncmp(ptr, "#EXT-X-TARGETDURATION:",
                strlen("#EXT-X-TARGETDURATION:"))) {
            ptr += strlen("#EXT-X-TARGETDURATION:");
            duration_max = atof(ptr);
            if (duration_max <= 0.001)
                M_LOGW("target duration invalid !\n");
            else
                M_LOGD("target duration %f\n", duration_max);

        } else if (!strncmp(ptr, "#EXT-X-ENDLIST",
                strlen("#EXT-X-ENDLIST"))) {
            M_LOGD("file end\n");
            break;
        }
    }

    if (!seg_found) {
        M_LOGE("segment not found !\n");
        return -1;
    }

#ifdef HTTP_LIST_FILE_LOAD
    os_fseek(hls->fp, pos_bak, OS_SEEK_SET);
#else
    hls->pos = (char *)pos_bak;
#endif
    hls->duration = (int)duration_sum;

    M_LOGD("duration %d count %d seg_index %d\n",
        hls->duration,
        hls->seg_count, hls->seg_index);

    if (strlen(hls->base) <= 0) {
        memcpy(hls->base, loader->url, strlen(loader->url) + 1);
        M_LOGD("seq base %s\n", hls->base);
    }

    if (!strncmp(hls->seg_desc, "http://", strlen("http://")) ||
        !strncmp(hls->seg_desc, "https://", strlen("https://"))) {
        if (strlen(hls->seg_desc) + 1 > HTTP_URL_LEN) {
            M_LOGE("seg desc length overrange !\n");
            return -1;
        }
        hls->sub = loader->url;
        memset(hls->sub, 0, HTTP_URL_LEN);
        memcpy(hls->sub, hls->seg_desc,
            strlen(hls->seg_desc) + 1);
    } else if (!strncmp(hls->seg_desc, "//", strlen("//"))) {
        if (strlen(hls->seg_desc) + 1 > HTTP_URL_LEN) {
            M_LOGE("seg desc length overrange !\n");
            return -1;
        }

        if (!strncmp(loader->url, "http://", strlen("http://"))) {
            hls->sub = loader->url + strlen("http:");
            memset(hls->sub, 0,
                HTTP_URL_LEN - (hls->sub - loader->url));
            if (strlen(hls->seg_desc) + 1 >
                HTTP_URL_LEN - (hls->sub - loader->url)) {
                M_LOGE("seg desc length overrange !\n");
                return -1;
            }
            memcpy(hls->sub, hls->seg_desc, strlen(hls->seg_desc) + 1);
        } else if (!strncmp(loader->url, "https://", strlen("https://"))) {
            hls->sub = loader->url + strlen("https:");
            memset(hls->sub, 0,
                HTTP_URL_LEN - (hls->sub - loader->url));
            memcpy(hls->sub, hls->seg_desc, strlen(hls->seg_desc) + 1);
        }
    } else {

        ptr = loader->url + strlen(loader->url);
        for (ptr_name = ptr - 1;
                ptr_name >= loader->url; ptr_name--) {
            if (*ptr_name == '/') {
                ptr_name++;
                break;
            }
        }

        if (strchr(hls->seg_desc, '/')) {
            char *seg_prefix;
            int prefix_len = 0;
            ptr = hls->seg_desc + strlen(hls->seg_desc);
            for (seg_prefix = ptr - 1;
                seg_prefix >= hls->seg_desc; seg_prefix--) {
                if (*seg_prefix == '/') {
                    prefix_len = seg_prefix - hls->seg_desc + 1;
                    break;
                }
            }

            if (prefix_len > 0) {
                if (!memcmp(ptr_name - prefix_len,
                    hls->seg_desc, prefix_len)) {
                    ptr_name -= prefix_len;
                    M_LOGD("prefix_len %d, ptr_name %s\n",
                        prefix_len, ptr_name);
                }
            }
        }

        if (HTTP_URL_LEN - (ptr_name - loader->url) <=
                strlen(hls->seg_desc)) {
            M_LOGE("seg desc name overrange !\n");
            if (hls->fp)
                os_fclose(hls->fp);
            if (hls->content)
                snd_free(hls->content);
            snd_free(hls);
            if (loader->hls)
                loader->hls = NULL;
            return -1;
        }

        hls->sub = ptr_name;

        memcpy(hls->sub, hls->seg_desc,
            strlen(hls->seg_desc) + 1);
    }
    M_LOGD("seg url %s\n", loader->url);

    hls->seg_offset = snd_zalloc(
        hls->seg_count * sizeof(int), AFM_EXTN);
    if (!hls->seg_offset) {
        M_LOGE("alloc seg offset failed !\n");
        if (hls->fp)
            os_fclose(hls->fp);
        if (hls->content)
            snd_free(hls->content);
        snd_free(hls);
        if (loader->hls)
            loader->hls = NULL;
        return -1;
    }

    hls->lock = os_mutex_new();

    if (!loader->hls)
        loader->hls = hls;

    M_LOGD("parse hls success\n");

__exit:
    return 0;
}

int uvoice_hls_release(http_loader_t *loader)
{
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (!loader->hls) {
        M_LOGE("hls null !\n");
        return -1;
    }

    if (loader->hls->fp) {
        os_fclose(loader->hls->fp);
        M_LOGD("hls file close\n");
    }

    if (loader->hls->content) {
        snd_free(loader->hls->content);
        M_LOGD("seq list content free\n");
    }

    if (loader->hls->seg_offset) {
        snd_free(loader->hls->seg_offset);
        M_LOGD("seq list offset free\n");
    }

    os_mutex_free(loader->hls->lock);
    snd_free(loader->hls);
    return 0;
}

