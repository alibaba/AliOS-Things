#ifndef MICROPY_INCLUDED_HAAS_MODMACHINE_H
#define MICROPY_INCLUDED_HAAS_MODMACHINE_H

#include "py/obj.h"

typedef enum {
    // MACHINE_WAKE_IDLE=0x01,
    MACHINE_WAKE_SLEEP=0x02,
    MACHINE_WAKE_DEEPSLEEP=0x04,
    SLEEP_WAKEUP_EXT0,
    SLEEP_WAKEUP_EXT1,
    SLEEP_WAKEUP_TIMER,
    SLEEP_WAKEUP_TOUCHPAD,
    SLEEP_WAKEUP_ULP,

    PWR_MODE_ACTIVE,
    PWR_MODE_LPDS,
    PWR_MODE_HIBERNATE,
    SLP_PWRON_RESET,
    SLP_HARD_RESET,
    SLP_WDT_RESET,
    SLP_HIB_RESET,
    SLP_SOFT_RESET,
    SLP_WAKED_BY_WLAN,
    SLP_WAKED_BY_GPIO,
    SLP_WAKED_BY_RTC,

    MP_HARD_RESET,
    MP_PWRON_RESET,
    MP_WDT_RESET,
    MP_DEEPSLEEP_RESET,
    MP_SOFT_RESET
} wake_type_t;


extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t mp_machine_soft_i2c_type;
extern const mp_obj_type_t mp_machine_soft_spi_type;
extern const mp_obj_type_t mp_machine_soft_timer_type;
extern const mp_obj_type_t machine_sdcard_type;
extern const mp_obj_type_t machine_hw_i2c_type;
extern const mp_obj_type_t machine_hw_spi_type;

extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_can_type;
#if MICROPY_PY_MACHINE_HW_PWM
extern const mp_obj_type_t machine_dac_type;
#endif
extern const mp_obj_type_t machine_wdt_type;

void machine_init(void);
void machine_deinit(void);
void machine_pins_init(void);
void machine_pins_deinit(void);
void machine_timer_deinit_all(void);

#endif // MICROPY_INCLUDED_HAAS_MODMACHINE_H
