/*
 * Generic map implementation.
 */
#include "hashmap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "siphash24.h"

#define INITIAL_SIZE (256)
#define MAX_CHAIN_LENGTH (8)

/* We need to keep keys and values */
typedef struct _hashmap_element{
	mkey_t key;
	int in_use;
	any_t data;
} hashmap_element;

/* A hashmap has some maximum size and current size,
 * as well as the data to hold. */
typedef struct _hashmap_map{
	int table_size;
	int size;
	int key_size; /* size of user's mkey_t in bytes */
	hashmap_element *data;
	unsigned char seed[siphash24_KEYBYTES];
} hashmap_map;

/*
 * Return an empty hashmap, or NULL on failure.
 */
map_t hashmap_new(int key_size) {
	hashmap_map* m = (hashmap_map*) malloc(sizeof(hashmap_map));
	if(!m) goto err;

	m->data = (hashmap_element*) calloc(INITIAL_SIZE, sizeof(hashmap_element));
	if(!m->data) goto err;

	m->table_size = INITIAL_SIZE;
	m->size = 0;
	m->key_size = key_size;

	/* TODO init seed randomly */
	memset(m->seed, 0, siphash24_KEYBYTES);

	return m;
err:
	if (m)
		hashmap_free(m);
	return NULL;
}

/*
 * Hashing function for a string
 */
unsigned int hashmap_hash_int(hashmap_map *m, mkey_t key) {
	uint64_t hash;
	siphash24((unsigned char *) &hash, key, m->key_size, m->seed);
	return hash % m->table_size;
}

/*
 * Return the integer of the location in data
 * to store the point to the item, or MAP_FULL.
 */
int hashmap_hash(map_t in, mkey_t key){
	int curr;
	int i;

	/* Cast the hashmap */
	hashmap_map* m = (hashmap_map *) in;

	/* If full, return immediately */
	if(m->size >= (m->table_size/2)) return MAP_FULL;

	/* Find the best index */
	curr = hashmap_hash_int(m, key);

	/* Linear probing */
	for(i = 0; i< MAX_CHAIN_LENGTH; i++){
		if(m->data[curr].in_use == 0)
			return curr;

		if (m->data[curr].in_use == 1 && (memcmp(m->data[curr].key, key, m->key_size) == 0))
			return curr;

		curr = (curr + 1) % m->table_size;
	}

	return MAP_FULL;
}

/*
 * Doubles the size of the hashmap, and rehashes all the elements
 */
enum map_status hashmap_rehash(map_t in){
	int i;
	int old_size;
	hashmap_element* curr;

	/* Setup the new elements */
	hashmap_map *m = (hashmap_map *) in;
	hashmap_element* temp = (hashmap_element *)
		calloc(2 * m->table_size, sizeof(hashmap_element));
	if(!temp) return MAP_OMEM;

	/* Update the array */
	curr = m->data;
	m->data = temp;

	/* Update the size */
	old_size = m->table_size;
	m->table_size = 2 * m->table_size;
	m->size = 0;

	/* Rehash the elements */
	for(i = 0; i < old_size; i++){
		int status;

		if (curr[i].in_use == 0)
			continue;

		status = hashmap_put(m, curr[i].key, curr[i].data);
		if (status != MAP_OK)
			return status;
	}

	free(curr);

	return MAP_OK;
}

/*
 * Add a pointer to the hashmap with some key
 */
enum map_status hashmap_put(map_t in, mkey_t key, any_t value){
	int index;
	hashmap_map* m;

	/* Cast the hashmap */
	m = (hashmap_map *) in;

	/* Find a place to put our value */
	index = hashmap_hash(in, key);
	while(index == MAP_FULL){
		if (hashmap_rehash(in) == MAP_OMEM) {
			return MAP_OMEM;
		}
		index = hashmap_hash(in, key);
	}

	/* Set the data */
	m->data[index].data = value;
	m->data[index].key = key;
	m->data[index].in_use = 1;
	m->size++;

	return MAP_OK;
}

/*
 * Get your pointer out of the hashmap with a key
 */
enum map_status hashmap_get(map_t in, mkey_t key, any_t *arg, int remove) {
	int curr;
	int i;
	hashmap_map* m;

	/* Cast the hashmap */
	m = (hashmap_map *) in;

	/* Find data location */
	curr = hashmap_hash_int(m, key);

	/* Linear probing, if necessary */
	for(i = 0; i<MAX_CHAIN_LENGTH; i++){

		int in_use = m->data[curr].in_use;
		if (in_use == 1){
			if (memcmp(m->data[curr].key, key, m->key_size) == 0) {
				if (arg)
					*arg = (m->data[curr].data);

				if (remove) {
					/* Blank out the fields */
					m->data[curr].in_use = 0;
					m->data[curr].data = NULL;
					m->data[curr].key = NULL;

					/* Reduce the size */
					m->size--;
				}

				return MAP_OK;
			}
		}

		curr = (curr + 1) % m->table_size;
	}

	if (arg)
		*arg = NULL;

	/* Not found */
	return MAP_MISSING;
}

/* Deallocate the hashmap */
void hashmap_free(map_t in){
	hashmap_map* m = (hashmap_map*) in;
	free(m->data);
	free(m);
}

/* Return the length of the hashmap */
int hashmap_length(map_t in){
	hashmap_map* m = (hashmap_map *) in;
	if(m != NULL) return m->size;
	else return 0;
}

typedef struct _hashmap_it {
	hashmap_map *map;
	int i; /* current index */
} hashmap_it;

/**
 * Get a new iterator.
 * @param in the hashmap
 * @return a new iterator of hashmap
 */
map_it_t hashmap_it_new(map_t in) {
	hashmap_it *it = (hashmap_it *) malloc(sizeof(hashmap_it));
	it->map = (hashmap_map *) in;
	it->i = -1;
	return (map_it_t) it;
}

/**
 * Get next element in hashmap. You should not modify the the map while iterating.
 * @param it hashmap iterator
 * @param key set to current element's key if return MAP_OK
 * @param value set to current element's value if return MAP_OK
 * @return MAP_OK if had next, MAP_MISSING otherwise
 */
enum map_status hashmap_it_next(map_it_t _it, mkey_t *key, any_t *value) {
	hashmap_it *it = (hashmap_it *) _it;

	/* Linear probing */
	for (it->i++; it->i < it->map->table_size; it->i++)
		if (it->map->data[it->i].in_use != 0) {
			if (key)
				*key = it->map->data[it->i].key;
			if (value)
				*value = (any_t) (it->map->data[it->i].data);
			return MAP_OK;
		}

	return MAP_MISSING;
}

/**
 * Removes current element from hashmap.
 * @param it hashmap iterator
 * @return MAP_OK if iterator points to valid element, MAP_MISSING otherwise
 */
enum map_status hashmap_it_remove(map_it_t _it) {
	hashmap_it *it = (hashmap_it *) _it;

	if (it->i < 0 || it->i >= it->map->table_size)
		return MAP_MISSING;

	if (!it->map->data[it->i].in_use)
		/* could be that we are calling remove on an already removed element */
		return MAP_MISSING;

	it->map->data[it->i].in_use = 0;
	it->map->data[it->i].data = NULL;
	it->map->data[it->i].key = NULL;

	/* Reduce the size */
	it->map->size--;

	return MAP_OK;
}

/**
 * Frees interator. {@code it} must not be used after calling this method.
 * @param it hashmap iterator
 */
void hashmap_it_free(map_it_t _it) {
	hashmap_it *it = (hashmap_it *) _it;
	free(it);
}
