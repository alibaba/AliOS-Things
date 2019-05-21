#ifndef __APP_DRONE_H__
#define __APP_DRONE_H__

enum
{
    DMSG_TIMER_POLL          = 0, 
    DMSG_WIFI_DISCONECTED,    
    DMSG_WIFI_CONECTED,      
    DMSG_APP_CONECTED,
    DMSG_APP_DISCONECTED,      
    DMSG_EXIT,
};

void app_drone_send_msg(u32 new_msg);
UINT32 app_drone_init(void);

#endif
