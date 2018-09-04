#include <stdio.h>
#include "hal/soc/soc.h"
#include "driver_adc.h"

extern ADC_DRV_DESC* sg_p_adc_drv_desc;

ADC_DRV_DESC  adc_init_l= {NULL,0,0,0,1,3,NULL,0,18,0};

int32_t hal_adc_init(adc_dev_t *adc)
{
    int32_t ret = 0;
    sg_p_adc_drv_desc = &adc_init_l;
    adc_init(sg_p_adc_drv_desc);

    return ret;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    static int32_t ret = 0;
    if(*((unsigned int*)REG_ADC_BASE_ADDR) & ADC_CONFIG_BUSY_MASK)
    {
        return ret;
    }
    else
    {
        ret = (int32_t)(REG_ADC_DATA & 0x3ff);
    }
    return ret;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    int32_t ret = 0;

    adc_disable();

    return ret;
}