#ifndef __LIGHT_H__
#define __LIGHT_H__

#ifndef CONFIG_INFO_DISABLE
#define LIGHT_DBG(fmt, ...) printf("[%s]" fmt "\n", __func__, ##__VA_ARGS__)
#else
#define LIGHT_DBG(fmt, ...)
#endif

#define LED_BLINK_PERIOD 1000
#define LED_BLINK_ON_TIME 600
#define LED_BLINK_OFF_TIME 400

/* unprovision device beacon adv time : 10 minutes*/
#define MESH_PBADV_TIME (600 * 1000)

#define MESH_ELEM_COUNT 1
#define ELEMENT_NUM MESH_ELEM_COUNT

typedef struct {
    struct k_timer led_blink_timer;
    uint16_t present_color_temperature;
    uint16_t target_color_temperature;
    uint16_t lightness_start;
    uint16_t present_lightness;
    uint16_t target_lightness;
    uint32_t color_temperature_end;
} light_param_t;

#endif
