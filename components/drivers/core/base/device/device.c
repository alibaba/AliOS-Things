/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <assert.h>
#include <ulog/ulog.h>
#include "device/aos_device.h"
#include <drivers/u_ld.h>
#include <devicevfs/devicevfs.h>
#include <stdlib.h>

#include "k_api.h"

#define TAG "aos_device_bridge"

#define device_init     (dev->init)
#define device_open     (dev->open)
#define device_close    (dev->close)
#define device_read     (dev->read)
#define device_write    (dev->write)
#define device_control  (dev->control)

extern struct file_ops m_dev_fops;
extern void aos_object_detach(aos_object_t object);

#ifndef AOS_DEVICE_BRIDGE_USE_VFS

static int bridge_device_probe (struct u_platform_device *pdev) {
    LOGW(TAG, "%s", __func__);
    return 0;
}

static int bridge_device_remove (struct u_platform_device *pdev) {
    LOGW(TAG, "%s", __func__);
    return 0;
}

static void bridge_device_shutdown (struct u_platform_device *pdev) {
    LOGW(TAG, "%s", __func__);
    return;
}

static int bridge_device_suspend (struct u_platform_device *pdev, int /*pm_message_t */state) {
    LOGW(TAG, "%s", __func__);
    return 0;
}

static int bridge_device_resume (struct u_platform_device *pdev) {
    LOGW(TAG, "%s", __func__);
    return 0;
}

int bridge_device_init (struct u_platform_device *pdev) {
    return 0;
}

int bridge_device_deinit (struct u_platform_device *pdev) {
    return 0;
}

int bridge_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    return 0;
}

static struct subsys_drv bridge_device_drv = {
    .drv_name = "bridge_drv",
    .init = bridge_device_init,
    .deinit = bridge_device_deinit,
    .pm = bridge_device_pm,
#if 0
    .probe = bridge_device_probe,
    .remove = bridge_device_remove,
    .shutdown = bridge_device_shutdown,
    .suspend = bridge_device_suspend,
    .resume = bridge_device_resume,
#endif
};

static int aos_device_bridge_reg(const char *name, subsys_file_ops_t *fops, aos_device_t udata)
{
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev *ppsdev = NULL;

    LOGI(TAG, "vfs driver init starts");

    node_name_len = strlen(name) + 2 + 1; // 2 for %d, 1 for '\0'
    LOGI(TAG, "node_name_len:%d", node_name_len);

    ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);
    if (!ppsdev) {
        LOGI(TAG, "malloc failed, ppsdev:%p", ppsdev);
        goto err;
    }

    memset(ppsdev, 0x0, sizeof(struct subsys_dev)+node_name_len);

    ppsdev->node_name = (char *)((ppsdev) + 1);
    snprintf((ppsdev)->node_name, node_name_len, "%s", name);
    LOGD(TAG, "ppsdev:%p, (ppsdev) + 1:%p, node_name:%s, sizeof(struct subsys_dev):%d",
         ppsdev, (ppsdev)->node_name, (ppsdev) + 1, sizeof(struct subsys_dev));
    ppsdev->permission = 0;
    // please refer to definitions in enum SUBSYS_BUS_TYPE
    ppsdev->type = BUS_TYPE_PLATFORM;
    // user_data will be passed to open operation via node->i_arg
    ppsdev->user_data = (void *)udata;
    udata->user_data2 = ppsdev;

    ret = aos_dev_reg(ppsdev, fops, &bridge_device_drv);
    if (ret) {
        LOGE(TAG, "aos_dev_reg for failed, ret:%d", ret);
        goto err;
    }

    LOGI(TAG, "%s vfs driver init finish, ret:%d", __func__, ret);
    return 0;

err:
    // shall uninstall devices who are already registered
    aos_dev_unreg(ppsdev);

    if (ppsdev) {
        LOGI(TAG, "free memory");
        free(ppsdev);
        ppsdev = NULL;
    }

    LOGE(TAG, "%s vfs driver init failed, ret:%d", __func__, ret);

    return ret;
}
#endif

/**
 * This function registers a device driver with specified name.
 *
 * @param dev the pointer of device driver structure
 * @param name the device's name
 * @param flags the capabilities flag of device
 *
 * @return the error code, 0 on initialization successfully.
 */
