#include "adc/drv_adc.h"
#include "adc/hal_adc.h"
#include "gpio/hal_gpio.h"
#include <hal/soc/adc.h>
#include "errno.h"

int32_t hal_adc_init(adc_dev_t *adc) {
    //return (drv_adc_init(adc->port) < 0) ? EIO : 0;
    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout) {
    uint16_t buf = 0;
    int ret;
    if (drv_adc_init(adc->port) < 0) {
        return EIO;
    }
    if (drv_adc_read(&buf) < 0) {
        drv_adc_deinit(adc->port);
        return EIO;
    }
    *(uint32_t *)output = (uint32_t)((ADC_CONVERT_OFFSET - (buf)*(ADC_CONVERT_SLOPE))/10000);
    if (drv_adc_deinit(adc->port) < 0) {
        return EIO;
    }
    return 0;
}

int32_t hal_adc_finalize(adc_dev_t *adc) {
    return 0;
}

