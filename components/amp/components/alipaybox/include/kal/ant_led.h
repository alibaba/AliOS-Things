#ifndef __ANT_LED_H__
#define __ANT_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ant_typedef.h"

typedef enum {
	LED_R = 0,
	LED_B,
	LED_Y,
	LED_OFF,
} led_color_e;

void led_on(led_color_e color);
void led_off(void);
void led_blink(led_color_e color);
void led_blink_off(void);
void led_quick_blink(led_color_e color);
void led_breath(led_color_e color);

void bled_on(ant_u32 bat_level);
void bled_blink(void);
void bled_off(void);
ant_bool led_off_check(void);

#ifdef __cplusplus
}
#endif
#endif

