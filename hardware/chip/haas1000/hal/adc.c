/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/adc.h"
#include "hal_trace.h"
#include "hal_gpadc.h"

static void __hal_adc_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
	//printf("%s irq_val=%d, volt=%d\n", __FUNCTION__, irq_val, volt);
}

static enum HAL_GPADC_CHAN_T __hal_adc_port2chan(uint8_t port)
{
	return (port<3) ? port+HAL_GPADC_CHAN_2 : HAL_GPADC_CHAN_QTY;
}

/**
 * Initialises an ADC interface, Prepares an ADC hardware interface for sampling
 *
 * @param[in]  adc  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_init(adc_dev_t *adc)
{
	//printf("%s port=%d, sampling_cycle=%d, priv=%p\n", __FUNCTION__, adc->port, adc->config.sampling_cycle, adc->priv);
	if (adc->config.sampling_cycle) {
		hal_gpadc_open(__hal_adc_port2chan(adc->port), HAL_GPADC_ATP_NULL, NULL);
	} else {
		hal_gpadc_open(__hal_adc_port2chan(adc->port), HAL_GPADC_ATP_ONESHOT, __hal_adc_irqhandler);
	}
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
	HAL_GPADC_MV_T volt = HAL_GPADC_BAD_VALUE;

    hal_gpadc_open(__hal_adc_port2chan(adc->port), HAL_GPADC_ATP_ONESHOT, NULL);
    osDelay(1);

    if (output && hal_gpadc_get_volt(__hal_adc_port2chan(adc->port), &volt)) {
        *output = volt;
        /*printf("%s port=%d, sampling_cycle=%d, priv=%p => output=%d\r\n", __FUNCTION__, adc->port, adc->config.sampling_cycle, adc->priv, *output);*/
    }
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
	//printf("%s port=%d, sampling_cycle=%d, priv=%p\n", __FUNCTION__, adc->port, adc->config.sampling_cycle, adc->priv);
	hal_gpadc_close(__hal_adc_port2chan(adc->port));
	return 0;
}
