/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "uvoice_types.h"
#include "uvoice_player.h"
#include "uvoice_event.h"

#include "uvoice_os.h"
#include "uvoice_comb.h"
#include "uvoice_list.h"

#include "cJSON.h"


#define TONE_SOURCE_PREFIX        ""
#define TONE_SOURCE_SUFFIX        ""

#define TONE_SOURCE_LEN_MAX        64

#define FILE_LIST_COUNT_MAX        32
#define HTTP_LIST_COUNT_MAX        32

#define BUILD_FOR_WTK_PAYBOX


enum {
    COMB_PLAY_STAT_STOP = 0,
    COMB_PLAY_STAT_RUNNING,
};

typedef struct {
    uint8_t running:1;
    uint8_t stop:1;
    uint8_t listplay_stop:1;
    int comb_play_state;
    int listplay_count;
    uvoice_player_t *player;
    uvoice_list_t list;
    uvoice_list_t url_list;
    os_sem_t listplay_stop_sem;
    os_mutex_t list_lock;
    os_mutex_t url_list_lock;
    os_task_t task;
    void (*state_cb)(int);
} comb_handler_t;

typedef struct {
    char source[TONE_SOURCE_LEN_MAX];
    int index;
    int final;
    void (*cplt_cb)(void *args, int event);
    void *userdata;
    uvoice_list_t list;
} comb_node_t;

typedef struct {
    char *url;
    int index;
    uvoice_list_t list;
} url_node_t;

static char *tonename_numb[] = {
    "num_0",
    "num_1",
    "num_2",
    "num_3",
    "num_4",
    "num_5",
    "num_6",
    "num_7",
    "num_8",
    "num_9",
    "num_10",
};

static char *tonename_unit[] = {
    "none",
    "ten",
    "hundred",
    "thousand",
};

static char *tonename_hunit[] = {
    "ten_thousand",
    "hundred_million",
};

static comb_handler_t *g_comb_handler;

