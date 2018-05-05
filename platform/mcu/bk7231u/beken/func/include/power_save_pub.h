#ifndef _POWER_SAVE_PUB_H_
#define _POWER_SAVE_PUB_H_
#include "rtos_pub.h"
#include "uart_pub.h"
#include "arm_arch.h"
#include "param_config.h"
#include "rw_pub.h"
#include "target_util_pub.h"
#include "wlan_ui_pub.h"
#include "ps_debug_pub.h"

//#define PS_DEBUG

#ifdef PS_DEBUG
#define PS_PRT                 os_printf
#define PS_WPRT                os_printf
#define PS_DBG                 os_printf

#else
#define PS_PRT                 os_null_printf
#define PS_WPRT                os_null_printf
#define PS_DBG                 os_printf

#endif

#define   PS_DTIM_WITH_NORMAL 0

#define   PS_WAKEUP_MOTHOD_PWM1   0
#define   PS_WAKEUP_MOTHOD_RW     1


#define   PS_USE_PWM2              1

#if PS_USE_PWM2
#define   PS_WAIT_BCN         1
#define   PS_DTIM_PERI_WAKE_DELAY  1
#define   PS_WAKE_DATA_DELAY  1
#endif

#define   CFG_DTIM_LOW_VOLT  0
#define   PS_XTAL26M_EN     1
#if (PS_XTAL26M_EN == 1)
#define   PS_WAKEUP_TIME   96//88 // 96       //3 ms 
#define   PS_WAKE_WAIT_TIME   32 
#define   PS_MAX_WAKEUP_TIME   160        //5 ms

#else
//#define   PS_WAKEUP_TIME   94       //3 ms   
#endif

#define   PS_MIN_BCN_INTV_TIME   3276       // 100ms 
#define   PS_MAX_WINDOWNS_TIME   402        //

#define PS_MAX_LOSS_BCN 10
#define PS_NEED_CK_AB_BCN 0
#define PS_CLOSE_PERI_CLK 0

#define FLASH_BASE                           (0x00803000)

#define REG_FLASH_CONF                       (FLASH_BASE + 7 * 4)
#define FLASH_CLK_CONF_POSI                  (0)
#define FLASH_CLK_CONF_MASK                  (0x0F)
#define MODEL_SEL_POSI                       (4)
#define MODEL_SEL_MASK                       (0x1F)
#define FWREN_FLASH_CPU                      (0x01UL << 9)
#define WRSR_DATA_POSI                       (10)
#define WRSR_DATA_MASK                       (0x00FFFF)
#define CRC_EN                               (0x01UL << 26)


#define ICU_BASE                                     (0x00802000)
#define ICU_INTERRUPT_ENABLE                         (ICU_BASE + 16 * 4)


#define ICU_PERI_CLK_PWD                             (ICU_BASE + 2 * 4)
#define PWD_PWM2_CLK                                 (1 << 11)
#define PWD_PWM1_CLK                                 (1 << 10)
#define PWD_UART2_CLK                                (1 <<  1)
#define PWD_UART1_CLK                                (1 <<  0)


#define ICU_ARM_WAKEUP_EN                            (ICU_BASE + 20 * 4)


#define ICU_PWM_CLK_MUX                              (ICU_BASE + 1 * 4)
#define PWM_CLK_LPO                                  (1)
#define PWM_CLK_PCLK                                 (0)
#define PWM2_BIT                                        (2)

typedef enum { 
	STA_GET_INIT = 0,
	STA_GET_TRUE = 1,
	STA_GET_FALSE = 2, 
	STA_GET_TIMEOUT = 3, 
	
	} PS_STA_BEACON_STATE;

typedef UINT8 PS_WAIT_STATUS;
typedef enum { 
	PS_BCN_SUCC = 0,
	PS_BCN_LOSS_LEFT = 1, 
	PS_BCN_LOSS_RIGHT = 2, 
	PS_BCN_LOSS_MIDDLE = 3,	
	PS_BCN_LOSS_FIRST = 4,
	} PS_BCN_STATUS;

