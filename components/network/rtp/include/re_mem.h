/**
 * @file re_mem.h  Interface to Memory management with reference counting
 *
 * Copyright (C) 2010 Creytiv.com
 */


#include <stdint.h>
/**
 * Defines the memory destructor handler, which is called when the reference
 * of a memory object goes down to zero
 *
 * @param data Pointer to memory object
 */
typedef void (mem_destroy_h)(void *data);

/** Memory Statistics */
struct memstat {
	size_t bytes_cur;    /**< Current bytes allocated      */
	size_t bytes_peak;   /**< Peak bytes allocated         */
	size_t blocks_cur;   /**< Current blocks allocated     */
	size_t blocks_peak;  /**< Peak blocks allocated        */
	size_t size_min;     /**< Lowest block size allocated  */
	size_t size_max;     /**< Largest block size allocated */
};

void    *mem_alloc(size_t size, mem_destroy_h *dh);
void    *mem_zalloc(size_t size, mem_destroy_h *dh);
void    *mem_realloc(void *data, size_t size);
void    *mem_reallocarray(void *ptr, size_t nmemb,
			  size_t membsize, mem_destroy_h *dh);
void    *mem_ref(void *data);
void    *mem_deref(void *data);
uint32_t mem_nrefs(const void *data);

void     mem_debug(void);
void     mem_threshold_set(ssize_t n);
struct re_printf;
int      mem_status(struct re_printf *pf, void *unused);
int      mem_get_stat(struct memstat *mstat);


/* Secure memory functions */
int  mem_seccmp(const volatile uint8_t *volatile s1,
		const volatile uint8_t *volatile s2,
		size_t n);
