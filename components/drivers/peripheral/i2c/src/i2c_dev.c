/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/i2c.h>
#include <vfsdev/i2c_dev.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_I2C_NUM
#define PLATFORM_I2C_NUM CONFIG_I2C_NUM
#else
#define PLATFORM_I2C_NUM 4
#endif

#if (PLATFORM_I2C_NUM > 0)
// I2C device node will be named with "/dev/i2c<x>", where <x> is i2c port id
#define I2C_DEV_NAME_FORMAT "i2c%d"

typedef struct vfs_i2c {
    bool started;
    i2c_dev_t dev;
} vfs_i2c_t;

/**
 * allocate memory for copy i2c_config_t
 * @param dst pointer to destinatin struct
 * @param src pointer to source struct
 * @return 0 for success; negative for failure
 */
static int _copy_i2c_control_arg (io_i2c_control_u *dst, io_i2c_control_u *src) {
    if (!dst || !src) {
        return -EINVAL;
    }
    aos_ipc_copy(dst, src, sizeof(io_i2c_control_u));

    return 0;
}

/**
 * allocate memory for copy io_i2c_data_t, data field in io_i2c_data_t is also included
 * @param dst pointer to destinatin struct
 * @param src pointer to source struct
 * @return 0 for success; negative for failure
 */
static int _alloc_i2c_data_arg (io_i2c_data_t *dst, io_i2c_data_t *src) {
    if (!dst || !src) {
        return -EINVAL;
    }

    aos_ipc_copy(dst, src, sizeof(io_i2c_data_t));
    if (!dst->data || !dst->length) {
        return -EINVAL;
    }

    dst->data = malloc(dst->length);
    if (!dst->data)
        return -ENOMEM;

    return 0;
}

/**
 * copy io_i2c_data_t's data field
 * @param dst pointer to destinatin struct
 * @param src pointer to source struct
 * @return 0 for success; negative for failure
 */
static int _copy_i2c_data_field (io_i2c_data_t *dst, io_i2c_data_t *src) {
    io_i2c_data_t tmp;
    unsigned char *data = NULL;

    if (!dst || !src) {
        return -EINVAL;
    }
    tmp.data = NULL;

    aos_ipc_copy(&tmp, dst, sizeof(io_i2c_data_t));

    data = tmp.data;
    if (!data || !tmp.length || !src->data || !src->length || (tmp.length != src->length)) {
        ddkc_err("invalid arg, dst[data:%p, length:%d], src[data:%p, length:%d]",
                 data, tmp.length, src->data, src->length);
        return -EINVAL;
    }

    aos_ipc_copy(data, src->data, src->length);

    return 0;
}

/**
 * copy io_i2c_data_t's content, data included
 * @param dst pointer to destinatin struct
 * @param src pointer to source struct
 * @return 0 for success; negative for failure
 */
static int _copy_i2c_data_arg (io_i2c_data_t *dst, io_i2c_data_t *src) {
    unsigned char *data = NULL;

    if (!dst || !src) {
        return -1;
    }

    aos_ipc_copy(dst, src, sizeof(io_i2c_data_t));

    data = dst->data;
    if (!data || !dst->length)
        return 0;

    dst->data = malloc(dst->length);
    if (!dst->data)
        return -ENOMEM;

    aos_ipc_copy(dst->data, data, dst->length);

    return 0;
}

static int _free_i2c_data_arg (io_i2c_data_t *dst) {
    if (!dst || !dst->data)
        return -EINVAL;

    free(dst->data);

    return 0;
}

