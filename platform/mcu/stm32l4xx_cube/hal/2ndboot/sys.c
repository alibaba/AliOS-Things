#include "main.h"
#include "stm32l4xx_hal.h"
#include "hal_boot_process.h"

void sys_delayms(volatile int timesMS)
{
     HAL_Delay(timesMS);
}

void sys_reboot(void)
{
    HAL_NVIC_SystemReset();
    for(;;);
}
