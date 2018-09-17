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

#define SPLICT_SIZE  0x4000
#define SPLISE_NUM   (1024*1024/SPLICT_SIZE)          // OTA 分区1M
#define OTA_ADDR_NUM (512*1024/SPLICT_SIZE)           // OTA 内容 256k

#define DIFF_OFFSET 0x4E00

#define IRAM_HEAP_BASE (0x40108000)

extern unsigned int rec_xz_backup_flash_addr;
extern unsigned int rec_conf_backup_flash_addr;
#define DIFF_CONF_OFFSET   0x00
#define DIFF_BACKUP_OFFSET rec_xz_backup_flash_addr
#define DIFF_CONF2_OFFSET  rec_conf_backup_flash_addr

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
    uint32_t patch_index; // 进入patch模式的次数。如果一次patch完成，该值为0，否则会增加
    uint8_t ota_flash_status[SPLISE_NUM];  // 每1byte代表64k的状态，0表示空闲，1表示占用，2表示已拷贝数据，3表示已从主区同步 TODO: 个数需要改成宏定义
    uint16_t ota_addr[OTA_ADDR_NUM]; //OTA数据的地址序号，乘以64k后为分区偏移，最多256k，即最多4个64k。TODO: 后续需要改为宏，适配其它的分片大小或ota大小
	uint16_t patch_crc;
}__attribute__((packed)) PatchStatus;

void rec_upgrade_reboot();
unsigned int rec_get_boot_addr();

int aos_watchdog_init();
void aos_soft_wdt_fead();
void aos_wdt_disable();

#endif
