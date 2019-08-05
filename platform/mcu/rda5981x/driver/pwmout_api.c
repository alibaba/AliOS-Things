/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed_assert.h"
#include "pwmout_api.h"
#include "gpio_api.h"
#include "cmsis.h"
#include "pinmap.h"
#include "mbed_interface.h"

#define PWM_CLK_SRC_20MHZ       (20000000)
#define PWM_CLK_SRC_32KHZ       (32768)
#define PWM_CLK_IDX_NUM         (3)
#define PWM_DIFF_CHN_NUM        (2)
#define PWM_SYNC_CHN_NUM        (2)

#define PWM_CLKGATE_REG         (RDA_SCU->CLKGATE1)
#define PWM_CLKSRC_REG          (RDA_SCU->PWMCFG)
#define PWM_CLKDIV_REG          (RDA_PWM->CLKR)
#define EXIF_PWM_EN_REG         (RDA_EXIF->MISCCFG)
#define PWM_DIFF0_REG           (RDA_EXIF->PWM0CFG)
#define PWM_DIFF3_REG           (RDA_EXIF->PWM3CFG)

/* PORT ID, PWM ID, Pin function */
static const PinMap PinMap_PWM[] = {
    {PA_0, PWM_6, 4},
    {PA_1, PWM_3, 4},
    {PB_0, PWM_2, 4},
    {PB_1, PWM_7, 4},
    {PB_2, PWM_5, 4},
    {PB_3, PWM_4, 4},
    {PB_8, PWM_0, 4},
    {PC_1, PWM_1, 5},
    {PD_0, PWM_0, 4},
    {PD_1, PWM_1, 4},
    {PD_2, PWM_2, 4},
    {PD_3, PWM_3, 4},
    {NC,   NC,    0}
};

__IO uint32_t *PWM_MATCH[] = {
    &(RDA_EXIF->PWM0CFG),
    &(RDA_EXIF->PWM1CFG),
    &(RDA_EXIF->PWM2CFG),
    &(RDA_EXIF->PWM3CFG),
    &( RDA_PWM->PWTCFG ),
    &( RDA_PWM->LPGCFG ),
    &( RDA_PWM->PWL0CFG),
    &( RDA_PWM->PWL1CFG)
};

static uint32_t BaseClk_PWM[PWM_CLK_IDX_NUM] = {0UL};

static PinName PWM_DIFF_CHN[PWM_DIFF_CHN_NUM]= {0UL};
static uint32_t pwm0_period;
static uint32_t pwm0_sync_pulsewidth;
static uint32_t pwm0_duty;
static uint32_t pwm3_period;
static uint32_t pwm3_sync_pulsewidth;
static uint32_t pwm3_duty;
static uint8_t  pwm_sync_cfged;
static PinName PWM_SYNC_CHN[PWM_SYNC_CHN_NUM]= {0UL};
extern gpio_t gpio_p1;

static uint8_t is_pwmout_started(pwmout_t* obj);
static void pwmout_start(pwmout_t* obj);
static void pwmout_stop(pwmout_t* obj);
static void pwmout_update_cfgreg(pwmout_t* obj);
static void pwmout_cfgreg(pwmout_t* obj);


void pwmout_clk_set(pwmout_t *obj, int src, int div)
{
    uint32_t reg_val = 0UL;
    uint32_t clk_idx = 0UL, clk_hz = PWM_CLK_SRC_32KHZ >> 1;
    PWMName pwm = (PWMName)(obj->channel);
    uint32_t divider = (uint32_t)div;

    MBED_ASSERT(PWM_7 >= pwm);
    if (PWM_5 == pwm) {
        clk_idx = 1;
    } else if (PWM_6 <= pwm) {
        clk_idx = 2;
    }
    if (divider > 0x80UL) { // max divider is 128
        divider = 0x80UL;
    }
    reg_val = PWM_CLKSRC_REG & ~((0x00FFUL << (clk_idx * 8)) | (0x01UL << (24 + clk_idx)));
    if (src) {
        reg_val |= (0x01UL << (7 + (clk_idx * 8)));
        clk_hz = PWM_CLK_SRC_20MHZ >> 1;
    }
    if (divider) {
        reg_val |= ((divider - 0x01UL) << (clk_idx * 8));
        BaseClk_PWM[clk_idx] = clk_hz / divider;
    } else {
        BaseClk_PWM[clk_idx] = clk_hz;
    }
    PWM_CLKSRC_REG = reg_val | (0x01UL << (24 + clk_idx));
}

