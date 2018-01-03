#include "esp_attr.h"
#include "k_api.h"

IRAM_ATTR void *heap_caps_malloc(size_t size, uint32_t caps)
{
    (void)caps;
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


void heap_caps_enable_nonos_stack_heaps()
{
}

int heap_caps_get_free_size(void)
{
    return 0;
}
