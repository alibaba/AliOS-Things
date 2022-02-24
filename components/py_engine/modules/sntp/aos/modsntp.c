#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#if MICROPY_PY_SNTP

#include "lwip/apps/sntp.h"
#include "py/obj.h"
#include "ulog/ulog.h"

#define TAG "MOD_SNTP"

static const char *m_sntp_servaddr[] = {
    "cn.pool.ntp.org",
    "ntp.aliyun.com",
    "ntp.tuna.tsinghua.edu.cn",
};

STATIC mp_obj_t mp_sntp_settime(size_t n_args, const mp_obj_t *args)
{
    char *serv_addr = "ntp.aliyun.com";
    char *timezone = "CST-8";

    if (n_args == 1) {
        timezone = mp_obj_str_get_str(args[0]);
    } else if (n_args == 2) {
        timezone = mp_obj_str_get_str(args[0]);
        serv_addr = mp_obj_str_get_str(args[1]);
    }

    struct timezone tz = {0};
    tz.tz_minuteswest = -480;
    tz.tz_dsttime = 0;
    sntp_set_timezone(&tz);

    return MP_OBJ_NEW_SMALL_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_sntp_settime_obj, 0, 2, mp_sntp_settime);

STATIC const mp_rom_map_elem_t sntp_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sntp) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setTime), MP_ROM_PTR(&mp_sntp_settime_obj) },
};

STATIC MP_DEFINE_CONST_DICT(sntp_module_globals, sntp_module_globals_table);

const mp_obj_module_t sntp_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sntp_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sntp, sntp_module, MICROPY_PY_SNTP);

#endif  // MICROPY_PY_SNTP