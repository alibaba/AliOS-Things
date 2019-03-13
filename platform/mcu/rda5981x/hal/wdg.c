#include "hal/soc/wdg.h"
#include <sys/errno.h>
#include <stddef.h>
#include "rda_ccfg_api.h"
#include "rda5981.h"
#include "PeripheralNames.h"

/** WDT HAL macro
 */
#define WDT_EN_BIT                  (9)
#define WDT_CLR_BIT                 (10)
#define WDT_TMRCNT_OFST             (11)
#define WDT_TMRCNT_WIDTH            (4)

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    uint32_t reg = 0;
    uint8_t to = wdg->config.timeout;
    RDA_WDT_TypeDef * wdt = NULL;
    rda_ccfg_wdt_en();

    if(0x00U != (to & 0xF0U))
        return EIO;
    wdt = (RDA_WDT_TypeDef *)WDT_0;
    reg = wdt->WDTCFG & ~(((0x01UL << WDT_TMRCNT_WIDTH) - 0x01UL) << WDT_TMRCNT_OFST);
    wdt->WDTCFG = reg | (to << WDT_TMRCNT_OFST);
    wdt->WDTCFG |= (0x01UL << WDT_EN_BIT);//start wdt
    wdg->priv = (void*)wdt;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    RDA_WDT_TypeDef * wdt = (RDA_WDT_TypeDef *)(wdg->priv);
    wdt->WDTCFG |= (0x01UL << WDT_CLR_BIT);
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    RDA_WDT_TypeDef * wdt = (RDA_WDT_TypeDef *)(wdg->priv);
    wdt->WDTCFG &= ~(0x01UL << WDT_EN_BIT);
    return 0;
}

