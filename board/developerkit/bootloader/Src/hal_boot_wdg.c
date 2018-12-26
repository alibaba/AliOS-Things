#include "stm32l4xx_hal.h"

#define HAL_IWDG_DEFAULT_TIMEOUT            48u

static unsigned char wdt_init_flag = 0;

HAL_StatusTypeDef HAL_IWDG_Init(IWDG_HandleTypeDef *hiwdg)
{
    uint32_t tickstart;
    if(hiwdg == NULL) {
        return HAL_ERROR;
    }
    assert_param(IS_IWDG_ALL_INSTANCE(hiwdg->Instance));
    assert_param(IS_IWDG_PRESCALER(hiwdg->Init.Prescaler));
    assert_param(IS_IWDG_RELOAD(hiwdg->Init.Reload));
    assert_param(IS_IWDG_WINDOW(hiwdg->Init.Window));
    __HAL_IWDG_START(hiwdg);
    IWDG_ENABLE_WRITE_ACCESS(hiwdg);
    /* Write to IWDG registers the Prescaler & Reload values to work with */
    hiwdg->Instance->PR = hiwdg->Init.Prescaler;
    hiwdg->Instance->RLR = hiwdg->Init.Reload;
    tickstart = HAL_GetTick();
    while(hiwdg->Instance->SR != RESET) {
        if((HAL_GetTick() - tickstart ) > HAL_IWDG_DEFAULT_TIMEOUT) {
            return HAL_TIMEOUT;
        }
    }
    if(hiwdg->Instance->WINR != hiwdg->Init.Window) {
        hiwdg->Instance->WINR = hiwdg->Init.Window;
    }
    else {
        __HAL_IWDG_RELOAD_COUNTER(hiwdg);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg)
{
    __HAL_IWDG_RELOAD_COUNTER(hiwdg);
    return HAL_OK;
}


static IWDG_HandleTypeDef   IwdgHandle;
void hal_boot_wdg_init(unsigned int feed_tm)
{
    uint32_t uwLsiFreq = 0;
    if(feed_tm == 0) {
        feed_tm = 3;
    }
    __HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST);
    __HAL_RCC_CLEAR_RESET_FLAGS();
    uwLsiFreq = 31616;
    IwdgHandle.Instance = IWDG;
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_64;
    IwdgHandle.Init.Reload = (feed_tm * uwLsiFreq / 64);
    IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;
    if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK) {
        hal_uart_send_string("iwdg init failed\r\n");
        while(1) {
            ;
        }
    }
    wdt_init_flag = 1;
}

void hal_boot_feed_dg()
{
    if(wdt_init_flag == 0)
        return;
    if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK) {
        hal_uart_send_string("feed dog failed\r\n");
    }
}

