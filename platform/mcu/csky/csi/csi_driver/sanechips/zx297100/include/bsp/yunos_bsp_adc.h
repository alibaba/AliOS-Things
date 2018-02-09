#ifndef YUNOS_BSP_ADC_H
#define YUNOS_BSP_ADC_H

/**
 * initialization adc module
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_init(uint8_t *count);

/**
 * This function is used for config process_ctrl reg
 * @param[in]    select FIFO channel
 * @param[in]    config FIFO deep
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_ctrl_config(uint32_t mask,uint32_t pos,uint32_t value);

/**
 * This function is used for reset adc module
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_rst(void);

/**
 * This function is used for enable adc module
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_enbale(void);

/**
 * This function is used forenable adc module
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_disable(void);

/**
 * This function is used for ADC channel sort
 * @param[in]  channel number of ADC
 * @param[in]  sequence value for the ADC channel
 * @return	   0-success, other-error
 */
int yunos_bsp_adc_channel_sort(uint8_t channel,uint32_t sequence);

/**
 * This function is used for set ADC sample rate
 * @param[in]  channel number of ADC
 * @param[in]  rate value for the ADC channel
 * @return	   0-success, other-error
 */
int yunos_bsp_adc_sample_rate(uint8_t channel,uint32_t rate);

/**
 * This function is used for DMA config
 * @param[in]    select DMA channel
 * @param[in]    enable or disbale DMA
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_dma_config(uint8_t dma_ch,uint32_t enbale);

/**
 * This function is used for config fifo
 * @param[in]    select FIFO channel
 * @param[in]    config FIFO deep
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_fifo_config(uint32_t fifo_ch,uint32_t deep);

/**
 * This function is used for fet fifo status
 * @param[in]    select FIFO channel
 * @param[in]    config FIFO deep
 * @return		 0-success, other-error
 */
uint32_t yunos_bsp_adc_get_fifo_status(void);

/**
 * This function is used for ADC trigger source select
 * @param[in]    the num trigger source
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_trigger_sel(uint8_t num);

/**
 * This function is used for config ADC ext trigger source edge
 * @param[in]    the num trigger edge
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_trigger_edge(uint32_t edge);

/**
 * This function is used for sync ADC regs 
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_sync(void);

/**
 * This function is used for set ADC work mode
 * @param[in]    the work mode
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_set_mode(uint32_t mode );

/**
 * This function is used for start ADC with software
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_start_soft(void );

/**
 * This function is used for enable or disable ADC int 
 * @param[in]    int config value
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_set_int(uint32_t para);

/**
 * This function is used for get ADC int 
 * @param[out]   int status value
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_get_int_status(uint32_t *para);

/**
 * This function is used for read ADC data0  
 * @param[in]    adc channel
 * @param[out]   adc value
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_read_noirq(uint32_t channel, uint32_t  *value);

/**
*  ADC interrupt service routine
*
* @param[in]   irq			irq number
* @param[in]   context		argument
* @return	   0-success , other-error code. see <errno.h>
*/

/**
 * uninitialization adc module
 * @return		0-success, other-error
 */
int yunos_bsp_adc_uninit(void);

/**
 * rst & enabel adc clk
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_clk_enable(void);

/**
 * init adc register state and enable adc module
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_clk_disable(void);

/**
 * init adc register state and enable adc module
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_open(void);

/**
 * This function is used for close adc module
 * @param[in]    the work mode
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_close(void);

/**
 * This function is used for read ADC data0  
 * @param[in]    adc channel
 * @param[out]   adc value
 * @return		 0-success, other-error
 */
int yunos_bsp_adc_read_irq(uint32_t channel, uint32_t  *value , const struct adc_si_dev_config *inst);


#endif
