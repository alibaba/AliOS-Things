#include "core_global.h"

typedef struct {
    void *mutex;
    uint8_t is_inited;
    uint32_t used_count;
    int32_t alink_id;
    char mqtt_backup_ip[16];
} g_core_global_t;

g_core_global_t g_core_global = {NULL, 0, 0, 0, {0}};

int32_t core_global_init(aiot_sysdep_portfile_t *sysdep)
{
    if (g_core_global.is_inited == 1) {
        g_core_global.used_count++;
        return STATE_SUCCESS;
    }
    g_core_global.is_inited = 1;


    g_core_global.mutex = sysdep->core_sysdep_mutex_init();
    g_core_global.used_count++;

    return STATE_SUCCESS;
}

int32_t core_global_alink_id_next(aiot_sysdep_portfile_t *sysdep, int32_t *alink_id)
{
    int32_t id = 0;
    sysdep->core_sysdep_mutex_lock(g_core_global.mutex);
    g_core_global.alink_id++;
    if (g_core_global.alink_id < 0) {
        g_core_global.alink_id = 0;
    }
    id = g_core_global.alink_id;
    sysdep->core_sysdep_mutex_unlock(g_core_global.mutex);

    *alink_id = id;
    return STATE_SUCCESS;
}

int32_t core_global_set_mqtt_backup_ip(aiot_sysdep_portfile_t *sysdep, char ip[16])
{
    sysdep->core_sysdep_mutex_lock(g_core_global.mutex);
    memset(g_core_global.mqtt_backup_ip, 0, 16);
    memcpy(g_core_global.mqtt_backup_ip, ip, strlen(ip));
    sysdep->core_sysdep_mutex_unlock(g_core_global.mutex);

    return STATE_SUCCESS;
}

int32_t core_global_get_mqtt_backup_ip(aiot_sysdep_portfile_t *sysdep, char ip[16])
{
    sysdep->core_sysdep_mutex_lock(g_core_global.mutex);
    memcpy(ip, g_core_global.mqtt_backup_ip, strlen(g_core_global.mqtt_backup_ip));
    sysdep->core_sysdep_mutex_unlock(g_core_global.mutex);

    return STATE_SUCCESS;
}

int32_t core_global_deinit(aiot_sysdep_portfile_t *sysdep)
{
    if (g_core_global.used_count > 0) {
        g_core_global.used_count--;
    }

    if (g_core_global.used_count != 0) {
        return STATE_SUCCESS;
    }
    sysdep->core_sysdep_mutex_deinit(&g_core_global.mutex);

    g_core_global.mutex = NULL;
    g_core_global.is_inited = 0;
    g_core_global.used_count = 0;

    return STATE_SUCCESS;
}

