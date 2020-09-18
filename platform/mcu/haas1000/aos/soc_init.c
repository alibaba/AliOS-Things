#include <stdio.h>
#include "hal_cmu.h"
#include "hal_bootmode.h"
#include "pmu.h"

extern volatile uint32_t g_crash_steps;
static int g_odm_type;

void set_odm_type(int type)
{
    g_odm_type = type;
}

void hal_reboot(void)
{
#ifdef AOS_COMP_DEBUG
	if (g_crash_steps) {
		hal_sw_bootmode_set(HAL_SW_BOOTMODE_REBOOT_FROM_CRASH);
	}
#endif
    if (g_odm_type == 1) {
        /*workaround for marie*/
        pmu_write(0x2b, 0xee89);
    }

    hal_cmu_sys_reboot();
}

int set_silent_reboot_flag(void)
{
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_RESERVED_BIT24);
    return 0;
}

/*0:first boot, 1:silent boot*/
int probe_silent_reboot_flag(void)
{
    return (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_RESERVED_BIT24);
}
