#ifndef UMM_H
#define UMM_H


void* umm_init(void *mem, size_t size);

void* umm_malloc(size_t size);

void* umm_realloc(void *ptr, size_t size);

void umm_free(void *ptr);

#endif // UMM_H
