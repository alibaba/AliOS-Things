/**
 * ant_pm.h
 *
 */
#ifndef __ANT_PM_H__
#define __ANT_PM_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ant_bool is_usb_inserted;   // true: charger usb inserted; false: charger usb not inserted;
    ant_bool is_charge_full;    // true: charge complete; false: charge not complete
    ant_u32  charge_current;    // mA
} ant_charge_info_t;

typedef struct {
    ant_u32 end_voltage;         // charge complete voltage, mV
	ant_u32 recharge_voltage;    // recharge start voltage, mV    
	ant_u32 shutdown_voltage;    // shutdown voltage, mV
	ant_u32 warning_voltage;     // low power warning voltage, mV
    ant_u32 max_charge_current;  // max charge current, mA
} ant_charger_cfg_t;

void ant_pm_init(void);

ant_s32 ant_pm_get_charger_cfg(ant_charger_cfg_t *charger_cfg);

ant_s32 ant_pm_get_battery_voltage(void);

ant_s32 ant_pm_get_battery_capacity(void);

ant_s32 ant_pm_get_charge_current(void);

ant_s32 ant_pm_get_battery_temperature(void);

ant_bool ant_pm_get_charger_inserted(void);

ant_bool ant_pm_get_charge_completed(void);

ant_s32  ant_pm_enable_charger(ant_bool enable);

ant_s32  ant_pm_sleep_mode(ant_s32 delay_seconds);

ant_bool ant_pm_get_charge_full(void);

ant_s32  ant_pm_get_charge_overvol(void);

ant_bool ant_pm_get_charge_enabled(void);

ant_s32  ant_pm_get_charge_temperature_high(void);

ant_s32  ant_pm_get_charge_temperature_low(void);


    
#ifdef __cplusplus
} // extern "C"
#endif

#endif

