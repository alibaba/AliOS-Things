/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * @file hal/soc/pwm.c
 * @brief PWM HAL
 * @version since 5.5.0
 */

#include "hal/soc/soc.h"
#include "driver/chip/hal_pwm.h"

/**@brief Initialises a PWM pin
 *
 * @note  Prepares a Pulse-Width Modulation pin for use.
 * Does not start the PWM output (use @ref MicoPwmStart).
 *
 * @param     pwm    : the PWM device
 * @return    0      : on success.
 * @return    EIO    : if an error occurred with any step
 */
int32_t hal_pwm_init(pwm_dev_t *pwm)
{
	HAL_Status status = HAL_ERROR;
	PWM_ClkParam clk_param = {0};
	PWM_ChInitParam ch_param = {0};
	int max_duty_ratio = 0;

	clk_param.clk = PWM_CLK_HOSC;
	clk_param.div = PWM_SRC_CLK_DIV_1;
	status = HAL_PWM_GroupClkCfg(pwm->port/2, &clk_param);
	if (status != HAL_OK)
		printf("[hal_pwm]: PWM ch %d clk config error\n", pwm->port);

	ch_param.hz = pwm->config.freq;
	ch_param.mode = PWM_CYCLE_MODE;
	ch_param.polarity = PWM_HIGHLEVE;
	max_duty_ratio = HAL_PWM_ChInit(pwm->port, &ch_param);
	if (max_duty_ratio == -1)
		printf("[hal_pwm]: PWM ch %d init error\n", pwm->port);

	status = HAL_PWM_ChSetDutyRatio(pwm->port, pwm->config.duty_cycle);
	if (status != HAL_OK)
		printf("[hal_pwm]: PWM ch %d set duty ratio error\n", pwm->port);

	return 0;
}


/**@brief Starts PWM output on a PWM interface
 *
 * @note  Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param     pwm  : the PWM device
 * @return    0    : on success.
 * @return    EIO  : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
	HAL_Status status = HAL_ERROR;
	status = HAL_PWM_EnableCh(pwm->port, PWM_CYCLE_MODE, 1);
	if (status != HAL_OK) {
		printf("[hal_pwm]: PWM ch %d start error\n", pwm->port);
		return -1;
	}
	return 0;
}


/**@brief Stops output on a PWM pin
 *
 * @note  Stops Pulse-Width Modulation signal output on a PWM pin
 *
 * @param     pwm : the PWM device
 * @return    0   : on success.
 * @return    EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
	HAL_PWM_ChDeinit(pwm->port);
	return -1;
}

