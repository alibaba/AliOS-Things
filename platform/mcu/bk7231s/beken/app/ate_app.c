#include "ate_app.h"
#if ATE_APP_FUN

#include "include.h"

#if (!CFG_SUPPORT_ALIOS)
#include "app.h"
#include "wlan_cli_pub.h"
#endif

char ate_mode_state = 0;

void ate_gpio_init(void)
{
    uint32_t param;

    param = GPIO_CFG_PARAM(ATE_GPIO_ID, GMODE_INPUT_PULLUP);
    gpio_ctrl( CMD_GPIO_CFG, &param);
}

uint32_t ate_mode_check(void)
{
    uint32_t ret;
    uint32_t param;

    param = ATE_GPIO_ID;
    ret = gpio_ctrl( CMD_GPIO_INPUT, &param);

    return (0 == ret);
}

void ate_app_init(void)
{
    uint32_t mode = 0;
    ate_gpio_init();

    mode = ate_mode_check();
    if(mode)
    {
        ate_mode_state = (char)1;
    }
    else
    {
        ate_mode_state = (char)0;
    }
}

uint32_t get_ate_mode_state(void)
{
    //ATE_PRT("ateflag:%d\r\n", ate_mode_state);
    if(ate_mode_state != (char)0)
        return 1;
    return 0;
}

#if (!CFG_SUPPORT_ALIOS)
void ate_start(void)
{
    app_pre_start();

    cli_init();

    ATE_PRT("ate_start\r\n");
}
#endif

#endif /*ATE_APP_FUN */
// eof
