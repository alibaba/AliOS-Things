/*
 * Copyright ( C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _IO_FLASH_H_
#define _IO_FLASH_H_

#define IOC_FLASH_BASE 'F'
/* FLASH erase cmd */
#define IOC_FLASH_ERASE_FLASH  	   IOC_FLASH_BASE + 0x1
#define IOC_FLASH_INFO_GET  	   IOC_FLASH_BASE + 0x2
#define IOC_FLASH_ENABLE_SECURE    IOC_FLASH_BASE + 0x4
#define IOC_FLASH_DISABLE_SECURE   IOC_FLASH_BASE + 0x8

#define IOC_BOOT_BASE 'O'
/* OTA function ioctl ID */
#define IOC_FLASH_SET_BOOT_INFO     IOC_FLASH_BASE + IOC_BOOT_BASE + 0x0
#define IOC_FLASH_CLEAR_BOOT_COUNT  IOC_FLASH_BASE + IOC_BOOT_BASE + 0x1
#define IOC_FLASH_GET_BOOT_TYPE     IOC_FLASH_BASE + IOC_BOOT_BASE + 0x2

/*
typedef struct {
    uint32_t off_set;
    uint32_t size;
} ioc_flash_erase_args_t;
*/

int vfs_flash_drv_init (void);

#endif //_IO_FLASH_H_
