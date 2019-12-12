/**
 * @file pton.c  Network address structure functions
 *
 * Copyright (C) 2010 Creytiv.com
 */

#define _BSD_SOURCE 1
#define _DEFAULT_SOURCE 1

#ifdef HAVE_INET_PTON
#ifdef WIN32
#include <windows.h>
#else
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


#define DEBUG_MODULE "net_pton"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


#ifndef HAVE_INET_PTON


#define NS_INADDRSZ      4       /**< IPv4 T_A */
#define NS_IN6ADDRSZ     16      /**< IPv6 T_AAAA */
#define NS_INT16SZ       2       /**< #/bytes of data in a u_int16_t */


/* int
 * inet_pton4(src, dst)
 *	like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *	1 if `src' is a valid dotted quad, else 0.
 * notice:
 *	does not touch `dst' unless it's returning 1.
 * author:
 *	Paul Vixie, 1996.
 */
static int
inet_pton4(const char *src, u_char *dst)
{
	static const char digits[] = "0123456789";
	int saw_digit, octets, ch;
	u_char tmp[NS_INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr(digits, ch)) != NULL) {
			u_int newVal = (u_int) (*tp * 10 + (pch - digits));

			if (newVal > 255)
				return 0;
			*tp = newVal;
			if (! saw_digit) {
				if (++octets > 4)
					return 0;
				saw_digit = 1;
			}
		}
		else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return 0;
			*++tp = 0;
			saw_digit = 0;
		}
		else
			return 0;
	}
	if (octets < 4)
		return 0;

	memcpy(dst, tmp, NS_INADDRSZ);
	return 1;
}


#ifdef HAVE_INET6
/* int
 * inet_pton6(src, dst)
 *	convert presentation level address to network order binary form.
 * return:
 *	1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *	(1) does not touch `dst' unless it's returning 1.
 *	(2) :: in a full address is silently ignored.
 * credit:
 *	inspired by Mark Andrews.
 * author:
 *	Paul Vixie, 1996.
 */
static int
inet_pton6(const char *src, u_char *dst)
{
	static const char xdigits_l[] = "0123456789abcdef",
		xdigits_u[] = "0123456789ABCDEF";
	u_char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, saw_xdigit;
	u_int val;

	memset((tp = tmp), '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return 0;
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);
		if (pch != NULL) {
			val <<= 4;
			val |= (u_int)(pch - xdigits);
			if (val > 0xffff)
				return 0;
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') {
			curtok = src;
			if (!saw_xdigit) {
				if (colonp)
					return 0;
				colonp = tp;
				continue;
			}
			if (tp + NS_INT16SZ > endp)
				return 0;
			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
		    inet_pton4(curtok, tp) > 0) {
			tp += NS_INADDRSZ;
			saw_xdigit = 0;
			break;	/* '\0' was seen by inet_pton4(). */
		}
		return 0;
	}
	if (saw_xdigit) {
		if (tp + NS_INT16SZ > endp)
			return 0;
		*tp++ = (u_char) (val >> 8) & 0xff;
		*tp++ = (u_char) val & 0xff;
	}
	if (colonp != NULL) {
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = (int)(tp - colonp);
		int i;

		for (i = 1; i <= n; i++) {
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return 0;
	memcpy(dst, tmp, NS_IN6ADDRSZ);

	return 1;
}
#endif


#if 0
/**
 * Implementation of inet_pton()
 */
static int inet_pton(int af, const char *src, void *dst)
{
	if (!src || !dst)
		return 0;

	switch (af) {

	case AF_INET:
		return inet_pton4(src, (u_char*) dst);

#ifdef HAVE_INET6
	case AF_INET6:
		return inet_pton6(src, (u_char*) dst);
#endif

	default:
		DEBUG_INFO("inet_pton: unknown address family %d\n", af);
		errno = EAFNOSUPPORT;
		return -1;
	}
}
#endif
#endif

/**
 * Convert character string to a network address structure
 *
 * @param addr IP address string
 * @param sa   Returned socket address
 *
 * @return 0 if success, otherwise errorcode
 */
int net_inet_pton(const char *addr, struct sa *sa)
{
	if (!addr)
		return EINVAL;

	if (inet_pton(AF_INET, addr, &sa->u.in.sin_addr) > 0) {
		sa->u.in.sin_family = AF_INET;
	}
#ifdef HAVE_INET6
	else if (inet_pton(AF_INET6, addr, &sa->u.in6.sin6_addr) > 0) {

		if (IN6_IS_ADDR_V4MAPPED(&sa->u.in6.sin6_addr)) {
			const uint8_t *a = &sa->u.in6.sin6_addr.s6_addr[12];
			sa->u.in.sin_family = AF_INET;
			memcpy(&sa->u.in.sin_addr.s_addr, a, 4);
		}
		else {
			sa->u.in6.sin6_family = AF_INET6;
		}
	}
#endif
	else {
		return EINVAL;
	}

	return 0;
}
