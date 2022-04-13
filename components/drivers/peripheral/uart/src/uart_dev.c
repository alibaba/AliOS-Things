/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <poll.h>

#include <aos/hal/uart.h>
#include <vfsdev/uart_dev.h>
#include <devicevfs/devicevfs.h>

#ifdef CONFIG_UART_NUM
#define PLATFORM_UART_NUM CONFIG_UART_NUM
#else
#define PLATFORM_UART_NUM 4
#endif

#define SYS_LOG_UART_PORT 0

#if (PLATFORM_UART_NUM > 0)

// UART device node will be named with "/dev/ttyUART<x>", where <x> is uart port id
#define UART_DEV_NAME_FORMAT "ttyUART%d"

/* used to notify user on poll events */
typedef struct uart_poll_notify {
    bool flag;
    struct mutex mutex;
    poll_notify_t notify;
    struct pollfd *fd;
    void *arg;
} uart_poll_notify_t;

typedef struct vfs_uart {
    int ref_cnt;
    struct mutex m;
    uart_dev_t dev;
} vfs_uart_t;

static uart_poll_notify_t g_uart_notify[PLATFORM_UART_NUM];

static int uart_rx_notify (uart_dev_t *uart);

__weak int32_t hal_uart_recv_cb_reg(uart_dev_t *uart, uart_rx_cb cb) {
    ddkc_warn("%s, uart:%p, cb:%p\r\n", __func__, uart, cb);
    return 0;
}

ssize_t uart_device_read (file_t *f, void *buffer, size_t size) {
    unsigned int i = 0;
    int ret = 0;
    uart_dev_t *uart = NULL;
    uart_poll_notify_t *uart_poll = NULL;
    vfs_uart_t *vu = (vfs_uart_t *)f->node->i_arg;;

    ddkc_dbg("%s - node:%p, i_name:%s, i_arg:%p\r\n", __func__, f->node, f->node->i_name, vu);

    if (!vu || !buffer || !size) {
        ddkc_err("invalid vfs_uart_t:%p, buffer:%p or size:%d\r\n", vu, buffer, size);
        return -EINVAL;
    }

    uart = &vu->dev;

    mutex_lock(&vu->m);

    uart_poll = &g_uart_notify[uart->port];
    if (uart_poll->flag) {
        mutex_lock(&uart_poll->mutex);
        uart_poll->flag = false;
        mutex_unlock(&uart_poll->mutex);
    }
    ret = hal_uart_recv_II(uart, buffer, size, &i, 100);

    ddkc_dbg("%s, uart:%p, i:%d, ret:%d\r\n", __func__, uart, i, ret);
    mutex_unlock(&vu->m);

    return !ret ? i : (ret < 0 ? ret : -ret);
}

ssize_t uart_device_write (file_t *f, const void *buffer, size_t size) {
    int i = 0;
    int ret = 0;
    char *ptr = (char *)buffer;
    uart_dev_t *uart = NULL;
    vfs_uart_t *vu = (vfs_uart_t *)f->node->i_arg;;

    ddkc_dbg("%s - node:%p, i_name:%s, i_arg:%p\r\n", __func__, f->node, f->node->i_name, vu);

    if (!vu || !buffer || !size) {
        ddkc_err("invalid vfs_uart_t:%p, buffer:%p or size:%d\r\n", vu, buffer, size);
        return -EINVAL;
    }
    uart = &vu->dev;

    mutex_lock(&vu->m);

    while (size--) {
        ret = hal_uart_send(uart, ptr++, 1, 1000);
        if (ret)
            break;
        i++;
    }
    ddkc_dbg("%s, uart:%p, i:%d, ret:%d\r\n", __func__, uart, i, ret);

    mutex_unlock(&vu->m);

    return i ? i : ret;
}

