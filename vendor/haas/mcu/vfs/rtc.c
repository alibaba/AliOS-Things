/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal_i2c.h"

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/vfs.h"
#include "posix/poll.h"
#include "aos/vfs/rtc.h"
#include "ulog/ulog.h"
#include "aos/vfs/rtc.h"

#ifdef LOG
#undef LOG
#define LOG printf
#endif
#define RTC_DRV_TAG "VFS_RTC_DRV"
typedef struct
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
} rtc_time;
typedef struct
{
	int i2c_port;
	int slave_address;
	struct HAL_I2C_CONFIG_T i2c_cfg;
} i2c_rtc_obj_t;

typedef struct
{
	aos_mutex_t mutex;
	int style;
	rtc_time rtc_cfg;
	i2c_rtc_obj_t i2c_rtc;
} vfs_rtc_obj_t;

static int vfs_rtc_open(inode_t *node, file_t *file);
static int vfs_rtc_close(file_t *file);
static ssize_t vfs_rtc_read(file_t *f, void *buf, size_t len);
static ssize_t vfs_rtc_write(file_t *f, const void *buf, size_t len);
static int vfs_rtc_poll(file_t *f, int flag, poll_notify_t notify, void *fd, void *arg);
static int vfs_rtc_ioctl(file_t *f, int cmd, unsigned long arg);
static bool mutex_init = false;

file_ops_t vfs_rtc_drv_fops = {
	.open = vfs_rtc_open,
	.close = vfs_rtc_close,
	.read = vfs_rtc_read,
	.write = vfs_rtc_write,
	.poll = vfs_rtc_poll,
	.ioctl = vfs_rtc_ioctl,
};


uint8_t TM_DS1307_Bcd2Bin(uint8_t bcd) {
	uint8_t dec = 10 * (bcd >> 4);
	dec += bcd & 0x0F;
	return dec;
}

uint8_t TM_DS1307_Bin2Bcd(uint8_t bin) {
	uint8_t low = 0;
	uint8_t high = 0;
	
	/* High nibble */
	high = bin / 10;
	/* Low nibble */
	low = bin - (high * 10);
	
	/* Return */
	return high << 4 | low;
}

uint8_t TM_DS1307_CheckMinMax(uint8_t val, uint8_t min, uint8_t max) {
	if (val < min) {
		return min;
	} else if (val > max) {
		return max;
	}
	return val;
}
/**
 *
 * @param node
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 */

static int vfs_rtc_open(inode_t *node, file_t *file)
{
	vfs_rtc_obj_t *rtc_obj = NULL;
	uint32_t ret = 0;
	int32_t lock_ret = -1;
	uint8_t i2c_port;
	if (!(node && file))
	{
		LOGE(RTC_DRV_TAG, "%s invalid node[%p] or file[%p]\n", __func__, node, file);
		return -EINVAL;
	}

	/* just open the /dev/vfs0 node here */
	if (!(strlen(node->i_name) == strlen(VFS_RTC_PATH)) &&
		(strncmp(RTC_DRV_TAG, node->i_name, strlen(node->i_name))))
	{
		LOGE(RTC_DRV_TAG, "%s incorrect device node path:%s\n", __func__, node->i_name);
		return -EINVAL;
	}

	LOGD(RTC_DRV_TAG, "%s start to open %s\n", __func__, node->i_name);
	rtc_obj = (vfs_rtc_obj_t *)malloc(sizeof(*rtc_obj));
	if (!rtc_obj)
	{
		LOGE(RTC_DRV_TAG, "%s malloc for vfs_rtc_obj_t failed\n", __func__);
		return -ENOMEM;
	}
	/* do your own operation here, usually power on hardware, local variable/resource init, etc. */
	if (node->i_arg == I2C_STYLE)
	{ // rtc used i2c io
		rtc_obj->style = I2C_STYLE;
		if (mutex_init == false)
		{
			aos_mutex_new(&rtc_obj->mutex);
			mutex_init = true;
		}

		lock_ret = aos_mutex_lock(&rtc_obj->mutex, AOS_WAIT_FOREVER);
		if (lock_ret != 0)
		{
			LOGE(RTC_DRV_TAG, "get i2c_mutex lock fail");
			return lock_ret;
		}

		i2c_port = rtc_obj->i2c_rtc.i2c_port = 1;

		if (i2c_port == 0)
		{
			hal_iomux_set_i2c0();
		}
		else if (i2c_port == 1)
		{
			hal_iomux_set_i2c1();
		}

		rtc_obj->i2c_rtc.i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
		rtc_obj->i2c_rtc.i2c_cfg.use_dma = 0;
		rtc_obj->i2c_rtc.i2c_cfg.use_sync = 1;
		rtc_obj->i2c_rtc.i2c_cfg.speed = 100000;
		rtc_obj->i2c_rtc.i2c_cfg.as_master = 1;
		ret = hal_i2c_open(i2c_port, &rtc_obj->i2c_rtc.i2c_cfg);
		if (ret)
		{
			LOGE(RTC_DRV_TAG, "open i2c fail\n");
		}
		else
		{
			LOGE(RTC_DRV_TAG, "open i2c succ.\n");
		}
		aos_mutex_unlock(&rtc_obj->mutex);
	}
	/* if you want to use some variable or object later in read/write/poll, store it via file->f_arg */
	file->f_arg = (void *)&rtc_obj;

	return 0;
}

