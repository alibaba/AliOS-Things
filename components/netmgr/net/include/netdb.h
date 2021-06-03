/**
 * @file
 * This file is a posix wrapper for lwip/netdb.h.
 */

/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 */

#ifndef _NETDB_H
#define _NETDB_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef WITH_LWIP
#include "lwip/netdb.h"
#endif
#ifdef WITH_SAL
#include "sal_sockets.h"
#endif

struct hostent *net_gethostbyname(const char *name);
int net_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                size_t buflen, struct hostent **result, int *h_errnop);
void net_freeaddrinfo(struct addrinfo *ai);
int net_getaddrinfo(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res);

#define gethostbyname(name) net_gethostbyname(name)
/** @ingroup netdbapi */
#define gethostbyname_r(name, ret, buf, buflen, result, h_errnop) \
       net_gethostbyname_r(name, ret, buf, buflen, result, h_errnop)
/** @ingroup netdbapi */
#define freeaddrinfo(addrinfo) net_freeaddrinfo(addrinfo)
/** @ingroup netdbapi */
#define getaddrinfo(nodname, servname, hints, res) \
       net_getaddrinfo(nodname, servname, hints, res)

#ifdef __cplusplus
}
#endif

#endif /* _NETDB_H */
