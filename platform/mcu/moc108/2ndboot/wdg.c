/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "bootloader.h"
#include "sys.h"
#include "flash.h"
#include "include.h"
#include "icu_pub.h"
#include "icu.h"
#include "wdt_pub.h"
#include "wdt.h"

static unsigned int wdt_icu_ctrl(unsigned int cmd, void *param)
{
    unsigned int ret;
    unsigned int reg;

    ret = ICU_SUCCESS;
    switch(cmd)
    {
    case CMD_CLK_PWR_UP:
        reg = REG_READ(ICU_PERI_CLK_PWD);
        reg &= ~(*(unsigned int *)param);
        REG_WRITE(ICU_PERI_CLK_PWD, reg);
        break;
    case CMD_CLK_PWR_DOWN:
        reg = REG_READ(ICU_PERI_CLK_PWD);
        reg |= (*(unsigned int *)param);
        REG_WRITE(ICU_PERI_CLK_PWD, reg);
        break;
    default:
        break;
    }
    return ret;
}

static unsigned int wdt_cmd_ctrl(unsigned int cmd, void *param)
{
    unsigned int ret;
    unsigned int reg;
    ret = WDT_SUCCESS;
    switch(cmd) {
        case WCMD_CLEAR_COUNTER:
            reg = REG_READ(WDT_CTRL_REG);
            reg &= ~(WDT_KEY_MASK << WDT_KEY_POSI);
            reg |= WDT_1ST_KEY << WDT_KEY_POSI;
            REG_WRITE(WDT_CTRL_REG, reg);
            reg = REG_READ(WDT_CTRL_REG);
            reg &= ~(WDT_KEY_MASK << WDT_KEY_POSI);
            reg |= WDT_2ND_KEY << WDT_KEY_POSI;
            REG_WRITE(WDT_CTRL_REG, reg);
            break;
        case WCMD_SET_PERIOD:
            reg = WDT_1ST_KEY << WDT_KEY_POSI;
            reg |= (*(unsigned int *)param & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
            REG_WRITE(WDT_CTRL_REG, reg);
            reg = WDT_2ND_KEY << WDT_KEY_POSI;
            reg |= (*(unsigned int *)param & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
            REG_WRITE(WDT_CTRL_REG, reg);
            break;
        default:
            break;
    }
    return ret;
}

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    unsigned int para;
    unsigned int param;

    if (wdg == NULL) {
        return -1;
    }

    param = wdg->config.timeout;

    para = PWD_ARM_WATCHDOG_CLK_BIT;
    wdt_icu_ctrl(CMD_CLK_PWR_UP, &para);
    wdt_cmd_ctrl(WCMD_SET_PERIOD, &param);
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    unsigned int para;
    para = PWD_ARM_WATCHDOG_CLK_BIT;
    wdt_icu_ctrl(CMD_CLK_PWR_DOWN, &para);
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    wdt_cmd_ctrl(WCMD_CLEAR_COUNTER, 0);
}

