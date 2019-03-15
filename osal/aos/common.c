#include <k_config.h>

const char *sysinfo_kernel_version="AOS-R-2.1.0";

extern void hal_reboot(void);

void aos_reboot(void)
{
    hal_reboot();
}

int aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

const char *aos_version_get(void)
{
    return sysinfo_kernel_version;
}
