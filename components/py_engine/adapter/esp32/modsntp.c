#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "sntp/sntp.h"
#include "ulog/ulog.h"
#include "esp_sntp.h"

#define LOG_TAG "MOD_SNTP"

static char *m_sntp_servaddr[] = {
    "cn.pool.ntp.org",
    "0.cn.pool.ntp.org",
    "1.cn.pool.ntp.org",
};

STATIC mp_obj_t mp_sntp_settime(size_t n_args, const mp_obj_t *args)
{
    char *server = "ntp.ntsc.ac.cn";
    size_t len;
    mp_obj_t *items = NULL;
    time_t now = 0;
    struct tm timeinfo = { 0 };
    char *timezone = NULL;

    if (n_args == 1) {
        timezone = mp_obj_str_get_str(args[0]);
        for (int i = 0; i < 3; ++i) {
            sntp_setservername(i, m_sntp_servaddr[i]);
        }
    } else if (n_args == 2) {
        timezone = mp_obj_str_get_str(args[0]);
        server = (char *)mp_obj_str_get_str(args[1]);
        sntp_setservername(0, server);
    } else {
        for (int i = 0; i < 3; ++i) {
            sntp_setservername(i, m_sntp_servaddr[i]);
        }
    }

    sntp_setoperatingmode(SNTP_OPMODE_POLL);

#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
    if (n_args > 0) {
        setenv("TZ", timezone, 1);
        tzset();
    }
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        LOGI(LOG_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
    sntp_stop();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_sntp_settime_obj, 0, 2,
                                           mp_sntp_settime);

STATIC const mp_rom_map_elem_t sntp_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sntp)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_setTime), MP_ROM_PTR(&mp_sntp_settime_obj)},
};

STATIC MP_DEFINE_CONST_DICT(sntp_module_globals, sntp_module_globals_table);

const mp_obj_module_t sntp_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&sntp_module_globals,
};
