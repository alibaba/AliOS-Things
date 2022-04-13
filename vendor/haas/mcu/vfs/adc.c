/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
 
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
 #include "aos/vfs.h"
#include "posix/poll.h"
#include "aos/vfs/adc.h"

 #include "hal_trace.h"
 #include "hal_gpadc.h"
 
#define ADC_DRV_TAG "VFS_ADC_DRV"
 
typedef struct {
    uint8 chan;
    int start;
} vfs_adc_obj_t;


#ifdef LOG
#undef LOG
#define LOG printf
#endif

static int vfs_adc_open(inode_t *node, file_t *file);
static int vfs_adc_close(file_t *file);
static ssize_t vfs_adc_read(file_t *f, void *buf, size_t len);
static ssize_t vfs_adc_write(file_t *f, const void *buf, size_t len);
static int vfs_adc_poll(file_t *f, int flag, poll_notify_t notify, void *fd, void *arg);
static int vfs_adc_ioctl(file_t *f, int cmd, unsigned long arg);

file_ops_t vfs_adc_drv_fops = {
	.open  = vfs_adc_open,
	.close = vfs_adc_close,
	.read  = vfs_adc_read,
	.write = vfs_adc_write,
	.poll  = vfs_adc_poll,
	.ioctl = vfs_adc_ioctl,
};

/**
 *
 * @param node
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 */

static int vfs_adc_open(inode_t *node, file_t *file) {
	vfs_adc_obj_t *obj = NULL;

	if (!(node && file)) {
		LOGE(ADC_DRV_TAG, "%s invalid node[%p] or file[%p]\n", __func__, node, file);
		return -EINVAL;
	}

	LOGD(ADC_DRV_TAG, "%s start to open %s\n", __func__, node->i_name);
	obj = malloc(sizeof(vfs_adc_obj_t));
	if (!obj) {
		LOGE(ADC_DRV_TAG, "%s malloc for vfs_adc_obj_t failed\n", __func__);
		return -ENOMEM;
	}
	obj->chan = (enum HAL_GPADC_CHAN_T)node->i_arg;
	obj->start = 0;

	hal_gpadc_open(obj->chan, HAL_GPADC_ATP_NULL, NULL);

	LOGD(ADC_DRV_TAG, "%s chan=%d\n", __func__, obj->chan);
	file->f_arg = (void *)obj;

	return 0;
 }
 
/**
 *
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int vfs_adc_close(file_t *file) {
	vfs_adc_obj_t *obj = NULL;
	if (!file) {
		LOGE(ADC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	obj = (vfs_adc_obj_t *)file->f_arg;
	LOGD(ADC_DRV_TAG, "%s chan=%d\n", __func__, obj->chan);

	if (obj->start == 1) {
		if (hal_gpadc_close(obj->chan) == 0) {
			obj->start = 0;
		}
	}
	if (obj) {
		free(obj);
		file->f_arg = NULL;
	}

	return 0;
}
 
/**
 *
 * @param f
 * @param buf
 * @param len
 * @return bytes number read if operation success; negative error code if operation fails
 *
 */
static ssize_t vfs_adc_read(file_t *file, void *buf, size_t len) {
    vfs_adc_obj_t *obj = NULL;
    HAL_GPADC_MV_T volt = HAL_GPADC_BAD_VALUE;
    if (!file) {
        LOGE(ADC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
        return -EIO;
    }

    obj = (vfs_adc_obj_t *)file->f_arg;
    if (buf && hal_gpadc_get_volt(obj->chan, &volt)) {
        *(uint32_t *)buf = volt;
        LOGD(ADC_DRV_TAG, "%s chan=%d => output=%d\n", __FUNCTION__, obj->chan, *(uint32_t *)buf);
    }

    return 0;
}
 
/**
 *
 * @param f
 * @param buf
 * @param len
 * @return bytes number written if operation success; negative error code if operation fails
 *
 */
static ssize_t vfs_adc_write(file_t *file, const void *buf, size_t len) {
    if (!(file && buf && len)) {
        LOGE(ADC_DRV_TAG, "%s invalid file[%p], buf[%p] or len[%d]\n", __func__, file, buf, len);
        return -EIO;
    }

    LOGE(ADC_DRV_TAG, "%s TBD!!\n", __func__);
    return 0;
}

/**
 *
 * @param f
 * @param flag
 * @param notify
 * @param fd
 * @param arg
 * @return
 *
 */
static int vfs_adc_poll(file_t *file, int flag, poll_notify_t notify, void *fd, void *arg) {
    vfs_adc_obj_t *obj = NULL;

    if (!file) {
        LOGE(ADC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
        return -EIO;
    }

    LOGE(ADC_DRV_TAG, "%s TBD!!\n", __func__);
    return 0;
}


/**
 *
 * @param file
 * @param cmd
 * @param arg
 * @return 0 if operation success; negative error code if operation fails
 */
static int vfs_adc_ioctl(file_t *file, int cmd, unsigned long arg) {
	vfs_adc_obj_t *obj = NULL;
	if (!file) {
		LOGE(ADC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	if (cmd < 0 || cmd >= IOCTL_ADC_MAX) {
		LOGE(ADC_DRV_TAG, "%s invalid command:%d\n", __func__, file, cmd);
		return -EINVAL;
	}

	obj = (vfs_adc_obj_t *)file->f_arg;
	LOGD(ADC_DRV_TAG, "%s chan=%d, cmd=%d, arg=%u\n", __func__, obj->chan, cmd, arg);

	switch (cmd) {
		case IOCTL_ADC_ON:
			if (obj->start == 0) {
				if (hal_gpadc_open(obj->chan, HAL_GPADC_ATP_NULL, NULL) == 0) {
					obj->start = 1;
				}
			}
			break;
		case IOCTL_ADC_OFF:
			if (obj->start == 1) {
				if (hal_gpadc_close(obj->chan) == 0) {
					obj->start = 0;
				}
			}
			break;
		case IOCTL_ADC_FREQ:
			break;
		default:
			break;
    }

    return 0;
}

static int vfs_adc_drv_register(void)
{
	int ret = 0;
	ret = aos_register_driver(VFS_ADC_PATH0, &vfs_adc_drv_fops, HAL_GPADC_CHAN_2);
	if (ret) {
		return -1;
	}
	ret = aos_register_driver(VFS_ADC_PATH1, &vfs_adc_drv_fops, HAL_GPADC_CHAN_3);
	if (ret) {
		return -1;
	}       
	ret = aos_register_driver(VFS_ADC_PATH2, &vfs_adc_drv_fops, HAL_GPADC_CHAN_4);
	if (ret) {
		return -1;
	}

	return 0;
}

int vfs_adc_drv_init(void) {
	int ret = 0;

	ret = vfs_adc_drv_register();

	if (ret) {
		LOGD(ADC_DRV_TAG, "%s success \n", __func__);
	}

	return ret;
}

void _vfs_adc_test(void) {
	uint32_t value = 0;

	vfs_adc_drv_init();

	// test led
	char *dev_name = VFS_ADC_PATH0;
	int fd = open(dev_name, O_RDWR);
	LOGD(ADC_DRV_TAG, "%s open %s fd=%d\n", __func__, dev_name, fd);
	//ioctl(fd, IOCTL_ADC_FREQ, 1000);
	while (1) {
		read(fd, &value, sizeof(uint32_t));
		osDelay(20);
	}
	close(fd);
	LOGD(ADC_DRV_TAG, "%s close fd=%d\n", __func__, fd);
}