static int add_amount(uvoice_list_t *head, double num, int index)
{
    comb_node_t *node;
    int slot;
    int depth = 0;
    int factor = 0;
    int ret = -1;

    long long numb = (long long)num;
    double deci = num - (double)numb;

    long long target = numb;
    int sub_target;
    int sub_number;
    bool prev_slot_zero = false;
    bool hundred_million_exist = false;
    bool ten_thousand_exist = false;
    char deci_desc[16];
    char *ptr;
    int i;
    int count = 0;

    if (!head) {
        M_LOGE("list head null !\n");
        return -1;
    }

    if (numb < 0 || numb >= 1000000000000) {
        M_LOGE("numb overrange !\n");
        return -1;
    }

    if (deci < 0.0001 && deci > 0.0) {
        M_LOGW("deci reset\n");
        deci = 0.0001;
    }

    M_LOGD("num %lf deci %lf numb %ld\n", num, deci, numb);

    for (i = 2; i >= 0; i--) {
        factor = pow(10000, i);
        if (target < factor)
            continue;

        sub_target = (int)(target / factor);

        target %= factor;

        if (sub_target == 0)
            continue;

        if (i == 2)
            hundred_million_exist = true;
        else if (i == 1)
            ten_thousand_exist = true;

        sub_number = sub_target;

        prev_slot_zero = false;

        for (depth = 3; depth >= 0; depth--) {

            if (sub_number == 0)
                break;

            factor = pow(10, depth);

            if ((hundred_million_exist || ten_thousand_exist) && i == 0) {
                M_LOGD("%s: nop\n", __func__);
            } else if (hundred_million_exist && ten_thousand_exist &&
                (depth > 0) && sub_target < factor) {
                M_LOGD("%s: nop\n", __func__);
            } else if (sub_target < factor) {
                continue;
            }

            slot = (int)(sub_number / factor);
            sub_number %= factor;

            if (slot == 0 && depth == 0)
                continue;

            if ((sub_target < 20 && depth == 1) ||
                (slot == 0 && prev_slot_zero) ||
                (slot == 0 && depth == 0)) {
                M_LOGD("%s: nop\n", __func__);
            } else {
                node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
                if (!node) {
                    M_LOGE("alloc tone list failed !\n");
                    return -1;
                }
                snprintf(node->source, sizeof(node->source),
                    "%s", tonename_numb[slot]);
                node->index = index;
                uvoice_list_add_tail(&node->list, head);
                count++;

                if (slot == 0 && !prev_slot_zero)
                    prev_slot_zero = true;
                else if (prev_slot_zero && slot != 0)
                    prev_slot_zero = false;
            }

            if (slot > 0 && depth > 0) {
                node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
                if (!node) {
                    M_LOGE("alloc tone list failed !\n");
                    return -1;
                }
                snprintf(node->source, sizeof(node->source),
                    "%s", tonename_unit[depth]);
                node->index = index;
                uvoice_list_add_tail(&node->list, head);
                count++;
            }
        }

        if (i > 0) {
            node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
            if (!node) {
                M_LOGE("alloc tone list failed !\n");
                return -1;
            }

            snprintf(node->source, sizeof(node->source),
                "%s", tonename_hunit[i - 1]);
            node->index = index;
            uvoice_list_add_tail(&node->list, head);
            count++;
        }
    }

    if (count == 0 && numb == 0) {
        node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
        if (!node) {
            M_LOGE("alloc tone list failed !\n");
            return -1;
        }

        snprintf(node->source, sizeof(node->source),
            "num_0");
        node->index = index;
        uvoice_list_add_tail(&node->list, head);
        count++;
    }

    if (deci >= 0.0001) {
        M_LOGD("add deci part\n");

        node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
        if (!node) {
            M_LOGE("alloc tone list failed !\n");
            return -1;
        }

        snprintf(node->source, sizeof(node->source), "dot");
        node->index = index;
        uvoice_list_add_tail(&node->list, head);

        snprintf(deci_desc, sizeof(deci_desc), "%lf", deci);
        memmove(deci_desc, (char *)deci_desc + 2,
            strlen((char *)deci_desc + 2));

        ptr = (char *)deci_desc + (strlen(deci_desc) - 1);

        while (*ptr == '0' || (ptr - deci_desc >= 4)) {
            *ptr = '\0';
            ptr--;
            if (ptr == deci_desc)
                break;
        }

        for (i = 0; i < strlen(deci_desc); i++) {
            sub_number = deci_desc[i] - '0';

            node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
            if (!node) {
                M_LOGE("alloc tone list failed !\n");
                return -1;
            }

            snprintf(node->source, sizeof(node->source),
                "%s", tonename_numb[sub_number]);
            node->index = index;
            uvoice_list_add_tail(&node->list, head);
        }
    }

    return 0;
}

static int add_number(uvoice_list_t *head, const char *number, int index)
{
    comb_node_t *node;
    char *ptr;
    int number_len;
    int i;
    char num_code;

    if (!number) {
        M_LOGE("number null !\n");
        return -1;
    }

    number_len = strlen(number);

    for (i = 0; i < number_len; i++) {
        num_code = *(number + i);
        if (num_code < '0' || num_code > '9')
            return -1;
    }

    for (i = 0; i < number_len; i++) {
        num_code = *(number + i);
        if (num_code < '0' || num_code > '9')
            continue;
        num_code -= '0';
        node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
        if (!node) {
            M_LOGE("alloc tone list failed !\n");
            continue;
        }
        snprintf(node->source, sizeof(node->source),
            "%s", tonename_numb[num_code]);
        node->index = index;
        uvoice_list_add_tail(&node->list, head);
    }

    return 0;
}

