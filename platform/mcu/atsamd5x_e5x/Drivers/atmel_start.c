#include <atmel_start.h>

/**
 * Initializes MCU, drivers and middleware in the project
**/
void atmel_start_init(void)
{
	system_init();

	usart_sync_enable(&EDBG_COM);
	gpio_set_pin_function(PB15, PINMUX_PB15M_GCLK_IO1);
	gpio_set_pin_function(PB14, PINMUX_PB14M_GCLK_IO0);
}
