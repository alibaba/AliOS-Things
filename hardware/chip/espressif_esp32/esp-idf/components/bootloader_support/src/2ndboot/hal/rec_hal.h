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
#define OTA_ADDR_NUM (512*1024/SPLICT_SIZE)           // OTA 内容 256k

#define DIFF_CONF_OFFSET   0x00

#endif