void pwmout_init(pwmout_t* obj, PinName pin)
{
    /* determine the channel */
    PWMName pwm = (PWMName)pinmap_peripheral(pin, PinMap_PWM);
    MBED_ASSERT(pwm != (PWMName)NC);

    obj->channel = pwm;
    obj->CFGR = PWM_MATCH[pwm];
    obj->pin = pin;

    if(obj->channel == PWM_0){
        PWM_DIFF_CHN[0] = pin;
    }else if(obj->channel == PWM_3){
        PWM_DIFF_CHN[1] = pin;
    }

    /* Enable PWM Clock-gating */
    PWM_CLKGATE_REG |= (0x01UL << 2);

    /* Init PWM clock source and divider */
    if (PWM_4 >= pwm) {
        /* default to 20MHz / 2 */
        pwmout_clk_set(obj, 1, 0);
        if (PWM_4 == pwm) {
            obj->period_ticks_max = 0x1FF8UL;
            obj->period_ticks_min = 0x0008UL;
        } else {
            obj->period_ticks_max = 0x20000UL;
            obj->period_ticks_min = 0x00002UL;
        }
        obj->base_clk_ptr = &(BaseClk_PWM[0]);
    } else if (PWM_5 == pwm) {
        /* default to 32KHz / 2 */
        pwmout_clk_set(obj, 0, 0);
        obj->period_ticks_max = 0x7000UL;
        obj->period_ticks_min = 0x0800UL;
        obj->base_clk_ptr = &(BaseClk_PWM[1]);
    } else {
        /* default to 20MHz / 2 */
        pwmout_clk_set(obj, 1, 0);
        //do not need to div
        obj->period_ticks_max = 0x0FFUL; // TBD
        obj->period_ticks_min = 0x001UL;
        obj->base_clk_ptr = &(BaseClk_PWM[2]);
    }

    // default to half of max period
    if (PWM_5 >= pwm) {
    pwmout_period_us(obj, (obj->period_ticks_max * 1e6) / (*(obj->base_clk_ptr)) / 2);
    }
    //PWM6 PWM7 has no period
    //pwmout_write    (obj, 0.5f);

    // Wire pinout
    pinmap_pinout(pin, PinMap_PWM);
    gpio_t gpio;
    gpio_init(&gpio,pin);
    gpio_dir(&gpio, PIN_OUTPUT);
}

void pwmout_free(pwmout_t* obj)
{
    /* Disable PWM Clock-gating */
    PWM_CLKGATE_REG &= ~(0x01UL << 2);
}

