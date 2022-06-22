/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/spi.h>
#include <vfsdev/spi_dev.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_SPI_NUM
#define PLATFORM_SPI_NUM CONFIG_SPI_NUM
#else
#define PLATFORM_SPI_NUM 4
#endif

#if (PLATFORM_SPI_NUM > 0)

// SPI device node will be named with "/dev/spi<x>", where <x> is spi port id
#define SPI_DEV_NAME_FORMAT "spi%d"
#define DEFAULT_SPI_FREQ 1000000
/* used to notify user on poll events */
typedef struct spi_poll_notify {
    bool flag;
    struct mutex mutex;
    poll_notify_t notify;
    struct pollfd *fd;
    void *arg;
} spi_poll_notify_t;

static spi_poll_notify_t g_spi_notify[PLATFORM_SPI_NUM];

__weak int hal_spi_recv_cb_reg(spi_dev_t *spi, spi_rx_cb cb) {
    ddkc_dbg("%s, spi:%p, cb:%p\r\n", __func__, spi, cb);
    return 0;
}

ssize_t spi_device_read (file_t *f, void *buffer, size_t size) {

    spi_dev_t *spi = (spi_dev_t *)f->node->i_arg;
    spi_poll_notify_t *spi_poll = NULL;

    if (!spi || !buffer || !size) {
        ddkc_dbg("invalid spi:%p, buffer:%p or size:%d\r\n", spi, buffer, size);
        return -EINVAL;
    }
    spi_poll = &g_spi_notify[spi->port];
    if (spi_poll->flag) {
        mutex_lock(&spi_poll->mutex);
        spi_poll->flag = false;
        mutex_unlock(&spi_poll->mutex);
    }
    return hal_spi_recv(spi, buffer, size, 0);
}

ssize_t spi_device_write (file_t *f, const void *buffer, size_t size) {
    spi_dev_t *spi = (spi_dev_t *)f->node->i_arg;

    if (!spi || !buffer || !size) {
        ddkc_err("invalid spi:%p, buffer:%p or size:%d\r\n", spi, buffer, size);
        return -EINVAL;
    }
    return hal_spi_send(spi, buffer, size, 1000);
}

static spi_role_e _arg_to_role (unsigned long arg) {
    spi_role_e role = 0;
    int control = arg & SPI_SLAVE;

    switch (control) {
        case SPI_SLAVE:
            role = SPI_ROLE_SLAVE;
            break;
        case SPI_MASTER:
            role = SPI_ROLE_MASTER;
            break;

        default:
            role = SPI_ROLE_MASTER;
            break;
    }

    return role;
}

static spi_firstbit_e _arg_to_firstbit (unsigned long arg) {
    spi_firstbit_e  firstbit = 0;
    int control = arg & SPI_LSB;

    switch (control) {
        case SPI_LSB:
            firstbit = SPI_FIRSTBIT_LSB;
            break;
        case SPI_MSB:
            firstbit = SPI_FIRSTBIT_MSB;
            break;

        default:
            firstbit = SPI_FIRSTBIT_MSB;
            break;
    }

    return firstbit;
}

static spi_work_mode_e _arg_to_work_mode (unsigned long arg) {
    spi_work_mode_e mode = 0;
    int control = arg & (SPI_CPOL|SPI_CPHA);

    switch (control) {
        case SPI_MODE_0:
            mode = SPI_WORK_MODE_0;
            break;
        case SPI_MODE_1:
            mode = SPI_WORK_MODE_1;
            break;
        case SPI_MODE_2:
            mode = SPI_WORK_MODE_2;
            break;
        case SPI_MODE_3:
            mode = SPI_WORK_MODE_3;
            break;
    }

    return mode;
}

static spi_transfer_mode_e _arg_to_tranmode (unsigned long arg) {
    spi_transfer_mode_e mode = 0;
    int control = arg & SPI_TRANSFER_MODE_MASK;

    switch (control) {
        case SPI_TRANSFER_DMA_MODE:
            mode = (spi_transfer_mode_e)SPI_TRANSFER_DMA;
            break;
        case SPI_TRANSFER_NORMAL_MODE:
            mode = (spi_transfer_mode_e)SPI_TRANSFER_NORMAL;
            break;

        default:
            mode = (spi_transfer_mode_e)SPI_TRANSFER_NORMAL;
            break;
    }

    return mode;
}

