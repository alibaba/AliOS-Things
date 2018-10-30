
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "rec_pub.h"

#define REC_2BOOT_VERSION SYSINFO_2BOOT_VERSION

int rec_2boot_cmd_flags = 0;
int rec_process_flags   = 0;

int rec_2boot_cmd_check(void)
{
    rec_flag_info_t rec_flag_info = {0, 0};
    unsigned char c = 0;
    unsigned int  i = 0;

#ifdef REC_2BOOT_WITH_CMDLINE
    rec_uart_init();
    printf("\r\nPress key \'w\' into Second bootloader cmd line, Waiting 100 ms ... \r\n");

    while(1) {
        if(uart_recv_byte(&c) && ('w' == c)) {
            printf("begin second bootloader cmd \r\n");
            return 0;
        }
        i ++;
        if(i >= 100)break;
        rec_delayms(1);
    }
    printf("normal bootup, flag 0x%x, num 0x%x\r\n", rec_flag_info.flag, rec_flag_info.num);
#endif
    rec_flash_init();
    recovery_get_flag_info(&rec_flag_info);
    if(REC_RECOVERY_VERIFY_FLAG == rec_flag_info.flag) {
        rec_wdt_init(REC_WDT_TIMEOUT_MS);
        rec_wdt_feed();
    }

    return 1;
}

void rec_2boot_cmd_process()
{
#ifdef REC_2BOOT_WITH_CMDLINE
    PatchStatus *pstatus;
    unsigned char c = 0;

    pstatus = nbpatch_get_pstatus();
    read_patch_status(pstatus);

    printf("aos second bootloader version: "REC_2BOOT_VERSION"\r\n");

    printf("[1] Query  firmware version information \r\n");
    printf("[2] Update firmware by UART XYMODEN protocol \r\n");
    printf("[3] Update firmware by CANbus \r\n");
    printf("[4] Update firmware by USB Storage \r\n");
    printf("[5] Active backup firmware\r\n");
    printf("[6] Reboot\r\n");
    printf("Please input 1-6 to select the corresponding function\r\n");
    printf("# ");
    while(1) {
        if(uart_recv_byte(&c)) {
            if((c == 0)||(c == 0xd)||(c == 'w')) {
                rec_delayms(1);
                continue;
            }
            printf("%c \r\n", c);
            switch(c) {
                case '1' :
                    printf("firmware main partion version: %s\r\n", pstatus->app_version);
                    printf("firmware back partion version: %s\r\n", pstatus->ota_version);
                    break;

                case '2' :
                case '3' :
                case '4' :
                    printf("command %c not supported \r\n", c);
                    break;

                case '5' :
                    printf("begin active backup firmware %s\r\n", pstatus->ota_version);
                    nbpatch_swap_app2ota(TRUE);
                    //说明：此处激活备区版本后，直接复位，故此处不需要break
                case '6' :
                    printf("begin reboot \r\n");
                    rec_reboot();
                    break;

                default:
                    printf("Please input 1-6 to select the corresponding function\r\n");
                    break;
            }
            printf("# ");
            rec_delayms(1);
        }
    }
#endif
}

void rec_2boot_roolback_count_clear()
{
    rec_flag_info_t rec_flag_info;

    recovery_get_flag_info(&rec_flag_info);

    rec_flag_info.num = 0;

    recovery_set_flag_info(&rec_flag_info);

    return;
}

void rec_2boot_rollback(void)
{
    printf("begin roll back firmware version...\r\n");

#if (OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_ABBACK)
    nbpatch_swap_app2ota(TRUE);
#elif (OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_ABBOOT)
#endif
	rec_2boot_roolback_count_clear();
    printf("begin reboot...\r\n");
    rec_reboot();
}

