#include <stdint.h>
#include "hal_gpio.h"
#include "hal_pinmux.h"

#include "ota_flash.h"
#include "tiny_printf.h"

// this function will execute before boot system start.
void _boot_init() {
    tiny_printf_display(1);

    //set pinmux for uart fw upgrade
	uint32_t pin_mode = 0;
    uint32_t sel_func = 0;

#if 1    //use gpio3 and gpio4 for uart fw upgrade
    pin_mode |= (0x1 << UART0_RXD_II);	//set gpio3 manual
    pin_mode |= (0x1 << UART0_TXD_II);	//set gpio4 manual
    sel_func |= (0x1 << SEL_UART0_II);	//enable uart0 as gpio3 and gpio4
#else    //use gpio21 and gpio22 for uart fw upgrade
    pin_mode |= (0x1 << UART0_TXD_I);	//set gpio21 manual
    pin_mode |= (0x1 << UART0_RXD_I);	//set gpio22 manual
    sel_func |= (0x1 << SEL_UART0_I);	//enable uart0 as gpio21 and gpio22
#endif    

    hal_pinmux_set_raw(pin_mode, sel_func);	
}

// define ota by filesystem.
// @return 1: ota by filesystem.
//         0: no ota.
int _ota_use_fs() {
    return 1;
}

