#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_SNTP

#include "esp_log.h"
#include "esp_sntp.h"
#include "py/obj.h"
#include "sntp/sntp.h"

#define TAG "MOD_SNTP"

static bool obtain_time(const char *serv_addr)
{
    if (sntp_enabled()) {
        sntp_stop();
    }

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, serv_addr);
    sntp_init();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry_count = 20;
    int retry = 0;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    if (retry == retry_count) {
        ESP_LOGE(TAG, "Failed to get sntp time within %d seconds", retry_count);
        return false;
    }

    time(&now);
    localtime_r(&now, &timeinfo);

    return true;
}

STATIC mp_obj_t mp_sntp_settime(size_t n_args, const mp_obj_t *args)
{
    char *timezone = "CST-8";
    char *serv_addr = "cn.pool.ntp.org";

    if (n_args == 1) {
        timezone = mp_obj_str_get_str(args[0]);
    } else if (n_args == 2) {
        timezone = mp_obj_str_get_str(args[0]);
        serv_addr = mp_obj_str_get_str(args[1]);
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        if (obtain_time(serv_addr) == false) {
            sntp_stop();
            return MP_OBJ_NEW_SMALL_INT(-1);
        }
        // update 'now' variable with current time
        time(&now);
    }

    char strftime_buf[64] = { 0 };
    setenv("TZ", timezone, 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in China is: %s", strftime_buf);

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