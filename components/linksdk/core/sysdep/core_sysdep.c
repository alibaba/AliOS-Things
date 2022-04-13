#include "core_sysdep.h"

static aiot_sysdep_portfile_t *g_sysdep_portfile = NULL;

void aiot_sysdep_set_portfile(aiot_sysdep_portfile_t *portfile)
{
    g_sysdep_portfile = portfile;
}

aiot_sysdep_portfile_t * aiot_sysdep_get_portfile(void)
{
    if (g_sysdep_portfile == NULL ||
        g_sysdep_portfile->core_sysdep_malloc == NULL ||
        g_sysdep_portfile->core_sysdep_free == NULL ||
        g_sysdep_portfile->core_sysdep_time == NULL ||
        g_sysdep_portfile->core_sysdep_sleep == NULL ||
        g_sysdep_portfile->core_sysdep_network_init == NULL ||
        g_sysdep_portfile->core_sysdep_network_setopt == NULL ||
        g_sysdep_portfile->core_sysdep_network_establish == NULL ||
        g_sysdep_portfile->core_sysdep_network_recv == NULL ||
        g_sysdep_portfile->core_sysdep_network_send == NULL ||
        g_sysdep_portfile->core_sysdep_network_deinit == NULL ||
        g_sysdep_portfile->core_sysdep_rand == NULL ||
        g_sysdep_portfile->core_sysdep_mutex_init == NULL ||
        g_sysdep_portfile->core_sysdep_mutex_lock == NULL ||
        g_sysdep_portfile->core_sysdep_mutex_unlock == NULL ||
        g_sysdep_portfile->core_sysdep_mutex_deinit == NULL) {
        return NULL;
    }
    return g_sysdep_portfile;
}

