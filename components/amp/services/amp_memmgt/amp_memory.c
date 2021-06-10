#include "amp_memmgt.h"
#include "aos_system.h"

void amp_memory_init()
{
    static int memory_initialized = 0;

    if(0 == memory_initialized) {
        amp_memmgt_config_t config;
        memset(&config, 0, sizeof(config));
        config.malloc_fn = aos_malloc;
        config.realloc_fn = aos_realloc;
        config.free_fn   = aos_free;
        config.pt_num  = 2;
        config.trace_pt = 1; //b0001
        config.mem_limit[1] = 512000;

        amp_memmgt_init(&config);
        memory_initialized = 1;
    }
}

void *amp_malloc(unsigned int size)
{
    void *ptr = NULL;

    AMP_MEMMGT_MALLOC(ptr, size, 0);

    return ptr;
}

void amp_free(void *ptr)
{
    AMP_MEMMGT_FREE(ptr, 0);
}

void *amp_calloc(unsigned int nitems, unsigned int size)
{
    void *ptr = NULL;

    AMP_MEMMGT_MALLOC(ptr, nitems * size, 0);
    if (ptr)
        memset(ptr, 0, nitems * size);

    return ptr;
}

void *amp_realloc(void *ptr, unsigned int size)
{
    void *ptr_new = NULL;

    if (ptr != NULL) {
        AMP_MEMMGT_REALLOC(ptr_new, ptr, size, 0);
    } else {
        AMP_MEMMGT_MALLOC(ptr_new, size, 0);
    }

    return ptr_new;
}