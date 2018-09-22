
#include <stdio.h>
#include "oled.h"
#include "color_led.h"
#include "sht2x.h"

void drv_board_init(void)
{
	/* System time ticker */
	ms_ticker_init();
	
	/* RGB color led initialize */
	color_led_init();
	color_led_open(0, 0, 0);
	
	/* Temperature and humidity sensor  */
	SHT2x_Init();

	/* init OLED */
	OLED_Init();
	OLED_Clear();

}