static int add_file_source(uvoice_list_t *head, const char *source,
    int index, int final, void (*cb)(void *args, int event), void *userdata)
{
    comb_node_t *node;

    if (!head) {
        M_LOGE("list head null !\n");
        return -1;
    }

    if (!source) {
        M_LOGE("source null !\n");
        return -1;
    }

    if (strlen(source) >= TONE_SOURCE_LEN_MAX || strlen(source) <= 0) {
        M_LOGE("source %s length overrange !\n", source);
        return -1;
    }

    node = snd_zalloc(sizeof(comb_node_t), AFM_EXTN);
    if (!node) {
        M_LOGE("alloc tone list failed !\n");
        return -1;
    }

    node->index = index;
    node->final = final;
    if (node->final) {
        node->cplt_cb = cb;
        node->userdata = userdata;
    }
    snprintf(node->source, sizeof(node->source), "%s", source);
    uvoice_list_add_tail(&node->list, head);
    return 0;
}

static int add_http_source(uvoice_list_t *head, const char *url, int index)
{
    url_node_t *node;

    if (!head) {
        M_LOGE("list head null !\n");
        return -1;
    }

    if (!url) {
        M_LOGE("url null !\n");
        return -1;
    }

    if (strncmp(url, "http://", strlen("http://")) &&
        strncmp(url, "https://", strlen("https://")) &&
        strncmp(url, "fs:", strlen("fs:"))) {
        M_LOGE("url invalid !\n");
        return -1;
    }

    node = snd_zalloc(sizeof(url_node_t), AFM_EXTN);
    if (!node) {
        M_LOGE("alloc url node failed !\n");
        return -1;
    }

    node->url = snd_zalloc(strlen(url) + 1, AFM_EXTN);
    if (!node->url) {
        M_LOGE("alloc url buffer failed !\n");
        snd_free(node);
        return -1;
    }

    node->index = index;
    snprintf(node->url, strlen(url) + 1, "%s", url);
    M_LOGI("play %s\n", node->url);
    uvoice_list_add_tail(&node->list, head);
    return 0;
}

static int file_is_accessed(char *source)
{
    if (os_access(source, OS_F_OK)) {
        M_LOGE("file %s not exist\n", source);
        return 0;
    }

    if (os_access(source, OS_R_OK)) {
        M_LOGE("file %s not readable\n", source);
        return 0;
    }

    return 1;
}

static int __comb_play_url(comb_handler_t *handler)
{
    uvoice_player_t *player;
    player_state_t state = -1;
    url_node_t *node;
    uvoice_list_t *temp;

    player = handler->player;

    os_mutex_lock(handler->url_list_lock, OS_WAIT_FOREVER);
    if (uvoice_list_empty(&handler->url_list)) {
        os_mutex_unlock(handler->url_list_lock);
        return 0;
    }

    if (player->get_state(&state)) {
        M_LOGE("get player state failed !\n");
        os_mutex_unlock(handler->url_list_lock);
        return -1;
    }

    if (state != PLAYER_STAT_IDLE) {
        os_mutex_unlock(handler->url_list_lock);
        return 0;
    }

    uvoice_list_for_each_entry_safe(&handler->url_list, temp,
            node, url_node_t, list) {
        if (node->url) {
            if (player->set_source(node->url)) {
                uvoice_list_del(&node->list);
                snd_free(node->url);
                snd_free(node);
                continue;
            }

            if (player->start()) {
                handler->player->clr_source();
            }
            uvoice_list_del(&node->list);
            snd_free(node->url);
            snd_free(node);
            break;
        }
    }

    os_mutex_unlock(handler->url_list_lock);
    return 0;
}

