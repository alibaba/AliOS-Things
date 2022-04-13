#ifndef __HEAP_API__
#define __HEAP_API__
#include "stdint.h"
#include "string.h"
#include "multi_heap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYSPOOL_PURE __attribute__((__const__))

void syspool_init(void);
void syspool_init_specific_size(uint32_t size);
int syspool_free_size(void);
int syspool_get_buff(uint8_t **buff, uint32_t size);
int syspool_get_available(uint8_t **buff);
uint8_t* syspool_start_addr(void) SYSPOOL_PURE;
uint32_t syspool_total_size(void);
uint32_t syspool_original_size(void) SYSPOOL_PURE;

#if defined(A2DP_LDAC_ON)
int syspool_force_used_size(uint32_t size);
#endif

#define heap_malloc multi_heap_malloc

#define heap_free multi_heap_free

#define heap_realloc multi_heap_realloc

#define heap_get_allocated_size multi_heap_get_allocated_size

#define heap_register multi_heap_register

#define heap_free_size multi_heap_free_size

#define heap_minimum_free_size multi_heap_minimum_free_size

#define heap_get_info multi_heap_get_info

#define heap_dump multi_heap_dump

#define heap_check multi_heap_check

typedef struct multi_heap_info *heap_handle_t;
heap_handle_t heap_register(void *start, size_t size);
void *heap_malloc(heap_handle_t heap, size_t size);
void heap_free(heap_handle_t heap, void *p);
void *heap_realloc(heap_handle_t heap, void *p, size_t size);
size_t heap_get_allocated_size(heap_handle_t heap, void *p);
void heap_set_lock(heap_handle_t heap, void* lock);
void heap_dump(heap_handle_t heap);
bool heap_check(heap_handle_t heap, bool print_errors);
size_t heap_free_size(heap_handle_t heap);
size_t heap_minimum_free_size(heap_handle_t heap);
void heap_get_info(heap_handle_t heap, multi_heap_info_t *info);
void heap_memory_info(heap_handle_t heap,
                    size_t *total,
                    size_t *used,
                    size_t *max_used);

#if 0
void *malloc(size_t size);
void free(void *p);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
#endif
void med_heap_init(void *begin_addr, size_t size);
void med_heap_set_cp(int switch_cp);
void med_heap_add_block(void *begin_addr, size_t size);
void *med_malloc(size_t size);
void med_free(void *p);
void *med_calloc(size_t nmemb, size_t size);
void *med_realloc(void *ptr, size_t size);
void med_memory_info(size_t *total,
                    size_t *used,
                    size_t *max_used);

#ifdef __cplusplus
}
#endif
#endif
