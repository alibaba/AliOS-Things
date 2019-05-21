#include "include.h"
#include "arm_arch.h"

#include "pwm.h"
#include "pwm_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"

static SDD_OPERATIONS pwm_op =
{
    pwm_ctrl
};

void (*p_PWM_Int_Handler[CHANNEL_NO])(UINT8);

static void pwm_gpio_configuration(UINT8 chan, UINT8 enable)
{
    UINT32 ret;
    UINT32 param;

    switch(chan)
    {
    case PWM0:
        param = GFUNC_MODE_PWM0;
        break;

    case PWM1:
        param = GFUNC_MODE_PWM1;
        break;

    case PWM2:
        param = GFUNC_MODE_PWM2;
        break;

    case PWM3:
        param = GFUNC_MODE_PWM3;
        break;

    case PWM4:
        param = GFUNC_MODE_PWM4;
        break;

    case PWM5:
        param = GFUNC_MODE_PWM5;
        break;

    default:
        break;
    }

	if(enable)
	{
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
	}
	else
	{
		param = GPIO_CFG_PARAM(param, GMODE_INPUT);
    	ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	}
    ASSERT(GPIO_SUCCESS == ret);
}

static void pwm_icu_configuration(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 ret;
    UINT32 prm;

    /* set clock power down of icu module*/
    switch(pwm_param->channel)
    {
    case PWM0:
        prm = PWD_PWM0_CLK_BIT;
        break;

    case PWM1:
        prm = PWD_PWM1_CLK_BIT;
        break;

    case PWM2:
        prm = PWD_PWM2_CLK_BIT;
        break;

    case PWM3:
        prm = PWD_PWM3_CLK_BIT;
        break;

    case PWM4:
        prm = PWD_PWM4_CLK_BIT;
        break;

    case PWM5:
        prm = PWD_PWM5_CLK_BIT;
        break;

    default:
		PWM_WARN("pwm_iconfig_fail\r\n");
        goto exit_icu;
    }

	if(enable)
	{
    ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&prm);
    ASSERT(ICU_SUCCESS == ret);

    if(PWM_CLK_32K == pwm_param->cfg.bits.clk)
    {
        prm = pwm_param->channel;
        ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_LPOCLK, (void *)&prm);
    }
    else
    {
        prm = pwm_param->channel;
        ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_PCLK, (void *)&prm);
    }
    ASSERT(ICU_SUCCESS == ret);
	}
	else
	{
    	ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&prm);
		ASSERT(ICU_SUCCESS == ret);
	}

    if(PWM_INT_EN == pwm_param->cfg.bits.int_en)
    {
        prm = IRQ_PWM_BIT;
        ret = sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, (void *)&prm);
    }

exit_icu:

    return;
}

static void init_pwm_param(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 value;

    if((pwm_param == NULL)
            || (pwm_param->channel >= PWM_COUNT)
            || (pwm_param->duty_cycle > pwm_param->end_value))
    {
        return;
    }

	if(pwm_param->cfg.bits.mode != PMODE_TIMER)
	{
#if (CFG_SOC_NAME == SOC_BK7231)
		pwm_gpio_configuration(pwm_param->channel, enable);
#else
		pwm_gpio_configuration(pwm_param->channel, enable);
#endif
	}

    value = REG_READ(PWM_CTL);
    value = (value & (~(0x0F << (0x04 *  pwm_param->channel))))
            | ((pwm_param->cfg.val & 0x0F) << (0x04 * pwm_param->channel));
    REG_WRITE(PWM_CTL, value);
	
#if (CFG_SOC_NAME == SOC_BK7231)
    value = (((UINT32)pwm_param->duty_cycle & 0x0000FFFF) << 16)
		            + ((UINT32)pwm_param->end_value & 0x0000FFFF);
    REG_WRITE(REG_APB_BK_PWMn_CNT_ADDR(pwm_param->channel), value);
#else
    value = ((UINT32)pwm_param->end_value);
    REG_WRITE(REG_APB_BK_PWMn_END_ADDR(pwm_param->channel), value);

    value = ((UINT32)pwm_param->duty_cycle);
    REG_WRITE(REG_APB_BK_PWMn_DC_ADDR(pwm_param->channel), value);
#endif

    p_PWM_Int_Handler[pwm_param->channel] = pwm_param->p_Int_Handler;

    pwm_icu_configuration(pwm_param, enable);
}

static UINT16 pwm_capture_value_get(UINT8 ucChannel)
{
    return REG_READ(REG_APB_BK_PWMn_CAP_ADDR(ucChannel));
}

static void pwm_int_handler_clear(UINT8 ucChannel)
{
    p_PWM_Int_Handler[ucChannel] = NULL;
}

void pwm_init(void)
{
    intc_service_register(IRQ_PWM, PRI_IRQ_PWM, pwm_isr);

    sddev_register_dev(PWM_DEV_NAME, &pwm_op);
}

void pwm_exit(void)
{
    sddev_unregister_dev(PWM_DEV_NAME);
}

UINT32 pwm_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = PWM_SUCCESS;
    UINT32 ucChannel;
    UINT32 value;
    pwm_param_t *p_param;
    pwm_capture_t *p_capture;

    switch(cmd)
    {
    case CMD_PWM_UNIT_ENABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value |= (1 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_UINT_DISABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value &= ~(3 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_IR_ENABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value |= (2 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_IR_DISABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value &= ~(2 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_IR_CLEAR:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        pwm_int_handler_clear(ucChannel);
        break;
    case CMD_PWM_INIT_PARAM:
        p_param = (pwm_param_t *)param;
        init_pwm_param(p_param, 1);
        break;
    case CMD_PWM_CAP_GET:
        p_capture = (pwm_capture_t *)param;
        if(p_capture->ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        p_capture->value = pwm_capture_value_get(p_capture->ucChannel);
        break;
	case CMD_PWM_DEINIT_PARAM:
        p_param = (pwm_param_t *)param;
        init_pwm_param(p_param, 0);
		break;
    default:
        ret = PWM_FAILURE;
        break;
    }

    return ret;
}

void pwm_isr(void)
{
    int i;
    UINT32 status;

    status = REG_READ(PWM_INTERRUPT_STATUS);

    for(i = 0; i < CHANNEL_NO; i++)
    {
        if(status & (1 << i))
        {
            if(p_PWM_Int_Handler[i])
            {
                p_PWM_Int_Handler[i]((UINT8)i);
                do
                {
                    REG_WRITE(PWM_INTERRUPT_STATUS, (1 << i));
                }
                while(REG_READ(PWM_INTERRUPT_STATUS) & (1 << i));
            }
        }
    }

    do
    {
        REG_WRITE(PWM_INTERRUPT_STATUS, status);
    }
    while(REG_READ(PWM_INTERRUPT_STATUS) & status & 0x3F);
}
