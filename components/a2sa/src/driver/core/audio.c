/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/vfs.h>
#include <ulog/ulog.h>
#include "audio_drv.h"
#include "pcm_dev.h"
#include "control.h"
#include "control_dev.h"

#define LOG_TAG     "[audio]"
#define AUDIO_DEVICE_NAME_LEN   50

AOS_DLIST_HEAD(dev_head);

static int audio_pcm_open(inode_t *inode, file_t *file)
{
	int ret = RET_ERR;
    pcm_device_t *pcm = NULL;
    audio_device_t *dev = NULL;
    if(!inode || !file) {
        LOGE(LOG_TAG, "%s:%d, inode is null", __func__, __LINE__);
        return -EINVAL;
    }
    dev = audio_get_device(inode->i_name);
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, not found audio device", __func__, __LINE__);
        return -EINVAL;
    }
    pcm = (pcm_device_t *)(dev->private_data);
    if(!pcm || !pcm->ops) {
        LOGE(LOG_TAG, "%s:%d, private_date is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm->ops->open(pcm->private_data);
    LOGD(LOG_TAG, "%s:%d, open audio dev %s %s", __func__, __LINE__, inode->i_name, (ret == RET_SUCCESS)? "success" : "failed");
	return ret;
}

static int audio_pcm_close(file_t *file)
{
    int ret = RET_ERR;
    pcm_device_t *pcm = NULL;
    audio_device_t *dev = NULL;
    if(!file) {
        LOGE(LOG_TAG, "%s:%d, file is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!file->node) {
        LOGE(LOG_TAG, "%s:%d, file->node is null", __func__, __LINE__);
        return -EINVAL;
    }
    dev = audio_get_device(file->node->i_name);
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, not found audio device", __func__, __LINE__);
        return -EINVAL;
    }
    pcm = (pcm_device_t *)(dev->private_data);
    if(!pcm && !pcm->ops) {
        LOGE(LOG_TAG, "%s:%d, private_date is null", __func__, __LINE__);
        return -EINVAL;
    }
    ret = pcm->ops->close(pcm->private_data);
	return ret;
}

static inline inode_t *file_inode(const file_t *file)
{
	return file->node;
}

static int audio_pcm_ioctl(file_t *file, int cmd, unsigned long arg)
{
    int ret = RET_ERR;
    pcm_device_t *pcm = NULL;
    audio_device_t *dev = NULL;
    inode_t *inode = NULL;

    if(!file) {
        LOGE(LOG_TAG, "%s:%d, file is null.", __func__, __LINE__);
        return -EINVAL;
    }
    inode = file->node;
    if(!inode) {
        LOGE(LOG_TAG, "%s:%d, inode is null.", __func__, __LINE__);
        return -EINVAL;
    }

    dev = audio_get_device(inode->i_name);
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, no matched device", __func__, __LINE__);
        return -EINVAL;
    }
    pcm = (pcm_device_t *)(dev->private_data);
    if(!pcm && !pcm->ops) {
        LOGE(LOG_TAG, "%s:%d, private_date is null", __func__, __LINE__);
        return -EINVAL;
    }
    switch(cmd) {
        case AUDIO_PCM_IOCTL_HW_PARAMS:
            ret = pcm->ops->hw_params(pcm->private_data, (audio_hw_params_t *)arg);
            break;
        case AUDIO_PCM_IOCTL_SW_PARAMS:
            ret = pcm->ops->sw_params(pcm->private_data, (audio_sw_params_t *)arg);
            break;
        case AUDIO_PCM_IOCTL_PREPARE:
            ret = pcm->ops->hw_prepare(pcm->private_data);
            break;
        case AUDIO_PCM_IOCTL_START:
            ret = pcm->ops->start(pcm->private_data);
            break;
        case AUDIO_PCM_IOCTL_READI_FRAMES:
            ret = pcm->ops->readi(pcm->private_data, (audio_xferi_t *)arg);
            break;
        case AUDIO_PCM_IOCTL_READN_FRAMES:
            ret = pcm->ops->readn(pcm->private_data, (audio_xfern_t *)arg);
            break;
        case AUDIO_PCM_IOCTL_WRITEI_FRAMES:
            ret = pcm->ops->writei(pcm->private_data, (audio_xferi_t *)arg);
            break;
        case AUDIO_PCM_IOCTL_WRITEN_FRAMES:
            ret = pcm->ops->writen(pcm->private_data, (audio_xfern_t *)arg);
            break;
        case AUDIO_PCM_IOCTL_DROP:
            ret = pcm->ops->stop(pcm->private_data);
            break;
        case AUDIO_PCM_IOCTL_DRAIN:
            ret = pcm->ops->drain(pcm->private_data);
            break;
        case AUDIO_PCM_IOCTL_PAUSE:
            ret = pcm->ops->pause(pcm->private_data, (int)arg);
            break;
        case AUDIO_PCM_IOCTL_SUSPEND:
            ret = pcm->ops->suspend(pcm->private_data);
            break;
        case AUDIO_PCM_IOCTL_RESUME:
            ret = pcm->ops->resume(pcm->private_data);
            break;
        case AUDIO_PCM_IOCTL_RECOVER:
            ret = pcm->ops->recover(pcm->private_data);
            break;
        default:
            break;
    }
    //LOGD(LOG_TAG, "%s:%d, ioctl audio dev %s %s", __func__, __LINE__, inode->i_name, (ret == RET_SUCCESS)? "success" : "failed");
    return ret;
}