static spi_data_size_e _arg_to_datasize (unsigned long arg) {
    spi_data_size_e datasize = 0;
    int control = arg & SPI_DATA_SIZE_MASK;

    switch (control) {
        case SPI_DATA_4BIT:
            datasize = SPI_DATA_SIZE_4BIT;
            break;
        case SPI_DATA_5BIT:
            datasize = SPI_DATA_SIZE_5BIT;
            break;
        case SPI_DATA_6BIT:
            datasize = SPI_DATA_SIZE_6BIT;
            break;
        case SPI_DATA_7BIT:
            datasize = SPI_DATA_SIZE_7BIT;
            break;
        case SPI_DATA_8BIT:
            datasize = SPI_DATA_SIZE_8BIT;
            break;
        case SPI_DATA_9BIT:
            datasize = SPI_DATA_SIZE_9BIT;
            break;
        case SPI_DATA_10BIT:
            datasize = SPI_DATA_SIZE_10BIT;
            break;
        case SPI_DATA_11BIT:
            datasize = SPI_DATA_SIZE_11BIT;
            break;
        case SPI_DATA_12BIT:
            datasize = SPI_DATA_SIZE_12BIT;
            break;
        case SPI_DATA_13BIT:
            datasize = SPI_DATA_SIZE_13BIT;
            break;
        case SPI_DATA_14BIT:
            datasize = SPI_DATA_SIZE_14BIT;
            break;
        case SPI_DATA_15BIT:
            datasize = SPI_DATA_SIZE_15BIT;
            break;
        case SPI_DATA_16BIT:
            datasize = SPI_DATA_SIZE_16BIT;
            break;

        default:
            datasize = SPI_DATA_SIZE_8BIT;
            break;
    }

    return datasize;
}

static spi_cs_e _arg_to_cs (int arg) {
    spi_cs_e cs = 0;
    int control = arg & SPI_NO_CS;

    switch (control) {
        case SPI_NO_CS:
            cs = SPI_CS_DIS;
            break;

        default:
            cs = SPI_CS_EN;
            break;
    }

    return cs;
}

int spi_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = -1;
    spi_config_t *config = NULL;
    spi_dev_t *spi = (spi_dev_t *)f->node->i_arg;
	ioc_spi_transfer_t *tranptr;

    if (!spi) {
        ddkc_dbg("spi is NULL,invalid and ignore\r\n");
        return -EINVAL;
    }

    config = &spi->config;

    ddkc_dbg("i_name:%s, spi:%p, cmd:%d, arg:0x%lx\r\n", f->node->i_name, spi, cmd, arg);

    switch (cmd) {
        case IOC_SPI_SET_CFLAG:
            ret = hal_spi_finalize(spi);
            if (ret) {
                ddkc_warn("hal_spi_finalize failed, ret:%d\r\n", ret);
                break;
            }

            config->role = _arg_to_role(arg);
            config->firstbit = _arg_to_firstbit(arg);
            config->mode = _arg_to_work_mode(arg);
            config->t_mode = _arg_to_tranmode(arg);
            config->data_size = _arg_to_datasize(arg);
            config->cs = _arg_to_cs(arg);
            ddkc_info("role:%d, firstbit:%d, mode:%d, t_mode:%d, data_size:%d, cs:%d\r\n",
                      config->role, config->firstbit, config->mode, config->t_mode, config->data_size, config->cs);

            ret = hal_spi_init(spi);
            if (ret) {
                ddkc_warn("hal_spi_init failed, ret:%d\r\n", ret);
            }
            break;
        case IOC_SPI_SET_FREQ:
            ret = hal_spi_finalize(spi);
            if (ret) {
                ddkc_warn("hal_spi_finalize failed, ret:%d\r\n", ret);
                break;
            }
            config->freq = arg;
            ret = hal_spi_init(spi);
            if (ret) {
                ddkc_warn("hal_spi_init failed, ret:%d\r\n", ret);
            }

            break;
        case IOC_SPI_SEND_RECV:
            tranptr = (ioc_spi_transfer_t *)arg;
            if(NULL == tranptr){
                ddkc_warn("tranptr is null, ret:%d\r\n", ret);
                break;
            }
            ret = hal_spi_send_recv(spi,tranptr->tx_buf,tranptr->rx_buf,tranptr->rx_size,0);
            if (ret) {
                ddkc_warn("hal_spi_send_recv failed, ret:%d\r\n", ret);
            }

            break;
        case IOC_SPI_SEND_AND_RECV:
            tranptr = (ioc_spi_transfer_t *)arg;
            if(NULL == tranptr){
                ddkc_warn("tranptr is null, ret:%d\r\n", ret);
                break;
            }
            ret = hal_spi_send_and_recv(spi,tranptr->tx_buf,tranptr->tx_size,tranptr->rx_buf,tranptr->rx_size,0);
            if (ret) {
                ddkc_warn("hal_spi_send_and_recv failed, ret:%d\r\n", ret);
            }

            break;
        case IOC_SPI_SEND_AND_SEND:
            tranptr = (ioc_spi_transfer_t *)arg;
            if(NULL == tranptr){
                ddkc_warn("tranptr is null, ret:%d\r\n", ret);
                break;
            }
            ret = hal_spi_send_and_send(spi,tranptr->tx_buf,tranptr->tx_size,tranptr->rx_buf,tranptr->rx_size,0);
            if (ret) {
                ddkc_warn("hal_spi_send_and_send failed, ret:%d\r\n", ret);
            }
            break;

        case IOC_SPI_SET_SERIAL_LEN:
            ret = hal_spi_finalize(spi);
            if (ret) {
                ddkc_warn("hal_spi_finalize failed, ret:%d\r\n", ret);
                break;
            }
            config->serial_len = arg;
            ret = hal_spi_init(spi);
            if (ret) {
                ddkc_warn("hal_spi_init failed, ret:%d\r\n", ret);
            }

            break;

        default:
            break;
    }

    return ret;
}