void pwmout_sync_write(pwmout_t *obj, float value, uint8_t duty_sel)
{
    uint32_t ticks;
    uint32_t p_ticks_h = 0xFFF;
    uint32_t p_ticks_l = 0x0;
    uint32_t reg_val;
	uint32_t pwm_src_deft;
    uint8_t clk_div;
    uint8_t clk_sel; //0:32K 1:20M
    int err_code = 0;
    static float value0;

    if (obj->period_ticks < obj->period_ticks_min) {
            obj->period_ticks = obj->period_ticks_min;
            err_code = 1; // too small period
    }
    if (obj->period_ticks > obj->period_ticks_max) {
        obj->period_ticks = obj->period_ticks_max;
        err_code = 2; // too large period
        if (obj->pulsewidth_ticks > obj->period_ticks_max) {
            obj->pulsewidth_ticks = obj->period_ticks_max;
            err_code = 3; // too large period & duty
        }
    }
    if (0 != err_code) {
        mbed_error_printf("PwmCfgErr:%d\r\n",err_code);
    }

    if (value < 0.0f) {
        value = 0.0;
    } else if (value > 1.0f) {
        value = 1.0;
    }

    ticks = (uint32_t)((float)(obj->period_ticks) * value);

    if(ticks != obj->pulsewidth_ticks) {
        obj->pulsewidth_ticks = ticks;
    }

    if(PWM_0 == (PWMName)obj->channel){
        pwm0_period = obj->period_ticks;
        pwm0_sync_pulsewidth = obj->pulsewidth_ticks;
        PWM_SYNC_CHN[0] = obj->pin;
        pwm0_duty = duty_sel;
        pwm_sync_cfged |= 1;
    }
    if(PWM_3 == (PWMName)obj->channel){
        pwm3_period = obj->period_ticks;
        pwm3_sync_pulsewidth = obj->pulsewidth_ticks;
        PWM_SYNC_CHN[1] = obj->pin;
        pwm3_duty = duty_sel;
        pwm_sync_cfged |= (1 << 1);
    }
    if(pwm_sync_cfged != 3){
    }
    else{
        core_util_critical_section_enter();
        if(pwm3_duty == pwm0_duty){
            mbed_error_printf("PwmCfgErr: same duty in sync mode\r\n");
        }

#if 1
        gpio_t gpio;
        gpio_init_out(&gpio, PWM_SYNC_CHN[0]);
        gpio_write(&gpio, 0);
        gpio_init_out_ex(&gpio, PWM_SYNC_CHN[1], 1);
        gpio_write(&gpio, 1);
#endif

        reg_val = PWM_CLKSRC_REG;
        clk_div = reg_val & 0x7F;
        clk_sel = ((reg_val& 0xFF) >> 7);
        pwm_src_deft = (reg_val & (~((0x1 << 24) | 0x7F))) | (0x1 << 7);   //pwt_clk:disable divider:0 pwt_fast_clk:20mhz
        //mux to gpio
        //disable PWM0 & PWM3

        EXIF_PWM_EN_REG &= ~(0x9 << 8);   //disable pwm0 pwm3
        if(pwm0_duty == 1){
            PWM_DIFF0_REG = (p_ticks_h << 16 | p_ticks_l);   //pwm1:0xFF 0x00
        }
        else if(pwm3_duty == 1){
            PWM_DIFF3_REG = (p_ticks_h << 16 | p_ticks_l);
        }
#if 0
        //reset clk
        if((clk_sel == 0) | (clk_div >= 0x2)){
            //---create a pwm clk---
            PWM_CLKSRC_REG &= ~(0x1 << 24);
            PWM_CLKSRC_REG = pwm_src_deft;
            PWM_CLKSRC_REG |= (0x1 << 24);   //pwt_clk:disable divider:0 pwt_fast_clk:20mhz
        }
        else{
        }
        //recfg clk_div
        if((clk_sel == 0) | (clk_div >= 0x2)){
            //---create a pwm clk---
            PWM_CLKSRC_REG &= ~(0x1 << 24);
            PWM_CLKSRC_REG |= (clk_sel << 7) | (clk_div);   //divider:clk_div pwt_fast_clk:20mhz
            PWM_CLKSRC_REG |= (0x1 << 24);
        }
        else{
        }
#endif
        //enable pwm 0
        if(pwm0_duty == 1){
            EXIF_PWM_EN_REG |= (0x1 << 8);  //enable pwm0
        }
        else if(pwm3_duty == 1){
            EXIF_PWM_EN_REG |= (0x1 << 11);
        }
#if 0
        if((clk_sel == 0) | (clk_div >= 0x1)){
            if((clk_sel == 1) & (clk_div == 0x1)){
                PWM_CLKSRC_REG &= ~(0x1 << 24);
                PWM_CLKSRC_REG |= (0x1 << 24);
            }
            else{
                //---create a pwm clk---
                PWM_CLKSRC_REG &= ~(0x1 << 24);
                PWM_CLKSRC_REG = pwm_src_deft;
                PWM_CLKSRC_REG |= (0x1 << 24);
            }
        }
        else{
        }

        if((clk_sel == 0) | (clk_div >= 0x2)){
        //---create a pwm clk---
        PWM_CLKSRC_REG &= ~(0x1 << 24);
        PWM_CLKSRC_REG |= (clk_sel << 7) | (clk_div);
        PWM_CLKSRC_REG |= (0x1 << 24);
        }
        else{
        }
#endif

//    if (obj->period_ticks == obj->pulsewidth_ticks) {
//        PWM_DIFF0_REG = ((obj->pulsewidth_ticks - 1) << 16);
//    } else {
        PWM_DIFF0_REG = (pwm0_period - pwm0_sync_pulsewidth - 1) |
                        ((pwm0_sync_pulsewidth - 1) << 16);
//    }
//	    PWM_DIFF3_REG = (pwm3_sync_pulsewidth - 1) |
//		    		  ((pwm3_period - pwm3_sync_pulsewidth - 1) << 16);
        PWM_DIFF3_REG = (pwm3_period - pwm3_sync_pulsewidth - 1) |
                        ((pwm3_sync_pulsewidth - 1) << 16);

        EXIF_PWM_EN_REG |= (0x9 << 8);
//        gpio_t gpio;
        if (pwm0_period == pwm0_sync_pulsewidth) {
            gpio_init_out_ex(&gpio, PWM_SYNC_CHN[0],1);
            gpio_write(&gpio, 1);
        }
        else if(0 == pwm0_sync_pulsewidth){
            gpio_init_out(&gpio, PWM_SYNC_CHN[0]);
            gpio_write(&gpio, 0);
        }
        else {
            pinmap_pinout(PWM_SYNC_CHN[0], PinMap_PWM);
        }

        if (pwm3_period == pwm3_sync_pulsewidth) {
            gpio_init_out_ex(&gpio, PWM_SYNC_CHN[1], 1);
            gpio_write(&gpio, 1);
        }
        else if (0 == pwm3_sync_pulsewidth){
            gpio_init_out(&gpio, PWM_SYNC_CHN[1]);
            gpio_write(&gpio, 0);
        }
        else {
            pinmap_pinout(PWM_SYNC_CHN[1], PinMap_PWM);
        }
        pwm_sync_cfged = 0;
		core_util_critical_section_exit();
    }
}
void pwmout_write(pwmout_t* obj, float value)
{
    uint32_t ticks;

    if (value < 0.0f) {
        value = 0.0;
    } else if (value > 1.0f) {
        value = 1.0;
    }

    /* Set channel match to percentage */
    if(PWM_5 >= (PWMName)obj->channel){
        ticks = (uint32_t)((float)(obj->period_ticks) * value);
    }
    else if(PWM_6 <= (PWMName)obj->channel){
        obj->period_ticks = 0xFF;
        ticks = (uint32_t)((float)(obj->period_ticks) * value);
    }
    if (0 == ticks) {
        obj->pulsewidth_ticks = 0;
    } else {
        /* Update Hw reg */
        if(ticks != obj->pulsewidth_ticks) {
            obj->pulsewidth_ticks = ticks;
            pwmout_update_cfgreg(obj);
        }
    }
    /* Check if already started */
    if(is_pwmout_started(obj)){
        if (PWM_4 != (PWMName)obj->channel){
        pwmout_stop(obj);
        }
    }
    /* Start PWM module */
    pwmout_start(obj);
}

