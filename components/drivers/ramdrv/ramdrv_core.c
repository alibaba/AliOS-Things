/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <hal2vfs/io_ramdrv.h>
#include <devicevfs/devicevfs.h>

#define PLATFORM_RAMDRV_NUM  4

#if (PLATFORM_RAMDRV_NUM > 0)

// magic number used for user_data validation
#define RAMDRV_MAGIC_NUMBER 0xBABAA107
// RAMDRV device node will be named with "/dev/ramdrv<x>", where <x> is ramdrv port id
#define RAMDRV_DEV_NAME_FORMAT "ramdrv%d"

typedef struct ramdrv_dev {
    unsigned int id;
} ramdrv_dev_t;

typedef struct vfs_ramdrv {
    int ref_cnt;
    ramdrv_dev_t dev;
    unsigned int magic_num;
} vfs_ramdrv_t;

ssize_t ramdrv_device_read (file_t *fp, void *buf, size_t nbytes) {
    int i = 0;
    char *p = buf;

    if (!fp || !buf || nbytes <= 0) {
         ddkc_err("invalid parameter detected, fp:%p, buf:%p, nbytes:%d\r\n", fp, buf, nbytes);
         return -EINVAL;
    }
    ddkc_info("fp:%p, buf:%p, nbytes:%d\r\n", fp, buf, nbytes);
    /* check whether all contents in buf equals with RAMDRV_MAGIC_NUM or not 
     * yes - test pass
     * eno - test fail
     * always return the size from which position on the contents in buf does not equal with RAMDRV_MAGIC_NUM
     * */
    for (i = 0; i < nbytes; i++) {
        *p++ = RAMDRV_MAGIC_NUM;
    }

    if (i != nbytes) {
        ddkc_err("read ops failed, [i:%d] != [nbytes:%d]\r\n", i, nbytes);
    }

    ddkc_info("return %d\r\n", i);

    return i;

}


ssize_t ramdrv_device_write (file_t *fp, const void *buf, size_t nbytes) {
    int i = 0;
    char *p = (char *)buf;

    if (!fp || !buf || nbytes <= 0) {
         ddkc_err("invalid parameter detected, fp:%p, buf:%p, nbytes:%d\r\n", fp, buf, nbytes);
         return -EINVAL;
    }

    /* fill the buf with RAMDRV_MAGIC_NUM */
    for (i = 0; i < nbytes; i++) {
        if (*p++ != RAMDRV_MAGIC_NUM)
            break;
    }

    ddkc_info("return %d\r\n", i);

    return i;
}

int ramdrv_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    io_ramdrv_arg_t *ramdrv_arg = NULL;
    char *fname = f->node->i_name;

    // arg is channel info
    ddkc_dbg("ioctl on %s with cmd:0x%x, arg:0x%lx \r\n", fname, cmd, arg);

    switch (cmd) {
        case IOC_RAMDRV_LOOPBACK:
            ramdrv_arg = (io_ramdrv_arg_t *)arg;
            BUG_ON_MSG(!ramdrv_arg, "IOC_RAMDRV_LOOPBACK - arg should not be NULL\r\n");
            aos_ipc_copy(ramdrv_arg->rbuf, ramdrv_arg->wbuf, sizeof(ramdrv_arg->rbuf));
            break;

        default:
            ddkc_err("invalid cmd:%d\r\n", cmd);
            break;
    }

    return ret;
}

