
#include "freertos_rom_init.h"

#include "FreeRTOS.h"

extern void _rom_clean_bss();
extern void _rom_init_mem();

int32_t _rom_rtos_init(void *data) {
//    _rom_clean_bss();
//    _rom_init_mem();
    if (data != NULL) {
        vPortDefineHeapRegions((HeapRegion_t *)data);
    }
    return 0;
}

void _rom_rtos_get_ptr(rom_rtos_init_t *ptr) {
    ptr->init = _rom_rtos_init;
    ptr->resume = NULL;
    ptr->suspend = NULL;
    ptr->deinit = NULL;
}

