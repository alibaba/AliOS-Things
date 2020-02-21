/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_GPIO_STM32F4_H
#define __HAL_GPIO_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"


#define PINS_IN_GROUP  16
#define GROUP_GPIOA    0
#define GROUP_GPIOB    1
#define GROUP_GPIOC    2
#define GROUP_GPIOD    3
#define GROUP_GPIOE    4
#define GROUP_GPIOF    5
#define GROUP_GPIOG    6
#define TOTAL_GPIO_NUM 112

#define HAL_GPIO_0                 ((uint8_t)0)        /* represent GPIOA pin 0 */
#define HAL_GPIO_1                 ((uint8_t)1)        /* represent GPIOA pin 1 */
#define HAL_GPIO_2                 ((uint8_t)2)        /* represent GPIOA pin 2 */
#define HAL_GPIO_3                 ((uint8_t)3)        /* represent GPIOA pin 3 */
#define HAL_GPIO_4                 ((uint8_t)4)        /* represent GPIOA pin 4 */
#define HAL_GPIO_5                 ((uint8_t)5)        /* represent GPIOA pin 5 */
#define HAL_GPIO_6                 ((uint8_t)6)        /* represent GPIOA pin 6 */
#define HAL_GPIO_7                 ((uint8_t)7)        /* represent GPIOA pin 7 */
#define HAL_GPIO_8                 ((uint8_t)8)        /* represent GPIOA pin 8 */
#define HAL_GPIO_9                 ((uint8_t)9)        /* represent GPIOA pin 9 */
#define HAL_GPIO_10                ((uint8_t)10)       /* represent GPIOA pin 10 */
#define HAL_GPIO_11                ((uint8_t)11)       /* represent GPIOA pin 11 */
#define HAL_GPIO_12                ((uint8_t)12)       /* represent GPIOA pin 12 */
#define HAL_GPIO_13                ((uint8_t)13)       /* represent GPIOA pin 13 */
#define HAL_GPIO_14                ((uint8_t)14)       /* represent GPIOA pin 14 */
#define HAL_GPIO_15                ((uint8_t)15)       /* represent GPIOA pin 15 */

#define HAL_GPIO_16                ((uint8_t)16)       /* represent GPIOB pin 0 */
#define HAL_GPIO_17                ((uint8_t)17)       /* represent GPIOB pin 1 */
#define HAL_GPIO_18                ((uint8_t)18)       /* represent GPIOB pin 2 */
#define HAL_GPIO_19                ((uint8_t)19)       /* represent GPIOB pin 3 */
#define HAL_GPIO_20                ((uint8_t)20)       /* represent GPIOB pin 4 */
#define HAL_GPIO_21                ((uint8_t)21)       /* represent GPIOB pin 5 */
#define HAL_GPIO_22                ((uint8_t)22)       /* represent GPIOB pin 6 */
#define HAL_GPIO_23                ((uint8_t)23)       /* represent GPIOB pin 7 */
#define HAL_GPIO_24                ((uint8_t)24)       /* represent GPIOB pin 8 */
#define HAL_GPIO_25                ((uint8_t)25)       /* represent GPIOB pin 9 */
#define HAL_GPIO_26                ((uint8_t)26)       /* represent GPIOB pin 10 */
#define HAL_GPIO_27                ((uint8_t)27)       /* represent GPIOB pin 11 */
#define HAL_GPIO_28                ((uint8_t)28)       /* represent GPIOB pin 12 */
#define HAL_GPIO_29                ((uint8_t)29)       /* represent GPIOB pin 13 */
#define HAL_GPIO_30                ((uint8_t)30)       /* represent GPIOB pin 14 */
#define HAL_GPIO_31                ((uint8_t)31)       /* represent GPIOB pin 15 */

