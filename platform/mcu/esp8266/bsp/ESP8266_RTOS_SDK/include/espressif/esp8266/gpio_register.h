/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _GPIO_REGISTER_H_
#define _GPIO_REGISTER_H_

#define PERIPHS_GPIO_BASEADDR               0x60000300

#define GPIO_OUT_ADDRESS                    0x00
#define GPIO_BT_SEL                             0x0000ffff
#define GPIO_BT_SEL_S                           16
#define GPIO_OUT_DATA                           0x0000ffff
#define GPIO_OUT_DATA_S                         0

#define GPIO_OUT_W1TS_ADDRESS               0x04
#define GPIO_OUT_DATA_W1TS                      0x0000ffff
#define GPIO_OUT_DATA_W1TS_S                    0

#define GPIO_OUT_W1TC_ADDRESS               0x08
#define GPIO_OUT_DATA_W1TC                      0x0000ffff
#define GPIO_OUT_DATA_W1TC_S                    0
#define GPIO_OUT_DATA_MASK                      0x0000ffff

#define GPIO_ENABLE_ADDRESS                 0x0c
#define GPIO_SDIO_SEL                           0x0000003f
#define GPIO_SDIO_SEL_S                         16
#define GPIO_ENABLE_DATA                        0x0000ffff
#define GPIO_ENABLE_DATA_S                      0

#define GPIO_ENABLE_W1TS_ADDRESS            0x10
#define GPIO_ENABLE_DATA_W1TS                   0x0000ffff
#define GPIO_ENABLE_DATA_W1TS_s                 0

#define GPIO_ENABLE_W1TC_ADDRESS            0x14
#define GPIO_ENABLE_DATA_W1TC                   0x0000ffff
#define GPIO_ENABLE_DATA_W1TC_S                 0
#define GPIO_ENABLE_DATA_DATA_MASK              0x0000ffff

#define GPIO_IN_ADDRESS                     0x18
#define GPIO_STRAPPING                          0x0000ffff
#define GPIO_STRAPPING_S                        16
#define GPIO_IN_DATA                            0x0000ffff
#define GPIO_IN_DATA_S                          0

#define GPIO_STATUS_ADDRESS                 0x1c
#define GPIO_STATUS_INTERRUPT                   0x0000ffff
#define GPIO_STATUS_INTERRUPT_S                 0

#define GPIO_STATUS_W1TS_ADDRESS            0x20
#define GPIO_STATUS_INTERRUPT_W1TS              0x0000ffff
#define GPIO_STATUS_INTERRUPT_W1TS_S            0

#define GPIO_STATUS_W1TC_ADDRESS            0x24
#define GPIO_STATUS_INTERRUPT_W1TC              0x0000ffff
#define GPIO_STATUS_INTERRUPT_W1TC_S            0
#define GPIO_STATUS_INTERRUPT_DATA_MASK         0x0000ffff

//Region1: used for gpio config for GPIO_PIN0_ADDRESS~GPIO_PIN15_ADDRESS
#define GPIO_ID_PIN0                        0
#define GPIO_ID_PIN(n)                      (GPIO_ID_PIN0+(n))
#define GPIO_LAST_REGISTER_ID               GPIO_ID_PIN(15)
#define GPIO_ID_NONE                        0xffffffff
#define GPIO_PIN_COUNT                      16

#define GPIO_PIN_CONFIG_MSB                 12
#define GPIO_PIN_CONFIG_LSB                 11
#define GPIO_PIN_CONFIG_MASK                (0x00000003<<GPIO_PIN_CONFIG_LSB)
#define GPIO_PIN_CONFIG_GET(x)              (((x) & GPIO_PIN_CONFIG_MASK) >> GPIO_PIN_CONFIG_LSB)
#define GPIO_PIN_CONFIG_SET(x)              (((x) << GPIO_PIN_CONFIG_LSB) & GPIO_PIN_CONFIG_MASK)

#define GPIO_WAKEUP_ENABLE                  1
#define GPIO_WAKEUP_DISABLE                 (~GPIO_WAKEUP_ENABLE)
#define GPIO_PIN_WAKEUP_ENABLE_MSB          10
#define GPIO_PIN_WAKEUP_ENABLE_LSB          10
#define GPIO_PIN_WAKEUP_ENABLE_MASK         (0x00000001<<GPIO_PIN_WAKEUP_ENABLE_LSB)
#define GPIO_PIN_WAKEUP_ENABLE_GET(x)       (((x) & GPIO_PIN_CONFIG_MASK) >> GPIO_PIN_CONFIG_LSB)
#define GPIO_PIN_WAKEUP_ENABLE_SET(x)       (((x) << GPIO_PIN_WAKEUP_ENABLE_LSB) & GPIO_PIN_WAKEUP_ENABLE_MASK)