void pwmout_write_ext(pwmout_t* obj, uint32_t value)
{
    uint32_t ticks;


    ticks = value;
    if (0 == ticks) {
        obj->pulsewidth_ticks = 0;
    } else {
        /* Update Hw reg */
        if(ticks != obj->pulsewidth_ticks) {
            obj->pulsewidth_ticks = ticks;
            pwmout_cfgreg(obj);
        }
    }
/* Check if already started */
    if(is_pwmout_started(obj)){
        if (PWM_4 != (PWMName)obj->channel){
        pwmout_stop(obj);
        }
    }
    /* Start PWM module */
    pwmout_start(obj);
}

//only use in 10M source clk mode
//gpio0 ->PB_8  gpio1 ->PC_1
void pwmout_diff_write(pwmout_t *obj, float value)
{
    uint32_t ticks;
    uint32_t p_ticks_h = 0xFFF;
    uint32_t p_ticks_l = 0x0;
    uint32_t reg_val;
    uint32_t pwm_src_deft;
    uint8_t clk_div;
    uint8_t clk_sel; //0:32K 1:20M
    int err_code = 0;

    MBED_ASSERT(PWM_0 >= (PWMName)(obj->channel));

    if (obj->period_ticks < obj->period_ticks_min) {
            obj->period_ticks = obj->period_ticks_min;
            err_code = 1; // too small period
    }
    if (obj->period_ticks > obj->period_ticks_max) {
        obj->period_ticks = obj->period_ticks_max;
        err_code = 2; // too large period
        if (obj->pulsewidth_ticks > obj->period_ticks_max) {
            obj->pulsewidth_ticks = obj->period_ticks_max;
            err_code = 3; // too large period & duty
        }
    }
    if (0 != err_code) {
        mbed_error_printf("PwmCfgErr:%d\r\n",err_code);
    }

    if (value < 0.0f) {
        value = 0.0;
    } else if (value > 1.0f) {
        value = 1.0;
    }

    ticks = (uint32_t)((float)(obj->period_ticks) * value);

    if(ticks != obj->pulsewidth_ticks) {
        obj->pulsewidth_ticks = ticks;
    }

    reg_val = PWM_CLKSRC_REG;
    clk_div = reg_val & 0x7F;
    clk_sel = (reg_val & 0xFF) >> 7;
    pwm_src_deft = (reg_val & (~((0x1 << 24) | 0x7F))) | (0x1 << 7);
    //mux to gpio
    //disable PWM pwm to 0
    EXIF_PWM_EN_REG &= ~(0x9 << 8);
    PWM_DIFF0_REG = (p_ticks_h << 16 | p_ticks_l);
    if((clk_sel == 0) | (clk_div >= 0x2)){
        //---create a pwm clk---
        PWM_CLKSRC_REG &= ~(0x1 << 24);
        PWM_CLKSRC_REG = pwm_src_deft;
        PWM_CLKSRC_REG |= (0x1 << 24);
    }
    else{
    }

    if((clk_sel == 0) | (clk_div >= 0x2)){
    //---create a pwm clk---
    PWM_CLKSRC_REG &= ~(0x1 << 24);
    PWM_CLKSRC_REG |= (clk_sel << 7) | (clk_div);
    PWM_CLKSRC_REG |= (0x1 << 24);
    }
    else{
    }
    //enable pwm 0
    EXIF_PWM_EN_REG |= (0x1 << 8);

    if((clk_sel == 0) | (clk_div >= 0x1)){
        if((clk_sel == 1) & (clk_div == 0x1)){
            PWM_CLKSRC_REG &= ~(0x1 << 24);
            PWM_CLKSRC_REG |= (0x1 << 24);
        }
        else{
            //---create a pwm clk---
            PWM_CLKSRC_REG &= ~(0x1 << 24);
            PWM_CLKSRC_REG = pwm_src_deft;
            PWM_CLKSRC_REG |= (0x1 << 24);
        }
    }
    else{
    }

    if((clk_sel == 0) | (clk_div >= 0x2)){
    PWM_CLKSRC_REG &= ~(0x1 << 24);
    PWM_CLKSRC_REG |= (clk_sel << 7) | (clk_div);
    PWM_CLKSRC_REG |= (0x1 << 24);
    }
    else{
    }

    if (obj->period_ticks == obj->pulsewidth_ticks) {
        PWM_DIFF0_REG = ((obj->pulsewidth_ticks - 1) << 16);
    } else {
        PWM_DIFF0_REG = (obj->period_ticks - obj->pulsewidth_ticks - 1) |
                  ((obj->pulsewidth_ticks - 1) << 16);
    }
    PWM_DIFF3_REG = (obj->pulsewidth_ticks - 1) |
                  ((obj->period_ticks - obj->pulsewidth_ticks - 1) << 16);

    EXIF_PWM_EN_REG |= (0x9 << 8);

    if (obj->period_ticks == obj->pulsewidth_ticks) {
        gpio_t gpio;
        //gpio_init_out_ex(&gpio, PB_8, 1);
		gpio_init_out_ex(&gpio, PWM_DIFF_CHN[0],1);
        gpio_write(&gpio, 1);
		gpio_init_out(&gpio, PWM_DIFF_CHN[1]);
        gpio_write(&gpio, 0);
        //mbed_error_printf("100\n");
    } else if (0 == obj->pulsewidth_ticks) {
        gpio_t gpio;
        gpio_init_out(&gpio, PWM_DIFF_CHN[0]);
        gpio_write(&gpio, 0);
        gpio_init_out_ex(&gpio, PWM_DIFF_CHN[1], 1);
        gpio_write(&gpio, 1);
        //mbed_error_printf("0\n");
    } else {
        pinmap_pinout(PWM_DIFF_CHN[0], PinMap_PWM);
        pinmap_pinout(PWM_DIFF_CHN[1], PinMap_PWM);
    }
}