typedef enum { 
	PS_BCN_STATUS_INIT = 0,
	PS_BCN_STATUS_UP = 1, 
	PS_BCN_ABNORMAL_AT = 2,	
	PS_BCN_STATUS_DOWN = 3, 
	} PS_BCN_ABNORMAL_STATUS;

typedef enum { 
	PS_NO_PS_MODE = 0,
	PS_STANDBY_PS_MODE = 1, 
	PS_MCU_PS_MODE = 2,	
	PS_DTIM_PS_MODE = 3, 
	PS_DTIM_PS_OPENING = 4,	
	PS_DTIM_PS_CLOSING = 5, 	
	} PS_MODE_STATUS;

typedef enum { 
	PS_ARM_WAKEUP_NONE = 0,
	PS_ARM_WAKEUP_RW = 1,     
	PS_DTIM_ARM_WAKEUP_PERI = 2, 
	PS_ARM_WAKEUP_PWM = 3,
	PS_ARM_WAKEUP_USER = 4,
	
	} PS_ARM_WAKEUP_WAY;

typedef enum { 
	PS_PWM2_LOW_POWER = 0,
	PS_PWM2_CK_AB_BCN = 1, 
	PS_PWM2_CK_TIMEOUT = 2,	
	PS_PWM2_CK_DATA_OVER = 3,	
	} PS_PWM2_STATUS;

typedef enum { 
	PS_BCN_NO_CAL = 0,
	PS_BCN_LOST_10 = 1, 
	} PS_BCN_CAL_STATUS;

typedef enum { 
	PS_LISTEN_MODE_DTIM = 0,
	PS_LISTEN_MODE_INTERVAL = 1, 
	} PS_LISTEN_MODE;

typedef enum { 
	PS_FORBID_NO_ON = 1,
	PS_FORBID_PREVENT = 2, 
	PS_FORBID_VIF_PREVENT = 3, 
	PS_FORBID_IN_DOZE = 4,	
	PS_FORBID_KEEVT_ON = 5,
	PS_FORBID_BMSG_ON = 6,
	PS_FORBID_TXING = 7, 
	PS_FORBID_HW_TIMER = 8, 
	PS_FORBID_TEMP_DETECT = 9, 
	
	} PS_FORBID_STATUS;


typedef struct ps_do_wkup_sem{
    beken_semaphore_t wkup_sema;
    struct co_list_hdr list;
}PS_DO_WKUP_SEM;

typedef struct  ps_sta{
    PS_BCN_ABNORMAL_STATUS     ps_bcn_ab_status ;
    PS_ARM_WAKEUP_WAY ps_arm_wakeup_way ;
    UINT8     ps_real_sleep ;    
    UINT8 sleep_first;
    UINT8 ps_can_sleep;
    UINT8 if_wait_bcn; 
	UINT8 liston_int;
	PS_LISTEN_MODE liston_mode;
	UINT8 pwm2_clkmux;
	PS_PWM2_STATUS pwm2_status;
	UINT8 ps_dtim_period;
	UINT8 ps_dtim_count;
	UINT8 ps_dtim_multi;
	volatile PS_STA_BEACON_STATE waited_beacon;  
    UINT8 ps_bcn_loss_cnt;
    PS_BCN_CAL_STATUS ps_bcn_cal_status;
    UINT16  PsWakeupTime;    
	UINT16 ps_beacon_int;
    UINT16 PsDataWakeupWaitTimeMs ;
    UINT16 PsPeriWakeupWaitTimeMs ; 
	UINT16 sleep_ms;
	UINT32 nxmac_timer_v;
	UINT32 pwm2_less_time;	
    UINT32 sleep_count ;
    UINT32 next_ps_time;
    struct co_list wk_list;
}STA_PS_INFO;


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
	PS_BMSG_IOCTL_RF_ENABLE = 0,
	PS_BMSG_IOCTL_RF_DISANABLE = 1, 
	PS_BMSG_IOCTL_MCU_ENABLE = 2, 
	PS_BMSG_IOCTL_MCU_DISANABLE = 3,	
	PS_BMSG_IOCTL_RF_USER_WKUP = 4,
	} PS_BMSG_IOCTL_CMD;

