/**
 * @file member.c  Real-time Transport Control Protocol member
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <string.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_list.h>
#include <re_hash.h>
#include <re_sa.h>
#include <re_rtp.h>
#include "rtcp.h"


static void destructor(void *data)
{
	struct rtp_member *mbr = data;

	hash_unlink(&mbr->le);
	mem_deref(mbr->s);
}


struct rtp_member *member_add(struct hash *ht, uint32_t src)
{
	struct rtp_member *mbr;

	mbr = mem_zalloc(sizeof(*mbr), destructor);
	if (!mbr)
		return NULL;

	hash_append(ht, src, &mbr->le, mbr);
	mbr->src = src;

	return mbr;
}


static bool hash_cmp_handler(struct le *le, void *arg)
{
	const struct rtp_member *mbr = le->data;

	return mbr->src == *(uint32_t *)arg;
}


struct rtp_member *member_find(struct hash *ht, uint32_t src)
{
	return list_ledata(hash_lookup(ht, src, hash_cmp_handler, &src));
}
