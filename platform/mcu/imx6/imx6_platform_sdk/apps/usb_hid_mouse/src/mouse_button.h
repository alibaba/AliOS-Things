/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file mouse_button.h
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief The file contains Macro's and functions needed by the Mouse
 *        Application
 *
 *****************************************************************************/


#ifndef _MOUSE_BUTTON_H
#define _MOUSE_BUTTON_H

#include "types.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define  CONTROLLER_ID   (0)   /* ID to identify USB CONTROLLER */

/* #define UP_LEFT   This macro enables up and left mouse movement via PTG2-3*/
#ifdef UP_LEFT
#define SHIFT_VAL  (0xF8)    /* send -8 for up and left movement */
#else
#define SHIFT_VAL  (0x08)    /* send +8 for down and right movement */
#endif

#define  MOUSE_BUFF_SIZE   (4)   /* report buffer size */
#define  REQ_DATA_SIZE     (1)
#define  KBI_STAT_MASK     (0x0F)
#define  LEFT_CLICK        (0x01) /* Left click */
#define  RIGHT_CLICK       (0x02) /* Right click */
#define  MOVE_LEFT_RIGHT   (0x04) /* Left-Right movement*/
#define  MOVE_UP_DOWN      (0x08) /* Up-Down movement*/

// X, Y displacement info
#define MAX_DISP	300

/*****************************************************************************
 * Global variables
 *****************************************************************************/
extern uint_8 kbi_stat; /*records the status of the buttons (PTG0-PTG3)
                                   used to emulate mouse */

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern void TestApp_Init(void);
extern void TestApp_Task(void);
#endif
