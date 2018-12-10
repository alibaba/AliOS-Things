#include "rec_define.h"
#include "ota_hal_plat.h"
#include "ota_hal_os.h"

void sys_clear_ota_flag()
{
    int offset = 0x00;
    int param_part = HAL_PARTITION_PARAMETER_1;
    ota_boot_param_t param_w, param_r;
    memset(&param_w, 0, sizeof(ota_boot_param_t));
    hal_flash_read(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
    if((param_w.boot_count != 0) && (param_w.boot_count != 0xff)) {
        ota_crc16_ctx ctx;
        unsigned short crc;
        param_w.upg_flag = 0;
        param_w.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ota_crc16_init(&ctx);
        ota_crc16_update(&ctx, &param_w, sizeof(ota_boot_param_t) - sizeof(unsigned short));
        ota_crc16_final(&ctx, &crc);
        param_w.param_crc = crc;
        offset = 0x00;
        hal_flash_erase(param_part, offset, sizeof(ota_boot_param_t));
        offset = 0x00;
        hal_flash_write(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
        offset = 0x00;
        memset(&param_r, 0, sizeof(ota_boot_param_t));
        hal_flash_read(param_part, (uint32_t*)&offset, &param_r, sizeof(ota_boot_param_t));
        if(memcmp(&param_w, &param_r, sizeof(ota_boot_param_t)) != 0) {
           printf("clear ota flag failed!\r\n");
            return;
        }
        printf("now need to reboot to disable wdg\r\n");
        aos_reboot();
    }
    return;
}
