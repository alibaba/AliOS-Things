#include "esp_system.h"

void aos_hal_reboot(void)
{
    esp_restart();
}
