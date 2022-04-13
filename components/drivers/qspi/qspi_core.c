/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <aos/hal/qspi.h>
#include <hal2vfs/io_qspi.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_QSPI_NUM
#define PLATFORM_QSPI_NUM CONFIG_QSPI_NUM
#else
#define PLATFORM_QSPI_NUM 4
#endif
// SPI device node will be named with "/dev/qspi<x>", where <x> is qspi port id
#define QSPI_DEV_NAME_FORMAT "qspi%d"

#if (PLATFORM_QSPI_NUM > 0)

#define IOC_STATE_CHECK_AND_BREAK() \
if (!f->f_arg) { \
    ddkc_warn("eighter CFLAG or FREQ is not set\r\n"); \
    ret = -EIO; \
    break; \
}

ssize_t qspi_device_read (file_t *f, void *buffer, size_t size) {
    qspi_transfer_t rx;
    qspi_data_t data;
    qspi_dev_t *qspi = (qspi_dev_t *)f->node->i_arg;

    if (!qspi || !buffer || !size) {
        ddkc_dbg("invalid qspi:%p, buffer:%p or size:%d\r\n", qspi, buffer, size);
        return -EINVAL;
    }
    memset(&data, 0, sizeof(data));

    data.rx_buf = buffer;
    data.rx_size = size;
    data.timeout = 100;

    rx.cmd = NULL;
    rx.data = &data;

    return hal_qspi_recv(qspi, &rx);
}

ssize_t qspi_device_write (file_t *f, const void *buffer, size_t size) {
    qspi_transfer_t tx;
    qspi_data_t data;
    qspi_dev_t *qspi = (qspi_dev_t *)f->node->i_arg;

    if (!qspi || !buffer || !size) {
        ddkc_dbg("invalid qspi:%p, buffer:%p or size:%d\r\n", qspi, buffer, size);
        return -EINVAL;
    }
    memset(&data, 0, sizeof(data));

    data.tx_buf = buffer;
    data.tx_size = size;
    data.timeout = 100;

    tx.cmd = NULL;
    tx.data = &data;

    return hal_qspi_send(qspi, &tx);
}

static qspi_work_mode_e _arg_to_work_mode (unsigned long arg) {
    qspi_work_mode_e mode = 0;
    int control = arg & (QSPI_CPOL|QSPI_CPHA);

    switch (control) {
        case QSPI_MODE_0:
            mode = QSPI_WORK_MODE_0;
            break;

        case QSPI_MODE_1:
            mode = QSPI_WORK_MODE_1;
            break;

        case QSPI_MODE_2:
            mode = QSPI_WORK_MODE_2;
            break;

        case QSPI_MODE_3:
            mode = QSPI_WORK_MODE_3;
            break;

        default:
            ddkc_dbg("!!!impossible!!! mode:%d, set to mode 0\r\n", control);
            mode = QSPI_WORK_MODE_0;
            break;
    }

    return mode;
}



static qspi_transfer_mode_e _arg_to_transfer_mode (unsigned long arg) {
    qspi_transfer_mode_e mode = 0;
    int control = arg & QSPI_TRANSFER_MODE_MASK;

    switch (control) {
        case QSPI_TRANSFER_DMA:
            mode = QSPI_TRANSFER_MODE_DMA;
            break;

        case QSPI_TRANSFER_PIO:
            mode = QSPI_TRANSFER_MODE_NORMAL;
            break;

        default:
            ddkc_dbg("invalid mode:%d, set to normal mode by default\r\n", control);
            mode = QSPI_TRANSFER_MODE_NORMAL;
            break;
    }

    return mode;
}

static unsigned int _arg_to_bus_width (unsigned long arg) {
    unsigned int bus_width = 0;
    int line = arg & QSPI_BUS_LINE_MASK;

    switch (line) {
        case QSPI_BUS_1_LINE:
            bus_width = 1;
            break;

        case QSPI_BUS_2_LINE:
            bus_width = 2;
            break;

        case QSPI_BUS_4_LINE:
            bus_width = 3;
            break;

        default:
            bus_width = 4;
            ddkc_dbg("invalid bus line set:%d, set to bus_width:%d\r\n",
                     line, bus_width);
            break;
    }

    return bus_width;
}

