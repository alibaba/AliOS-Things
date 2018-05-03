#include "lwip_rom_init.h"

rom_lwip_depend_t _rom_lwip_depend = {.netstack_output = NULL};

void _rom_lwip_init(rom_lwip_depend_t dep) {
    _rom_lwip_depend = dep;
}

void _rom_lwip_get_ptr(rom_lwip_init_t *ptr) {
    ptr->init = NULL;
    ptr->resume = NULL;
    ptr->suspend = NULL;
    ptr->deinit = NULL;
}

