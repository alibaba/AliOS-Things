/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/timer.h>
#include <vfsdev/timer_dev.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_TIMER_NUM
#define PLATFORM_TIMER_NUM CONFIG_TIMER_NUM
#else
#define PLATFORM_TIMER_NUM 4
#endif

#if (PLATFORM_TIMER_NUM > 0)

// TIMER device node will be named with "/dev/timer<x>", where <x> is timer port id
#define TIMER_DEV_NAME_FORMAT "timer%d"

typedef struct vfs_timer {
    bool started;
    timer_dev_t dev;
} vfs_timer_t;

int timer_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    timer_alarm_t *alarm;
    timer_config_t *config = NULL;
    vfs_timer_t *vt = (vfs_timer_t *)f->node->i_arg;
    timer_dev_t *timer_dev = &vt->dev;
    ddkc_dbg("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
        case IOC_TIMER_CONTROL:
            ddkc_dbg("IOC_TIMER_CONTROL, arg:%ld\r\n", arg);
            if (arg == IO_TIMER_START) {
                if (!vt->started) {
                    ret = hal_timer_start(timer_dev);
                    vt->started = ret ? false : true;
                } else
                    ddkc_warn("timer%d is already started\r\n", vt->dev.port);
            } else {
                if (vt->started)
                    hal_timer_stop(timer_dev);
                else
                    ddkc_warn("timer%d is not started yet\r\n", vt->dev.port);
                // anyway, set started flag to false
                vt->started = false;
            }
            if (ret) {
                ddkc_err("timer control[%ld] failed, ret:%d\r\n", arg, ret);
            }
            break;

        case IOC_TIMER_IRQP_SET:
            ddkc_dbg("IOC_TIMER_IRQP_SET\r\n");
            alarm = (timer_alarm_t *)(arg);
            if (!alarm || !alarm->cb || !alarm->period) {
                ddkc_err("invalid alarm:%p, handler:%p or period:%ld\r\n",
                         alarm, alarm ? alarm->cb: NULL, alarm ? alarm->period : 0);
                return -EINVAL;
            }

            // TODO: shall we use local variable and store parameters back to timer_dev->config after set operation success?
            config = &timer_dev->config;
            config->cb = alarm->cb;
            config->arg = alarm->arg;
            config->period = alarm->period;
            config->reload_mode = alarm->auto_reload ? TIMER_RELOAD_AUTO : TIMER_RELOAD_MANU;
            if (!vt->started) {
                ret = hal_timer_init(timer_dev);
            } else {
                ret = hal_timer_para_chg(timer_dev, *config);

                /* restart it if enable timer reload function */
                if (alarm->auto_reload) {
                    hal_timer_stop(timer_dev);
                    ret = hal_timer_start(timer_dev);
                    vt->started = ret ? false : true;
                    if (ret) {
                        ddkc_err("restart timer failed， ret:%d\r\n", ret);
                    }
                }
            }

            if (ret) {
                ddkc_err("set timer param failed, ret:%d\r\n", ret);
            }
            break;

        case IOC_TIMER_RELOAD:
            ddkc_dbg("IOC_TIMER_RELOAD\r\n");

            config = &timer_dev->config;
            config->reload_mode = (arg ? TIMER_RELOAD_AUTO : TIMER_RELOAD_MANU);
            ret = hal_timer_para_chg(timer_dev, *config);
            if (ret) {
                ddkc_err("start timer failed, ret:%d\r\n", ret);
            }

            /* restart it if enable timer reload function */
            if (arg && vt->started) {
                hal_timer_stop(timer_dev);
                ret = hal_timer_start(timer_dev);
                vt->started = ret ? false : true;
                if (ret) {
                    ddkc_err("restart timer failed， ret:%d\r\n", ret);
                }
            }
            break;

        case IOC_TIMER_IRQP_GET:
            ddkc_dbg("IOC_TIMER_IRQP_GET\r\n");

            config = &timer_dev->config;

            if (!arg) {
                ddkc_err("invalid arg:0x%lx\r\n", arg);
                return -EINVAL;
            }
            alarm = (timer_alarm_t *)(arg);
            alarm->auto_reload = config->reload_mode == TIMER_RELOAD_AUTO ? true : false;
            alarm->period = config->period;
            alarm->cb = config->cb;
            alarm->arg = config->arg;
            break;
        default:
            ddkc_err("invalid cmd:%d\r\n", cmd);
            break;
    }

    return ret;
}

int timer_device_open (inode_t *node, file_t *f) {
    vfs_timer_t *vt = (vfs_timer_t *)f->node->i_arg;
    timer_dev_t *timer_dev = &vt->dev;

    ddkc_dbg("%s opened, port:%d\r\n", node->i_name, timer_dev->port);

    return 0;
}

int timer_device_close (file_t *f) {
    int ret = 0;
    vfs_timer_t *vt = (vfs_timer_t *)f->node->i_arg;
    timer_dev_t *timer_dev = &vt->dev;

    ddkc_dbg("%s closed, port:%d\r\n", f->node->i_name, timer_dev->port);

    if (vt->started) {
        hal_timer_stop(timer_dev);
        vt->started = false;
    }

    ret = hal_timer_finalize(timer_dev);
    if (ret) {
        ddkc_err("hal_timer_finalize on timer%d failed, ret=%d\r\n", timer_dev->port, ret);
    }

    memset(&timer_dev->config, 0, sizeof(timer_dev->config));

    return 0;
}


/************************** device ****************************/


subsys_file_ops_t timer_device_fops = {
    .open = timer_device_open,
    .close = timer_device_close,
    .read = NULL,
    .write = NULL,
    .ioctl = timer_device_ioctl,
    .poll = NULL,
};

int timer_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int timer_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int timer_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv timer_device_drv = {
    .drv_name = "timer",
    .init = timer_device_init,
    .deinit = timer_device_deinit,
    .pm = timer_device_pm,
};

struct subsys_dev *g_timer_device_array[PLATFORM_TIMER_NUM];


int vfs_timer_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_dbg("timer vfs driver init starts\r\n");

    node_name_len = strlen(TIMER_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    memset(g_timer_device_array, 0, sizeof(g_timer_device_array));
    ppsdev = g_timer_device_array;

    for (i = 0; i < PLATFORM_TIMER_NUM; i++) {
        vfs_timer_t *vt = malloc(sizeof(vfs_timer_t));

        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);
        if (!(*ppsdev) || !vt) {
            ddkc_err("malloc failed, *ppsdev:%p, vt:%p\r\n", *ppsdev, vt);

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
        // vfs_timer_t's port should be remained during the whole driver life
        vt->dev.port = i;

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, TIMER_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, node_name:%s , (*ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = vt;

        ret = aos_dev_reg(*ppsdev, &timer_device_fops, &timer_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for timer%d failed, ret:%d\r\n", i, ret);

            free(vt);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_dbg("timer vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ppsdev = g_timer_device_array;
    for (j = 0; j < i; j++) {
        // shall uninstall timer devices who are already registered
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for timer%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("timer vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_timer_drv_init)

__weak int32_t hal_timer_init(timer_dev_t *tim) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_timer_start(timer_dev_t *tim) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak void hal_timer_stop(timer_dev_t *tim) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return;
}

__weak int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_timer_finalize(timer_dev_t *tim) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}
#endif
