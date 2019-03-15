/*-
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mbuf.h	8.5 (Berkeley) 2/19/95
 * $FreeBSD: releng/10.1/sys/sys/mbuf.h 269047 2014-07-24 06:03:45Z kevlo $
 */

#ifndef _SYS_MBUF_0_H_
#define _SYS_MBUF_0_H_

#if (__CONFIG_MBUF_IMPL_MODE == 0)

#include <stdint.h>
#ifdef __CONFIG_ARCH_DUAL_CORE
#include "sys/ducc/ducc_addr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Callback structure
 */
struct m_cb {
	void *func;
	void *arg;
};

/*
 * Record/packet header in first mbuf of chain; always valid and M_PKTHDR is set.
 * Size : 24
 */
struct pkthdr {
	void    *rcvif; /* rcv interface */
	int32_t len;    /* total packet length */

	union {
		uint8_t   eigth[8];
		uint16_t  sixteen[4];
		uint32_t  thirtytwo[2];
		uint64_t  sixtyfour[1];
		uintptr_t unintptr[1];
		void      *ptr;
	} PH_per;

	struct m_cb cb;
};
#define ether_vtag  PH_per.sixteen[0]
#define PH_vt       PH_per
#define vt_nrecs    sixteen[0]
#define tso_segsz   PH_per.sixteen[1]
#define csum_phsum  PH_per.sixteen[2]
#define csum_data   PH_per.thirtytwo[1]

/*
 * The core of the mbuf object along with some shortcut defines for practical
 * purposes.
 */
struct mbuf { // Size : 48
	/*
	 * Header present at the beginning of every mbuf.
	 * Size : 24
	 */
	uint8_t       *m_buf;         /* a continuous buffer */ // useless now
	struct mbuf   *m_nextpkt;     /* next chain in queue/record */
	uint8_t       *m_data;        /* location of data */
	int32_t       m_len;          /* amount of data in this mbuf */
	uint16_t      m_headspace;    /* empty space available at the head */
	uint16_t      m_tailspace;    /* empty space available at the tail */
	uint32_t      m_type  :8,     /* type of data in this mbuf */ // useless now
	              m_flags :24;    /* flags; see below */
	/*** End of the mbuf header ***/

	struct pkthdr m_pkthdr;	/* M_PKTHDR always set */
};

#ifdef __CONFIG_ARCH_DUAL_CORE
/*
 * NB: mbuf is allocated from net core, pointer conversion MUST be done when
 *     transfer mbuf from app core to net core, or vice versa.
 */

#ifdef __CONFIG_ARCH_NET_CORE
/* convert mbuf's pointers in net core, the new mbuf is used by net core */
#define MBUF_APP2NET(m)                                             \
    do {                                                            \
        (m) = (struct mbuf *)DUCC_NETMEM_APP2NET(m);                \
        (m)->m_data = (uint8_t *)DUCC_NETMEM_APP2NET((m)->m_data);  \
    } while (0)

/* convert mbuf's pointers in net core, the new mbuf is used by app core */
#define MBUF_NET2APP(m)                                             \
    do {                                                            \
        (m)->m_data = (uint8_t *)DUCC_NETMEM_NET2APP((m)->m_data);  \
        (m) = (struct mbuf *)DUCC_NETMEM_NET2APP(m);                \
    } while (0)
#endif /* __CONFIG_ARCH_NET_CORE */

#ifdef __CONFIG_ARCH_APP_CORE
/* convert mbuf's pointers in app core, the new mbuf is used by net core */
#define MBUF_APP2NET(m)                                             \
    do {                                                            \
        (m)->m_data = (uint8_t *)DUCC_NETMEM_APP2NET((m)->m_data);  \
        (m) = (struct mbuf *)DUCC_NETMEM_APP2NET(m);                \
    } while (0)

/* convert mbuf's pointers in app core, the new mbuf is used by app core */
#define MBUF_NET2APP(m)                                             \
    do {                                                            \
        (m) = (struct mbuf *)DUCC_NETMEM_NET2APP(m);                \
        (m)->m_data = (uint8_t *)DUCC_NETMEM_NET2APP((m)->m_data);  \
    } while (0)
#endif /* __CONFIG_ARCH_APP_CORE */

#endif /* __CONFIG_ARCH_DUAL_CORE */

/*
 * Macro for type conversion: convert mbuf pointer to data pointer of correct
 * type:
 *
 * mtod(m, t)	-- Convert mbuf pointer to data pointer of correct type.
 */
#define mtod(m, t)          ((t)((m)->m_data))

#ifdef __cplusplus
}
#endif

#endif /* (__CONFIG_MBUF_IMPL_MODE == 0) */
#endif /* !_SYS_MBUF_0_H_ */
