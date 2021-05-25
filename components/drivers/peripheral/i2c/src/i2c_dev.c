/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/driver/i2c.h>
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
    unsigned port;
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
    vfs_i2c_t *vd = (vfs_i2c_t *)f->node->i_arg;
    i2c_dev_handle_t slave = (i2c_dev_handle_t)f->f_arg;

    // VFS make sure ioctl on the same fd is sequenced, so big-lock is not necessary
    ddkc_dbg("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
        case IOC_I2C_SET_FREQ:
            ddkc_dbg("IOC_I2C_SET_FREQ\r\n");
            c = (io_i2c_control_u *)arg;
            ret = aos_i2c_clk_set(slave, c->freq);
            if (ret) {
                ddkc_err("set clock to i2c%d failed, ret:%d\r\n", c->freq, ret);
            }
            break;

        case IOC_I2C_SET_CONFIG:
            ddkc_dbg("IOC_I2C_SET_CONFIG\r\n");

            c = (io_i2c_control_u *)arg;
            if (!c->c.role) {
                ret = -EINVAL;
                ddkc_err("i2c%d only support master mode\r\n", c->freq);
            }

            ret = aos_i2c_slave_addr_set(slave, c->c.addr);
            if (ret) {
                ddkc_err("set slave address:0x%x to i2c%d failed, ret:%d\r\n", c->c.addr, ret);
            }

            ret += aos_i2c_addr_width_set(slave, c->c.addr_width ? I2C_SLAVE_ADDR_WIDTH_10BIT : I2C_SLAVE_ADDR_WIDTH_7BIT);
            if (ret) {
                ddkc_err("set slave address:0x%x to i2c%d failed, ret:%d\r\n", c->c.addr, ret);
            }
            break;

        case IOC_I2C_MASTER_RX:
            ddkc_dbg("IOC_I2C_MASTER_RX\r\n");

            d = (io_i2c_data_t *)arg;

            ret = aos_i2c_slave_addr_set(slave, d->addr);
            if (ret) {
                ddkc_err("set slave address:0x%x to i2c%d failed, ret:%d\r\n", c->c.addr, ret);
                return ret;
            }

            ret = aos_i2c_master_recv(slave, d->data, d->length, d->timeout);
            if (ret != d->length) {
                ddkc_err("i2c%d rx failed, tx_buffer:%p, d->length:%d, d->timeout:%d, ret:%d\n",
                    vd->port, d->data, d->length, d->timeout, ret);
                    ret = -EIO;
            } else {
                ddkc_dbg("i2c%d rx succeed, tx_buffer:%p, d->length:%d, d->timeout:%d\n",
                    vd->port, d->data, d->length, d->timeout);
                    ret = 0;
            }
            break;
        case IOC_I2C_MASTER_TX:
            ddkc_dbg("IOC_I2C_MASTER_TX\r\n");

            d = (io_i2c_data_t *)arg;

            ret = aos_i2c_slave_addr_set(slave, d->addr);
            if (ret) {
                ddkc_err("set slave address:0x%x to i2c%d failed, ret:%d\r\n", c->c.addr, ret);
                return ret;
            }

            ret = aos_i2c_master_send(slave, d->data, d->length, d->timeout);
            if (ret != d->length) {
                ddkc_err("i2c%d tx failed, tx_buffer:%p, d->length:%d, d->timeout:%d, ret:%d\n",
                    vd->port, d->data, d->length, d->timeout, ret);
                    ret = -EIO;
            } else {
                ddkc_dbg("i2c%d tx succeed, tx_buffer:%p, d->length:%d, d->timeout:%d\n",
                    vd->port, d->data, d->length, d->timeout);
                    ret = 0;
            }
            break;
        case IOC_I2C_SLAVE_RX:
            ddkc_dbg("IOC_I2C_SLAVE_RX\r\n");
            ddkc_err("i2c slave operation is not supported\r\n");
            ret = -ENOTSUP;
            break;
        case IOC_I2C_SLAVE_TX:
            ddkc_dbg("IOC_I2C_SLAVE_TX\r\n");
            ddkc_err("i2c slave operation is not supported\r\n");
            ret = -ENOTSUP;
            break;
        case IOC_I2C_MEM_RX:
            ddkc_dbg("IOC_I2C_MEM_RX\r\n");
            d = (io_i2c_data_t *)arg;

            ret = aos_i2c_slave_addr_set(slave, d->addr);
            if (ret) {
                ddkc_err("set slave address:0x%x to i2c%d failed, ret:%d\r\n", c->c.addr, ret);
                return ret;
            }

            ret = aos_i2c_mem_read(slave, d->maddr, d->mlength, d->data, d->length, d->timeout);
            if (ret != d->length) {
                ddkc_err("i2c%d memory rx failed, mem_addr:0x%x, addr_len:%d, tx_buffer:%p, d->length:%d, d->timeout:%d, ret:%d\n",
                    vd->port, d->maddr, d->mlength, d->data, d->length, d->timeout, ret);
                    ret = -EIO;
            } else {
                ddkc_dbg("i2c%d memory rx succeed, mem_addr:0x%x, addr_len:%d, tx_buffer:%p, d->length:%d, d->timeout:%d\n",
                    vd->port, d->maddr, d->mlength, d->data, d->length, d->timeout);
                    ret = 0;
            }
            break;
        case IOC_I2C_MEM_TX:
            ddkc_dbg("IOC_I2C_MEM_TX\r\n");
            d = (io_i2c_data_t *)arg;

            ret = aos_i2c_slave_addr_set(slave, d->addr);
            if (ret) {
                ddkc_err("set slave address:0x%x to i2c%d failed, ret:%d\r\n", c->c.addr, ret);
                return ret;
            }

            ret = aos_i2c_mem_write(slave, d->maddr, d->mlength, d->data, d->length, d->timeout);
            if (ret != d->length) {
                ddkc_err("i2c%d memory tx failed, mem_addr:0x%x, addr_len:%d, tx_buffer:%p, d->length:%d, d->timeout:%d, ret:%d\n",
                    vd->port, d->maddr, d->mlength, d->data, d->length, d->timeout, ret);
                    ret = -EIO;
            } else {
                ddkc_dbg("i2c%d memory tx succeed, mem_addr:0x%x, addr_len:%d, tx_buffer:%p, d->length:%d, d->timeout:%d\n",
                    vd->port, d->maddr, d->mlength, d->data, d->length, d->timeout);
                    ret = 0;
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
    i2c_dev_handle_t slave = NULL;
    i2c_slave_config_t config;

    config.addr = 0x0; // set to 0x0 by default
    config.addr_width = I2C_SLAVE_ADDR_WIDTH_7BIT; // set to 7-bit address mode by default
    config.clk = I2C_BUS_CLK_100K; // set to 100k by default

    slave = aos_i2c_open(vd->port, &config);
    if (!slave) {
        printf("i2c%d open failed\r\n", vd->port);
        return -1;
    }

    f->f_arg = slave;

    ddkc_dbg("device:%s open success\r\n", node->i_name);

    return 0;
}

int i2c_device_close (file_t *f) {
    int ret = -1;
    vfs_i2c_t *vd = (vfs_i2c_t *)f->node->i_arg;
    i2c_dev_handle_t slave = (i2c_dev_handle_t)f->f_arg;

    if (slave) {
        ret = aos_i2c_close(slave);
        if (ret) {
            printf("i2c%d close failed, ret:%d\r\n", vd->port, ret);
        }
        f->f_arg = NULL;
    } else {
        ddkc_warn("invalid f_arg:%x\r\n", slave);
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
        vd->port = i;

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


#endif
