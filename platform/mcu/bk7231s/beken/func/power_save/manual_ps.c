#include "manual_ps.h"
#include "manual_ps_pub.h"
#include "gpio_pub.h"
#include "power_save_pub.h"
#include "sys_ctrl_pub.h"
#include "target_util_pub.h"


#if PS_SUPPORT_MANUAL_SLEEP
#include "icu_pub.h"
#include "mcu_ps_pub.h"
#include "BkDriverGpio.h"
#include "rwnx.h"

extern void phy_init_after_wakeup(void);


void general_sleep_wakeup_with_peri( UINT8 uart2_wk, UINT32 gpio_index_map, UINT32 gpio_edge_map)
{
    UINT32 reg, ret;
    UINT32 param = 0;
    UINT32 i;
    UINT32    gpio_stat_cfg[32] = {0};

    if(power_save_ps_mode_get() != PS_NO_PS_MODE)
    {
        os_printf("can't peri ps,ps in mode %d!\r\n", power_save_ps_mode_get());
        return ;
    }

    for (i = 0; i < GPIONUM; i++)
    {
        if (gpio_index_map & (0x01UL << i))
        {
            gpio_stat_cfg[i] = REG_READ(0x00802800 + i * 4);

            if(gpio_index_map & gpio_edge_map & (0x01UL << i))
            {
                BkGpioEnableIRQ(i, 0x3, NULL, NULL);
            }
            else
            {
                BkGpioEnableIRQ(i, 0x2, NULL, NULL);
            }
            os_printf("set peri wkup gpio %d\r\n", i);
        }
    }

#if(PS_XTAL26M_EN == 1)
    param = LPO_SELECT_32K_DIV;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param);
#endif

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    ps_pwm0_disable();
    param = 0;
    if(uart2_wk == 1)
    {
        param |= ( UART2_ARM_WAKEUP_EN_BIT);
        os_printf("set peri wkup uart2\r\n");
    }

    if(gpio_index_map)
        param |= ( GPIO_ARM_WAKEUP_EN_BIT);

    os_printf("enter peri ps\r\n");
    sddev_control(ICU_DEV_NAME, CMD_ARM_WAKEUP, &param);
    param = (0xfffff & ~PWD_UART2_CLK_BIT);

    wifi_mac_state_set_idle();
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_SLEEP, &param);
    delay(2);
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_WAKEUP, 0);
    wifi_mac_state_set_prev();

    for (i = 0; i < GPIONUM; i++)
    {
        if (gpio_index_map & (0x01UL << i))
        {
            BkGpioDisableIRQ(i);
            REG_WRITE((0x00802800 + i * 4), gpio_stat_cfg[i]);
        }
    }

    ps_pwm0_enable();
    GLOBAL_INT_RESTORE();
    os_printf("exit peri ps\r\n");
}

void general_sleep_wakeup_with_timer(UINT32 sleep_time)
{
    UINT32 reg;
    UINT32 param;
    UINT32 wakeup_timer;

    if(power_save_ps_mode_get() != PS_NO_PS_MODE)
    {
        os_printf("can't pwm ps,ps in mode %d!\r\n", power_save_ps_mode_get());
        return ;
    }

    if(sleep_time != 0xffffffff)
    {
        os_printf("sleep with pwm,%d ms\r\n", sleep_time);
        param = LPO_SELECT_ROSC;
        sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param);

        wakeup_timer = (sleep_time * 32);
        if(wakeup_timer > 65535)
            wakeup_timer = 65535;
        else if(wakeup_timer < 160)
            wakeup_timer = 160;

        delay(5);
        ps_pwm0_suspend_tick(wakeup_timer);
    }
    else
    {
        ps_pwm0_disable();
        os_printf("sleep forever\r\n");
    }

    os_printf("enter pwm ps\r\n");
    param = PWM_ARM_WAKEUP_EN_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ARM_WAKEUP, &param);
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    param = (0xfffff & (~PWD_PWM0_CLK_BIT) & (~PWD_UART2_CLK_BIT));

    wifi_mac_state_set_idle();
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_SLEEP, &param);
    delay(5);
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_WAKEUP, 0);
    wifi_mac_state_set_prev();

    phy_init_after_wakeup();
    GLOBAL_INT_RESTORE();
    ps_pwm0_resume_tick();
    os_printf("exit pwm ps\r\n");
}


