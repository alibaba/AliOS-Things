#include "hal_boot_wdg.h"
#include "2ndboot.h"

void wdg_init(unsigned int ms)
{
    unsigned int param = ms / 1000;
    hal_boot_wdg_init(param);
}

void wdg_finish(void)
{
    ;
}

void wdg_feed(void)
{
    hal_boot_feed_dg();
}
