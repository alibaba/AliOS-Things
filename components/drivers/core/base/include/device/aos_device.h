/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_DEVICE_H
#define _AOS_DEVICE_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "object.h"

#include "aos/kernel.h"
#include "aos/vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define AOS_USING_DEVICE_OPS

#define DEV_ASSERT(test)
#define AOS_DEVICE(device)            ((aos_device_t)device)

/**
 * device flags defitions
 */
#define AOS_DEVICE_FLAG_DEACTIVATE       0x000           /**< device is not not initialized */

#define AOS_DEVICE_FLAG_RDONLY           0x001           /**< read only */
#define AOS_DEVICE_FLAG_WRONLY           0x002           /**< write only */
#define AOS_DEVICE_FLAG_RDWR             0x003           /**< read and write */

#define AOS_DEVICE_FLAG_REMOVABLE        0x004           /**< removable device */
#define AOS_DEVICE_FLAG_STANDALONE       0x008           /**< standalone device */
#define AOS_DEVICE_FLAG_ACTIVATED        0x010           /**< device is activated */
#define AOS_DEVICE_FLAG_SUSPENDED        0x020           /**< device is suspended */
#define AOS_DEVICE_FLAG_STREAM           0x040           /**< stream mode */

#define AOS_DEVICE_FLAG_INT_RX           0x100           /**< INT mode on Rx */
#define AOS_DEVICE_FLAG_DMA_RX           0x200           /**< DMA mode on Rx */
#define AOS_DEVICE_FLAG_INT_TX           0x400           /**< INT mode on Tx */
#define AOS_DEVICE_FLAG_DMA_TX           0x800           /**< DMA mode on Tx */

#define AOS_DEVICE_OFLAG_CLOSE           0x000           /**< device is closed */
#define AOS_DEVICE_OFLAG_RDONLY          0x001           /**< read only access */
#define AOS_DEVICE_OFLAG_WRONLY          0x002           /**< write only access */
#define AOS_DEVICE_OFLAG_RDWR            0x003           /**< read and write */
#define AOS_DEVICE_OFLAG_OPEN            0x008           /**< device is opened */
#define AOS_DEVICE_OFLAG_MASK            0xf0f           /**< mask of open flag */

#define AOS_DEVICE_CTRL_CHAR_STREAM           0x10            /**< stream mode on char device */
#define AOS_DEVICE_CTRL_BLK_GETGEOME          0x10            /**< get geometry information   */
#define AOS_DEVICE_CTRL_BLK_SYNC              0x11            /**< flush data to block device */
#define AOS_DEVICE_CTRL_BLK_ERASE             0x12            /**< erase block on block device */
#define AOS_DEVICE_CTRL_BLK_AUTOREFRESH       0x13            /**< block device : enter/exit auto refresh mode */
#define AOS_DEVICE_CTRL_NETIF_GETMAC          0x10            /**< get mac address */
#define AOS_DEVICE_CTRL_MTD_FORMAT            0x10            /**< format a MTD device */
#define AOS_DEVICE_CTRL_RTC_GET_TIME          0x10            /**< get time */
#define AOS_DEVICE_CTRL_RTC_SET_TIME          0x11            /**< set time */
#define AOS_DEVICE_CTRL_RTC_GET_ALARM         0x12            /**< get alarm */
#define AOS_DEVICE_CTRL_RTC_SET_ALARM         0x13            /**< set alarm */

enum aos_device_class_type
{
    AOS_Device_Class_Char = 0,                           /**< character device */
    AOS_Device_Class_Block,                              /**< block device */
    AOS_Device_Class_NetIf,                              /**< net interface */
    AOS_Device_Class_MTD,                                /**< memory device */
    AOS_Device_Class_CAN,                                /**< CAN device */
    AOS_Device_Class_RTC,                                /**< RTC device */
    AOS_Device_Class_Sound,                              /**< Sound device */
    AOS_Device_Class_Graphic,                            /**< Graphic device */
    AOS_Device_Class_I2CBUS,                             /**< I2C bus device */
    AOS_Device_Class_USBDevice,                          /**< USB slave device */
    AOS_Device_Class_USBHost,                            /**< USB host bus */
    AOS_Device_Class_SPIBUS,                             /**< SPI bus device */
    AOS_Device_Class_SPIDevice,                          /**< SPI device */
    AOS_Device_Class_SDIO,                               /**< SDIO bus device */
    AOS_Device_Class_PM,                                 /**< PM pseudo device */
    AOS_Device_Class_Pipe,                               /**< Pipe device */
    AOS_Device_Class_Portal,                             /**< Portal device */
    AOS_Device_Class_Timer,                              /**< Timer device */
    AOS_Device_Class_Miscellaneous,                      /**< Miscellaneous device */
    AOS_Device_Class_Sensor,                             /**< Sensor device */
    AOS_Device_Class_Touch,                              /**< Touch device */
    AOS_Device_Class_Unknown                             /**< unknown device */
};

