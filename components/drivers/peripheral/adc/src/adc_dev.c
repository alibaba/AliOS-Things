/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/adc.h>
#include <vfsdev/adc_dev.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_ADC_NUM
#define PLATFORM_ADC_NUM CONFIG_ADC_NUM
#else
#define PLATFORM_ADC_NUM  4
#endif

#if (PLATFORM_ADC_NUM > 0)

// ADC device node will be named with "/dev/adc<x>", where <x> is adc port id
#define ADC_DEV_NAME_FORMAT "adc%d"

typedef struct vfs_adc {
    int ref_cnt;
    adc_dev_t dev;
} vfs_adc_t;

int adc_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    io_adc_arg_t adc_arg;
    io_adc_arg_t *p_adc_arg = NULL;
    char *fname = f->node->i_name;
    vfs_adc_t *vd = (vfs_adc_t *)f->node->i_arg;
    adc_dev_t *adc_dev = &vd->dev;

    // arg is channel info
    ddkc_dbg("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
        case IOC_ADC_GET_VALUE:
            p_adc_arg = (io_adc_arg_t *)arg;
            BUG_ON_MSG(!p_adc_arg, "IOC_ADC_GET_VALUE - arg should not be NULL\r\n");
            /* copy timeout config from user */
            aos_ipc_copy(&adc_arg, p_adc_arg, sizeof(io_adc_arg_t));

            ret = hal_adc_value_get(adc_dev, &adc_arg.value, adc_arg.timeout);

            /* copy value back to user */
            aos_ipc_copy(p_adc_arg, &adc_arg, sizeof(io_adc_arg_t));
            ddkc_info("IOC_ADC_GET_VALUE, adc_arg.value:%d, ret:%d\r\n", adc_arg.value, ret);

            break;

        case IOC_ADC_START:
            ddkc_dbg("IOC_ADC_START, arg:%ld\r\n", arg);
            adc_dev->config.sampling_cycle = arg;
            ret = vd->ref_cnt ? 0 : hal_adc_init(adc_dev);
            if (ret) {
                ddkc_err("%s starts failed, port:%d, ref_cnt:%d\r\n", fname, adc_dev->port, vd->ref_cnt);
            } else {
                vd->ref_cnt++;
                ddkc_dbg("%s starts success, port:%d, ref_cnt:%d\r\n", fname, adc_dev->port, vd->ref_cnt);
            }
            break;

        case IOC_ADC_STOP:
            vd->ref_cnt--;
            ret = vd->ref_cnt ? 0 : hal_adc_finalize(adc_dev);

            if (ret) {
                ddkc_err("%s stop failed, port:%d\r\n", fname, adc_dev->port);
            } else {
                ddkc_dbg("%s stop success, port:%d\r\n", fname, adc_dev->port);
            }

            break;

        default:
            ddkc_err("invalid cmd:%d\r\n", cmd);
            break;
    }

    return ret;
}

int adc_device_open (inode_t *node, file_t *f) {

    ddkc_info("device:%s open succeed\r\n", node->i_name);

    return 0;
}

int adc_device_close (file_t *f) {

    ddkc_info("device:%s close succeed\r\n", f->node->i_name);

    return 0;
}

/************************** device ****************************/


subsys_file_ops_t adc_device_fops = {
    .open = adc_device_open,
    .close = adc_device_close,
    .read = NULL,
    .write = NULL,
    .ioctl = adc_device_ioctl,
    .poll = NULL,
};

int adc_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int adc_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int adc_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv adc_device_drv = {
    .drv_name = "adc",
    .init = adc_device_init,
    .deinit = adc_device_deinit,
    .pm = adc_device_pm,
};

struct subsys_dev *g_adc_device_array[PLATFORM_ADC_NUM];


int vfs_adc_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_dbg("adc vfs driver init starts\r\n");

    node_name_len = strlen(ADC_DEV_NAME_FORMAT) + 1;

    memset(g_adc_device_array, 0, sizeof(g_adc_device_array));
    ppsdev = g_adc_device_array;

    for (i = 0; i < PLATFORM_ADC_NUM; i++) {
        vfs_adc_t *vd = malloc(sizeof(vfs_adc_t));

        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);
        if (!(*ppsdev) || !vd) {
            ddkc_err("malloc failed, *ppsdev:%p, vd:%p\r\n", *ppsdev, vd);

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
        // vfs_adc_t's port should be remained during the whole driver life
        vd->dev.port = i;

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, ADC_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, node_name:%s, (*ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = vd;

        ret = aos_dev_reg(*ppsdev, &adc_device_fops, &adc_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for adc%d failed, ret:%d\r\n", i, ret);

            free(vd);
            free(*ppsdev);
            *ppsdev = NULL;
            goto err;
        }

        ppsdev++;
    }

    ddkc_dbg("adc vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ppsdev = g_adc_device_array;
    for (j = 0; j < i; j++) {
        // shall uninstall adc devices who are already registered
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for adc%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("adc vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_adc_drv_init)

#endif
