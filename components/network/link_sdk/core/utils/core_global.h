#ifndef _CORE_GLOBAL_H_
#define _CORE_GLOBAL_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"

#define CORE_GLOBAL_MODULE_NAME "global"

int32_t core_global_init(aiot_sysdep_portfile_t *sysdep);
int32_t core_global_alink_id_next(aiot_sysdep_portfile_t *sysdep, int32_t *alink_id);
int32_t core_global_set_mqtt_backup_ip(aiot_sysdep_portfile_t *sysdep, char ip[16]);
int32_t core_global_get_mqtt_backup_ip(aiot_sysdep_portfile_t *sysdep, char ip[16]);
int32_t core_global_deinit(aiot_sysdep_portfile_t *sysdep);

#if defined(__cplusplus)
}
#endif

#endif

