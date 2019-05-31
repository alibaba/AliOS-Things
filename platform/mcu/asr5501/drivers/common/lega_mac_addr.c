#include <stdint.h>
#include <stdlib.h>
#ifdef ALIOS_SUPPORT
#include "lega_rhino.h"
#else
#include "lega_rtos_api.h"
#endif
#include "lega_common.h"

int get_random_int_c()
{
    int i;
    srand(lega_rtos_get_time());
    i = rand();
    return i;
}

/**
  * @brief    if no mac address burned in efuse memory,
  *           user could modify this function to set default mac address
  *
  * @param    mac_addr : mac_addr pointer wifi stack used
  */
void lega_wlan_default_mac_addr(uint8_t *mac_addr)
{
    mac_addr[0] = 0xAA;
    mac_addr[1] = 0x00;
    mac_addr[2] = 0xCC;
    mac_addr[3] = (uint8_t)((get_random_int_c()>>16)&0xff);
    mac_addr[4] = (uint8_t)((get_random_int_c()>>8)&0xff);
    mac_addr[5] = (uint8_t)((get_random_int_c()>>0)&0xff);
}

void lega_rf_set_xo_freq_bias(void)
{
#ifdef XO_FREQ_BIAS_CONFIG
    REG_WR(0x40000A74, 0x9191);
#endif
    printf("Current xo config is 0x%x\r\n",REG_RD(0x40000A74));
}