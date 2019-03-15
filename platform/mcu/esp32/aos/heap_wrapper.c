#include "esp_attr.h"
#include "k_api.h"
#include "sdkconfig.h"

#if (CONFIG_SPIRAM_SUPPORT > 0)
#include <xtensa/config/core-isa.h>
#include <stdarg.h>


#define MALLOC_CAP_SPIRAM           (1<<10) ///< Memory must be in SPI RAM
#define MALLOC_CAP_INTERNAL         (1<<11) ///< Memory must be internal; specifically it should not disappear when flash/spiram cache is switched off
#define MALLOC_CAP_DEFAULT          (1<<12) ///< Memory can be returned in a non-capability-specific memory allocation (e.g. malloc(), calloc()) call
#define MALLOC_CAP_INVALID          (1<<31) ///< Memory can't be used / list end marker

extern void *ORAM_heap_malloc(size_t alloc_size);
extern void ORAM_heap_free(void *pfree);
extern size_t ORAM_heap_free_size(void);

#define MALLOC_DISABLE_EXTERNAL_ALLOCS -1
//Dual-use: -1 (=MALLOC_DISABLE_EXTERNAL_ALLOCS) disables allocations in external memory, >=0 sets the limit for allocations preferring internal memory.
static int malloc_alwaysinternal_limit=MALLOC_DISABLE_EXTERNAL_ALLOCS;

IRAM_ATTR void *heap_caps_calloc( size_t n, size_t size, uint32_t caps)
{
    (void)caps;
    void * addr = NULL;
    size_t size_bytes;

    if (__builtin_mul_overflow(n, size, &size_bytes)) {
        return NULL;
    }

    if (malloc_alwaysinternal_limit!=MALLOC_DISABLE_EXTERNAL_ALLOCS && (caps & MALLOC_CAP_SPIRAM)){
        addr =  ORAM_heap_malloc(size_bytes);
    }else{
        addr = krhino_mm_alloc(size_bytes);
    }

    if (addr) {
        bzero(addr, size_bytes);
    }

    return addr;

}


IRAM_ATTR void *heap_caps_malloc(size_t size, uint32_t caps)
{
    (void)caps;

    if (malloc_alwaysinternal_limit!=MALLOC_DISABLE_EXTERNAL_ALLOCS && (caps & MALLOC_CAP_SPIRAM)){
        return ORAM_heap_malloc(size);
    }

    return krhino_mm_alloc(size);
}

IRAM_ATTR void heap_caps_free( void *ptr)
{
    __intptr_t addr = (__intptr_t)ptr;
    if(addr >= XCHAL_DATARAM1_VADDR && addr < (XCHAL_DATARAM1_VADDR + XCHAL_DATARAM1_SIZE) ){
        ORAM_heap_free(ptr);

    }else{
        krhino_mm_free(ptr);
    }
}


IRAM_ATTR void *heap_caps_calloc_prefer( size_t n, size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    while (num--) {
        uint32_t caps = va_arg( argp, uint32_t );
        r = heap_caps_calloc( n, size, caps );
        if (r != NULL) break;
    }
    va_end( argp );
    return r;
}


IRAM_ATTR uint32_t heap_get_mm_blk_len( void *ptr)
{
    k_mm_list_t *free_b;

    free_b = MM_GET_THIS_BLK(ptr);

    return MM_GET_BUF_SIZE(free_b);
}


extern uint32_t ORAM_heap_getblk_size(void *pfree);

IRAM_ATTR uint32_t heap_get_caps_blk_len( void *ptr)
{
    return ORAM_heap_getblk_size(ptr);
}



typedef unsigned char BOOL;

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

IRAM_ATTR void *heap_caps_realloc( void *ptr, size_t size, int caps)
{
    BOOL  old_is_mm = TRUE;
    BOOL  new_is_mm = TRUE;
    int   old_size;
    void *new_p = NULL;
    __intptr_t addr = (__intptr_t)ptr;
    if (malloc_alwaysinternal_limit==MALLOC_DISABLE_EXTERNAL_ALLOCS)
    {
        if(caps & MALLOC_CAP_SPIRAM){
            return NULL;
    }

    return krhino_mm_realloc(ptr, size);
    }else{
        if(addr >= XCHAL_DATARAM1_VADDR && addr < (XCHAL_DATARAM1_VADDR + XCHAL_DATARAM1_SIZE) ){
            old_is_mm = FALSE;
            old_size = heap_get_caps_blk_len(ptr);
        }else{
            old_is_mm = TRUE;
            old_size = heap_get_mm_blk_len(ptr);
        }

        if(caps & MALLOC_CAP_SPIRAM){
            new_is_mm = FALSE;
        }

        if(old_is_mm == TRUE && new_is_mm == TRUE){
            return krhino_mm_realloc(ptr, size);
        }else if(old_is_mm == TRUE && new_is_mm == FALSE){
            new_p = ORAM_heap_malloc(size);
            if(new_p != NULL){
                memcpy(new_p, ptr, MIN(size, old_size));
                krhino_mm_free(ptr);
            }
            }else if(old_is_mm == FALSE && new_is_mm == FALSE){
                new_p = ORAM_heap_malloc(size);
            if(new_p != NULL){
                memcpy(new_p, ptr, MIN(size, old_size));
                ORAM_heap_free(ptr);
            }
            }else{
                new_p = krhino_mm_alloc(size);
            if(new_p != NULL){
                memcpy(new_p, ptr, MIN(size, old_size));
                ORAM_heap_free(ptr);
            }
        }

        return new_p;

    }




}


