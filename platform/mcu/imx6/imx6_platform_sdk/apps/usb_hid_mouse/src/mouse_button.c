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
 * (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file mouse_button.c
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief  The file emulates a mouse with buttons
 *         4 buttons are used on the demo board for the emulation
 *         PTG0--------- for left click(For JS16 PTG0 is inactive)
 *         PTG1--------- for right click
 *         PTG2--------- if macro "UP_LEFT" is defined mouse moves "left"
 *                       else mouse moves "right"
 *         PTG3--------- if macro "UP_LEFT" is defined mouse moves "up"
 *                       else mouse moves "down"
 *
 *   NOTE: key pressed  == LOGIC 0
 *         key released == LOGIC 1
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <math.h>
#include "sdk.h"
#include "platform_init.h"
#include "hidef.h"          /* for EnableInterrupts macro */
#include "derivative.h"     /* include peripheral declarations */
#include "types.h"          /* User Defined Data Types */
#include "usb_hid.h"        /* USB HID Class Header File */
#include "mouse_button.h"   /* Mouse Application Header File */
#if HIGH_SPEED_DEVICE
//#include "usbhs_common.h"
//#include "usbhs_host_device.h"
#endif
#include "timer/timer.h"

/* skip the inclusion in dependency state */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);

/****************************************************************************
 * Global Variables - None
 ****************************************************************************/


/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void USB_App_Callback(
    uint_8 controller_ID,
    uint_8 event_type,
    void* val);
static uint_8 USB_App_Param_Callback(
    uint_8 request,
    uint_16 value,
    uint_8_ptr* data,
    USB_PACKET_SIZE* size);
static void Emulate_Mouse_WithButton(void);

/*****************************************************************************
 * Local Variables
 *****************************************************************************/

uint_8 kbi_stat;

static boolean mouse_init=FALSE;            /* Application Init Flag */
static uint_8 rpt_buf[MOUSE_BUFF_SIZE];     /* Mouse Event Report Buffer
                                               Key press */
static uint_8 null_buf[MOUSE_BUFF_SIZE];    /* Mouse Event Report Buffer
                                               Key Release */
static uint_8 g_app_request_params[2];      /* for get/set idle and protocol
                                               requests*/
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

void fillBuffer(uint32_t *buffer) {

	static int init=1;
	static float angle, angle_disp;
	const float PI = 3.1415926535897932384626433832795f;

	static int x, x_1;
	static int y, y_1;
	char x_disp, y_disp;

	if(init == 1){					// if this is the first time we call this routine
		angle = 0;
		x_1 = x = MAX_DISP / 2;
		y_1 = y = 0;
		angle_disp = PI / 180;
		init = 0;
	}

	//! - Put the data in the transfer descriptor buffers
	// Calculate pointer positions to make the cursor move in a circle
	//! Calculate the  mouse displacement values to fill the buffer
	x = (int) (cosf(angle) * MAX_DISP / 2);
	y = (int) (sinf(angle) * MAX_DISP / 2);

	x_disp = x - x_1;
	y_disp = y - y_1;

	x_1 = x;
	y_1 = y;

	angle = angle + angle_disp;
	if (angle > 2 * PI) {
		angle -= 2 * PI;
	}

	//! Put the data in the buffer
	*buffer = (0 | ((x_disp & 0xFF) << 16) | ((y_disp & 0xFF) << 8));
}

/******************************************************************************
 *
 *    @name       Emulate_Mouse_WithButton
 *
 *    @brief      This function gets the input from mouse movement, the mouse
 *                will move if the any button are pushed,otherwise USB gets NAK
 *
 *    @param      None
 *
 *    @return     None
 *
 *****************************************************************************
 * This function sends the mouse data depending on which key was pressed on
 * the board
 *****************************************************************************/
static void Emulate_Mouse_WithButton(void)
{
#if 0
    if(kbi_stat > 0)
    {
        switch(kbi_stat & KBI_STAT_MASK)
        {
           case LEFT_CLICK : /* PTG0 (left click) is pressed */
               rpt_buf[0] = LEFT_CLICK;
               rpt_buf[1] = 0x00;
               rpt_buf[2] = 0x00;
               break;

           case RIGHT_CLICK : /* PTG1 (right click)   is pressed */
               rpt_buf[0] = RIGHT_CLICK;
               rpt_buf[1] = 0x00;
               rpt_buf[2] = 0x00;

               break;

           case MOVE_LEFT_RIGHT :  /* PTG2 (left
                                      or right movement--depends on
                                      UP_LEFT macro) is pressed*/
               rpt_buf[1] = SHIFT_VAL;
               rpt_buf[0] = 0x00;
               rpt_buf[2] = 0x00;
               break;

           case MOVE_UP_DOWN :          /* PTG3 (up or down
                                          movement--depends on
                                          UP_LEFT macro) is pressed*/
               rpt_buf[2] = SHIFT_VAL;
               rpt_buf[1] = 0x00;
               rpt_buf[0] = 0x00;
               break;
           default:break; /* otherwise */
        }
        kbi_stat = 0x00; /* reset status after servicing interrupt*/
        (void)USB_Class_HID_Send_Data(CONTROLLER_ID,HID_ENDPOINT,rpt_buf,
                                        MOUSE_BUFF_SIZE);

        if(rpt_buf[0])
        {
            /* required to send Click Release for Click Press Events */
            (void)USB_Class_HID_Send_Data(CONTROLLER_ID,HID_ENDPOINT,null_buf,
                                        MOUSE_BUFF_SIZE);

        }
    }
#endif // 0
    
    fillBuffer((uint32_t *)&rpt_buf);
    USB_Class_HID_Send_Data(CONTROLLER_ID, HID_ENDPOINT, rpt_buf, MOUSE_BUFF_SIZE);
    
    return;
}


