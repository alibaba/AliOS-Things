#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_wdg.h"

void hw_watchdog_unlock(void)
{
    WATCHDOG->LOCK = WDG_LOCK_TOKEN;
}

void hw_watchdog_lock(void)
{
    WATCHDOG->LOCK = ~(WDG_LOCK_TOKEN);
}

void hw_watchdog_disable(void)
{
    hw_watchdog_unlock();
    WATCHDOG->CONTROL = 0x0;
    WATCHDOG->LOAD = 0x0;
    hw_watchdog_lock();
}

void hw_watchdog_isr(unsigned int delay)
{
    hw_watchdog_unlock();
    WATCHDOG->CONTROL = WDG_INTEN;
    WATCHDOG->LOAD = delay;
    hw_watchdog_lock();
}

void hw_watchdog_reset(unsigned int delay)
{
    hw_watchdog_unlock();
    WATCHDOG->CONTROL = WDG_RESEN | WDG_INTEN;
    WATCHDOG->LOAD = delay;
    hw_watchdog_lock();
}

void hw_watchdog_isr_clr(void)
{
    hw_watchdog_unlock();
    WATCHDOG->INTCLR = 0x1;
    hw_watchdog_lock();
}

void WDG_IRQHandler(void)
{
    lega_intrpt_enter();
    lega_wdg_dev_t lega_wdg_dev;
    lega_wdg_dev.port = 0;
    lega_wdg_reload(&lega_wdg_dev);
    lega_intrpt_exit();
}

/**
 * This function will initialize the on board CPU hardware watch dog
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_wdg_init(lega_wdg_dev_t *wdg)
{
    uint32_t reg_value;
    if(NULL == wdg)
    {
        return -1;
    }
    if(0 == wdg->port)
    {
        // Set WDG Clock Enable
        reg_value = REG_RD(PERI_CLK_CFG);
        REG_WR(PERI_CLK_CFG, (reg_value|WDG_CLK_EN));
        hw_watchdog_reset(wdg->config.timeout * (SYSTEM_CLOCK / 1000 / 2)); //1000 for ms, 2 for watchdog feature
        NVIC_EnableIRQ(WDG_IRQn);
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * Reload watchdog counter.
 *
 * @param[in]  wdg  the watch dog device
 */
void lega_wdg_reload(lega_wdg_dev_t *wdg)
{
    if(NULL == wdg)
    {
        return;
    }
    if(0 == wdg->port)
    {
        hw_watchdog_isr_clr();
    }
}

/**
 * This function performs any platform-specific cleanup needed for hardware watch dog.
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_wdg_finalize(lega_wdg_dev_t *wdg)
{
    uint32_t reg_value;
    if(NULL == wdg)
    {
        return -1;
    }
    if(0 == wdg->port)
    {
        // Set WDG Clock Disable
        reg_value = REG_RD(PERI_CLK_CFG);
        REG_WR(PERI_CLK_CFG, (reg_value&(~WDG_CLK_EN)));
        NVIC_DisableIRQ(WDG_IRQn);
        hw_watchdog_disable();
        return 0;
    }
    else
    {
        return -1;
    }
}

