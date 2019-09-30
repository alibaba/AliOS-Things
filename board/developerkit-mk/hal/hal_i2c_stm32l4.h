/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_I2C_STM32L4_H
#define __HAL_I2C_STM32L4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"  
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define AOS_PORT_I2C1                      1
#define AOS_PORT_I2C2                      2
#define AOS_PORT_I2C3                      3
#define AOS_PORT_I2C4                      4

/******************************** I2C2 configure *****************************/
#define I2C2_INSTANCE                      I2C2
#define I2C2_TIMING                        0x10909CEC
#define I2C2_OWN_ADDRESS1                  0x00
#define I2C2_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C2_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C2_OWNADDRESS2                   0x00
#define I2C2_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C2_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C2_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/******************************** I2C3 configure *****************************/
#define I2C3_INSTANCE                      I2C3
#define I2C3_TIMING                        0x10909CEC
#define I2C3_OWN_ADDRESS1                  0x00
#define I2C3_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C3_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C3_OWNADDRESS2                   0x00
#define I2C3_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C3_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C3_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/******************************** I2C4 configure *****************************/
#define I2C4_INSTANCE                      I2C4
#define I2C4_TIMING                        0x10909CEC
#define I2C4_OWN_ADDRESS1                  0x00
#define I2C4_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C4_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C4_OWNADDRESS2                   0x00
#define I2C4_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C4_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C4_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE


#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_STM32L4_H */