float pwmout_read(pwmout_t* obj)
{
    float v = (float)(obj->pulsewidth_ticks) / (float)(obj->period_ticks);
    return (v > 1.0f) ? (1.0f) : (v);
}

void pwmout_period(pwmout_t* obj, float seconds)
{
    pwmout_period_us(obj, seconds * 1000000.0f);
}

void pwmout_period_ms(pwmout_t* obj, int ms)
{
    pwmout_period_us(obj, ms * 1000);
}

/* Set the PWM period, keeping the duty cycle the same. */
void pwmout_period_us(pwmout_t* obj, int us)
{
    uint32_t ticks;
    MBED_ASSERT(PWM_5 >= (PWMName)(obj->channel));

    /* Check if already started */
    if(is_pwmout_started(obj))
        pwmout_stop(obj);

    /* Calculate number of ticks */
    ticks = (uint64_t)(*(obj->base_clk_ptr)) * us / 1000000;

    if(ticks != obj->period_ticks) {
        float duty_ratio;

        /* Preserve the duty ratio */
        if (0 == obj->period_ticks)
            duty_ratio = 0.5f;
        else
            duty_ratio = (float)obj->pulsewidth_ticks / (float)obj->period_ticks;
        obj->period_ticks = ticks;
        obj->pulsewidth_ticks = (uint32_t)(ticks * duty_ratio);
        MBED_ASSERT(obj->period_ticks >= obj->pulsewidth_ticks);

        pwmout_update_cfgreg(obj);
    }

    /* Start PWM module */
    pwmout_start(obj);
}

