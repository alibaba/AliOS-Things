/**
 * @file ntp.c  NTP Routines
 *
 * Copyright (C) 2010 Creytiv.com
 */
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <re_types.h>
#include <re_fmt.h>
#include <re_list.h>
#include <re_sa.h>
#include <re_rtp.h>
#include "rtcp.h"


/*
 * Unix time:  seconds relative to 0h January 1, 1970
 * NTP time:   seconds relative to 0h UTC on 1 January 1900
 */


/* Number of seconds from 1900 to 1970 */
#define UNIX_NTP_OFFSET 0x83aa7e80


/**
 * Convert from Unix time to NTP time
 *
 * @param ntp NTP time to convert to (output)
 * @param tv  Unix time to convert from (input)
 */
void unix2ntp(struct ntp_time *ntp, const struct timeval *tv)
{
	ntp->hi = (uint32_t)(tv->tv_sec + UNIX_NTP_OFFSET);
	ntp->lo = (uint32_t)((double)tv->tv_usec*(double)(1LL<<32)*1.0e-6);
}


/**
 * Convert from NTP time to Unix time
 *
 * @param tv  Unix time to convert to (output)
 * @param ntp NTP time to convert from (input)
 */
void ntp2unix(struct timeval *tv, const struct ntp_time *ntp)
{
	tv->tv_sec  = ntp->hi - UNIX_NTP_OFFSET;
	tv->tv_usec = (uint32_t)(1.0e6 * (double) ntp->lo / (1LL<<32));
}


int ntp_time_get(struct ntp_time *ntp)
{
	struct timeval tv;
#ifdef WIN32
	union {
		long long ns100;
		FILETIME ft;
	} now;

	GetSystemTimeAsFileTime(&now.ft);
	tv.tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
	tv.tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
#else
	if (gettimeofday(&tv, NULL) != 0)
		return errno;
#endif
	unix2ntp(ntp, &tv);

	return 0;
}


/**
 * Convert NTP time to middle 32-bits (compact representation)
 *
 * @param ntp NTP time
 *
 * @return NTP time in compact representation
 */
uint32_t ntp_compact(const struct ntp_time *ntp)
{
	return ntp ? ((ntp->hi & 0xffff) << 16 | (ntp->lo >> 16)) : 0;
}


/**
 * Convert NTP compact representation to microseconds
 *
 * @param ntpc  NTP time in compact representation
 *
 * @return NTP time in microseconds
 */
uint64_t ntp_compact2us(uint32_t ntpc)
{
	const uint32_t hi = (ntpc >> 16) & 0xffff;
	const uint32_t lo = (ntpc & 0xffff) << 16;

	return (1000000ULL * hi) + ((1000000ULL * lo) >> 32);
}
