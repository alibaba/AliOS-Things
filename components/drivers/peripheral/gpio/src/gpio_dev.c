/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <poll.h>

#include <drivers/u_ld.h>
#include <aos/hal/gpio.h>
#include <vfsdev/gpio_dev.h>
#include <devicevfs/devicevfs.h>


#ifdef CONFIG_GPIO_NUM
#define PLATFORM_GPIO_NUM CONFIG_GPIO_NUM
#else
#define PLATFORM_GPIO_NUM 40
#endif

#if (PLATFORM_GPIO_NUM > 0)
// UART device node will be named with "/dev/ttyUART<x>", where <x> is uart port id
#define GPIO_DEV_NAME_FORMAT "gpio"

static gpio_config_t gpio_hal_configs[PLATFORM_GPIO_NUM];

static void prepare_gpio_hal(gpio_dev_t *gpio_dev)
{
    if (gpio_dev->config == gpio_hal_configs[gpio_dev->port])
        return;

    if (gpio_hal_configs[gpio_dev->port] != -1)
        (void)hal_gpio_finalize(gpio_dev);

    (void)hal_gpio_init(gpio_dev);
    gpio_hal_configs[gpio_dev->port] = gpio_dev->config;
}

ssize_t gpio_device_read (file_t *f, void *buffer, size_t size) {

    ddkc_warn("%s not implemented\r\n", __func__);

    return -1;
}

ssize_t gpio_device_write (file_t *f, const void *buffer, size_t size) {
    ddkc_warn("%s not implemented\r\n", __func__);

    return -1;
}

int gpio_io_set (gpio_io_config_t *config) {
    int ret = 0;
    gpio_dev_t gpio_dev;

    ddkc_dbg("%s\r\n", __func__);

    if (!config || config->id >= PLATFORM_GPIO_NUM) {
        ddkc_err("invalid param: config:%p, id:%d\r\n", config, !config ? 0 : config->id);
        return -EINVAL;
    }

    memset(&gpio_dev, 0, sizeof(gpio_dev_t));

    gpio_dev.port = config->id;

    if (config->config & GPIO_IO_OUTPUT_MASK) {
        /* check if */
        if ((config->config & GPIO_IO_OUTPUT_MASK) == GPIO_IO_OUTPUT_TOGGLE) {
            ret = hal_gpio_output_toggle(&gpio_dev);
            ddkc_dbg("GPIO:%d, output toggle done, ret:%d", gpio_dev.port, ret);
            return 0;
        }

        switch (config->config & GPIO_IO_OUTPUT_MASK) {
            case GPIO_IO_OUTPUT_PP:
                gpio_dev.config = OUTPUT_PUSH_PULL;
                break;
            case GPIO_IO_OUTPUT_ODNP:
                gpio_dev.config = OUTPUT_OPEN_DRAIN_NO_PULL;
                break;
            case GPIO_IO_OUTPUT_ODPU:
                gpio_dev.config = OUTPUT_OPEN_DRAIN_PULL_UP;
                break;
            default:
                ddkc_dbg("invalid config:0x%x, set to ODNP\r\n", config->config);
                gpio_dev.config = OUTPUT_OPEN_DRAIN_NO_PULL;
                break;
        }

        prepare_gpio_hal(&gpio_dev);

        if (config->data) {
            ret = hal_gpio_output_high(&gpio_dev);
        } else
            ret = hal_gpio_output_low(&gpio_dev);

        ddkc_dbg("GPIO:%d, output config:0x%x, ret:%d", gpio_dev.port, gpio_dev.config, ret);

        return ret;
    }

    ddkc_warn("!!!WARNING!!! THIS SHOULD NEVER HAPPEN, config:0x%x\r\n", config->config);

    return ret;
}

int gpio_io_get (gpio_io_config_t *config) {
    int ret = 0;
    gpio_dev_t gpio_dev;

    ddkc_dbg("%s\r\n", __func__);

    if (!config || config->id >= PLATFORM_GPIO_NUM) {
        ddkc_err("invalid param: config:%p, id:%d\r\n", config, !config ? 0 : config->id);
        return -EINVAL;
    }

    memset(&gpio_dev, 0, sizeof(gpio_dev_t));

    gpio_dev.port = config->id;

    if (config->config & GPIO_IO_INPUT_MASK) {
        unsigned int value = 0;
        switch (config->config & GPIO_IO_INPUT_MASK) {
            case GPIO_IO_INPUT_HI:
                gpio_dev.config = INPUT_HIGH_IMPEDANCE;
                break;
            case GPIO_IO_INPUT_PU:
                gpio_dev.config = INPUT_PULL_UP;
                break;
            case GPIO_IO_INPUT_PD:
                gpio_dev.config = INPUT_PULL_DOWN;
                break;
            default:
                gpio_dev.config = INPUT_HIGH_IMPEDANCE;
                break;
        }
        prepare_gpio_hal(&gpio_dev);
        ret = hal_gpio_input_get(&gpio_dev, &value);
        ddkc_dbg("GPIO:%d, input config:0x%x, ret:%d\r\n", gpio_dev.port, gpio_dev.config, ret);

        return !ret ? value: ret;
    }

    ddkc_warn("!!!WARNING!!! THIS SHOULD NEVER HAPPEN, config:0x%x\r\n", config->config);

    return ret;
}

