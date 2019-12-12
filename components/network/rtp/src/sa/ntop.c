/**
 * @file ntop.c  Network address structure functions
 *
 * Copyright (C) 2010 Creytiv.com
 */

#define _BSD_SOURCE 1
#define _DEFAULT_SOURCE 1

#ifdef HAVE_INET_NTOP
#ifdef WIN32
#ifdef _MSC_VER
#pragma warning (disable: 4090)
#endif
#include <windows.h>
#else
#define __USE_BSD 1    /**< Use BSD code */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define __USE_POSIX 1  /**< Use POSIX code */
#include <netdb.h>
#endif /* WIN32 */
#endif
#include <string.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mbuf.h>
#include <re_sa.h>
#include "sa.h"


#define DEBUG_MODULE "net_ntop"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


#ifndef HAVE_INET_NTOP


#define NS_IN6ADDRSZ     16      /**< IPv6 T_AAAA */
#define NS_INT16SZ       2       /**< #/bytes of data in a u_int16_t */


static const char*
inet_ntop4(const u_char *src, char *dst, size_t size)
{
	if (re_snprintf(dst, size, "%u.%u.%u.%u",
			src[0], src[1], src[2], src[3]) < 0) {
		errno = ENOSPC;
		dst[size-1] = 0;
		return NULL;
	}

	return dst;
}


#ifdef HAVE_INET6
/* const char *
 * inet_ntop6(src, dst, size)
 *	convert IPv6 binary address into presentation (printable) format
 * author:
 *	Paul Vixie, 1996.
 */

static const char *
inet_ntop6(const u_char *src, char *dst, size_t size)
{
	/*
	 * Note that int32_t and int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 */
	char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
	struct { int base, len; } best, cur;
	u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
	int i;

	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	memset(words, '\0', sizeof words);
	for (i = 0; i < NS_IN6ADDRSZ; i++)
		words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
	best.base = -1;
	best.len = 0;
	cur.base = -1;
	cur.len = 0;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		}
		else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base &&
		    i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex?*/
		if (i != 0)
			*tp++ = ':';
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 &&
		    (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if (!inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp)))
				return NULL;
			tp += strlen(tp);
			break;
		}
		tp += sprintf(tp, "%x", words[i]);
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) ==
	    (NS_IN6ADDRSZ / NS_INT16SZ))
		*tp++ = ':';
	*tp++ = '\0';

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((size_t)(tp - tmp) > size) {
		errno = ENOSPC;
		return NULL;
	}
	strcpy(dst, tmp);

	return dst;
}
#endif


/*
 * Implementation of inet_ntop()
 */
//const char* inet_ntop(int af, const void *src, char *dst, size_t size);
#if 0
const char* inet_ntop(int af, const void *src, char *dst, size_t size)
{
	switch (af) {

	case AF_INET:
		return inet_ntop4(src, dst, size);

#ifdef HAVE_INET6
	case AF_INET6:
		return inet_ntop6(src, dst, size);
#endif

	default:
		DEBUG_WARNING("inet_ntop: unknown address family %d\n", af);
		return NULL;
	}
}
#endif
#endif


/*
 *  POSIX  1003.1-2001 marks gethostbyaddr() and gethostbyname() obsolescent.
 *  See getaddrinfo(3), getnameinfo(3), gai_strerror(3).
 */


/**
 * Convert network address structure to a character string
 *
 * @param sa   Socket address
 * @param buf  Buffer to return IP address
 * @param size Size of buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int net_inet_ntop(const struct sa *sa, char *buf, int size)
{
	if (!sa || !buf || !size)
		return EINVAL;

	switch (sa->u.sa.sa_family) {

	case AF_INET:
		inet_ntop(AF_INET, &sa->u.in.sin_addr, buf, size);
		break;

#ifdef HAVE_INET6
	case AF_INET6:
		inet_ntop(AF_INET6, &sa->u.in6.sin6_addr, buf, size);
		break;
#endif

	default:
		return EAFNOSUPPORT;
	}

	return 0;
}
