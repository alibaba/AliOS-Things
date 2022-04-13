/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/wdg.h>
#include <hal2vfs/io_wdg.h>
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
    int ref_cnt;
    bool started;
    wdg_dev_t dev;
} vfs_wdg_t;

int wdg_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = -1;
    wdg_config_t *config = NULL;
    vfs_wdg_t *vt = (vfs_wdg_t *)f->node->i_arg;
    wdg_dev_t *wdg_dev = &vt->dev;
    ddkc_dbg("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
        case IOC_WDG_RELOAD:
            ddkc_dbg("IOC_WDG_RELOAD\r\n");
            hal_wdg_reload(wdg_dev);
            ret = 0;
            break;

        default:
            ddkc_err("invalid cmd:%d\r\n", cmd);
            break;
    }

    return ret;
}

int wdg_device_open (inode_t *node, file_t *f) {
    int ret = 0;
    vfs_wdg_t *vt = (vfs_wdg_t *)f->node->i_arg;
    wdg_dev_t *wdg_dev = NULL;

    if (!vt) {
        ddkc_err("%s open failed, invalid i_arg:%p\r\n", node->i_name, vt);
        return -EINVAL;
    }

    wdg_dev = &vt->dev;

    // TODO: need to add lock for multiple wdg user scenarios
    if (!vt->started) {
        ret = hal_wdg_init(wdg_dev);
        if (ret) {
            ddkc_err("%s open failed, hal_wdg_init ret:%d\r\n", node->i_name, ret);
            return ret;
        }
    }
    vt->started = true;
    vt->ref_cnt++;

    ddkc_dbg("%s opened, port:%d, ref_cnt:%d\r\n", node->i_name, wdg_dev->port, vt->ref_cnt);

    return 0;
}

int wdg_device_close (file_t *f) {
    int ret = 0;
    vfs_wdg_t *vt = (vfs_wdg_t *)f->node->i_arg;
    wdg_dev_t *wdg_dev = &vt->dev;

    ddkc_dbg("%s closed, port:%d\r\n", f->node->i_name, wdg_dev->port);
    if (!vt->started) {
        ddkc_err("%s is not started yet\r\n", f->node->i_name);
        return -EIO;
    }
    vt->ref_cnt--;
    if (!vt->ref_cnt) {
        ddkc_err("ignore wdg finialize, ref_cnt:%d\r\n", vt->started, vt->ref_cnt);
        return 0;
    }

    ret = hal_wdg_finalize(wdg_dev);
    if (ret) {
        ddkc_err("hal_wdg_finalize on wdg%d failed, ret=%d\r\n", wdg_dev->port, ret);
    }
    vt->started = false;

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
        vt->dev.port = i;
        vt->started = false;
        vt->ref_cnt = 0;

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

__weak int32_t hal_wdg_init(wdg_dev_t *tim) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_wdg_start(wdg_dev_t *tim) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak void hal_wdg_stop(wdg_dev_t *tim) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return;
}

__weak int32_t hal_wdg_para_chg(wdg_dev_t *tim, wdg_config_t para) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_wdg_finalize(wdg_dev_t *tim) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}
#endif