static unsigned int _arg_to_baudrate (unsigned long arg) {
    unsigned baudrate = 0;
    int bits = arg & CBAUD;

    if (!(bits)) {
        ddkc_warn("arg:0x%lx - no baudrate bits set, ignore\r\n", arg);
        return 0;
    }

    switch (bits) {
        case B50:
            baudrate = 50;
            break;
        case B75:
            baudrate = 75;
            break;
        case B110:
            baudrate = 110;
            break;
        case B134:
            baudrate = 134;
            break;
        case B150:
            baudrate = 150;
            break;
        case B200:
            baudrate = 200;
            break;
        case B300:
            baudrate = 300;
            break;
        case B600:
            baudrate = 600;
            break;
        case B1200:
            baudrate = 1200;
            break;
        case B1800:
            baudrate = 1800;
            break;
        case B2400:
            baudrate = 2400;
            break;
        case B4800:
            baudrate = 4800;
            break;
        case B9600:
            baudrate = 9600;
            break;
        case B19200:
            baudrate = 19200;
            break;
        case B38400:
            baudrate = 38400;
            break;
        case B57600:
            baudrate = 57600;
            break;
        case B115200:
            baudrate = 115200;
            break;
        case B230400:
            baudrate = 230400;
            break;
        case B460800:
            baudrate = 460800;
            break;
        case B500000:
            baudrate = 500000;
            break;
        case B576000:
            baudrate = 576000;
            break;
        case B921600:
            baudrate = 921600;
            break;
        case B1000000:
            baudrate = 1000000;
            break;
        case B1152000:
            baudrate = 1152000;
            break;
        case B1500000:
            baudrate = 1500000;
            break;
        case B2000000:
            baudrate = 2000000;
            break;
        case B2500000:
            baudrate = 2500000;
            break;
        case B3000000:
            baudrate = 3000000;
            break;
        case B3500000:
            baudrate = 3500000;
            break;
        case B4000000:
            baudrate = 4000000;
            break;
        default:
            ddkc_warn("arg:0x%lx - invalid and impossible case, set to 912600 by default\r\n",
                      arg);
            /* set to 921600 by default */
            baudrate = 921600;
            break;
    }
    return baudrate;
}

static hal_uart_flow_control_t _arg_to_flow_control (unsigned long arg) {
    hal_uart_flow_control_t c = 0;
    int control = arg & CRTS_CTS;

    switch (control) {
        case 0:
            c = FLOW_CONTROL_DISABLED;
            break;
        case CRTS_CTS:
            c = FLOW_CONTROL_CTS_RTS;
            break;
        case CRTS:
            c = FLOW_CONTROL_RTS;
            break;
        case CCTS:
            c = FLOW_CONTROL_CTS;
            break;
        default:
            c = FLOW_CONTROL_DISABLED;
            break;
    }

    return c;
}

static hal_uart_stop_bits_t _arg_to_stop_bits (unsigned long arg) {
    return  arg & CSTOPB ? STOP_BITS_2 : STOP_BITS_1;
}

static hal_uart_parity_t _arg_to_parity(int arg) {

    if (arg & PARENB)
        return EVEN_PARITY;
    else if (arg & PARODD)
        return ODD_PARITY;
    else
        return NO_PARITY;
}

static hal_uart_data_width_t _arg_to_data_width (unsigned long arg) {
    unsigned size = 0;
    int bits = arg & CSIZE;

    switch (bits) {
        case CS5:
            size = DATA_WIDTH_5BIT;
            break;
        case CS6:
            size = DATA_WIDTH_6BIT;
            break;
        case CS7:
            size = DATA_WIDTH_7BIT;
            break;
        case CS8:
            size = DATA_WIDTH_8BIT;
            break;
        default:
            ddkc_warn("arg:0x%lx - invalid and impossible case, set to 8bit by default\r\n",
                      arg);
            size = DATA_WIDTH_8BIT;
            break;
    }

    return size;
}

int uart_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    uart_config_t *config = NULL;
    uart_dev_t *uart = NULL;
    vfs_uart_t *vu = (vfs_uart_t *)f->node->i_arg;;

    ddkc_dbg("%s - node:%p, i_name:%s, i_arg:%p\r\n", __func__, f->node, f->node->i_name, vu);
    if (!vu) {
        ddkc_err("uart is NULL,invalid and ignore\r\n");
        return -EINVAL;
    }
    uart = &vu->dev;

    mutex_lock(&vu->m);

    config = &uart->config;

    ddkc_dbg("i_name:%s, uart:%p, cmd:%d, arg:0x%lx\r\n", f->node->i_name, uart, cmd, arg);

    switch (cmd) {
        case IOC_UART_SET_CFLAG:
            ret = hal_uart_finalize(uart);
            if (ret) {
                ddkc_warn("hal_uart_finalize failed, ret:%d\r\n", ret);
                break;
            }

            config->baud_rate = _arg_to_baudrate(arg);
            config->flow_control = _arg_to_flow_control(arg);
            config->stop_bits = _arg_to_stop_bits(arg);
            config->parity = _arg_to_parity(arg);
            config->data_width = _arg_to_data_width(arg);

            ret = hal_uart_init(uart);
            if (ret) {
                ddkc_warn("hal_uart_init failed, ret:%d\r\n", ret);
            }
            break;
        case IOC_UART_GET_CFLAG:
            //TODO: to be implemented
            break;
        case IOC_UART_POLL_TEST:
            uart_rx_notify(uart);
            break;
        default:
            break;
    }
    mutex_unlock(&vu->m);

    return ret;
}

