/*
 * Generic hashmap manipulation functions
 *
 * Originally by Elliot C Back - http://elliottback.com/wp/hashmap-implementation-in-c/
 *
 * Modified by Pete Warden to fix a serious performance problem, support strings as keys
 * and removed thread synchronization - http://petewarden.typepad.com
 *
 * Modified by Milan Stute to support arbitrary key types.
 */
#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <stdint.h>

enum map_status {
	MAP_OK = 0, 	/* OK */
	MAP_OMEM = -1, 	/* Out of Memory */
	MAP_FULL = -2, 	/* Hashmap is full */
	MAP_MISSING = -3,  /* No such element */
};

/*
 * any_t is a pointer.  This allows you to put arbitrary structures in
 * the hashmap.
 */
typedef void *any_t;

/*
 * Key can be anything. But user has to provide byte length of type.
 */
typedef any_t mkey_t;

/*
 * map_t is a pointer to an internally maintained data structure.
 * Clients of this package do not need to know how hashmaps are
 * represented.  They see and manipulate only map_t's.
 */
typedef any_t map_t;

/*
 * Return an empty hashmap. Returns NULL if empty.
*/
extern map_t hashmap_new(int mkey_len);

/*
 * Add an element to the hashmap. Return MAP_OK or MAP_OMEM.
 */
extern enum map_status hashmap_put(map_t in, mkey_t key, any_t value);

/*
 * Get an element from the hashmap. Return MAP_OK or MAP_MISSING.
 * If remove is true, element will be removed if it exists.
 * arg will be set to the value if found, can be NULL if not required
 */
extern enum map_status hashmap_get(map_t in, mkey_t key, any_t *arg, int remove);

/*
 * Free the hashmap
 */
extern void hashmap_free(map_t in);

/*
 * Get the current size of a hashmap
 */
extern int hashmap_length(map_t in);

/**
 * Opaque iterator type
 */
typedef any_t map_it_t;

/**
 * Get a new iterator.
 * @param in the hashmap
 * @return a new iterator of hashmap
 */
extern map_it_t hashmap_it_new(map_t in);

/**
 * Get next element in hashmap. You should not modify the the map while iterating.
 * @param it hashmap iterator
 * @param key set to current element's key if return MAP_OK
 * @param value set to current element's value if return MAP_OK
 * @return MAP_OK if had next, MAP_MISSING otherwise
 */
extern enum map_status hashmap_it_next(map_it_t it, mkey_t *key, any_t *value);

/**
 * Removes current element from hashmap.
 * @param it hashmap iterator
 * @return MAP_OK if iterator points to valid element, MAP_MISSING otherwise
 */
extern enum map_status hashmap_it_remove(map_it_t it);

/**
 * Frees interator. {@code it} must not be used after calling this method.
 * @param it hashmap iterator
 */
extern void hashmap_it_free(map_it_t it);

#endif /* __HASHMAP_H__ */
