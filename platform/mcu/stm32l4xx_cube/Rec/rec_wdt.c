#include "hal_boot_wdg.h"

void rec_wdt_init(unsigned int timeout_ms)
{
    unsigned int param = timeout_ms / 1000;
    hal_boot_wdg_init(param);
}

void rec_wdt_start()
{
    ;
}

void rec_wdt_stop()
{
    ;
}

void rec_wdt_feed()
{
    hal_boot_feed_dg();
}
