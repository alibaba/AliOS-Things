#include "rom_init.h"


void _rom_get_ptr(rom_init_t *ptr) {
    ptr->init = NULL;
    ptr->resume = NULL;
    ptr->suspend = NULL;
    ptr->deinit = NULL;
}

