#ifndef _HAL_ADC_H_
#define _HAL_ADC_H_

#include "sys/intc.h"   // components/

typedef enum{
    ADC_EXT_VOLTAGE_GPIO_GPIO22 = 0,   
    ADC_EXT_VOLTAGE_GPIO_GPIO21 = 1,   
    ADC_EXT_VOLTAGE_GPIO_GPIO20 = 2,   
    ADC_EXT_VOLTAGE_GPIO_GPIO00 = 3    
}ADC_EXT_VOLTAGE_GPIO;

#if defined(CONFIG_OS_RHINO)
void _hal_adc_init (uint8_t aio_source);
#else
void hal_adc_init (uint8_t aio_source);
#endif

void hal_adc_deinit (uint8_t aio_source);

void hal_adc_read (uint16_t *value);

#endif  // #ifndef _HAL_ADC_H_
