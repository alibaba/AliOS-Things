#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <aos/log.h>
#include <errno.h>
#include <hal/ota.h>
#include <hal/soc/soc.h>
#include "sys/flash.h"
#include "osal.h"
#include <CheckSumUtils.h>
#include "wdt/drv_wdt.h"
#include "common.h"

#define KV_HAL_OTA_CRC16  "hal_ota_get_crc16"

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
//static  CRC16_Context contex;

static uint16_t hal_ota_get_crc16(void);
static void  hal_ota_save_crc16(uint16_t crc16);
static int sv6266_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len);
static int sv6266_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len);
static int sv6266_ota_set_boot(hal_ota_module_t *m, void *something);

extern uint32_t FLASH_BEGIN;
extern uint32_t __lds_reserved_start;
const char* bootflag = "bootflag";

/*! \brief hal_ota_switch_to_new_fw update bootflag.
 *
 *  check the ota image & update bootflag.
 *
 * \return 0: success, otherwise: failed.
 */
int hal_ota_switch_to_new_fw()
{
#define M_OTA_HEADER_CHECK      (6)
    uint32_t reserved_addr = (uint32_t)(&__lds_reserved_start) - (uint32_t)(&FLASH_BEGIN);
    uint32_t ota_data_check[M_OTA_HEADER_CHECK];
    OS_DeclareCritical();

    uint32_t offset = 4;
    int32_t ret = 0;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    ret = hal_flash_read(pno, &offset, ota_data_check, sizeof(uint32_t)*M_OTA_HEADER_CHECK);
    if (ret == EIO) {
        return -1;
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
    REG32(0xc0000000) = 0x00200000;
    return 0;
}


unsigned int _off_set = 0;

static int sv6266_ota_set_boot(hal_ota_module_t *m, void *something)
{
    ota_finish_param_t *param = (ota_finish_param_t *)something;

    if (param == NULL) {
        return kGeneralErr;
    }
    if (param->result_type == OTA_FINISH) {
        //CRC16_Final( &contex, (uint16_t *)&ota_info.ota_crc );
        printf("switch to new fw\n");
        memset(&ota_info, 0 , sizeof ota_info);
        if (hal_ota_switch_to_new_fw() != 0) {
            return kGeneralErr;
        }
    }
    return kNoErr;
}

