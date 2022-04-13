/**
 * @file re_sa.h  Interface to Socket Address
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_SA_H
#define RE_SA_H

#include "re_common.h"
#if defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <network/network.h>
#endif

/**
 * @addtogroup aos_rtp_sa rtp_sa
 * @{
 */

/** Socket Address flags */
enum sa_flag {
	SA_ADDR      = 1<<0,
	SA_PORT      = 1<<1,
	SA_ALL       = SA_ADDR | SA_PORT
};

/**
 * Initialize a Socket Address
 *
 * @param sa Socket Address
 * @param af Address Family
 */
void     sa_init(struct sa *sa, int af);

/**
 * Set a Socket Address from a PL string
 *
 * @param sa   Socket Address
 * @param addr IP-address
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
int      sa_set(struct sa *sa, const struct pl *addr, uint16_t port);

/**
 * Set a Socket Address from a string
 *
 * @param sa   Socket Address
 * @param addr IP-address
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
int      sa_set_str(struct sa *sa, const char *addr, uint16_t port);

/**
 * Set a Socket Address from an IPv4 address
 *
 * @param sa   Socket Address
 * @param addr IPv4 address in host order
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
void     sa_set_in(struct sa *sa, uint32_t addr, uint16_t port);

/**
 * Set a Socket Address from an IPv6 address
 *
 * @param sa   Socket Address
 * @param addr IPv6 address
 * @param port Port number
 *
 * @return 0 if success, otherwise errorcode
 */
void     sa_set_in6(struct sa *sa, const uint8_t *addr, uint16_t port);

/**
 * Set a Socket Address from a sockaddr
 *
 * @param sa Socket Address
 * @param s  Sockaddr
 *
 * @return 0 if success, otherwise errorcode
 */
int      sa_set_sa(struct sa *sa, const struct sockaddr *s);

/**
 * Set the port number on a Socket Address
 *
 * @param sa   Socket Address
 * @param port Port number
 */
void     sa_set_port(struct sa *sa, uint16_t port);

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
int      sa_decode(struct sa *sa, const char *str, size_t len);

/**
 * Get the Address Family of a Socket Address
 *
 * @param sa Socket Address
 *
 * @return Address Family
 */
int      sa_af(const struct sa *sa);

/**
 * Get the IPv4-address of a Socket Address
 *
 * @param sa Socket Address
 *
 * @return IPv4 address in host order
 */
uint32_t sa_in(const struct sa *sa);

/**
 * Get the IPv6-address of a Socket Address
 *
 * @param sa   Socket Address
 * @param addr On return, contains the IPv6-address
 */
void     sa_in6(const struct sa *sa, uint8_t *addr);

/**
 * Convert a Socket Address to Presentation format
 *
 * @param sa   Socket Address
 * @param buf  Buffer to store presentation format
 * @param size Buffer size
 *
 * @return 0 if success, otherwise errorcode
 */
int      sa_ntop(const struct sa *sa, char *buf, int size);

/**
 * Get the port number from a Socket Address
 *
 * @param sa Socket Address
 *
 * @return Port number  in host order
 */
uint16_t sa_port(const struct sa *sa);

/**
 * Check if a Socket Address is set
 *
 * @param sa   Socket Address
 * @param flag Flags specifying which fields to check
 *
 * @return true if set, false if not set
 */
bool     sa_isset(const struct sa *sa, int flag);

/**
 * Calculate the hash value of a Socket Address
 *
 * @param sa   Socket Address
 * @param flag Flags specifying which fields to use
 *
 * @return Hash value
 */
uint32_t sa_hash(const struct sa *sa, int flag);

/**
 * Copy a Socket Address
 *
 * @param dst Socket Address to be written
 * @param src Socket Address to be copied
 */
void     sa_cpy(struct sa *dst, const struct sa *src);

/**
 * Compare two Socket Address objects
 *
 * @param l    Socket Address number one
 * @param r    Socket Address number two
 * @param flag Flags specifying which fields to use
 *
 * @return true if match, false if no match
 */
bool     sa_cmp(const struct sa *l, const struct sa *r, int flag);

/**
 * Check if socket address is a link-local address
 *
 * @param sa Socket address
 *
 * @return true if link-local address, otherwise false
 */
bool     sa_is_linklocal(const struct sa *sa);

/**
 * Check if socket address is a loopback address
 *
 * @param sa Socket address
 *
 * @return true if loopback address, otherwise false
 */
bool     sa_is_loopback(const struct sa *sa);

/**
 * Check if socket address is any/unspecified address
 *
 * @param sa Socket address
 *
 * @return true if any address, otherwise false
 */
bool     sa_is_any(const struct sa *sa);

/**
 * Print a Socket Address including IPv6 scope identifier
 *
 * @param pf Print function
 * @param sa Socket Address
 *
 * @return 0 if success, otherwise errorcode
 */
int      sa_print_addr(struct re_printf *pf, const struct sa *sa);

/** @} */
#endif /* RE_SA_H */
