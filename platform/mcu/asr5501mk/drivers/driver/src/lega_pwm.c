#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_pwm.h"

void PWM_IRQHandler(void)
{
    lega_intrpt_enter();
    lega_intrpt_exit();
}

//pwm pinmux init
void lega_pwm_pinmux_init(lega_pwm_dev_t *pwm)
{
    uint32_t reg_value;
    switch(pwm->port)
    {
        case PWM_OUTPUT_CH0:
            //pin mux control
            //PWM0_PAD PAD14 1
            reg_value = REG_RD(PINMUX_CTRL_REG1) & (~PWM0_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG1, (reg_value|PWM0_PINMUX_VALUE));
            break;
        case PWM_OUTPUT_CH1:
            //PWM1_PAD PAD10 1
            reg_value = REG_RD(PINMUX_CTRL_REG1) & (~PWM1_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG1, (reg_value|PWM1_PINMUX_VALUE));
            break;
        case PWM_OUTPUT_CH2:
            //PWM2_PAD PAD15 1
            reg_value = REG_RD(PINMUX_CTRL_REG1) & (~PWM2_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG1, (reg_value|PWM2_PINMUX_VALUE));
            break;
        case PWM_OUTPUT_CH3:
            //PWM3_PAD PAD11 1
            reg_value = REG_RD(PINMUX_CTRL_REG1) & (~PWM3_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG1, (reg_value|PWM3_PINMUX_VALUE));
            break;
        case PWM_OUTPUT_CH4:
            //PWM4_PAD PAD6  4
            reg_value = REG_RD(PINMUX_CTRL_REG0) & (~PWM4_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG0, (reg_value|PWM4_PINMUX_VALUE));
            break;
        case PWM_OUTPUT_CH5:
            //PWM5_PAD PAD0  4
            reg_value = REG_RD(PINMUX_CTRL_REG0) & (~PWM5_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG0, (reg_value|PWM5_PINMUX_VALUE));
            break;
        case PWM_OUTPUT_CH6:
            //PWM6_PAD PAD7  4
            reg_value = REG_RD(PINMUX_CTRL_REG0) & (~PWM6_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG0, (reg_value|PWM6_PINMUX_VALUE));
            break;
        case PWM_OUTPUT_CH7:
            //PWM7_PAD PAD1  4
            reg_value = REG_RD(PINMUX_CTRL_REG0) & (~PWM7_PINMUX_MASK);
            REG_WR(PINMUX_CTRL_REG0, (reg_value|PWM7_PINMUX_VALUE));
            break;
        default:
            break;
    }
}

//pwm freq and duty cycle config
void lega_pwm_cfg(lega_pwm_dev_t *pwm)
{
    uint32_t tmp_value;
    switch(pwm->port)
    {
        case PWM_OUTPUT_CH0:
            PWM->PWMCTL |= PWM_COUNT_MODE; //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM01LOAD & (~(0x0000FFFF));
            tmp_value |= (SYSTEM_CLOCK / pwm->config.freq - 1);
            PWM->PWM01LOAD = tmp_value;

            tmp_value = PWM->PWM0CMP & (~(0x0000FFFF));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle));
            PWM->PWM0CMP = tmp_value;
            PWM->PWM01DB = 0;
            break;
        case PWM_OUTPUT_CH1:
            PWM->PWMCTL |= PWM_COUNT_MODE; //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM01LOAD & (~(0x0000FFFF));
            tmp_value |= (SYSTEM_CLOCK / pwm->config.freq - 1);
            PWM->PWM01LOAD = tmp_value;

            tmp_value = PWM->PWM0CMP & (~(0xFFFF0000));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle)) << 16;
            PWM->PWM0CMP = tmp_value;
            PWM->PWM01DB = 0;
            break;
        case PWM_OUTPUT_CH2:
            PWM->PWMCTL |= (PWM_COUNT_MODE << 1); //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM01LOAD & (~(0xFFFF0000));
            tmp_value |= ((uint16_t)(SYSTEM_CLOCK / pwm->config.freq - 1) << 16);
            PWM->PWM01LOAD = tmp_value;

            tmp_value = PWM->PWM1CMP & (~(0x0000FFFF));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle));
            PWM->PWM1CMP = tmp_value;
            PWM->PWM01DB = 0;
            break;
        case PWM_OUTPUT_CH3:
            PWM->PWMCTL |= (PWM_COUNT_MODE << 1); //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM01LOAD & (~(0xFFFF0000));
            tmp_value |= ((uint16_t)(SYSTEM_CLOCK / pwm->config.freq - 1) << 16);
            PWM->PWM01LOAD = tmp_value;

            tmp_value = PWM->PWM1CMP & (~(0xFFFF0000));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle)) << 16;
            PWM->PWM1CMP = tmp_value;
            PWM->PWM01DB = 0;
            break;
        case PWM_OUTPUT_CH4:
            PWM->PWMCTL |= (PWM_COUNT_MODE << 2); //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM23LOAD & (~(0x0000FFFF));
            tmp_value |= (SYSTEM_CLOCK / pwm->config.freq - 1);
            PWM->PWM23LOAD = tmp_value;

            tmp_value = PWM->PWM2CMP & (~(0x0000FFFF));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle));
            PWM->PWM2CMP = tmp_value;
            PWM->PWM23DB = 0;
            break;
        case PWM_OUTPUT_CH5:
            PWM->PWMCTL |= (PWM_COUNT_MODE << 2); //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM23LOAD & (~(0x0000FFFF));
            tmp_value |= (SYSTEM_CLOCK / pwm->config.freq - 1);
            PWM->PWM23LOAD = tmp_value;

            tmp_value = PWM->PWM2CMP & (~(0xFFFF0000));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle)) << 16;
            PWM->PWM2CMP = tmp_value;
            PWM->PWM23DB = 0;
            break;
        case PWM_OUTPUT_CH6:
            PWM->PWMCTL |= (PWM_COUNT_MODE << 3); //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM23LOAD & (~(0xFFFF0000));
            tmp_value |= ((uint16_t)(SYSTEM_CLOCK / pwm->config.freq - 1) << 16);
            PWM->PWM23LOAD = tmp_value;

            tmp_value = PWM->PWM3CMP & (~(0x0000FFFF));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle));
            PWM->PWM3CMP = tmp_value;
            PWM->PWM23DB = 0;
            break;
        case PWM_OUTPUT_CH7:
            PWM->PWMCTL |= (PWM_COUNT_MODE << 3); //0: count-up mode, 1: count-up/down mode
            tmp_value = PWM->PWM23LOAD & (~(0xFFFF0000));
            tmp_value |= ((uint16_t)(SYSTEM_CLOCK / pwm->config.freq - 1) << 16);
            PWM->PWM23LOAD = tmp_value;

            tmp_value = PWM->PWM3CMP & (~(0xFFFF0000));
            tmp_value |= (uint16_t)(SYSTEM_CLOCK / pwm->config.freq * (1-pwm->config.duty_cycle)) << 16;
            PWM->PWM3CMP = tmp_value;
            PWM->PWM23DB = 0;
            break;
        default:
            break;
    }
}

