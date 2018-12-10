/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * 该头文件用于为recovery屏蔽平台差异
 */

#ifndef _REC_HAL_H_
#define _REC_HAL_H_

#define FLASH_SECTOR_SIZE   0x1000
#define SECTOR_SIZE         0x1000

#define SPLICT_SIZE  0x10000
#define SPLISE_NUM   (512*1024/SPLICT_SIZE)
#define OTA_ADDR_NUM (512*1024/SPLICT_SIZE)

extern unsigned int rec_xz_backup_flash_addr;
extern unsigned int rec_conf_backup_flash_addr;
#define DIFF_CONF_OFFSET   0x00
#define DIFF_BACKUP_OFFSET rec_xz_backup_flash_addr
#define DIFF_CONF2_OFFSET  rec_conf_backup_flash_addr

void rec_upgrade_reboot();
unsigned int rec_get_boot_addr();
#endif