static int audio_ctrl_open(inode_t *inode, file_t *file)
{
    ctrl_device_t *ctrl = NULL;
    audio_device_t *dev = NULL;
    if(!inode || !file) {
        LOGE(LOG_TAG, "%s:%d, inode is null", __func__, __LINE__);
        return -EINVAL;
    }
    dev = audio_get_device(inode->i_name);
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, no matched device", __func__, __LINE__);
        return -EINVAL;
    }
    ctrl = (ctrl_device_t *)(dev->private_data);
    if(!ctrl) {
        LOGE(LOG_TAG, "%s:%d, ctrl dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(true == ctrl->ctrl_dev_state) {
        LOGE(LOG_TAG, "%s:%d, ctrl device already opened", __func__, __LINE__);
        return -EACCES;
    }
    ctrl->ctrl_dev_state = true;

    LOGD(LOG_TAG, "%s:%d, open device %s successfully!!", __func__, __LINE__, inode->i_name);
	return RET_SUCCESS;
}

static int audio_ctrl_close(file_t *file)
{
    int ret = RET_SUCCESS;
    ctrl_device_t *ctrl = NULL;
    audio_device_t *dev = NULL;

    if(!file) {
        LOGE(LOG_TAG, "%s:%d, file is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(!file->node) {
        LOGE(LOG_TAG, "%s:%d, file->node is null", __func__, __LINE__);
        return -EINVAL;
    }
    dev = audio_get_device(file->node->i_name);
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, no matched device", __func__, __LINE__);
        return -EINVAL;
    }
    ctrl = (ctrl_device_t *)(dev->private_data);
    if(!ctrl) {
        LOGE(LOG_TAG, "%s:%d, ctrl dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    if(false == ctrl->ctrl_dev_state) {
        LOGE(LOG_TAG, "%s:%d, ctrl device already closed", __func__, __LINE__);
        return -EACCES;
    }
    ctrl->ctrl_dev_state = false;

    LOGD(LOG_TAG, "%s:%d, close audio dev %s %s", __func__, __LINE__, file->node->i_name, (ret == RET_SUCCESS)? "success" : "failed");
	return ret;
}
static int audio_ctrl_ioctl(file_t *file, int cmd, unsigned long arg)
{
    int ret = -1;
    ctrl_device_t *ctrl = NULL;
    audio_device_t *dev = NULL;
    inode_t *inode = NULL;

    if(!file) {
        LOGE(LOG_TAG, "%s:%d, file is null.", __func__, __LINE__);
        return -EINVAL;
    }
    inode = file->node;
    if(!inode) {
        LOGE(LOG_TAG, "%s:%d, inode is null.", __func__, __LINE__);
        return -EINVAL;
    }

    dev = audio_get_device(inode->i_name);
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, no matched device", __func__, __LINE__);
        return -EINVAL;
    }
    ctrl = (ctrl_device_t *)(dev->private_data);
    if(!ctrl) {
        LOGE(LOG_TAG, "%s:%d, private_data is null", __func__, __LINE__);
        return -EINVAL;
    }

    switch(cmd) {
        case AUDIO_CTL_IOCTL_CARD_INFO:
            ret = audio_ctl_card_info(ctrl, (struct audio_ctl_card_info *)arg);
            break;
        case AUDIO_CTL_IOCTL_ELEM_LIST:
            ret = audio_ctl_elem_list(ctrl, (struct audio_ctl_elem_list *)arg);
            break;
        case AUDIO_CTL_IOCTL_ELEM_INFO:
            ret = audio_ctl_elem_info(ctrl, (struct audio_ctl_elem_info *)arg);
            break;
        case AUDIO_CTL_IOCTL_ELEM_READ:
            ret = audio_ctl_elem_read(ctrl, (struct audio_ctl_elem_value *)arg);
            break;
        case AUDIO_CTL_IOCTL_ELEM_WRITE:
            ret = audio_ctl_elem_write(ctrl, (struct audio_ctl_elem_value *)arg);
            break;
        case AUDIO_CTL_IOCTL_TLV_READ:
            ret = audio_ctl_tlv_ioctl(ctrl, (struct audio_ctl_tlv *)arg, AOS_CTL_TLV_OP_READ);
            break;
        case AUDIO_CTL_IOCTL_TLV_WRITE:
            ret = audio_ctl_tlv_ioctl(ctrl, (struct audio_ctl_tlv *)arg, AOS_CTL_TLV_OP_WRITE);
            break;
        case AUDIO_CTL_IOCTL_TLV_CMD:
            ret = audio_ctl_tlv_ioctl(ctrl, (struct audio_ctl_tlv *)arg, AOS_CTL_TLV_OP_CMD);
            break;
        default:
            LOGD(LOG_TAG, "%s:%d, unsupported cmd 0x%x", __func__, __LINE__, cmd);
            break;
    }

    //LOGD(LOG_TAG, "%s:%d, file path %s, cmd 0x%x", __func__, __LINE__, inode->i_name, cmd);
    return ret;
}