#define HAL_GPIO_32                ((uint8_t)32)       /* represent GPIOC pin 0 */
#define HAL_GPIO_33                ((uint8_t)33)       /* represent GPIOC pin 1 */
#define HAL_GPIO_34                ((uint8_t)34)       /* represent GPIOC pin 2 */
#define HAL_GPIO_35                ((uint8_t)35)       /* represent GPIOC pin 3 */
#define HAL_GPIO_36                ((uint8_t)36)       /* represent GPIOC pin 4 */
#define HAL_GPIO_37                ((uint8_t)37)       /* represent GPIOC pin 5 */
#define HAL_GPIO_38                ((uint8_t)38)       /* represent GPIOC pin 6 */
#define HAL_GPIO_39                ((uint8_t)39)       /* represent GPIOC pin 7 */
#define HAL_GPIO_40                ((uint8_t)40)       /* represent GPIOC pin 8 */
#define HAL_GPIO_41                ((uint8_t)41)       /* represent GPIOC pin 9 */
#define HAL_GPIO_42                ((uint8_t)42)       /* represent GPIOC pin 10 */
#define HAL_GPIO_43                ((uint8_t)43)       /* represent GPIOC pin 11 */
#define HAL_GPIO_44                ((uint8_t)44)       /* represent GPIOC pin 12 */
#define HAL_GPIO_45                ((uint8_t)45)       /* represent GPIOC pin 13 */
#define HAL_GPIO_46                ((uint8_t)46)       /* represent GPIOC pin 14 */
#define HAL_GPIO_47                ((uint8_t)47)       /* represent GPIOC pin 15 */

#define HAL_GPIO_48                ((uint8_t)48)       /* represent GPIOD pin 0 */
#define HAL_GPIO_49                ((uint8_t)49)       /* represent GPIOD pin 1 */
#define HAL_GPIO_50                ((uint8_t)50)       /* represent GPIOD pin 2 */
#define HAL_GPIO_51                ((uint8_t)51)       /* represent GPIOD pin 3 */
#define HAL_GPIO_52                ((uint8_t)52)       /* represent GPIOD pin 4 */
#define HAL_GPIO_53                ((uint8_t)53)       /* represent GPIOD pin 5 */
#define HAL_GPIO_54                ((uint8_t)54)       /* represent GPIOD pin 6 */
#define HAL_GPIO_55                ((uint8_t)55)       /* represent GPIOD pin 7 */
#define HAL_GPIO_56                ((uint8_t)56)       /* represent GPIOD pin 8 */
#define HAL_GPIO_57                ((uint8_t)57)       /* represent GPIOD pin 9 */
#define HAL_GPIO_58                ((uint8_t)58)       /* represent GPIOD pin 10 */
#define HAL_GPIO_59                ((uint8_t)59)       /* represent GPIOD pin 11 */
#define HAL_GPIO_60                ((uint8_t)60)       /* represent GPIOD pin 12 */
#define HAL_GPIO_61                ((uint8_t)61)       /* represent GPIOD pin 13 */
#define HAL_GPIO_62                ((uint8_t)62)       /* represent GPIOD pin 14 */
#define HAL_GPIO_63                ((uint8_t)63)       /* represent GPIOD pin 15 */

#define HAL_GPIO_64                ((uint8_t)64)       /* represent GPIOE pin 0 */
#define HAL_GPIO_65                ((uint8_t)65)       /* represent GPIOE pin 1 */
#define HAL_GPIO_66                ((uint8_t)66)       /* represent GPIOE pin 2 */
#define HAL_GPIO_67                ((uint8_t)67)       /* represent GPIOE pin 3 */
#define HAL_GPIO_68                ((uint8_t)68)       /* represent GPIOE pin 4 */
#define HAL_GPIO_69                ((uint8_t)69)       /* represent GPIOE pin 5 */
#define HAL_GPIO_70                ((uint8_t)70)       /* represent GPIOE pin 6 */
#define HAL_GPIO_71                ((uint8_t)71)       /* represent GPIOE pin 7 */
#define HAL_GPIO_72                ((uint8_t)72)       /* represent GPIOE pin 8 */
#define HAL_GPIO_73                ((uint8_t)73)       /* represent GPIOE pin 9 */
#define HAL_GPIO_74                ((uint8_t)74)       /* represent GPIOE pin 10 */
#define HAL_GPIO_75                ((uint8_t)75)       /* represent GPIOE pin 11 */
#define HAL_GPIO_76                ((uint8_t)76)       /* represent GPIOE pin 12 */
#define HAL_GPIO_77                ((uint8_t)77)       /* represent GPIOE pin 13 */
#define HAL_GPIO_78                ((uint8_t)78)       /* represent GPIOE pin 14 */
#define HAL_GPIO_79                ((uint8_t)79)       /* represent GPIOE pin 15 */

