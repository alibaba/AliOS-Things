#include <atmel_start.h>
#include <string.h>
#include "drv_board.h"
#include "emh_api.h"


			
void drv_board_test(void)
{
	int i = 0;

	char sensor_display[3][OLED_DISPLAY_MAX_CHAR_PER_ROW + 1];
	
	if ( true == gpio_get_pin_level(USR) ) return;

	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_1, "Board Test");
	emh_init();
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, emh_module_get_fw_version());
	
	/* Replace with your application code */
	while (1) {
		/* Test SHT20 sensor */
		sprintf(sensor_display[0], "Board Test, %d/%d", gpio_get_pin_level(SW_1), gpio_get_pin_level(SW_2));
		sprintf(sensor_display[1], "Temp: %.2f C", SHT2x_GetTempPoll());
		sprintf(sensor_display[2], "humi: %.2f %%", SHT2x_GetHumiPoll());
		
		OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_1, sensor_display[0]);
		OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_2, sensor_display[1]);
		OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, sensor_display[2]);
		
		if(i%6==0) color_led_open_rgb(255, 0, 0);
		else if(i%6==1) color_led_open_rgb(0, 255, 0);
		else if(i%6==2) color_led_open_rgb(0, 0, 255);
		else if(i%6==3) color_led_open_rgb(30, 30, 30);
		else if(i%6==4) color_led_open_rgb(130, 130, 130);
		else if(i%6==5) color_led_open_rgb(255, 255, 255);
		i++;
		
		printf("[Time: %d]\r\n", (int)mx_hal_ms_ticker_read());
		
		mx_hal_delay_ms(500);
	}
}
