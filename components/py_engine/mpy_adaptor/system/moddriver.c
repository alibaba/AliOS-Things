#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t driver_adc_type;
extern const mp_obj_type_t driver_pwm_type;
extern const mp_obj_type_t driver_gpio_type;
extern const mp_obj_type_t driver_i2c_type;
extern const mp_obj_type_t driver_spi_type;
extern const mp_obj_type_t driver_uart_type;
extern const mp_obj_type_t driver_rtc_type;
extern const mp_obj_type_t driver_timer_type;
extern const mp_obj_type_t driver_can_type;
//extern const mp_obj_type_t driver_dac_type;
extern const mp_obj_type_t driver_ir_type;
extern const mp_obj_type_t driver_wdg_type;
//advanced
extern const mp_obj_type_t driver_keypad_type;
extern const mp_obj_type_t driver_location_type;
extern const mp_obj_type_t driver_und_type;
//utils
extern const mp_obj_type_t driver_crypto_type;
// this is the actual C-structure for our new object

STATIC const mp_rom_map_elem_t driver_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_driver)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&driver_adc_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_PWM), MP_ROM_PTR(&driver_pwm_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_GPIO), MP_ROM_PTR(&driver_gpio_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&driver_i2c_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UART), MP_ROM_PTR(&driver_uart_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&driver_spi_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&driver_rtc_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_TIMER), MP_ROM_PTR(&driver_timer_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CAN), MP_ROM_PTR(&driver_can_type)},
    //{MP_OBJ_NEW_QSTR(MP_QSTR_DAC), MP_ROM_PTR(&driver_dac_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_IR), MP_ROM_PTR(&driver_ir_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_WDG), MP_ROM_PTR(&driver_wdg_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_KeyPad), MP_ROM_PTR(&driver_keypad_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Location), MP_ROM_PTR(&driver_location_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UND), MP_ROM_PTR(&driver_und_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Crypto), MP_ROM_PTR(&driver_crypto_type)},
};

STATIC MP_DEFINE_CONST_DICT(driver_locals_dict, driver_locals_dict_table);

const mp_obj_module_t driver_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&driver_locals_dict,
};

