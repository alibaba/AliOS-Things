/*
 * Copyright ( C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _IO_FLASH_H_
#define _IO_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup flash_device_api flash
 *  @ingroup driver_api
 * @{
 */

/* Flash设备IOCTL命令 */
#define IOC_FLASH_BASE 'F'
#define IOC_FLASH_ERASE_FLASH      (IOC_FLASH_BASE + 0x1)  /**< 擦除Flash */
#define IOC_FLASH_INFO_GET         (IOC_FLASH_BASE + 0x2)  /**< 获取Flash信息 */
#define IOC_FLASH_ENABLE_SECURE    (IOC_FLASH_BASE + 0x4)  /**< 使能Flash的安全区域 */
#define IOC_FLASH_DISABLE_SECURE   (IOC_FLASH_BASE + 0x8)  /**< 关闭Flash的安全区域 */

#define IOC_BOOT_BASE 'O'
/* OTA function ioctl ID */
#define IOC_FLASH_SET_BOOT_INFO     (IOC_FLASH_BASE + IOC_BOOT_BASE + 0x0)  /**< 设置Boot信息 */
#define IOC_FLASH_CLEAR_BOOT_COUNT  (IOC_FLASH_BASE + IOC_BOOT_BASE + 0x1)  /**< 清理Boot信息 */
#define IOC_FLASH_GET_BOOT_TYPE     (IOC_FLASH_BASE + IOC_BOOT_BASE + 0x2)  /**< 获取Boot类型 */

/**
 * 注册Flash设备驱动到VFS框架
 *
 * @return  0：成功，否则失败
 */
int vfs_flash_drv_init (void);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /*_IO_FLASH_H_*/