static void __comb_task(void *arg)
{
    comb_handler_t *handler = (comb_handler_t *)arg;
    uvoice_player_t *player;
    player_state_t state = -1;
    comb_node_t *node;
    uvoice_list_t *temp;
    media_format_t format;
    bool player_paused = false;
    bool stream_open = false;
    int file_size;
    int read_size;
    int buffer_size = 1200;
    char *buffer;
    os_file_t fp;
    int ret;
    int first_buffer = 0;
    int list_play_count = 0;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    player = handler->player;

    buffer = snd_zalloc(buffer_size, AFM_MAIN);
    if (!buffer) {
        M_LOGE("alloc buffer failed !\n");
        os_task_exit(handler->task);
        return;
    }

    handler->running = 1;
    handler->comb_play_state = PLAYER_STAT_LIST_PLAY_STOP;
    M_LOGI("comb running\n");
    while (!handler->stop) {
        os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
        if (uvoice_list_empty(&handler->list)) {
            os_mutex_unlock(handler->list_lock);
            __comb_play_url(handler);
            os_msleep(100);
            continue;
        }

        player->get_state(&state);
        if (state == PLAYER_STAT_RUNNING ||
            state == PLAYER_STAT_COMPLETE) {
            #ifdef BUILD_FOR_WTK_PAYBOX
            player->stop();
            player->clr_source();
            player_paused = false;
            #else
            player->pause();
            player_paused = true;
            #endif
        }

        uvoice_list_for_each_entry_safe(&handler->list, temp,
            node, comb_node_t, list) {
            if (handler->listplay_stop) {
                uvoice_list_del(&node->list);
                snd_free(node);
                if (node->final) {
                    if (node->cplt_cb)
                        node->cplt_cb(node->userdata, 1);
                    handler->listplay_count--;
                }
                continue;
            }
            os_mutex_unlock(handler->list_lock);

            if (handler->comb_play_state == PLAYER_STAT_LIST_PLAY_STOP) {
                handler->comb_play_state = PLAYER_STAT_LIST_PLAY_START;
                uvoice_event_post(UVOICE_EV_PLAYER,
                        UVOICE_CODE_PLAYER_STATE, handler->comb_play_state);
            }

            M_LOGD("play %s, index %d\n", node->source, node->index);
            fp = os_fopen(node->source, "r");
            if (!fp) {
                M_LOGE("open %s failed\n", node->source);
                os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
                uvoice_list_del(&node->list);
                snd_free(node);
                continue;
            }

            if (!stream_open) {
                if (strstr(node->source, ".mp3") || strstr(node->source, ".MP3")) {
                    format = MEDIA_FMT_MP3;
                    buffer_size = 1200;
                } else if (strstr(node->source, ".amr") || strstr(node->source, ".AMR")) {
                    format = MEDIA_FMT_AMR;
                    buffer_size = 320;
                } else if (strstr(node->source, ".wav") || strstr(node->source, ".WAV")) {
                    format = MEDIA_FMT_WAV;
                    buffer_size = 960;
                } else {
                    M_LOGE("format not support !\n");
                    os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
                    uvoice_list_del(&node->list);
                    snd_free(node);
                    os_fclose(fp);
                    continue;
                }
                if (player->set_stream(format, 0, 0)) {
                    M_LOGE("set stream failed !\n");
                    os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
                    uvoice_list_del(&node->list);
                    snd_free(node);
                    os_fclose(fp);
                    continue;
                }
                list_play_count = 0;
                stream_open = true;
            } else {
                list_play_count++;
            }

            os_fseek(fp, 0, OS_SEEK_END);
            file_size = os_ftell(fp);
            os_fseek(fp, 0, OS_SEEK_SET);

            first_buffer = 2;
            while (1) {
                #if 1
                ret = os_fread(buffer, 1, buffer_size, fp);
                if (ret < 0) {
                    M_LOGE("read failed %d!\n", ret);
                    break;
                } else if (ret == 0) {
                    break;
                }
                read_size = ret;
                #else
                if (file_size - play_size < buffer_size) {
                    read_size = file_size - play_size;
                    if (read_size <= 0)
                        break;
                } else {
                    read_size = buffer_size;
                }
                ret = os_fread(buffer, 1, read_size, fp);
                if (ret <= 0) {
                    M_LOGD("read end\n");
                    break;
                }
                play_size += read_size;
                #endif

                if (node->index == 0 && first_buffer == 2 && list_play_count > 0 && format == MEDIA_FMT_WAV) {
                    player->put_stream(buffer + 44, read_size - 44);
                    if (handler->listplay_stop) {
                        M_LOGD("list playing stop\n");
                        break;
                    }
                    first_buffer = 1;
                    continue;
                }

                if (node->index != 0 && first_buffer && format == MEDIA_FMT_WAV) {
                    player->put_stream(buffer + 44, read_size - 44);
                    first_buffer = 0;
                    if (handler->listplay_stop) {
                        M_LOGD("list playing stop\n");
                        break;
                    }
                    continue;
                }

                if (player->put_stream(buffer, read_size)) {
                    M_LOGE("put stream failed !\n");
                    break;
                }

                if (handler->listplay_stop) {
                    M_LOGD("list playing stop\n");
                    break;
                }
            }

            os_fclose(fp);
            os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
            if (node->final) {
                handler->listplay_count--;
                if (node->cplt_cb)
                    node->cplt_cb(node->userdata, 0);
            }
            uvoice_list_del(&node->list);
            snd_free(node);
        }

        player->clr_stream(0);
        stream_open = false;
        handler->comb_play_state = PLAYER_STAT_LIST_PLAY_STOP;
        uvoice_event_post(UVOICE_EV_PLAYER,
                UVOICE_CODE_PLAYER_STATE, handler->comb_play_state);

        if (handler->listplay_stop) {
            handler->listplay_stop = 0;
            os_sem_signal(handler->listplay_stop_sem);
        }

        if (player_paused) {
            player->set_fade(40, 60);
            player->resume();
            player_paused = false;
        }
        os_mutex_unlock(handler->list_lock);
    }

    snd_free(buffer);
    M_LOGW("comb task exit\n");
    os_task_exit(handler->task);
}

