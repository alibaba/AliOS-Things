/*
 * wpa_supplicant/hostapd - Default include files
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * This header file is included into all C files so that commonly used header
 * files can be selected with OS specific ifdef blocks in one place instead of
 * having to have OS/C library specific selection in many files.
 */
#include <config.h>

#ifndef INCLUDES_H
#define INCLUDES_H

//#define DCONFIG_NO_WPA
//#define DCONFIG_NO_WPA2

#define CONFIG_CRYPTO_INTERNAL
//#define CONFIG_RANDOM_POOL

//Debug option
/*-------------------------------*/
#define CONFIG_NO_STDOUT_DEBUG
#define CONFIG_NO_WPA_MSG
#define CONFIG_NO_WPA_HEXDUMP
/*-------------------------------*/
#define EAPOL_TEST
//#define PROFILE_SUPPLICANT
// Dynamic allocate packet buffer for HMAC SHA1 key calculation.
// Packet buffer access is slower than SRAM. PMK calculation time is longer.
// But static memory costs image size.
#ifdef ENABLE_DYNAMIC_HMAC_SHA1_BUF
#undef ENABLE_DYNAMIC_HMAC_SHA1_BUF
#endif




//#define SHA1_HARDWARE_IMPLEMENT
#define OS_NO_C_LIB_DEFINES


//#include <stdlib.h>
//#include <stdio.h>
//#include <stdarg.h>
//#include <string.h>

#endif /* INCLUDES_H */
