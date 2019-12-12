/**
 * @file sa.c  Socket Address
 *
 * Copyright (C) 2010 Creytiv.com
 */
#define _BSD_SOURCE 1
#define _DEFAULT_SOURCE 1
#include <string.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_list.h>
#include <re_sa.h>
#include "sa.h"


#define DEBUG_MODULE "sa"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


/**
 * Initialize a Socket Address
 *
 * @param sa Socket Address
 * @param af Address Family
 */
void sa_init(struct sa *sa, int af)
{
	if (!sa)
		return;

	memset(sa, 0, sizeof(*sa));
	sa->u.sa.sa_family = af;
	sa->len = sizeof(sa->u);
}


/**
 * Set a Socket Address from a PL string
 *
 * @param sa   Socket Address
 * @param addr IP-address
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
int sa_set(struct sa *sa, const struct pl *addr, uint16_t port)
{
	char buf[64];

	(void)pl_strcpy(addr, buf, sizeof(buf));
	return sa_set_str(sa, buf, port);
}


/**
 * Set a Socket Address from a string
 *
 * @param sa   Socket Address
 * @param addr IP-address
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
int sa_set_str(struct sa *sa, const char *addr, uint16_t port)
{
	int err;

	if (!sa || !addr)
		return EINVAL;

	err = net_inet_pton(addr, sa);
	if (err)
		return err;

	switch (sa->u.sa.sa_family) {

	case AF_INET:
		sa->u.in.sin_port = htons(port);
		sa->len = sizeof(struct sockaddr_in);
		break;

#ifdef HAVE_INET6
	case AF_INET6:
		sa->u.in6.sin6_port = htons(port);
		sa->len = sizeof(struct sockaddr_in6);
		break;
#endif

	default:
		return EAFNOSUPPORT;
	}

	return 0;
}


/**
 * Set a Socket Address from an IPv4 address
 *
 * @param sa   Socket Address
 * @param addr IPv4 address in host order
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
void sa_set_in(struct sa *sa, uint32_t addr, uint16_t port)
{
	if (!sa)
		return;

	sa->u.in.sin_family = AF_INET;
	sa->u.in.sin_addr.s_addr = htonl(addr);
	sa->u.in.sin_port = htons(port);
	sa->len = sizeof(struct sockaddr_in);
}


/**
 * Set a Socket Address from an IPv6 address
 *
 * @param sa   Socket Address
 * @param addr IPv6 address
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
void sa_set_in6(struct sa *sa, const uint8_t *addr, uint16_t port)
{
	if (!sa)
		return;

#ifdef HAVE_INET6
	sa->u.in6.sin6_family = AF_INET6;
	memcpy(&sa->u.in6.sin6_addr, addr, 16);
	sa->u.in6.sin6_port = htons(port);
	sa->len = sizeof(struct sockaddr_in6);
#else
	(void)addr;
	(void)port;
#endif
}


/**
 * Set a Socket Address from a sockaddr
 *
 * @param sa Socket Address
 * @param s  Sockaddr
 *
 * @return 0 if success, otherwise errorcode
 */
int sa_set_sa(struct sa *sa, const struct sockaddr *s)
{
	if (!sa || !s)
		return EINVAL;

	switch (s->sa_family) {

	case AF_INET:
		memcpy(&sa->u.in, s, sizeof(struct sockaddr_in));
		sa->len = sizeof(struct sockaddr_in);
		break;

#ifdef HAVE_INET6
	case AF_INET6:
		memcpy(&sa->u.in6, s, sizeof(struct sockaddr_in6));
		sa->len = sizeof(struct sockaddr_in6);
		break;
#endif

	default:
		return EAFNOSUPPORT;
	}

	sa->u.sa.sa_family = s->sa_family;

	return 0;
}


/**
 * Set the port number on a Socket Address
 *
 * @param sa   Socket Address
 * @param port Port number
 */
void sa_set_port(struct sa *sa, uint16_t port)
{
	if (!sa)
		return;

	switch (sa->u.sa.sa_family) {

	case AF_INET:
		sa->u.in.sin_port = htons(port);
		break;

#ifdef HAVE_INET6
	case AF_INET6:
		sa->u.in6.sin6_port = htons(port);
		break;
#endif

	default:
		DEBUG_WARNING("sa_set_port: no af %d (port %u)\n",
			      sa->u.sa.sa_family, port);
		break;
	}
}


