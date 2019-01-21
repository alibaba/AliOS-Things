#ifndef ROM_INIT_H
#define ROM_INIT_H

#include "soc_types.h"

typedef struct {
    int32_t (*init)(void *data);
    int32_t (*resume)(void *data);
    int32_t (*suspend)(void *data);
    int32_t (*deinit)(void *data);
} rom_init_t;

void _rom_get_ptr(rom_init_t *ptr);

#endif /* end of include guard: ROM_INIT_H */
