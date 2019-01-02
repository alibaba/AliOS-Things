/**
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */
#include "mx_debug.h"
#include "mx_common.h"
#include "alicloud_sds.h"

#include "drv_board.h"
#include "main.h"


mx_status console_task_init(void)
{
	alisds_attr_t attr;
	attr.name = "DebVal";
	attr.type = ALI_ATT_TYPE_STRING;
	attr.read_func = handle_read_console;
	attr.write_func = handle_write_console;
	alisds_attr_init(ALI_HANDLE_CONSOLE, attr);

	return kNoErr;
}

#if 0
static uint32_t latest_console_send_tick = 0;
#define CONSOLE_PACKET_SIZE   512
int con_seq = 0;

char console_buff[CONSOLE_PACKET_SIZE+20];

void console_task(void)
{
	mx_status err = kNoErr;
	struct json_str jstr;

	char console_string[CONSOLE_PACKET_SIZE];

	json_str_init(&jstr, console_buff, sizeof(console_buff));

	if ( mx_hal_ms_ticker_read()-latest_console_send_tick > SENSOR_READ_INTERVAL) {

		snprintf(console_string, CONSOLE_PACKET_SIZE, "This is Packet %d\r\n", con_seq);

		err = json_start_object(&jstr);
		require_noerr(err, exit);

		json_set_val_str(&jstr, "value", console_string);
		json_set_val_int(&jstr, "seq", con_seq);

		err = json_close_object(&jstr);
		require_noerr(err, exit);

		alisds_attr_indicate_by_handle(ALI_HANDLE_CONSOLE);
		con_seq++;
	}
	exit:
	return;
}
#endif



char console_buff[ALISDS_ATTR_VAL_MAX_LEN]="hello, world!";
const char oled_clear_line[OLED_DISPLAY_MAX_CHAR_PER_ROW]="                ";

void OLED_ShowStatusString(const char *status_str)
{
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, (char *)oled_clear_line);
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, (char *)status_str);
}


mx_status handle_read_console (alisds_att_val_t *value)
{
	(*value).stringValue = console_buff;
	return kNoErr;
}

mx_status handle_write_console(alisds_att_val_t value)
{
	app_log("recv %s", value.stringValue);
	strncpy(console_buff, value.stringValue, ALISDS_ATTR_VAL_MAX_LEN);
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, (char *)oled_clear_line);
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, (char *)console_buff);
	return kNoErr;
}

void alisds_event_handler(alisds_event_t event)
{
	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, (char *)oled_clear_line);
	switch (event) {
		case ALISDS_EVENT_WLAN_CONFIG_STARTED: {
			OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "Wi-Fi config....");
			break;
		}
		case ALISDS_EVENT_WLAN_CONNECTED: {
			OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "Wi-Fi connected");
			break;
		}
		case ALISDS_EVENT_WLAN_DISCONNECTED: {
			OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "Wi-Fi disconnected");
			break;
		}
		case ALISDS_EVENT_CLOUD_CONNECTED: {
			OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "Cloud connected");
			break;
		}
		case ALISDS_EVENT_CLOUD_DISCONNECTED: {
			OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "Cloud disconnected");
			break;
		}
	}
}
