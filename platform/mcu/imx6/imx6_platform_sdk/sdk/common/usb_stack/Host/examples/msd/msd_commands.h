#ifndef __msc_commands_h__
#define __msc_commands_h__
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
* $FileName: msc_commands.h$
* $Version : 3.0.1.0$
* $Date    : Sep-23-2008$
*
* Comments:
*
*   This file contains mass storage class application types and definitions.
*
*END************************************************************************/

#include "host_dev_list.h"
#include "host_common.h"

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS      16
#define  MAX_FRAME_SIZE                1024
#define  HOST_CONTROLLER_NUMBER        0
#if (MQX_CPU == 51129) 
    #define BUFF_IN_SIZE                   0x40c
#else    
    #define BUFF_IN_SIZE                   0x40c
#endif    
#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
#define ERASE_BLOCK_SIZE	1 	/* Unknown erasable block size*/

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/

typedef struct device_struct
{
   uint_32                          dev_state;  /* Attach/detach state */
   PIPE_BUNDLE_STRUCT_PTR           pbs;        /* Device & pipes */
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   CLASS_CALL_STRUCT                class_intf; /* Class-specific info */

} DEVICE_STRUCT,  _PTR_ DEVICE_STRUCT_PTR;


/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void    usb_host_mass_device_event
                  (
                  _usb_device_instance_handle,
                  _usb_interface_descriptor_handle,
                  uint_32
                  );

static void    usb_host_mass_ctrl_callback
                  (
                  _usb_pipe_handle,
                  pointer,
                  uchar_ptr,
                  uint_32,
                  uint_32
                  );


void callback_bulk_pipe (USB_STATUS status,
      pointer p1,
      pointer  p2,
      uint_32 buffer_length
      );

static void usb_host_mass_test_storage();

#ifdef __cplusplus
}
#endif


#endif

/* EOF */