int comb_receipt_play(const char *req)
{
    comb_handler_t *handler = g_comb_handler;
    cJSON *root, *item, *sub_item;
    char *ptr;
    int index, timestamp;
    int i, ret;

    if (!req) {
        M_LOGE("args invalid !\n");
        return -1;
    }

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    root = AMP_cJSON_Parse(req);
    if (!root || !AMP_cJSON_IsObject(root)) {
        M_LOGE("parse json root failed !\n");
        return -1;
    }

    item = AMP_cJSON_GetObjectItem(root, "Timestamp");
    if (!item || !AMP_cJSON_IsString(item)) {
        M_LOGE("receipt timestamp not found !\n");
        cJSON_Delete(root);
        return -1;
    }

    timestamp = atoi(item->valuestring);
    if (timestamp < 0) {
        M_LOGE("receipt timestamp invalid !\n");
        cJSON_Delete(root);
        return -1;
    }
    M_LOGI("timestamp %d\n", timestamp);

    item = AMP_cJSON_GetObjectItem(root, "Index");
    if (!item || !AMP_cJSON_IsNumber(item)) {
        M_LOGE("receipt index not found !\n");
        cJSON_Delete(root);
        return -1;
    }
    index = item->valueint;

    item = AMP_cJSON_GetObjectItem(root, "MSG");
    if (!item || !AMP_cJSON_IsArray(item)) {
        M_LOGE("get receipt object failed !\n");
        cJSON_Delete(root);
        return -1;
    }

    for (i = 0; i < cJSON_GetArraySize(item); i++) {
        sub_item = AMP_cJSON_GetArrayItem(item, i);
        if (!sub_item || !AMP_cJSON_IsString(sub_item)) {
            M_LOGE("parse array item %d failed !\n", i);
            continue;
        }

        ptr = sub_item->valuestring;
        if (*ptr != '$' && (*ptr < '0' || *ptr > '9')) {
            ret = file_is_accessed(ptr);
            if (!ret)
                return -2;
        }
    }

    os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
    for (i = 0; i < cJSON_GetArraySize(item); i++) {
        sub_item = AMP_cJSON_GetArrayItem(item, i);
        if (!sub_item || !AMP_cJSON_IsString(sub_item)) {
            M_LOGE("parse array item %d failed !\n", i);
            continue;
        }

        ptr = sub_item->valuestring;
        if (*ptr == '$') {
            if (strlen(ptr) < 2) {
                M_LOGE("parse number value failed !\n");
                continue;
            }
            add_amount(&handler->list, atof(ptr + 1), index);
            continue;
        }

        if (*ptr >= '0' && *ptr <= '9') {
            add_number(&handler->list, ptr, index);
            continue;
        }

        ret = add_file_source(&handler->list, sub_item->valuestring, index, 0, NULL, NULL);
    }
    os_mutex_unlock(handler->list_lock);

    cJSON_Delete(root);
    return 0;
}

