#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 

#include <errno.h>
#include <hal/ota.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include "stm32l4xx_hal_flash.h"


extern int FLASH_set_boot_bank(uint32_t bank);

int hal_ota_switch_to_new_fw()
{
    if (0 == FLASH_set_boot_bank(FLASH_BANK_BOTH)) {
        printf("Default boot bank switched successfully.\n");
        return 0;
    } else {
        printf("Error: failed changing the boot configuration\n");
        return -1;
    }
}


unsigned int _off_set = 0;
static int stm32l475_ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;

    printf("set ota init---------------\n");
    if (!FLASH_bank1_enabled(FLASH_BANK_BOTH)) {
        pno = HAL_PARTITION_APPLICATION;
    }
    
    partition_info = hal_flash_get_info( pno );
    hal_flash_erase(pno, 0 ,partition_info->partition_length);

    _off_set = 0;

    return 0;
}


static int stm32l475_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;

    printf("set write len---------------%d\n", in_buf_len);
    if (!FLASH_bank1_enabled(FLASH_BANK_BOTH)) {
        pno = HAL_PARTITION_APPLICATION;
    }
    printf("stm32l475_ota_write: pno = %d\n", pno);
    int ret = hal_flash_write(pno, &_off_set, in_buf, in_buf_len);

    return ret;
}

static int stm32l475_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
    return 0;
}

static int stm32l475_ota_set_boot(hal_ota_module_t *m, void *something)
{
    printf("set boot---------------\n");
    return hal_ota_switch_to_new_fw();
}

struct hal_ota_module_s stm32l475_ota_module = {
    .init = stm32l475_ota_init,
    .ota_write = stm32l475_ota_write,
    .ota_read = stm32l475_ota_read,
    .ota_set_boot = stm32l475_ota_set_boot,
};
