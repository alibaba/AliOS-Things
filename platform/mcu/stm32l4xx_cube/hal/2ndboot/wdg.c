#include "hal_boot_wdg.h"
#include "bootloader.h"

void wdg_init(unsigned int ms)
{
    unsigned int param = ms / 1000;
    hal_boot_wdg_init(param);
}

void wdg_finish(void)
{
}

void wdg_feed(void)
{
    hal_boot_feed_dg();
}

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    unsigned int param;

    if (wdg == NULL) {
        return -1;
    }

    param = wdg->config.timeout / 1000;

    hal_boot_wdg_init(param);

    return 0;
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    hal_boot_feed_dg();
}