void general_sleep_wakeup_with_gpio(UINT32 gpio_index)
{
    UINT32 reg;
    UINT32 param;

    if(power_save_ps_mode_get() != PS_NO_PS_MODE)
    {
        os_printf("can't gpio ps,ps in mode %d!\r\n", power_save_ps_mode_get());
        return ;
    }

    os_printf("enter gpio ps\r\n");
    bk_gpio_config_input_pdwn(gpio_index);
    BkGpioEnableIRQ(gpio_index, 0x2, NULL, NULL);

    param = LPO_SELECT_ROSC;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param);

    param = (GPIO_ARM_WAKEUP_EN_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ARM_WAKEUP, &param);
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    param = (0xfffff & (~PWD_UART2_CLK_BIT));

    wifi_mac_state_set_idle();
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_SLEEP, &param);
    delay(5);
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_WAKEUP, 0);
    wifi_mac_state_set_prev();

    phy_init_after_wakeup();
    GLOBAL_INT_RESTORE();
    os_printf("exit gpio ps\r\n");
}

/*
 *  idle sleep
 *
*/
void idle_sleep_wakeup_with_timer(UINT32 sleep_time)
{
    UINT32 reg, param;
    PS_DEEP_CTRL_PARAM deep_param;

    if(power_save_ps_mode_get() != PS_NO_PS_MODE)
    {
        os_printf("can't pwm ps,ps in mode %d!\r\n", power_save_ps_mode_get());
        return ;
    }


    if(sleep_time != 0xffffffff)
    {
        os_printf("idle sleep with rtc,%d ms\r\n", sleep_time);
        deep_param.param = ((sleep_time * 102400) / 3125) ;

        if(deep_param.param > 0xffff) //only 16 bit
            deep_param.param = 0xffff;
        else if(deep_param.param < 32)
            deep_param.param = 32;
    }
    else
    {
        os_printf("idle sleep forever\r\n");
        deep_param.param = 0xffffffff;
    }

    param = LPO_SELECT_ROSC;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param);

    deep_param.deep_wkway = PS_DEEP_WAKEUP_RTC;
    os_printf("enter rtc ps\r\n");
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    ps_pwm0_disable();

    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_SLEEP, &deep_param);
    delay(5);
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_WAKEUP, 0);
    phy_init_after_wakeup();

    ps_pwm0_enable();

    ke_evt_set(KE_EVT_RESET_BIT);
    GLOBAL_INT_RESTORE();
    os_printf("exit rtc ps\r\n");
}


/** @brief  Request idle sleep,and wakeup by gpio.
 */
void idle_sleep_wakeup_with_gpio(UINT32 gpio_index_map, UINT32 gpio_edge_map)
{
    UINT32 reg, param;
    PS_DEEP_CTRL_PARAM deep_param;
    UINT32 i;

    if(power_save_ps_mode_get() != PS_NO_PS_MODE)
    {
        os_printf("can't gpio ps,ps in mode %d!\r\n", power_save_ps_mode_get());
        return ;
    }

    for (i = 0; i < GPIONUM; i++)
    {
        if (gpio_index_map & (0x01UL << i))
        {
            if(gpio_index_map & gpio_edge_map & (0x01UL << i))
            {
                param = GPIO_CFG_PARAM(i, GMODE_INPUT_PULLUP);
                sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
            }
            else
            {
                param = GPIO_CFG_PARAM(i, GMODE_INPUT_PULLDOWN);
                sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
            }
        }
    }

    os_printf("enter idle ps %x %x\r\n", gpio_index_map, gpio_edge_map);

    param = LPO_SELECT_ROSC;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param);

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    ps_pwm0_disable();

    deep_param.deep_wkway = PS_DEEP_WAKEUP_GPIO;
    deep_param.param = gpio_index_map;
    deep_param.gpio_lv = gpio_edge_map;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_SLEEP, &deep_param);
    delay(5);
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_WAKEUP, 0);
    phy_init_after_wakeup();

    ps_pwm0_enable();

    ke_evt_set(KE_EVT_RESET_BIT);
    GLOBAL_INT_RESTORE();

    os_printf("exit gpio ps\r\n");
}


/** @brief  Request power save,and wakeup some time later
 *  @mode   0:normal sleep,600uA,connect no influence
 *          1:idle sleep,20uA,but wifi need reconnect
 *  @param  sleep_time: Sleep time with milliseconds.
 *              if 0xffffffff not wakeup
 */
