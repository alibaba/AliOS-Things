#ifndef YUNOS_BSP_ADC_H
#define YUNOS_BSP_ADC_H

uint32_t yunos_bsp_dac_get_data(void);

/**
 * This function is used for clear dac int 
 * @param    	 int_type:DATA_UNDERRUN, DATA_NULL
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_int_clear(enum int_source int_type);

/**
 * This function is used for init DAC  
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_init(void);

/**
 * This function is used for uninitialization DAC module 
 * @return		0-success, other-error
 */
int yunos_bsp_dac_uninit(void);

/**
 * This function is used for disbale  dac  clk 
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_clk_disable(void);

/**
 * This function is used for enable or disbale 
DAC module
 * @param[in]    ENABLE or DIASBLE
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_enable(uint32_t enable);

/**
 * This function is used for sync DAC regs value to CLK
 * it should be call everytime when modify ady DAC regs value
 * @return		 0-success, other-error
 */

int yunos_bsp_dac_sync(void);
/**
 * This function is used for software start DAC transleate
 * @return		 0-success, other-error
 */

int yunos_bsp_dac_start(void);

/**
 * This function is used for enable or diaable dac int 
 * @param    	 int_type:DATA_UNDERRUN, DATA_NULL
 * @param    	 enable: ENABLE ,DISABLE
 * @return		 0-success, other-error
 */

int yunos_bsp_dac_int_enable(enum int_source int_type,uint32_t enable);

/**
 * This function is used for get dac int status
 * @param    	 int_type:DATA_UNDERRUN, DATA_NULL
 * @return		 0-int not active, 1-int active
 */

int yunos_bsp_dac_get_int_status(enum int_source int_type);
/**
 * This function is used for get dac module status
 * @return		 reg DAC_SW
 */
int yunos_bsp_dac_get_status(void);

/**
 * This function is used for set dac  out & sample & hold num
 * @param[in]    		out: (num+1)Twclk 
 * @param[in]			sample: (num+1)Twclk 
 * @param[in]			hold: (num+1)Twclk 
 * @return		 		0-success, other-error
 */

int yunos_bsp_dac_set_num(uint32_t out,uint32_t sample,uint32_t hold);
/**
 * This function is used for set dac trigger type
 * @param[in]    value for reg DAC_TRG_TYP
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_set_trigger(uint32_t tirgger);
/**
 * This function is used for set dac trigger type
 * @param        start or stop
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_start_trigger(uint32_t enable);

/**
 * This function is used for set dac module work mode
 * @param  mode
 *   				NORMAL_MODE 	
 *  				LOPOWER_MODE  
 *  				TRIANGLE_MODE 
 *  				NOISE_MODE   
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_set_mode(enum adc_mode mode );
/**
 * This function is used for set dac width
 * @param   width
 *    				WIDTH_12BIT 	
 *   				WIDTH_8BIT 	
 * @return	0-success, other-error
 */
int yunos_bsp_dac_set_width( enum adc_width width );
/**
 * This function is used for enabel or disable DAC trigger 
 *
 * @param    	 enable:ENABLE , DISABLE
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_trigger_enable (uint32_t enable );
/**
 * This function is used for enable  dac  clk
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_clk_enable(void);
/**
 * This function is used for  init dac 
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_open(void);

/**
 * This function is used for close adc module
 *
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_close(void);

/**
 * This function is used for user get adc date wihtout irq
 *
 * @param    	 none
 * @return		 0-success, other-error
 */
int yunos_bsp_dac_trans(uint32_t  data);


#endif