/**
 * Set a socket address from a string of type "address:port"
 * IPv6 addresses must be encapsulated in square brackets.
 *
 * @param sa   Socket Address
 * @param str  Address and port string
 * @param len  Length of string
 *
 * @return 0 if success, otherwise errorcode
 *
 * Example strings:
 *
 * <pre>
 *   1.2.3.4:1234
 *   [::1]:1234
 *   [::]:5060
 * </pre>
 */
int sa_decode(struct sa *sa, const char *str, size_t len)
{
	struct pl addr, port, pl;
	const char *c;

	if (!sa || !str || !len)
		return EINVAL;

	pl.p = str;
	pl.l = len;

	if ('[' == str[0] && (c = pl_strchr(&pl, ']'))) {
		addr.p = str + 1;
		addr.l = c - str - 1;
		++c;
	}
	else if (NULL != (c = pl_strchr(&pl, ':'))) {
		addr.p = str;
		addr.l = c - str;
	}
	else {
		return EINVAL;
	}

	if (len < (size_t)(c - str + 2))
		return EINVAL;

	if (':' != *c)
		return EINVAL;

	port.p = ++c;
	port.l = len + str - c;

	return sa_set(sa, &addr, pl_u32(&port));
}


/**
 * Get the Address Family of a Socket Address
 *
 * @param sa Socket Address
 *
 * @return Address Family
 */
int sa_af(const struct sa *sa)
{
	return sa ? sa->u.sa.sa_family : AF_UNSPEC;
}


/**
 * Get the IPv4-address of a Socket Address
 *
 * @param sa Socket Address
 *
 * @return IPv4 address in host order
 */
uint32_t sa_in(const struct sa *sa)
{
	return sa ? ntohl(sa->u.in.sin_addr.s_addr) : 0;
}


/**
 * Get the IPv6-address of a Socket Address
 *
 * @param sa   Socket Address
 * @param addr On return, contains the IPv6-address
 */
void sa_in6(const struct sa *sa, uint8_t *addr)
{
	if (!sa || !addr)
		return;

#ifdef HAVE_INET6
	memcpy(addr, &sa->u.in6.sin6_addr, 16);
#endif
}


/**
 * Convert a Socket Address to Presentation format
 *
 * @param sa   Socket Address
 * @param buf  Buffer to store presentation format
 * @param size Buffer size
 *
 * @return 0 if success, otherwise errorcode
 */
int sa_ntop(const struct sa *sa, char *buf, int size)
{
	return net_inet_ntop(sa, buf, size);
}


/**
 * Get the port number from a Socket Address
 *
 * @param sa Socket Address
 *
 * @return Port number  in host order
 */
uint16_t sa_port(const struct sa *sa)
{
	if (!sa)
		return 0;

	switch (sa->u.sa.sa_family) {

	case AF_INET:
		return ntohs(sa->u.in.sin_port);

#ifdef HAVE_INET6
	case AF_INET6:
		return ntohs(sa->u.in6.sin6_port);
#endif

	default:
		return 0;
	}
}


/**
 * Check if a Socket Address is set
 *
 * @param sa   Socket Address
 * @param flag Flags specifying which fields to check
 *
 * @return true if set, false if not set
 */
bool sa_isset(const struct sa *sa, int flag)
{
	if (!sa)
		return false;

	switch (sa->u.sa.sa_family) {

	case AF_INET:
		if (flag & SA_ADDR)
			if (INADDR_ANY == sa->u.in.sin_addr.s_addr)
				return false;
		if (flag & SA_PORT)
			if (0 == sa->u.in.sin_port)
				return false;
		break;

#ifdef HAVE_INET6
	case AF_INET6:
		if (flag & SA_ADDR)
			if (IN6_IS_ADDR_UNSPECIFIED(&sa->u.in6.sin6_addr))
				return false;
		if (flag & SA_PORT)
			if (0 == sa->u.in6.sin6_port)
				return false;
		break;
#endif

	default:
		return false;
	}

	return true;
}


/**
 * Calculate the hash value of a Socket Address
 *
 * @param sa   Socket Address
 * @param flag Flags specifying which fields to use
 *
 * @return Hash value
 */