/**
 * Initialises a PWM pin
 *
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_pwm_init(lega_pwm_dev_t *pwm)
{
    uint32_t reg_value;
    if(NULL == pwm)
    {
        return -1;
    }
    if(pwm->port >= LEGA_PWM_CH_NUM)
    {
        return -1;
    }
    //pinmux
    lega_pwm_pinmux_init(pwm);

    //pwm clock enable
    reg_value = REG_RD(PERI_CLK_CFG);
    REG_WR(PERI_CLK_CFG, (reg_value|PWM_CLK_EN));

    PWM->PWMCFG &= ~(1 << pwm->port);
    //PWM->PWMCFG |= (CNT_CLK_DIV_EN | CLK_DIV_CFG);
    lega_pwm_cfg(pwm);
    PWM->PWMINVERTTRIG = 0; //invert control
    return 0;
}

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_pwm_start(lega_pwm_dev_t *pwm)
{
    if(NULL == pwm)
    {
        return -1;
    }
    if(pwm->port >= LEGA_PWM_CH_NUM)
    {
        return -1;
    }
    PWM->PWMCFG |= (1 << pwm->port);
    return 0;
}

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_pwm_stop(lega_pwm_dev_t *pwm)
{
    if(NULL == pwm)
    {
        return -1;
    }
    if(pwm->port >= LEGA_PWM_CH_NUM)
    {
        return -1;
    }
    PWM->PWMCFG &= ~(1 << pwm->port);
    return 0;
}

/**
 * change the para of pwm
 *
 * @param[in]  pwm  the PWM device
 * @param[in]  para the para of pwm
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_pwm_para_chg(lega_pwm_dev_t *pwm, lega_pwm_config_t para)
{
    if(NULL == pwm)
    {
        return -1;
    }
    if(pwm->port >= LEGA_PWM_CH_NUM)
    {
        return -1;
    }
    //lega_pwm_stop(pwm);
    pwm->config = para;
    lega_pwm_cfg(pwm);
    //lega_pwm_start(pwm);
    return 0;
}

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_pwm_finalize(lega_pwm_dev_t *pwm)
{
    //pwm clock disable
    //uint32_t reg_value;
    if(NULL == pwm)
    {
        return -1;
    }
    if(pwm->port >= LEGA_PWM_CH_NUM)
    {
        return -1;
    }
    //one clk enable for 8 pwm channel
    //reg_value = REG_RD(PERI_CLK_CFG);
    //REG_WR(PERI_CLK_CFG, (reg_value&(~PWM_CLK_EN)));
    return lega_pwm_stop(pwm);
}

