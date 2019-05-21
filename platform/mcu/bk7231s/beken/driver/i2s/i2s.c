#include "include.h"
#include "arm_arch.h"

#include "i2s.h"
#include "i2s_pub.h"
#include "gpio_pub.h"
#include "drv_model_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "driver_audio_if_pub.h"
#include "uart_pub.h"
#include "music_msg_pub.h"
static SDD_OPERATIONS i2s_op =
{
    i2s_ctrl
};
static UINT8 flag_i2s_active =0;

static void i2s_clk_config(UINT8 enable)
{
	UINT32 param;
	UINT32 cmd;

	if(enable)
		cmd = CMD_CLK_PWR_UP;
	else
		cmd = CMD_CLK_PWR_DOWN;

	param = PWD_I2S_PCM_CLK_BIT;
	sddev_control(ICU_DEV_NAME, cmd, &param);
}

static void i2s_hw_init(void)
{
	UINT32 value_cfg2,value_cn;
	UINT32 param = GFUNC_MODE_I2S;
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
	
	i2s_clk_config(1);
	
	value_cfg2 = (2 << TXFIFO_TXFIFO_MODE) | (0 << TXFIFO_DOWN_SMPRATIO);
	REG_WRITE(PCM_CFG2,value_cfg2);
	value_cn = (1 << TX_FIFO0_LEVEL_POSI) | (1 << RX_FIFO_LEVEL_POSI);
	REG_WRITE(PCM_CN,value_cfg2);
}

static void i2s_active(int enable)
{
    UINT32 value_ctrl, value_cn,sts,param;

	os_printf("====i2s active:%x===\r\n",enable);

    value_ctrl = REG_READ(PCM_CTRL);
    value_cn = REG_READ(PCM_CN);
    if(enable)
    {	
    	flag_i2s_active =1;
        value_ctrl |= I2S_PCM_EN;
		value_cn |= TX_INT0_EN | TX_UDF0_EN|TX_INT1_EN | TX_UDF1_EN|TX_INT2_EN | TX_UDF2_EN;
    }
    else
    {
    	flag_i2s_active = 0;
        value_ctrl &= ~I2S_PCM_EN;
		value_cn &= ~( TX_INT0_EN | TX_UDF0_EN|TX_INT1_EN | TX_UDF1_EN|TX_INT2_EN |TX_UDF2_EN| RX_INT_EN | RX_OVF_EN);	
    }
    REG_WRITE(PCM_CTRL, value_ctrl);
    REG_WRITE(PCM_CN, value_cn);
	
   sts = REG_READ(PCM_STAT);
   REG_WRITE(PCM_STAT,sts);
   
   param = IRQ_I2S_PCM_BIT;
   if(enable)
   		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
   else
    	sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
  	
}

static void i2s_rx_active(int enable)
{
    UINT32 value_cn;

    value_cn = REG_READ(PCM_CN);
    if(enable)
    {
        value_cn |= (RX_INT_EN | RX_OVF_EN | RX_FIFO_CLR);
    }
    else
    {
        value_cn &= ~(RX_INT_EN | RX_OVF_EN);
    }
    REG_WRITE(PCM_CN, value_cn);
}
/*default to slave mode*/
static void i2s_set_freq_datawidth(i2s_rate_t *p_rate)
{
    UINT32 bitratio, value = 0;
	
    if( (p_rate->freq != 8000) && (p_rate->freq != 16000) && 
		(p_rate->freq != 24000) && (p_rate->freq != 32000) &&(p_rate->freq != 48000)&&
		(p_rate->freq != 11025) && (p_rate->freq != 22050) &&(p_rate->freq != 44100) )
    {
        return;
    }
    if(p_rate->datawidth > 32)
    {
        return;
    }

    bitratio = ((I2S_SYS_CLK / (p_rate->freq * p_rate->datawidth ) / 4) - 1) & 0xFF;
    value = (0 << MODE_SEL_POSI)
			| ((p_rate->datawidth - 1) << DATALEN_POSI)
            | ((p_rate->datawidth - 1) << SMPRATIO_POSI)
            | (bitratio << BITRATIO_POSI)//this value is unused in slave mode
            | (SLAVEEN);
    REG_WRITE(PCM_CTRL, value);
}

static void i2s_select_mode(UINT8 mode)
{
    UINT32 value;

    if(mode == 3 || mode > 7)
    {
        return;
    }

    value = REG_READ(PCM_CTRL);
    value &= ~(MODE_SEL_MASK << MODE_SEL_POSI);
    value |= (mode << MODE_SEL_POSI);
    REG_WRITE(PCM_CTRL, value);
}

static void i2s_set_level(i2s_level_t *p_level)
{
    UINT32 value;

    value = REG_READ(PCM_CN);

    value &= ~((RX_FIFO_LEVEL_MASK << RX_FIFO_LEVEL_POSI) | (TX_FIFO0_LEVEL_MASK << TX_FIFO0_LEVEL_POSI));
    value |= ((p_level->rx_level << RX_FIFO_LEVEL_POSI) | (p_level->tx_level << TX_FIFO0_LEVEL_POSI));

    REG_WRITE(PCM_CN, value);
}

void i2s_init(void)
{
    intc_service_register(IRQ_I2S_PCM, PRI_IRQ_I2S_PCM, i2s_isr);
    sddev_register_dev(I2S_DEV_NAME, &i2s_op);
}

void i2s_exit(void)
{
    sddev_unregister_dev(I2S_DEV_NAME);
}

UINT8 is_i2s_active(void)
{
	return flag_i2s_active;
}

static UINT32 i2s_ctrl(UINT32 cmd, void *param)
{
    UINT8 ret = I2S_SUCCESS;

    switch(cmd)
    {
    case I2S_HW_SET:
		i2s_hw_init();
		break;
    case I2S_CMD_SET_FREQ_DATAWIDTH:
        i2s_set_freq_datawidth((i2s_rate_t *)param);
        break;
    case I2S_CMD_ACTIVE:
        i2s_active(*(UINT8 *)param);
        break;
    case I2S_CMD_RXACTIVE:
        i2s_rx_active(*(UINT8 *)param);
        break;
    case I2S_CMD_SELECT_MODE:
        i2s_select_mode(*(UINT8 *)param);
        break;
    case I2S_CMD_SET_LEVEL:
        i2s_set_level((i2s_level_t *)param);
        break;
    default:
        ret = I2S_FAILURE;
        break;
    }
    return ret;
}

void i2s_isr(void)
{
	uint16_t i;
	uint16_t sample[32];
	uint16_t len = 0;
	uint32_t sts = REG_READ(PCM_STAT);
	
#if (CONFIG_APP_MP3PLAYER==1)
	if(sts & (TX_INT0|TX_UDF0))
	{
		len = aud_get_buffer_length((uint8_t*)sample,32*2);
		if( len != 0 )
		{
			for( i = 0; i < len >> 1; i+= 2 )
			{
				REG_WRITE(PCM_DAT0,sample[i]);//test_pcm[i]);//
				REG_WRITE(PCM_DAT0,sample[i+1]);//test_pcm[i+1]);//
			}
		}
		else
		{
			i2s_active(0);
		}
	}
#endif
	REG_WRITE(PCM_STAT,sts);
}
