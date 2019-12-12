/**
 * @file re_hash.h  Interface to hashmap table
 *
 * Copyright (C) 2010 Creytiv.com
 */


struct hash;
struct pl;


int  hash_alloc(struct hash **hp, uint32_t bsize);
void hash_append(struct hash *h, uint32_t key, struct le *le, void *data);
void hash_unlink(struct le *le);
struct le *hash_lookup(const struct hash *h, uint32_t key, list_apply_h *ah,
		       void *arg);
struct le *hash_apply(const struct hash *h, list_apply_h *ah, void *arg);
struct list *hash_list(const struct hash *h, uint32_t key);
uint32_t hash_bsize(const struct hash *h);
void hash_flush(struct hash *h);
void hash_clear(struct hash *h);
uint32_t hash_valid_size(uint32_t size);


/* Hash functions */
uint32_t hash_joaat(const uint8_t *key, size_t len);
uint32_t hash_joaat_ci(const char *str, size_t len);
uint32_t hash_joaat_str(const char *str);
uint32_t hash_joaat_str_ci(const char *str);
uint32_t hash_joaat_pl(const struct pl *pl);
uint32_t hash_joaat_pl_ci(const struct pl *pl);
uint32_t hash_fast(const char *k, size_t len);
uint32_t hash_fast_str(const char *str);
