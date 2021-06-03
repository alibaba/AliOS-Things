/**
 * ant_gpio.h
 *
 * GPIO访问接口
 */
#ifndef __ANT_GPIO_H__
#define __ANT_GPIO_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef enum {
    GPIO_LED_RED = 0,
    GPIO_LED_GREEN, 
    GPIO_LED_BLUE,
    GPIO_KEY_POWER,
    GPIO_KEY_VOLDN,
    GPIO_KEY_VOLUP,
    GPIO_KEY_FUNC,
    GPIO_PA_CTRL,
    GPIO_POWER_CTRL,
} ant_gpio_name_e;

typedef enum {
    GPIO_IN  = 0,
    GPIO_OUT = 1,   
} ant_gpio_dir_e;

typedef enum {
    GPIO_LOW  = 0,
    GPIO_HIGH = 1,  
} ant_gpio_level_e;

typedef enum {
    GPIO_PULL_DISABLE = 0,
    GPIO_PULL_DOWN    = 1,  
    GPIO_PULL_UP      = 2,
} ant_gpio_pullsel_e;

/**
 * GPIO 模块初始化
 * 
 * @return  0： 初始化成功
 *         -1: 初始化失败
 */
ant_s32 ant_gpio_init(void);

/**
 * 设置GPIO
 * 
 * @param gpio_name GPIO名称 
 * @param level GPIO高低电平
 * 
 * @return 0： 设置成功
 *        -1： 设置失败
 */
ant_s32 ant_gpio_set(ant_gpio_name_e gpio_name, ant_gpio_level_e level);

/**
 * 获取GPIO电平状态
 * 
 * @param gpio_name GPIO名称 
 * @param level GPIO高低电平
 * 
 * @return 大于等于0： 获取成功
 *              -1： 获取失败
 */
ant_s32 ant_gpio_get(ant_gpio_name_e gpio_name);

/**
 * 设置GPIO输入输出方向
 * 
 * @param gpio_name GPIO名称 
 * @param dir 输入输出方向
 * 
 * @return 大于等于0： 设置成功
 *              -1： 设置失败
 */
ant_s32 ant_gpio_set_dir(ant_gpio_name_e gpio_name, ant_gpio_dir_e dir);

    
#ifdef __cplusplus
}
#endif

#endif

