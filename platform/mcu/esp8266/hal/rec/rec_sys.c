/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "esp_system.h"
#include "upgrade.h"
#include "rec_sys.h"

extern void rec_uart_init();
extern void rec_uart_deinit();
extern void rec_flash_init();

extern void krhino_sched_disable(void);
extern void vPortETSIntrLock(void);
extern void vPortETSIntrUnlock(void);

unsigned int rec_xz_backup_flash_addr;
unsigned int rec_conf_backup_flash_addr;
extern volatile uint32_t g_crash_steps;

void rec_delayms(volatile int timesMS)
{
	volatile unsigned long i;

    while (timesMS --)
	{
		i = 0;
		while (i < 29500)
		{
			i++;
		}
	}
}

void rec_hal_flashmap_init()
{
     int ret = 0;
    hal_logic_partition_t *logic_partition = NULL;
    uint32_t addr = 0;
    uint32_t ota_len;

    /*
      * ota download区    : 0 ~ 256k
      * ota conf backup区 : end-4k ~ end存放配置数据备份信息
      */

    // 1. 获取OTA分区大小
    logic_partition = rec_flash_get_info(HAL_PARTITION_OTA_TEMP);
    if(logic_partition == NULL) {
        printf("error:get OTA partion fail!!!\n");
        return;
    }

    rec_xz_backup_flash_addr   = logic_partition->partition_length - FLASH_SECTOR_SIZE - 64*1024; // 注意，该变量无用，此处仅为了保持跟其它平台定义统一
    rec_conf_backup_flash_addr = logic_partition->partition_length - FLASH_SECTOR_SIZE;
}

void rec_hal_init()
{
    aos_wdt_disable();
    rec_flash_init();
    rec_uart_init();
    rec_hal_flashmap_init();
}

void rec_err_print(void *errinfo)
{
    /* exception print, same with "_recovery_error" */
}

void rec_reboot(void)
{
    printf("reboot!\n");

    vPortETSIntrLock();
    g_crash_steps = 0x87654321;
    aos_watchdog_init();
}

unsigned int rec_get_boot_addr()
{
    return system_get_userbin_addr();
}

unsigned int rec_get_ota_mode()
{
    if(SYS_BOOT_NORMAL_MODE == system_get_boot_mode())
    {
        return 2;  //TODO: 需要定义成枚举，2表示双分区主备模式
    }

    return 1; //TODO: 需要定义成枚举，1表示单分区主备模式
}

// 获取备区启动地址
unsigned int rec_get_bakpartion_boot_addr()
{
    unsigned int boot_addr;
    boot_addr = system_get_userbin_addr();
    if(boot_addr == 0x1000)
    {
        return 0x101000;
    }
    else
    {
        return 0x1000;
    }
}

// 重启
void rec_upgrade_reboot()
{
    printf("system upgrade init...\n");
    system_upgrade_init();
    system_upgrade_flag_set(UPGRADE_FLAG_FINISH);

    vPortETSIntrLock();
    aos_wdt_disable();
    krhino_sched_disable();
    printf("upgrade reboot...\n");
    system_upgrade_reboot();
}

