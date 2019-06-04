#include "hal/soc/soc.h"
#include "rtos_pub.h"
#include "drv_model_pub.h"
#include "saradc_pub.h"

typedef struct {
    uint16_t *pData;
    uint8_t data_buff_size;
    /* mode:     ADC mode
     * bit[0:1]: ADC operation mode
     *          00:  ADC power down mode
     *          01:  ADC one-step mode
     *          10:  ADC software control mode
     *          11:  ADC continuous mode
     * bit[3:2]: ADC sample rate selection
     *          00: adc_clk/36
     *          01: adc_clk/18
     *          10: Reserved
     *          11: Reserved
     * bit[7:4]: reserved
     */
    uint8_t mode;
    void (*p_Int_Handler)(void);
} adc_priv_config_t;
static saradc_desc_t adc_desc;
static DD_HANDLE adc_hdl;

int32_t hal_adc_init(adc_dev_t *adc)
{
	adc_priv_config_t *adc_priv_cfg = (adc_priv_config_t*)adc->priv;
	uint32_t status;
	int32_t ret = 0;

	adc_desc.channel = adc->port;
	adc_desc.pData = adc_priv_cfg->pData;
	adc_desc.data_buff_size = adc_priv_cfg->data_buff_size;
	adc_desc.mode = adc_priv_cfg->mode;
    adc_desc.p_Int_Handler = adc_priv_cfg->p_Int_Handler;

	adc_hdl = ddev_open(SARADC_DEV_NAME, &status, (uint32_t)&adc_desc);
	if(status)
	{
		ret = -1;
	}

	return ret;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
	uint8_t read_cnt = *(uint8_t *)adc->priv;
	int32_t ret = -1;

	if(adc_desc.has_data && adc_desc.current_sample_data_cnt > read_cnt)
	{
		*(uint32_t*)output = adc_desc.pData[read_cnt];
		adc_desc.has_data = 0;
		adc_desc.current_sample_data_cnt = 0;
		ret = 0;
	}

	return ret;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
	uint8_t param;

	param = 0;
	ddev_control(adc_hdl, SARADC_CMD_RUN_OR_STOP_ADC, (void *)&param);
	ddev_close(adc_hdl);

	return 0;
}

