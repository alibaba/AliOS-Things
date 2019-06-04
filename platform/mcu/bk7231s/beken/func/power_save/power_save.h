#ifndef _POWER_SAVE_H_
#define _POWER_SAVE_H_

#include "param_config.h"
#include "co_list.h"
#include "power_save_pub.h"
#include "ps_debug_pub.h"

#define     PS_DTIM_PERI_WAKE_DELAY                 0
#define     PS_WAKE_DATA_DELAY                      1

#define     PS_BCN_MAX_LOSS_LIMIT                      (5)

typedef enum
{
    STA_GET_INIT = 0,
    STA_GET_TRUE = 1,
    STA_GET_FALSE = 2,
    STA_GET_TIMEOUT = 3,
} PS_STA_BEACON_STATE;

typedef UINT8 PS_WAIT_STATUS;
typedef enum
{
    PS_BCN_SUCC = 0,
    PS_BCN_LOSS_LEFT = 1,
    PS_BCN_LOSS_RIGHT = 2,
    PS_BCN_LOSS_MIDDLE = 3,
    PS_BCN_LOSS_FIRST = 4,
} PS_BCN_STATUS;

typedef enum
{
    PS_BCN_STATUS_INIT = 0,
    PS_BCN_STATUS_UP = 1,
    PS_BCN_ABNORMAL_AT = 2,
    PS_BCN_STATUS_DOWN = 3,
} PS_BCN_ABNORMAL_STATUS;


typedef enum
{
    PS_ARM_WAKEUP_NONE = 0,
    PS_ARM_WAKEUP_RW = 1,
    PS_DTIM_ARM_WAKEUP_PERI = 2,
    PS_ARM_WAKEUP_PWM = 3,
    PS_ARM_WAKEUP_USER = 4,
    PS_ARM_WAKEUP_UPING = 5,
} PS_ARM_WAKEUP_WAY;

typedef enum
{
    TM_LOW_POWER = 0,
    PS_TM_CK_AB_BCN = 1,
    PS_TM_CK_TIMEOUT = 2,
    PS_TM_CK_DATA_OVER = 3,
} PS_TM_STATUS;

typedef enum
{
    PS_BCN_NO_CAL = 0,
    PS_BCN_LOST_10 = 1,
} PS_BCN_CAL_STATUS;

typedef enum
{
    PS_LISTEN_MODE_DTIM = 0,
    PS_LISTEN_MODE_INTERVAL = 1,
} PS_LISTEN_MODE;

typedef struct ps_do_wkup_sem
{
    beken_semaphore_t wkup_sema;
    struct co_list_hdr list;
} PS_DO_WKUP_SEM;

typedef struct  ps_sta
{
    PS_BCN_ABNORMAL_STATUS     ps_bcn_ab_status ;
    PS_ARM_WAKEUP_WAY ps_arm_wakeup_way ;
    UINT8     ps_real_sleep ;
    UINT8 sleep_first;
    UINT8 ps_can_sleep;
    UINT8 if_wait_bcn;
    UINT8 liston_int;
    PS_LISTEN_MODE liston_mode;
    UINT8 pwm0_clkmux;
    PS_TM_STATUS tm_status;
    UINT8 ps_dtim_period;
    UINT8 ps_dtim_count;
    UINT8 ps_dtim_multi;
    volatile PS_STA_BEACON_STATE waited_beacon;
    UINT8 ps_bcn_loss_cnt;
    PS_BCN_CAL_STATUS ps_bcn_cal_status;
    UINT16 ps_beacon_int;
    UINT16 PsDataWakeupWaitTimeMs ;
    UINT16 PsPeriWakeupWaitTimeMs ;
    UINT16 sleep_ms;
    UINT32 nxmac_timer_v;
    UINT32 pwm0_less_time;
    UINT32 sleep_count ;
    UINT32 next_ps_time;
    struct co_list wk_list;
} STA_PS_INFO;


__INLINE struct ps_do_wkup_sem *list2sem(struct co_list_hdr const *l_list)
{
    return (struct ps_do_wkup_sem *) (((uint8_t *)l_list) - offsetof(struct ps_do_wkup_sem, list));
}

void power_save_mac_idle_callback(void);
UINT32 power_save_wkup_event_get(void);
void power_save_dtim_ps_init();
void power_save_ieee_dtim_wakeup(void);
UINT8 power_save_me_ps_set_all_state(UINT8 state);
PS_STA_BEACON_STATE power_save_beacon_state_get(void);
PS_ARM_WAKEUP_WAY power_save_wkup_way_get(void);
void power_save_set_uart_linger_time(UINT32 uart_wakeup_time);
extern void bmsg_ps_sender(uint8_t ioctl);
extern void ps_fake_data_rx_check(void);
extern bool ps_sleep_check(void);
extern u8 rwn_mgmt_is_only_sta_role_add(void);

//#define  PS_NEXT_DATA_CK_TM    2500 //5s

#define PS_STA_DTIM_SWITCH (power_save_if_ps_rf_dtim_enabled() \
                            && net_if_is_up()                   \
                            &&g_wlan_general_param->role == CONFIG_ROLE_STA)


#define PS_STA_DTIM_CAN_SLEEP (PS_STA_DTIM_SWITCH          \
                && (power_save_beacon_state_get() == STA_GET_TRUE    \
                || power_save_wkup_way_get() == PS_ARM_WAKEUP_USER) \
                && power_save_if_ps_can_sleep())

#endif // _POWER_SAVE_H_
// eof

