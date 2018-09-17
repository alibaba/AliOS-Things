#ifndef __PING_H__
#define __PING_H__

#include "lwip/raw.h"
#include "lwip/icmp.h"

/**
 * PING_USE_SOCKETS: Set to 1 to use sockets, otherwise the raw api is used
 */
#ifndef PING_USE_SOCKETS
#define PING_USE_SOCKETS    LWIP_SOCKET
#endif

//! @brief Typedef for callback function.
//!
//! If a valid echo reply is received, this callback will be invoked. The @a echo
//! parameter will be a valid pointer. If the echo timed out, the callback will
//! be invoked will @a echo set to NULL.
typedef void (*ping_callback_t)(const struct icmp_echo_hdr * echo, const ip_addr_t *addr, int elapsed);

void ping_init(void);

#if !PING_USE_SOCKETS
void ping_send_now(void);
void ping_send_to(ip_addr_t * ping_target, ping_callback_t callback);
#endif /* !PING_USE_SOCKETS */

#endif /* __PING_H__ */
