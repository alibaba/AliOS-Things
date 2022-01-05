#include "dm_export.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_dm_api.h"

extern const char *ali_ca_cert;
SYMBOL_EXPORT(ali_ca_cert);
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;
SYMBOL_EXPORT(g_aiot_sysdep_portfile);
SYMBOL_EXPORT(aiot_dm_send);
SYMBOL_EXPORT(aiot_mqtt_deinit);
SYMBOL_EXPORT(aiot_mqtt_connect);
SYMBOL_EXPORT(aiot_mqtt_recv);
SYMBOL_EXPORT(aiot_sysdep_set_portfile);
SYMBOL_EXPORT(aiot_mqtt_process);
SYMBOL_EXPORT(aiot_state_set_logcb);
SYMBOL_EXPORT(aiot_mqtt_setopt);
SYMBOL_EXPORT(aiot_dm_setopt);
SYMBOL_EXPORT(aiot_dm_init);
SYMBOL_EXPORT(aiot_mqtt_init);