enum
{
    NEED_DISABLE = 0,
    NEED_ME_DISABLE = 1,
    NEED_REBOOT = 2,
};
#define NEED_DISABLE_BIT            CO_BIT(NEED_DISABLE) 
#define NEED_ME_DISABLE_BIT         CO_BIT(NEED_ME_DISABLE) 
#define NEED_REBOOT_BIT             CO_BIT(NEED_REBOOT) 

typedef struct  sctrl_ps_save_values{
UINT32    peri_clk_cfg;
UINT32    int_enable_cfg;

}SCTRL_PS_SAVE_VALUES;

__INLINE struct ps_do_wkup_sem * list2sem(struct co_list_hdr const *l_list)
{
    return (struct ps_do_wkup_sem *) (((uint8_t*)l_list) - offsetof(struct ps_do_wkup_sem, list));
}

extern volatile UINT8 ps_dtim_normal_enable;
extern STA_PS_INFO bk_ps_info;
extern volatile UINT32 sleep_check_count;
extern volatile PS_MODE_STATUS    bk_ps_mode;
extern UINT16 bk_forbid_count;
extern bool ps_sleep_check(void);
extern int net_if_is_up(void); 
extern void power_save_dtim_ps_init();
extern void  power_save_init(void);
extern bool  power_save_sleep(void);
extern void power_save_pwm1_enable(UINT16 period);
extern void power_save_pwm2_set_period(UINT16,UINT8);
extern void power_save_pwm2_enable();
extern void power_save_pwm1_disable(void );
extern void power_save_pwm2_disable(void );
extern bool rwnxl_if_idle(void);
extern void rwnxl_ask_idle(void);
extern int bmsg_is_empty(void);
extern u8 rwn_mgmt_is_only_sta_role_add(void);
extern void power_save_dtim_wake(UINT32 );
extern void ps_msg_process(UINT8 ioctl);


extern void power_save_wakeup_with_timer(UINT32 sleep_time);
extern void deep_sleep_wakeup_with_timer(UINT32 sleep_time);
extern void power_save_wakeup_with_peri( UINT8 uart2_wk,UINT32 gpio_index_map);
extern int power_save_dtim_enable();
extern int power_save_dtim_disable();
extern void power_save_wakeup_with_gpio(UINT32 gpio_index);
extern void deep_sleep_wakeup_with_gpio(UINT32 gpio_index);
extern void power_save_cal_bcn_liston_int(UINT16 bcn_int);
extern int ps_get_pwm2_less_ms();


extern void power_save_ieee_dtim_wakeup(void);
extern UINT16 power_save_forbid_trace(PS_FORBID_STATUS forbid);
extern void power_save_dump();
extern void power_save_rf_dtim_manual_do_wakeup(void);
extern void power_save_check_delay_time(void);
extern int power_save_dtim_ck_wakeup();
extern UINT8 power_save_me_ps_set_all_state(UINT8 state);
extern int power_save_dtim_disable_handler();
extern int power_save_dtim_enable_handler(void);
extern void sctrl_sta_ps_init(void);
extern void ps_set_key_prevent(void);
extern void ps_clear_key_prevent(void);
extern void ps_set_data_prevent(void);
extern void txl_cntrl_dec_pck_cnt(void);
extern void txl_cntrl_inc_pck_cnt(void);
extern bool power_save_rf_sleep_check( void );
extern void power_save_pwm2_init();


extern SCTRL_PS_SAVE_VALUES ps_saves[];

extern msg_sta_states msg_sta_old_state;
extern UINT8 ps_dis_flag;
extern  UINT16 s_step_test;

#define PS_STA_DTIM_SWITCH ((bk_ps_mode == PS_DTIM_PS_MODE) \
                            && net_if_is_up()                   \
                            &&g_wlan_general_param->role == CONFIG_ROLE_STA)


#define PS_STA_DTIM_CAN_SLEEP (PS_STA_DTIM_SWITCH          \
                && (bk_ps_info.waited_beacon == STA_GET_TRUE    \
                /*|| bk_ps_info.waited_beacon == STA_GET_TIMEOUT*/) \
                && bk_ps_info.ps_can_sleep)

#endif // _POWER_SAVE_PUB_H_
// eof

