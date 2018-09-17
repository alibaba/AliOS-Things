/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "k_api.h"
#include "rec_pub.h"

#define OTA_RECOVERY_FLAG   0xFFFFFFFF
#define OTA_UPGRADE_FLAG    0xAAAAAAAA

//0 recovery start, 1 normal start, 2 upgrade to another partion
#define RECOVERY_START      0
#define NORMAL_START        1
#define UPGRADE_START       2
#define REC_MAX_NUM         3

extern int nbpatch_main();

#if (defined IS_ESP8266)
unsigned int *reg_flash_flags;
#else
extern int reg_flash_flags[512];
#endif

#if (defined IS_ESP8266)
extern void vPortETSIntrLock(void);
extern void vPortETSIntrUnlock(void);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////
void recovery_get_flag_info(REC_FLAG_INFO_STRU *rec_flag_info)
{
    rec_flash_read_data((unsigned char *)rec_flag_info, rec_flash_addr2ofst((unsigned long)&reg_flash_flags[0]), sizeof(REC_FLAG_INFO_STRU));
}

void recovery_set_flag_info(REC_FLAG_INFO_STRU *rec_flag_info)
{
    uint32_t reg_flag_addr = (unsigned long)&reg_flash_flags[0];

    rec_flash_erase(rec_flash_addr2ofst(reg_flag_addr));
    rec_flash_write_data((unsigned char *)rec_flag_info, rec_flash_addr2ofst(reg_flag_addr), sizeof(REC_FLAG_INFO_STRU));
}

/* return value: 0 recovery start, 1 normal start, 2 upgrade to another partion  */
#if (defined IS_ESP8266)
int recovery_check()
{
    REC_FLAG_INFO_STRU rec_flag_info = {0, 0};
    recovery_get_flag_info(&rec_flag_info);

    if(rec_flag_info.flag == OTA_RECOVERY_FLAG)
    {
        if(rec_flag_info.num <= REC_MAX_NUM)
        {
            rec_flag_info.num ++; //将recovery次数加1
            recovery_set_flag_info(&rec_flag_info);
            return RECOVERY_START;
        }
        else
        {
            rec_flag_info.flag = 0;
            rec_flag_info.num  = 0;
            recovery_set_flag_info(&rec_flag_info);

            return NORMAL_START;
        }
    } else if (rec_flag_info.flag == OTA_UPGRADE_FLAG)
    {
        rec_flag_info.flag = 0;
        rec_flag_info.num  = 0;
        recovery_set_flag_info(&rec_flag_info);

        return UPGRADE_START;
    }

    return NORMAL_START;
}
#else
int recovery_check()
{
    REC_FLAG_INFO_STRU rec_flag_info = {0, 0};

    recovery_get_flag_info((REC_FLAG_INFO_STRU *)&rec_flag_info);

    return (rec_flag_info.flag == OTA_RECOVERY_FLAG) ? RECOVERY_START : NORMAL_START;
}
#endif

void recovery_error(void *errinfo)
{
    rec_err_print(errinfo);
    rec_reboot();
}

void rec_success()
{
    REC_FLAG_INFO_STRU rec_flag_info = {0, 0};

#if (defined IS_ESP8266)
    rec_flag_info.flag = OTA_UPGRADE_FLAG;
#else
    rec_flag_info.flag = 0;
#endif
    recovery_set_flag_info(&rec_flag_info);

    rec_delayms(500);

    rec_reboot();
}

void rec_start()
{
    REC_FLAG_INFO_STRU rec_flag_info = {0, 0};

    rec_flag_info.flag = OTA_RECOVERY_FLAG;
    recovery_set_flag_info(&rec_flag_info);

    rec_delayms(500);
    LOG("reboot ...\r\n");
    rec_reboot();
}

void recovery_process()
{
	int ret;
	/* recovery start */
   ret = nbpatch_main();
   /* recovery end */
#if (!defined IS_ESP8266)
    ret = 0; // 只有8266判断结果，其它平台默认成功
#endif
    if(ret == 0) {
        LOG("rec succ!\r\n");
        rec_success();
        
    }
    else {
        LOG("rec fail!\r\n");
        rec_reboot();
    }
}

void recovery_main()
{
#if (defined IS_ESP8266)
    int flag = 0;

    reg_flash_flags = (unsigned int *)rec_get_recflag_addr();

    RHINO_CPU_INTRPT_DISABLE_NMI();
    vPortETSIntrLock();
    flag = recovery_check();
    if(flag == NORMAL_START)
    {
        vPortETSIntrUnlock();
        RHINO_CPU_INTRPT_ENABLE_NMI();
        return;
    }
    else if(flag == UPGRADE_START)
    {
        rec_hal_init();
        rec_upgrade_reboot();
        return;
    }
    // 后面肯定会复位，强行切换栈空间
    __asm__ volatile("movi a1, 0x3FFFFFF0" : : : "memory");
#endif
    rec_hal_init();
    LOG("rec init\r\n");
    recovery_process();
}

