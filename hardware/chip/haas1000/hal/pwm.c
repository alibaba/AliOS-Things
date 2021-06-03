/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/pwm.h"
#include "hal_pwm.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_cmu.h"
#include "pmu.h"

#define _HAL_PWM_MAX_NUM 4
static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[_HAL_PWM_MAX_NUM] = {
	{HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
};

typedef struct {
	uint8 chan;
	int start;
	struct HAL_PWM_CFG_T pwm_cfg;
} _HAL_PWM_PRIV_T;

static int inited[_HAL_PWM_MAX_NUM] = {0};

static enum HAL_PWM_ID_T __hal_pwm_port2chan(uint8_t port)
{
	return (port<_HAL_PWM_MAX_NUM) ? port+HAL_PWM_ID_0 : HAL_PWM_ID_QTY;
}

static void __hal_pwm_pram2cfg(struct HAL_PWM_CFG_T *cfg, pwm_config_t *pram)
{
	cfg->freq = pram->freq;
	cfg->ratio = pram->duty_cycle*1000;
	cfg->inv = false;
	cfg->sleep_on = false;
}

/**
 * Initialises a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_init(pwm_dev_t *pwm)
{
	//printf("%s port=%d, duty_cycle=%f, freq=%d, priv=%p\r\n", __FUNCTION__, pwm->port, pwm->config.duty_cycle, pwm->config.freq, pwm->priv);
	if (pwm->priv != NULL) {
		printf("%s priv=%p, SHOULD be NULL!\n", __FUNCTION__, pwm->priv);
		return -1;
	}
	_HAL_PWM_PRIV_T *_cfg = malloc(sizeof(_HAL_PWM_PRIV_T));
	if (!_cfg) {
		printf("%s malloc for _HAL_PWM_PRIV_T failed\n", __FUNCTION__);
		return -1;
	}
	if (inited[pwm->port] == 0) {
		hal_iomux_init(&pinmux_pwm[pwm->port], 1);
		// for (int i=0; i<_HAL_PWM_MAX_NUM; i++) {
		hal_gpio_pin_set_dir(pinmux_pwm[pwm->port].pin, HAL_GPIO_DIR_OUT, 1);
		// }
		//init pmu pwm
		//pmu_write(0x2b, 0xce89);
		//pmu_write(0x30, 0x560c);
		//pmu_write(0x2f, 0x0000);

		hal_pwm_set_ratio_max(1000);
		inited[pwm->port] = 1;
	}
	_cfg->chan = __hal_pwm_port2chan(pwm->port);
	_cfg->start = 0;
	__hal_pwm_pram2cfg(&_cfg->pwm_cfg, &pwm->config);
	pwm->priv = (void *)_cfg;

    return 0;
}

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
	//printf("%s port=%d, duty_cycle=%f, freq=%d, priv=%p\r\n", __FUNCTION__, pwm->port, pwm->config.duty_cycle, pwm->config.freq, pwm->priv);
	_HAL_PWM_PRIV_T *_cfg = (_HAL_PWM_PRIV_T *)pwm->priv;
	if (_cfg->start == 0) {
		if (hal_pwm_enable(_cfg->chan, &_cfg->pwm_cfg) == 0) {
			_cfg->start = 1;
		}
	}
	return 0;
}

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
	//printf("%s port=%d, duty_cycle=%f, freq=%d, priv=%p\r\n", __FUNCTION__, pwm->port, pwm->config.duty_cycle, pwm->config.freq, pwm->priv);
	_HAL_PWM_PRIV_T *_cfg = (_HAL_PWM_PRIV_T *)pwm->priv;
	if (_cfg->start == 1) {
		if (hal_pwm_disable(_cfg->chan) == 0) {
			_cfg->start = 0;
		}
	}
	return 0;
}

/**
 * change the para of pwm
 *
 * @param[in]  pwm   the PWM device
 * @param[in]  para  the para of pwm
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
	/*printf("%s port=%d, duty_cycle=%f, freq=%d, priv=%p\n", __FUNCTION__, pwm->port, pwm->config.duty_cycle, pwm->config.freq, pwm->priv);*/
	/*printf("%s new duty_cycle=%f, freq=%d\n", __FUNCTION__, para.duty_cycle, para.freq);*/
	_HAL_PWM_PRIV_T *_cfg = (_HAL_PWM_PRIV_T *)pwm->priv;
	memcpy(&pwm->config, &para, sizeof(pwm_config_t));
	__hal_pwm_pram2cfg(&_cfg->pwm_cfg, &pwm->config);
	if (_cfg->start == 1) {
		hal_pwm_enable(_cfg->chan, &_cfg->pwm_cfg);
	}
	return 0;
}

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
	//printf("%s port=%d, duty_cycle=%f, freq=%d, priv=%p\n", __FUNCTION__, pwm->port, pwm->config.duty_cycle, pwm->config.freq, pwm->priv);
	_HAL_PWM_PRIV_T *_cfg = (_HAL_PWM_PRIV_T *)pwm->priv;
	if (_cfg) {
		if (_cfg->start == 1) {
			hal_pwm_stop(pwm);
		}
		free(_cfg);
		pwm->priv = NULL;
	}
	return 0;
}

