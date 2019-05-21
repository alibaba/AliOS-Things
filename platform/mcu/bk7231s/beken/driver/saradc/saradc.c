#include "include.h"
#include "arm_arch.h"

#include "saradc.h"
#include "saradc_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"

#include "icu_pub.h"
#include "gpio_pub.h"

#include "uart_pub.h"
#include "mcu_ps_pub.h"

#include <string.h>

#if CFG_SUPPORT_ALIOS
#include "ll.h"
#endif

saradc_desc_t *saradc_desc = NULL;

saradc_calibrate_val saradc_val = {0x55, 0x354};

static DD_OPERATIONS saradc_op = {
            saradc_open,
            saradc_close,
            NULL,
            NULL,
            saradc_ctrl
};

static void saradc_int_clr(void);

void saradc_init(void)
{
	intc_service_register(IRQ_SARADC, PRI_IRQ_SARADC, saradc_isr); 

	ddev_register_dev(SARADC_DEV_NAME, &saradc_op);
}

void saradc_exit(void)
{
	ddev_unregister_dev(SARADC_DEV_NAME);
}

static void saradc_enable_icu_config(void)
{
    UINT32 param;
    param = PWD_SARADC_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
}

static void saradc_disable_icu_config(void)
{
    UINT32 param;
    param = PWD_SARADC_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
}

