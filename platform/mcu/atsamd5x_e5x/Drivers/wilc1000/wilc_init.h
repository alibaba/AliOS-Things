#ifndef WINC1500_MAIN_H
#define WINC1500_MAIN_H

#include "atmel_start.h"

#include "driver/include/m2m_wifi.h"

#define CONF_WINC_PIN_CHIP_SELECT CARD_CS_PIN
#define CONF_WINC_PIN_CHIP_ENABLE CARD_CE_PIN
#define CONF_WINC_PIN_RESET CARD_RESET_PIN

void wifi_init(tstrWifiInitParam *params);

#endif
