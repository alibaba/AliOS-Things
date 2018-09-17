#ifndef __bm_host_h__
#define __bm_host_h__
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
 * $FileName: ehci_host.h$
 * $Version : 3.4.9.0$
 * $Date    : Sep-23-2009$
 *
 * Comments:
 *
 *   This file contains generic and OHCI-specific implementations of USB
 *   interfaces
 *
 *END************************************************************************/

#include "host_common.h"
#include "usbprv_host.h"

#if HIGH_SPEED_DEVICE

#define EHCI_EVENT_ISR       1

typedef enum 
{
	TR_MSG_UNKNOWN,     /* unknown - not used   */
	TR_MSG_SETUP,       /* setup transaction   */
	TR_MSG_SEND,        /* send trnasaction    */
	TR_MSG_RECV         /* receive transaction */
} TR_MSG_TYPE;

/* atom transaction message */
typedef struct 
{
	TR_MSG_TYPE type;                       /* transaction type */
#ifdef __GNUC__
	uchar       reserved[3];
#endif
	PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc;   /* pointer to pipe descriptor */
	PIPE_TR_STRUCT_PTR pipe_tr;             /* pointer to transaction */
} TR_MSG_STRUCT, _PTR_ TR_MSG_STRUCT_PTR;

/*
 **
 ** Low-level function list structure for USB
 **
 ** This is the structure used to store chip specific functions to be called 
 ** by generic layer
 */
typedef struct usb_host_callback_functions_struct
{
	/* The Host/Device init function */
	USB_STATUS (_CODE_PTR_ HOST_INIT)(_usb_host_handle);

	/* The function to shutdown the host/device */
	USB_STATUS (_CODE_PTR_ HOST_SHUTDOWN)(_usb_host_handle);

	/* The function to send data */
	USB_STATUS (_CODE_PTR_ HOST_SEND)(_usb_host_handle, struct pipe_descriptor_struct _PTR_, struct pipe_tr_struct _PTR_);

	/* The function to send setup data */
	USB_STATUS (_CODE_PTR_ HOST_SEND_SETUP)(_usb_host_handle, struct pipe_descriptor_struct _PTR_, struct pipe_tr_struct _PTR_);

	/* The function to receive data */
	USB_STATUS (_CODE_PTR_ HOST_RECV)(_usb_host_handle, struct pipe_descriptor_struct _PTR_, struct pipe_tr_struct _PTR_);

	/* The function to cancel the transfer */
	USB_STATUS (_CODE_PTR_ HOST_CANCEL)(_usb_host_handle, struct pipe_descriptor_struct _PTR_, struct pipe_tr_struct _PTR_);

	/* The function for USB bus control */
	USB_STATUS (_CODE_PTR_ HOST_BUS_CONTROL)(_usb_host_handle, uint_8);

	uint_32 (_CODE_PTR_ HOST_ALLOC_BANDWIDTH)(_usb_host_handle, struct pipe_descriptor_struct _PTR_);

	USB_STATUS (_CODE_PTR_ HOST_FREE_CONTROLLER_RESOURCE)(_usb_host_handle, struct pipe_descriptor_struct _PTR_);

	uint_32 (_CODE_PTR_ HOST_EHCI_GET_FRAME_NUM)(_usb_host_handle);   

	uint_32 (_CODE_PTR_ HOST_EHCI_GET_MICRO_FRAME_NUM)(_usb_host_handle);   

	USB_STATUS (_CODE_PTR_ HOST_OPEN_PIPE)(_usb_host_handle, struct pipe_descriptor_struct _PTR_);

	USB_STATUS (_CODE_PTR_ HOST_UPDATE_MAX_PACKET_SIZE)(_usb_host_handle, struct pipe_descriptor_struct _PTR_);

	USB_STATUS (_CODE_PTR_ HOST_UPDATE_DEVICE_ADDRESS)(_usb_host_handle, struct pipe_descriptor_struct _PTR_);

} USB_HOST_CALLBACK_FUNCTIONS_STRUCT, _PTR_ USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR;

extern const USB_HOST_CALLBACK_FUNCTIONS_STRUCT _bsp_usb_host_callback_table;

#endif // HIGH_SPEED_DEVICE
#endif
