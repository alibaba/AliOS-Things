/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "sound_mixer.h"
#include "ulog/ulog.h"

#define LOG_TAG  "[sound_mixer]"
#define MIXER_RETURN_ERR      -1
#define MIXER_RETURN_SUCCESS  0

#define AOS_MASTER_VOLUME_TX_STRING    "Master Volume TX"
#define AOS_MASTER_MUTE_STATE_STRING   "Master Mute State"

static int aos_mixer_common_set_int_value(char *name, int value)
{
    int card = 0;
    aos_mixer_t *mixer = NULL;
    if(!name) {
        return MIXER_RETURN_ERR;
    }
    for(card = 0; card < AOS_SNDCARD_NUM_MAX; card++) {
        if(MIXER_RETURN_SUCCESS == aos_mixer_open(&mixer, card)) {
            aos_mixer_print_info(mixer);
            if(MIXER_RETURN_SUCCESS == aos_mixer_set_int_value(mixer, name, value)) {
                aos_mixer_close(mixer);
                mixer = NULL;
                return MIXER_RETURN_SUCCESS;
            }
        }
        aos_mixer_close(mixer);
        mixer = NULL;
    }
    return MIXER_RETURN_ERR;
}

int aos_mixer_common_get_int_value(char *name, int *value)
{
    int card = 0;
    aos_mixer_t *mixer = NULL;
    if(!name) {
        return MIXER_RETURN_ERR;
    }
    for(card = 0; card < AOS_SNDCARD_NUM_MAX; card++) {
        if(MIXER_RETURN_SUCCESS == aos_mixer_open(&mixer, card)) {
            aos_mixer_print_info(mixer);
            if(MIXER_RETURN_SUCCESS == aos_mixer_get_int_value(mixer, name, value)) {
                aos_mixer_close(mixer);
                mixer = NULL;
                return MIXER_RETURN_SUCCESS;
            }
        }
        aos_mixer_close(mixer);
        mixer = NULL;
    }
    return MIXER_RETURN_ERR;
}

int aos_mixer_open(aos_mixer_t **mixer, int card)
{
    struct audio_ctl_elem_list elist;
    struct audio_ctl_elem_id *eid = NULL;
    struct audio_ctl_elem_info *ei = NULL;
    int fd;
    unsigned int n;
    char fn[256];

    /* 1. open fd */
    snprintf(fn, sizeof(fn), "/dev/controlC%u", card);
    fd = open(fn, O_RDWR);
    if (fd < 0) {
        LOGE(LOG_TAG, "%s:%d, open %s failed", __func__, __LINE__, fn);
        return MIXER_RETURN_ERR;
    }
    LOGD(LOG_TAG, "%s:%d, open %s successfully", __func__, __LINE__, fn);

    /* 2. new aos_mixer_t object */
    *mixer = (aos_mixer_t *)calloc(1, sizeof(aos_mixer_t));
    memset(*mixer, 0, sizeof(aos_mixer_t));
    (*mixer)->fd = fd;
    (*mixer)->name = strdup(fn);
    (*mixer)->card = card;

    /* 3. get elem_list count number */
    memset(&elist, 0, sizeof(elist));
    if (ioctl(fd, AUDIO_CTL_IOCTL_ELEM_LIST, &elist) < 0) {
        return MIXER_RETURN_ERR;
    }
    (*mixer)->count = elist.count;
    elist.space = elist.count;

    /* 4. get card info */
    if(ioctl(fd, AUDIO_CTL_IOCTL_CARD_INFO, &((*mixer)->card_info)) < 0) {
        return MIXER_RETURN_ERR;
    }

    /* 5. get audio_ctl_elem_list */
    eid = calloc(elist.count, sizeof(struct audio_ctl_elem_id));
    if(!eid) {
        return MIXER_RETURN_ERR;
    }
    elist.pids = eid;
    if (ioctl(fd, AUDIO_CTL_IOCTL_ELEM_LIST, &elist) < 0) {
        return MIXER_RETURN_ERR;
    }

    /* 6. get audio_ctl_elem_info */
    (*mixer)->elem_info = calloc(elist.count, sizeof(struct audio_ctl_elem_info));
    if(!(*mixer)->elem_info) {
        return MIXER_RETURN_ERR;
    }
    for (n = 0; n < (*mixer)->count; n++) {
        ei = (*mixer)->elem_info + n;
        ei->id.id = eid[n].id;
        if (ioctl(fd, AUDIO_CTL_IOCTL_ELEM_INFO, ei) < 0) {
            LOGE(LOG_TAG, "%s:%d, ioctl AUDIO_CTL_IOCTL_ELEM_INFO failed", __func__, __LINE__);
        }
    }
    if(eid) {
        free(eid);
    }
    return MIXER_RETURN_SUCCESS;
}

