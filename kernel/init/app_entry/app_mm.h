#ifndef APP_MM_H
#define APP_MM_H


void app_mm_init(void *mem, size_t size);

void* app_mm_malloc(size_t size);

void* app_mm_realloc(void *ptr, size_t size);

void app_mm_free(void *ptr);

#endif // APP_MM_H
