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
 * @file     ck_usi_spi.h
 * @brief    header file for usi spi
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_USI_SPI_H_
#define _CK_USI_SPI_H_

#include "ck_usi.h"

typedef enum {
    CKENUM_SPI_TXRX = 0,
    CKENUM_SPI_TX   = 1,
    CKENUM_SPI_RX   = 2,
    CKENUM_SPI_EERX = 3
} CKENUM_SPI_MODE;

typedef enum {
    CKENUM_SPI_CLOCK_POLARITY_LOW   = 0,
    CKENUM_SPI_CLOCK_POLARITY_HIGH  = 1
} CKENUM_SPI_POLARITY;

typedef enum {
    CKENUM_SPI_CLOCK_PHASE_MIDDLE   = 0,
    CKENUM_SPI_CLOCK_PHASE_START    = 1
} CKENUM_SPI_PHASE;

typedef enum {
    CKENUM_SPI_DATASIZE_4   = 3,
    CKENUM_SPI_DATASIZE_5   = 4,
    CKENUM_SPI_DATASIZE_6   = 5,
    CKENUM_SPI_DATASIZE_7   = 6,
    CKENUM_SPI_DATASIZE_8   = 7,
    CKENUM_SPI_DATASIZE_9   = 8,
    CKENUM_SPI_DATASIZE_10  = 9,
    CKENUM_SPI_DATASIZE_11  = 10,
    CKENUM_SPI_DATASIZE_12  = 11,
    CKENUM_SPI_DATASIZE_13  = 12,
    CKENUM_SPI_DATASIZE_14  = 13,
    CKENUM_SPI_DATASIZE_15  = 14,
    CKENUM_SPI_DATASIZE_16  = 15
} CKENUM_SPI_DATAWIDTH;

#define CK_USI_SPI_MODE_MASTER  0x1
#define CK_USI_SPI_MODE_SLAVE   0x0
/* some register bits macro for spi control register */
#define CK_USI_SPI_CTRL_CPOL        (1 << 7)
#define CK_USI_SPI_CTRL_CPHA        (1 << 6)
#define CK_USI_SPI_CTRL_TMODE       (3 << 4)
#define CK_USI_SPI_CTRL_TMODE_TXRX  (0 << 4)
#define CK_USI_SPI_CTRL_TMODE_TX    (1 << 4)
#define CK_USI_SPI_CTRL_TMODE_RX    (2 << 4)
#define CK_USI_SPI_CTRL_DATA_SIZE   0xfff0

#define SPI_INITIALIZED         ((uint8_t)(1U))             // SPI initalized
#define SPI_POWERED             ((uint8_t)(1U<< 1))         // SPI powered on
#define SPI_CONFIGURED          ((uint8_t)(1U << 2))        // SPI configured
#define SPI_DATA_LOST           ((uint8_t)(1U << 3))        // SPI data lost occurred
#define SPI_MODE_FAULT          ((uint8_t)(1U << 4))        // SPI mode fault occurred

#endif /* _CK_USI_SPI_H_ */
