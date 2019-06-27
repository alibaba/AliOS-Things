#ifndef _LEGA_AON_H_
#define _LEGA_AON_H_
#include "lega_peripheral_reg.h"

#define LEGA_AON_POWER_ON_FLAG 0x12345678
uint32_t lega_aon_is_power_on_reset();

#endif