static int gpio_irq_set (gpio_irq_config_t *config) {
    int ret = 0;
    gpio_dev_t gpio_dev;
    gpio_irq_trigger_t irq_type;

    ddkc_dbg("%s\r\n", __func__);

    if (!config || config->id >= PLATFORM_GPIO_NUM) {
        ddkc_err("invalid param: config:%p, id:%d\r\n", config, !config ? 0 : config->id);
        return -EINVAL;
    }

    memset(&gpio_dev, 0, sizeof(gpio_dev_t));
    gpio_dev.port = config->id;
    gpio_dev.config = IRQ_MODE;

    prepare_gpio_hal(&gpio_dev);

    if (config->config & GPIO_IRQ_ENABLE) {

        switch (config->config & GPIO_IRQ_MODE_MASK) {
            case GPIO_IRQ_LEVEL_LOW:
                ddkc_warn("not implemented yet\r\n");
                return -EINVAL;
            case GPIO_IRQ_LEVEL_HIGH:
                ddkc_warn("not implemented yet\r\n");
                return -EINVAL;
            case GPIO_IRQ_EDGE_FALLING:
                irq_type = IRQ_TRIGGER_FALLING_EDGE;
                break;
            case GPIO_IRQ_EDGE_RISING:
                irq_type = IRQ_TRIGGER_RISING_EDGE;
                break;
            case GPIO_IRQ_EDGE_BOTH:
                irq_type = IRQ_TRIGGER_BOTH_EDGES;
                break;
            default:
                ddkc_warn("ERROR - invalid irq_type:0x%x\r\n", config->config & GPIO_IRQ_MODE_MASK);
                return -EINVAL;
        }
        ret = hal_gpio_enable_irq(&gpio_dev, irq_type, (gpio_irq_handler_t)config->cb, config->arg);

    } else if (config->config & GPIO_IRQ_DISABLE) {
        ret = hal_gpio_disable_irq(&gpio_dev);
    } else if (config->config & GPIO_IRQ_CLEAR) {
        ret = hal_gpio_clear_irq(&gpio_dev);
    }

    return ret;
}



int gpio_device_ioctl (file_t *f, int cmd, unsigned long arg) {
    int ret = 0;
    gpio_io_config_t config;
    gpio_irq_config_t irq_config;
    ddkc_dbg("i_name:%s, cmd:%d, arg:0x%lx\r\n", f->node->i_name, cmd, arg);

    switch (cmd) {
        case IOC_GPIO_SET:
            aos_ipc_copy(&config, (void *)arg, sizeof(gpio_io_config_t));
            ret = gpio_io_set(&config);
            break;
        case IOC_GPIO_GET:
            aos_ipc_copy(&config, (void *)arg, sizeof(gpio_io_config_t));
            ret = gpio_io_get(&config);
            aos_ipc_copy((void *)arg, &config, sizeof(gpio_io_config_t));
            break;
        case IOC_GPIO_SET_IRQ:
            aos_ipc_copy(&irq_config, (void *)arg, sizeof(gpio_irq_config_t));
            ret = gpio_irq_set(&irq_config);
            break;

        default:
            ddkc_warn("invalid command:%d\r\n", cmd);
            break;
    }

    return ret;
}

int gpio_device_open (inode_t *node, file_t *f) {
    int ret = 0;

    return ret;
}

int gpio_device_close (file_t *f) {
    int ret = 0;

    return ret;
}

/************************** device ****************************/


subsys_file_ops_t gpio_device_fops = {
    .open = gpio_device_open,
    .close = gpio_device_close,
    .read = gpio_device_read,
    .write = gpio_device_write,
    .ioctl = gpio_device_ioctl,
    .poll = NULL,
    .lseek = NULL,
};

int gpio_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int gpio_device_deinit (struct u_platform_device *pdev) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

int gpio_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_dbg("%s\r\n", __func__);
    return 0;
}

struct subsys_drv gpio_device_drv = {
    .drv_name = "gpio",
    .init = gpio_device_init,
    .deinit = gpio_device_deinit,
    .pm = gpio_device_pm,
};

struct subsys_dev g_gpio_device = {
    .user_data = NULL,
    .type = BUS_TYPE_PLATFORM,
    .permission = 0,
    .node_name = "gpio"
};

int vfs_gpio_drv_init (void) {
    int ret = 0;
    int i;

    for (i = 0; i < PLATFORM_GPIO_NUM; i++)
        gpio_hal_configs[i] = -1;

    ddkc_dbg("gpio vfs driver init starts\r\n");
    ret = aos_dev_reg(&g_gpio_device, &gpio_device_fops, &gpio_device_drv);
    if (ret) {
        ddkc_err("aos_dev_reg for uart devices failed, ret:%d\r\n", ret);
        goto err;
    }

    ddkc_dbg("gpio vfs driver init finish, ret:%d\r\n", ret);

    return 0;

err:

    ddkc_warn("gpio vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}

VFS_DRIVER_ENTRY(vfs_gpio_drv_init)

#endif
