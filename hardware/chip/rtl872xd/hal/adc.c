#include <stdint.h>
#include "analogin_api.h"
#include "aos/hal/adc.h"

static analogin_t hal_adc[8];

static u32 adc_pin2chan[8][2]={
		{PB_4,			0},
		{PB_5,			1},
		{PB_6,			2},
		{PB_7,			3},
		{PB_1,			4},
		{PB_2,			5},
		{PB_3,			6},
		{VBAT_MEAS,		7}
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
	uint32_t startcount = SYSTIMER_TickGet();
	analogin_t *adc_t = adc->priv;
	uint8_t  ChIdx = adc_t->adc_idx;
	uint32_t data;

	/* Set channel index into channel switch list*/
	ADC->ADC_CHSW_LIST[0] = ChIdx;

	/* Clear FIFO */
	ADC_ClearFIFO();

	/* SW trigger to sample */
	ADC_SWTrigCmd(ENABLE);
	while(1){
		if(ADC_Readable())
			break;
		if(SYSTIMER_GetPassTime(startcount)>timeout)
			return -1;
	}
	
	ADC_SWTrigCmd(DISABLE);

	data = ADC_Read();

	*(uint16_t*)output=(uint16_t*)(data & BIT_MASK_DAT_GLOBAL);

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
