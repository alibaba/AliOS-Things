#include <network/hal/wifi.h>
//#include <aos/hal/ota.h>
#include "soc_defs.h"

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