typedef struct aos_device * aos_device_t;

/**
 * Device structure
 */
struct aos_device
{
    struct aos_object          parent;                   /**< inherit from aos_object */

    enum aos_device_class_type type;                     /**< device type */
    uint16_t                   flag;                     /**< device flag */
    uint16_t                   open_flag;                /**< device open flag */

    uint8_t                ref_count;                /**< reference count */
    uint8_t                device_id;                /**< 0 - 255 */

    /* device call back */
    int (*rx_indicate)(aos_device_t dev, size_t size);
    int (*tx_complete)(aos_device_t dev, void *buffer);

#ifdef AOS_USING_DEVICE_OPS
    const struct aos_device_ops *ops;
#else
    /* common device interface */
    int  (*init)     (aos_device_t dev);
    int  (*open)     (aos_device_t dev, uint16_t oflag);
    int  (*close)    (aos_device_t dev);
    size_t (*read)   (aos_device_t dev, long pos, void *buffer, size_t size);
    size_t (*write)  (aos_device_t dev, long pos, const void *buffer, size_t size);
    int    (*control)(aos_device_t dev, int cmd, void *args);
#endif

#if defined(AOS_USING_POSIX)
    const struct file_ops *fops;
#endif

    void                     *user_data;                /**< device private data */
    void                     *user_data2;                /**< device private data2 */

    void                     *wait_queue;
};

#ifdef AOS_USING_DEVICE_OPS
struct aos_device_ops
{
    /* common device interface */
    long  (*init)   (aos_device_t dev);
    long  (*open)   (aos_device_t dev, uint16_t oflag);
    long  (*close)  (aos_device_t dev);
    size_t (*read)   (aos_device_t dev, off_t pos, void *buffer, size_t size);
    size_t (*write)  (aos_device_t dev, off_t pos, const void *buffer, size_t size);
    long  (*control)(aos_device_t dev, int cmd, void *args);
};
#endif

/**
 * This function registers a device driver with specified name.
 *
 * @param dev the pointer of device driver structure
 * @param name the device driver's name
 * @param flags the capabilities flag of device
 *
 * @return the error code, 0 on initialization successfully.
 */
int aos_device_register(aos_device_t dev,
                        const char *name,
                        uint16_t flags);

/**
 * This function removes a previously registered device driver
 *
 * @param dev the pointer of device driver structure
 *
 * @return the error code, 0 on successfully.
 */
int aos_device_unregister(aos_device_t dev);

/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or NULL on failure.
 */
aos_device_t aos_device_find(const char *name);

/**
 * This function creates a device object with user data size.
 *
 * @param type, the kind type of this device object.
 * @param attach_size, the size of user data.
 *
 * @return the allocated device object, or NULL when failed.
 */
aos_device_t aos_device_create(int type, int attach_size);

/**
 * This function destroy the specific device object.
 *
 * @param dev, the specific device object.
 */
void aos_device_destroy(aos_device_t dev);

/**
 * This function will initialize the specified device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
int aos_device_init(aos_device_t dev);

/**
 * This function will open a device
 *
 * @param dev the pointer of device driver structure
 * @param oflag the flags for device open
 *
 * @return the result
 */
int aos_device_open(aos_device_t dev, uint16_t oflag);

/**
 * This function will close a device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
int aos_device_close(aos_device_t dev);

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
                       size_t   size);

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
                       size_t   size);

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param dev the pointer of device driver structure
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
int aos_device_control(aos_device_t dev, int cmd, void *arg);


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
                               int (*rx_ind)(aos_device_t dev, size_t size));

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
                               int (*tx_done)(aos_device_t dev, void *buffer));


#ifdef __cplusplus
}
#endif


#endif
