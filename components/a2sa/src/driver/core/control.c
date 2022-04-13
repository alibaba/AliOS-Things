/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <ulog/ulog.h>
#include <errno.h>
#include <string.h>
#include "control.h"

#define LOG_TAG  "[control]"
#define MAX_CONTROL_COUNT	1028

static int audio_ctl_new(struct audio_kcontrol **kctl, unsigned int count, unsigned int access)
{
	unsigned int size;
	unsigned int idx;
	
	if (count == 0 || count > MAX_CONTROL_COUNT) {
		return -EINVAL;
	}

	size  = sizeof(struct audio_kcontrol) + sizeof(struct audio_kcontrol_volatile) * count;
	*kctl = malloc(size);
	if (!*kctl) {
		return -ENOMEM;
	}
	(*kctl)->count = count;

	for (idx = 0; idx < count; idx++) {
		(*kctl)->vd[idx].access = access;
	}
	return 0;
}

static struct audio_kcontrol *audio_ctl_new1(const struct audio_kcontrol_new *ncontrol, void *private_data)
{
	struct audio_kcontrol *kctl;
	unsigned int count;
	unsigned int access;
	int err;
	
	if (!ncontrol) {
        LOGE(LOG_TAG, "%s:%d, ncontrol is null", __func__, __LINE__);
        return NULL;
	}
	if (!ncontrol->info) {
        LOGE(LOG_TAG, "%s:%d, control info is null", __func__, __LINE__);
        return NULL;
	}
	count = ncontrol->count;
	if (count == 0)
		count = 1;

	access = ncontrol->access;
	if (access == 0)
		access = AOS_CTL_ELEM_ACCESS_READWRITE;
	access &= AOS_CTL_ELEM_ACCESS_READWRITE;

	err = audio_ctl_new(&kctl, count, access);
	if (err < 0) {
		return NULL;
	}

	/* The 'numid' member is decided when calling audio_ctl_add(). */
	kctl->id.iface = ncontrol->iface;
	kctl->id.deviceId = ncontrol->deviceId;
	kctl->id.subdeviceId = ncontrol->subdeviceId;
	if (ncontrol->name) {
		strlcpy((char*)kctl->id.name, (const char*)ncontrol->name, sizeof(kctl->id.name));
	}
	kctl->id.index = ncontrol->index;
	kctl->info = ncontrol->info;
	kctl->get = ncontrol->get;
	kctl->put = ncontrol->put;
	kctl->tlv.p = ncontrol->tlv.p;
	kctl->private_value = ncontrol->private_value;
	kctl->private_data = private_data;

	return kctl;
}

static struct audio_kcontrol *audio_soc_cnew(const struct audio_kcontrol_new *_template, void *data, const char *long_name)
{
	struct audio_kcontrol_new template;
	struct audio_kcontrol *kcontrol;

	memcpy(&template, _template, sizeof(template));
	template.index = 0;

	if (long_name) {
		template.name = long_name;
	}

	kcontrol = audio_ctl_new1(&template, data);
	return kcontrol;
}

