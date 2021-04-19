/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <aos/hal/flash.h>
#include <vfsdev/flash_dev.h>
#include <devicevfs/devicevfs.h>

#define PLATFORM_FLASH_NUM  HAL_PARTITION_MAX
// FLASH device node will be named with "/dev/flash<x>", where <x> is flash port id
#define FLASH_DEV_NAME_FORMAT "flash%d"

__weak int32_t hal_flash_init(hal_partition_t in_partition) {
    ddkc_warn("weak %s\r\n", __FUNCTION__);
    return 0;
}

#ifdef AOS_MCU_OTA_ADAPT
// temp usage
#define IOCTL_FLASH_BOOT_VALIDATE() \
    if (in_partition == HAL_PARTITION_LITTLEFS) { \
        ddkc_warn("fs is not allowed to access boot operations\r\n"); \
    return -EINVAL; \
    }

    extern int ota_set_user_bootinfo(void *param);
    extern int ota_clear_reboot_count(void);
    extern int ota_get_boot_type();
#endif
/*
offset set by lseek
*/
ssize_t flash_device_read (file_t *f, void *buffer, size_t size) {
    int ret;
    ssize_t s = 0;
    hal_partition_t in_partition = (hal_partition_t)f->node->i_arg;
    size_t offset = f->offset;

    if (!buffer || !size) {
        ddkc_warn("invalid flash:%d, offset:%d, buffer:%p or size:%d\r\n", in_partition, offset, buffer, size);
        return -EINVAL;
    }

    ddkc_dbg("f->offset:%d, buffer:%p, size:%d\r\n", f->offset, buffer, size);
    ret = hal_flash_read(in_partition, &f->offset, buffer, size);
    if(ret < 0){
        return -1;
    } else {
        if ((f->offset - offset) != size) {
            ddkc_err("hal_flash_write returns success, but offset is not set\r\n");
        }
    }
    s = f->offset - offset;
    ddkc_dbg("read return %d\r\n", s);
    return s;
}

/*
offset set by lseek
*/
ssize_t flash_device_write (file_t *f, const void *buffer, size_t size) {
    int ret = 0;
    ssize_t s = 0;
    hal_partition_t in_partition = (hal_partition_t)f->node->i_arg;
	size_t offset = f->offset;

    if (!buffer || !size) {
        ddkc_warn("invalid flash:%d, offset:%d, buffer:%p or size:%d\r\n", in_partition, offset, buffer, size);
        return -EINVAL;
    }

    ret = hal_flash_write(in_partition, &f->offset, buffer, size);
    ddkc_dbg("f->offset:%d, ret:%d\r\n", f->offset, ret);

    if(ret < 0){
        return -1;
    } else {
        if ((f->offset - offset) != size) {
            ddkc_err("hal_flash_write returns success, but offset is not set\r\n");
        }
    }
    s = f->offset - offset;
    ddkc_dbg("write return %d\r\n", s);
    return s;
}

static uint32_t flash_device_lseek(file_t *f, int64_t off, int32_t whence){
    ddkc_dbg("f->offset:%x, off:%llx, whence:%d\r\n", f->offset, off, whence);
    switch (whence) {
	    case SEEK_CUR:
	        off = f->offset + off;
	        break;
	    /* case SEEK_END: break; */
	    case SEEK_SET:
	        f->offset = off;
	        break;
    }

    return off;
}

