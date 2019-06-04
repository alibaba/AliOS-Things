#include "aos/hal/pwm.h"
#include "rtos_pub.h"
#include "drv_model_pub.h"
#include "pwm_pub.h"

int32_t hal_pwm_init(pwm_dev_t *pwm)
{
    UINT32 ret = 0;
	pwm_param_t param;

    /*init pwm*/
    param.channel         = pwm->port;
    param.cfg.bits.en     = PWM_DISABLE;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PMODE_PWM;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle      = (pwm->config.duty_cycle * 26000);
    param.end_value       = (pwm->config.freq * 26000);
	
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);

    return ret;
}

int32_t hal_pwm_start(pwm_dev_t *pwm)
{
    UINT32 ret = 0;
    UINT32 param;

    param = pwm->port;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_ENABLE, &param);

    return ret;
}

int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
    UINT32 ret = 0;
    UINT32 param;

    param = pwm->port;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UINT_DISABLE, &param);

    return ret;
}

int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
    UINT32 ret = 0;
	pwm_param_t param;

    /*deinit pwm*/
    param.channel         = pwm->port;
    param.cfg.val         = 0;
    param.p_Int_Handler   = 0;
    param.duty_cycle      = 0;
    param.end_value       = 0;
	
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_DEINIT_PARAM, &param);

    return ret;
}

