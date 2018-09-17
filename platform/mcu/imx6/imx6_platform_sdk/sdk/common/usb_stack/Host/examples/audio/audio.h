#ifndef __audio_h__
#define __audio_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: audio.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains audio types and definitions.
*
*END************************************************************************/

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif
#include "usb_host_audio.h"
#include "usbprv_host.h"

/***************************************
**
** Application-specific definitions
**
****************************************/

/* Used to initialize USB controller */
#define MAX_FRAME_SIZE           1024
#define HOST_CONTROLLER_NUMBER      0

#define USB_EVENT_CTRL 				0x01
#define USB_EVENT_RECEIVED_DATA  	0x02
#define USB_EVENT_SEND_DATA  		0x04

#define MAX_ISO_PACKET_SIZE			48

#define IN_DEVICE         			0x00
#define OUT_DEVICE         			0x01
#define UNDEFINE_DEVICE         	0xFF

#define USB_TERMINAL_TYPE			0x01
#define INPUT_TERMINAL_TYPE			0x02
#define OUTPUT_TERMINAL_TYPE		0x03

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/


typedef struct audio_device_struct {
   uint_32                          DEV_STATE;  /* Attach/detach state */
   _usb_device_instance_handle      DEV_HANDLE;
   _usb_interface_descriptor_handle INTF_HANDLE;
   CLASS_CALL_STRUCT                CLASS_INTF; /* Class-specific info */
} AUDIO_CONTROL_DEVICE_STRUCT,  _PTR_ AUDIO_CONTROL_DEVICE_STRUCT_PTR;

typedef struct data_device_struct {
   uint_32                          DEV_STATE;  /* Attach/detach state */
   _usb_device_instance_handle      DEV_HANDLE;
   _usb_interface_descriptor_handle INTF_HANDLE;
   CLASS_CALL_STRUCT                CLASS_INTF; /* Class-specific info */
} AUDIO_STREAM_DEVICE_STRUCT,  _PTR_ AUDIO_STREAM_DEVICE_STRUCT_PTR;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void usb_host_audio_control_event(_usb_device_instance_handle,
   _usb_interface_descriptor_handle, uint_32);
void usb_host_audio_stream_event(_usb_device_instance_handle,
   _usb_interface_descriptor_handle, uint_32);

void usb_host_audio_tr_callback(_usb_pipe_handle ,pointer ,uchar_ptr ,uint_32 ,uint_32 );

void usb_host_audio_mute_ctrl_callback(_usb_pipe_handle,pointer,uchar_ptr,uint_32,uint_32);

void usb_host_audio_interrupt_callback(_usb_pipe_handle,pointer,uchar_ptr,uint_32,uint_32);



#ifdef __cplusplus
}
#endif


#endif

/* EOF */
