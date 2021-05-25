#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stackctrl.h"

#include "sntp/sntp.h"

#include "HaasLog.h"

#define SNTP_RCV_TIMEOUT                (5000)
#define SNTP_PERSERV_RETRY_TIMES        (2)

static char *m_sntp_servaddr[] = {
    "cn.pool.ntp.org",
    "0.cn.pool.ntp.org",
    "1.cn.pool.ntp.org",
};

static int retry_backoff_time[]= {
    1, 2, 1, 2, 1, 2, 1, 2
};

static void sntp_config_servaddr(void)
{
    int i = 0;
    LOG_D("sntp config servadd start.");
    for (i = 0; i < SNTP_MAX_SERVERS; i++) {
        if (0 != sntp_set_server(i, m_sntp_servaddr[i])) {
            LOG_E("set sntp server:%s failed\n", m_sntp_servaddr[i]);
        }
        else {
            LOG_I("set sntp server:%s successfully\n", m_sntp_servaddr[i]);
        }
    }
    LOG_I("sntp config servadd end.");
}

static bool sntp_gettime(struct timeval *ntp_time)
{
    int round = 0;
    sntp_arg m_sntp_arg = {
            NULL,
            SNTP_RCV_TIMEOUT,
            SNTP_PERSERV_RETRY_TIMES};

    while(1) {
        LOG_D("sntp getting time.");
        sntp_config_servaddr();
        if (0 == sntp_get_time(&m_sntp_arg, ntp_time)) {
            LOG_I("[sntp] OK: sec %ld usec %ld\n", ntp_time->tv_sec, ntp_time->tv_usec);
            return true;
        } else {
            int retry_time = retry_backoff_time[round];
            LOG_I("[sntp] wait for sntp done...e\n");
            if(round < sizeof(retry_backoff_time)/sizeof(int) - 1) {
                round ++;
            } else {
                LOG_E("[sntp] Failed to get SNTP from server\n");
                return false;
            }
            aos_msleep(retry_time * 1000);
        }
    }
}

STATIC mp_obj_t mp_sntp_settime(size_t n_args, const mp_obj_t *args) {
    mp_int_t timezone = 8;
    char *server = "ntp.ntsc.ac.cn";

    if (n_args == 1) {
        timezone = mp_obj_get_int(args[0]);
    } else if (n_args == 2) {
        timezone = mp_obj_get_int(args[0]);
        server = (char*)mp_obj_str_get_str(args[1]);
    }

    printf("%s, %d, timezone=%d, server=%s\r\n", __func__, __LINE__, timezone, server);

    struct timeval ntp_time = {0};
    mp_int_t ret = sntp_gettime(&ntp_time);

    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_sntp_settime_obj, 0, 2, mp_sntp_settime);

STATIC const mp_rom_map_elem_t sntp_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sntp)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_settime), MP_ROM_PTR(&mp_sntp_settime_obj)},    
};

STATIC MP_DEFINE_CONST_DICT(sntp_module_globals, sntp_module_globals_table);

const mp_obj_module_t sntp_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&sntp_module_globals,
};


