#ifndef _MCU_PS_PUB_H_
#define _MCU_PS_PUB_H_

#include "typedef.h"

#define     CFG_MCU_PS_SELECT_120M      1

typedef struct  mcu_ps{
    UINT8 mcu_ps_on;
    int peri_busy_count;
    UINT32 mcu_prevent;
}MCU_PS_INFO;

typedef struct  sctrl_mcu_ps{
    UINT8 hw_sleep ;
    UINT8 first_sleep ;    
    UINT8 mcu_use_dco;
}SCTRL_MCU_PS_INFO;

#define     MCU_PS_CONNECT      CO_BIT(0)
#define     MCU_PS_ADD_KEY      CO_BIT(1)

#define CHIP_U_MCU_WKUP_USE_TIMER  1


extern void vTaskStepTick( const TickType_t  );
extern void mcu_ps_init(void);
extern void mcu_ps_exit(void);
extern UINT32 mcu_power_save(UINT32 );
extern void mcu_prevent_clear(UINT32 );
extern void mcu_prevent_set(UINT32 );
extern void peri_busy_count_dec(void );
extern void peri_busy_count_add(void );
extern UINT32 peri_busy_count_get(void );
extern UINT32 mcu_prevent_get(void );
extern UINT32 fclk_update_tick(UINT32 tick);
extern void mcu_ps_dump(void);
extern void ps_pwm0_reconfig(UINT32 ,UINT8 );
extern void ps_pwm0_resume_tick(void);
extern void ps_pwm0_suspend_tick(UINT32 );
extern void ps_pwm0_disable(void );
extern void ps_pwm0_enable(void);
extern UINT32 ps_timer3_disable(void);
extern void ps_timer3_enable(UINT32 );
extern UINT32 ps_timer3_measure_prepare(void);

#endif

