/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aos/vfs.h>
#include <aos/list.h>
#include <aos/kernel.h>
#include "control.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _IOC_NONE	0U                                 /**< 0x00 */
#define _IOC_WRITE	1U                                 /**< 0x01 */
#define _IOC_READ	2U                                 /**< 0x02 */
#define _IOC_TYPECHECK(t) (sizeof(t))
#define _IOC(dir,type,nr,size) (((dir)  << 30) | \
								((size) << 16) | \
	                            ((type) << 8)  | \
	                            ((nr)   << 0))

/* audio device type */
#define AUDIO_DEVICE_TYPE_PCM_CAPTURE       0
#define AUDIO_DEVICE_TYPE_PCM_PLAYBACK      1
#define AUDIO_DEVICE_TYPE_CONTROL           2

/* pcm device ioctl */
#define _IO(type,nr)		                _IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size)	                _IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOW(type,nr,size)	                _IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOWR(type,nr,size)	                _IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))

#define AUDIO_PCM_IOCTL_HW_PARAMS           _IOWR('A', 0x11, audio_hw_params_t)
#define AUDIO_PCM_IOCTL_SW_PARAMS           _IOWR('A', 0x12, audio_sw_params_t)
#define AUDIO_PCM_IOCTL_PREPARE             _IO('A', 0x40)
#define AUDIO_PCM_IOCTL_START               _IO('A', 0x42)
#define AUDIO_PCM_IOCTL_WRITEI_FRAMES       _IOW('A', 0x50, audio_xferi_t)
#define AUDIO_PCM_IOCTL_READI_FRAMES        _IOR('A', 0x51, audio_xferi_t)
#define AUDIO_PCM_IOCTL_WRITEN_FRAMES       _IOW('A', 0x52, audio_xfern_t)
#define AUDIO_PCM_IOCTL_READN_FRAMES        _IOR('A', 0x53, audio_xfern_t)
#define AUDIO_PCM_IOCTL_DROP                _IO('A', 0x43)   /* looks like only for read */
#define AUDIO_PCM_IOCTL_DRAIN               _IO('A', 0x44)
#define AUDIO_PCM_IOCTL_PAUSE               _IOW('A', 0x45, int)
#define AUDIO_PCM_IOCTL_SUSPEND             _IO('A', 0x46)
#define AUDIO_PCM_IOCTL_RESUME              _IO('A', 0x47)
#define AUDIO_PCM_IOCTL_RECOVER             _IO('A', 0x80)   /* aos private */


/* mixer virtual device ioctl */
#define AUDIO_CTL_IOCTL_CARD_INFO	        _IOR('U', 0x01, struct audio_ctl_card_info)
#define AUDIO_CTL_IOCTL_ELEM_LIST	        _IOWR('U', 0x10, struct audio_ctl_elem_list)
#define AUDIO_CTL_IOCTL_ELEM_INFO	        _IOWR('U', 0x11, struct audio_ctl_elem_info)
#define AUDIO_CTL_IOCTL_ELEM_READ	        _IOWR('U', 0x12, struct audio_ctl_elem_value)
#define AUDIO_CTL_IOCTL_ELEM_WRITE	        _IOWR('U', 0x13, struct audio_ctl_elem_value)
#define AUDIO_CTL_IOCTL_TLV_READ	        _IOWR('U', 0x1a, struct audio_ctl_tlv)
#define AUDIO_CTL_IOCTL_TLV_WRITE	        _IOWR('U', 0x1b, struct audio_ctl_tlv)
#define AUDIO_CTL_IOCTL_TLV_CMD	            _IOWR('U', 0x1c, struct audio_ctl_tlv)


#define RET_ERR                 -1
#define RET_SUCCESS              0

typedef struct {
    int block;                              /**< 0: non-block mode, otherwise block mode, */
    int interleave;                         /**< 0: non-interleave access, otherwise interleave access, */
    int rate;                               /**< sample rate, e.g. 8KHz, 16KHz */
    int channels;                           /**< channel number, e.g. CH1/ CH2 */
    int sample_bits;                        /**< sample size in bit */
} audio_hw_params_t;

typedef struct {
    aos_hdl_t *hdl;                         /**< aos_event_t handler pointer */
    int period;                             /**< sample size in bit */
} audio_sw_params_t;

typedef struct {
	int result;                             /**< transfer result, 0/ SUCCESS, otherwise FAILED */
	void *buf;                              /**< data buffer */
	unsigned int frames;                    /**< data buffer size in frame */
} audio_xferi_t;

typedef struct {
	int result;                             /**< transfer result, 0/ SUCCESS, otherwise FAILED */
	void **bufs;                            /**< data buffer */
	unsigned int frames;                    /**< data buffer size in frame */
} audio_xfern_t;

typedef struct {
    struct dlist_s list;                    /**< audio device node */
    int type;                               /**< audio device type */
	char *name;                             /**< transfer result, 0/ SUCCESS, otherwise FAILED */
    file_ops_t *f_ops;                      /**< file operations */
	void *private_data;                     /**< private data buffer */
} audio_device_t;


int audio_register_device(int type, const char *name, void *private_data);
int audio_unregister_device(audio_device_t *dev);
audio_device_t *audio_get_device(const char *name);


#ifdef __cplusplus
}
#endif
#endif /* AUDIO_H */