int spi_device_open (inode_t *node, file_t *f) {
    int ret = -1;
    spi_dev_t *spi = (spi_dev_t *)node->i_arg;

    ddkc_dbg("%s - node:%p, i_name:%s, spi:%p\r\n", __func__, node, node->i_name, spi);
    if (!spi) {
        ddkc_dbg("spi is NULL,invalid and ignore\r\n");
        return -EINVAL;
    }

    //TODO: check whether it is for console, special operation needed for console
    /* open spi with default parameter */
    ret = hal_spi_init(spi);
    if (ret) {
        ddkc_err("hal_spi_init failed, ret:%d\r\n", ret);
        return ret;
    }

    hal_spi_recv_cb_reg(spi, NULL);
    //spi->priv = f;

    return 0;
}

int spi_device_close (file_t *f) {
    int ret = 0;

    ddkc_dbg("%s\r\n", __func__);
    spi_dev_t *spi = (spi_dev_t *)f->node->i_arg;

    //TODO: check whether it is for console, special operation needed for console
    if (!spi) {
        ddkc_dbg("f->node->i_arg is NULL, invalid and ignore\r\n");
        return -EINVAL;
    }

    hal_spi_recv_cb_reg(spi, NULL);
    /* open spi with default parameter */
    ret = hal_spi_finalize(spi);
    if (ret) {
        ddkc_err("hal_spi_finalize failed, ret:%d\r\n", ret);
        return ret;
    }

    return 0;
}

int spi_rx_notify (spi_dev_t *spi) {
    struct pollfd *fd = NULL;
    spi_poll_notify_t *spi_poll = NULL;

    if (!spi) {
        ddkc_err("spi should not be NULL in %s\r\n", __func__);
        return -1;
    }
    ddkc_dbg("enter %s\r\n", __func__);
    //fp = (file_t *)spi->priv;

    spi_poll = &g_spi_notify[spi->port];

    if (spi_poll->notify) {
        mutex_lock(&spi_poll->mutex);
        spi_poll->flag = true;

        fd = (struct pollfd *)spi_poll->fd;
        fd->revents |= POLLIN;
        /* add POLLOUT by default */
        fd->revents |= POLLOUT;

        (*(spi_poll->notify)) (fd, spi_poll->arg);

        spi_poll->notify = NULL;
        spi_poll->fd = NULL;
        spi_poll->arg = NULL;

        hal_spi_recv_cb_reg(spi, NULL);

        mutex_unlock(&spi_poll->mutex);

    } else {
        ddkc_err("!!!impossible case happened!!! spi_poll->notify is NULL\r\n");
    }


    return 0;
}

