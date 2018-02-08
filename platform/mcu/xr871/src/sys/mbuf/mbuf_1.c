/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if (__CONFIG_MBUF_IMPL_MODE == 1)

#include "sys/mbuf_1.h"
#include "mbuf_util.h"

#if (defined(__CONFIG_ARCH_APP_CORE) || !defined(__CONFIG_ARCH_DUAL_CORE))

#include "lwip/pbuf.h"
#include "lwip/mem.h"
#include "lwip/memp.h"

/* Init mbuf data info from pbuf, no sanity checks */
static void mb_data_init(struct mbuf *m, struct pbuf *p)
{
	m->m_pbuf = p;
	m->m_data = p->payload;
	m->m_len = p->tot_len;
	m->m_pkthdr.len = p->tot_len;
	m->m_headspace = pbuf_head_space(p);
	if (p->mb_flags & PBUF_FLAG_MBUF_SPACE)
	    m->m_tailspace = MBUF_TAIL_SPACE;
	p->mb_flags |= PBUF_FLAG_MBUF_REF;
}

/*
 * Alloc a new mbuf without pbuf, zero the header.
 */
static struct mbuf *mb_alloc()
{
	struct mbuf *m = memp_malloc(MEMP_MBUF);
	if (m) {
		MB_MEMSET(m, 0, sizeof(struct mbuf));
		m->m_flags = M_PKTHDR;
	} else {
		MBUF_WRN("Out of memory in pbuf pool for mbuf\n");
	}
	return m;
}

/*
 * @param tx
 *   - 1 means mbuf is used to do Tx, always alloc it from PBUF_RAM.
 *   - 0 means mbuf is used to do RX, try to alloc it from PBUF_POOL first
 * @return a mbuf including @len data
 */
struct mbuf *mb_get(int len, int tx)
{
	if (len < 0)
		return NULL;

	if (len > PBUF_POOL_BUFSIZE) {
		MBUF_WRN("try to get large data, len %d\n", len);
	}

	struct mbuf *m = mb_alloc();
	if (m == NULL) {
		return NULL;
	}

	struct pbuf *p;
#if LWIP_PBUF_POOL_SMALL
	int pbuf_pool_small = 0;
#endif
	int tot_len = len;
	pbuf_type type = (tx || len > PBUF_POOL_BUFSIZE) ? PBUF_RAM : PBUF_POOL;
	if (type == PBUF_RAM) {
		/* space is not reserved after pbuf_alloc(), add more space */
		tot_len += MBUF_HEAD_SPACE + MBUF_TAIL_SPACE;
	}
#if LWIP_PBUF_POOL_SMALL
	else if (len <= PBUF_POOL_SMALL_BUFSIZE) {
		pbuf_pool_small = 1;
	}

retry:
	p = pbuf_alloc_ext(PBUF_MBUF_RAW, tot_len, type, pbuf_pool_small);
	if (p == NULL) {
		MBUF_DBG("pbuf_alloc_ext() failed, tot_len %d, type %d, small %d\n",
		         tot_len, type, pbuf_pool_small);
		if (pbuf_pool_small) {
			/* try to get pbuf from bigger pbuf pools */
			pbuf_pool_small = 0;
			goto retry;
		}
		mb_free(m);
		return NULL;
	}
#else /* LWIP_PBUF_POOL_SMALL */
	p = pbuf_alloc(PBUF_MBUF_RAW, tot_len, type);
	if (p == NULL) {
		MBUF_DBG("pbuf_alloc() failed, tot_len %d, type %d\n", tot_len, type);
		mb_free(m);
		return NULL;
	}
#endif /* LWIP_PBUF_POOL_SMALL */
	if (type == PBUF_RAM) {
		/* reserved head and tail space of pbuf */
		pbuf_header(p, -MBUF_HEAD_SPACE);
		p->len -= MBUF_TAIL_SPACE;
		p->tot_len -= MBUF_TAIL_SPACE;
		p->mb_flags |= PBUF_FLAG_MBUF_SPACE;
	}

	mb_data_init(m, p);
	return m;
}

/*
 * Free a mbuf.
 */
void mb_free(struct mbuf *m)
{
	if (m == NULL) {
		MBUF_WRN("mb_free(), m is NULL\n");
		return;
	}

	struct pbuf *p = m->m_pbuf;
	if (p) {
		p->mb_flags &= ~PBUF_FLAG_MBUF_REF;
		pbuf_free(p);
	}
	memp_free(MEMP_MBUF, m);
}

/*
 * Create a new mbuf including all pbuf data.
 */
struct mbuf *mb_pbuf2mbuf(void *p)
{
	struct pbuf *pb = p;
	struct mbuf *m;

	if ((pbuf_clen(pb) > 1) || ((pb->mb_flags & PBUF_FLAG_MBUF_SPACE) == 0)) {
		/* copy all data from @pb to a new single pbuf */
		m = mb_get(pb->tot_len, 1);
		if (m == NULL)
			return NULL;

		if (pbuf_copy_partial(pb, m->m_data, pb->tot_len, 0) != pb->tot_len) {
			mb_free(m);
			return NULL;
		}
	} else {
		/* no need to copy data, link @pb to a new mbuf header */
		m = mb_alloc();
		if (m == NULL)
			return NULL;

		pbuf_ref(pb); /* @pb is referenced by @m now */
		mb_data_init(m, pb);
	}
	return m;
}

/*
 * Return a pbuf included in a mbuf
 */
void *mb_mbuf2pbuf(struct mbuf *m)
{
	struct pbuf *p = m->m_pbuf;

	p->payload = m->m_data;
	p->tot_len = m->m_len;
	p->len = m->m_len;
	if (m->m_tailspace >= MBUF_TAIL_SPACE)
		p->mb_flags |= PBUF_FLAG_MBUF_SPACE;
	pbuf_ref(p); /* add reference to avoid freed from @m */
	return p;
}

#endif /* (defined(__CONFIG_ARCH_APP_CORE) || !defined(__CONFIG_ARCH_DUAL_CORE)) */
#endif /* (__CONFIG_MBUF_IMPL_MODE == 1) */
