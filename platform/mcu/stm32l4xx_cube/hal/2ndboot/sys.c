#include "main.h"
#include "stm32l4xx_hal.h"
#include "hal_boot_process.h"

void hal_sys_delay(unsigned int ms)
{
     HAL_Delay(ms);
}

void hal_sys_reboot(void)
{
    HAL_NVIC_SystemReset();
    for(;;);
}