int i2c_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    io_i2c_data_t *d = NULL;
    io_i2c_control_u *c = NULL;
    i2c_config_t *config = NULL;
    vfs_i2c_t *vd = (vfs_i2c_t *)f->node->i_arg;
    i2c_dev_t *i2c_dev = &vd->dev;

    // arg is channel info
    ddkc_dbg("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
        case IOC_I2C_SET_FREQ:
            ddkc_dbg("IOC_I2C_SET_FREQ\r\n");
            c = (io_i2c_control_u *)arg;
            config = &i2c_dev->config;
            config->freq = c->freq;

            if (config->mode) {
                if (vd->started) {
                    hal_i2c_finalize(i2c_dev);
                    vd->started = false;
                }
                // IOC_I2C_SET_CONFIG is already set
                ret = hal_i2c_init(i2c_dev);
                if (ret) {
                    ddkc_err("hal_i2c_init failed, addr:%x, mode:%d, width:%d, freq:%d ret:%d\r\n",
                             config->dev_addr, config->mode, config->address_width, config->freq,
                             ret);
                } else
                    vd->started = true;
            }
            break;

        case IOC_I2C_SET_CONFIG:
            ddkc_dbg("IOC_I2C_SET_CONFIG\r\n");

            c = (io_i2c_control_u *)arg;
            config = &i2c_dev->config;

            config->mode = c->c.role ? I2C_MODE_MASTER : I2C_MODE_SLAVE;
            config->address_width = c->c.addr_width ? I2C_HAL_ADDRESS_WIDTH_10BIT : I2C_HAL_ADDRESS_WIDTH_7BIT;
            config->dev_addr = c->c.addr;

            if (config->freq) {
                if (vd->started) {
                    hal_i2c_finalize(i2c_dev);
                    vd->started = false;
                }

                // IOC_I2C_SET_FREQ is already set
                ret = hal_i2c_init(i2c_dev);
                if (ret) {
                    ddkc_err("hal_i2c_init failed, addr:%x, mode:%d, width:%d, freq:%d ret:%d\r\n",
                             config->dev_addr, config->mode, config->address_width, config->freq,
                             ret);
                } else
                    vd->started = true;
            }
            break;

        case IOC_I2C_MASTER_RX:
            ddkc_dbg("IOC_I2C_MASTER_RX\r\n");
            if (!vd->started) {
                ddkc_err("i2c%d is not started yet\r\n", vd->dev.port);
                return -EIO;
            }

            d = (io_i2c_data_t *)arg;
            ret = hal_i2c_master_recv(i2c_dev, d->addr, d->data, d->length, d->timeout);
            if (ret) {
                ddkc_err("IOC_I2C_MASTER_RX failed, ret:%d\r\n", ret);
            }
            break;
        case IOC_I2C_MASTER_TX:
            ddkc_dbg("IOC_I2C_MASTER_TX\r\n");
            if (!vd->started) {
                ddkc_err("i2c%d is not started yet\r\n", vd->dev.port);
                return -EIO;
            }

            d = (io_i2c_data_t *)arg;
            ddkc_dbg("d->addr:%x, d->data:%p, d->length:%d, d->timeout:%d\r\n", d->addr, d->data, d->length, d->timeout);
            ret = hal_i2c_master_send(i2c_dev, d->addr, d->data, d->length, d->timeout);
            if (ret) {
                ddkc_err("IOC_I2C_MASTER_TX failed, ret:%d\r\n", ret);
            }
            break;
        case IOC_I2C_SLAVE_RX:
            ddkc_dbg("IOC_I2C_SLAVE_RX\r\n");
            if (!vd->started) {
                ddkc_err("i2c%d is not started yet\r\n", vd->dev.port);
                return -EIO;
            }

            d = (io_i2c_data_t *)arg;
            ret = hal_i2c_slave_recv(i2c_dev, d->data, d->length, d->timeout);
            if (ret) {
                ddkc_err("IOC_I2C_MASTER_RX failed, ret:%d\r\n", ret);
            }
            break;
        case IOC_I2C_SLAVE_TX:
            ddkc_dbg("IOC_I2C_SLAVE_TX\r\n");
            if (!vd->started) {
                ddkc_err("i2c%d is not started yet\r\n", vd->dev.port);
                return -EIO;
            }

            d = (io_i2c_data_t *)arg;
            ret = hal_i2c_slave_send(i2c_dev, d->data, d->length, d->timeout);
            if (ret) {
                ddkc_err("IOC_I2C_SLAVE_TX failed, ret:%d\r\n", ret);
            }
            break;
        case IOC_I2C_MEM_RX:
            ddkc_dbg("IOC_I2C_MEM_RX\r\n");
            if (!vd->started) {
                ddkc_err("i2c%d is not started yet\r\n", vd->dev.port);
                return -EIO;
            }
            d = (io_i2c_data_t *)arg;
            ddkc_dbg("d->addr:%x, d->maddr:%p, d->mlength:%d, d->data:%p, d->length:%d, d->timeout:%d\r\n",
                      d->addr, d->maddr, d->mlength, d->data, d->length, d->timeout);
            ret = hal_i2c_mem_read(i2c_dev, d->addr, d->maddr, d->mlength, d->data, d->length, d->timeout);
            if (ret) {
                ddkc_err("IOC_I2C_MEM_RX failed, ret:%d\r\n", ret);
            }
            break;
        case IOC_I2C_MEM_TX:
            ddkc_dbg("IOC_I2C_MEM_TX\r\n");
            if (!vd->started) {
                ddkc_err("i2c%d is not started yet\r\n", vd->dev.port);
                return -EIO;
            }

            d = (io_i2c_data_t *)arg;
            ddkc_dbg("d->addr:%x, d->maddr:%p, d->mlength:%d, d->data:%p, d->length:%d, d->timeout:%d\r\n",
                      d->addr, d->maddr, d->mlength, d->data, d->length, d->timeout);
            ret = hal_i2c_mem_write(i2c_dev, d->addr, d->maddr, d->mlength, d->data, d->length, d->timeout);
            if (ret) {
                ddkc_err("IOC_I2C_MEM_TX failed, ret:%d\r\n", ret);
            }
            break;
        default:
            ddkc_err("invalid cmd:%d\r\n", cmd);
            break;
    }

    return ret;
}

