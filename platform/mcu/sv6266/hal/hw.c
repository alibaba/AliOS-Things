#include <hal/soc/soc.h>
#include <hal/wifi.h>
#include <hal/ota.h>
#include "soc_defs.h"
#include "osal.h"

void hal_reboot(void)
{
    OS_DeclareCritical();
    OS_EnterCritical();
    drv_wdt_init();
    drv_wdt_enable(1, 100);
    while(1);
    OS_ExitCritical();
}

extern hal_wifi_module_t sim_aos_wifi_icomm;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_icomm);
    sim_aos_wifi_icomm.init(&sim_aos_wifi_icomm);
}
