/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 2007 Denys Vlasenko
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */
#include "udhcp_common.h"
#if DHCPD_SUPPORT

#define os_getSysTime (os_tick2ms(OS_GetSysTick())/1000) //ms to s
uint32_t ssv_time(void *arg)
{
	return os_getSysTime;
}
#endif