int aos_device_register(aos_device_t dev,
                        const char *name,
                        uint16_t flags)
{
    if (dev == NULL)
        return -1;

    if (aos_device_find(name) != NULL)
        return -1;

    aos_object_init(&(dev->parent), AOS_Object_Class_Device, name);
    dev->flag = flags;
    dev->ref_count = 0;
    dev->open_flag = 0;

#ifdef AOS_DEVICE_BRIDGE_USE_VFS
    if (0 != aos_register_driver(name, &m_dev_fops, (void *)dev)) {
#else
    LOGD(TAG, "%s dev %p name %s", __func__, dev, name);
    if (0 != aos_device_bridge_reg(name, &m_dev_fops, dev)) {
#endif
        return -1;
    }

    return 0;
}

/**
 * This function removes a previously registered device driver
 *
 * @param dev the pointer of device driver structure
 *
 * @return the error code, 0 on successfully.
 */
int aos_device_unregister(aos_device_t dev)
{
    struct subsys_dev *ppsdev;

    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);
    DEV_ASSERT(aos_object_is_systemobject(&dev->parent));

    ppsdev = dev->user_data2;
    if (ppsdev) {
        aos_dev_unreg(ppsdev);
        free(ppsdev);
        ppsdev = NULL;
    }

    aos_object_detach(&(dev->parent));

    return 0;
}

/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or NULL on failure.
 */
aos_device_t aos_device_find(const char *name)
{
    struct aos_object *object;
    dlist_t *node;
    struct aos_object_information *information;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    /* try to find device object */
    information = aos_object_get_information(AOS_Object_Class_Device);
    DEV_ASSERT(information != NULL);
    for (node  = information->object_list.next;
         node != &(information->object_list);
         node  = node->next)
    {
        object = dlist_entry(node, struct aos_object, list);
        if (strncmp(object->name, name, AOS_OBJ_NAME_MAX) == 0)
        {
            /* leave critical */
            RHINO_CRITICAL_EXIT();

            return (aos_device_t)object;
        }
    }

    /* leave critical */
    RHINO_CRITICAL_EXIT();

    /* not found */
    return NULL;
}

/**
 * This function creates a device object with user data size.
 *
 * @param type, the kind type of this device object.
 * @param attach_size, the size of user data.
 *
 * @return the allocated device object, or NULL when failed.
 */
aos_device_t aos_device_create(int type, int attach_size)
{
    int size;
    aos_device_t device;

    size = AOS_ALIGN(sizeof(struct aos_device), AOS_ALIGN_SIZE);
    attach_size = AOS_ALIGN(attach_size, AOS_ALIGN_SIZE);
    /* use the total size */
    size += attach_size;

    device = (aos_device_t)aos_malloc(size);
    if (device)
    {
        memset(device, 0x0, sizeof(struct aos_device));
        device->type = (enum aos_device_class_type)type;
    }

    return device;
}

/**
 * This function destroy the specific device object.
 *
 * @param dev, the specific device object.
 */
void aos_device_destroy(aos_device_t dev)
{
    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);
    DEV_ASSERT(aos_object_is_systemobject(&dev->parent) == false);

    aos_object_detach(&(dev->parent));

    /* release this device object */
    aos_free(dev);
}

/**
 * This function will initialize the specified device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
int aos_device_init(aos_device_t dev)
{
    int result = 0;

    DEV_ASSERT(dev != NULL);

    /* get device_init handler */
    if (device_init != NULL)
    {
        if (!(dev->flag & AOS_DEVICE_FLAG_ACTIVATED))
        {
            result = device_init(dev);
            if (result != 0)
            {
                LOGD(TAG,"To initialize device:%s failed. The error code is %d",
                        dev->parent.name, result);
            }
            else
            {
                dev->flag |= AOS_DEVICE_FLAG_ACTIVATED;
            }
        }
    }

    return result;
}

/**
 * This function will open a device
 *
 * @param dev the pointer of device driver structure
 * @param oflag the flags for device open
 *
 * @return the result
 */
