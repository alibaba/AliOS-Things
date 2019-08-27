#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_boot.h"

uint8_t lega_get_boot_type(void)
{
    uint32_t flag1 = REG_RD(POWERON_FLAG_RETENTION_RAM_ADDR);
    uint32_t flag2 = REG_RD(AON_RST_CHECK_REG);
    if((LEGA_AON_POWER_ON_FLAG == flag1) && (AON_RST_CHECK_FLAG == (flag2 & AON_RST_CHECK_FLAG)))
    {
        return SOFTWARE_RST;
    }
    else if((LEGA_AON_POWER_ON_FLAG == flag1) && (0 == flag2))
    {
        REG_WR(AON_RST_CHECK_REG, AON_RST_CHECK_FLAG);
        return HARDWARE_PIN_RST;
    }
    else
    {
        REG_WR(AON_RST_CHECK_REG, AON_RST_CHECK_FLAG);
        REG_WR(POWERON_FLAG_RETENTION_RAM_ADDR, LEGA_AON_POWER_ON_FLAG);
        return PWR_ON_RST;
    }
}
