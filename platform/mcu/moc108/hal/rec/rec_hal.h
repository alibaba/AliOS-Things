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
#define SPLISE_NUM   (1024*1024/SPLICT_SIZE)          // OTA 分区1M
#define OTA_ADDR_NUM (256*1024/SPLICT_SIZE)           // OTA 内容 256k

#define DIFF_OFFSET 0x4E00

#define DIFF_CONF_OFFSET   0x00
#define DIFF_BACKUP_OFFSET 0x40000
#define DIFF_CONF2_OFFSET  0x51000

typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
    //below data used to recovery
    uint32_t patch_size;
    uint32_t seekpos;
    uint32_t patched_size;
    uint32_t pending_size;
    uint32_t splict_size;
    uint16_t status;
    uint16_t num;
    uint8_t diff_version;//39 bytes   
	uint16_t patch_crc;
}__attribute__((packed)) PatchStatus;

void rec_upgrade_reboot();
unsigned int rec_get_boot_addr();

#endif