int aos_device_open(aos_device_t dev, uint16_t oflag)
{
    int result = 0;

    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);

    /* if device is not initialized, initialize it. */
    if (!(dev->flag & AOS_DEVICE_FLAG_ACTIVATED))
    {
        if (device_init != NULL)
        {
            result = device_init(dev);
            if (result != 0)
            {
                //printf("To initialize device:%s failed. The error code is %d\n",dev->parent.name, result);
                return result;
            }
        }

        dev->flag |= AOS_DEVICE_FLAG_ACTIVATED;
    }

    /* device is a stand alone device and opened */
    if ((dev->flag & AOS_DEVICE_FLAG_STANDALONE) &&
        (dev->open_flag & AOS_DEVICE_OFLAG_OPEN))
    {
        return -1; // EBUSY
    }

    /* call device_open interface */
    if (device_open != NULL)
    {
        result = device_open(dev, oflag);
    } else {
        /* set open flag */
        dev->open_flag = (oflag & AOS_DEVICE_OFLAG_MASK);
    }

    /* set open flag */
    if (result == 0) {
        dev->open_flag |= AOS_DEVICE_OFLAG_OPEN;

        dev->ref_count++;
        /* don't let bad things happen silently. If you are bitten by this assert,
         * please set the ref_count to a bigger type. */
        DEV_ASSERT(dev->ref_count != 0);
    }

    return result;
}

/**
 * This function will close a device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
int aos_device_close(aos_device_t dev)
{
    int result = 0;

    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);

    if (dev->ref_count == 0)
        return -1;

    dev->ref_count--;

    if (dev->ref_count != 0)
        return 0;

    /* call device_close interface */
    if (device_close != NULL) {
        result = device_close(dev);
    }

    /* set open flag */
    if (result == 0)
        dev->open_flag = AOS_DEVICE_OFLAG_CLOSE;

    return result;
}

/**
 * This function will read some data from a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of reading
 * @param buffer the data buffer to save read data
 * @param size the size of buffer
 *
 * @return the actually read size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
size_t aos_device_read(aos_device_t dev,
                      long    pos,
                      void       *buffer,
                      size_t   size)
{
    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);

    if (dev->ref_count == 0) {
        return 0;
    }

    /* call device_read interface */
    if (device_read != NULL)
    {
        return device_read(dev, pos, buffer, size);
    }

    return 0;
}

/**
 * This function will write some data to a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of written
 * @param buffer the data buffer to be written to device
 * @param size the size of buffer
 *
 * @return the actually written size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
size_t aos_device_write(aos_device_t dev,
                        long    pos,
                        const void *buffer,
                        size_t   size)
{
    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);

    if (dev->ref_count == 0) {
        /* set error code -ENOSYS */
        return 0;
    }

    /* call device_write interface */
    if (device_write != NULL) {
        return device_write(dev, pos, buffer, size);
    }

    /* set error code -ENOSYS */

    return 0;
}

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param dev the pointer of device driver structure
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
int aos_device_control(aos_device_t dev, int cmd, void *arg)
{
    LOGD(TAG, "%s entry, cmd %d, dev %p", __func__, cmd, dev);
    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);

    /* call device_write interface */
    if (device_control != NULL)
    {
        return device_control(dev, cmd, arg);
    } else {
        LOGD(TAG, "%s dev %p device_control is NULL\r\n", __func__, dev);
    }

    return -1;
}

/**
 * This function will set the reception indication callback function. This callback function
 * is invoked when this device receives data.
 *
 * @param dev the pointer of device driver structure
 * @param rx_ind the indication callback function
 *
 * @return 0
 */
int aos_device_set_rx_indicate(aos_device_t dev,
                          int (*rx_ind)(aos_device_t dev, size_t size))
{
    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);

    dev->rx_indicate = rx_ind;

    return 0;
}


/**
 * This function will set the indication callback function when device has
 * written data to physical hardware.
 *
 * @param dev the pointer of device driver structure
 * @param tx_done the indication callback function
 *
 * @return 0
 */
int aos_device_set_tx_complete(aos_device_t dev,
                           int (*tx_done)(aos_device_t dev, void *buffer))
{
    DEV_ASSERT(dev != NULL);
    DEV_ASSERT(aos_object_get_type(&dev->parent) == AOS_Object_Class_Device);

    dev->tx_complete = tx_done;

    return 0;
}
