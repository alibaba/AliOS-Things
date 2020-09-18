#ifndef _PLATFORM_CONF_H_
#define _PLATFORM_CONF_H_

#define LITTLEFS_CNT 1
#define PROG_SIZE 1024
#define PAGE_NUMS_ON_BLOCK 4
#ifdef CFG_HW_ALI_MODULE
#define DATA_BLOCK_NUMS 520
#else
#define DATA_BLOCK_NUMS 1198
#endif

#endif /* _PLATFORM_CONF_H_ */
