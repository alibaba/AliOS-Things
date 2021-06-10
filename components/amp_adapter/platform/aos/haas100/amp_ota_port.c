#include "amp_platform.h"
#include "aos_system.h"
#include "ota_agent.h"

void internal_sys_upgrade_start(void *ctx)
{
    ota_service_start((ota_service_t *)ctx);
    aos_task_exit(0);
    return;
}