/******************************************************************************
 *
 *    @name        USB_App_Callback
 *
 *    @brief       This function handles the callback
 *
 *    @param       controller_ID    : Controller ID
 *    @param       event_type       : value of the event
 *    @param       val              : gives the configuration value
 *
 *    @return      None
 *
 *****************************************************************************
 * This function is called from the class layer whenever reset occurs or enum
 * is complete. after the enum is complete this function sets a variable so
 * that the application can start
 *****************************************************************************/
void USB_App_Callback(
      uint_8 controller_ID,/* [IN] Controller ID */
      uint_8 event_type,   /* [IN] value of the event*/
      void* val            /* [IN] gives the configuration value*/
)
{
    UNUSED (controller_ID)
    UNUSED (val)
    if((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED))
    {
        mouse_init=FALSE;
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {   /* if enumeration is complete set mouse_init
           so that application can start */
        mouse_init=TRUE;
    }

    return;
}

/******************************************************************************
 *
 *    @name        USB_App_Param_Callback
 *
 *    @brief       This function handles callbacks for USB HID Class request
 *
 *    @param       request  : request type
 *    @param       value    : give report type and id
 *    @param       data     : pointer to the data
 *    @param       size     : size of the transfer
 *
 *    @return      status
 *                 USB_OK  :  if successful
 *                 else return error
 *
 *****************************************************************************
 * This function is called whenever a HID class request is received. This
 * function handles these class requests
 *****************************************************************************/
uint_8 USB_App_Param_Callback(
      uint_8 request,        /* [IN] request type */
      uint_16 value,         /* [IN] report type and ID */
      uint_8_ptr* data,      /* [OUT] pointer to the data */
      USB_PACKET_SIZE* size  /* [OUT] size of the transfer */
)
{
    uint_8 status = USB_OK;
    uint_8 direction =  (uint_8)((request & USB_HID_REQUEST_DIR_MASK) >>3);
    uint_8 index = (uint_8)((request - 2) & USB_HID_REQUEST_TYPE_MASK);
                                              /* index == 0 for get/set idle,
                                            index == 1 for get/set protocol */

    *size = 0;
    /* handle the class request */
    switch(request)
    {
        case USB_HID_GET_REPORT_REQUEST :
            *data = &rpt_buf[0]; /* point to the report to send */
            *size = MOUSE_BUFF_SIZE; /* report size */
            break;

        case USB_HID_SET_REPORT_REQUEST :
            for(index = 0; index < MOUSE_BUFF_SIZE ; index++)
            {   /* copy the report sent by the host */
                rpt_buf[index] = *(*data + index);
            }
            break;

        case USB_HID_GET_IDLE_REQUEST :
            /* point to the current idle rate */
            *data = &g_app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;

        case USB_HID_SET_IDLE_REQUEST :
            /* set the idle rate sent by the host */
            g_app_request_params[index] =(uint_8)((value & MSB_MASK) >>
                                                  HIGH_BYTE_SHIFT);
            break;

        case USB_HID_GET_PROTOCOL_REQUEST :
            /* point to the current protocol code
               0 = Boot Protocol
               1 = Report Protocol*/
            *data = &g_app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;

        case USB_HID_SET_PROTOCOL_REQUEST :
              /* set the protocol sent by the host
                 0 = Boot Protocol
                 1 = Report Protocol*/
              g_app_request_params[index] = (uint_8)(value);
              break;
    }

    return status;
}

/******************************************************************************
 *
 *   @name        TestApp_Init
 *
 *   @brief       This function is the entry for Mouse Application
 *
 *   @param       None
 *
 *   @return      None
 *
 *****************************************************************************
 * This function starts the Mouse Application
 *****************************************************************************/

void TestApp_Init(void)
{
    uint_8   error;

    rpt_buf[3] = 0x00; /* always zero */

    *((uint_32_ptr)rpt_buf) = 0;
    *((uint_32_ptr)null_buf) = 0;
    DisableInterrupts;
    #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
     usb_int_dis();
    #endif	

    /* Initialize the USB interface */
    error = USB_Class_HID_Init(CONTROLLER_ID, USB_App_Callback, NULL,
                            USB_App_Param_Callback);

    EnableInterrupts;
    #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
     usb_int_en();
    #endif	
}

/******************************************************************************
 *
 *   @name        TestApp_Task
 *
 *   @brief       Application task function. It is called from the main loop
 *
 *   @param       None
 *
 *   @return      None
 *
 *****************************************************************************
 * Application task function. It is called from the main loop
 *****************************************************************************/
void TestApp_Task(void)
{
	/* call the periodic task function */
	USB_Class_HID_Periodic_Task();

	if(mouse_init) /*check whether enumeration is
                                        complete or not */
	{
		/* run the button emulation code */
		Emulate_Mouse_WithButton();
	}
}

void main(void)
{
    platform_init();

    TestApp_Init();
    
    while (true)
    {
        TestApp_Task();
        // generate a mouse move every 5 ms
        hal_delay_us(5000);
    }
}