int spi_device_poll (file_t *f, int flag, poll_notify_t notify, void *fd, void *arg) {
    spi_poll_notify_t *spi_poll = NULL;
    spi_dev_t *spi = (spi_dev_t *)f->node->i_arg;

    //TODO: check whether it is for console, special operation needed for console
    if (!spi) {
        ddkc_dbg("f->node->i_arg is NULL, invalid and ignore\r\n");
        return -EINVAL;
    }
    ddkc_dbg("enter %s, fd:%p, arg:%p, notify:%p\r\n", __func__, fd, arg, notify);

    spi_poll = &g_spi_notify[spi->port];

    if (!fd) {
        mutex_lock(&spi_poll->mutex);
        hal_spi_recv_cb_reg(spi, NULL);
        spi_poll->notify = NULL;
        spi_poll->fd = NULL;
        spi_poll->arg = NULL;
        mutex_unlock(&spi_poll->mutex);
        return 0;
    }

    if (!spi_poll->flag) {
        mutex_lock(&spi_poll->mutex);

        spi_poll->notify = notify;
        spi_poll->fd = (struct pollfd *)fd;
        spi_poll->arg = arg;

        hal_spi_recv_cb_reg(spi, spi_rx_notify);
        ((struct pollfd *)fd)->revents &= ~POLLIN;

        mutex_unlock(&spi_poll->mutex);
    } else {
        ddkc_warn("spi_poll->flag is true, notify user directly\r\n");
        ((struct pollfd *)fd)->events |= POLLIN;
        (*notify)(fd, arg);
    }

    // add POLLOUT to events by default
    ((struct pollfd *)fd)->events |= POLLOUT;

    return 0;
}

/************************** device ****************************/


subsys_file_ops_t spi_device_fops = {
    .open = spi_device_open,
    .close = spi_device_close,
    .read = spi_device_read,
    .write = spi_device_write,
    .ioctl = spi_device_ioctl,
    .poll = spi_device_poll,
    .lseek = NULL,
};

int spi_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int spi_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int spi_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv spi_device_drv = {
    .drv_name = "spi",
    .init = spi_device_init,
    .deinit = spi_device_deinit,
    .pm = spi_device_pm,
};

struct subsys_dev *g_spi_device_array[PLATFORM_SPI_NUM];

static spi_dev_t g_def_setting = {
        .port = 0,
        .config = {
            .role = SPI_ROLE_MASTER,
            .firstbit = SPI_FIRSTBIT_MSB,
            .mode = SPI_WORK_MODE_0,
            .t_mode = SPI_TRANSFER_NORMAL,
            .freq = DEFAULT_SPI_FREQ,
            .serial_len = DEFAULT_SPI_SERAIL_LEN,
            .data_size = SPI_DATA_SIZE_8BIT,
            .cs = SPI_CS_EN,
        },
        .priv = NULL,
    };

int vfs_spi_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_info("spi vfs driver init starts\r\n");

    node_name_len = strlen(SPI_DEV_NAME_FORMAT) + 1;

    memset(g_spi_notify, 0, sizeof(g_spi_notify));
    memset(g_spi_device_array, 0, sizeof(g_spi_device_array));
    ppsdev = g_spi_device_array;

    for (i = 0; i < PLATFORM_SPI_NUM; i++) {
        spi_dev_t *spi_dev = malloc(sizeof(*spi_dev));

        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);

        if (!spi_dev || !(*ppsdev)) {
            ddkc_info("malloc for spi_dev:%p or subsys_dev:%p failed, \r\n", spi_dev, *ppsdev);
            if (spi_dev) {
                free(spi_dev);
            }
            if (*ppsdev) {
                free(*ppsdev);
                *ppsdev = NULL;
            }

            goto err;
        }

        memset(*ppsdev, 0, sizeof(struct subsys_dev) + node_name_len);

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, SPI_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, node_name:%s, (*ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        memcpy(spi_dev, &g_def_setting, sizeof(*spi_dev));
        spi_dev->port = i;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = spi_dev;

        mutex_init(&g_spi_notify[i].mutex);
        ret = aos_dev_reg(*ppsdev, &spi_device_fops, &spi_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for pwm%d failed, ret:%d\r\n", i, ret);

            free(spi_dev);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_info("spi vfs driver init finish, ret:%d\r\n", ret);

    return 0;

err:
    ppsdev = g_spi_device_array;

    for (j = 0; j < i; j++) {
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for spi%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);
            free(*ppsdev);

            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("spi vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

//FIXME: not implement for pangu now.
#ifndef CONFIG_CHIP_PANGU
VFS_DRIVER_ENTRY(vfs_spi_drv_init)
#endif

#endif
