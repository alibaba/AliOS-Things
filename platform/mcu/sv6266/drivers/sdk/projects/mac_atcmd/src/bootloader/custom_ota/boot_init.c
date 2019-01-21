#include <stdint.h>
#include "hal_gpio.h"
#include "hal_pinmux.h"

#include "ota_flash.h"
#include "tiny_printf.h"

// this function will execute before boot system start.
void _boot_init() {
    tiny_printf_display(1);
}

// define ota by filesystem.
// @return 1: ota by filesystem.
//         0: no ota.
int _ota_use_fs() {
    return 1;
}

