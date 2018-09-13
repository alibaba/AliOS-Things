#ifdef DHCPD_TIMEALT
#include "kernel/os/os_time.h"
#include "dhcp_time.h"

time_t dhcp_time(time_t *timer)
{
	if (!timer)
		return (time_t)OS_GetTime();
	*timer = (time_t)OS_GetTime();
	return *timer;
}

#endif