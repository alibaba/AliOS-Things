/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file hal/soc/adc.h
 * @brief adc HAL
 * @version since 5.5.0
 */

#include "aos/hal/adc.h"
#include "driver/chip/hal_adc.h"
#include "aos/hal/adc_priv.h"

/**@biref Initialises an ADC interface
 *
 * Prepares an ADC hardware interface for sampling
 *
 * @param     adc   : the interface which should be initialised
 *
 * @return    0     : on success.
 * @return    EIO   : if an error occurred with any step
 */
int32_t hal_adc_init(adc_dev_t *adc)
{
	HAL_Status status = HAL_ERROR;
	adc_priv_config_t *adc_priv_cfg = (adc_priv_config_t*)adc->priv;

	status = HAL_ADC_ConfigChannel(adc->port, ADC_SELECT_ENABLE,
						adc_priv_cfg->irqmode, adc_priv_cfg->irqlow, adc_priv_cfg->irqhigh);
	if (status != HAL_OK) {
		printf("[hal_adc]: ADC ch %d config error %d\n", adc->port, status);
		return -1;
	}

	status = HAL_ADC_EnableIRQCallback(adc->port, adc_priv_cfg->irq_callback, adc_priv_cfg->arg);
	if (status != HAL_OK) {
		printf("[hal_adc]: ADC ch %d IRQ Enable error %d\n", adc->port, status);
		return -1;
	}

	return 0;
}


/**@biref Takes a single sample from an ADC interface
 *
 * Takes a single sample from an ADC interface
 *
 * @param  adc      : the interface which should be sampled
 * @param  output   : pointer to a variable which will receive the sample
 * @param  timeout  : ms timeout
 * @return 0        : on success.
 * @return EIO      : if an error occurred with any step
 */
int32_t hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
	*(uint32_t*)output = HAL_ADC_GetValue(adc->port);
	return 0;
}


/**@biref     De-initialises an ADC interface
 *
 * @abstract Turns off an ADC hardware interface
 *
 * @param  adc : the interface which should be de-initialised
 *
 * @return 0   : on success.
 * @return EIO : if an error occurred with any step
 */
int32_t hal_adc_finalize(adc_dev_t *adc)
{
	HAL_Status status = HAL_ERROR;

	status = HAL_ADC_ConfigChannel(adc->port, ADC_SELECT_DISABLE, 0, 0, 0);
	if (status != HAL_OK) {
		printf("[hal_adc]: ADC ch config error %d\n", status);
	}

	status = HAL_ADC_DisableIRQCallback(adc->port);
	if (status != HAL_OK) {
		printf("[hal_adc]: ADC ch cb disable error %d\n", status);
	}
	return 0;
}