#define HAL_GPIO_80                ((uint8_t)80)       /* represent GPIOF pin 0 */
#define HAL_GPIO_81                ((uint8_t)81)       /* represent GPIOF pin 1 */
#define HAL_GPIO_82                ((uint8_t)82)       /* represent GPIOF pin 2 */
#define HAL_GPIO_83                ((uint8_t)83)       /* represent GPIOF pin 3 */
#define HAL_GPIO_84                ((uint8_t)84)       /* represent GPIOF pin 4 */
#define HAL_GPIO_85                ((uint8_t)85)       /* represent GPIOF pin 5 */
#define HAL_GPIO_86                ((uint8_t)86)       /* represent GPIOF pin 6 */
#define HAL_GPIO_87                ((uint8_t)87)       /* represent GPIOF pin 7 */
#define HAL_GPIO_88                ((uint8_t)88)       /* represent GPIOF pin 8 */
#define HAL_GPIO_89                ((uint8_t)89)       /* represent GPIOF pin 9 */
#define HAL_GPIO_90                ((uint8_t)90)       /* represent GPIOF pin 10 */
#define HAL_GPIO_91                ((uint8_t)91)       /* represent GPIOF pin 11 */
#define HAL_GPIO_92                ((uint8_t)92)       /* represent GPIOF pin 12 */
#define HAL_GPIO_93                ((uint8_t)93)       /* represent GPIOF pin 13 */
#define HAL_GPIO_94                ((uint8_t)94)       /* represent GPIOF pin 14 */
#define HAL_GPIO_95                ((uint8_t)95)       /* represent GPIOF pin 15 */

#define HAL_GPIO_96                ((uint8_t)96 )       /* represent GPIOG pin 0 */
#define HAL_GPIO_97                ((uint8_t)97 )       /* represent GPIOG pin 1 */
#define HAL_GPIO_98                ((uint8_t)98 )       /* represent GPIOG pin 2 */
#define HAL_GPIO_99                ((uint8_t)99 )       /* represent GPIOG pin 3 */
#define HAL_GPIO_100               ((uint8_t)100)       /* represent GPIOG pin 4 */
#define HAL_GPIO_101               ((uint8_t)101)       /* represent GPIOG pin 5 */
#define HAL_GPIO_102               ((uint8_t)102)       /* represent GPIOG pin 6 */
#define HAL_GPIO_103               ((uint8_t)103)       /* represent GPIOG pin 7 */
#define HAL_GPIO_104               ((uint8_t)104)       /* represent GPIOG pin 8 */
#define HAL_GPIO_105               ((uint8_t)105)       /* represent GPIOG pin 9 */
#define HAL_GPIO_106               ((uint8_t)106)       /* represent GPIOG pin 10 */
#define HAL_GPIO_107               ((uint8_t)107)       /* represent GPIOG pin 11 */
#define HAL_GPIO_108               ((uint8_t)108)       /* represent GPIOG pin 12 */
#define HAL_GPIO_109               ((uint8_t)109)       /* represent GPIOG pin 13 */
#define HAL_GPIO_110               ((uint8_t)110)       /* represent GPIOG pin 14 */
#define HAL_GPIO_111               ((uint8_t)111)       /* represent GPIOG pin 15 */
#define HAL_GPIO_CNT               ((uint8_t)112)


//LED
#define LED_0  HAL_GPIO_21 //GPIOB-PIN5
#define LED_1  HAL_GPIO_69 //GPIOE-PIN5

//KEY
#define KEY_0  HAL_GPIO_68 //GPIOE-PIN4
#define KEY_1  HAL_GPIO_67 //GPIOE-PIN3
#define KEY_2  HAL_GPIO_66 //GPIOE-PIN2

//BEEP
#define BEEP_0 HAL_GPIO_24 //GPIO-PIN8

#define GPIOA_SPEED	 GPIO_SPEED_FREQ_VERY_HIGH

extern int32_t brd_gpio_init(void);

extern GPIO_TypeDef *hal_gpio_typedef(uint16_t hal_pin);
extern uint32_t hal_gpio_pin(uint16_t hal_pin);
extern void hal_gpio_enable_clk(uint16_t hal_pin);

#endif /* #ifdef HAL_GPIO_MODULE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __HAL_GPIO_STM32L4_H */