void pwmout_pulsewidth(pwmout_t* obj, float seconds)
{
    pwmout_pulsewidth_us(obj, seconds * 1000000.0f);
}

void pwmout_pulsewidth_ms(pwmout_t* obj, int ms)
{
    pwmout_pulsewidth_us(obj, ms * 1000);
}

/* Set the PWM pulsewidth, keeping the period the same. */
void pwmout_pulsewidth_us(pwmout_t* obj, int us)
{
    uint32_t ticks;

    MBED_ASSERT(PWM_7 >= (PWMName)(obj->channel));

    /* Check if already started */
    if(is_pwmout_started(obj))
        pwmout_stop(obj);

    /* Calculate number of ticks */
    ticks = (uint64_t)(*(obj->base_clk_ptr)) * us / 1000000;

    if(ticks != obj->pulsewidth_ticks) {
        obj->pulsewidth_ticks = ticks;
        MBED_ASSERT(obj->period_ticks >= obj->pulsewidth_ticks);

        pwmout_update_cfgreg(obj);
    }

    /* Start PWM module */
    pwmout_start(obj);
}

static uint8_t is_pwmout_started(pwmout_t* obj)
{
    uint8_t retVal = 0;
    uint32_t reg_val;

    MBED_ASSERT(PWM_7 >= (PWMName)(obj->channel));

    if(PWM_3 >= (PWMName)obj->channel) {
        reg_val = (EXIF_PWM_EN_REG >> 8) & 0x0FUL;
        if(reg_val & (0x01UL << obj->channel))
            retVal = 1;
    } else if(PWM_4 == (PWMName)obj->channel) {
        if(*(obj->CFGR) & (0x01UL << 1))
            retVal = 1;
    } else if(PWM_5 == (PWMName)obj->channel) {
        retVal = 1;
    } else {
        if(*(obj->CFGR) & (0x01UL << 16))
            retVal = 1;
    }

    return retVal;
}