int i2c_device_open (inode_t *node, file_t *f) {
    vfs_i2c_t *vd = (vfs_i2c_t *)node->i_arg;
    i2c_dev_t *i2c_dev = &vd->dev;

    if (vd->started) {
        ddkc_err("i2c%d is already started\r\n", i2c_dev->port);
        return -EIO;
    }
    memset(&i2c_dev->config, 0, sizeof(i2c_dev->config));
    ddkc_dbg("device:%s open success\r\n", node->i_name);

    return 0;
}

int i2c_device_close (file_t *f) {
    vfs_i2c_t *vd = (vfs_i2c_t *)f->node->i_arg;
    i2c_dev_t *i2c_dev = &vd->dev;

    if (vd->started) {
        int ret = -1;
        ret = hal_i2c_finalize(i2c_dev);
        if (ret) {
            ddkc_err("dhal_i2c_finalize on %s failed, ret:%d\r\n", f->node->i_name, ret);
        }
        vd->started = false;
    } else {
        ddkc_warn("i2c%d is not started yet\r\n", i2c_dev->port);
    };
    ddkc_dbg("device:%s close success\r\n", f->node->i_name);

    return 0;
}


/************************** device ****************************/


subsys_file_ops_t i2c_device_fops = {
    .open = i2c_device_open,
    .close = i2c_device_close,
    .read = NULL,
    .write = NULL,
    .ioctl = i2c_device_ioctl,
    .poll = NULL,
};

int i2c_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int i2c_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int i2c_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv i2c_device_drv = {
    .drv_name = "i2c",
    .init = i2c_device_init,
    .deinit = i2c_device_deinit,
    .pm = i2c_device_pm,
};

struct subsys_dev *g_i2c_device_array[PLATFORM_I2C_NUM];


int vfs_i2c_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_dbg("i2c vfs driver init starts\r\n");

    node_name_len = strlen(I2C_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    memset(g_i2c_device_array, 0, sizeof(g_i2c_device_array));
    ppsdev = g_i2c_device_array;

    for (i = 0; i < PLATFORM_I2C_NUM; i++) {
        vfs_i2c_t *vd = malloc(sizeof(vfs_i2c_t));

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
        // vfs_i2c_t's port should be remained during the whole driver life
        vd->dev.port = i;

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, I2C_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, node_name:%s, (*ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = vd;

        ret = aos_dev_reg(*ppsdev, &i2c_device_fops, &i2c_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for i2c%d failed, ret:%d\r\n", i, ret);

            free(vd);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_dbg("i2c vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    ppsdev = g_i2c_device_array;
    for (j = 0; j < i; j++) {
        // shall uninstall i2c devices who are already registered

        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for i2c%d\r\n", i);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);

            free(*ppsdev);
            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("i2c vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_i2c_drv_init)

__weak int32_t hal_i2c_init(i2c_dev_t *i2c) {
    ddkc_warn("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                                   uint16_t size, uint32_t timeout) {
    ddkc_warn("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                                   uint16_t size, uint32_t timeout) {
    ddkc_warn("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data,
                                  uint16_t size, uint32_t timeout) {
    ddkc_warn("__weak %s\r\n", __func__);
return 0;
}


__weak int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data,
                                  uint16_t size, uint32_t timeout) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}


__weak int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                                 uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                                 uint32_t timeout) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                                uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                                uint32_t timeout) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

__weak int32_t hal_i2c_finalize(i2c_dev_t *i2c) {
    ddkc_dbg("__weak %s\r\n", __func__);
    return 0;
}

#endif
