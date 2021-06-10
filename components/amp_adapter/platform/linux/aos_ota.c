#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "ota_agent.h"
#include "ota_import.h"
#include "app_upgrade.h"

void app_js_stop()
{
}
int ota_install_jsapp(void *ota_ctx, char *store_file, int store_file_len, char *install_path)
{
    return 0;
}
int ota_verify_fsfile(ota_boot_param_t *param, char *file_path)
{
    return 0;
}
int ota_download_store_fs_start(char *url, unsigned int url_len, char *store_path,
                                report_func report_func, void *user_param)
{
    return 0;
}
int ota_report_module_version(void *ota_ctx, char *module_name, char *version)
{
    return 0;
}
int amp_app_version_get(char *version)
{
    return 0;
}
void internal_sys_upgrade_start(void *ctx)
{
}
int ota_read_parameter(ota_boot_param_t *ota_param)
{
    return 0;
}
void ota_service_param_reset(ota_service_t *ctx)
{
}
int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return 0;
}
int ota_register_module_store(ota_service_t *ctx, ota_store_module_info_t *queue, int queue_len)
{
    return 0;
}
typedef int32_t (*cbb)(void *pctx, char *ver, char *module_name, void *args);
int ota_register_trigger_msg_cb(ota_service_t *ctx, void *cb, void *param)
{
    ((cbb)cb)(1,"1", "js", param);
    return 0;
}
int ota_set_module_information(ota_service_t *ctx, char *module_name,
                               char *store_path, int module_type)
{
    return 0;
}
int ota_service_init(ota_service_t *ctx)
{
    return 0;
}
const char *amp_jsapp_version_get(void)
{
    return "0.0.1";
}
int ota_transport_inform(void *mqttclient, char *pk, char *dn, char *module_name, char *ver)
{
    return 0;
}