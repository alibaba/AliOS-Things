#ifndef UMM_MALLOC_H
#define UMM_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char umm_heap_array[];

#ifndef size_t
typedef unsigned int                size_t;
#endif

/* Start addresses and the size of the heap */
#define UMM_MALLOC_CFG_HEAP_ADDR (umm_heap_array)
#if defined(__ALEXA_WWE) || defined(__KWS_ALEXA__)
#define UMM_MALLOC_CFG_HEAP_SIZE 0x1000 // 4K
#else
#define UMM_MALLOC_CFG_HEAP_SIZE 0x3000 // 12K
#endif

/* A couple of macros to make packing structures less compiler dependent */

#define UMM_H_ATTPACKPRE
#define UMM_H_ATTPACKSUF __attribute__((__packed__))

#define UMM_BEST_FIT
#undef  UMM_FIRST_FIT

/*
 * A couple of macros to make it easier to protect the memory allocator
 * in a multitasking system. You should set these macros up to use whatever
 * your system uses for this purpose. You can disable interrupts entirely, or
 * just disable task switching - it's up to you
 *
 * NOTE WELL that these macros MUST be allowed to nest, because umm_free() is
 * called from within umm_malloc()
 */

#define UMM_CRITICAL_ENTRY()    LOCK_UMM_CLOCK()
#define UMM_CRITICAL_EXIT()     UNLOCK_UMM_CLOCK()

/* ------------------------------------------------------------------------ */

void  umm_init( void );
void *umm_malloc( size_t size );
void *umm_calloc( size_t num, size_t size );
void *umm_realloc( void *ptr, size_t size );
void  umm_free( void *ptr );


/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

#endif /* UMM_MALLOC_H */
