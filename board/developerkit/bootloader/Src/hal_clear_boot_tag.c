#include "stm32l4xx_hal.h"
#include "hal_clear_boot_tag.h"
#include "rec_define.h"

void HAL_NVIC_SystemReset(void);

void hal_clear_boot_tag()
{
    rec_flag_info_t ota_packet_info;
    ota_packet_info.flag = REC_NORMAL_FLAG;
    ota_packet_info.num = 0;
    recovery_set_flag_info(&ota_packet_info);
    HAL_NVIC_SystemReset();
}