int uart_device_open (inode_t *node, file_t *f) {
    int ret = -1;
    uart_dev_t *uart = NULL;
    vfs_uart_t *vu = (vfs_uart_t *)node->i_arg;;

    ddkc_dbg("%s - node:%p, i_name:%s, i_arg:%p\r\n", __func__, node, node->i_name, vu);
    if (!vu) {
        ddkc_err("vfs_uart_t is NULL,invalid and ignore\r\n");
        return -EINVAL;
    }
    uart = &vu->dev;

    mutex_lock(&vu->m);

    if (vu->ref_cnt) {
        ddkc_dbg("uart%d is already opened, ref_cnt:%d\r\n", uart->port, vu->ref_cnt);
        mutex_unlock(&vu->m);
        return 0;
    }
    //TODO: check whether it is for console, special operation needed for console
    /* open UART with default parameter */
    ret = hal_uart_init(uart);
    if (ret) {
        ddkc_err("hal_uart_init failed, ret:%d\r\n", ret);
        mutex_unlock(&vu->m);
        return -EIO;
    }

    hal_uart_recv_cb_reg(uart, NULL);
    uart->priv = f;
    vu->ref_cnt++;
    mutex_unlock(&vu->m);

    return 0;
}

int uart_device_close (file_t *f) {
    int ret = 0;
    uart_dev_t *uart = NULL;
    vfs_uart_t *vu = (vfs_uart_t *)f->node->i_arg;;

    ddkc_dbg("%s - node:%p, i_name:%s, i_arg:%p\r\n", __func__, f->node, f->node->i_name, vu);
    if (!vu) {
        ddkc_err("vfs_uart_t is NULL,invalid and ignore\r\n");
        return -EINVAL;
    }
    mutex_lock(&vu->m);

    vu->ref_cnt--;

    if (vu->ref_cnt != 0) {
        ddkc_warn("vu->ref_cnt：%d, ignore close operation on %s\r\n", vu->ref_cnt, f->node->i_name);
        mutex_unlock(&vu->m);
        return 0;
    }

    uart = &vu->dev;

    hal_uart_recv_cb_reg(uart, NULL);
    /* open UART with default parameter */
    ret = hal_uart_finalize(uart);
    if (ret) {
        ddkc_err("hal_uart_finalize failed, ret:%d\r\n", ret);
    }

    uart->priv = NULL;

    mutex_unlock(&vu->m);

    return 0;
}

int uart_rx_notify (uart_dev_t *uart) {
    file_t *fp = NULL;
    struct pollfd *fd = NULL;
    uart_poll_notify_t *uart_poll = NULL;

    if (!uart) {
        ddkc_err("uart should not be NULL in %s\r\n", __func__);
        return -1;
    }

    fp = (file_t *)uart->priv;
    ddkc_dbg("enter %s, fp:%p\r\n", __func__, fp);

    uart_poll = &g_uart_notify[uart->port];

    if (uart_poll->notify) {
        mutex_lock(&uart_poll->mutex);
        uart_poll->flag = true;

        fd = (struct pollfd *)uart_poll->fd;
        fd->revents |= POLLIN;
        /* add POLLOUT by default */
        fd->revents |= POLLOUT;

        (*(uart_poll->notify)) (fd, uart_poll->arg);

        uart_poll->notify = NULL;
        uart_poll->fd = NULL;
        uart_poll->arg = NULL;

        hal_uart_recv_cb_reg(uart, NULL);

        mutex_unlock(&uart_poll->mutex);

    } else {
        ddkc_err("!!!impossible case happened!!! uart_poll->notify is NULL\r\n");
    }


    return 0;
}

int uart_device_poll (file_t *f, int flag, poll_notify_t notify, void *fd, void *arg) {
    uart_poll_notify_t *uart_poll = NULL;
    uart_dev_t *uart = NULL;
    vfs_uart_t *vu = (vfs_uart_t *)f->node->i_arg;;

    ddkc_dbg("%s - node:%p, i_name:%s, i_arg:%p\r\n", __func__, f->node, f->node->i_name, vu);
    if (!vu) {
        ddkc_err("vfs_uart_t is NULL,invalid and ignore\r\n");
        return -EINVAL;
    }
    uart = &vu->dev;

    ddkc_dbg("enter %s, fd:%p, arg:%p, notify:%p\r\n", __func__, fd, arg, notify);

    uart_poll = &g_uart_notify[uart->port];

    if (!fd) {
        mutex_lock(&uart_poll->mutex);
        hal_uart_recv_cb_reg(uart, NULL);
        uart_poll->notify = NULL;
        uart_poll->fd = NULL;
        uart_poll->arg = NULL;
        mutex_unlock(&uart_poll->mutex);
        return 0;
    }

    if (!uart_poll->flag) {
        mutex_lock(&uart_poll->mutex);

        uart_poll->notify = notify;
        uart_poll->fd = (struct pollfd *)fd;
        uart_poll->arg = arg;

        hal_uart_recv_cb_reg(uart, uart_rx_notify);
        ((struct pollfd *)fd)->revents &= ~POLLIN;

        mutex_unlock(&uart_poll->mutex);
    } else {
        ddkc_warn("uart_poll->flag is true, notify user directly\r\n");
        ((struct pollfd *)fd)->events |= POLLIN;
        (*notify)(fd, arg);
    }

    // add POLLOUT to events by default
    ((struct pollfd *)fd)->events |= POLLOUT;
    (*notify)(fd, arg);

    return 0;
}

