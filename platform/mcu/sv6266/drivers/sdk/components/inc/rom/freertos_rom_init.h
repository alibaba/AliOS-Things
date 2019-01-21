#ifndef FREERTOS_ROM_INIT_H
#define FREERTOS_ROM_INIT_H

#include "soc_types.h"

typedef struct {
    int32_t (*init)(void *data);
    int32_t (*resume)(void *data);
    int32_t (*suspend)(void *data);
    int32_t (*deinit)(void *data);
} rom_rtos_init_t;

void _rom_rtos_get_ptr(rom_rtos_init_t *ptr);


#endif /* end of include guard: FREERTOS_ROM_INIT_H */