#define GPIO_PIN_INT_TYPE_MSB               9
#define GPIO_PIN_INT_TYPE_LSB               7
#define GPIO_PIN_INT_TYPE_MASK              (0x00000007<<GPIO_PIN_INT_TYPE_LSB)
#define GPIO_PIN_INT_TYPE_GET(x)            (((x) & GPIO_PIN_INT_TYPE_MASK) >> GPIO_PIN_INT_TYPE_LSB)
#define GPIO_PIN_INT_TYPE_SET(x)            (((x) << GPIO_PIN_INT_TYPE_LSB) & GPIO_PIN_INT_TYPE_MASK)

#define GPIO_PAD_DRIVER_ENABLE              1
#define GPIO_PAD_DRIVER_DISABLE             (~GPIO_PAD_DRIVER_ENABLE)
#define GPIO_PIN_DRIVER_MSB                 2
#define GPIO_PIN_DRIVER_LSB                 2
#define GPIO_PIN_DRIVER_MASK                (0x00000001<<GPIO_PIN_DRIVER_LSB)
#define GPIO_PIN_DRIVER_GET(x)              (((x) & GPIO_PIN_INT_TYPE_MASK) >> GPIO_PIN_INT_TYPE_LSB)
#define GPIO_PIN_PAD_DRIVER_SET(x)          (((x) << GPIO_PIN_DRIVER_LSB) & GPIO_PIN_DRIVER_MASK)

#define GPIO_PIN_SOURCE_MSB                 0
#define GPIO_PIN_SOURCE_LSB                 0
#define GPIO_PIN_SOURCE_MASK                (0x00000001<<GPIO_PIN_SOURCE_LSB)
#define GPIO_PIN_SOURCE_GET(x)              (((x) & GPIO_PIN_INT_TYPE_MASK) >> GPIO_PIN_INT_TYPE_LSB)
#define GPIO_PIN_SOURCE_SET(x)              (((x) << GPIO_PIN_SOURCE_LSB) & GPIO_PIN_SOURCE_MASK)
//end of region1

#define GPIO_PIN0_ADDRESS                   0x28
#define GPIO_PIN0_CONFIG                        0x00000003
#define GPIO_PIN0_CONFIG_S                      11
#define GPIO_PIN0_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN0_WAKEUP_ENABLE_S               10
#define GPIO_PIN0_INT_TYPE                      0x00000007
#define GPIO_PIN0_INT_TYPE_S                    7
#define GPIO_PIN0_DRIVER                        BIT2
#define GPIO_PIN0_DRIVER_S                      2
#define GPIO_PIN0_SOURCE                        BIT0
#define GPIO_PIN0_SOURCE_S                      0

#define GPIO_PIN1_ADDRESS                   0x2c
#define GPIO_PIN1_CONFIG                        0x00000003
#define GPIO_PIN1_CONFIG_S                      11
#define GPIO_PIN1_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN1_WAKEUP_ENABLE_S               10
#define GPIO_PIN1_INT_TYPE                      0x00000007
#define GPIO_PIN1_INT_TYPE_S                    7
#define GPIO_PIN1_DRIVER                        BIT2
#define GPIO_PIN1_DRIVER_S                      2
#define GPIO_PIN1_SOURCE                        BIT0
#define GPIO_PIN1_SOURCE_S                      0

#define GPIO_PIN2_ADDRESS                   0x30
#define GPIO_PIN2_CONFIG                        0x00000003
#define GPIO_PIN2_CONFIG_S                      11
#define GPIO_PIN2_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN2_WAKEUP_ENABLE_S               10
#define GPIO_PIN2_INT_TYPE                      0x00000007
#define GPIO_PIN2_INT_TYPE_S                    7
#define GPIO_PIN2_DRIVER                        BIT2
#define GPIO_PIN2_DRIVER_S                      2
#define GPIO_PIN2_SOURCE                        BIT0
#define GPIO_PIN2_SOURCE_S                      0

#define GPIO_PIN3_ADDRESS                   0x34
#define GPIO_PIN3_CONFIG                        0x00000003
#define GPIO_PIN3_CONFIG_S                      11
#define GPIO_PIN3_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN3_WAKEUP_ENABLE_S               10
#define GPIO_PIN3_INT_TYPE                      0x00000007
#define GPIO_PIN3_INT_TYPE_S                    7
#define GPIO_PIN3_DRIVER                        BIT2
#define GPIO_PIN3_DRIVER_S                      2
#define GPIO_PIN3_SOURCE                        BIT0
#define GPIO_PIN3_SOURCE_S                      0

