
#include "aos/hal/wdg.h"
#include "drv/wdt/hal_wdt.h"

int32_t
hal_wdg_init (
    wdg_dev_t   *wdg
    )
{
    int32_t retval;

    hal_wdt_init ();
    retval = hal_wdt_register_isr ((WDT_ID_E) wdg->port, 0, NULL);
    return hal_wdt_enable ((WDT_ID_E) wdg->port, wdg->config.timeout);
}

void
hal_wdg_reload (
    wdg_dev_t   *wdg
    )
{
    int32_t retval;
    retval = hal_wdt_kick ((WDT_ID_E) wdg->port);
}

int32_t
hal_wdg_finalize (
    wdg_dev_t   *wdg
    )
{
    return hal_wdt_disable ((WDT_ID_E) wdg->port);
}