int aos_mixer_print_info(aos_mixer_t *mixer)
{
    const char *type;
    unsigned int info_cnt;
    unsigned int i;
    struct audio_ctl_elem_info * elem_info = NULL;

    if(!mixer) {
        return MIXER_RETURN_ERR;
    }
    info_cnt = mixer->count;
    LOGD(LOG_TAG, "%s:%d, element_info count %d", __func__, __LINE__, info_cnt);
    for (i = 0; i < info_cnt; i++) {
        elem_info = mixer->elem_info + i;
        switch (elem_info->type) {
            case AOS_CTL_ELEM_TYPE_BOOLEAN:
                type = "BOOL";
                break;
            case AOS_CTL_ELEM_TYPE_INTEGER:
                type = "INT";
                break;
            case AOS_CTL_ELEM_TYPE_ENUMERATED:
                type = "ENUM";
                break;
            case AOS_CTL_ELEM_TYPE_BYTES:
                type = "BYTE";
                break;
            case AOS_CTL_ELEM_TYPE_IEC958:
                type = "IEC958";
                break;
            case AOS_CTL_ELEM_TYPE_INTEGER64:
                type = "INT64";
                break;
            default:
                type = "Unknown";
                break;
        }

        LOGD(LOG_TAG, "%s:%d, element_info[%d].id.id = %d", __func__, __LINE__, i, elem_info->id.id);
        LOGD(LOG_TAG, "%s:%d, element_info[%d].id.name = %s", __func__, __LINE__, i, elem_info->id.name);
        LOGD(LOG_TAG, "%s:%d, element_info[%d].count = %d", __func__, __LINE__, i, elem_info->count);
        LOGD(LOG_TAG, "%s:%d, element_info[%d].type = %d (%s)", __func__, __LINE__, i, elem_info->type, type);
    }
    return MIXER_RETURN_SUCCESS;
}

int aos_mixer_set_int_value(aos_mixer_t *mixer, char *name, int value)
{
    int i = 0;
    struct audio_ctl_elem_info * elem_info = NULL;
    struct audio_ctl_elem_value ev;
    if(!mixer || !name) {
        return MIXER_RETURN_ERR;
    }
    for(i = 0; i < mixer->count; i++) {
        elem_info = mixer->elem_info + i;
        if(!strcmp(elem_info->id.name, name)) {
            break;
        }
    }
    if(i >= mixer->count) {
        return MIXER_RETURN_ERR;
    }
    if(AOS_CTL_ELEM_TYPE_INTEGER != elem_info->type) {
        return MIXER_RETURN_ERR;
    }
    ev.id.id = elem_info->id.id;
    ev.value.integer.value[0] = value;
    if(ioctl(mixer->fd, AUDIO_CTL_IOCTL_ELEM_WRITE, &ev) < 0) {
        return MIXER_RETURN_ERR;
    }
    LOGD(LOG_TAG, "%s:%d, set [%s, %d] successfully", __func__, __LINE__, name, value);
    return MIXER_RETURN_SUCCESS;
}

int aos_mixer_get_int_value(aos_mixer_t *mixer, char *name, int *value)
{
    int i = 0;
    struct audio_ctl_elem_info * elem_info = NULL;
    struct audio_ctl_elem_value ev;
    if(!mixer || !name) {
        return MIXER_RETURN_ERR;
    }
    for(i = 0; i < mixer->count; i++) {
        elem_info = mixer->elem_info + i;
        if(!strcmp(elem_info->id.name, name)) {
            break;
        }
    }
    if(i >= mixer->count) {
        return MIXER_RETURN_ERR;
    }
    if(AOS_CTL_ELEM_TYPE_INTEGER != elem_info->type) {
        return MIXER_RETURN_ERR;
    }
    ev.id.id = elem_info->id.id;
    if(ioctl(mixer->fd, AUDIO_CTL_IOCTL_ELEM_READ, &ev) < 0) {
        return MIXER_RETURN_ERR;
    }
    *value = ev.value.integer.value[0];
    LOGD(LOG_TAG, "%s:%d, get [%s, %d] successfully", __func__, __LINE__, name, *value);
    return MIXER_RETURN_SUCCESS;
}

int aos_mixer_close(aos_mixer_t *mixer)
{
    if(!mixer) {
        LOGE(LOG_TAG, "%s:%d, invalid mixer", __func__, __LINE__);
        return MIXER_RETURN_ERR;
    }
    if(mixer->fd >= 0) {
        close(mixer->fd);
        mixer->fd = -1;
    }
    if(mixer->name) {
        free(mixer->name);
        mixer->name = NULL;
    }
    if(mixer->elem_info) {
        free(mixer->elem_info);
        mixer->elem_info = NULL;
    }
    free(mixer);
    mixer = NULL;
    LOGD(LOG_TAG, "%s:%d, close mixer successfully", __func__, __LINE__);
    return MIXER_RETURN_SUCCESS;
}

int aos_set_master_volume(int volume)
{
    return aos_mixer_common_set_int_value(AOS_MASTER_VOLUME_TX_STRING, volume);
}

int aos_get_master_volume(int *volume)
{
    return aos_mixer_common_get_int_value(AOS_MASTER_VOLUME_TX_STRING, volume);
}

int aos_set_mute_state(int mute)
{
    return aos_mixer_common_set_int_value(AOS_MASTER_MUTE_STATE_STRING, mute);
}

int aos_get_mute_state(int *mute)
{
    return aos_mixer_common_get_int_value(AOS_MASTER_MUTE_STATE_STRING, mute);
}

void aos_mixer_test(char *pbuffer, int outlen, int argc, char **argv)
{
    int volume = -1;
    if (argc < 1) {
        LOGE(LOG_TAG, "%s:%d: Usage: %s", __func__, __LINE__, argv[0]);
        return;
    }
    aos_set_master_volume(55);
    aos_get_master_volume(&volume);
    aos_set_mute_state(1);
}