#include "hal/ota.h"
#include "hal/soc/flash.h"

#define DEBUG_OTA csp_printf

int cc3220_ota_init(hal_ota_module_t *m, void *something)
{
    DEBUG_OTA("cc3220_ota_init\r\n");

    return 0;
}
int cc3220_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set,
				 uint8_t *in_buf , uint32_t in_buf_len)
{
    DEBUG_OTA("cc3220_ota_write\r\n");
    return in_buf_len; 
    //hal_flash_erase_write(HAL_PARTITION_APPLICATION, (uint32_t *)off_set, in_buf, in_buf_len);
}

int cc3220_ota_read(hal_ota_module_t *m,  volatile uint32_t *off_set,
				uint8_t *out_buf , uint32_t out_buf_len)
{
    DEBUG_OTA("cc3220_ota_read\r\n");
    return 0;
}

int cc3220_ota_set_boot(hal_ota_module_t *m, void *something)
{
    DEBUG_OTA("cc3220_ota_set_boot\r\n");
    return 0;
}


struct hal_ota_module_s hal_cc3220_ota_module = {
    .init = cc3220_ota_init,
    .ota_write = cc3220_ota_write,
    .ota_read = cc3220_ota_read,
    .ota_set_boot = cc3220_ota_set_boot,
};