static int audio_ctl_add(ctrl_device_t *dev, struct audio_kcontrol *kcontrol)
{
	struct audio_mixer_control *private_value = NULL;
	if (! kcontrol) {
		return -EINVAL;
	}
	if (!dev || !kcontrol->info) {
        LOGE(LOG_TAG, "%s:%d, card or kcontrol is null", __func__, __LINE__);
        goto __error__;
    }

	dlist_add_tail(&kcontrol->list, &dev->kcontrol_list);
	dev->kcontrols_count += kcontrol->count;
	kcontrol->id.id = dev->last_numid + 1;
	dev->last_numid += kcontrol->count;
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->count: %d", __func__, __LINE__, kcontrol->count);
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->id.id: %d", __func__, __LINE__, kcontrol->id.id);
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->id.index: %d", __func__, __LINE__, kcontrol->id.index);
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->id.iface: %d", __func__, __LINE__, kcontrol->id.iface);
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->id.name: %s", __func__, __LINE__, kcontrol->id.name);
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->id.device/subdevice: %d/%d", __func__, __LINE__, kcontrol->id.deviceId, kcontrol->id.subdeviceId);
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->vd[0].access: %d", __func__, __LINE__, kcontrol->vd[0].access);
	LOGD(LOG_TAG, "%s:%d, ----kcontrol->tlv.p: 0x%x", __func__, __LINE__, kcontrol->tlv.p);
	private_value = (struct audio_mixer_control *)kcontrol->private_value;
	if(private_value) {
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->min: 0x%x", __func__, __LINE__, private_value->min);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->max: 0x%x", __func__, __LINE__, private_value->max);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->platform_max: 0x%x", __func__, __LINE__, private_value->platform_max);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->reg: 0x%x", __func__, __LINE__, private_value->reg);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->rreg: 0x%x", __func__, __LINE__, private_value->rreg);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->shift: 0x%x", __func__, __LINE__, private_value->shift);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->rshift: 0x%x", __func__, __LINE__, private_value->rshift);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->sign_bit: 0x%x", __func__, __LINE__, private_value->sign_bit);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->invert: 0x%x", __func__, __LINE__, private_value->invert);
		LOGD(LOG_TAG, "%s:%d, ----kcontrol->private_value->autodisable: 0x%x", __func__, __LINE__, private_value->autodisable);
	}

	return 0;

 __error__:
	if(kcontrol->private_free) {
		kcontrol->private_free(kcontrol);
	}
	free(kcontrol);
	return -EINVAL;
}

/**************************************************************************
 ** audio driver interface
**************************************************************************/
int audio_add_controls(ctrl_device_t *dev, const struct audio_kcontrol_new *controls, int num_controls, void *data)
{
	int err, i;
	for (i = 0; i < num_controls; i++) {
		const struct audio_kcontrol_new *control = &controls[i];
		err = audio_ctl_add(dev, audio_soc_cnew(control, data, control->name));
		if (err < 0) {
            LOGE(LOG_TAG, "%s:%d, Failed to add %s: %d", __func__, __LINE__, control->name, err);
			return err;
		}
	}
	return 0;
}

/**************************************************************************
 ** audio service interface
**************************************************************************/
static struct audio_kcontrol *audio_ctl_find_numid(ctrl_device_t *dev, unsigned int numid)
{
	struct audio_kcontrol *kctl = NULL;

	if (!dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return NULL;
    }
    dlist_for_each_entry(&dev->kcontrol_list, kctl, struct audio_kcontrol, list) {
		if (kctl->id.id <= numid && kctl->id.id + kctl->count > numid) {
            return kctl;
        }
    }
	return NULL;
}

static struct audio_kcontrol *audio_ctl_find_id(ctrl_device_t *dev, struct audio_ctl_elem_id *id)
{
	struct audio_kcontrol *kctl = NULL;

	if (!dev || !id) {
        LOGE(LOG_TAG, "%s:%d, dev or id is null", __func__, __LINE__);
        return NULL;
    }
	if (id->id != 0) {
        return audio_ctl_find_numid(dev, id->id);
    }
	dlist_for_each_entry(&dev->kcontrol_list, kctl, struct audio_kcontrol, list) {
		if (kctl->id.iface != id->iface)
			continue;
		if (kctl->id.deviceId != id->deviceId)
			continue;
		if (kctl->id.subdeviceId != id->subdeviceId)
			continue;
		if (strncmp((const char*)kctl->id.name, (const char*)id->name, sizeof(kctl->id.name)))
			continue;
		if (kctl->id.index > id->index)
			continue;
		if (kctl->id.index + kctl->count <= id->index)
			continue;
		return kctl;
	}
	return NULL;
}

