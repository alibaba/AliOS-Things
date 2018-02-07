#ifndef DHCP_TMER_H_H
#define DHCP_TMER_H_H

#ifdef DHCPD_TIMEALT
#include <time.h>
time_t dhcp_time(time_t *timer);

#define time dhcp_time

#endif

#endif
