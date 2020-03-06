
#include <stdio.h>
#include <stdarg.h>
#include "bootloader.h"

extern const hal_logic_partition_t hal_partitions[];

extern void uart_hw_init(unsigned char uport);
extern void clock_setup();

void boot_entry(void)
{
    int              ret;
    ota_boot_param_t param;
    unsigned int     addr       = 0;
    unsigned int     flash_addr = 0;

    void (*os_entry)(void);

    pwm_init();

    clock_setup();

    uart_hw_init(1);
    flash_init();

    boot_main();

    memset(&param, 0, sizeof(param));

    ret = ota_patch_read_param(&param);

    if (ret != 0) {
        printf("ota_patch_read_param fail\n\r");
        return;
    }

    if ((param.upg_flag == OTA_UPGRADE_ALL) || (param.upg_flag == OTA_UPGRADE_XZ) || (param.upg_flag == OTA_UPGRADE_DIFF)) {

        if ((param.dst_adr == hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr) ||
            (param.dst_adr == hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr)) {

            if ((param.dst_adr % 0x11) == 0)
            {
                flash_addr = param.dst_adr;
                addr = (flash_addr / 0x11) * 0x10;
            }
        }
    }


    if (addr == 0) {

        flash_addr = hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr;
        addr = (flash_addr / 0x11) * 0x10;
    }

    os_entry = (void (*)(void))addr;

    os_entry();

}

void print_exception_addr(unsigned int pc, unsigned int lr, unsigned int sp)
{
    printf("boot print_exception_addr\n\r");
    while (1);
}

void error_show()
{
    printf("boot error_show\n\r");
    hal_sys_delay(100);
    hal_sys_reboot();
}


