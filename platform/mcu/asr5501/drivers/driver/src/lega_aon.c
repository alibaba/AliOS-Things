#include "lega_aon.h"

uint32_t lega_aon_is_power_on_reset()
{
    uint32_t powerOnFlag = *((uint32_t *)(POWERON_FLAG_RETENTION_RAM_ADDR));
    if(powerOnFlag!=LEGA_AON_POWER_ON_FLAG){
        *((uint32_t *)(POWERON_FLAG_RETENTION_RAM_ADDR)) = LEGA_AON_POWER_ON_FLAG;
        return 1;
    }else
        return 0;
}