/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/adc.h"
#include "board.h"

static uint32_t eadc_modinit_mask = 0;

static const struct nu_modinit_s adc_modinit_tab[] = {
    {ADC_0_0, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_1, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_2, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_3, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_4, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_5, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_6, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_7, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_8, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_9, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_10, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_11, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_12, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_13, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_14, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
    {ADC_0_15, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC0_IRQn, NULL},
};

static int32_t platform_analogin_init(struct analogin_s *obj, PinName pin )
{
		struct nu_modinit_s *modinit;
	
		if ( obj->adc != (ADCName) pinmap_peripheral(pin, PinMap_ADC))
			goto exit_platform_analogin_init;

		if ( (modinit = get_modinit(obj->adc, adc_modinit_tab)) == NULL )
			goto exit_platform_analogin_init;

		if ( modinit->modname != (int) obj->adc )
			goto exit_platform_analogin_init;

    EADC_T *eadc_base = (EADC_T *) NU_MODBASE(obj->adc);

    // NOTE: All channels (identified by ADCName) share a ADC module. This reset will also affect other channels of the same ADC module.
    if (! eadc_modinit_mask) {
        // Reset this module if no channel enabled
        SYS_ResetModule(modinit->rsetidx);

        // Select clock source of paired channels
        CLK_SetModuleClock(modinit->clkidx, modinit->clksrc, modinit->clkdiv);
        // Enable clock of paired channels
        CLK_EnableModuleClock(modinit->clkidx);

        // Set the ADC internal sampling time, input mode as single-end and enable the A/D converter
        EADC_Open(eadc_base, EADC_CTL_DIFFEN_SINGLE_END);
    }

    uint32_t chn =  NU_MODSUBINDEX(obj->adc);

    // Wire pinout
    pinmap_pinout(pin, PinMap_ADC);

    // Configure the sample module Nmod for analog input channel Nch and software trigger source
    EADC_ConfigSampleModule(eadc_base, chn, EADC_SOFTWARE_TRIGGER, chn);

    eadc_modinit_mask |= 1 << chn;

		return HAL_OK;
		
exit_platform_analogin_init:
		
		return HAL_ERROR;
}

static void platform_analogin_finalize(struct analogin_s *obj )
{
	uint32_t chn =  NU_MODSUBINDEX(obj->adc);
	eadc_modinit_mask &= ~(1 << chn);
	if ( !eadc_modinit_mask )
	{
		EADC_T *eadc_base = (EADC_T *) NU_MODBASE(obj->adc);
		EADC_Close(eadc_base);
	}	
}

static struct analogin_s* hal_get_analogin_s ( adc_dev_t *adc )
{
	if ( !(adc) || (adc->port >= i32BoardMaxADCNum) )
		goto exit_hal_get_analogin_s;

	return (struct analogin_s*)&board_analogin[adc->port];

exit_hal_get_analogin_s:
	return NULL;
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
	struct analogin_s *obj = hal_get_analogin_s(adc);
	
	if ( !obj )
		goto exit_hal_adc_init;
	
	return platform_analogin_init(obj, obj->pin );
	
exit_hal_adc_init:	
	return HAL_ERROR;
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
	struct analogin_s *obj = hal_get_analogin_s(adc);
  uint16_t *value = (uint16_t *)output;

	if ( !obj )
		goto exit_hal_adc_value_get;

  uint32_t chn =  NU_MODSUBINDEX(obj->adc);

	if ( !eadc_modinit_mask & (1 << chn) )

		goto exit_hal_adc_value_get;

	else 
	{
		EADC_T *eadc_base = (EADC_T *) NU_MODBASE(obj->adc);	
		EADC_START_CONV(eadc_base, 1 << chn);
		while (EADC_GET_DATA_VALID_FLAG(eadc_base, 1 << chn) != ((uint32_t) (1 << chn)));
		uint16_t conv_res_12 = EADC_GET_CONV_DATA(eadc_base, chn);
		// Just 12 bits are effective. Convert to 16 bits.
		// conv_res_12: 0000 b11b10b9b8 b7b6b5b4 b3b2b1b0
		// conv_res_16: b11b10b9b8 b7b6b5b4 b3b2b1b0 b11b10b9b8
		*value = (conv_res_12 << 4) | (conv_res_12 >> 8);	
	}
	
	return HAL_OK;

exit_hal_adc_value_get:

	return HAL_ERROR;
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
	struct analogin_s *obj = hal_get_analogin_s(adc);

	if ( !obj )
		goto exit_hal_adc_finalize;

	platform_analogin_finalize(obj);
	
	return HAL_OK;

exit_hal_adc_finalize:
	
	return HAL_ERROR;
}
