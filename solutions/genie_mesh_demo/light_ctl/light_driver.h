#ifndef __LIGHT_DRIVER_H__
#define __LIGHT_DRIVER_H__

// #define WARM_PIN 24 //warm led
// #define COLD_PIN 20 //cold led

// #define COLD_PORT_NUM 0
// #define WARM_PORT_NUM 1

// #define PWM_CHANNEL_COUNT 2

// #define LIGHT_PERIOD 500

// void light_driver_init(void);
// void light_driver_update(uint8_t onoff, uint16_t lightness, uint16_t temperature);

void light_driver_init(void);
void light_set(uint8_t onoff);
void light_blink(void);

#endif
