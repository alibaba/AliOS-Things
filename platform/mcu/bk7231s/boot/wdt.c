
#include "include.h"
#include "arm_arch.h"
#include "bootloader.h"

#include "wdt_pub.h"
#include "wdt.h"
#include "icu_pub.h"
#include "icu.h"

static unsigned int g_wdt_period = 0;

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    unsigned int reg;
    unsigned int para;

    if (wdg == NULL) {
        return -1;
    }

    para = PWD_ARM_WATCHDOG_CLK_BIT;

    reg = REG_READ(ICU_PERI_CLK_PWD);
    reg &= ~(para);
    REG_WRITE(ICU_PERI_CLK_PWD, reg);


    g_wdt_period = (wdg->config.timeout);

    reg = WDT_1ST_KEY << WDT_KEY_POSI;
    reg |= ((wdg->config.timeout) & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
    REG_WRITE(WDT_CTRL_REG, reg);

    reg = WDT_2ND_KEY << WDT_KEY_POSI;
    reg |= ((wdg->config.timeout) & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
    REG_WRITE(WDT_CTRL_REG, reg);

    return 0;
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    unsigned int reg;
    unsigned int para;
    para = PWD_ARM_WATCHDOG_CLK_BIT;

    reg = REG_READ(ICU_PERI_CLK_PWD);
    reg |= para;
    REG_WRITE(ICU_PERI_CLK_PWD, reg);

    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    unsigned int reg;

    reg = WDT_1ST_KEY << WDT_KEY_POSI;
    reg |= (g_wdt_period & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
    REG_WRITE(WDT_CTRL_REG, reg);

    reg = WDT_2ND_KEY << WDT_KEY_POSI;
    reg |= (g_wdt_period & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
    REG_WRITE(WDT_CTRL_REG, reg);
}