void heap_caps_enable_nonos_stack_heaps()
{
}

int heap_caps_get_free_size(void)
{
    int freesize;
    freesize = ORAM_heap_free_size();
    extern k_mm_head    *g_kmm_head;
    freesize += g_kmm_head->free_size;

    return freesize;
}



void heap_caps_malloc_extmem_enable(size_t limit)
{
    malloc_alwaysinternal_limit=limit;
}

/*
 Default memory allocation implementation. Should return standard 8-bit memory. malloc() essentially resolves to this function.
*/
IRAM_ATTR void *heap_caps_malloc_default( size_t size )
{
    if (malloc_alwaysinternal_limit==MALLOC_DISABLE_EXTERNAL_ALLOCS) {
        return heap_caps_malloc( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    } else {
        void *r;
        if (size <= malloc_alwaysinternal_limit) {
            r=heap_caps_malloc( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
        } else {
            r=heap_caps_malloc( size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM );
        }
        if (r==NULL) {
            //try again while being less picky
            r=heap_caps_malloc( size, MALLOC_CAP_DEFAULT );
        }
        return r;
    }
}



/*
 Memory allocation as preference in decreasing order.
 */
IRAM_ATTR void *heap_caps_malloc_prefer( size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    while (num--) {
        uint32_t caps = va_arg( argp, uint32_t );
        r = heap_caps_malloc( size, caps );
        if (r != NULL) {
            break;
        }
    }
    va_end( argp );
    return r;
}



/*
 Same for realloc()
 Note: keep the logic in here the same as in heap_caps_malloc_default (or merge the two as soon as this gets more complex...)
 */
IRAM_ATTR void *heap_caps_realloc_default( void *ptr, size_t size )
{
    if (malloc_alwaysinternal_limit==MALLOC_DISABLE_EXTERNAL_ALLOCS) {
        return heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
    } else {
        void *r;
        if (size <= malloc_alwaysinternal_limit) {
            r=heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL );
        } else {
            r=heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM );
        }
        if (r==NULL && size>0) {
            //We needed to allocate memory, but we didn't. Try again while being less picky.
            r=heap_caps_realloc( ptr, size, MALLOC_CAP_DEFAULT );
        }
        return r;
    }
}



/*
 Memory reallocation as preference in decreasing order.
 */
IRAM_ATTR void *heap_caps_realloc_prefer( void *ptr, size_t size, size_t num, ... )
{
    va_list argp;
    va_start( argp, num );
    void *r = NULL;
    while (num--) {
        uint32_t caps = va_arg( argp, uint32_t );
        r = heap_caps_realloc( ptr, size, caps );
        if (r != NULL || size == 0) {
            break;
        }
    }
    va_end( argp );
    return r;
}



#else
IRAM_ATTR void *heap_caps_malloc(size_t size, uint32_t caps)
{
    (void)caps;
    return krhino_mm_alloc(size);
}

/*
 Default memory allocation implementation. Should return standard 8-bit memory. malloc() essentially resolves to this function.
*/
IRAM_ATTR void *heap_caps_malloc_default( size_t size )
{
    return krhino_mm_alloc(size);
}


IRAM_ATTR void heap_caps_free( void *ptr)
{
    krhino_mm_free(ptr);
}

IRAM_ATTR void *heap_caps_realloc( void *ptr, size_t size, int caps)
{
    return krhino_mm_realloc(ptr, size);
}

IRAM_ATTR void *heap_caps_calloc( size_t n, size_t size, uint32_t caps)
{
    (void)caps;
    void * addr = NULL;
    size_t size_bytes;

    if (__builtin_mul_overflow(n, size, &size_bytes)) {
        return NULL;
    }

    addr = krhino_mm_alloc(size_bytes);

    if (addr) {
        bzero(addr, size_bytes);
    }

    return addr;

}

/*
 Same for realloc()
 Note: keep the logic in here the same as in heap_caps_malloc_default (or merge the two as soon as this gets more complex...)
 */
IRAM_ATTR void *heap_caps_realloc_default( void *ptr, size_t size )
{
    return krhino_mm_realloc(ptr, size);
}

void heap_caps_enable_nonos_stack_heaps()
{
}

int heap_caps_get_free_size(void)
{
    return 0;
}

#endif
