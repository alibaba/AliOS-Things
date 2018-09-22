#include "mx_debug.h"
#include "mx_common.h"
#include "alicloud_sds.h"

#include "drv_board.h"
#include "main.h"

static float bright = 100.0;
static float saturation = 100.0;
static float hue = 50.0;
static bool  light_on = true;

mx_status rgbled_task_init(void)
{
	alisds_attr_t attr;
	color_led_open_hsb(hue, saturation, bright);

	attr.name = "Saturation";
	attr.type = ALI_ATT_TYPE_INT;
	attr.read_func = handle_read_cur_saturation;
	attr.write_func = handle_write_cur_saturation;
	alisds_attr_init(ALI_HANDLE_SATURATION, attr);

	attr.name = "Luminance";
	attr.type = ALI_ATT_TYPE_INT;
	attr.read_func = handle_read_cur_bright;
	attr.write_func = handle_write_cur_bright;
	alisds_attr_init(ALI_HANDLE_BRIGHTNESS, attr);

	attr.name = "Hue";
	attr.type = ALI_ATT_TYPE_INT;
	attr.read_func = handle_read_cur_hue;
	attr.write_func = handle_write_cur_hue;
	alisds_attr_init(ALI_HANDLE_HUE, attr);

	attr.name = "lampswitch";
	attr.type = ALI_ATT_TYPE_BOOL;
	attr.read_func = handle_read_cur_light_switch;
	attr.write_func = handle_write_cur_light_switch;
	alisds_attr_init(ALI_HANDLE_LIGHT_SWITCH, attr);
	
	return kNoErr;
}


/* RGB led attribute handlers */
mx_status handle_write_cur_saturation	(alisds_att_val_t value)
{
	saturation = (float)value.intValue;
	if (light_on == true) {
		color_led_open_hsb(hue, saturation, bright);
	}
	return kNoErr;
}
mx_status handle_write_cur_bright		(alisds_att_val_t value)
{
	bright = (float)value.intValue;
	if (light_on == true) {
		color_led_open_hsb(hue, saturation, bright);
	}
	return kNoErr;
}

mx_status handle_write_cur_hue			(alisds_att_val_t value)
{
	hue = (float)value.intValue;
	if (light_on == true) {
		app_log("Light: %.2f, %.2f, %.2f", hue, saturation, bright);
		color_led_open_hsb(hue, saturation, bright);
	}
	return kNoErr;
}

mx_status handle_write_cur_light_switch(alisds_att_val_t value)
{
	light_on = value.boolValue;
	if (light_on == true) {
		color_led_open_hsb(hue, saturation, bright);
	}
	else {
		color_led_close();
	}
	return kNoErr;
}


mx_status handle_read_cur_saturation(alisds_att_val_t *value)
{
	(*value).intValue = (int)saturation;
	return kNoErr;
}

mx_status handle_read_cur_bright(alisds_att_val_t *value)
{
	(*value).intValue = (int)bright;
	return kNoErr;
}

mx_status handle_read_cur_hue(alisds_att_val_t *value)
{
	(*value).intValue = (int)hue;
	return kNoErr;
}

mx_status handle_read_cur_light_switch(alisds_att_val_t *value)
{
	(*value).boolValue = light_on;
	return kNoErr;
}