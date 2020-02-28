#include "aos/compiler.h"

#define adc_dev_t void
#define pwm_dev_t void
#define rtc_dev_t void
#define rtc_time_t void
#define wdg_dev_t void


WEAK int hal_adc_finalize(adc_dev_t *adc)
{
	 return 0;
}

WEAK int hal_adc_init(adc_dev_t *adc)
{
	return 0;
}

WEAK int  hal_adc_value_get(adc_dev_t *adc, uint32_t *output, unsigned int timeout)
{
	return 0;
}

WEAK int hal_pwm_finalize(pwm_dev_t *pwm)
{
	return 0;
}

WEAK int hal_pwm_init(pwm_dev_t *pwm)
{
	return 0;
}


WEAK int hal_pwm_start(pwm_dev_t *pwm)
{
	return 0;
}


WEAK int hal_pwm_stop(pwm_dev_t *pwm)
{
	return 0;
}


WEAK int hal_rtc_finalize(rtc_dev_t *rtc)
{
	return 0;
}


WEAK int hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
	return 0;
}


WEAK int hal_rtc_init(rtc_dev_t *rtc)
{
	return 0;
}


WEAK int hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
	return 0;
}


WEAK int hal_wdg_finalize(wdg_dev_t *wdg)
{
	return 0;
}


WEAK int hal_wdg_init(wdg_dev_t *wdg)
{
	return 0;
}


WEAK void hal_wdg_reload(wdg_dev_t *wdg)
{

}




