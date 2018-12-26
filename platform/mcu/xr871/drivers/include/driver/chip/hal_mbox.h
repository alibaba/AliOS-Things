/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_MBOX_H_
#define _DRIVER_CHIP_HAL_MBOX_H_

#include "driver/chip/hal_def.h"
#include "driver/chip/hal_global.h"

#ifdef __cplusplus
extern "C" {
#endif

/* queue index */
typedef enum {
    MBOX_QUEUE_0   = 0,
    MBOX_QUEUE_1   = 1,
    MBOX_QUEUE_2   = 2,
    MBOX_QUEUE_3   = 3,
    MBOX_QUEUE_4   = 4,
    MBOX_QUEUE_5   = 5,
    MBOX_QUEUE_6   = 6,
    MBOX_QUEUE_7   = 7,
    MBOX_QUEUE_NUM = 8
} MBOX_Queue;

typedef struct {
    __IO uint32_t CTRL[2];                      /* offset: 0x0000 */
         uint32_t RESERVED0[14];
    __IO uint32_t IRQ0_EN;                      /* offset: 0x0040 */
         uint32_t RESERVED1[3];
    __IO uint32_t IRQ0_STATUS;                  /* offset: 0x0050 */
         uint32_t RESERVED2[3];
    __IO uint32_t IRQ1_EN;                      /* offset: 0x0060 */
         uint32_t RESERVED3[3];
    __IO uint32_t IRQ1_STATUS;                  /* offset: 0x0070 */
         uint32_t RESERVED4[35];
    __I  uint32_t FIFO_STATUS[MBOX_QUEUE_NUM];  /* offset: 0x0100 */
         uint32_t RESERVED5[8];
    __I  uint32_t MSG_STATUS[MBOX_QUEUE_NUM];   /* offset: 0x0140 */
         uint32_t RESERVED6[8];
    __IO uint32_t MSG[MBOX_QUEUE_NUM];          /* offset: 0x0180 */
         uint32_t RESERVED7[8];
    __IO uint32_t DEBUG;                        /* offset: 0x01C0 */
} MBOX_T;

#define MBOX_A ((MBOX_T *)MBOX_A_BASE)          /* address: 0x40009000 */
#define MBOX_N ((MBOX_T *)MBOX_N_BASE)          /* address: 0xA0003000 */

/* user
 *   - MBOX_A: app core is MBOX_USER0, net core is MBOX_USER1
 *   - MBOX_N: app core is MBOX_USER1, net core is MBOX_USER0
 */
typedef enum {
	MBOX_USER0	  = 0,
	MBOX_USER1	  = 1,
	MBOX_USER_NUM = 2
} MBOX_User;

/* direction */
typedef enum {
	MBOX_DIR_RX	 = 0,
	MBOX_DIR_TX	 = 1,
	MBOX_DIR_NUM = 2
} MBOX_Direction;

/* queue size */
#define MBOX_QUEUE_SIZE		4

/*
 * bit field definition of MBOX->FIFO_STATUS
 */
#define MBOX_QUEUE_FULL_BIT			HAL_BIT(0)

/*
 * bit field definition of MBOX->MSG_STATUS
 */
#define MBOX_QUEUE_MSG_NUM_SHIFT	0
#define MBOX_QUEUE_MSG_NUM_VMASK	0x7

/* MBOX->MSG */

/*
 * bit field definition of MBOX->DEBUG
 */
#define MBOX_DEBUG_FIFO_CTRL_SHIFT	8
#define MBOX_DEBUG_EN_BIT			HAL_BIT(0)

/******************************************************************************/

void HAL_MBOX_Init(MBOX_T *mbox);
void HAL_MBOX_DeInit(MBOX_T *mbox);
void HAL_MBOX_EnableIRQ(MBOX_T *mbox);
void HAL_MBOX_DisableIRQ(MBOX_T *mbox);
void HAL_MBOX_QueueInit(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir);
void HAL_MBOX_QueueDeInit(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir);
void HAL_MBOX_QueueEnableIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir);
void HAL_MBOX_QueueDisableIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir);
int HAL_MBOX_QueueIsPendingIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir);
void HAL_MBOX_QueueClrPendingIRQ(MBOX_T *mbox, MBOX_User user, MBOX_Queue queue, MBOX_Direction dir);

int HAL_MBOX_QueueIsFull(MBOX_T *mbox, MBOX_Queue queue);
uint32_t HAL_MBOX_QueueGetMsgNum(MBOX_T *mbox, MBOX_Queue queue);
void HAL_MBOX_QueuePutMsg(MBOX_T *mbox, MBOX_Queue queue, uint32_t msg);
uint32_t HAL_MBOX_QueueGetMsg(MBOX_T *mbox, MBOX_Queue queue);

__STATIC_INLINE void HAL_MBOX_QueueEnableFIFO(MBOX_T *mbox, MBOX_Queue queue)
{
	HAL_CLR_BIT(mbox->DEBUG, HAL_BIT((uint32_t)queue + MBOX_DEBUG_FIFO_CTRL_SHIFT));
}

__STATIC_INLINE void HAL_MBOX_QueueDisableFIFO(MBOX_T *mbox, MBOX_Queue queue)
{
	HAL_SET_BIT(mbox->DEBUG, HAL_BIT((uint32_t)queue + MBOX_DEBUG_FIFO_CTRL_SHIFT));
}

__STATIC_INLINE void HAL_MBOX_EnableDebugMode(MBOX_T *mbox)
{
	HAL_SET_BIT(mbox->DEBUG, MBOX_DEBUG_EN_BIT);
}

__STATIC_INLINE void HAL_MBOX_DisableDebugMode(MBOX_T *mbox)
{
	HAL_CLR_BIT(mbox->DEBUG, MBOX_DEBUG_EN_BIT);
}

__STATIC_INLINE int HAL_MBOX_IsPmPatchEnabled(void)
{
	return (HAL_GlobalGetChipVer() <= 0xB);
}

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_MBOX_H_ */
