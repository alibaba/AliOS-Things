/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_WDG_H
#define HAL_WDG_H
#include "stdint.h"
#include "aos/errno.h"
#include "fm33a0xx_include_all.h"
#include "board.h"

// ============== Fm33A0X Gpio Define Begin =============== //

const uint32_t wdg_timeout[] = {125, 500, 2000, 8000};

// ============== Fm33A0X Gpio Define End =============== //



typedef struct {
        uint32_t timeout;  /* Watchdag timeout based on ms*/
} wdg_config_t;

typedef struct {
        uint8_t      port;   /* wdg port */
        wdg_config_t config; /* wdg config */
        void        *priv;   /* priv data */
} wdg_dev_t;

/**
 * This function will initialize the on board CPU hardware watch dog
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    uint8_t i;
    
    if (NULL == wdg)
    {
        return EIO;
    }
    
    for (i = 0; i < sizeof(wdg_timeout)/sizeof(uint32_t); i++)
    {
        if (wdg->config.timeout < wdg_timeout[i])
        {
            break;
        }
    }
    
    RCC_PERCLK_SetableEx(IWDTCLK, ENABLE);      //IWDT clk enable
    IWDT_Clr();                                               //feed IWDT
    
    if (0 == i)
    {
        IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_125ms);//config timeout
    }
    else if (1 == i)
    {
        IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_500ms);//config timeout
    }
    else if (2 == i)
    {
        IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_2s);//config timeout
    }
    else
    {
        IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_8s);//config timeout
    }
    
    IWDT_IWDTCFG_IWDTSLP4096S_Setable(DISABLE);         //disable sleep 4096s
    
    return 0;
}

/**
 * Reload watchdog counter.
 *
 * @param[in]  wdg  the watch dog device
 */
void hal_wdg_reload(wdg_dev_t *wdg)
{
    if (NULL == wdg)
    {
        return;
    }
    
    IWDT_Clr();
}

/**
 * This function performs any platform-specific cleanup needed for hardware watch dog.
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    if (NULL == wdg)
    {
        return EIO;
    }
    
    IWDT_Deinit();
    return 0;
}

#endif /* HAL_WDG_H */

