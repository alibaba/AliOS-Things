#include "mx_debug.h"
#include "mx_common.h"
#include "alicloud_sds.h"

#include "drv_board.h"
#include "main.h"

#define SENSOR_READ_INTERVAL    5000


static float temperature = 24.00;
static float humidity = 50.00;
static uint32_t latest_sensor_read_tick = 0;


char sensor_display[2][OLED_DISPLAY_MAX_CHAR_PER_ROW + 1];


mx_status SHT20_task_init(void)
{
	alisds_attr_t attr;
	temperature = SHT2x_GetTempPoll();
	humidity = SHT2x_GetHumiPoll();

	sprintf(sensor_display[0], "Temp: %.2f C", temperature);
	sprintf(sensor_display[1], "Humi: %.2f %%", humidity);
	
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_2, sensor_display[0]);
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, sensor_display[1]);
	
	attr.name = "CurrentTemperature_1";
	attr.type = ALI_ATT_TYPE_FLOAT;
	attr.read_func = handle_read_cur_temperature;
	attr.write_func = NULL;
	alisds_attr_init(ALI_HANDLE_CURRENT_TEMPERATURE, attr);

	attr.name = "CurrentHumidity";
	attr.type = ALI_ATT_TYPE_FLOAT;
	attr.read_func = handle_read_cur_humidity;
	attr.write_func = NULL;
	alisds_attr_init(ALI_HANDLE_CURRENT_HUMIDITY, attr);
	
	return kNoErr;
}

void SHT20_task(void)
{
	if ( mx_hal_ms_ticker_read()-latest_sensor_read_tick > SENSOR_READ_INTERVAL) {
		temperature = SHT2x_GetTempPoll();
		humidity = SHT2x_GetHumiPoll();
		sprintf(sensor_display[0], "Temp: %.2f C", temperature);
		sprintf(sensor_display[1], "Humi: %.2f %%", humidity);
		
		OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_2, sensor_display[0]);
		OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, sensor_display[1]);

		alisds_attr_indicate_by_handle(ALI_HANDLE_CURRENT_TEMPERATURE);
		alisds_attr_indicate_by_handle(ALI_HANDLE_CURRENT_HUMIDITY);
		
		latest_sensor_read_tick = mx_hal_ms_ticker_read();
	}
}


mx_status handle_read_cur_humidity(alisds_att_val_t *value)
{
	(*value).floatValue = humidity;
	return kNoErr;
}


mx_status handle_read_cur_temperature(alisds_att_val_t *value)
{
	(*value).floatValue = temperature;
	return kNoErr;
}
