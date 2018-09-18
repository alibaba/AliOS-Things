#ifndef __hidmouse_h__
#define __hidmouse_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: hidmouse.h$
* $Version : 3.4.3.0$
* $Date    : Sep-3-2009$
*
* Comments:
*
*   This file contains mouse-application types and definitions.
*
*END************************************************************************/

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif


/***************************************
**
** Application-specific definitions
*/

/* Used to initialize USB controller */
#define MAX_FRAME_SIZE           1024
#define HOST_CONTROLLER_NUMBER      0
#define HID_MOUSE_BUFFER_SIZE               4

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_SETTING_PROTOCOL       (5)
#define  USB_DEVICE_INUSE                  (6)
#define  USB_DEVICE_DETACHED               (7)
#define  USB_DEVICE_OTHER                  (8)



/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/



/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#if 0
	void otg_service(_usb_otg_handle, uint_32);
#endif

extern void usb_host_hid_mouse_recv_callback(_usb_pipe_handle, pointer, uchar_ptr, uint_32,
   uint_32);
extern void usb_host_hid_mouse_ctrl_callback(_usb_pipe_handle, pointer, uchar_ptr, uint_32,
   uint_32);
extern void usb_host_hid_mouse_event(_usb_device_instance_handle,
   _usb_interface_descriptor_handle, uint_32);

extern void Mouse_Task(void);

#ifdef __cplusplus
}
#endif


#endif

/* EOF */