#define GPIO_PIN4_ADDRESS                   0x38
#define GPIO_PIN4_CONFIG                        0x00000003
#define GPIO_PIN4_CONFIG_S                      11
#define GPIO_PIN4_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN4_WAKEUP_ENABLE_S               10
#define GPIO_PIN4_INT_TYPE                      0x00000007
#define GPIO_PIN4_INT_TYPE_S                    7
#define GPIO_PIN4_DRIVER                        BIT2
#define GPIO_PIN4_DRIVER_S                      2
#define GPIO_PIN4_SOURCE                        BIT0
#define GPIO_PIN4_SOURCE_S                      0

#define GPIO_PIN5_ADDRESS                   0x3c
#define GPIO_PIN5_CONFIG                        0x00000003
#define GPIO_PIN5_CONFIG_S                      11
#define GPIO_PIN5_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN5_WAKEUP_ENABLE_S               10
#define GPIO_PIN5_INT_TYPE                      0x00000007
#define GPIO_PIN5_INT_TYPE_S                    7
#define GPIO_PIN5_DRIVER                        BIT2
#define GPIO_PIN5_DRIVER_S                      2
#define GPIO_PIN5_SOURCE                        BIT0
#define GPIO_PIN5_SOURCE_S                      0

#define GPIO_PIN6_ADDRESS                   0x40
#define GPIO_PIN6_CONFIG                        0x00000003
#define GPIO_PIN6_CONFIG_S                      11
#define GPIO_PIN6_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN6_WAKEUP_ENABLE_S               10
#define GPIO_PIN6_INT_TYPE                      0x00000007
#define GPIO_PIN6_INT_TYPE_S                    7
#define GPIO_PIN6_DRIVER                        BIT2
#define GPIO_PIN6_DRIVER_S                      2
#define GPIO_PIN6_SOURCE                        BIT0
#define GPIO_PIN6_SOURCE_S                      0

#define GPIO_PIN7_ADDRESS                   0x44
#define GPIO_PIN7_CONFIG                        0x00000003
#define GPIO_PIN7_CONFIG_S                      11
#define GPIO_PIN7_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN7_WAKEUP_ENABLE_S               10
#define GPIO_PIN7_INT_TYPE                      0x00000007
#define GPIO_PIN7_INT_TYPE_S                    7
#define GPIO_PIN7_DRIVER                        BIT2
#define GPIO_PIN7_DRIVER_S                      2
#define GPIO_PIN7_SOURCE                        BIT0
#define GPIO_PIN7_SOURCE_S                      0

#define GPIO_PIN8_ADDRESS                   0x48
#define GPIO_PIN8_CONFIG                        0x00000003
#define GPIO_PIN8_CONFIG_S                      11
#define GPIO_PIN8_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN8_WAKEUP_ENABLE_S               10
#define GPIO_PIN8_INT_TYPE                      0x00000007
#define GPIO_PIN8_INT_TYPE_S                    7
#define GPIO_PIN8_DRIVER                        BIT2
#define GPIO_PIN8_DRIVER_S                      2
#define GPIO_PIN8_SOURCE                        BIT0
#define GPIO_PIN8_SOURCE_S                      0

#define GPIO_PIN9_ADDRESS                   0x4c
#define GPIO_PIN9_CONFIG                        0x00000003
#define GPIO_PIN9_CONFIG_S                      11
#define GPIO_PIN9_WAKEUP_ENABLE                 BIT10
#define GPIO_PIN9_WAKEUP_ENABLE_S               10
#define GPIO_PIN9_INT_TYPE                      0x00000007
#define GPIO_PIN9_INT_TYPE_S                    7
#define GPIO_PIN9_DRIVER                        BIT2
#define GPIO_PIN9_DRIVER_S                      2
#define GPIO_PIN9_SOURCE                        BIT0
#define GPIO_PIN9_SOURCE_S                      0

#define GPIO_PIN10_ADDRESS                  0x50
#define GPIO_PIN10_CONFIG                       0x00000003
#define GPIO_PIN10_CONFIG_S                     11
#define GPIO_PIN10_WAKEUP_ENABLE                BIT10
#define GPIO_PIN10_WAKEUP_ENABLE_S              10
#define GPIO_PIN10_INT_TYPE                     0x00000007
#define GPIO_PIN10_INT_TYPE_S                   7
#define GPIO_PIN10_DRIVER                       BIT2
#define GPIO_PIN10_DRIVER_S                     2
#define GPIO_PIN10_SOURCE                       BIT0
#define GPIO_PIN10_SOURCE_S                     0

