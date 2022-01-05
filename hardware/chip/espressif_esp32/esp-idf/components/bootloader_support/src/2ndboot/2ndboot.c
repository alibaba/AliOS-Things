/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "2ndboot.h"
#include "updater.h"
#include "libc.h"
int ota_patch_show_percent(int per)
{
   int ret = 0;
   printf("Show upgrade percent:%d \n", per);
   return ret;
}

int ota_updater(void)
{
    int ret = -1;
    ota_boot_param_t ota_param = {0};
    ret = ota_patch_read_param(&ota_param);
    if(ret < 0) {
        printf("read param err.\n");
        return -1;
    }
    ota_param.boot_count++;
    printf("ota upg_flag:0x%04x count:%d crc:0x%04x boot_type:%d \n", ota_param.upg_flag, ota_param.boot_count, ota_param.param_crc, ota_param.boot_type);
    if(ota_param.upg_flag == OTA_UPGRADE_DIFF) {
        ret = ota_nbpatch_main();
    } else if(ota_param.upg_flag == OTA_UPGRADE_XZ) {
        ret = ota_xz_main();
    } else if(ota_param.upg_flag == OTA_UPGRADE_ALL) {
        if(ota_param.boot_type == 0) {
            ret = ota_image_check(ota_param.src_adr, ota_param.len, ota_param.crc);
            if(ret < 0) {
                printf("CRC fail.\n");
                goto EXIT;
            }
            ret = ota_image_copy(ota_param.dst_adr, ota_param.src_adr, ota_param.len);
        } else {
            printf("dual boot, not copy.\n");
        }
    } else {
        printf("No OTA upgrade.\n");
        return 0;
    }

EXIT:
    printf("ota update complete ret:%d \n", ret);
    if(ret < 0 && ota_param.boot_count <= 3) {
        ota_patch_write_param(&ota_param);
        printf("sleep 100ms\r\n");
        sys_delayms(100);
        printf("reboot\r\n");
        sys_reboot();
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
    //printf(errinfo);
    sys_reboot();
}

void print_usage(void)
{
#ifdef SYSINFO_2NDBOOT_VERSION
    printf("2ndboot ver: " SYSINFO_2NDBOOT_VERSION "\r\n");
#else
	
    printf("2ndboot ver:\r\n");
#endif
#ifdef AOS_2ND_BOOT_AB
    printf("Please input 1-6 to select functions\r\n");
#else
    printf("Please input 1-2 to select functions\r\n");
#endif
    printf("[1] Uart Ymodem Upgrade \r\n");
    printf("[2] System Reboot \r\n");
#ifdef AOS_2ND_BOOT_AB
    printf("[3] Show AB Boot \r\n");
    printf("[4] Switch AB Boot \r\n");
#endif
    printf("[h] Help Info\r\n");
}

int ymodem_upgrade(void);
static void ota_2ndboot_cli_menu(void)
{
    unsigned char c   = 0;
#ifdef AOS_2ND_BOOT_AB
    int ab = 0;
#endif

    print_usage();
    printf("\r\naos boot# ");

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
                    sys_reboot();
                    break;
#ifdef AOS_2ND_BOOT_AB
                case '3' :
                    ab = ota_2ndboot_ab_get();
                    printf("%s Boot .. \r\n", (ab == 0)?"A":"B");
                    break;
                case '4' :
                    ab = ota_2ndboot_ab_get();
                    printf("Switch %s to %s Boot .. \r\n", (ab == 0)?"A":"B", (ab == 0)?"B":"A");
                    ota_2ndboot_ab_switch();
                    break;
#endif 
                default:
                    print_usage();
                    break;
            }
            printf("\r\naos boot# ");
            sys_delayms(1);
        }
    }
}
#ifdef AOS_2ND_BOOT_NO_LDS
#define HEAP_SIZE (85*1024)
#define TEST_HEAP_SIZE (80*1024)
static unsigned char heap[HEAP_SIZE];
void *sys_set_heap(unsigned int *size)
{   
   *size = HEAP_SIZE;
   return (void *)heap;
}
void test_heap(void)
{
    unsigned char *buf;
    int i;

    buf = (unsigned char *)malloc(TEST_HEAP_SIZE);
    if(!buf) {
        printf("%s:%d test heap fail\r\n", __func__, __LINE__);
        return;
    }
    for (i = 0; i < TEST_HEAP_SIZE; i++) {
            buf[i] = i % 0xff;
    }
    for (i = 0; i < TEST_HEAP_SIZE; i++) {
        if (buf[i] != (i % 0xff)) {
            break;
        }
    }
    if (i == TEST_HEAP_SIZE) {
        printf("test sram success\r\n");
    } else {
        printf("test heap fail\r\n");
    }
    free(buf);

}
#endif
static int ota_2ndboot_init()
{
    #ifdef AOS_2ND_BOOT_NO_LDS
    _rec_heap_start = sys_set_heap(&_rec_heap_len);
    test_heap();
    #endif
    return 0;
}

int ota_2ndboot_main(void)
{
    int ret = 0;
    unsigned char c = 0;
    unsigned int  i = 0;

    ota_2ndboot_init();
    uart_init();
    printf("%s\r\n", __DATE__);
    printf("\r\nPress key \'w\' to 2ndboot cli menu in 100ms.\r\n");
    while(1) {
        if(uart_recv_byte(&c) && ('w' == c)) {
            ota_2ndboot_cli_menu();
			return 0;
        }
        i ++;
        if(i >= 100)break;
        sys_delayms(1);
    }
    wdg_init(OTA_2NDBOOT_WDG_TIMEOUT*1000);
    wdg_feed();
    /* check OTA upgrade */
    ret = ota_updater();
    wdg_finish();

    return ret;
}