int audio_ctl_card_info(ctrl_device_t *dev, struct audio_ctl_card_info *info)
{
	if (!dev || !info) {
        LOGE(LOG_TAG, "%s:%d, dev or info is null", __func__, __LINE__);
        return -EINVAL;
    }
	info->card = dev->id;
	strlcpy((char*)info->shortName, (const char*)dev->name, sizeof(info->shortName));
	LOGD(LOG_TAG, "%s:%d, ----audio_ctl_card_info->card: %d", __func__, __LINE__, info->card);
	LOGD(LOG_TAG, "%s:%d, ----audio_ctl_card_info->shortName: %s", __func__, __LINE__, info->shortName);
	return 0;
}


int audio_ctl_elem_list(ctrl_device_t *dev, struct audio_ctl_elem_list *list)
{
    struct audio_kcontrol *kctl;
    struct audio_ctl_elem_id *dst, *id;
    unsigned int offset, space, jidx;
    struct dlist_s *node;

    if(!dev || !list) {
        LOGE(LOG_TAG, "%s:%d, dev or list is null", __func__, __LINE__);
        return -EINVAL;
    }

    offset = list->offset;
    space = list->space;
	LOGD(LOG_TAG, "%s:%d, ----offset: %d, space: %d", __func__, __LINE__, offset, space);
    if(space > 16384) {
        LOGE(LOG_TAG, "%s:%d, unexpected space size", __func__, __LINE__);
        return -ENOMEM;
    }

    list->count = dev->kcontrols_count;
    list->used = 0;

    if(space > 0) {
        dst = malloc(space * sizeof(struct audio_ctl_elem_id));
        if(!dst) {
            LOGE(LOG_TAG, "%s:%d, dst is null", __func__, __LINE__);
            return -ENOMEM;
        }
        
        /* 1. find the node by count offset */
        node = dev->kcontrol_list.next;
        while(node != &dev->kcontrol_list) {
            if(0 == offset) {
                break;
            }
            kctl = aos_container_of(node, struct audio_kcontrol, list);
            if(offset < kctl->count) {
                break;
            }
            offset -= kctl->count;
            node = node->next;
        }

        /* 2. copy all kctl->id to dst[] */
        id = dst;
        while(space > 0 && node != &dev->kcontrol_list) {
            kctl = aos_container_of(node, struct audio_kcontrol, list);
            for (jidx = offset; space > 0 && jidx < kctl->count; jidx++) {
                /* copy kctl->id to dst[xx] */
				*id = kctl->id;
                id->index += jidx;
                id->id += jidx;
				LOGD(LOG_TAG, "%s:%d, ----id->numid: %d", __func__, __LINE__, id->id);
				LOGD(LOG_TAG, "%s:%d, ----id->index: %d", __func__, __LINE__, id->index);
				LOGD(LOG_TAG, "%s:%d, ----id->iface: %d", __func__, __LINE__, id->iface);
				LOGD(LOG_TAG, "%s:%d, ----id->name: %s", __func__, __LINE__, id->name);
				LOGD(LOG_TAG, "%s:%d, ----id->device/subdevice: %d/%d", __func__, __LINE__, id->deviceId, id->subdeviceId);
				id++;
				space--;
				list->used++;
			}
			node = node->next;
			offset = 0;
        }

        /* 3. copy dst[] to list */
        memcpy(list->pids, dst, list->used * sizeof(struct audio_ctl_elem_id));
    }
    return 0;
}

int audio_ctl_elem_info(ctrl_device_t *dev, struct audio_ctl_elem_info *info)
{
    int ret = -1;
    struct audio_kcontrol *kctl = NULL;

    if(!dev || !info) {
        LOGE(LOG_TAG, "%s:%d, dev or info is null", __func__, __LINE__);
        return -EINVAL;
    }
    kctl = audio_ctl_find_id(dev, &info->id);
    if(!kctl) {
        LOGE(LOG_TAG, "%s:%d, no matched kctl", __func__, __LINE__);
        return -ENOENT;
    }

    if(kctl->info) {
        ret = kctl->info(kctl, info);
    }
	LOGD(LOG_TAG, "%s:%d, ----id->numid: %d", __func__, __LINE__, info->id.id);
	LOGD(LOG_TAG, "%s:%d, ----id->index: %d", __func__, __LINE__, info->id.index);
	LOGD(LOG_TAG, "%s:%d, ----id->iface: %d", __func__, __LINE__, info->id.iface);
	LOGD(LOG_TAG, "%s:%d, ----id->name: %s", __func__, __LINE__, info->id.name);
    return ret;
}

