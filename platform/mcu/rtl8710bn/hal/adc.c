#include <stdint.h>
#include "analogin_api.h"
#include "aos/hal/adc.h"
#include "PinNames.h"
#include "board.h"

static analogin_t hal_adc[PORT_ADC_MAX];

/* see platform_adc_peripherals */
static u32 adc_pin2chan[PORT_ADC_MAX][2]={
    {AD_1,          0},
    {AD_2,          1},
    {AD_3,          2},
};

/**
 * Initialises an ADC interface, Prepares an ADC hardware interface for sampling
 *
 * @param[in]  adc  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_init(adc_dev_t *adc)
{
    adc->priv = &hal_adc[adc->port];

    analogin_init(adc->priv, adc_pin2chan[adc->port][0]);

    return 0;
}

/**
 * Takes a single sample from an ADC interface
 *
 * @param[in]   adc      the interface which should be sampled
 * @param[out]  output   pointer to a variable which will receive the sample
 * @param[in]   timeout  ms timeout
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    float fval;
    analogin_t *adc_t = adc->priv;

    fval = analogin_read(adc_t);

    *(uint32_t*)output = (uint32_t)fval;

    #if 0
    uint8_t  ChIdx = adc_t->adc_idx;
    uint32_t data;

    /* Set channel index into channel switch list*/
    ADC->ADC_CHSW_LIST[0] = ChIdx;

    ADC_Cmd(ENABLE)

    data = ADC_Read();

    *(uint32_t*)output= data;
    #endif

    return 0;
}

/**
 * De-initialises an ADC interface, Turns off an ADC hardware interface
 *
 * @param[in]  adc  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_finalize(adc_dev_t *adc)
{
    analogin_deinit(adc->priv);

}
