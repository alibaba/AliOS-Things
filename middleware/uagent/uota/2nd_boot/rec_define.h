
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _REC_DEFINE_H_
#define _REC_DEFINE_H_
#include <stddef.h>
#include <stdint.h>
#include "typedef.h"
#include "aos/hal/flash.h"
#include "rec_hal.h"

#ifndef SPLISE_NUM
#define SPLISE_NUM   (1024*1024/0x10000)
#endif

/* 0 recovery start, 1 normal start, 2 upgrade to another partion, 3 first start after recovery */
#define REC_RECOVERY_START      0
#define REC_NORMAL_START        1
#define REC_UPGRADE_START       2   /* Just for esp8266 */
#define REC_ROLLBACK_START      3
#define REC_SWAP_UPDATE_START   4

#define REC_NORMAL_FLAG          0
#define REC_RECOVERY_FLAG        0x52455659   /* "REVY" */
#define REC_UPGRADE_FLAG         0x55504745   /* "UPGE" */
#define REC_ROLLBACK_FLAG        0x524F424B   /* "ROBK" */
#define REC_RECOVERY_VERIFY_FLAG 0x52455656   /* "REVV" */
#define REC_SWAP_UPDATE_FLAG     0x52535546   /* "RSUF" */
#define REC_DUAL_UPDATE_FLAG     0x52445546   /* "RDUF" */

#define REC_MAX_NUM         3
#define REC_WDT_TIMEOUT_MS  6000

#define OTA_RECOVERY_TYPE_DIRECT  0  /* Direct recovery, no backup rollback */
#define OTA_RECOVERY_TYPE_ABBACK  1  /* Dual partition backup, support for rollback, single boot address */
#define OTA_RECOVERY_TYPE_ABBOOT  2  /* Dual partition backup, support for rollback, dual boot address */

#ifndef OTA_RECOVERY_TYPE
#define OTA_RECOVERY_TYPE OTA_RECOVERY_TYPE_DIRECT
#endif

typedef enum {
    OTA_FLASH_STATUS_FREE = 0,
    OTA_FLASH_STATUS_USED = 1,
    OTA_FLASH_STATUS_REVY = 2,   /* Alreay recovered */
    OTA_FLASH_STATUS_SYNC = 3,   /* Already sync to app */
    OTA_FLASH_STATUS_NULL,
} REC_FLASH_STAT_E;

typedef enum {
    REC_PHASE_INIT    = 0,
    REC_PHASE_NBPATCH = 1,  /* Differential recovery */
    REC_PHASE_COPY    = 2,  /* APP copy to OTA */
    REC_PHASE_SWAP    = 3,  /* APP exchanges with OTA */
    REC_PHASE_SWITCH  = 4,  /* Switch start address, just for OTA_RECOVERY_TYPE_ABBOOT */
    REC_PHASE_DONE,         /* Differential recovery done */
} REC_STAT_PHASE_E;

/* This Structure used just for flash read，write flash need use structure PatchStatus */
typedef struct
{
    unsigned int flag; /* recovery flag */
    unsigned int num;  /* recovery Serial number */
} rec_flag_info_t;

/* The structure is stored in PARAM1 and cannot exceed 4k in length */
typedef struct
{
    unsigned int dst_adr;
    unsigned int src_adr;
    unsigned int len;
    unsigned short crc;
    unsigned int  upg_flag;
    unsigned char boot_count;
    unsigned int  rec_size;
    unsigned int  splict_size;
    unsigned short recovery_phase;
    unsigned int seekpos;
    unsigned int patched_size;
    unsigned int pending_size;
    unsigned short status;
    unsigned char diff;
    unsigned short num;
    unsigned short swap_addr;  /* Within the partition, offset from the starting address, divided by the fragment size */
    unsigned short swaped_idx; /* Within the slice, offset from the starting slice, divided by 4k */
    unsigned short swaped_state; /* 0 means APP data is still in the APP area, OTA data is in the OTA area;
                                    1 means APP data is in the backup area;
                                    2 means OTA data is copied to the APP area;
                                    3 means complete exchange */
    unsigned char patch_index; /* The number of times to enter patch mode. If a patch is completed,
                                  the value is 0, otherwise it will increase */
    unsigned char REC_FLASH_STAT_E[SPLISE_NUM];  /* Every 1 byte represents the status of a SLICE */
    unsigned short ota_addr[OTA_ADDR_NUM]; /* The address number of the OTA data, multiplied by SLICE for the partition offset */
    unsigned short patch_crc;
}__attribute__((packed)) PatchStatus;
#endif
