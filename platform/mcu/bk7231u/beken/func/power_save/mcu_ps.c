#include "intc_pub.h"
#include "pwm_pub.h"
#include "rw_pub.h"
#include "rtos_pub.h"
#include "arm_arch.h"
#include "sys_ctrl_pub.h"
#include "mcu_ps.h"
#include "mcu_ps_pub.h"
#include "power_save_pub.h"
#include "ps_debug_pub.h"
#include "target_util_pub.h"
#include "icu_pub.h"
#include "bk_timer_pub.h"
#include "drv_model_pub.h"

#if CFG_USE_MCU_PS

static MCU_PS_INFO mcu_ps_info =
{
    .mcu_ps_on = 0,
    .peri_busy_count = 0,
    .mcu_prevent = 0,

};

#if (CFG_SUPPORT_ALIOS & CFG_USE_STA_PS)
static UINT32 sleep_us, start_sleep, wkup_type;
#endif

void peri_busy_count_add(void )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    mcu_ps_info.peri_busy_count ++;
    GLOBAL_INT_RESTORE();
}

void peri_busy_count_dec(void )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    mcu_ps_info.peri_busy_count --;
    GLOBAL_INT_RESTORE();
}

UINT32 peri_busy_count_get(void )
{
    return mcu_ps_info.peri_busy_count;
}

void mcu_prevent_set(UINT32 prevent )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    mcu_ps_info.mcu_prevent |= prevent;
    GLOBAL_INT_RESTORE();
}

void mcu_prevent_clear(UINT32 prevent )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    mcu_ps_info.mcu_prevent &= ~ prevent;
    GLOBAL_INT_RESTORE();
}

UINT32 mcu_prevent_get(void )
{
    return mcu_ps_info.mcu_prevent;
}

void mcu_ps_enable(void )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    mcu_ps_info.mcu_ps_on = 1;
    GLOBAL_INT_RESTORE();
}

void mcu_ps_disable(void )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    mcu_ps_info.mcu_ps_on = 0;
    GLOBAL_INT_RESTORE();
}


UINT32 mcu_power_save(UINT32 sleep_tick)
{
    UINT32 sleep_ms, sleep_pwm_t, param, miss_ticks = 0;
    UINT32 real_sleep_ms, start_sleep, ret, front_ms, wkup_type,reg;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(mcu_ps_info.mcu_ps_on == 1
            && (peri_busy_count_get() == 0)
            && (mcu_prevent_get() == 0)
#if CFG_USE_STA_PS
#if NX_POWERSAVE
            && (txl_sleep_check())
#endif
            && (! power_save_use_timer0())
#endif
      )
    {
        uart_wait_tx_over();
        sleep_ms = sleep_tick * MS_TO_TICK;

        sleep_pwm_t = (sleep_ms * 32);
        if(sleep_pwm_t > 65535)
            sleep_pwm_t = 65535;
        else if(sleep_pwm_t < 160)
            sleep_pwm_t = 160;

#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
        ps_pwm0_disable();
        ps_timer3_enable(sleep_pwm_t);

#else
        ps_pwm0_suspend_tick(sleep_pwm_t);
#endif

#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
        param = (0xfffff & (~PWD_TIMER_32K_CLK_BIT) & (~PWD_UART2_CLK_BIT)
#if (!PS_NO_USE_UART1_WAKE)
            & (~PWD_UART1_CLK_BIT)
#endif
            );
#else
        param = (0xfffff & (~PWD_PWM0_CLK_BIT) & (~PWD_UART2_CLK_BIT)
#if (!PS_NO_USE_UART1_WAKE)
            & (~PWD_UART1_CLK_BIT)
#endif
            );
#endif
        sctrl_mcu_sleep(param);
#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
        ps_timer3_measure_prepare();
#endif
        wkup_type = sctrl_mcu_wakeup();

#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
        miss_ticks =  ps_timer3_disable() / MS_TO_TICK;
        ps_pwm0_enable();
#else
        miss_ticks = sleep_tick;
        ps_pwm0_resume_tick();
#endif
    }
    else
    {
    }

    fclk_update_tick(miss_ticks);

    GLOBAL_INT_RESTORE();
    ASSERT(miss_ticks >= 0);
    return miss_ticks;
}