uint32_t sa_hash(const struct sa *sa, int flag)
{
	uint32_t v = 0;

	if (!sa)
		return 0;

	switch (sa->u.sa.sa_family) {

	case AF_INET:
		if (flag & SA_ADDR)
			v += ntohl(sa->u.in.sin_addr.s_addr);
		if (flag & SA_PORT)
			v += ntohs(sa->u.in.sin_port);
		break;

#ifdef HAVE_INET6
	case AF_INET6:
		if (flag & SA_ADDR) {
			uint32_t *a = (uint32_t *)&sa->u.in6.sin6_addr;
			v += a[0] ^ a[1] ^ a[2] ^ a[3];
		}
		if (flag & SA_PORT)
			v += ntohs(sa->u.in6.sin6_port);
		break;
#endif

	default:
		DEBUG_WARNING("sa_hash: unknown af %d\n", sa->u.sa.sa_family);
		return 0;
	}

	return v;
}


/**
 * Copy a Socket Address
 *
 * @param dst Socket Address to be written
 * @param src Socket Address to be copied
 */
void sa_cpy(struct sa *dst, const struct sa *src)
{
	if (!dst || !src)
		return;

	memcpy(dst, src, sizeof(*dst));
}


/**
 * Compare two Socket Address objects
 *
 * @param l    Socket Address number one
 * @param r    Socket Address number two
 * @param flag Flags specifying which fields to use
 *
 * @return true if match, false if no match
 */
bool sa_cmp(const struct sa *l, const struct sa *r, int flag)
{
	if (!l || !r)
		return false;

	if (l == r)
		return true;

	if (l->u.sa.sa_family != r->u.sa.sa_family)
		return false;

	switch (l->u.sa.sa_family) {

	case AF_INET:
		if (flag & SA_ADDR)
			if (l->u.in.sin_addr.s_addr != r->u.in.sin_addr.s_addr)
				return false;
		if (flag & SA_PORT)
			if (l->u.in.sin_port != r->u.in.sin_port)
				return false;
		break;

#ifdef HAVE_INET6
	case AF_INET6:
		if (flag & SA_ADDR)
			if (memcmp(&l->u.in6.sin6_addr,
				   &r->u.in6.sin6_addr, 16))
				return false;
		if (flag & SA_PORT)
			if (l->u.in6.sin6_port != r->u.in6.sin6_port)
				return false;
		break;
#endif

	default:
		return false;
	}

	return true;
}


/** IPv4 Link-local test */
#define IN_IS_ADDR_LINKLOCAL(a)					\
	(((a) & htonl(0xffff0000)) == htonl (0xa9fe0000))


/**
 * Check if socket address is a link-local address
 *
 * @param sa Socket address
 *
 * @return true if link-local address, otherwise false
 */
bool sa_is_linklocal(const struct sa *sa)
{
	if (!sa)
		return false;

	switch (sa_af(sa)) {

	case AF_INET:
		return IN_IS_ADDR_LINKLOCAL(sa->u.in.sin_addr.s_addr);

#ifdef HAVE_INET6
	case AF_INET6:
		return IN6_IS_ADDR_LINKLOCAL(&sa->u.in6.sin6_addr);
#endif

	default:
		return false;
	}
}


/**
 * Check if socket address is a loopback address
 *
 * @param sa Socket address
 *
 * @return true if loopback address, otherwise false
 */
bool sa_is_loopback(const struct sa *sa)
{
	if (!sa)
		return false;

	switch (sa_af(sa)) {

	case AF_INET:
		return INADDR_LOOPBACK == ntohl(sa->u.in.sin_addr.s_addr);

#ifdef HAVE_INET6
	case AF_INET6:
		return IN6_IS_ADDR_LOOPBACK(&sa->u.in6.sin6_addr);
#endif

	default:
		return false;
	}
}


/**
 * Check if socket address is any/unspecified address
 *
 * @param sa Socket address
 *
 * @return true if any address, otherwise false
 */
bool sa_is_any(const struct sa *sa)
{
	if (!sa)
		return false;

	switch (sa_af(sa)) {

	case AF_INET:
		return INADDR_ANY == ntohl(sa->u.in.sin_addr.s_addr);

#ifdef HAVE_INET6
	case AF_INET6:
		return IN6_IS_ADDR_UNSPECIFIED(&sa->u.in6.sin6_addr);
#endif

	default:
		return false;
	}
}
