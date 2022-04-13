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
#include "aos/vfs/pwm.h"

#include "hal_pwm.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_cmu.h"
#include "pmu.h"

#define PWM_DRV_TAG "VFS_PWM_DRV"

#define _HAL_PWM_MAX_NUM 4
static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[_HAL_PWM_MAX_NUM] = {
	{HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
};

typedef struct {
	uint8 chan;
	int start;
	struct HAL_PWM_CFG_T pwm_cfg;
} vfs_pwm_obj_t;


#ifdef LOG
#undef LOG
#define LOG printf
#endif

static int vfs_pwm_open(inode_t *node, file_t *file);
static int vfs_pwm_close(file_t *file);
static ssize_t vfs_pwm_read(file_t *f, void *buf, size_t len);
static ssize_t vfs_pwm_write(file_t *f, const void *buf, size_t len);
static int vfs_pwm_poll(file_t *f, int flag, poll_notify_t notify, void *fd, void *arg);
static int vfs_pwm_ioctl(file_t *f, int cmd, unsigned long arg);

file_ops_t vfs_pwm_drv_fops = {
	.open  = vfs_pwm_open,
	.close = vfs_pwm_close,
	.read  = vfs_pwm_read,
	.write = vfs_pwm_write,
	.poll  = vfs_pwm_poll,
	.ioctl = vfs_pwm_ioctl,
};

/**
 *
 * @param node
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 */

static int vfs_pwm_open(inode_t *node, file_t *file) {
	vfs_pwm_obj_t *obj = NULL;

	if (!(node && file)) {
		LOGE(PWM_DRV_TAG, "%s invalid node[%p] or file[%p]\n", __func__, node, file);
		return -EINVAL;
	}

	LOGD(PWM_DRV_TAG, "%s start to open %s\n", __func__, node->i_name);
	obj = malloc(sizeof(vfs_pwm_obj_t));
	if (!obj) {
		LOGE(PWM_DRV_TAG, "%s malloc for vfs_pwm_obj_t failed\n", __func__);
		return -ENOMEM;
	}
	obj->chan = (enum HAL_PWM_ID_T)node->i_arg;
	obj->start = 0;
	obj->pwm_cfg.freq = 1000;
	obj->pwm_cfg.ratio = 50;
	obj->pwm_cfg.inv = false;
	obj->pwm_cfg.sleep_on = false;
	LOGD(PWM_DRV_TAG, "%s chan=%d, ratio=%d, freq=%d\n", __func__, obj->chan, obj->pwm_cfg.ratio, obj->pwm_cfg.freq);
	file->f_arg = (void *)obj;

	return 0;
}

/**
 *
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 *
 */
static int vfs_pwm_close(file_t *file) {
	vfs_pwm_obj_t *obj = NULL;
	if (!file) {
		LOGE(PWM_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	obj = (vfs_pwm_obj_t *)file->f_arg;
	LOGD(PWM_DRV_TAG, "%s chan=%d, ratio=%d, freq=%d\n", __func__, obj->chan, obj->pwm_cfg.ratio, obj->pwm_cfg.freq);

	if (obj->start == 1) {
		if (hal_pwm_disable(obj->chan) == 0) {
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
static ssize_t vfs_pwm_read(file_t *file, void *buf, size_t len) {
	if (!file) {
		LOGE(PWM_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	LOGE(PWM_DRV_TAG, "%s TBD!!\n", __func__);
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
static ssize_t vfs_pwm_write(file_t *file, const void *buf, size_t len) {
	if (!(file && buf && len)) {
		LOGE(PWM_DRV_TAG, "%s invalid file[%p], buf[%p] or len[%d]\n", __func__, file, buf, len);
		return -EIO;
	}

	LOGE(PWM_DRV_TAG, "%s TBD!!\n", __func__);
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
static int vfs_pwm_poll(file_t *file, int flag, poll_notify_t notify, void *fd, void *arg) {
	vfs_pwm_obj_t *obj = NULL;

	if (!file) {
		LOGE(PWM_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	LOGE(PWM_DRV_TAG, "%s TBD!!\n", __func__);
	return 0;
}


/**
 *
 * @param file
 * @param cmd
 * @param arg
 * @return 0 if operation success; negative error code if operation fails
 */
static int vfs_pwm_ioctl(file_t *file, int cmd, unsigned long arg) {
	vfs_pwm_obj_t *obj = NULL;
	if (!file) {
		LOGE(PWM_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	if (cmd < 0 || cmd >= IOCTL_PWM_MAX) {
		LOGE(PWM_DRV_TAG, "%s invalid command:%d\n", __func__, file, cmd);
		return -EINVAL;
	}

	obj = (vfs_pwm_obj_t *)file->f_arg;
	LOGD(PWM_DRV_TAG, "%s chan=%d, ratio=%d, freq=%d, cmd=%d, arg=%u\n", __func__, obj->chan, obj->pwm_cfg.ratio, obj->pwm_cfg.freq, cmd, arg);

    switch (cmd) {
		case IOCTL_PWM_ON:
			if (obj->start == 0) {
				if (hal_pwm_enable(obj->chan, &obj->pwm_cfg) == 0) {
					obj->start = 1;
				}
			}
			break;
		case IOCTL_PWM_OFF:
			if (obj->start == 1) {
				if (hal_pwm_disable(obj->chan) == 0) {
					obj->start = 0;
				}
			}
			break;
        case IOCTL_PWM_FREQ:
			obj->pwm_cfg.freq = arg;
			if (obj->start == 1) {
				hal_pwm_enable(obj->chan, &obj->pwm_cfg);
			}
			break;
        case IOCTL_PWM_DUTY_CYCLE:
			obj->pwm_cfg.ratio = arg;
			if (obj->start == 1) {
				hal_pwm_enable(obj->chan, &obj->pwm_cfg);
			}
			break;
		default:
			break;
    }

	return 0;
}

static int vfs_pwm_drv_register(void)
{
	int ret = 0;
	ret = aos_register_driver(VFS_PWM_PATH0, &vfs_pwm_drv_fops, HAL_PWM_ID_0);
	if (ret) {
		return -1;
	}
	ret = aos_register_driver(VFS_PWM_PATH1, &vfs_pwm_drv_fops, HAL_PWM_ID_1);
	if (ret) {
		return -1;
	}	
	ret = aos_register_driver(VFS_PWM_PATH2, &vfs_pwm_drv_fops, HAL_PWM_ID_2);
	if (ret) {
		return -1;
	}	
	ret = aos_register_driver(VFS_PWM_PATH3, &vfs_pwm_drv_fops, HAL_PWM_ID_3);
	if (ret) {
		return -1;
	}

	hal_iomux_init(&pinmux_pwm, sizeof(pinmux_pwm)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
	for (int i=0; i<_HAL_PWM_MAX_NUM; i++) {
		hal_gpio_pin_set_dir(pinmux_pwm[i].pin, HAL_GPIO_DIR_OUT, 1);
	}

	hal_pwm_set_ratio_max(1000);

	return 0;
}

int vfs_pwm_drv_init(void) {
	int ret = 0;

	ret = vfs_pwm_drv_register();

	if (ret) {
		LOGD(PWM_DRV_TAG, "%s success \n", __func__);
	}

	return ret;
}

void _vfs_pwm_test(void) {
	uint32_t ratio = 0;

	vfs_pwm_drv_init();

	// test led
	char *dev_name = VFS_PWM_PATH2;
	int fd = open(dev_name, O_RDWR);
	LOGD(PWM_DRV_TAG, "%s open %s fd=%d\n", __func__, dev_name, fd);
	for (int j=0; j<13; j++) {
		ioctl(fd, IOCTL_PWM_FREQ, 1000*(j+1));
		ioctl(fd, IOCTL_PWM_DUTY_CYCLE, 0);
		ioctl(fd, IOCTL_PWM_ON, 0);
		for (int i=0; i<10; i++) {
			ratio = 100*i;
			ioctl(fd, IOCTL_PWM_DUTY_CYCLE, ratio);
			osDelay(200);
		}
		for (int i=10; i>0; i--) {
			ratio = 100*i;
			ioctl(fd, IOCTL_PWM_DUTY_CYCLE, ratio);
			osDelay(200);
		}
		ioctl(fd, IOCTL_PWM_OFF, 0);
	}
	close(fd);
	LOGD(PWM_DRV_TAG, "%s close fd=%d\n", __func__, fd);
}