#if (CFG_SUPPORT_ALIOS & CFG_USE_STA_PS)
int aos_mcu_ps_timer_start(UINT32 tm_us)
{
    UINT32 sleep_ms, sleep_pwm_t, param, reg;
    if(mcu_ps_info.mcu_ps_on == 1
            && (peri_busy_count_get() == 0)
            && (mcu_prevent_get() == 0)
            && (txl_sleep_check()
                && (! power_save_use_timer0()))
      )
    {
        uart_wait_tx_over();

        sleep_ms = tm_us / 1000;

        sleep_pwm_t = (sleep_ms * 32);
        if(sleep_pwm_t > 65535)
            sleep_pwm_t = 65535;
        else if(sleep_pwm_t < 160)
            sleep_pwm_t = 160;

        sleep_us = tm_us;
#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
        ps_pwm0_disable();
        ps_timer3_enable(sleep_pwm_t);

#else
        ps_pwm0_suspend_tick(sleep_pwm_t);
#endif
        return 0;
    }
    else
    {
        return -1;
    }

}

void aos_mcu_ps_sleep()
{
    UINT32 param;
    GLOBAL_INT_DECLARATION();

#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
    param = (0xfffff & (~PWD_TIMER_32K_CLK_BIT) & (~PWD_UART2_CLK_BIT)
#if (!PS_NO_USE_UART1_WAKE)
             & (~PWD_UART1_CLK_BIT)
#endif
            );
#else
    param = (0xfffff & (~PWD_PWM0_CLK_BIT) & (~PWD_UART2_CLK_BIT)
#if (!PS_NO_USE_UART1_WAKE)
             & (~PWD_UART1_CLK_BIT)
#endif
            );
#endif
    GLOBAL_INT_DISABLE();
    sctrl_mcu_sleep(param);
#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
    ps_timer3_measure_prepare();
#endif
    wkup_type = sctrl_mcu_wakeup();
    GLOBAL_INT_RESTORE();

}

int aos_mcu_ps_timer_stop(UINT64 *tm_us)
{
    UINT32 reg, real_sleep_us, miss_ticks = 0;

#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
    *tm_us =  ps_timer3_disable() * 1000;
    ps_pwm0_enable();
#else
    miss_ticks = sleep_us;
    *tm_us = miss_ticks;
    ps_pwm0_resume_tick();
#endif
}

#endif

void mcu_ps_dump(void)
{
    os_printf("mcu:%x\r\n", mcu_ps_info.mcu_ps_on);
}

#if (CHIP_U_MCU_WKUP_USE_TIMER)
void timer3_isr(UINT8 param)
{
    //os_printf("t3\r\n");
}

void mcu_init_timer3(void)
{
	timer_param_t param;
	param.channel = 3;
	param.div = 1;
	param.period = 50 * 32;
	param.t_Int_Handler= timer3_isr;

	sddev_control(TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM, &param);
}
#endif

void mcu_ps_init(void)
{
    UINT32 reg;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
    mcu_init_timer3();
#endif

    if(0 == mcu_ps_info.mcu_ps_on)
    {
        sctrl_mcu_init();
        mcu_ps_info.mcu_ps_on = 1;
        mcu_ps_info.peri_busy_count = 0;
        mcu_ps_info.mcu_prevent = 0;
        os_printf("%s\r\n", __FUNCTION__);
    }

    GLOBAL_INT_RESTORE();
}

void mcu_ps_exit(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(1 == mcu_ps_info.mcu_ps_on)
    {
        mcu_ps_info.mcu_ps_on = 0;
        sctrl_mcu_exit();
        mcu_ps_info.peri_busy_count = 0;
        mcu_ps_info.mcu_prevent = 0;
        os_printf("%s\r\n", __FUNCTION__);
    }

    GLOBAL_INT_RESTORE();
}
#else
void peri_busy_count_add(void )
{
}

void peri_busy_count_dec(void )
{
}
void mcu_prevent_set(UINT32 prevent )
{
}

void mcu_prevent_clear(UINT32 prevent )
{
}

#endif

