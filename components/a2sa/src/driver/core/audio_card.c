/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <errno.h>
#include "audio_card.h"
#include "audio_drv.h"

#define LOG_TAG "[audio_card]"

static int audio_card_bitmap = 0;

static int audio_card_get_available_id()
{
    int map = 0, id = 0;
    map = audio_card_bitmap;
    if(map > 0x80000000) {
        return -1;
    }
    while(map & 0x01) {
        id ++;
        map = map >> 1;
    }
    return id;
}

static void audio_card_set_id(int id)
{
    audio_card_bitmap |= (0x01 << id);
}

static void audio_card_clear_id(int id)
{
    audio_card_bitmap &= ~(0x01 << id);
}

audio_card_t *audio_card_new()
{
    int id = 0;
    int ret = -1;
    audio_card_t *card = NULL;

    id = audio_card_get_available_id();
    if(id < 0) {
        LOGE(LOG_TAG, "%s:%d, no available card id!!!", __func__, __LINE__);
        return NULL;
    }
    card = (audio_card_t *)malloc(sizeof(audio_card_t));
    if(!card) {
        LOGE(LOG_TAG, "%s:%d, malloc card failed!!!", __func__, __LINE__);
        return NULL;
    }
    memset(card, 0, sizeof(audio_card_t));
    card->id = id;
    card->pcm_dev_num = 0;
    card->pcm_str_num = 0;
    dlist_init(&card->pcm_dev_list);

    /* new control device */
    card->ctrl_dev = audio_ctrl_device_new(id);
    if(!card->ctrl_dev) {
        free(card);
        return NULL;
    }
    ret = audio_register_device(AUDIO_DEVICE_TYPE_CONTROL, card->ctrl_dev->name, (void *)card->ctrl_dev);
    if(ret < 0) {
        LOGE(LOG_TAG, "%s:%d, register control device %s failed, ret %d", __func__, __LINE__, card->ctrl_dev->name, ret);
        audio_ctrl_device_free(card->ctrl_dev);
        free(card);
        return NULL;
    }
    audio_card_set_id(id);
    return card;
}

pcm_device_t * audio_card_get_pcm_device(audio_card_t *card, void *private_data)
{
    pcm_device_t *pcm_dev = NULL, *node = NULL;

    if(!card || !private_data) {
        LOGE(LOG_TAG, "%s:%d, card or private_data is null", __func__, __LINE__);
        return NULL;
    }

    dlist_for_each_entry(&card->pcm_dev_list, node, pcm_device_t, list) {
        if(node->private_data == private_data) {
            pcm_dev = node;
            break;
        }
    }
    if(!pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, no matched pcm dev", __func__, __LINE__);
        return NULL;
    }
    return pcm_dev;
}

int audio_card_add_pcm_dev(audio_card_t *card, pcm_device_t *pcm_dev)
{
    int ret = -1;
    if(!card || !pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, card or pcm_dev is null!!!", __func__, __LINE__);
        return ret;
    }
    dlist_add_tail(&pcm_dev->list, &card->pcm_dev_list);
    card->pcm_dev_num ++;

    ret = audio_register_device(pcm_dev->dirType, pcm_dev->name, (void *)pcm_dev);
    return ret;
}

int audio_card_remove_pcm_dev(audio_card_t *card, pcm_device_t *pcm_dev)
{
    int ret = -1;
    if(!card || !pcm_dev) {
        LOGE(LOG_TAG, "%s:%d, card or pcm_dev is null!!!", __func__, __LINE__);
        return ret;
    }

    if(!pcm_dev->audioDevice) {
        pcm_dev->audioDevice = audio_get_device(pcm_dev->name);
    }
    if(!pcm_dev->audioDevice) {
        LOGE(LOG_TAG, "%s:%d, pcm dev audio device is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = audio_unregister_device(pcm_dev->audioDevice);

    if(!ret) {
        /* unregister pcm dev successfully */
        audio_pcm_device_free(pcm_dev);
        card->pcm_dev_num --;
    }
    return 0;
}

int audio_card_remove(audio_card_t *card)
{
    int ret = -1;
    pcm_device_t *pcm_dev = NULL;

    if(!card) {
        LOGE(LOG_TAG, "%s:%d, card is null!!!", __func__, __LINE__);
        return ret;
    }
    if(card->ctrl_dev) {
        ret = audio_ctrl_device_free(card->ctrl_dev);
    }

    dlist_for_each_entry(&card->pcm_dev_list, pcm_dev, pcm_device_t, list) {
        if(pcm_dev) {
            ret = audio_card_remove_pcm_dev(card, pcm_dev);
        }
    }

    audio_card_clear_id(card->id);

    free(card);
    card = NULL;
    return ret;
}