static void pwmout_start(pwmout_t* obj)
{
    MBED_ASSERT(PWM_7 >= (PWMName)(obj->channel));

    if (obj->period_ticks == obj->pulsewidth_ticks) {
        gpio_t gpio;
        gpio_init_out_ex(&gpio, obj->pin,1);
        gpio_write(&gpio, 1);
        //mbed_error_printf("100\n");
    } else if (0 == obj->pulsewidth_ticks) {
        gpio_t gpio;
        gpio_init_out(&gpio, obj->pin);
        gpio_write(&gpio, 0);
        //mbed_error_printf("0\n");
    } else {
        pinmap_pinout(obj->pin, PinMap_PWM);
    }

    if(PWM_3 >= (PWMName)obj->channel) {
        EXIF_PWM_EN_REG |= (0x01UL << (8 + obj->channel));
    } else if(PWM_4 == (PWMName)obj->channel) {
        *(obj->CFGR) |= 0x01UL;
    } else if(PWM_5 == (PWMName)obj->channel) {
        /* Nothing to be done */
    } else {
        *(obj->CFGR) |= (0x01UL << 16);
    }
}

static void pwmout_stop(pwmout_t* obj)
{
    MBED_ASSERT(PWM_7 >= (PWMName)(obj->channel));

    if (obj->period_ticks == obj->pulsewidth_ticks) {
        gpio_t gpio;
        gpio_init_out(&gpio, obj->pin);
        gpio_write(&gpio, 0);
    }

    if(PWM_3 >= (PWMName)obj->channel) {
        EXIF_PWM_EN_REG &= ~(0x01UL << (8 + obj->channel));
    } else if(PWM_4 == (PWMName)(obj->channel)) {
        *(obj->CFGR) &= ~0x01UL;
    } else if(PWM_5 == (PWMName)(obj->channel)) {
        /* Nothing to be done */
    } else {
        *(obj->CFGR) &= ~(0x01UL << 16);
    }
}