static qspi_cs_e _arg_to_cs (unsigned long arg) {
    qspi_cs_e cs = 0;
    int control = arg & (QSPI_NO_CS | QSPI_CS_HIGH);

    switch (control) {
        case QSPI_NO_CS:
            cs = QSPI_CS_DIS;
            break;
        case QSPI_CS_HIGH:
            cs = QSPI_CS_H;
            break;
        default:
            cs = QSPI_CS_L;
            break;
    }

    return cs;
}

static int _arg_to_ddr_enable (unsigned long arg) {
    return arg & QSPI_DDR_ENABLE ? 1 : 0;
}

static inline int _qspi_check_transfer (ioc_qspi_transfer_t *transfer) {

    if (!transfer || !transfer->cmd || !transfer->data) {
        ddkc_err("transfer:%p, cmd:%p or data:%p is NULL, ignore\r\n",
                 transfer, transfer->cmd, transfer->data);
        return -EINVAL;
    }

    return 0;
}

static inline int _qspi_send_cmd(qspi_dev_t *qdev, ioc_qspi_cmd_t *cmd) {

    if (!cmd)
        ddkc_err("invalid cmd:%p\r\n", cmd);

    return hal_qspi_send_cmd(qdev, (qspi_cmd_t *)cmd);
}

static inline int _qspi_send_data(qspi_dev_t *qdev, ioc_qspi_transfer_t *tx) {
    int ret = _qspi_check_transfer(tx);

    return ret ? ret : hal_qspi_send(qdev, (qspi_transfer_t *)tx);
}

static inline int _qspi_recv_data(qspi_dev_t *qdev, ioc_qspi_transfer_t *rx) {
    int ret = _qspi_check_transfer(rx);

    return ret ? ret : hal_qspi_recv(qdev, (qspi_transfer_t *)rx);
}

static inline int _qspi_send_recv_data(qspi_dev_t *qdev, ioc_qspi_transfer_t *trx) {
    return _qspi_check_transfer(trx);
}

