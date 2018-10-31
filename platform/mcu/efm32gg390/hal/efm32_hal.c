#include <stdio.h>
#include <k_api.h>


volatile  uint32_t uwTick = 0;

void HAL_IncTick(void)
{
    uwTick++;
}

uint32_t HAL_GetTick(void)
{
    return uwTick;
}

void hal_reboot(void)
{
    // reboot hard ware
}