static void saradc_enable_interrupt(void)
{
    UINT32 param;    
    param = (IRQ_SARADC_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void saradc_disable_interrupt(void)
{
    UINT32 param;    
    param = (IRQ_SARADC_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

static void saradc_gpio_config(void)
{
	UINT32 param;
    
    switch (saradc_desc->channel)
    {
        case 1:
        {
            param = GFUNC_MODE_ADC1;
	        sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
            break;
        }
        case 2:
        {
            param = GFUNC_MODE_ADC2;
	        sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
            break;
        }
        case 3:
        {
            param = GFUNC_MODE_ADC3;
	        sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
            break;
        }

#if ((SOC_BK7231U == CFG_SOC_NAME) || (SOC_BK7221U == CFG_SOC_NAME))
        case 4:
        {
            param = GFUNC_MODE_ADC4;
	        sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
            break;
        }
        case 5:
        {
            param = GFUNC_MODE_ADC5;
	        sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
            break;
        }
        case 6:
        {
            param = GFUNC_MODE_ADC6;
	        sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
            break;
        }
        case 7:
        {
            param = GFUNC_MODE_ADC7;
	        sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
            break;
        }
#endif  // ((SOC_BK7231U == CFG_SOC_NAME) || (SOC_BK7221U == CFG_SOC_NAME))
        
        default:    
            break;
    }

}

static UINT32 saradc_open(UINT32 op_flag)
{
	UINT32 value = 0;

    saradc_desc = (saradc_desc_t*)op_flag;

	if(saradc_desc->pData == NULL){
		return SARADC_FAILURE;
	}

	if(saradc_desc->channel > SARADC_ADC_CHNL_MAX){
		return SARADC_FAILURE;
	}

	if(saradc_desc->data_buff_size == 0){
		return SARADC_FAILURE;
	}

	saradc_enable_icu_config();
    
	saradc_desc->has_data = 0;
    saradc_desc->current_read_data_cnt = 0;
    saradc_desc->current_sample_data_cnt = 0;

	saradc_gpio_config();

    // clear fifo
    while((value & SARADC_ADC_FIFO_EMPTY) == 0) {
        REG_READ(SARADC_ADC_DATA);
        value = REG_READ(SARADC_ADC_CONFIG);
    }
    saradc_int_clr();

    value = ((saradc_desc->mode & SARADC_ADC_MODE_MASK) << SARADC_ADC_MODE_POSI)
        | SARADC_ADC_CHNL_EN
        | ((saradc_desc->channel & SARADC_ADC_CHNL_MASK) << SARADC_ADC_CHNL_POSI)
        | (((saradc_desc->mode>>2)&0x01) << SARADC_ADC_DELAY_CLK_POSI)
        | SARADC_ADC_INT_CLR
        | ((saradc_desc->pre_div & SARADC_ADC_PRE_DIV_MASK) << SARADC_ADC_PRE_DIV_POSI)
        | ((saradc_desc->samp_rate & SARADC_ADC_SAMPLE_RATE_MASK) << SARADC_ADC_SAMPLE_RATE_POSI)
        | ((saradc_desc->filter & SARADC_ADC_FILTER_MASK)<< SARADC_ADC_FILTER_POSI);
    REG_WRITE(SARADC_ADC_CONFIG, value);

#if (CFG_SOC_NAME != SOC_BK7231)    
    value = SARADC_ADC_SAT_ENABLE 
        | ((0x01 & SARADC_ADC_SAT_CTRL_MASK) << SARADC_ADC_SAT_CTRL_POSI);
    REG_WRITE(SARADC_ADC_SATURATION_CFG, value);
#endif
    
    saradc_enable_interrupt();

	return SARADC_SUCCESS;
}

static UINT32 saradc_close(void)
{
	UINT32 value;
    
    saradc_disable_interrupt();
#if 1
	value = REG_READ(SARADC_ADC_CONFIG);
    value &= ~(SARADC_ADC_MODE_MASK << SARADC_ADC_MODE_POSI);
    value &= ~(SARADC_ADC_CHNL_EN); 
    value |= SARADC_ADC_INT_CLR;
	REG_WRITE(SARADC_ADC_CONFIG, value);

    saradc_disable_icu_config();
#endif
    // clear fifo
    value = REG_READ(SARADC_ADC_CONFIG);
    while((value & SARADC_ADC_FIFO_EMPTY) == 0) {
        REG_READ(SARADC_ADC_DATA);
        value = REG_READ(SARADC_ADC_CONFIG);
    }
    saradc_int_clr();

	return SARADC_SUCCESS;
}

static UINT32 saradc_set_mode(UINT8 mode)
{
	UINT32 value;
	
	if(mode > 3){
		return SARADC_FAILURE;
	}

	value = REG_READ(SARADC_ADC_CONFIG);
	value &= ~(SARADC_ADC_MODE_MASK << SARADC_ADC_MODE_POSI);
	value |= (mode << SARADC_ADC_MODE_POSI);
	REG_WRITE(SARADC_ADC_CONFIG, value);

	return SARADC_SUCCESS;
}

static UINT32 saradc_set_channel(saradc_chan_t *p_chan)
{
	UINT32 value;

	value = REG_READ(SARADC_ADC_CONFIG);
	if(p_chan->enable == 0){
		value &= ~SARADC_ADC_CHNL_EN;
	}else{
		value &= ~(SARADC_ADC_CHNL_MASK << SARADC_ADC_CHNL_POSI);
		value |= (p_chan->channel << SARADC_ADC_CHNL_POSI);
		value |= SARADC_ADC_CHNL_EN;
	}
	REG_WRITE(SARADC_ADC_CONFIG, value);

	return SARADC_SUCCESS;
}

static UINT32 saradc_set_sample_rate(UINT8 rate)
{
	UINT32 value;

	if(rate > 3){
		return SARADC_FAILURE;
	}
	
	value = REG_READ(SARADC_ADC_CONFIG);
	value &= ~(SARADC_ADC_SAMPLE_RATE_MASK << SARADC_ADC_SAMPLE_RATE_POSI);
	value |= (rate << SARADC_ADC_SAMPLE_RATE_POSI);
	REG_WRITE(SARADC_ADC_CONFIG, value);

	return SARADC_SUCCESS;
}

static UINT32 saradc_set_waiting_time(UINT8 time)
{
	UINT32 value, mode;

	value = REG_READ(SARADC_ADC_CONFIG);

	mode = value & (SARADC_ADC_MODE_MASK << SARADC_ADC_MODE_POSI);
	if(mode == 0 || mode == 3){
		return SARADC_FAILURE;
	}
	
	if(time == 0){
		value &= ~SARADC_ADC_SETTING;
	}else{
		value |= SARADC_ADC_SETTING;
	}
	
	REG_WRITE(SARADC_ADC_CONFIG, value);

	return SARADC_SUCCESS;
}

static UINT32 saradc_set_valid_mode(UINT8 mode)
{

	return SARADC_SUCCESS;
}

static void saradc_int_clr(void)
{
	UINT32 value;

	value = REG_READ(SARADC_ADC_CONFIG);
	value |= SARADC_ADC_INT_CLR;
	REG_WRITE(SARADC_ADC_CONFIG, value);
}

static UINT32 saradc_set_clk_rate(UINT8 rate)
{
	UINT32 value;

	if(rate > SARADC_ADC_PRE_DIV_MASK){
		return SARADC_FAILURE;
	}

	value = REG_READ(SARADC_ADC_CONFIG);
	value &= ~(SARADC_ADC_PRE_DIV_MASK << SARADC_ADC_PRE_DIV_POSI);
	value |= (rate << SARADC_ADC_PRE_DIV_POSI);
	REG_WRITE(SARADC_ADC_CONFIG, value);

	return SARADC_SUCCESS;
}

static UINT32 saradc_run_or_stop_adc(UINT8 run_stop)
{
	UINT32 value;

	value = REG_READ(SARADC_ADC_CONFIG);
    
    if(run_stop)
        value |= (SARADC_ADC_CHNL_EN);
    else
	    value &= ~(SARADC_ADC_CHNL_EN);
	
	REG_WRITE(SARADC_ADC_CONFIG, value);

	return SARADC_SUCCESS;
}

static UINT32 saradc_set_calibrate_val(saradc_cal_val_t *p_cal)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    if(SARADC_CALIBRATE_LOW == p_cal->mode)
    {
        saradc_val.low = p_cal->val;
    }
    else if(SARADC_CALIBRATE_HIGH == p_cal->mode)
    {
        saradc_val.high = p_cal->val;
    }
    else
    {
        GLOBAL_INT_RESTORE();
        return SARADC_FAILURE;
    }
    GLOBAL_INT_RESTORE();
    return SARADC_SUCCESS;
}

void saradc_config_param_init(saradc_desc_t* adc_config)
{
    memset(adc_config, 0x00, sizeof(saradc_desc_t));
    adc_config->channel = 1;
    adc_config->current_read_data_cnt = 0;
    adc_config->current_sample_data_cnt = 0;
    adc_config->filter = 0;
    adc_config->has_data = 0;
    adc_config->mode = (ADC_CONFIG_MODE_CONTINUE << 0)
                      |(ADC_CONFIG_MODE_4CLK_DELAY << 2);
    adc_config->pre_div = 0x10;
    adc_config->samp_rate = 0x20;
}

float saradc_calculate(UINT16 adc_val)
{
    float practic_voltage;
    practic_voltage = ((adc_val - saradc_val.low) * 1.8);
    practic_voltage = (practic_voltage / (saradc_val.high - saradc_val.low)) + 0.2;
    return practic_voltage;
}


static UINT32 saradc_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret = SARADC_SUCCESS;

    peri_busy_count_add();

	switch(cmd){
	case SARADC_CMD_SET_MODE:
		ret = saradc_set_mode(*(UINT8 *)param);
		break;
	case SARADC_CMD_SET_CHANNEL:
		ret = saradc_set_channel((saradc_chan_t *)param);
		break;
	case SARADC_CMD_SET_SAMPLE_RATE:
		ret = saradc_set_sample_rate(*(UINT8 *)param);
		break;
	case SARADC_CMD_SET_WAITING_TIME:
		ret = saradc_set_waiting_time(*(UINT8 *)param);
		break;
	case SARADC_CMD_SET_VALID_MODE:
		ret = saradc_set_valid_mode(*(UINT8 *)param);
		break;
	case SARADC_CMD_CLEAR_INT:
		saradc_int_clr();
		break;
	case SARADC_CMD_SET_CLK_RATE:
		ret = saradc_set_clk_rate(*(UINT8 *)param);
		break;
    case SARADC_CMD_RUN_OR_STOP_ADC:
        ret = saradc_run_or_stop_adc(*(UINT8 *)param);
        break;
    case SARADC_CMD_SET_CAL_VAL:
        ret = saradc_set_calibrate_val((saradc_cal_val_t *)param);
        break;
	default:
		ret = SARADC_FAILURE;
		break;
	}

    peri_busy_count_dec();

	return ret;
}

void saradc_isr(void)
{
	UINT32 value;

       
	value = REG_READ(SARADC_ADC_CONFIG);
    while((value & SARADC_ADC_FIFO_EMPTY) == 0) 
    {
        UINT16 dac_val;
        
        #if (CFG_SOC_NAME == SOC_BK7231)
        dac_val = REG_READ(SARADC_ADC_DATA)&0x03FF;
        #else
        dac_val = REG_READ(SARADC_ADC_DAT_AFTER_STA)&0xFFFF;
        #endif // (CFG_SOC_NAME == SOC_BK7231)
        
        if (saradc_desc->current_sample_data_cnt < saradc_desc->data_buff_size)
        {
            saradc_desc->pData[saradc_desc->current_sample_data_cnt++] = dac_val;
            saradc_desc->has_data = 1;

            if(saradc_desc->current_sample_data_cnt == saradc_desc->data_buff_size) 
            {
                saradc_close();
            }
        }
       
        value = REG_READ(SARADC_ADC_CONFIG);
    }
    saradc_int_clr();

    if(saradc_desc->p_Int_Handler != NULL) 
        (void)saradc_desc->p_Int_Handler();

}

