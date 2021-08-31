

#ifndef _OLED_H_
#define _OLED_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OLED_STR_IDLE,
    OLED_STR_WAKEUP,
    OLED_STR_LIGHT_ON,
    OLED_STR_LIGHT_OFF,
    OLED_STR_MAX
} oled_str_e;

void oled_show(oled_str_e str);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif
