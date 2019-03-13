#include "hal/soc/adc.h"
#include <stdint.h>
#include "rda5981.h"
#include "pinmap.h"
#include "PeripheralNames.h"
#include "pinnames.h"

static const PinMap PinMap_ADC[] = {
    {PB_6, ADC0_0, 0},
    {PB_8, ADC0_1, 0},
    {NC,   NC,     0}
};
static int adc_inited_cnt = 0;

int32_t hal_adc_init(adc_dev_t *adc)
{
    PinName pin;
    if(adc->port == 0)
        pin = PB_6;
    else if(adc->port == 1)
        pin = PB_8;
    unsigned char gp = (unsigned char)(pin & 0x00FFU);
    /* Init ADC */
    if(0 == adc_inited_cnt) {
        rda_ccfg_adc_init();
    }
    adc_inited_cnt++;

    rda_ccfg_gp(gp, 0x00U);
    rda_ccfg_adc_gp(gp, 0x00U);
    rda_ccfg_adc_oenb(adc->port, 0x01U);

    pinmap_pinout(pin, PinMap_ADC);
    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    *(uint16_t *) output = rda_ccfg_adc_read((unsigned char)(adc->port));
    return 0;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    unsigned char gp;
    PinName pin;
    adc_inited_cnt--;
    if(0 == adc_inited_cnt) {
        rda_ccfg_adc_free();
    }

    if(adc->port == 0)
        pin = PB_6;
    else if(adc->port == 0)
        pin = PB_8;
    gp = (unsigned char)(pin & 0x00FFU);

    rda_ccfg_adc_gp(gp, 0x01U);
    rda_ccfg_gp(gp, 0x01U);

    return 0;
}

