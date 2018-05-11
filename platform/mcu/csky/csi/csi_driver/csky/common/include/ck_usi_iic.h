/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     ck_usi_iic.h
 * @brief    header File for IIC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_USI_IIC_H_
#define _CK_USI_IIC_H_
#include "soc.h"

#define CK_USI_I2C_MODE_SLAVE   0x00
#define CK_USI_I2C_MODE_MATER   0x01

#define CK_USI_I2CM_CTRL_HS_MODE        0x04

#define CK_USI_I2CM_CTRL_ADDR_MODE      0x1e
#define CK_USI_I2CM_CTRL_ADDR7_MODE     0x00
#define CK_USI_I2CM_CTRL_ADDR10_MODE    0x01

#define CK_USI_I2CM_CTRL_SBYTE_EN       0x08

#define CK_USI_I2C_STA_SLAVE_WORK       0x100
#define CK_USI_I2C_STA_MASTER_WORK      0x1

typedef enum {
    CK_IIC_FASTSPEED     = 1,
    CK_IIC_HIGHSPEED     = 2
} CKENUM_IIC_SPEED;

enum i2c_state_e {
    IIC_STATE_NONE = 0,          /* Send start + (first part of) address. */
    IIC_STATE_DATASEND,          /* Send data. */
    IIC_STATE_WFDATA,            /* Wait for data. */
    IIC_STATE_WFSTOPSENT,        /* Wait for STOP to have been transmitted. */
    IIC_STATE_DONE,              /* Transfer completed successfully. */
    IIC_STATE_SEND_DONE,         /* send completed successfully. */
    IIC_STATE_RECV_DONE,         /* receive completed successfully. */
    IIC_STATE_ERROR              /* Transfer error. */
};

#endif /* _CK_USI_IIC_H_ */
