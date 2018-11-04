#ifndef WINC1500_MAIN_H
#define WINC1500_MAIN_H

#include "atmel_start.h"

#include "driver/include/m2m_wifi.h"

#define CONF_WINC_PIN_CHIP_SELECT CS_PIN
#define CONF_WINC_PIN_CHIP_ENABLE CE_PIN
#define CONF_WINC_PIN_RESET RESET_PIN

void wifi_init(tstrWifiInitParam *params);

#endif
