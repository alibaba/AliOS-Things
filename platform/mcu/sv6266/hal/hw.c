#include <network/hal/wifi.h>
#include "soc_defs.h"
#include "osal.h"
#include "aos/hal/flash.h"
#include "wdt/drv_wdt.h"

extern uint32_t FLASH_BEGIN;
extern uint32_t __lds_reserved_start;
const char* bootflag = "bootflag";

int hal_reboot_bank(void)
{
#define M_OTA_HEADER_CHECK      (6)
    uint32_t reserved_addr = (uint32_t)(&__lds_reserved_start) - (uint32_t)(&FLASH_BEGIN);
    uint32_t ota_data_check[M_OTA_HEADER_CHECK];
    OS_DeclareCritical();

    uint32_t offset = 4;
    int32_t ret = 0;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    ret = hal_flash_read(pno, &offset, ota_data_check, sizeof(uint32_t)*M_OTA_HEADER_CHECK);
    if (ret < 0 ) {
        return ret;
    }
    if (ota_data_check[0] != XTAL) {
        return -2;
    }
    if (ota_data_check[1] != SYS_BUS_SPEED) {
        return -3;
    }
    if (ota_data_check[2] != XIP_BIT) {
        return -4;
    }
    if (ota_data_check[5] != SETTING_PSRAM_HEAP_BASE) {
        return -7;
    }
    OS_EnterCritical();
#if defined(CONFIG_ENABLE_WDT)
    drv_wdt_kick(SYS_WDT);
#endif
    flash_sector_erase(reserved_addr);
#if defined(CONFIG_ENABLE_WDT)
    drv_wdt_kick(SYS_WDT);
#endif
    flash_page_program(reserved_addr, strlen(bootflag), bootflag);
#if defined(CONFIG_ENABLE_WDT)
    drv_wdt_kick(SYS_WDT);
#endif
    OS_ExitCritical();
    REG32(0xC0000000) = (1<<21);
    return 0;
}

void hal_reboot(void)
{
    REG32(0xC0000000) = (1<<21);
}

extern hal_wifi_module_t sim_aos_wifi_icomm;
//extern struct hal_ota_module_s sv6266_ota_module;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_icomm);
//    hal_ota_register_module(&sv6266_ota_module);
    sim_aos_wifi_icomm.init(&sim_aos_wifi_icomm);
}
