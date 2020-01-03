/**
 * @file mem.c  Memory management with reference counting
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_PTHREAD
#include <posix/pthread.h>
#endif
#include <re_types.h>
#include <re_list.h>
#include <re_fmt.h>
#include <re_mbuf.h>
#include <re_mem.h>


#define DEBUG_MODULE "mem"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


#ifndef RELEASE
#define MEM_DEBUG 1  /**< Enable memory debugging */
#endif


/** Defines a reference-counting memory object */
struct mem {
	uint32_t nrefs;     /**< Number of references  */
	mem_destroy_h *dh;  /**< Destroy handler       */
#if MEM_DEBUG
	struct le le;       /**< Linked list element   */
	uint32_t magic;     /**< Magic number          */
	size_t size;        /**< Size of memory object */
#endif
};

#if MEM_DEBUG
/* Memory debugging */
static struct list meml = LIST_INIT;
static const uint32_t mem_magic = 0xe7fb9ac4;
static ssize_t threshold = -1;  /**< Memory threshold, disabled by default */

static struct memstat memstat = {
	0,0,0,0,~0,0
};

#ifdef HAVE_PTHREAD

static pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;

static inline void mem_lock(void)
{
	pthread_mutex_lock(&mem_mutex);
}


static inline void mem_unlock(void)
{
	pthread_mutex_unlock(&mem_mutex);
}

#else

#define mem_lock()    /**< Stub */
#define mem_unlock()  /**< Stub */

#endif

/** Update statistics for mem_zalloc() */
#define STAT_ALLOC(m, size) \
	mem_lock(); \
	memstat.bytes_cur += (size); \
	memstat.bytes_peak = max(memstat.bytes_cur, memstat.bytes_peak); \
	++memstat.blocks_cur; \
	memstat.blocks_peak = max(memstat.blocks_cur, memstat.blocks_peak); \
	memstat.size_min = min(memstat.size_min, size);	\
	memstat.size_max = max(memstat.size_max, size);	\
	mem_unlock(); \
	(m)->size = (size); \
	(m)->magic = mem_magic;

/** Update statistics for mem_realloc() */
#define STAT_REALLOC(m, size) \
	mem_lock(); \
	memstat.bytes_cur += ((size) - (m)->size); \
	memstat.bytes_peak = max(memstat.bytes_cur, memstat.bytes_peak); \
	memstat.size_min = min(memstat.size_min, size);	\
	memstat.size_max = max(memstat.size_max, size); \
	mem_unlock(); \
	(m)->size = (size)

/** Update statistics for mem_deref() */
#define STAT_DEREF(m) \
	mem_lock(); \
	memstat.bytes_cur -= (m)->size; \
	--memstat.blocks_cur; \
	mem_unlock(); \
	memset((m), 0xb5, sizeof(struct mem) + (m)->size)

/** Check magic number in memory object */
#define MAGIC_CHECK(m) \
	if (mem_magic != (m)->magic) { \
		DEBUG_WARNING("%s: magic check failed 0x%08x (%p)\n", \
			      __REFUNC__, (m)->magic, (m)+1);	      \
	}
#else
#define STAT_ALLOC(m, size)
#define STAT_REALLOC(m, size)
#define STAT_DEREF(m)
#define MAGIC_CHECK(m)
#endif


/**
 * Allocate a new reference-counted memory object
 *
 * @param size Size of memory object
 * @param dh   Optional destructor, called when destroyed
 *
 * @return Pointer to allocated object
 */
void *mem_alloc(size_t size, mem_destroy_h *dh)
{
	struct mem *m;

#if MEM_DEBUG
	mem_lock();
	if (-1 != threshold && (memstat.blocks_cur >= (size_t)threshold)) {
		mem_unlock();
		return NULL;
	}
	mem_unlock();
#endif

	m = malloc(sizeof(*m) + size);
	if (!m)
		return NULL;

#if MEM_DEBUG
	memset(&m->le, 0, sizeof(struct le));
	mem_lock();
	list_append(&meml, &m->le, m);
	mem_unlock();
#endif

	m->nrefs = 1;
	m->dh    = dh;

	STAT_ALLOC(m, size);

	return (void *)(m + 1);
}


