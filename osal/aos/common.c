#include <k_config.h>

const char *SYSINFO_KERNEL_VERSION="AOS-R-2.0.0";

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
    return SYSINFO_KERNEL_VERSION;
}