/************************** device ****************************/


subsys_file_ops_t uart_device_fops = {
    .open = uart_device_open,
    .close = uart_device_close,
    .read = uart_device_read,
    .write = uart_device_write,
    .ioctl = uart_device_ioctl,
    .poll = uart_device_poll,
    .lseek = NULL,
};

int uart_device_init (struct u_platform_device *pdev) {
    void *user_data = NULL;
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    user_data = u_platform_get_user_data(pdev);
    ddkc_dbg("pdev:%p, user_data:%p\r\n", pdev, user_data);
    return 0;
}

int uart_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int uart_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv uart_device_drv = {
    .drv_name = "uart",
    .init = uart_device_init,
    .deinit = uart_device_deinit,
    .pm = uart_device_pm,
};

struct subsys_dev *g_uart_device_array[PLATFORM_UART_NUM];

static uart_dev_t g_def_setting = {
        .port = 0,
        .config = {
            .baud_rate = 1500000,
            .data_width = DATA_WIDTH_8BIT,
            .parity = NO_PARITY,
            .stop_bits = STOP_BITS_1,
            .flow_control = FLOW_CONTROL_DISABLED,
            .mode = MODE_TX_RX,
        },
        .priv = NULL,
    };

int vfs_uart_drv_init (void) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev **ppsdev = NULL;

    ddkc_dbg("uart vfs driver init starts\r\n");

    node_name_len = strlen(UART_DEV_NAME_FORMAT) + 1;

    memset(g_uart_notify, 0, sizeof(g_uart_notify));
    memset(g_uart_device_array, 0, sizeof(g_uart_device_array));
    ppsdev = g_uart_device_array;

    for (i = 0; i < PLATFORM_UART_NUM; i++) {
        vfs_uart_t *vu = NULL;
        uart_dev_t *uart_dev = NULL;
        vu = malloc(sizeof(vfs_uart_t));
        *ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);

        if (!vu || !(*ppsdev)) {
            ddkc_err("malloc for vfs_uart_t：%p or subsys_dev:%p failed, \r\n", vu, *ppsdev);
            if (*ppsdev) {
                free(*ppsdev);
                *ppsdev = NULL;
            }

            if (vu)
                free(vu);

            goto err;
        }

        memset(*ppsdev, 0, sizeof(struct subsys_dev) + node_name_len);

        (*ppsdev)->node_name = (char *)((*ppsdev) + 1);
        snprintf((*ppsdev)->node_name, node_name_len, UART_DEV_NAME_FORMAT, i);
        ddkc_dbg("*ppsdev:%p, node_name:%s, (*ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
                *ppsdev, (*ppsdev)->node_name, (*ppsdev) + 1, sizeof(struct subsys_dev));
        (*ppsdev)->permission = 0;
        // please refer to definitions in enum SUBSYS_BUS_TYPE
        (*ppsdev)->type = BUS_TYPE_PLATFORM;
        vu->m.flag = 0;
        mutex_init(&vu->m);

        uart_dev = &vu->dev;
        vu->ref_cnt = 0;
        memcpy(uart_dev, &g_def_setting, sizeof(*uart_dev));
        uart_dev->port = i;
        // user_data will be passed to open operation via node->i_arg
        (*ppsdev)->user_data = vu;

        mutex_init(&g_uart_notify[i].mutex);

        ret = aos_dev_reg(*ppsdev, &uart_device_fops, &uart_device_drv);
        if (ret) {
            ddkc_err("aos_dev_reg for uart devices failed, ret:%d\r\n", ret);

            free(vu);
            free(*ppsdev);
            *ppsdev = NULL;

            goto err;
        }
        ppsdev++;
    }


    ddkc_dbg("uart vfs driver init finish, ret:%d\r\n", ret);

    return 0;

err:
    ppsdev = g_uart_device_array;
    for (j = 0; j < i; j++) {
        // uninstall uart devices who are already registered
        if (*ppsdev) {
            aos_dev_unreg(*ppsdev);
            ddkc_info("free memory for UART%d\r\n", j);

            if ((*ppsdev)->user_data)
                free((*ppsdev)->user_data);
            free(*ppsdev);

            *ppsdev = NULL;
        }
        ppsdev++;
    }
    ddkc_err("uart vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

//VFS_DRIVER_ENTRY(vfs_uart_drv_init)
VFS_DRIVER_FULL_ENTRY(vfs_uart_drv_init, NULL)

#endif