/**
 * Allocate a new reference-counted memory object. Memory is zeroed.
 *
 * @param size Size of memory object
 * @param dh   Optional destructor, called when destroyed
 *
 * @return Pointer to allocated object
 */
void *mem_zalloc(size_t size, mem_destroy_h *dh)
{
	void *p;

	p = mem_alloc(size, dh);
	if (!p)
		return NULL;

	memset(p, 0, size);

	return p;
}


/**
 * Re-allocate a reference-counted memory object
 *
 * @param data Memory object
 * @param size New size of memory object
 *
 * @return New pointer to allocated object
 *
 * @note Realloc NULL pointer is not supported
 */
void *mem_realloc(void *data, size_t size)
{
	struct mem *m, *m2;

	if (!data)
		return NULL;

	m = ((struct mem *)data) - 1;

	MAGIC_CHECK(m);

#if MEM_DEBUG
	mem_lock();

	/* Simulate OOM */
	if (-1 != threshold && size > m->size) {
		if (memstat.blocks_cur >= (size_t)threshold) {
			mem_unlock();
			return NULL;
		}
	}

	list_unlink(&m->le);
	mem_unlock();
#endif

	m2 = realloc(m, sizeof(*m2) + size);

#if MEM_DEBUG
	mem_lock();
	list_append(&meml, m2 ? &m2->le : &m->le, m2 ? m2 : m);
	mem_unlock();
#endif

	if (!m2) {
		return NULL;
	}

	STAT_REALLOC(m2, size);

	return (void *)(m2 + 1);
}


#ifndef SIZE_MAX
#define SIZE_MAX    (~((size_t)0))
#endif


/**
 * Re-allocate a reference-counted array
 *
 * @param ptr      Pointer to existing array, NULL to allocate a new array
 * @param nmemb    Number of members in array
 * @param membsize Number of bytes in each member
 * @param dh       Optional destructor, only used when ptr is NULL
 *
 * @return New pointer to allocated array
 */
void *mem_reallocarray(void *ptr, size_t nmemb, size_t membsize,
		       mem_destroy_h *dh)
{
	size_t tsize;

	if (membsize && nmemb > SIZE_MAX / membsize) {
		return NULL;
	}

	tsize = nmemb * membsize;

	if (ptr) {
		return mem_realloc(ptr, tsize);
	}
	else {
		return mem_alloc(tsize, dh);
	}
}


/**
 * Reference a reference-counted memory object
 *
 * @param data Memory object
 *
 * @return Memory object (same as data)
 */
void *mem_ref(void *data)
{
	struct mem *m;

	if (!data)
		return NULL;

	m = ((struct mem *)data) - 1;

	MAGIC_CHECK(m);

	++m->nrefs;

	return data;
}


/**
 * Dereference a reference-counted memory object. When the reference count
 * is zero, the destroy handler will be called (if present) and the memory
 * will be freed
 *
 * @param data Memory object
 *
 * @return Always NULL
 */
void *mem_deref(void *data)
{
	struct mem *m;

	if (!data)
		return NULL;

	m = ((struct mem *)data) - 1;

	MAGIC_CHECK(m);

	if (--m->nrefs > 0)
		return NULL;

	if (m->dh)
		m->dh(data);

	/* NOTE: check if the destructor called mem_ref() */
	if (m->nrefs > 0)
		return NULL;

#if MEM_DEBUG
	mem_lock();
	list_unlink(&m->le);
	mem_unlock();
#endif

	STAT_DEREF(m);

	free(m);

	return NULL;
}


/**
 * Get number of references to a reference-counted memory object
 *
 * @param data Memory object
 *
 * @return Number of references
 */