static uint32_t ramdrv_device_lseek(file_t *f, int64_t off, int32_t whence){
    ddkc_info("f->offset:0x%x, off:0x%llx, whence:%d\r\n", f->offset, off, whence);
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

int ramdrv_device_open (inode_t *node, file_t *f) {
    vfs_ramdrv_t *vd = NULL;

    ddkc_info("device:%s open succeed in pid:%d\r\n", node->i_name, getpid());
    ddkc_info("node->i_arg:%p\r\n", node->i_arg);

    vd = (vfs_ramdrv_t *)node->i_arg;
    if (vd->magic_num == (RAMDRV_MAGIC_NUMBER + vd->dev.id))
        return 0;

    ddkc_err("user_data:%p, calculated magic_num:0x%x != magic_num:0x%x\r\n",
             vd, RAMDRV_MAGIC_NUMBER + vd->dev.id, vd->magic_num);

    return -EIO;
}

int ramdrv_device_close (file_t *f) {

    ddkc_info("device:%s close succeed\r\n", f->node->i_name);

    return 0;
}


/************************** device ****************************/


subsys_file_ops_t ramdrv_device_fops = {
    .open = ramdrv_device_open,
    .close = ramdrv_device_close,
    .read = ramdrv_device_read,
    .write = ramdrv_device_write,
    .ioctl = ramdrv_device_ioctl,
    .lseek = ramdrv_device_lseek,
    .poll = NULL,
};

int ramdrv_device_init (struct u_platform_device *pdev) {
    vfs_ramdrv_t *vd = (vfs_ramdrv_t *)u_platform_get_user_data(pdev);

    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_info("%s, pid:%d\r\n", __func__, getpid());
    ddkc_info("user_data:%p, magic_num:0x%x\r\n", vd, vd->magic_num);
    ddkc_info("cal magic_num:0x%x\r\n", RAMDRV_MAGIC_NUMBER + vd->dev.id);
    if (vd->magic_num == (RAMDRV_MAGIC_NUMBER + vd->dev.id))
        return 0;

    ddkc_err("user_data:%p, calculated magic_num:0x%x != magic_num:0x%x\r\n",
             vd, RAMDRV_MAGIC_NUMBER + vd->dev.id, vd->magic_num);

    return -EINVAL;
}

int ramdrv_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int ramdrv_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}


struct subsys_drv ramdrv_device_drv = {
    .drv_name = "ramdrv",
    .init = ramdrv_device_init,
    .deinit = ramdrv_device_deinit,
    .pm = ramdrv_device_pm,
};

struct subsys_dev *g_ramdrv_device_array[PLATFORM_RAMDRV_NUM];


int vfs_ramdrv_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_info("ramdrv vfs driver init starts\r\n");

    node_name_len = strlen(RAMDRV_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    memset(g_ramdrv_device_array, 0, sizeof(g_ramdrv_device_array));
    ppsdev = g_ramdrv_device_array;

    for (i = 0; i < PLATFORM_RAMDRV_NUM; i++) {
        vfs_ramdrv_t *vd = NULL;
#ifdef AOS_KERNEL_MODE
        if ((i != 0) && (i != 1))
            continue;
#else
        if ((i == 0) || (i == 1))
            continue;
#endif
       vd = malloc(sizeof(vfs_ramdrv_t));
        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);
        if (!(*ppsdev) || !vd) {
            ddkc_info("malloc failed, *ppsdev:%p, vd:%p\r\n", *ppsdev, vd);

            if (*ppsdev) {
                free(*ppsdev);
                *ppsdev = NULL;
            }

            if (vd)
                free(vd);

            goto err;
        }

        memset(*ppsdev, 0, sizeof(struct subsys_dev) + node_name_len);
        memset(vd, 0, sizeof(*vd));
        // vfs_ramdrv_t's port should be remained during the whole driver life
        vd->dev.id = i;
        vd->magic_num = RAMDRV_MAGIC_NUMBER + vd->dev.id;

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, RAMDRV_DEV_NAME_FORMAT, i);
        ddkc_info("*ppsdev:%p, node_name:%s, (*ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        ddkc_info("user_data:%p, magic_num:0x%x\r\n", vd, vd->magic_num);
        (*ppsdev)->user_data = vd;
        (*ppsdev)->delay_init = true;
        if (i % 2) {
            ret = aos_dev_reg(*ppsdev, &ramdrv_device_fops, &ramdrv_device_drv);
        } else {
            ret = aos_remote_dev_reg(*ppsdev, &ramdrv_device_fops, &ramdrv_device_drv);
        }
        if (ret) {
            ddkc_err("aos_dev_reg for ramdrv%d failed, ret:%d\r\n", i, ret);

            free(vd);
            free(*ppsdev);
            *ppsdev = NULL;
            goto err;
        }

        ppsdev++;
    }

    ddkc_info("ramdrv vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ppsdev = g_ramdrv_device_array;
    for (j = 0; j < i; j++) {
        // shall uninstall ramdrv devices who are already registered
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for ramdrv%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("ramdrv vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

int vfs_ramdrv_drv_deinit (void) {
    int i = 0;
    struct subsys_dev **ppsdev = NULL;
    ppsdev = g_ramdrv_device_array;

    for (i = 0; i < PLATFORM_RAMDRV_NUM; i++) {
        // shall uninstall ramdrv devices who are already registered
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for ramdrv%d\r\n", i);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }

    return 0;

}

#ifndef AOS_KERNEL_MODE
VFS_DRIVER_FULL_ENTRY(vfs_ramdrv_drv_init, NULL)
#endif
#endif