void bk_wlan_ps_wakeup_with_timer(MANUAL_MODE mode, UINT32 sleep_time)
{
    if(mode == MANUAL_MODE_NORMAL)
        general_sleep_wakeup_with_timer(sleep_time);
    else if(mode == MANUAL_MODE_IDLE)
        idle_sleep_wakeup_with_timer(sleep_time);
}

/** @brief  Request power save,and wakeup by uart if uart2_wk=1,nd wakeup by gpio from bitmap of gpio_index_map.
 */
void bk_wlan_ps_wakeup_with_peri( UINT8 uart2_wk, UINT32 gpio_index_map, UINT32 gpio_edge_map)
{
    general_sleep_wakeup_with_peri(uart2_wk, gpio_index_map, gpio_edge_map);
}

/** @brief  Request power save,and wakeup by gpio
 *  @mode   0:normal sleep,600uA,connect no influence
 *          1:idle sleep,20uA,but wifi need reconnect
 *  @param  gpio_index_map:The gpio bitmap which set 1 enable wakeup deep sleep.
 *              gpio_index_map is hex and every bits is map to gpio0-gpio31.
 *          gpio_edge_map:The gpio edge bitmap for wakeup gpios,
 *              gpio_edge_map is hex and every bits is map to gpio0-gpio31.
 *              0:rising,1:falling.
 */
void bk_wlan_ps_wakeup_with_gpio(MANUAL_MODE mode, UINT32 gpio_index_map, UINT32 gpio_edge_map)
{
    if(mode == MANUAL_MODE_NORMAL)
        general_sleep_wakeup_with_peri(0, gpio_index_map, gpio_edge_map);
    else if(mode == MANUAL_MODE_IDLE)
        idle_sleep_wakeup_with_gpio(gpio_index_map, gpio_edge_map);
}


#endif

#if CFG_USE_DEEP_PS

/*
 *  deep sleep
 *
*/
void deep_sleep_wakeup_with_timer(UINT32 sleep_time)
{
    UINT32 reg;
    PS_DEEP_CTRL_PARAM deep_param;

    if(power_save_ps_mode_get() != PS_NO_PS_MODE)
    {
        os_printf("can't pwm ps,ps in mode %d!\r\n", power_save_ps_mode_get());
        return ;
    }


    if(sleep_time != 0xffffffff)
    {
        os_printf("sleep with rtc,%d ms\r\n", sleep_time);
        deep_param.param = ((sleep_time * 102400) / 3125) ;

        if(deep_param.param > 0xffff) //only 16 bit
            deep_param.param = 0xffff;
        else if(deep_param.param < 32)
            deep_param.param = 32;
    }
    else
    {
        os_printf("sleep forever\r\n");
        deep_param.param = 0xffffffff;
    }

    deep_param.deep_wkway = PS_DEEP_WAKEUP_RTC;
    os_printf("enter rtc ps\r\n");
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_RTOS_DEEP_SLEEP, &deep_param);
    delay(5);
    GLOBAL_INT_RESTORE();
    os_printf("exit pwm ps\r\n");
}


/** @brief  Request deep sleep,and wakeup by gpio.
 *          First user must set gpio to GMODE_INPUT,GMODE_INPUT_PULLUP,
 *          or GMODE_INPUT_PULLDOWN,as required.
 *
 *  @param  gpio_index_map:The gpio bitmap which set 1 enable wakeup deep sleep.
 *              gpio_index_map is hex and every bits is map to gpio0-gpio31.
 *          gpio_edge_map:The gpio edge bitmap for wakeup gpios,
 *              gpio_edge_map is hex and every bits is map to gpio0-gpio31.
 *              0:rising,1:falling.
 */
void deep_sleep_wakeup_with_gpio(UINT32 gpio_index_map, UINT32 gpio_edge_map)
{
    UINT32 reg, param;
    PS_DEEP_CTRL_PARAM deep_param;
    UINT32 i;

    if(power_save_ps_mode_get() != PS_NO_PS_MODE)
    {
        os_printf("can't gpio ps,ps in mode %d!\r\n", power_save_ps_mode_get());
        return ;
    }

    os_printf("enter deep ps %x %x\r\n", gpio_index_map, gpio_edge_map);

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    deep_param.deep_wkway = PS_DEEP_WAKEUP_GPIO;
    deep_param.param = gpio_index_map;
    deep_param.gpio_lv = gpio_edge_map;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_RTOS_DEEP_SLEEP, &deep_param);
    delay(5);
    GLOBAL_INT_RESTORE();
    os_printf("exit gpio ps\r\n");
}
#endif