int qspi_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    qspi_config_t *config = NULL;
    qspi_dev_t *qspi = (qspi_dev_t *)f->node->i_arg;

    if (!qspi) {
        ddkc_dbg("qspi is NULL,invalid and ignore\r\n", qspi);
        return -EINVAL;
    }

    config = &qspi->config;

    ddkc_dbg("i_name:%s, qspi:%p, cmd:%d, arg:0x%x\r\n", f->node->i_name, qspi, cmd, arg);

    switch (cmd) {
        case IOC_QSPI_SEND_DATA:
            IOC_STATE_CHECK_AND_BREAK();
            ddkc_dbg("IOC_QSPI_SEND_DATA, arg:%p\r\n", arg);
            ret = _qspi_send_data(qspi, (ioc_qspi_transfer_t *)arg);
            break;

        case IOC_QSPI_RECV_DATA:
            IOC_STATE_CHECK_AND_BREAK();
            ddkc_dbg("IOC_QSPI_RECV_DATA, arg:%p\r\n", arg);
            ret = _qspi_recv_data(qspi, (ioc_qspi_transfer_t *)arg);
            break;

        case IOC_QSPI_SEND_RECV_DATA:
            IOC_STATE_CHECK_AND_BREAK();
            ddkc_dbg("IOC_QSPI_SEND_RECV_DATA, arg:%p\r\n", arg);
            ret = _qspi_send_recv_data(qspi, (ioc_qspi_transfer_t *)arg);
            break;

        case IOC_QSPI_SEND_CMD:
            IOC_STATE_CHECK_AND_BREAK();
            ddkc_dbg("IOC_QSPI_SEND_CMD, arg:%p\r\n", arg);
            ret = _qspi_send_cmd(qspi, (ioc_qspi_cmd_t *)arg);
            break;
        case IOC_QSPI_SET_CFLAG:
            ddkc_dbg("IOC_QSPI_SET_CFLAG, arg:%p\r\n", arg);
            config->cs = _arg_to_cs(arg);
            config->mode = _arg_to_work_mode(arg);
            config->t_mode = _arg_to_transfer_mode(arg);
            config->bus_width = _arg_to_bus_width(arg);
            config->ddr_enable = _arg_to_ddr_enable(arg);
            ddkc_dbg("cs:%d, mode:%d, trans_mode:%d, bus_width:%d, ddr_enable:%d, freq:%d\r\n",
                     config->cs, config->mode, config->t_mode,
                     config->bus_width, config->ddr_enable, config->freq);

            if (config->freq) {
                // reinitialization check
                if (f->f_arg)
                    hal_qspi_finalize(qspi);

                ret = hal_qspi_init(qspi);
                if (ret) {
                    ddkc_warn("hal_qspi_init failed, ret:%d\r\n", ret);
                    f->f_arg = NULL;
                } else
                    f->f_arg = qspi;
            }
            break;

        case IOC_QSPI_SET_FREQ:
            ddkc_dbg("IOC_QSPI_SET_FREQ, arg:%d\r\n", arg);
            config->freq = arg;
            ddkc_dbg("cs:%d, mode:%d, trans_mode:%d, bus_width:%d, ddr_enable:%d, freq:%d\r\n",
                     config->cs, config->mode, config->t_mode,
                     config->bus_width, config->ddr_enable, config->freq);

            if (config->bus_width) {
                // reinitialization check
                if (f->f_arg)
                    hal_qspi_finalize(qspi);

                ret = hal_qspi_init(qspi);
                if (ret) {
                    f->f_arg = NULL;
                    ddkc_warn("hal_qspi_init failed, ret:%d\r\n", ret);
                } else
                    f->f_arg = qspi;
            }

            break;

        default:
            ddkc_warn("invalid command, cmd:%d, arg:%ld\r\n", cmd, arg);
            break;
    }

    return ret;
}

int qspi_device_open (inode_t *node, file_t *f) {
    qspi_dev_t *qspi = (qspi_dev_t *)node->i_arg;

    ddkc_dbg("%s - node:%p, i_name:%s, qspi:%p\r\n", __func__, node, node->i_name, qspi);
    if (!qspi) {
        ddkc_dbg("qspi is NULL,invalid and ignore\r\n", qspi);
        return -EINVAL;
    }

    return 0;
}

int qspi_device_close (file_t *f) {
    int ret = 0;

    ddkc_dbg("%s\r\n", __func__);
    qspi_dev_t *qspi = (qspi_dev_t *)f->node->i_arg;

    //TODO: check whether it is for console, special operation needed for console
    if (!qspi) {
        ddkc_dbg("qspi:%p is NULL, invalid and ignore\r\n",
                 qspi, f->node, f->node->i_arg);
        return -EINVAL;
    }

    if (f->f_arg) {
        //f_arg is not NULL means hal_qspi_init is called in eralier operation
        ret = hal_qspi_finalize(qspi);
        if (ret)
            ddkc_err("hal_qspi_finalize failed, ret:%d\r\n", ret);
        f->f_arg = NULL;
    }

    memset(&qspi->config, 0, sizeof(qspi->config));

    return ret;
}

/************************** device ****************************/


subsys_file_ops_t qspi_device_fops = {
    .open = qspi_device_open,
    .close = qspi_device_close,
    .read = qspi_device_read,
    .write = qspi_device_write,
    .ioctl = qspi_device_ioctl,
    .poll = NULL,
    .lseek = NULL,
};

