#include "include.h"
#include "arm_arch.h"

#include "icu_pub.h"
#include "icu.h"

#include "drv_model_pub.h"

static SDD_OPERATIONS icu_op =
{
    icu_ctrl
};

/*******************************************************************/
void icu_init(void)
{
    UINT32 param;

    sddev_register_dev(ICU_DEV_NAME, &icu_op);

    /*pclk select 26m */
    #if (CFG_SOC_NAME == SOC_BK7231)
    param = PCLK_POSI;
    #else
    param = PCLK_POSI_UART1 | PCLK_POSI_UART2
            | PCLK_POSI_PWMS | PCLK_POSI_SDIO
            | PCLK_POSI_I2C1 | PCLK_POSI_I2C2;
    #endif // (CFG_SOC_NAME == SOC_BK7231)
    icu_ctrl(CMD_CONF_PCLK_26M, &param);

    #if (CFG_SOC_NAME == SOC_BK7231)
    param = FIQ_MAILBOX0_BIT | FIQ_MAILBOX1_BIT;
    icu_ctrl(CMD_ICU_INT_DISABLE, &param);
    #endif // (CFG_SOC_NAME == SOC_BK7231)
}

void icu_exit(void)
{
    sddev_unregister_dev(ICU_DEV_NAME);
}

UINT32 icu_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret;
    UINT32 reg;

    ret = ICU_SUCCESS;

    switch(cmd)
    {
    case CMD_CONF_PCLK_26M:
        reg = REG_READ(ICU_PERI_CLK_MUX);
        #if (CFG_SOC_NAME == SOC_BK7231)
        reg |= (1 << (*(UINT32 *)param));
        #else
        reg |= (*(UINT32 *)param);
        #endif // (CFG_SOC_NAME == SOC_BK7231)
        REG_WRITE(ICU_PERI_CLK_MUX, reg);
        break;


    case CMD_CONF_PWM_LPOCLK:
        reg = REG_READ(ICU_PWM_CLK_MUX);
        reg |= (1 << (*(UINT32 *)param));
        REG_WRITE(ICU_PWM_CLK_MUX, reg);
        break;

    case CMD_CONF_PCLK_DCO:
        reg = REG_READ(ICU_PERI_CLK_MUX);
        #if (CFG_SOC_NAME == SOC_BK7231)
        reg &= ~(1 << (*(UINT32 *)param));
        #else
        reg &= ~(*(UINT32 *)param);
        #endif // (CFG_SOC_NAME == SOC_BK7231)
        REG_WRITE(ICU_PERI_CLK_MUX, reg);
        break;

    case CMD_CONF_PWM_PCLK:
        reg = REG_READ(ICU_PWM_CLK_MUX);
        reg &= ~(1 << (*(UINT32 *)param));
        REG_WRITE(ICU_PWM_CLK_MUX, reg);
        break;

    case CMD_CLK_PWR_UP:
        reg = REG_READ(ICU_PERI_CLK_PWD);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_PERI_CLK_PWD, reg);
        break;

    case CMD_CLK_PWR_DOWN:
        reg = REG_READ(ICU_PERI_CLK_PWD);
        reg |= (*(UINT32 *)param);
        REG_WRITE(ICU_PERI_CLK_PWD, reg);
        break;

    case CMD_TL410_CLK_PWR_UP:
        reg = REG_READ(ICU_TL410_CLK_PWD);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_TL410_CLK_PWD, reg);
        break;

    case CMD_TL410_CLK_PWR_DOWN:
        reg = REG_READ(ICU_TL410_CLK_PWD);
        reg |= (*(UINT32 *)param);
        REG_WRITE(ICU_TL410_CLK_PWD, reg);
        break;

    case CMD_ICU_CLKGATING_DISABLE:
        reg = REG_READ(ICU_PERI_CLK_GATING);
        reg |= (*(UINT32 *)param);
        REG_WRITE(ICU_PERI_CLK_GATING, reg);
        break;

    case CMD_ICU_CLKGATING_ENABLE:
        reg = REG_READ(ICU_PERI_CLK_GATING);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_PERI_CLK_GATING, reg);
        break;

    case CMD_ICU_INT_DISABLE:
        reg = REG_READ(ICU_INTERRUPT_ENABLE);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
        break;

    case CMD_ICU_INT_ENABLE:
        reg = REG_READ(ICU_INTERRUPT_ENABLE);
        reg |= (*(UINT32 *)param);
        REG_WRITE(ICU_INTERRUPT_ENABLE, reg);
        break;

    case CMD_ICU_GLOBAL_INT_DISABLE:
        reg = REG_READ(ICU_GLOBAL_INT_EN);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_GLOBAL_INT_EN, reg);
        break;

    case CMD_ICU_GLOBAL_INT_ENABLE:
        reg = REG_READ(ICU_GLOBAL_INT_EN);
        reg |= (*(UINT32 *)param);
        REG_WRITE(ICU_GLOBAL_INT_EN, reg);
        break;

    case CMD_GET_INTR_STATUS:
        ret = REG_READ(ICU_INT_STATUS);
        break;

    case CMD_CLR_INTR_STATUS:
        ASSERT(param);
        reg = REG_READ(ICU_INT_STATUS);
        REG_WRITE(ICU_INT_STATUS, (reg | *(UINT32 *)param));
        break;

    case CMD_GET_INTR_RAW_STATUS:
        ret = REG_READ(ICU_INT_RAW_STATUS);
        break;

    case CMD_CLR_INTR_RAW_STATUS:
        ASSERT(param);
        reg = REG_READ(ICU_INT_RAW_STATUS);
        REG_WRITE(ICU_INT_RAW_STATUS, (reg | *(UINT32 *)param));
        break;

    case CMD_SET_JTAG_MODE:
        ASSERT(param);
        if(JTAG_ARM_MODE == (*(UINT32 *)param))
            REG_WRITE(ICU_JTAG_SELECT, JTAG_SEL_WR_ARM);
        else if(JTAG_TL410_MODE == (*(UINT32 *)param))
            REG_WRITE(ICU_JTAG_SELECT, JTAG_SEL_WR_TL4);
        break;

    case CMD_GET_JTAG_MODE:
        ret = REG_READ(ICU_JTAG_SELECT);
        break;

    case CMD_ARM_WAKEUP:
		reg = (*(UINT32*)param);
		REG_WRITE(ICU_ARM_WAKEUP_EN, reg);            
        break;

    default:
        break;
    }

    return ret;
}

// EOF
