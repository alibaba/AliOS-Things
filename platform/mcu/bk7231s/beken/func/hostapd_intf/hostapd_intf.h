#ifndef _HOSTAPD_INTF_H_
#define _HOSTAPD_INTF_H_

#define HINTF_SUCCESS           (0)
#define HINTF_FAILURE           (-1)

#define WPAS_DEBUG

#ifdef WPAS_DEBUG
#define WPAS_PRT       os_printf
#define WPAS_WPRT      warning_prf
#else
#define WPAS_PRT       os_null_printf
#define WPAS_WPRT      warning_prf
#endif

#endif
// eof