/**
 *
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 *
 */
static int vfs_rtc_close(file_t *file)
{
	int32_t lock_ret = -1;
	vfs_rtc_obj_t *rtc_obj = NULL;
	if (!file)
	{
		LOGE(RTC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	rtc_obj = (vfs_rtc_obj_t *)file->f_arg;
	/* do your own operation here, usually do hardware power off, variable/resource deinit, etc. */
	if (rtc_obj->style == I2C_STYLE){
		lock_ret = aos_mutex_lock(&rtc_obj->mutex, AOS_WAIT_FOREVER);
    	if (lock_ret != 0) {
			LOGE(RTC_DRV_TAG,"i2c_master_recv, get i2c_mutex lock fail");
			return lock_ret;
		}
    	hal_i2c_close(rtc_obj->i2c_rtc.i2c_port);
    	aos_mutex_unlock(&rtc_obj->mutex);
    	free(rtc_obj);
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
static ssize_t vfs_rtc_read(file_t *file, void *buf, size_t len)
{
	vfs_rtc_obj_t *rtc_obj = NULL;
	rtc_time *time_rtc;
	int32_t ret;
	size_t size;
	uint8_t data[7];
	uint8_t rxbuf[2];
	int32_t lock_ret = -1;
	if (!file)
	{
		LOGE(RTC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}
	if (buf == NULL)
	{
		return -1;
	}
	rtc_obj = (vfs_rtc_obj_t *)file->f_arg;
	time_rtc = (rtc_time *)buf;
	size = sizeof(rtc_time);

	if (len < size)
	{
		return -1;
	}
	/* do your own operation here, usually do hardware power off, variable/resource deinit, etc. */
	if (rtc_obj->style == I2C_STYLE)
	{

		uint8_t i2c_port = rtc_obj->i2c_rtc.i2c_port;
		rxbuf[0] = (uint8_t)DS1307_SECONDS; // TODO:CHANGE read rtc time reg_address
		rxbuf[1] = 0;

		lock_ret = aos_mutex_lock(&rtc_obj->mutex, AOS_WAIT_FOREVER);
		if (lock_ret != 0)
		{
			LOGE(RTC_DRV_TAG, "hal_i2c_mem_read, get i2c_mutex lock fail");
			return lock_ret;
		}

		ret = hal_i2c_recv(i2c_port, rtc_obj->i2c_rtc.slave_address, rxbuf, 1, len, HAL_I2C_RESTART_AFTER_WRITE, 0, 0);
		if (ret)
		{
			LOGE(RTC_DRV_TAG, "%s:i2c recv failed,dev_addr = 0x%x,ret = %d\n", __func__, rtc_obj->i2c_rtc.slave_address, ret);
		}
		else
		{
			*data = rxbuf[1];
		}
		aos_mutex_unlock(&rtc_obj->mutex);
		/* Fill data */
		time_rtc->tm_sec = TM_DS1307_Bcd2Bin(data[DS1307_SECONDS]);
    	time_rtc->tm_min = TM_DS1307_Bcd2Bin(data[DS1307_MINUTES]); 	
    	time_rtc->tm_hour = TM_DS1307_Bcd2Bin(data[DS1307_HOURS]); 	
    	time_rtc->tm_mday = TM_DS1307_Bcd2Bin(data[DS1307_DAY]); 	
    	time_rtc->tm_wday = TM_DS1307_Bcd2Bin(data[DS1307_DATE]); 	
    	time_rtc->tm_yday = TM_DS1307_Bcd2Bin(data[DS1307_MONTH]); 	
    	time_rtc->tm_year = TM_DS1307_Bcd2Bin(data[DS1307_YEAR]);
    
    	time_rtc->tm_isdst = aos_now_ms();
		return (int)size;
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
static ssize_t vfs_rtc_write(file_t *file, const void *buf, size_t len)
{
	vfs_rtc_obj_t *rtc_obj = NULL;
	int ret = 0;
	size_t size;
	uint8_t data[7];
	uint8_t txbuf[2];
	uint8_t i2c_port;
	int32_t lock_ret = -1;
	if (!(file && buf && len))
	{
		LOGE(RTC_DRV_TAG, "%s invalid file[%p], buf[%p] or len[%d]\n", __func__, file, buf, len);
		return -EIO;
	}
	rtc_obj = (vfs_rtc_obj_t *)file->f_arg;
	/* do your own operation here, usually do hardware power off, variable/resource deinit, etc. */
	if (rtc_obj->style == I2C_STYLE)
	{
		const rtc_time *time_rtc = (rtc_time *)buf;

		if (buf == NULL)
		{
			return -1;
		}

		size = sizeof(rtc_time);

		if (len < size)
		{
			return -1;
		}
		//TODO:/* Format data */
		data[DS1307_SECONDS] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time_rtc->tm_sec, 0, 59)); 	
    	data[DS1307_MINUTES] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time_rtc->tm_min, 0, 59)); 	
    	data[DS1307_HOURS] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time_rtc->tm_hour, 0, 23)); 	
    	data[DS1307_DAY] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time_rtc->tm_mday, 1, 7)); 	
    	data[DS1307_DATE] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time_rtc->tm_wday, 1, 31)); 	
    	data[DS1307_MONTH] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time_rtc->tm_yday, 1, 12)); 	
    	data[DS1307_YEAR] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time_rtc->tm_year, 0, 99));
		//
		txbuf[0] = (uint8_t)DS1307_I2C_ADDR;//TODO:reg_address
		txbuf[1] = *data;
		i2c_port = rtc_obj->i2c_rtc.i2c_port;

		lock_ret = aos_mutex_lock(&rtc_obj->mutex, AOS_WAIT_FOREVER);
		if (lock_ret != 0)
		{
			LOGE(RTC_DRV_TAG,"hal_i2c_mem_write, get i2c_mutex lock fail");
			return lock_ret;
		}
		ret = hal_i2c_task_send(i2c_port, rtc_obj->i2c_rtc.slave_address, txbuf, size + 1, 0, NULL);
		if (ret)
		{
			LOGE(RTC_DRV_TAG,"%s:%d,i2c send failed,dev_addr = 0x%x,ret = %d\n", __func__, __LINE__, rtc_obj->i2c_rtc.slave_address, ret);
		}
		aos_mutex_unlock(&rtc_obj->mutex);

		return (int)size;
	}
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
static int vfs_rtc_poll(file_t *file, int flag, poll_notify_t notify, void *fd, void *arg)
{
	vfs_rtc_obj_t *obj = NULL;

	if (!file)
	{
		LOGE(RTC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	obj = (vfs_rtc_obj_t *)file->f_arg;
	if (aos_mutex_lock(&obj->mutex, AOS_WAIT_FOREVER) != 0)
	{
		LOGE(RTC_DRV_TAG, "Failed to lock mutex (%s).", __func__);
		return -1;
	}
	aos_mutex_unlock(&obj->mutex);
}

/**
 *
 * @param file
 * @param cmd
 * @param arg
 * @return 0 if operation success; negative error code if operation fails
 */
static int vfs_rtc_ioctl(file_t *file, int cmd, unsigned long arg)
{
	vfs_rtc_obj_t *rtc_obj = NULL;

	if (!file)
	{
		LOGE(RTC_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	rtc_obj = (vfs_rtc_obj_t *)file->f_arg;

	if (cmd < 0 || cmd >= VFS_RTC_IOCTL_MAX)
	{
		LOGE(RTC_DRV_TAG, "%s invalid command:%d\n", __func__, file, cmd);
		return -EINVAL;
	}

	switch (cmd)
	{
	case RTC_RD_TIME:
		break;
	case RTC_SET_TIME:
		break;
	case RTC_SET_I2C_SLAVE:
		rtc_obj->i2c_rtc.slave_address = DS1307_I2C_ADDR;
		break;
	default:
		break;
	}

	return 0;
}

static int vfs_rtc_drv_register(void)
{
	int ret = 0;
	ret = aos_register_driver(VFS_RTC_PATH, &vfs_rtc_drv_fops, I2C_STYLE);
	if (ret)
	{
		return -1;
	}
	return 0;
}

int vfs_rtc_drv_init(void)
{
	int ret = 0;

	ret = vfs_rtc_drv_register();

	if (ret)
	{
		LOGD(RTC_DRV_TAG, "%s success \n", __func__);
	}

	return ret;
}


void _vfs_rtc_test(void)
{
	int fd;
    long mode;
	char *buf;
	rtc_time *time;
	int len = 1024;
	LOGE(RTC_DRV_TAG,"%s\r\n",__FUNCTION__);
    vfs_rtc_drv_init();
    char *dev_name = VFS_RTC_PATH;
    for (size_t i = 0; i < 15; i++)
    {
        /* code */
        fd = open(dev_name,O_RDWR);
        ioctl(fd,0x00,0x00);//must set slave address after open
		osDelay(200);
        write(fd,(void*)time,7);
		osDelay(200);
        close(fd);
		osDelay(2000);
    } 
}
