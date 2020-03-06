#include <stdio.h>
#include "bootloader.h"
#include "include.h"
#include "uart_pub.h"
#include "sys_ctrl_pub.h"
#include "pwm_pub.h"
#include "arm_arch.h"
#include "uart.h"


#define PWD_UART2_CLK_BIT       (1 <<  1)
#define PWD_UART1_CLK_BIT       (1 <<  0)

#define ICU_BASE         (0x00802000)
#define ICU_PERI_CLK_PWD (ICU_BASE + 2 * 4)

#define SCTRL_BASE       (0x00800000)
#define SCTRL_CONTROL    (SCTRL_BASE + 02 * 4)
#define PWM_NEW_BASE     (0x00802A00)

//#define  CFG_SOC_NAME  SOC_BK7231
#if (CFG_SOC_NAME == SOC_BK7231)
#define PWM_BASE                                     (PWM_NEW_BASE)
#else
#define PWM_BASE                                     (PWM_NEW_BASE + 0x20 * 4 )
#endif

#define PWM_CTL                                      (PWM_BASE + 0 * 4)

#define FCLK_PWM_ID           PWM0
#if CFG_SUPPORT_RTT
#define FCLK_DURATION_MS      (1000 / RT_TICK_PER_SECOND)

#define FCLK_SECOND           (RT_TICK_PER_SECOND)
#else
#define FCLK_DURATION_MS      2

#define FCLK_SECOND           (1000/FCLK_DURATION_MS)
#endif

#if (CFG_SOC_NAME == SOC_BK7231)

#define REG_APB_BK_PWMn_CNT_ADDR(n)         (PWM_BASE + 0x08 + 2 * 0x04 * (n))
#define REG_APB_BK_PWMn_CAP_ADDR(n)         (PWM_BASE + 0x0C + 2 * 0x04 * (n))
#else

#define REG_APB_BK_PWMn_END_ADDR(n)         (PWM_BASE + 0x08 + 3 * 0x04 * (n))
#define REG_APB_BK_PWMn_DC_ADDR(n)         (PWM_BASE + 0x0C + 3 * 0x04 * (n))
#define REG_APB_BK_PWMn_CAP_ADDR(n)         (PWM_BASE + 0x10 + 3 * 0x04 * (n))

#endif
void clock_setup()
{
    UINT32 ret;
    UINT32 reg;
    UINT32 param;
    param = PWD_UART2_CLK_BIT;

    reg = REG_READ(ICU_PERI_CLK_PWD);
    reg &= ~((UINT32 )param);
    REG_WRITE(ICU_PERI_CLK_PWD, reg);
}

UINT32 fclk_cal_endvalue(UINT32 mode)
{
    UINT32 value = 1;

    if(PWM_CLK_32K == mode)
    {
        /*32k clock*/
        value = FCLK_DURATION_MS * 32;
    }
    else if(PWM_CLK_26M == mode)
    {
        /*26m clock*/
        value = FCLK_DURATION_MS * 26000;
    }

    return value;
}

void pwm_init()
{
    UINT32 value;

    pwm_param_t param_data;

    pwm_param_t *pwm_param = &param_data;

            param_data.channel         = PWM0;
            param_data.cfg.bits.en     = PWM_ENABLE;
            param_data.cfg.bits.int_en = PWM_INT_EN;
            param_data.cfg.bits.mode   = PMODE_TIMER;

            //param_data.cfg.bits.clk    = PWM_CLK_32K;
            param_data.cfg.bits.clk    = PWM_CLK_26M;

            //param_data.p_Int_Handler   = fclk_hdl;
            param_data.duty_cycle      = 0;
            param_data.end_value       = fclk_cal_endvalue((UINT32)param_data.cfg.bits.clk);


        value = REG_READ(PWM_CTL);
        value = (value & (~(0x0F << (0x04 *  PWM0))))
                | ((pwm_param->cfg.val & 0x0F) << (0x04 * PWM0));
        REG_WRITE(PWM_CTL, value);

#if (CFG_SOC_NAME == SOC_BK7231)
        value = (((UINT32)pwm_param->duty_cycle & 0x0000FFFF) << 16)
                        + ((UINT32)pwm_param->end_value & 0x0000FFFF);
        REG_WRITE(REG_APB_BK_PWMn_CNT_ADDR(PWM0), value);

#else
        value = ((UINT32)pwm_param->end_value);
        REG_WRITE(REG_APB_BK_PWMn_END_ADDR(PWM0), value);

        value = ((UINT32)pwm_param->duty_cycle);
        REG_WRITE(REG_APB_BK_PWMn_DC_ADDR(PWM0), value);
#endif


}

void delay(INT32 num)
{
    volatile INT32 i, j;
    for(i = 0; i < num; i ++)
    {
        for(j = 0; j < 100; j ++)
            ;
    }
}

void hal_sys_delay(uint32_t  ms)
{
    UINT32 ret;
    UINT32 div;
    UINT32 clk = 0;
    UINT32 cell;
    SYS_CTRL_U param;


    param = (SYS_CTRL_U)REG_READ(SCTRL_CONTROL);

    div = param.bits.mclk_div;
    switch(param.bits.mclk_mux)
    {
        case MCLK_MODE_DCO:
        case MCLK_MODE_26M_XTAL:
            clk = 26000000;
            break;

        case MCLK_MODE_DPLL:
            clk = 480000000 / (2 << div);
            break;

        case MCLK_MODE_LPO:
            clk = 32000;
            break;

        default:
            break;
    }

    //cell = 100 * clk / 26000000;
    cell = 100;
    delay(ms * cell);
}

void hal_sys_reboot(void)
{
    int ret = 0;
    wdg_dev_t para;

    para.port = 0;
    para.config.timeout = 1;

    ret = hal_wdg_init(&para);
    if (ret != 0) {
        printf("hal_wdg_init fail \n\r");
    }

    while(1);
}