static void pwmout_update_cfgreg(pwmout_t* obj)
{
    int err_code = 0;
    if (obj->period_ticks < obj->period_ticks_min) {
        obj->period_ticks = obj->period_ticks_min;
        err_code = 1; // too small duty & period
    }
    if (obj->period_ticks > obj->period_ticks_max) {
        obj->period_ticks = obj->period_ticks_max;
        err_code = 2; // too large period
        if (obj->pulsewidth_ticks > obj->period_ticks_max) {
            obj->pulsewidth_ticks = obj->period_ticks_max;
            err_code = 3; // too large period & duty
        }
    }
    if (0 != err_code) {
        mbed_error_printf("PwmCfgErr:%d\r\n",err_code);
    }
    if(PWM_3 >= (PWMName)(obj->channel)) {
        if (obj->period_ticks == obj->pulsewidth_ticks) {
            *(obj->CFGR) = ((obj->pulsewidth_ticks - 1) << 16);
        } else {
            *(obj->CFGR) = ((obj->period_ticks - obj->pulsewidth_ticks - 1) & 0xFFFFUL) |
                ((obj->pulsewidth_ticks - 1) << 16);
        }
    } else if(PWM_4 == (PWMName)(obj->channel)) {
        *(obj->CFGR) = ((obj->pulsewidth_ticks & ~0x07UL) << ( 4 - 3)) |
                       ((obj->period_ticks     & ~0x07UL) << (16 - 3));
    } else if(PWM_5 == (PWMName)(obj->channel)) {
        /* TBD */
        uint32_t reg_val = *(obj->CFGR) & ~(0xFUL << 4) & ~(0x7UL << 16);
        uint32_t lpg_field_ontime = (0x01UL << 4) & (0xFUL << 4); // to be confirm
        uint32_t lpg_field_period = (obj->period_ticks << 4) & (0x7UL << 16);
        *(obj->CFGR) = reg_val | lpg_field_ontime | lpg_field_period;
    } else if(PWM_6 == (PWMName)(obj->channel)){
        uint32_t reg_val = *(obj->CFGR) & ~(0xFF);
        *(obj->CFGR) = reg_val | (obj->pulsewidth_ticks);//1~254
    } else{
        //PWM_7
        uint32_t reg_val = *(obj->CFGR) & ~(0xFF);
        *(obj->CFGR) = reg_val | (obj->pulsewidth_ticks);//1~254
    }
}
static void pwmout_cfgreg(pwmout_t* obj)
{
  __NOP();

  if(PWM_3 >= (PWMName)(obj->channel)) {

        if (obj->period_ticks == obj->pulsewidth_ticks) {
            *(obj->CFGR) = ((obj->pulsewidth_ticks - 1) << 16);
        } else {
            *(obj->CFGR) = ((obj->period_ticks - obj->pulsewidth_ticks - 1) & 0xFFFFUL) |
                ((obj->pulsewidth_ticks - 1) << 16);
        }
    } else if(PWM_4 == (PWMName)(obj->channel)) {
        *(obj->CFGR) = ((obj->pulsewidth_ticks & ~0x07UL) << ( 4 - 3)) |
                       ((obj->period_ticks     & ~0x07UL) << (16 - 3));
    } else if(PWM_5 == (PWMName)(obj->channel)) {
        /* TBD */
        uint32_t reg_val = *(obj->CFGR) & ~(0xFUL << 4) & ~(0x7UL << 16);
        uint32_t lpg_field_ontime = (0x01UL << 4) & (0xFUL << 4); // to be confirm
        uint32_t lpg_field_period = (obj->period_ticks << 4) & (0x7UL << 16);
        *(obj->CFGR) = reg_val | lpg_field_ontime | lpg_field_period;
    } else if(PWM_6 == (PWMName)(obj->channel)){
        uint32_t reg_val = *(obj->CFGR) & ~(0xFF);
        *(obj->CFGR) = reg_val | (obj->pulsewidth_ticks);//1~254
    } else{
        //PWM_7
        uint32_t reg_val = *(obj->CFGR) & ~(0xFF);
        *(obj->CFGR) = reg_val | (obj->pulsewidth_ticks);//1~254
    }
}


