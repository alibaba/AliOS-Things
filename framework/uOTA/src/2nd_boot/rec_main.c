/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if (defined BOARD_ESP8266)
#include "k_api.h"
#endif
#include "rec_pub.h"
#include "nbpatch.h"

unsigned int *reg_flash_flags = NULL;

#if (defined BOARD_ESP8266)
extern void vPortETSIntrLock(void);
extern void vPortETSIntrUnlock(void);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////

void recovery_get_flag_info(rec_flag_info_t *rec_flag_info)
{
    int ret = 0;
    PatchStatus pstatus;
    memset(&pstatus, 0, sizeof(PatchStatus));
    ret = patch_flash_read(HAL_PARTITION_PARAMETER_1,
          (unsigned char *) &pstatus, 0, sizeof(PatchStatus));
    if(ret < 0) {
        LOG("read flag err.");
        return;
    }
    rec_flag_info->flag = pstatus.upg_flag;
    rec_flag_info->num  = pstatus.boot_count;
}

void recovery_set_flag_info(rec_flag_info_t *rec_flag_info)
{
    int ret = 0;
    PatchStatus pstatus;
    memset(&pstatus, 0, sizeof(PatchStatus));
    ret = patch_flash_read(HAL_PARTITION_PARAMETER_1,
          (unsigned char *) &pstatus, 0, sizeof(PatchStatus));
    if(ret < 0) {
        LOG("read err");
        return;
    }

    pstatus.upg_flag = rec_flag_info->flag;
    pstatus.boot_count  = rec_flag_info->num;
    save_patch_status(&pstatus);
}

/* return value: 0 recovery start, 1 normal start, 2 upgrade to another partion  */
int recovery_check(void)
{
    int flag = REC_NORMAL_START;
    rec_flag_info_t rec_flag_info = {0, 0};
    rec_flash_init();
    recovery_get_flag_info(&rec_flag_info);
    switch (rec_flag_info.flag) {
		case REC_RECOVERY_FLAG :
		    flag = REC_RECOVERY_START;
		break;
        case REC_SWAP_UPDATE_FLAG :
            flag = REC_SWAP_UPDATE_START;
        break;
        case REC_RECOVERY_VERIFY_FLAG :
            if(rec_flag_info.num <= REC_MAX_NUM) {
                rec_flag_info.num += 1;
                recovery_set_flag_info(&rec_flag_info);
                flag = REC_NORMAL_START;
            } else { // more than 3 startup failures, automatic rollback
                rec_flag_info.flag = REC_ROLLBACK_FLAG;
                rec_flag_info.num  = 0;
                recovery_set_flag_info(&rec_flag_info);
                flag = REC_ROLLBACK_START;
            }
        break;
        case REC_ROLLBACK_FLAG :
            flag = REC_ROLLBACK_START;
        break;
        #if (defined BOARD_ESP8266)
	        case REC_UPGRADE_FLAG :
	        flag = REC_UPGRADE_START;
            break;
        #endif
        default:
            flag = REC_NORMAL_START;
        break;
    }
    return flag;
}

int recovery_flag_update()
{
    int flag = REC_NORMAL_START;
    rec_flag_info_t rec_flag_info = {0, 0};
    rec_flag_info_t rec_flag_set  = {REC_NORMAL_FLAG, 0};
    recovery_get_flag_info(&rec_flag_info);

    printf("flag = 0x%x\n", rec_flag_info.flag);
    switch (rec_flag_info.flag) {
        case REC_RECOVERY_FLAG :
            if(rec_flag_info.num <= REC_MAX_NUM) {
                rec_flag_set.flag = REC_RECOVERY_FLAG;
                rec_flag_set.num  = rec_flag_info.num + 1;
            }
            flag = REC_RECOVERY_START;
            break;
        case REC_SWAP_UPDATE_FLAG :
            flag = REC_SWAP_UPDATE_START;
            break;
        case REC_ROLLBACK_FLAG :
            flag = REC_ROLLBACK_START;
            break;
	    #if (defined BOARD_ESP8266)
	    case REC_UPGRADE_FLAG :
	        flag = REC_UPGRADE_START;
	    break;
	    #endif
        default:
            flag = REC_RECOVERY_START;
        break;
    }
    recovery_set_flag_info(&rec_flag_set);
    return flag;
}

void recovery_error(void *errinfo)
{
    rec_err_print(errinfo);
    rec_reboot();
}

void rec_success()
{
    rec_flag_info_t rec_flag_info = {0, 0};
#if (defined BOARD_ESP8266)
    rec_flag_info.flag = REC_UPGRADE_FLAG;
#else
    rec_flag_info.flag = REC_RECOVERY_VERIFY_FLAG;
#endif
    recovery_set_flag_info(&rec_flag_info);
    rec_delayms(500);
    rec_reboot();
}

void rec_start()
{
    rec_flag_info_t rec_flag_info = {0, 0};
    rec_flag_info.flag = REC_RECOVERY_FLAG;
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
#if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_DIRECT)  //
    ret = 0;
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

#if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_ABBACK)
void rec_swap_update_process()
{
    int ret;
    ret = nbpatch_swap_app2ota(TRUE);
    if(ret == 0) {
        LOG("update succ!\r\n");
        rec_success();
    }
    else {
        LOG("update fail!\r\n");
        rec_reboot();
    }
}
#endif

void recovery_main()
{
    int flag = 0;
#if (defined BOARD_ESP8266)
    RHINO_CPU_INTRPT_DISABLE_NMI();
    vPortETSIntrLock();
#endif
    rec_hal_init();
    flag = recovery_flag_update();
#if (defined BOARD_ESP8266)
    if((REC_UPGRADE_START == flag) || (REC_ROLLBACK_START == flag)) {
		vPortETSIntrUnlock();
        RHINO_CPU_INTRPT_ENABLE_NMI();
        rec_upgrade_reboot();
        return;
    }
    // switch the stack space, it's very important for esp8266
    __asm__ volatile("movi a1, 0x3FFFFFF0" : : : "memory");
#else
    rec_wdt_init(REC_WDT_TIMEOUT_MS);
    rec_wdt_feed();
    if(REC_ROLLBACK_START == flag) {
        rec_2boot_rollback();
        return;
    }
#if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_ABBACK)
    else if (REC_SWAP_UPDATE_START == flag) {
        rec_swap_update_process();
        return;
    }
#endif
#endif
    recovery_process();
}

