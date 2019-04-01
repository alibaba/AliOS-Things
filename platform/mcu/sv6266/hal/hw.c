#include <hal/soc/soc.h>
#include <hal/wifi.h>
#include <hal/ota.h>
#include "soc_defs.h"

void hal_reboot(void)
{
    REG32(0xC0000000) = (1<<21);
}

extern hal_wifi_module_t sim_aos_wifi_icomm;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_icomm);
    sim_aos_wifi_icomm.init(&sim_aos_wifi_icomm);
}