int comb_content_play(const char *req)
{
    comb_handler_t *handler = g_comb_handler;
    cJSON *root, *item;
    int index, timestamp;

    if (!req) {
        M_LOGE("args invalid !\n");
        return -1;
    }

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    root = AMP_cJSON_Parse(req);
    if (!root || !AMP_cJSON_IsObject(root)) {
        M_LOGE("parse json root failed !");
        return -1;
    }

    item = AMP_cJSON_GetObjectItem(root, "Timestamp");
    if (!item || !AMP_cJSON_IsString(item)) {
        M_LOGE("content timestamp not found !\n");
        cJSON_Delete(root);
        return -1;
    }

    timestamp = atoi(item->valuestring);
    if (timestamp < 0) {
        M_LOGE("receipt timestamp invalid !\n");
        cJSON_Delete(root);
        return -1;
    }
    M_LOGI("timestamp %d\n", timestamp);

    item = AMP_cJSON_GetObjectItem(root, "Index");
    if (!item || !AMP_cJSON_IsNumber(item)) {
        M_LOGE("content index not found !\n");
        cJSON_Delete(root);
        return -1;
    }
    index = item->valueint;

    item = AMP_cJSON_GetObjectItem(root, "URL");
    if (!item || !AMP_cJSON_IsString(item)) {
        M_LOGE("get url object failed !\n");
        cJSON_Delete(root);
        return -1;
    }

    os_mutex_lock(handler->url_list_lock, OS_WAIT_FOREVER);
    add_http_source(&handler->url_list, item->valuestring, index);
    os_mutex_unlock(handler->url_list_lock);

    cJSON_Delete(root);
    return 0;
}

int comb_add_amount(const char *amount, int index)
{
    comb_handler_t *handler = g_comb_handler;
    double _amount;
    int ret;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    _amount = atof(amount);

    os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
    ret = add_amount(&handler->list, _amount, index);
    os_mutex_unlock(handler->list_lock);
    return ret;
}

int comb_add_number(const char *number, int index)
{
    comb_handler_t *handler = g_comb_handler;
    int ret;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
    ret = add_number(&handler->list, number, index);
    os_mutex_unlock(handler->list_lock);
    return ret;
}

int comb_add_file_source_list(comb_source_info_t *info)
{
    comb_handler_t *handler = g_comb_handler;
    int i;
    int ret = 0;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
    if (handler->listplay_count <= FILE_LIST_COUNT_MAX) {
        for (i = 0; i < info->count; i++) {
            if (i == info->count - 1)
                add_file_source(&handler->list, info->sources[i], i, 1, info->callback, info->userdata);
            else
                add_file_source(&handler->list, info->sources[i], i, 0, NULL, NULL);
        }
        handler->listplay_count++;

    } else {
        M_LOGW("play list is full\n");
    }
    os_mutex_unlock(handler->list_lock);
    return 0;
}

int comb_add_file_source_unlock(const char *source, int index,
    int final, void (*cb)(void *args, int event), void *userdata)
{
    comb_handler_t *handler = g_comb_handler;
    int ret = 0;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    if (!file_is_accessed(source)) {
        return -1;
    }

    if (handler->listplay_count <= FILE_LIST_COUNT_MAX) {
        ret = add_file_source(&handler->list, source, index, final, cb, userdata);
        if (!ret) {
            if (final)
                handler->listplay_count++;
        }
    } else {
        M_LOGW("play list is full\n");
    }
    return ret;
}

