/*
 * coap_config.h.lwip -- LwIP configuration for libcoap
 *
 * Copyright (C) 2021 Olaf Bergmann <bergmann@tzi.org> and others
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#ifndef COAP_CONFIG_H_
#define COAP_CONFIG_H_

#include <lwip/opt.h>
#include <lwip/debug.h>
#include <lwip/def.h> /* provide ntohs, htons */

#define WITH_LWIP 1

#ifndef COAP_CONSTRAINED_STACK
#define COAP_CONSTRAINED_STACK 1
#endif

#ifndef COAP_DISABLE_TCP
#define COAP_DISABLE_TCP 1
#endif

#define PACKAGE_NAME "libcoap"
#define PACKAGE_VERSION "4.3.0"
#define PACKAGE_STRING "libcoap 4.3.0"

#define assert(x) LWIP_ASSERT("CoAP assert failed", x)

/* it's just provided by libc. i hope we don't get too many of those, as
 * actually we'd need autotools again to find out what environment we're
 * building in */
#define HAVE_STRNLEN 1

#define HAVE_LIMITS_H

#endif /* COAP_CONFIG_H_ */
