#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os.h"

#include "hal_i2c.h"
#include "hal_trace.h"
#include "aos/vfs/i2c.h"
#include "aos/vfs.h"
#include "aos/errno.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "lcd_i2c.h"   
    
typedef struct {
    uint16_t dev_addr;      /**< slave device addr */
    uint32_t address_width; /**< Addressing mode: 7 bit or 10 bit */
    aos_mutex_t    mutex;
    int port;
    struct HAL_I2C_CONFIG_T i2c_cfg;
} vfs_i2c_obj_t;

static int vfs_i2c_open(inode_t *node, file_t *file);
static ssize_t vfs_i2c_read(file_t *file,uint8_t *buf,size_t len);
static ssize_t vfs_i2c_write(file_t *file, const void *buf,size_t len);
static int vfs_i2c_ioctl(file_t *file, int cmd,unsigned long arg);
static int vfs_i2c_close(file_t *file);


file_ops_t i2c_fops = {
    .open = vfs_i2c_open,
    .read = vfs_i2c_read,
    .write = vfs_i2c_write,
    .close = vfs_i2c_close,
    .ioctl = vfs_i2c_ioctl,
};

/**
 *
 * @param node
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 */
int vfs_i2c_open(inode_t *node,file_t *file)
{
    int ret;
    int32_t lock_ret = -1;
    vfs_i2c_obj_t *i2c_obj;
    //TRACE("%s\r\n",__FUNCTION__);
    if (!(node && file)) {
		LOGE(I2C_DRV_TAG, "%s invalid node[%p] or file[%p]\n", __func__, node, file);
		return -EINVAL;
	}
	int i2c_port = (int)node->i_arg;
    /* just open the /dev/vfs0 node here */
	if (!(strncmp(VFS_I2C_0_PATH, node->i_name, strlen(node->i_name))) &&
		!(strncmp(VFS_I2C_1_PATH, node->i_name, strlen(node->i_name)))) {
		LOGE(I2C_DRV_TAG, "%s incorrect device node path:%s\n", __func__, node->i_name);
		return -EINVAL;
	}
    i2c_obj = (vfs_i2c_obj_t *)malloc(sizeof(*i2c_obj));
    if (!i2c_obj) {
		LOGE(I2C_DRV_TAG, "%s malloc for vfs_sample_obj_t failed\n", __func__);
		return -ENOMEM;
	}
    aos_mutex_new(&i2c_obj->mutex);
    i2c_obj->port                    = i2c_port;
    i2c_obj->address_width           = 8;
    i2c_obj->i2c_cfg.speed           = 100000;
    i2c_obj->i2c_cfg.mode            = HAL_I2C_API_MODE_TASK;
	i2c_obj->i2c_cfg.use_dma         = 0;
	i2c_obj->i2c_cfg.use_sync        = 1;
	//i2c_obj->i2c_cfg->addr_as_slave = 0;
	i2c_obj->i2c_cfg.as_master    = 1;
    lock_ret = aos_mutex_lock(&i2c_obj->mutex, AOS_WAIT_FOREVER);
    if (lock_ret != 0) {
		LOGE(I2C_DRV_TAG,"i2c_master_recv, get i2c_mutex lock fail");
		return lock_ret;
	}
    if (i2c_port == 0) {
		hal_iomux_set_i2c0();
	} else if (i2c_port == 1) {
		hal_iomux_set_i2c1();
	}
    ret = hal_i2c_open(i2c_port, &i2c_obj->i2c_cfg);
	if (ret) {
		LOGE(I2C_DRV_TAG,"open i2c fail\n");
	} else {
		LOGE(I2C_DRV_TAG,"open i2c succ.\n");
	}
    aos_mutex_unlock(&i2c_obj->mutex);
    file->f_arg = (void *)i2c_obj;
    return 0;
}
/**
 *
 * @param file
 * @param buf: in----reg_addr   out------read_data
 * @param len
 * @return bytes number read if operation success; negative error code if operation fails
 *
 */
ssize_t vfs_i2c_read(file_t *file,uint8_t *buf,size_t len)
{
    int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;
	unsigned char data_buf[2] = {0};
    vfs_i2c_obj_t *i2c_obj = (vfs_i2c_obj_t *)file->f_arg;
	lock_ret = aos_mutex_lock(&i2c_obj->mutex, AOS_WAIT_FOREVER);
	if (lock_ret != 0) {
		LOGE(I2C_DRV_TAG,"i2c_master_recv, get i2c_mutex lock fail");
		return lock_ret;
	}
	i2c_port = i2c_obj->port;
	data_buf[0]= 0xA0;
	data_buf[1]= 0;
	ret = hal_i2c_recv(i2c_port, i2c_obj->dev_addr, data_buf, 1, len, HAL_I2C_RESTART_AFTER_WRITE, 0, NULL);
	if(ret) {
		//TRACEME("%s:%d,i2c read failed, dev_addr = 0x%x, data[0] = 0x%x, data[1]= 0x%x, ret = %d\n", __func__,__LINE__,i2c_obj->dev_addr, buf[0], buf[1], ret);
	}
	*buf = data_buf[1];
	aos_mutex_unlock(&i2c_obj->mutex);
	return ret;
}
/**
 *
 * @param f
 * @param buf:in------reg+data
 * @param len
 * @return bytes number written if operation success; negative error code if operation fails
 *
 */
