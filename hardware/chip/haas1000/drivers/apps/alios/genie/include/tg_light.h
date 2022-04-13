/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tgcts_light.h
 * @brief:
 * @author:  
 * @date:    2019/12/15
 * @version: 1.0
 */

#ifndef _TG_VENDOR_LIGHT_H_
#define _TG_VENDOR_LIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif


enum tg_light_channel {
    TG_LIGHT_PWM_CHANNEL_0 = 0,
    TG_LIGHT_PWM_CHANNEL_1,
    TG_LIGHT_PWM_CHANNEL_2,
    TG_LIGHT_PWM_CHANNEL_NUM
};

#define TG_LIGHT_BRIGHTNESS_MAX         (0xff)


/**
 * @brief init pwm led
 * @param[in] channel: the channel to be set
 * @return Zero on success or error code otherwise
 */
int tg_light_pwm_led_init(unsigned int  channel);

/**
 * @brief set pwm led brightness
 * @param[in] channel: the channel to be set
 * @param[in] brightness: the brightness, 0~255
 * @return Zero on success or error code otherwise
 */
int tg_light_pwm_led_set_brightness(unsigned int channel, unsigned char brightness);

/**
 * add for super marie
 * @brief set pwm led brightness
 * @param[in] channel: the color channel to be set
 * @param[in] brightness: the brightness, 0~255
 * @return Zero on success or error code otherwise
 */
int tg_set_pwm_led_target_channel_brightness(unsigned int color_channel, unsigned int brightness);

#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_LIGHT_H_ */
