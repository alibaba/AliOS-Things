/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "bootloader.h"

int ota_patch_show_percent(int per)
{
   int ret = 0;
   OTA_LOG_I("Show upgrade percent:%d \n", per);
   return ret;
}

int ota_updater(void)
{
    int ret = -1;
    ota_boot_param_t ota_param = {0};
    ret = ota_patch_read_param(&ota_param);
    if(ret < 0) {
        OTA_LOG_I("read param err.\n");
        return -1;
    }
    ota_param.boot_count++;
    OTA_LOG_I("ota upg_flag:0x%04x count:%d crc;0x%04x \n", ota_param.upg_flag, ota_param.boot_count, ota_param.param_crc);
    if(ota_param.upg_flag == OTA_UPGRADE_DIFF) {
        ret = ota_nbpatch_main();
    } else if(ota_param.upg_flag == OTA_UPGRADE_XZ){
        ret = ota_xz_main();
    } else if(ota_param.upg_flag == OTA_UPGRADE_ALL) {
        ret = ota_image_check(ota_param.src_adr, ota_param.len, ota_param.crc);
        if(ret < 0){
             OTA_LOG_I("CRC fail.\n");
             goto EXIT;
        }
        ret = ota_image_copy(ota_param.dst_adr, ota_param.src_adr, ota_param.len);
    } else {
        OTA_LOG_I("No OTA upgrade.\n");
        return 0;
    }

EXIT:
    OTA_LOG_I("ota update complete ret:%d \n", ret);
    if(ret < 0 && ota_param.boot_count <= 3) {
        ota_patch_write_param(&ota_param);
        boot_delay(100);
        boot_reset();
    } else {
        if(ota_param.upg_flag != 0) {
            ota_param.upg_flag = 0;
            ota_patch_write_param(&ota_param);
        }
        if(ota_param.boot_count > 3) { /* todo: rollback old version for dual banker boot. */
        }
    }
    return ret;
}

void ota_2ndboot_error(void *errinfo)
{
    printf(errinfo);
    boot_reset();
}

void print_usage(void)
{
    printf("2ndboot ver: " SYSINFO_2NDBOOT_VERSION "\r\n");
    printf("Please input 1-2 to select functions\r\n");
    printf("[1] Uart Ymodem Upgrade \r\n");
    printf("[2] System Reboot \r\n");
    printf("[h] Help Info\r\n");
    printf("2ndboot# ");
}

static void ota_2ndboot_cli_menu(void)
{
    unsigned char c   = 0;
    print_usage();
    while(1) {
        if(uart_recv_byte(&c)) {
            if('w' == c)  {
                continue;
            }
            if( ('\r' == c) || ('\n' == c) ) {
                printf("\r\naos boot# ");
                continue;
            }
            printf("%c \r\n", c);
            switch(c) {
                case '1' :
                    ymodem_upgrade();
                    break;
                case '2' :
                    printf("Reboot \r\n");
                    boot_reset();
                    break;
                default:
                    print_usage();
                    break;
            }
            printf("\r\n2ndboot# ");
            boot_delay(1);
        }
    }
}

int boot_main(void)
{
    int ret = 0;
    unsigned char c = 0;
    unsigned int  i = 0;

    uart_init();

    printf("\r\nPress key \'w\' to 2ndboot cli menu in 100ms.\r\n");
    while(1) {
        if(uart_recv_byte(&c) && ('w' == c)) {
            ota_2ndboot_cli_menu();
            return 0;
        }
        i ++;
        if(i >= 100) {
            break;
        }
        boot_delay(1);
    }

    boot_wdg_init(OTA_2NDBOOT_WDG_TIMEOUT*1000);

    boot_wdg_reload();

    /* check OTA upgrade */
    ret = ota_updater();

    boot_wdg_finalize();

    return ret;
}

