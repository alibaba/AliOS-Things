/**
 * @file hash.c  Hashmap table
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re_types.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_list.h>
#include <re_hash.h>


/** Defines a hashmap table */
struct hash {
	struct list *bucket;  /**< Bucket with linked lists */
	uint32_t bsize;       /**< Bucket size              */
};


static void hash_destructor(void *data)
{
	struct hash *h = data;

	mem_deref(h->bucket);
}


/**
 * Allocate a new hashmap table
 *
 * @param hp     Address of hashmap pointer
 * @param bsize  Bucket size
 *
 * @return 0 if success, otherwise errorcode
 */
int hash_alloc(struct hash **hp, uint32_t bsize)
{
	struct hash *h;
	int err = 0;

	if (!hp || !bsize)
		return EINVAL;

	/* Validate bucket size */
	if (bsize & (bsize-1))
		return EINVAL;

	h = mem_zalloc(sizeof(*h), hash_destructor);
	if (!h)
		return ENOMEM;

	h->bsize = bsize;

	h->bucket = mem_zalloc(bsize*sizeof(*h->bucket), NULL);
	if (!h->bucket) {
		err = ENOMEM;
		goto out;
	}

 out:
	if (err)
		mem_deref(h);
	else
		*hp = h;

	return err;
}


/**
 * Add an element to the hashmap table
 *
 * @param h      Hashmap table
 * @param key    Hash key
 * @param le     List element
 * @param data   Element data
 */
void hash_append(struct hash *h, uint32_t key, struct le *le, void *data)
{
	if (!h || !le)
		return;

	list_append(&h->bucket[key & (h->bsize-1)], le, data);
}


/**
 * Unlink an element from the hashmap table
 *
 * @param le     List element
 */
void hash_unlink(struct le *le)
{
	list_unlink(le);
}


/**
 * Apply a handler function to all elements in the hashmap with a matching key
 *
 * @param h   Hashmap table
 * @param key Hash key
 * @param ah  Apply handler
 * @param arg Handler argument
 *
 * @return List element if traversing stopped, otherwise NULL
 */
struct le *hash_lookup(const struct hash *h, uint32_t key, list_apply_h *ah,
		       void *arg)
{
	if (!h || !ah)
		return NULL;

	return list_apply(&h->bucket[key & (h->bsize-1)], true, ah, arg);
}


/**
 * Apply a handler function to all elements in the hashmap
 *
 * @param h   Hashmap table
 * @param ah  Apply handler
 * @param arg Handler argument
 *
 * @return List element if traversing stopped, otherwise NULL
 */
struct le *hash_apply(const struct hash *h, list_apply_h *ah, void *arg)
{
	struct le *le = NULL;
	uint32_t i;

	if (!h || !ah)
		return NULL;

	for (i=0; (i<h->bsize) && !le; i++)
		le = list_apply(&h->bucket[i], true, ah, arg);

	return le;
}


/**
 * Return bucket list for a given index
 *
 * @param h   Hashmap table
 * @param key Hash key
 *
 * @return Bucket list if valid input, otherwise NULL
 */
struct list *hash_list(const struct hash *h, uint32_t key)
{
	return h ? &h->bucket[key & (h->bsize - 1)] : NULL;
}


/**
 * Get hash bucket size
 *
 * @param h Hashmap table
 *
 * @return hash bucket size
 */
uint32_t hash_bsize(const struct hash *h)
{
	return h ? h->bsize : 0;
}


/**
 * Flush a hashmap and free all elements
 *
 * @param h Hashmap table
 */
void hash_flush(struct hash *h)
{
	uint32_t i;

	if (!h)
		return;

	for (i=0; i<h->bsize; i++)
		list_flush(&h->bucket[i]);
}


/**
 * Clear a hashmap without dereferencing the elements
 *
 * @param h Hashmap table
 */
void hash_clear(struct hash *h)
{
	uint32_t i;

	if (!h)
		return;

	for (i=0; i<h->bsize; i++)
		list_clear(&h->bucket[i]);
}


/**
 * Calculate a valid hash size from a random size
 *
 * @param size Requested size
 *
 * @return Valid hash size
 */
uint32_t hash_valid_size(uint32_t size)
{
	uint32_t x;

	for (x=0; (uint32_t)1<<x < size && x < 31; x++)
		;

	return 1<<x;
}
