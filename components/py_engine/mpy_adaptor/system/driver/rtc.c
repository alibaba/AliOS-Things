#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "board_mgr.h"
#include "aos_hal_rtc.h"

extern const mp_obj_type_t driver_rtc_type;

//#define RTC_JSON_CONFIG_PATH "/data/python/config/rtc.json"
#define RTC_JSON_CONFIG_PATH "/sdcard/python/config/rtc.json"

#define RTC_TIME_FORMAT                                                 \
    "{\"year\":\"%d\",\"month\":\"%d\",\"day\":\"%d\",\"hour\":\"%d\"," \
    "\"minute\":\"%d\",\"second\":\"%d\"}"
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    rtc_dev_t rtc_dev
} mp_rtc_obj_t;

void rtc_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t rtc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_rtc_obj_t* driver_obj = m_new_obj(mp_rtc_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_rtc_type;
    driver_obj->ModuleName = "rtc";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_rtc_obj_t* driver_obj = (mp_rtc_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    ret = aos_hal_rtc_init(&(driver_obj->rtc_dev));
    LOG_D("%s:port: %d, format: %d;\n", __func__, driver_obj->rtc_dev.port, driver_obj->rtc_dev.config.format);
    if (ret != 0)
    {
        LOG_E("aos_hal_rtc_init fail!");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(rtc_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_rtc_obj_t* driver_obj = (mp_rtc_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    ret = aos_hal_rtc_finalize(&(driver_obj->rtc_dev));
    if (ret != 0)
    {
        LOG_E("aos_hal_rtc_finalize fail!");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(rtc_obj_close, 1, obj_close);

STATIC mp_obj_t obj_getTime(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    rtc_time_t rtcTime;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_rtc_obj_t* driver_obj = (mp_rtc_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    ret = aos_hal_rtc_get_time(&(driver_obj->rtc_dev), &rtcTime);
    if (ret != 0)
    {
        LOG_E("aos_hal_rtc_get_time failed\n");
        return mp_const_none;
    }

    LOG_D("year = %d; month = %d;data = %d;\n", (uint32_t)rtcTime.year, (uint32_t)rtcTime.month, (uint32_t)rtcTime.date);
    LOG_D("hr = %d; min = %d;sec = %d;\n", (uint32_t)rtcTime.hr, (uint32_t)rtcTime.min, (uint32_t)rtcTime.sec);
    mp_obj_t dict = mp_obj_new_dict(6);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("year",4),mp_obj_new_int((uint32_t)rtcTime.year));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("month",5),mp_obj_new_int((uint32_t)rtcTime.month));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("date",4),mp_obj_new_int((uint32_t)rtcTime.date));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("hr",2),mp_obj_new_int((uint32_t)rtcTime.hr));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("min",3),mp_obj_new_int((uint32_t)rtcTime.min));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("sec",3),mp_obj_new_int((uint32_t)rtcTime.sec));

    return dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(rtc_obj_getTime, 1, obj_getTime);

STATIC mp_obj_t obj_setTime(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    rtc_time_t rtcTime;
    if (n_args < 7)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_rtc_obj_t* driver_obj = (mp_rtc_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    ret = aos_hal_rtc_get_time(&(driver_obj->rtc_dev), &rtcTime);
    if (ret != 0)
    {
        LOG_E("aos_hal_rtc_get_time failed\n");
        return mp_const_none;
    }

    rtcTime.year = (uint8_t)mp_obj_get_int(args[1]);
    rtcTime.month = (uint8_t)mp_obj_get_int(args[2]);
    rtcTime.date = (uint8_t)mp_obj_get_int(args[3]);
    rtcTime.hr = (uint8_t)mp_obj_get_int(args[4]);
    rtcTime.min = (uint8_t)mp_obj_get_int(args[5]);
    rtcTime.sec = (uint8_t)mp_obj_get_int(args[6]);

    ret = aos_hal_rtc_set_time(&(driver_obj->rtc_dev), &rtcTime);
    if (ret != 0)
    {
        LOG_E("aos_hal_rtc_get_time failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(rtc_obj_setTime, 7, obj_setTime);

STATIC const mp_rom_map_elem_t rtc_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_RTC)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&rtc_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&rtc_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_getTime), MP_ROM_PTR(&rtc_obj_getTime)},
    {MP_ROM_QSTR(MP_QSTR_setTime), MP_ROM_PTR(&rtc_obj_setTime)},
};

STATIC MP_DEFINE_CONST_DICT(rtc_locals_dict, rtc_locals_dict_table);

const mp_obj_type_t driver_rtc_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_RTC,
    .print = rtc_obj_print,
    .make_new = rtc_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&rtc_locals_dict,
};

