#ifndef __APP_LED_H__
#define __APP_LED_H__

typedef enum
{
    STA_NONE         = 0,
    POWER_ON,
    LED_CONNECT,
    LED_DISCONNECT,
    MONITOR_MODE,
    SOFTAP_MODE,
    TIMER_POLL,
} DEV_STATE;


UINT32 app_led_init(void);
void app_led_send_msg(DEV_STATE new_msg);

#endif