file_ops_t audio_fops[2] = {
    {
        .open  = audio_pcm_open,
        .close = audio_pcm_close,
        .ioctl = audio_pcm_ioctl,
    },
    {
        .open  = audio_ctrl_open,
        .close = audio_ctrl_close,
        .ioctl = audio_ctrl_ioctl,
    },
};

int audio_register_device(int type, const char *name, void *private_data)
{
    int ret = RET_ERR;
    audio_device_t *new_dev = NULL, *node = NULL;

    if(!name || !private_data) {
        LOGE(LOG_TAG, "%s:%d, invalid name or private_data.", __func__, __LINE__);
        return -EINVAL;
    }
    if((type < AUDIO_DEVICE_TYPE_PCM_CAPTURE) || (type > AUDIO_DEVICE_TYPE_CONTROL)) {
        LOGE(LOG_TAG, "%s:%d, invalid type %d.", __func__, __LINE__, type);
        return -EINVAL;
    }

    dlist_for_each_entry(&dev_head, node, audio_device_t, list) {
        if((node->type == type) && (node->private_data == private_data)) {
            LOGE(LOG_TAG, "%s:%d, adevice %d 0x%x already existed.", __func__, __LINE__, type, private_data);
            return -EINVAL;
        }
    }

    new_dev = (audio_device_t *)malloc(sizeof(audio_device_t));
    if(!new_dev) {
        LOGE(LOG_TAG, "%s:%d, new_dev is null.", __func__, __LINE__);
        return -ENOMEM;
    }
    memset(new_dev, 0, sizeof(audio_device_t));

    new_dev->name = strdup(name);
    if(!new_dev->name) {
        LOGE(LOG_TAG, "%s:%d, name is null.", __func__, __LINE__);
        return -ENOMEM;
    }
    dlist_init(&new_dev->list);
    new_dev->type = type;
    new_dev->private_data = private_data;
    if(type == AUDIO_DEVICE_TYPE_PCM_CAPTURE || type == AUDIO_DEVICE_TYPE_PCM_PLAYBACK) {
        new_dev->f_ops = &audio_fops[0];
    } else if (type == AUDIO_DEVICE_TYPE_CONTROL){
        new_dev->f_ops = &audio_fops[1];
    }

    ret = aos_register_driver(new_dev->name, new_dev->f_ops, NULL);
    if(ret < 0) {
        LOGE(LOG_TAG, "%s:%d, register %s failed, ret %d", __func__, __LINE__, new_dev->name, ret);
        if(new_dev->name)
            free(new_dev->name);
        if(new_dev)
            free(new_dev);
        return ret;
    }
    dlist_add_tail(&new_dev->list, &dev_head);
    LOGD(LOG_TAG, "%s:%d, register device %s successfully", __func__, __LINE__, new_dev->name);

    return RET_SUCCESS;
}

int audio_unregister_device(audio_device_t *dev)
{
    audio_device_t *node = NULL;
    if(!dev) {
        return -EINVAL;
    }
    dlist_for_each_entry(&dev_head, node, audio_device_t, list) {
        if((node->type == dev->type) && (!strcmp(node->name, dev->name) && (node->private_data == dev->private_data))) {
            dlist_del(&node->list);
            aos_unregister_driver(node->name);
            if(node->name)
                free(node->name);
            if(node->private_data)
                free(node->private_data);
            if(node)
                free(node);
            return RET_SUCCESS;
        }
    }
    return -EINVAL;
}

audio_device_t *audio_get_device(const char *name)
{
    audio_device_t *node = NULL;
    if(!name) {
        LOGE(LOG_TAG, "%s:%d, name is null.", __func__, __LINE__);
        return NULL;
    }
    dlist_for_each_entry(&dev_head, node, audio_device_t, list) {
        if (strcmp(node->name, name) == 0) {
            return node;
        }
    }
    LOGE(LOG_TAG, "%s:%d, not found audio device %s.", __func__, __LINE__, name);
    return NULL;
}