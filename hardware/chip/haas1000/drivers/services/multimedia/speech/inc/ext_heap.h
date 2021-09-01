


#ifndef __KWS_EXT_HEAP_H__
#define __KWS_EXT_HEAP_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

int32_t ext_heap_init(void);
int32_t ext_heap_deinit(void);
uint32_t ext_heap_get_used_buff_size();
uint32_t ext_heap_get_free_buff_size();
void *ext_get_buff(uint32_t size);
void *ext_alloc(int size);
void *ext_malloc(int size);
void *ext_calloc(int nitems, int size);
void ext_free(void *mem_ptr);

#ifdef __cplusplus
}
#endif

#endif