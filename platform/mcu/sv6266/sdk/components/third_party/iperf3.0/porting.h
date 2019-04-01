/*
*  (C) Copyright 2017 
*
*  All Rights Reserved
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "netstack_def.h"

#ifndef _OS_PORTING_H_
#define _OS_PORTING_H_

#define PLATFORM_SUPPORT_64BIT    0
#define PLATFORM_IPERF_TIMER_USE_US    0
#define PLATFORM_IPERF_ENABLE_JSON_OUTPUT    0
#define PLATFORM_IPERF_ENABLE_LINUX_API    0
#define PLATFORM_IPERF_ENABLE_FILE_SEND    0
#define PLATFORM_IPERF_SUPPORT_TCP_INFO    0
#define PLATFORM_IPERF_ENABLE_TURNING    1
#define PLATFORM_IPERF_ENABLE_CJSON_PRINT_TURNING    1
#define PLATFORM_IPERF_ENABLE_TCP_MSS 0
#define PLATFORM_IPERF_ENABLE_ORIGIN_UNITS_API 0

//#define strdup iperf_strdup
#define ISUPPER isupper
#define TOUPPER toupper
#define TOLOWER tolower
#define MEMSET memset
#define STRCPY strcpy
#define STRLEN strlen
#define STRCHR strchr
#define STRNCAT strncat
#define ATOI atoi
#define STRNCMP strncmp

#if( PLATFORM_SUPPORT_64BIT == 1 )
#define int64_t  long long
#define uint64_t unsigned long long
#define retUIntType uint64_t
#define uIntType uint64_t
#define intType int64_t
#define retFloatType double
#define floatType double
#else
//#define int32_t  long
//#define uint32_t unsigned long
#define retUIntType uint32_t
#define uIntType uint32_t
#define intType int32_t
#define retFloatType float
#define floatType float
#endif

//typedef uIntType iperf_size_t;
//#define ssize_t int
#define time_t int


/* Possible values for `ai_flags' field in `addrinfo' structure.  */
#ifndef AI_PASSIVE
    # define AI_PASSIVE	0x0001	/* Socket address is intended for `bind'.  */
#endif

#ifndef AI_CANONNAME
    # define AI_CANONNAME	0x0002	/* Request for canonical name.  */
#endif

#ifndef AI_NUMERICHOST
# define AI_NUMERICHOST	0x0004	/* Don't use name resolution.  */
#endif

#ifndef AI_V4MAPPED
# define AI_V4MAPPED	0x0008	/* IPv4 mapped addresses are acceptable.  */
#endif

#ifndef AI_ALL
# define AI_ALL		0x0010	/* Return IPv4 mapped and IPv6 addresses.  */
#endif

#ifndef AI_ADDRCONFIG
# define AI_ADDRCONFIG	0x0020	/* Use configuration of this host to choose
				   returned address type..  */
#endif

#endif
