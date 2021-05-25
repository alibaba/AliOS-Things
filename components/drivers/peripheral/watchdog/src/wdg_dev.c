/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/driver/wdg.h>
#include <vfsdev/wdg_dev.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_WDG_NUM
#define PLATFORM_WDG_NUM CONFIG_WDG_NUM
#else
#define PLATFORM_WDG_NUM 1
#endif

#if (PLATFORM_WDG_NUM > 0)

// WDG device node will be named with "/dev/wdg<x>", where <x> is wdg port id
#define WDG_DEV_NAME_FORMAT "wdg%d"

typedef struct vfs_wdg {
    unsigned char port;   /**< wdg port */
} vfs_wdg_t;

int wdg_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = -1;
    vfs_wdg_t *vt = (vfs_wdg_t *)f->node->i_arg;
    wdg_dev_handle_t wdg = (wdg_dev_handle_t)f->f_arg;

    if (!vt || !wdg) {
        ddkc_err("%s invalid vt:%p or wdg:%p", vt, wdg);
        return -EINVAL;
    }

    ddkc_dbg("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
        case IOC_WDG_RELOAD:
        case IOC_WDG_FEED:
            ret = aos_wdg_feed(wdg);
            if (ret) {
                ddkc_err("%s feed watchdog failed\r\n", __func__, ret);
            }
            break;
        case IOC_WDG_START:
            ret = aos_wdg_start(wdg);
            if (ret) {
                ddkc_err("%s start watchdog failed\r\n", __func__, ret);
            }
            break;
        case IOC_WDG_STOP:
            ret = aos_wdg_stop(wdg);
            if (ret) {
                ddkc_err("%s start watchdog failed\r\n", __func__, ret);
            }
            break;
        default:
            ddkc_err("%s invalid cmd:%d\r\n", __func__, cmd);
            break;
    }

    return ret;
}

int wdg_device_open (inode_t *node, file_t *f) {
    int ret = 0;
    vfs_wdg_t *vt = (vfs_wdg_t *)f->node->i_arg;
    wdg_dev_handle_t wdg = NULL;

    if (!vt) {
        ddkc_err("%s open failed, invalid i_arg:%p\r\n", node->i_name, vt);
        return -EINVAL;
    }

    wdg = aos_wdg_open(vt->port, 0xFFFFFFFF - 1);
    if (!wdg) {
        printf("i2c%d open failed\r\n", vt->port);
        return -1;
    }

    f->f_arg = wdg;

    ddkc_dbg("%s opened, port:%d\r\n", node->i_name, vt->port);

    return 0;
}

int wdg_device_close (file_t *f) {
    int ret = 0;
    vfs_wdg_t *vt = (vfs_wdg_t *)f->node->i_arg;
    wdg_dev_handle_t wdg = (wdg_dev_handle_t)f->f_arg;

    if (!vt || !wdg) {
        ddkc_err("%s invalid vt:%p or wdg:%p", vt, wdg);
        return -EINVAL;
    }

    ddkc_dbg("close %s, port:%d\r\n", f->node->i_name, vt->port);

    ret = aos_wdg_close(wdg);
    if (ret) {
        ddkc_err("aos_wdg_close on wdg%d failed, ret=%d\r\n", vt->port, ret);
    }
    f->f_arg = NULL;

    return 0;
}


/************************** device ****************************/


subsys_file_ops_t wdg_device_fops = {
    .open = wdg_device_open,
    .close = wdg_device_close,
    .read = NULL,
    .write = NULL,
    .ioctl = wdg_device_ioctl,
    .poll = NULL,
};

int wdg_device_probe (struct u_platform_device *pdev) {
    // make sure 0 is returned if probe operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int wdg_device_remove (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

void wdg_device_shutdown (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return;
}

int wdg_device_suspend (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int wdg_device_resume (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv wdg_device_drv = {
    .drv_name = "wdg",
    .probe = wdg_device_probe,
    .remove = wdg_device_remove,
    .shutdown = wdg_device_shutdown,
    .suspend = wdg_device_suspend,
    .resume = wdg_device_resume,
};

struct subsys_dev *g_wdg_device_array[PLATFORM_WDG_NUM];


int vfs_wdg_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_info("wdg vfs driver init starts\r\n");

    node_name_len = strlen(WDG_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    memset(g_wdg_device_array, 0, sizeof(g_wdg_device_array));
    ppsdev = g_wdg_device_array;

    for (i = 0; i < PLATFORM_WDG_NUM; i++) {
        vfs_wdg_t *vt = malloc(sizeof(vfs_wdg_t));

        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);
        if (!(*ppsdev) || !vt) {
            ddkc_info("malloc failed, *ppsdev:%p, vt:%p\r\n", *ppsdev, vt);

            if (*ppsdev) {
                free(*ppsdev);
                *ppsdev = NULL;
            }

            if (vt)
                free(vt);

            goto err;
        }

        memset(*ppsdev, 0, sizeof(struct subsys_dev) + node_name_len);
        memset(vt, 0, sizeof(*vt));
        // vfs_wdg_t's port should be remained during the whole driver life
        vt->port = i;

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, WDG_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, node_name:%s, (*ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = vt;

        ret = aos_dev_reg(*ppsdev, &wdg_device_fops, &wdg_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for wdg%d failed, ret:%d\r\n", i, ret);

            free(vt);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_info("wdg vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ppsdev = g_wdg_device_array;
    for (j = 0; j < i; j++) {
        // shall uninstall wdg devices who are already registered
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for wdg%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("wdg vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_wdg_drv_init)

#endif
