#ifndef _PSRAM_RESERVE_H_
#define _PSRAM_RESERVE_H_

//#include <stdio.h>
#include <stdint.h>
#include <osdep_service.h>

/* NOTE: struct size must be a 2's power! */
typedef struct _PsramMemChunk
{
	struct _PsramMemChunk *next;
	int size;
} PsramMemChunk;

typedef PsramMemChunk Psram_heap_buf_t;

// A heap
typedef struct Psram_Heap
{
	struct _PsramMemChunk *FreeList;	///< Head of the free list
} Psram_Heap;

 
/**
 * Utility macro to allocate a heap of size \a size.
 *
 * \param name Variable name for the heap.
 * \param size Heap size in bytes.
 */
#define PSRAM_HEAP_DEFINE_BUF(name, size) \
	Psram_heap_buf_t name[((size) + sizeof(Psram_heap_buf_t) - 1) / sizeof(Psram_heap_buf_t)]


#define Psram_ROUND_UP2(x, pad) (((x) + ((pad) - 1)) & ~((pad) - 1))

/// Initialize \a heap within the buffer pointed by \a memory which is of \a size bytes
void Psram_heap_init(void);

/// Allocate a chunk of memory of \a size bytes from the heap
void *Psram_heap_allocmem(int size);

/// Free a chunk of memory of \a size bytes from the heap
void Psram_reserved_heap_freemem(void *mem, int size);

int Psram_reserve_free_size(void);

/**
 * \name Compatibility interface with C standard library
 * \{
 */
void *Psram_reserve_malloc(int size);
void *Psram_reserve_calloc(int num, int size);
void Psram_reserve_free(void *mem);
/** \} */


#endif /* _PSRAM_RESERVE_H_ */