#define GPIO_PIN11_ADDRESS                  0x54
#define GPIO_PIN11_CONFIG                       0x00000003
#define GPIO_PIN11_CONFIG_S                     11
#define GPIO_PIN11_WAKEUP_ENABLE                BIT10
#define GPIO_PIN11_WAKEUP_ENABLE_S              10
#define GPIO_PIN11_INT_TYPE                     0x00000007
#define GPIO_PIN11_INT_TYPE_S                   7
#define GPIO_PIN11_DRIVER                       BIT2
#define GPIO_PIN11_DRIVER_S                     2
#define GPIO_PIN11_SOURCE                       BIT0
#define GPIO_PIN11_SOURCE_S                     0

#define GPIO_PIN12_ADDRESS                  0x58
#define GPIO_PIN12_CONFIG                       0x00000003
#define GPIO_PIN12_CONFIG_S                     11
#define GPIO_PIN12_WAKEUP_ENABLE                BIT10
#define GPIO_PIN12_WAKEUP_ENABLE_S              10
#define GPIO_PIN12_INT_TYPE                     0x00000007
#define GPIO_PIN12_INT_TYPE_S                   7
#define GPIO_PIN12_DRIVER                       BIT2
#define GPIO_PIN12_DRIVER_S                     2
#define GPIO_PIN12_SOURCE                       BIT0
#define GPIO_PIN12_SOURCE_S                     0

#define GPIO_PIN13_ADDRESS                  0x5c
#define GPIO_PIN13_CONFIG                       0x00000003
#define GPIO_PIN13_CONFIG_S                     11
#define GPIO_PIN13_WAKEUP_ENABLE                BIT10
#define GPIO_PIN13_WAKEUP_ENABLE_S              10
#define GPIO_PIN13_INT_TYPE                     0x00000007
#define GPIO_PIN13_INT_TYPE_S                   7
#define GPIO_PIN13_DRIVER                       BIT2
#define GPIO_PIN13_DRIVER_S                     2
#define GPIO_PIN13_SOURCE                       BIT0
#define GPIO_PIN13_SOURCE_S                     0

#define GPIO_PIN14_ADDRESS                  0x60
#define GPIO_PIN14_CONFIG                       0x00000003
#define GPIO_PIN14_CONFIG_S                     11
#define GPIO_PIN14_WAKEUP_ENABLE                BIT10
#define GPIO_PIN14_WAKEUP_ENABLE_S              10
#define GPIO_PIN14_INT_TYPE                     0x00000007
#define GPIO_PIN14_INT_TYPE_S                   7
#define GPIO_PIN14_DRIVER                       BIT2
#define GPIO_PIN14_DRIVER_S                     2
#define GPIO_PIN14_SOURCE                       BIT0
#define GPIO_PIN14_SOURCE_S                     0

#define GPIO_PIN15_ADDRESS                  0x64
#define GPIO_PIN15_CONFIG                       0x00000003
#define GPIO_PIN15_CONFIG_S                     11
#define GPIO_PIN15_WAKEUP_ENABLE                BIT10
#define GPIO_PIN15_WAKEUP_ENABLE_S              10
#define GPIO_PIN15_INT_TYPE                     0x00000007
#define GPIO_PIN15_INT_TYPE_S                   7
#define GPIO_PIN15_DRIVER                       BIT2
#define GPIO_PIN15_DRIVER_S                     2
#define GPIO_PIN15_SOURCE                       BIT0
#define GPIO_PIN15_SOURCE_S                     0

#define GPIO_SIGMA_DELTA_ADDRESS            0x68
#define SIGMA_DELTA_ENABLE                      BIT16
#define SIGMA_DELTA_ENABLE_S                    16
#define SIGMA_DELTA_PRESCALAR                   0x000000ff
#define SIGMA_DELTA_PRESCALAR_S                 8
#define SIGMA_DELTA_TARGET                      0x000000ff
#define SIGMA_DELTA_TARGET_S                    0

#define GPIO_RTC_CALIB_SYNC_ADDRESS         0x6c
#define RTC_CALIB_START                         BIT31
#define RTC_CALIB_START_S                       31
#define RTC_PERIOD_NUM                          0x000003ff
#define RTC_PERIOD_NUM_S                        0

#define GPIO_RTC_CALIB_VALUE_ADDRESS        0x70
#define RTC_CALIB_RDY                           BIT31
#define RTC_CALIB_RDY_S                         31
#define RTC_CALIB_RDY_REAL                      BIT30
#define RTC_CALIB_RDY_REAL_S                    30
#define RTC_CALIB_VALUE                         0x000fffff
#define RTC_CALIB_VALUE_S                       0

#define GPIO_REG_READ(reg)                  READ_PERI_REG(PERIPHS_GPIO_BASEADDR + reg)
#define GPIO_REG_WRITE(reg, val)            WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + reg, val)

#endif
