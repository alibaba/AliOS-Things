#ifndef _DRV_ADC_H_
#define _DRV_ADC_H_
#include "stdint.h"
#include "hal_adc.h"


//# adc convert formula : -0.9792*code +1014
#define ADC_CONVERT_SLOPE   9792
#define ADC_CONVERT_OFFSET  10140000

/**
 * Initialize the ADC hardware.
 * @param  pin                      Initialize with that ADC pin.
 * @retval   0                      Initialize success.
 * @retval  -1                      The ADC pin is unavailable.
 */

int8_t drv_adc_init (ADC_EXT_VOLTAGE_GPIO pin);

/**
 * Deinitialize the ADC hardware.
 * @param  pin                      Deinitialize with that ADC pin.
 * @retval   0                      Deinitialize success.
 * @retval  -1                      The ADC pin is unavailable.
 */

int8_t drv_adc_deinit (ADC_EXT_VOLTAGE_GPIO pin);

/**
 * Read out the ADC value(10bit).
 * @param  value                    Pointer to the value to store the adc code from ADC pin.
 * @retval   0                      ADC read out success. 
 * @retval  -1                      The pointer to the value is unavailable. 
 */

int8_t drv_adc_read (uint16_t *value);

#endif
