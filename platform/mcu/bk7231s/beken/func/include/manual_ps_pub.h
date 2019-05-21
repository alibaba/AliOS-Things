#ifndef _MANUAL_PS_PUB_H_
#define _MANUAL_PS_PUB_H_

typedef enum { 
	PS_DEEP_WAKEUP_GPIO = 0,
	PS_DEEP_WAKEUP_RTC = 1, 
} PS_DEEP_WAKEUP_WAY;

typedef struct  ps_deep_ctrl{
    PS_DEEP_WAKEUP_WAY deep_wkway;
    UINT32 gpio_lv;
    UINT32 param;
}PS_DEEP_CTRL_PARAM;

typedef enum { 
	MANUAL_MODE_NORMAL = 0,
	MANUAL_MODE_IDLE = 1, 
} MANUAL_MODE;

#define     PS_SUPPORT_MANUAL_SLEEP     0
typedef void (*ps_wakeup_cb)(void);
extern void deep_sleep_wakeup_with_gpio(UINT32 gpio_index_map,UINT32 gpio_edge_map);
extern void bk_wlan_ps_wakeup_with_timer(MANUAL_MODE mode,UINT32 sleep_time);
extern void bk_wlan_ps_wakeup_with_peri( UINT8 uart2_wk, UINT32 gpio_index_map,UINT32 gpio_edge_map);
extern void bk_wlan_ps_wakeup_with_gpio(MANUAL_MODE mode,UINT32 gpio_index_map,UINT32 gpio_edge_map);

#endif