int qspi_device_init (struct platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int qspi_device_deinit (struct platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int qspi_device_pm (struct platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv qspi_device_drv = {
    .drv_name = "qspi",
    .init = qspi_device_init,
    .deinit = qspi_device_deinit,
    .pm = qspi_device_pm,
};

struct subsys_dev *g_qspi_device_array[PLATFORM_QSPI_NUM];

#if 0
static qspi_dev_t g_def_setting = {
    .port = 0,
    .config = {
        .cs = QSPI_CS_EN,
        .mode = QSPI_WORK_MODE_0,
        .t_mode = QSPI_TRANSFER_PIO,
        .freq = DEFAULT_QSPI_FREQ,
        .bus_width = QSPI_BUS_QUAD,
        .ddr_enable = false,
    },
    .priv = NULL,
};
#endif

int vfs_qspi_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_info("qspi vfs driver init starts\r\n");

    /**
     * base on the following assumpations:
     * 1. ioc_qspi_cmd_t and qspi_cmd_t are completely the same
     * 2. ioc_qspi_data_t and qspi_data_t are completely the same
     * 3. ioc_qspi_transfer_t and qspi_transfer_t are completely the same
     * here we simply check whether their size qeuals or not
     */

    BUG_ON_MSG(sizeof(ioc_qspi_transfer_t) != sizeof(qspi_transfer_t),
               "ioc_qspi_transfer_t and qspi_transfer_t are different, please check !!!\r\n");
    BUG_ON_MSG(sizeof(ioc_qspi_data_t) != sizeof(qspi_data_t),
               "ioc_qspi_data_t and qspi_data_t are different, please check !!!\r\n");
    BUG_ON_MSG(sizeof(ioc_qspi_cmd_t) != sizeof(qspi_cmd_t),
               "ioc_qspi_cmd_t and qspi_cmd_t are different, please check !!!\r\n");

    node_name_len = strlen(QSPI_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    memset(g_qspi_device_array, 0, sizeof(g_qspi_device_array));
    ppsdev = g_qspi_device_array;

    for (i = 0; i < PLATFORM_QSPI_NUM; i++) {
        qspi_dev_t *qspi_dev = malloc(sizeof(*qspi_dev));

        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);

        if (!qspi_dev || !(*ppsdev)) {
            ddkc_info("malloc for qspi_dev:%p or subsys_dev:%p failed, \r\n", qspi_dev, *ppsdev);
            if (qspi_dev)
                free(qspi_dev);
            qspi_dev = NULL;

            if (*ppsdev)
                free(*ppsdev);
            *ppsdev = NULL;
            goto err;
        }

        memset(*ppsdev, 0, sizeof(struct subsys_dev) + node_name_len);

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, QSPI_DEV_NAME_FORMAT, i);
        pr_info("*ppsdev:%p, (*ppsdev) + 1:%p, node_name:%s, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        // donot assign default setting
        //memcpy(qspi_dev, &g_def_setting, sizeof(*qspi_dev));
        qspi_dev->port = i;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = qspi_dev;
        ret = aos_dev_reg(*ppsdev, &qspi_device_fops, &qspi_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for pwm%d failed, ret:%d\r\n", i, ret);

            free(qspi_dev);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }

        ppsdev++;
    }

    ddkc_info("qspi vfs driver init finish, ret:%d\r\n", ret);

    return 0;

err:
    ppsdev = g_qspi_device_array;

    for (j = 0; j < i; j++) {
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);

            ddkc_info("free memory for qspi%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);
            free(*ppsdev);

            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("qspi vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_qspi_drv_init)


__weak int32_t hal_qspi_init (qspi_dev_t *qspi) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_qspi_send (qspi_dev_t *qspi, qspi_transfer_t *tx) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_qspi_recv (qspi_dev_t *qspi, qspi_transfer_t *rx) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_qspi_send_recv (qspi_dev_t *qspi, qspi_transfer_t *trx) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_qspi_send_cmd (qspi_dev_t *qspi, qspi_cmd_t *cmd) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

__weak int32_t hal_qspi_finalize (qspi_dev_t *qspi) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}
#endif