ssize_t vfs_i2c_write(file_t *file,const void *buf, size_t len)
{
    int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;
	char *p_buff = (char *)buf;
	//LOGE(I2C_DRV_TAG,"%s\r\n",__FUNCTION__);
    vfs_i2c_obj_t *i2c_obj = (vfs_i2c_obj_t *)file->f_arg;
	lock_ret = aos_mutex_lock(&i2c_obj->mutex, AOS_WAIT_FOREVER);
	if (lock_ret != 0) {
		LOGE(I2C_DRV_TAG,"i2c_master_send, get i2c_mutex lock fail");
		return lock_ret;
	}
	i2c_port = i2c_obj->port;
	ret = hal_i2c_task_send(i2c_port, *p_buff, p_buff+1, len, 0, NULL);
	if(ret) {
		LOGE(I2C_DRV_TAG,"%s:%d,i2c send fail, dev_addr = 0x%x, data[0] = 0x%x, data[1]= 0x%x, ret = %d\n", __func__,__LINE__,*p_buff, *(p_buff+1), *(p_buff+2), ret);
	}
	aos_mutex_unlock(&i2c_obj->mutex);
	return len;
}
/**
 *
 * @param file
 * @param cmd
 * @param arg
 * @return 0 if operation success; negative error code if operation fails
 */
int vfs_i2c_ioctl(file_t *file,int cmd, unsigned long arg)
{
    vfs_i2c_obj_t *i2c_obj = (vfs_i2c_obj_t *)file->f_arg;
    if (!file) {
		LOGE(I2C_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	if (cmd < 0 || cmd >= I2C_IOCTL_MAX) {
		LOGE(I2C_DRV_TAG, "%s invalid command:%d\n", __func__, file, cmd);
		return -EINVAL;
	}

	switch (cmd) {
		case I2C_SET_SLAVE:
			i2c_obj->dev_addr = (uint16_t)arg;
			break;
		case I2C_SLAVE_FORCE:
			
			break;
		case I2C_FUNCS:
			
			break;
		default:
			break;
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
int vfs_i2c_poll(file_t *file, int flag, poll_notify_t notify, void *fd, void *arg) {
	vfs_i2c_obj_t *obj = NULL;

	if (!file) {
		LOGE(I2C_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	obj = (vfs_i2c_obj_t *)file->f_arg;
	if (aos_mutex_lock(&obj->mutex, AOS_WAIT_FOREVER) != 0) {
		LOGE(I2C_DRV_TAG, "Failed to lock mutex (%s).", __func__);
		return -1;
	}
	aos_mutex_unlock(&obj->mutex);
}

/**
 *
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 *
 */
int vfs_i2c_close(file_t *file)
{
  vfs_i2c_obj_t *i2c_obj = NULL;
  int lock_ret;
	if (!file) {
		LOGE(I2C_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	i2c_obj = (vfs_i2c_obj_t *)file->f_arg;
	/* do your own operation here, usually do hardware power off, variable/resource deinit, etc. */
    lock_ret = aos_mutex_lock(&i2c_obj->mutex, AOS_WAIT_FOREVER);
    if (lock_ret != 0) {
		LOGE(I2C_DRV_TAG,"i2c_master_recv, get i2c_mutex lock fail");
		return lock_ret;
	}
    hal_i2c_close(i2c_obj->port);
    aos_mutex_unlock(&i2c_obj->mutex);
    free(i2c_obj);
	return 0;
}


static int vfs_i2c_drv_register(void)
{
	int ret;
	//LOGE(I2C_DRV_TAG,"%s i2c0 %d\r\n",__FUNCTION__,CHIP_HAS_I2C);
    ret = aos_register_driver(VFS_I2C_0_PATH, &i2c_fops, HAL_I2C_ID_0);
#if (CHIP_HAS_I2C > 1)
	//LOGE(I2C_DRV_TAG,"%s i2c1\r\n",__FUNCTION__);
    ret = aos_register_driver(VFS_I2C_1_PATH, &i2c_fops, HAL_I2C_ID_1);
#endif
    return ret;
}

void vfs_i2c_drv_init(void)
{
    int ret = 0;
	//LOGE(I2C_DRV_TAG,"%s\r\n",__FUNCTION__);
	ret = vfs_i2c_drv_register();
	if (ret) {
		LOGD(I2C_DRV_TAG, "%s success \n", __func__);
	}
	return ret;
}


void _vfs_i2c_test(void)
{
    int fd;
    long mode;
	char *buf;
	int len = 128;
	uint16_t Addr;
	uint8_t WriteBuffer[128],ReadBuffer[128];
	LOGE(I2C_DRV_TAG,"%s\r\n",__FUNCTION__);
    vfs_i2c_drv_init();
	for(Addr=1; Addr<128; Addr++){
		WriteBuffer[Addr]=Addr-1;	 /* Ìî³äWriteBuffer */
	}
	fd = open(VFS_I2C_1_PATH,O_RDWR);
	WriteBuffer[0] = 0xA0;
	WriteBuffer[1] = 0;
	//ioctl(fd,I2C_SET_SLAVE,0xA0);
	//ioctl(fd,I2C_SET_SLAVE_REG,0x00);
	write(fd,WriteBuffer,sizeof(WriteBuffer));
	//oled_main();
	osDelay(5);
	read(fd,ReadBuffer,sizeof(ReadBuffer));
	for (size_t i = 0; i < 128; i++)
	{
		LOGE(I2C_DRV_TAG,"%x %x\r\n",WriteBuffer[i],ReadBuffer[i]);
	}
	
}