int audio_ctl_elem_read(ctrl_device_t *dev, struct audio_ctl_elem_value *value)
{
    int ret = -1;
    struct audio_kcontrol *kctl;
    struct audio_kcontrol_volatile *vd;

	if(!dev || !value) {
        LOGE(LOG_TAG, "%s:%d, dev or info is null", __func__, __LINE__);
        return -EINVAL;
    }

    kctl = audio_ctl_find_id(dev, &value->id);
    if(!kctl) {
        LOGE(LOG_TAG, "%s:%d, no matched kctl", __func__, __LINE__);
        return -ENOENT;
    }
    vd = &kctl->vd[0];
    if ((vd->access & AOS_CTL_ELEM_ACCESS_READ) && kctl->get != NULL) {
		//audio_ctl_build_ioff(&value->id, kctl, index_offset);
        value->id = kctl->id;
		ret = kctl->get(kctl, value);
	}
    return ret;
}

int audio_ctl_elem_write(ctrl_device_t *dev, struct audio_ctl_elem_value *value)
{
    int ret = -1;
    struct audio_kcontrol *kctl;
    struct audio_kcontrol_volatile *vd;

	if(!dev || !value) {
        LOGE(LOG_TAG, "%s:%d, dev or value is null", __func__, __LINE__);
        return -EINVAL;
    }

    kctl = audio_ctl_find_id(dev, &value->id);
    if(!kctl) {
        LOGE(LOG_TAG, "%s:%d, no matched kctl", __func__, __LINE__);
        return -ENOENT;
    }
    vd = &kctl->vd[0];
    if ((vd->access & AOS_CTL_ELEM_ACCESS_WRITE) && kctl->put != NULL) {
		//audio_ctl_build_ioff(&value->id, kctl, index_offset);
        value->id = kctl->id;
		ret = kctl->put(kctl, value);
	}
    return ret;
}

int audio_ctl_tlv_ioctl(ctrl_device_t *dev, struct audio_ctl_tlv *tlv, int op_flag)
{
    int ret = -1;
	struct audio_kcontrol *kctl;

	if(!dev || !tlv) {
        LOGE(LOG_TAG, "%s:%d, dev or tlv is null", __func__, __LINE__);
        return -EINVAL;
    }

    kctl = audio_ctl_find_numid(dev, tlv->numid);
    if(!kctl) {
        LOGE(LOG_TAG, "%s:%d, no matched kctl", __func__, __LINE__);
        return -ENOENT;
    }

    if(!kctl->tlv.c) {
        LOGE(LOG_TAG, "%s:%d, kctl->tlv.c is null", __func__, __LINE__);
        return -EPERM;
    }
    ret = kctl->tlv.c(kctl, op_flag, tlv->len, tlv->tlv);
    return ret;
}

int get_integer_info(struct audio_kcontrol * kcontrol, struct audio_ctl_elem_info * uinfo)
{
	struct audio_mixer_control *mc = (struct audio_mixer_control *)kcontrol->private_value;
	int platform_max;

	if (!mc->platform_max)
		mc->platform_max = mc->max;
	platform_max = mc->platform_max;

	if (platform_max == 1 && !strstr((const char*)kcontrol->id.name, " Volume"))
		uinfo->type = AOS_CTL_ELEM_TYPE_BOOLEAN;
	else
		uinfo->type = AOS_CTL_ELEM_TYPE_INTEGER;

	uinfo->id = kcontrol->id;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = platform_max - mc->min;
	return 0;
}

