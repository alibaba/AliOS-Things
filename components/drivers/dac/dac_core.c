/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/dac.h>
#include <hal2vfs/io_dac.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_DAC_NUM
#define PLATFORM_DAC_NUM CONFIG_DAC_NUM
#else
#define PLATFORM_DAC_NUM  4
#endif

#if (PLATFORM_DAC_NUM > 0)

// DAC device node will be named with "/dev/dac<x>", where <x> is dac port id
#define DAC_DEV_NAME_FORMAT "dac%d"

typedef struct vfs_dac {
    int ref_cnt;
    dac_dev_t dev;
} vfs_dac_t;

int dac_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    unsigned int channel = 0;
    unsigned int value = 0;
    vfs_dac_t *vd = (vfs_dac_t *)f->node->i_arg;
    dac_dev_t *dac_dev = &vd->dev;

    // arg is channel info
    ddkc_dbg("cmd:0x%x, arg:0x%x\r\n", cmd, arg);

    switch (cmd) {
        case IOC_DAC_START:
            ddkc_dbg("IOC_DAC_START, arg:%d\r\n", arg);
            ret = hal_dac_start(dac_dev, (unsigned int)arg);
            ddkc_dbg("IOC_DAC_START, result:%s\r\n", ret ? "fail" : "success");
            break;

        case IOC_DAC_STOP:
            ret = hal_dac_stop(dac_dev, (unsigned int)arg);
            ddkc_dbg("IOC_DAC_STOP, result:%s\r\n", ret ? "fail" : "success");

            break;

        case IOC_DAC_SET_VALUE:
            channel = GET_DAC_CH(arg);
            value = GET_DAC_VAL(arg);
            ret = hal_dac_set_value(dac_dev, channel, value);
            ddkc_info("IOC_DAC_SET_VALUE, channel:%d, value:%d, result:%s\r\n", channel, value, ret ? "fail" : "success");

            break;

        case IOC_DAC_GET_VALUE:
            channel = GET_DAC_CH(arg);
            ret = hal_dac_get_value(dac_dev, channel);
            ddkc_info("IOC_DAC_GET_VALUE, channel:%d, value:%d\r\n", channel, ret);

            break;
        default:
            ddkc_err("invalid cmd:%d\r\n", cmd);
            break;
    }

    return ret;
}

int dac_device_open (inode_t *node, file_t *f) {
    int ret = 0;
    vfs_dac_t *vd = (vfs_dac_t *)f->node->i_arg;
    dac_dev_t *dac_dev = &vd->dev;

    ret = vd->ref_cnt ? 0 : hal_dac_init(dac_dev);
    if (ret) {
        ddkc_err("%s opened failed, port:%d\r\n", node->i_name, dac_dev->port);
    } else {
        vd->ref_cnt++;
        ddkc_dbg("%s opened success, port:%d\r\n", node->i_name, dac_dev->port);
    }

    return ret;
}

int dac_device_close (file_t *f) {
    int ret = 0;
    vfs_dac_t *vd = (vfs_dac_t *)f->node->i_arg;
    dac_dev_t *dac_dev = &vd->dev;

    // anyway decrease by 1
    vd->ref_cnt--;
    ret = vd->ref_cnt ? hal_dac_finalize(dac_dev) : 0;

    if (ret) {
        ddkc_err("%s close failed, port:%d\r\n", f->node->i_name, dac_dev->port);
    } else {
        ddkc_dbg("%s closed success, port:%d\r\n", f->node->i_name, dac_dev->port);
    }

    return 0;
}


/************************** device ****************************/


subsys_file_ops_t dac_device_fops = {
    .open = dac_device_open,
    .close = dac_device_close,
    .read = NULL,
    .write = NULL,
    .ioctl = dac_device_ioctl,
    .poll = NULL,
};

int dac_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    printk("%s\r\n", __func__);
    return 0;
}

int dac_device_deinit (struct u_platform_device *pdev) {
    printk("%s\r\n", __func__);
    return 0;
}

int dac_device_suspend (struct u_platform_device *pdev, u_pm_ops_t state) {
    printk("%s\r\n", __func__);
    return 0;
}

struct subsys_drv dac_device_drv = {
    .drv_name = "dac",
    .init = dac_device_init,
    .deinit = dac_device_deinit,
    .shutdown = dac_device_shutdown,
    .suspend = dac_device_suspend,
    .resume = dac_device_resume,
};

struct subsys_dev *g_dac_device_array[PLATFORM_DAC_NUM];


int vfs_dac_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_info("dac vfs driver init starts\r\n");

    node_name_len = strlen(DAC_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    memset(g_dac_device_array, 0, sizeof(g_dac_device_array));
    ppsdev = g_dac_device_array;

    for (i = 0; i < PLATFORM_DAC_NUM; i++) {
        vfs_dac_t *vd = malloc(sizeof(vfs_dac_t));

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
        // vfs_dac_t's port should be remained during the whole driver life
        vd->dev.port = i;

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, DAC_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, (*ppsdev) + 1:%p, node_name:%s, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = vd;

        ret = aos_dev_reg(*ppsdev, &dac_device_fops, &dac_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for dac%d failed, ret:%d\r\n", i, ret);

            free(vd);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_info("dac vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ppsdev = g_dac_device_array;
    for (j = 0; j < i; j++) {
        // shall uninstall dac devices who are already registered

        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for dac%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("dac vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_dac_drv_init)

__weak int32_t hal_dac_init(dac_dev_t *dac) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_dac_start(dac_dev_t *dac, uint32_t channel) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}


__weak int32_t hal_dac_stop(dac_dev_t *dac, uint32_t channel) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_dac_get_value(dac_dev_t *dac, uint32_t channel) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_dac_finalize(dac_dev_t *dac){
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}
#endif
