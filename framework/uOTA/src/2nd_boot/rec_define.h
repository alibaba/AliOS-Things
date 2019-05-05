
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _REC_DEFINE_H_
#define _REC_DEFINE_H_
#include <stddef.h>
#include <stdint.h>
#include "typedef.h"
#include "hal/soc/flash.h"
#include "rec_hal.h"

#ifndef OTA_MAX_VER_LEN
#define OTA_MAX_VER_LEN (64)
#endif

#ifndef OTA_MAX_VER_LEN
#define SPLISE_NUM   (1024*1024/0x10000)
#endif


//0 recovery start, 1 normal start, 2 upgrade to another partion, 3 first start after recovery
#define REC_RECOVERY_START      0
#define REC_NORMAL_START        1
#define REC_UPGRADE_START       2   // just for esp8266
#define REC_ROLLBACK_START      3
#define REC_SWAP_UPDATE_START   4

#define REC_NORMAL_FLAG          0
#define REC_RECOVERY_FLAG        0x52455659   //"REVY"
#define REC_UPGRADE_FLAG         0x55504745   //"UPGE"
#define REC_ROLLBACK_FLAG        0x524F424B   //"ROBK"
#define REC_RECOVERY_VERIFY_FLAG 0x52455656   //"REVV"
#define REC_SWAP_UPDATE_FLAG     0x52535546   //"RSUF"
#define REC_DUAL_UPDATE_FLAG     0x52445546   //"RDUF"

#define REC_MAX_NUM         3
#define REC_WDT_TIMEOUT_MS  6000

#define OTA_RECOVERY_TYPE_DIRECT  0  //直接恢复，无备份回滚
#define OTA_RECOVERY_TYPE_ABBACK  1  //双分区备份，支持回滚，单启动地址
#define OTA_RECOVERY_TYPE_ABBOOT  2  //双分区备份，支持回滚，双启动地址

#ifndef OTA_RECOVERY_TYPE
#define OTA_RECOVERY_TYPE OTA_RECOVERY_TYPE_DIRECT
#endif

typedef enum {
    OTA_FLASH_STATUS_FREE = 0,   // 空闲
    OTA_FLASH_STATUS_USED = 1,   // 已占用
    OTA_FLASH_STATUS_REVY = 2,   // 已恢复出来
    OTA_FLASH_STATUS_SYNC = 3,   // 已与主区同步
    OTA_FLASH_STATUS_NULL,       // 无效
} REC_FLASH_STAT_E;

typedef enum {
    REC_PHASE_INIT    = 0,  // 初始化
    REC_PHASE_NBPATCH = 1,  // 差分恢复中
    REC_PHASE_COPY    = 2,  // APP拷贝到OTA
    REC_PHASE_SWAP    = 3,  // APP与OTA交换
    REC_PHASE_SWITCH  = 4,  // 切换启动地址，仅针对OTA_RECOVERY_TYPE_ABBOOT
    REC_PHASE_DONE,         // 完成
} REC_STAT_PHASE_E;

//该结构体仅用于读flash时使用，写flash时需要使用结构PatchStatus
typedef struct
{
    unsigned int flag; // recovery标志
    unsigned int num;  // recovery序号
} rec_flag_info_t;

//该结构体存储与PARAM1，长度不能超过4k
typedef struct
{
    unsigned int dst_adr;
    unsigned int src_adr;
    unsigned int len;
    unsigned short crc;
    unsigned int  upg_flag;
    unsigned char boot_count;
    unsigned int  splict_size;
    unsigned short recovery_phase;
    unsigned int seekpos;
    unsigned int patched_size;
    unsigned int pending_size;
    unsigned short status;
    unsigned char diff;
    unsigned short num;
    unsigned short swap_addr;  // 分区内，相对于起始地址的偏移，再除以分片size
    unsigned short swaped_idx; // 分片片内，相对于起始分片的偏移，再除以4k
    unsigned short swaped_state; //0表示APP数据还在APP区，OTA数据在OTA区；1表示APP数据在备份区；2表示OTA数据拷贝到APP区；3表示完成交换
    unsigned char patch_index; // 进入patch模式的次数。如果一次patch完成，该值为0，否则会增加
    unsigned char REC_FLASH_STAT_E[SPLISE_NUM];  // 每1byte代表64k的状态，REC_FLASH_STAT_E
    unsigned short ota_addr[OTA_ADDR_NUM]; //OTA数据的地址序号，乘以64k后为分区偏移，最多256k，即最多4个64k。
    char ota_version[OTA_MAX_VER_LEN];
    char app_version[OTA_MAX_VER_LEN];
    unsigned short patch_crc;
}__attribute__((packed)) PatchStatus;
#endif