/*
offset set by lseek
*/
int flash_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    hal_partition_t in_partition = (hal_partition_t)f->node->i_arg;
    uint32_t offset = f->offset;
    uint32_t size ;
    hal_logic_partition_t *partition;
    hal_logic_partition_t p;

    if(!arg){
        ddkc_warn("i_name:%s, flash:%d, cmd:%d, arg:0x%lx\r\n", f->node->i_name, in_partition, cmd, arg);
    }

    switch (cmd) {
        case IOC_FLASH_ERASE_FLASH:
            if (!arg)
                return -EINVAL;
            size = (uint32_t)arg;
            ddkc_dbg("IOC_FLASH_ERASE_FLASH i_name:%s, flash:%d, cmd:%d, off_set:0x%x, size:0x%x\r\n", f->node->i_name, in_partition, cmd, offset, size);
            ret = hal_flash_erase(in_partition,offset, size);
            if (ret) {
                ddkc_warn("hal_flash_erase failed, ret:%d\r\n", ret);
                return -1;
            }
            break;
        case IOC_FLASH_INFO_GET:
            if (!arg)
                return -EINVAL;
            partition = (hal_logic_partition_t *)arg;
            ddkc_dbg("IOC_FLASH_INFO_GET i_name:%s, flash:%d, cmd:%d\r\n", f->node->i_name, in_partition, cmd);
            ret = hal_flash_info_get(in_partition, &p);
            if (ret) {
                ddkc_warn("hal_flash_info_get failed, ret:%d\r\n", ret);
                return -1;
            }
            p.partition_description = NULL;
            aos_ipc_copy(partition, &p, sizeof(hal_logic_partition_t));
            break;

        case IOC_FLASH_ENABLE_SECURE:
            size = (uint32_t)arg;
            ddkc_dbg("i_name:%s, flash:%d, cmd:%d, off_set:0x%x, size:0x%x\r\n", f->node->i_name, in_partition, cmd, offset, size);
            ret = hal_flash_enable_secure(in_partition,offset, size);
            if (ret) {
                ddkc_warn("hal_flash_enable_secure failed, ret:%d\r\n", ret);
                return -1;
            }
            break;

        case IOC_FLASH_DISABLE_SECURE:
            size = (uint32_t)arg;
            ddkc_dbg("IOC_FLASH_DISABLE_SECURE i_name:%s, flash:%d, cmd:%d, off_set:0x%x, size:0x%x\r\n", f->node->i_name, in_partition, cmd, offset, size);
            ret = hal_flash_dis_secure(in_partition,offset, size);
            if (ret) {
                ddkc_warn("hal_flash_dis_secure failed, ret:%d\r\n", ret);
                return -1;
            }
            break;

#ifdef AOS_MCU_OTA_ADAPT
        case IOC_FLASH_SET_BOOT_INFO:
            IOCTL_FLASH_BOOT_VALIDATE();
            ret = ota_set_user_bootinfo((void*)arg);
            break;

        case IOC_FLASH_CLEAR_BOOT_COUNT:
            IOCTL_FLASH_BOOT_VALIDATE();
            ret = ota_clear_reboot_count();
            break;

        case IOC_FLASH_GET_BOOT_TYPE:
            IOCTL_FLASH_BOOT_VALIDATE();
            ret = ota_get_boot_type();
            break;
#endif
        default:
            break;
    }

    return ret;
}

int flash_device_open (inode_t *node, file_t *f) {
    ddkc_dbg("open %s done\r\n", node->i_name);
    return 0;
}

int flash_device_close (file_t *f) {
    ddkc_dbg("close %s done\r\n", f->node->i_name);
    return 0;
}


/************************** device ****************************/


subsys_file_ops_t flash_device_fops = {
    .open = flash_device_open,
    .close = flash_device_close,
    .read = flash_device_read,
    .write = flash_device_write,
    .ioctl = flash_device_ioctl,
    .poll = NULL,
    .lseek = flash_device_lseek,
};

int flash_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    int ret = 0;
    void *user_data = NULL;

    user_data = u_platform_get_user_data(pdev);
#if defined(USER_SPACE_DRIVER)
    ret = hal_flash_init((hal_partition_t)user_data);
    ddkc_info("%s hal_flash_init ret:%d\r\n", __func__, ret);
#else
    ddkc_info("%s hal_flash_init USER_SPACE_DRIVER not defined\r\n", __func__);
#endif
    return 0;
}

int flash_device_deinit (struct u_platform_device *pdev) {
    ddkc_info("%s\r\n", __func__);
    return 0;
}

int flash_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_info("%s\r\n", __func__);
    return 0;
}

struct subsys_drv flash_device_drv = {
    .drv_name = "flash",
    .init = flash_device_init,
    .deinit = flash_device_deinit,
    .pm = flash_device_pm,
};

struct subsys_dev *g_flash_device_array[PLATFORM_FLASH_NUM];

extern const size_t hal_partitions_amount;
extern const hal_logic_partition_t hal_partitions[];

int vfs_flash_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_info("flash vfs driver init starts, hal_partitions_amount:%d\r\n", hal_partitions_amount);

    node_name_len = strlen(FLASH_DEV_NAME_FORMAT) + 1;

    memset(g_flash_device_array, 0, sizeof(g_flash_device_array));
    ppsdev = g_flash_device_array;

    for (i = 0; i < hal_partitions_amount; i++) {
        if(0 == hal_partitions[i].partition_length){
            continue;
        }

        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);

        if (!(*ppsdev)) {
            ddkc_info("malloc for subsys_dev failed, \r\n");
            goto err;
        }

        memset(*ppsdev, 0, sizeof(struct subsys_dev) + node_name_len);

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, FLASH_DEV_NAME_FORMAT, i);

        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = (void *)i;

        ret = aos_dev_reg(*ppsdev, &flash_device_fops, &flash_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for flash%d failed, ret:%d\r\n", i, ret);

            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_dbg("flash vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ppsdev = g_flash_device_array;

    for (j = 0; j < i; j++) {
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for flash%d\r\n", j);
            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("flash vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_flash_drv_init)