uint32_t mem_nrefs(const void *data)
{
	struct mem *m;

	if (!data)
		return 0;

	m = ((struct mem *)data) - 1;

	MAGIC_CHECK(m);

	return m->nrefs;
}


#if MEM_DEBUG
static bool debug_handler(struct le *le, void *arg)
{
	struct mem *m = le->data;
	const uint8_t *p = (const uint8_t *)(m + 1);
	size_t i;

	(void)arg;

	(void)re_fprintf(stderr, "  %p: nrefs=%-2u", p, m->nrefs);

	(void)re_fprintf(stderr, " size=%-7u", m->size);

	(void)re_fprintf(stderr, " [");

	for (i=0; i<16; i++) {
		if (i >= m->size)
			(void)re_fprintf(stderr, "   ");
		else
			(void)re_fprintf(stderr, "%02x ", p[i]);
	}

	(void)re_fprintf(stderr, "] [");

	for (i=0; i<16; i++) {
		if (i >= m->size)
			(void)re_fprintf(stderr, " ");
		else
			(void)re_fprintf(stderr, "%c",
					 isprint(p[i]) ? p[i] : '.');
	}

	(void)re_fprintf(stderr, "]");

	MAGIC_CHECK(m);

	(void)re_fprintf(stderr, "\n");

	return false;
}
#endif


/**
 * Debug all allocated memory objects
 */
void mem_debug(void)
{
#if MEM_DEBUG
	uint32_t n;

	mem_lock();
	n = list_count(&meml);
	mem_unlock();

	if (!n)
		return;

	DEBUG_WARNING("Memory leaks (%u):\n", n);

	mem_lock();
	(void)list_apply(&meml, true, debug_handler, NULL);
	mem_unlock();
#endif
}


/**
 * Set the memory allocation threshold. This is only used for debugging
 * and out-of-memory simulation
 *
 * @param n Threshold value
 */
void mem_threshold_set(ssize_t n)
{
#if MEM_DEBUG
	mem_lock();
	threshold = n;
	mem_unlock();
#else
	(void)n;
#endif
}


/**
 * Print memory status
 *
 * @param pf     Print handler for debug output
 * @param unused Unused parameter
 *
 * @return 0 if success, otherwise errorcode
 */
int mem_status(struct re_printf *pf, void *unused)
{
#if MEM_DEBUG
	struct memstat stat;
	uint32_t c;
	int err = 0;

	(void)unused;

	mem_lock();
	memcpy(&stat, &memstat, sizeof(stat));
	c = list_count(&meml);
	mem_unlock();

	err |= re_hprintf(pf, "Memory status: (%u bytes overhead pr block)\n",
			  sizeof(struct mem));
	err |= re_hprintf(pf, " Cur:  %u blocks, %u bytes (total %u bytes)\n",
			  stat.blocks_cur, stat.bytes_cur,
			  stat.bytes_cur
			  +(stat.blocks_cur*sizeof(struct mem)));
	err |= re_hprintf(pf, " Peak: %u blocks, %u bytes (total %u bytes)\n",
			  stat.blocks_peak, stat.bytes_peak,
			  stat.bytes_peak
			  +(stat.blocks_peak*sizeof(struct mem)));
	err |= re_hprintf(pf, " Block size: min=%u, max=%u\n",
			  stat.size_min, stat.size_max);
	err |= re_hprintf(pf, " Total %u blocks allocated\n", c);

	return err;
#else
	(void)pf;
	(void)unused;
	return 0;
#endif
}


/**
 * Get memory statistics
 *
 * @param mstat Returned memory statistics
 *
 * @return 0 if success, otherwise errorcode
 */
int mem_get_stat(struct memstat *mstat)
{
	if (!mstat)
		return EINVAL;
#if MEM_DEBUG
	mem_lock();
	memcpy(mstat, &memstat, sizeof(*mstat));
	mem_unlock();
	return 0;
#else
	return ENOSYS;
#endif
}