int comb_file_source_lock(void)
{
    comb_handler_t *handler = g_comb_handler;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    return os_mutex_lock(handler->list_lock, OS_WAIT_FOREVER);
}

int comb_file_source_unlock(void)
{
    comb_handler_t *handler = g_comb_handler;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    return os_mutex_unlock(handler->list_lock);
}

int comb_add_http_source(const char *source, int index)
{
    comb_handler_t *handler = g_comb_handler;
    int ret = 0;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    os_mutex_lock(handler->url_list_lock, OS_WAIT_FOREVER);
    if (uvoice_list_entry_count(&handler->url_list) <= HTTP_LIST_COUNT_MAX) {
        ret = add_http_source(&handler->url_list, source, index);
    } else {
        M_LOGW("play list is full\n");
    }
    os_mutex_unlock(handler->url_list_lock);
    return ret;
}

int comb_clr_http_source(void)
{
    comb_handler_t *handler = g_comb_handler;
    url_node_t *node;
    uvoice_list_t *temp;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (!handler->running) {
        M_LOGW("comb not running, ignore\n");
        return -1;
    }

    os_mutex_lock(handler->url_list_lock, OS_WAIT_FOREVER);
    uvoice_list_for_each_entry_safe(&handler->url_list, temp,
            node, url_node_t, list) {
        uvoice_list_del(&node->list);
        snd_free(node->url);
        snd_free(node);
    }
    os_mutex_unlock(handler->url_list_lock);
    return 0;
}

int comb_play_stop(void)
{
    comb_handler_t *handler = g_comb_handler;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    if (handler->comb_play_state == PLAYER_STAT_LIST_PLAY_START) {
        handler->listplay_stop = 1;
        os_sem_wait(handler->listplay_stop_sem, 5000);
    }
    return 0;
}

int comb_init(void)
{
    comb_handler_t *handler = g_comb_handler;
    cache_config_t config;

    if (handler) {
        M_LOGW("comb init already, ignore\n");
        return -1;
    }

    handler = snd_zalloc(sizeof(comb_handler_t), AFM_EXTN);
    if (!handler) {
        M_LOGE("alloc comb handler failed !\n");
        return -1;
    }

    handler->player = uvoice_player_create();
    if (!handler->player) {
        M_LOGE("create uvoice player failed !\n");
        snd_free(handler);
        return -1;
    }

    config.place = 2;
    config.mem_size = 128;
    handler->player->cache_config(&config);

    handler->player->set_standby(0);

    uvoice_list_init(&handler->list);
    uvoice_list_init(&handler->url_list);
    handler->list_lock = os_mutex_new();
    handler->url_list_lock = os_mutex_new();
    handler->listplay_stop_sem = os_sem_new(0);

    g_comb_handler = handler;

    if (os_task_create(&handler->task, "comb_player_task",
        __comb_task,
        handler,
        8192,
        UVOICE_TASK_PRI_NORMAL)) {
        M_LOGE("create comb task failed !\n");
        os_mutex_free(handler->list_lock);
        os_mutex_free(handler->url_list_lock);
        os_sem_free(handler->listplay_stop_sem);
        uvoice_player_release(handler->player);
        snd_free(handler);
        g_comb_handler = NULL;
        return -1;
    }

    os_msleep(20);
    M_LOGI("comb init success\n");
    return 0;
}

int comb_deinit(void)
{
    comb_handler_t *handler = g_comb_handler;

    if (!handler) {
        M_LOGE("handler null !\n");
        return -1;
    }

    handler->stop = 1;
    os_msleep(120);

    os_mutex_free(handler->list_lock);
    os_mutex_free(handler->url_list_lock);
    os_sem_free(handler->listplay_stop_sem);
    uvoice_player_release(handler->player);
    snd_free(handler);
    g_comb_handler = NULL;
    return 0;
}
