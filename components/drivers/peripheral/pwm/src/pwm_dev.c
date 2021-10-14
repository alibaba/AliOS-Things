/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/pwm.h>
#include <vfsdev/pwm_dev.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_PWM_NUM
#define PLATFORM_PWM_NUM CONFIG_PWM_NUM
#else
#define PLATFORM_PWM_NUM 4
#endif

#if (PLATFORM_PWM_NUM > 0)

// PWM device node will be named with "/dev/pwm<x>", where <x> is pwm port id
#define PWM_DEV_NAME_FORMAT "pwm%d"

typedef struct vfs_pwm {
    bool on;
    bool start;
    pwm_dev_t dev;
} vfs_pwm_t;

int pwm_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    pwm_config_t *config = NULL;
    vfs_pwm_t *vp = (vfs_pwm_t *)f->node->i_arg;
    pwm_dev_t *pwm_dev = &vp->dev;
    ddkc_info("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
        case IOC_PWM_ON:
            ddkc_info("IOC_PWM_ON, arg:%ld\r\n", arg);
            if (!vp->on) {
                ddkc_info("before hal_pwm_init\r\n");
                ret = hal_pwm_init(pwm_dev);
            }
            vp->on = ret ? false : true;
            if (vp->on && !vp->start) {
                ret = hal_pwm_start(pwm_dev);
                vp->start = ret ? false : true;
            } else
                ddkc_warn("pwm%d is already ON\r\n", pwm_dev->port);

            if (!vp->start) {
                ddkc_err("pwm%d on failed, ret:%d\r\n", pwm_dev->port, ret);
            }
            break;
        case IOC_PWM_OFF:
            if (vp->start)
                hal_pwm_stop(pwm_dev);
            else
                ddkc_warn("pwm%d is off now, ignore OFF request\r\n", pwm_dev->port);
            // anyway, set on flag to false
            vp->start = false;
            ddkc_info("IOC_PWM_OFF, arg:%ld\r\n", arg);
            break;
        case IOC_PWM_FREQ:
            ddkc_info("IOC_PWM_FREQ, freq:%ld\r\n", arg);
            config = &pwm_dev->config;
            config->freq = (unsigned int)arg;

            if (vp->start) {
                ret = hal_pwm_para_chg(pwm_dev, *config);
                if (ret)
                    ddkc_err("change freq to %d failed, ret:%d\r\n", config->freq, ret);
            }
            break;
        case IOC_PWM_DUTY_CYCLE:
            ddkc_info("IOC_PWM_DUTY_CYCLE, duty_cycle:%ld\r\n", arg);
            config = &pwm_dev->config;
            aos_ipc_copy(&config->duty_cycle, (void *)arg, sizeof(float));

            if (vp->start) {
                ret = hal_pwm_para_chg(pwm_dev, *config);
                if (ret)
                    ddkc_err("change duty cycle to %f failed, ret:%d\r\n", config->duty_cycle, ret);
            }
            break;
        default:
            ddkc_err("invalid cmd:%d\r\n", cmd);
            break;
    }

    return ret;
}

int pwm_device_open (inode_t *node, file_t *f) {
    vfs_pwm_t *vp = (vfs_pwm_t *)f->node->i_arg;
    pwm_dev_t *pwm_dev = &vp->dev;

    ddkc_info("%s opened, port:%d\r\n", node->i_name, pwm_dev->port);

    return 0;
}

int pwm_device_close (file_t *f) {
    int ret = 0;
    vfs_pwm_t *vp = (vfs_pwm_t *)f->node->i_arg;
    pwm_dev_t *pwm_dev = &vp->dev;

    ddkc_info("%s closed, port:%d\r\n", f->node->i_name, pwm_dev->port);

    if (vp->start) {
        ret = hal_pwm_stop(pwm_dev);
        if (ret) {
            ddkc_err("hal_pwm_stop on pwm%d failed, ret=%d\r\n", pwm_dev->port, ret);
        }
        vp->start = false;
    }
    if (vp->on) {
        ret = hal_pwm_finalize(pwm_dev);
        if (ret) {
            ddkc_err("hal_pwm_finalize on pwm%d failed, ret=%d\r\n", pwm_dev->port, ret);
        }
        vp->on = false;
    }

    memset(&pwm_dev->config, 0, sizeof(pwm_dev->config));

    return 0;
}


/************************** device ****************************/

subsys_file_ops_t pwm_device_fops = {
    .open = pwm_device_open,
    .close = pwm_device_close,
    .read = NULL,
    .write = NULL,
    .ioctl = pwm_device_ioctl,
    .poll = NULL,
};

int pwm_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int pwm_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int pwm_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_info("%s\r\n", __func__);
    return 0;
}

struct subsys_drv pwm_device_drv = {
    .drv_name = "pwm",
    .init = pwm_device_init,
    .deinit = pwm_device_deinit,
    .pm = pwm_device_pm,
};

struct subsys_dev *g_pwm_device_array[PLATFORM_PWM_NUM];


int vfs_pwm_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_dbg("pwm vfs driver init starts\r\n");

    node_name_len = strlen(PWM_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    memset(g_pwm_device_array, 0, sizeof(g_pwm_device_array));
    ppsdev = g_pwm_device_array;

    for (i = 0; i < PLATFORM_PWM_NUM; i++) {
        vfs_pwm_t *vp = malloc(sizeof(vfs_pwm_t));

        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);
        if (!(*ppsdev) || !vp) {
            ddkc_err("malloc failed, *ppsdev:%p, vp:%p\r\n", *ppsdev, vp);

            if (*ppsdev) {
                free(*ppsdev);
                *ppsdev = NULL;
            }

            if (vp)
                free(vp);

            goto err;
        }

        memset(*ppsdev, 0, sizeof(struct subsys_dev) + node_name_len);
        memset(vp, 0, sizeof(*vp));
        // vfs_pwm_t's port should be remained during the whole driver life
        vp->dev.port = i;

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, PWM_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, (char *)((*ppsdev) + 1):%p, node_name:%s, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (char *)((*ppsdev) + 1), sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = vp;

        ret = aos_dev_reg(*ppsdev, &pwm_device_fops, &pwm_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for pwm%d failed, ret:%d\r\n", i, ret);

            free(vp);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_dbg("pwm vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ddkc_info("handle pwm%d reg fail\r\n", j);
    // shall uninstall pwm devices who are already registered

    ppsdev = g_pwm_device_array;
    for (j = 0; j < i; j++) {
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for pwm%d\r\n", i);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("pwm vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_pwm_drv_init)

__weak int32_t hal_pwm_init(pwm_dev_t *tim) {
    ddkc_info("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_pwm_start(pwm_dev_t *tim) {
    ddkc_info("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_pwm_stop(pwm_dev_t *tim) {
    ddkc_info("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_pwm_para_chg(pwm_dev_t *tim, pwm_config_t para) {
    ddkc_info("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_pwm_finalize(pwm_dev_t *tim) {
    ddkc_info("__weak %s\r\n", __func__);
    return 0;
}

#endif
