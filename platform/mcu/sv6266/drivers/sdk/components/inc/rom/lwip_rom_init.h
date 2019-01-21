#ifndef LWIP_ROM_INIT_H
#define LWIP_ROM_INIT_H

#include "soc_types.h"

typedef struct {
    int (*netstack_output)(struct netif *netif, void *data, u32 len);
} rom_lwip_depend_t;

typedef struct {
    int32_t (*init)(void *data);
    int32_t (*resume)(void *data);
    int32_t (*suspend)(void *data);
    int32_t (*deinit)(void *data);
} rom_lwip_init_t;

void _rom_lwip_init(rom_lwip_depend_t dep);
void _rom_lwip_get_ptr(rom_lwip_init_t *ptr);


extern rom_lwip_depend_t _rom_lwip_depend;

#endif /* end of include guard: LWIP_ROM_INIT_H */
