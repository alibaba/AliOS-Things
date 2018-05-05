#include "rtos_pub.h"
#include "uart_pub.h"
#include "arm_arch.h"
#include "param_config.h"
#include "rw_pub.h"
#include "target_util_pub.h"
#include "wlan_ui_pub.h"

#define CFG_MCU_PS_SELECT_120M 1

typedef struct  mcu_ps{
    UINT8 mcu_ps_on;
    UINT8 hw_sleep ;
    UINT8 first_sleep ;    
    UINT8 mcu_use_dco;
    int peri_busy_count;
    UINT32 mcu_prevent;
}MCU_PS_INFO;

#define MCU_PS_CONNECT       CO_BIT(0)
#define MCU_PS_ADD_KEY       CO_BIT(1)


extern MCU_PS_INFO mcu_ps_info;


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

