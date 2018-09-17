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
* $FileName: host_main.c$
* $Version : 3.4.19.0$
* $Date    : Sep-22-2009$
*
* Comments:
*
*   This file contains the USB Host API specific main functions.
*
*END************************************************************************/
#include "hostapi.h"
#include "user_config.h"
#if HIGH_SPEED_DEVICE
#include "ehci_host.h"
#include "ehci_usbprv_host.h"
#else
#include "khci.h"
#include "usbprv_host.h"
#endif
#include "usb_bsp.h"
#include "host_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem_util.h"

/* this should be global for access by the OTG source code */
USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;

volatile USB_ISO_PACKET_DESC_POOL_STRUCT usb_host_iso_packet_desc_pool;

static PIPE_TR_STRUCT_PTR _usb_host_new_tr_element (_usb_pipe_handle);
extern const USB_HOST_CALLBACK_FUNCTIONS_STRUCT _bsp_usb_host_callback_table;
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_init_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_init_call_interface routine is used to initialize device
*  drivers when the USB host stack is initialized.  Device drivers may
*  perform device register, interrupt, and internal data structure
*  initialization.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_init_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
#ifdef USB_HOST_INCLUDE_TASK_SUPPORT
   error = _usb_rtos_host_init ();
   
   if (error != USB_OK)
   {
      return USB_log_error(__FILE__,__LINE__,error);
   }
#endif
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_INIT != NULL)
   {
      error = temp_ptr->HOST_INIT (handle);
   }

#ifdef USB_HOST_INCLUDE_TASK_SUPPORT
   if (error != USB_OK)
   {
      error = _usb_rtos_host_shutdown();
   }
#endif

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_shutdown_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_shutdown_call_interface routine is used to shutdown device
*  drivers when the USB host stack is shutdown.  Device drivers may
*  perform host controller resets and/or free internal data structures.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_shutdown_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_SHUTDOWN != NULL)
   {
      error = temp_ptr->HOST_SHUTDOWN (handle);
   }
   
   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_send_call_interface routine is used to send non-control data
*  to a USB device.  Device drivers may allocate internal data structures
*  and modify registers to initiate a bus transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_send_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct pipe_tr_struct _PTR_ pipe_tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_SEND != NULL)
   {
      error = temp_ptr->HOST_SEND (handle, pipe_descr_ptr, pipe_tr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_setup_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_send_setup_call_interface routine is used to send control
*  data to a USB device.  Device drivers may allocate internal data
*  structures and modify registers to initiate a bus transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_send_setup_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct pipe_tr_struct _PTR_ pipe_tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_SEND_SETUP != NULL)
   {
      error = temp_ptr->HOST_SEND_SETUP (handle, pipe_descr_ptr, pipe_tr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_recv_data_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_recv_data_call_interface routine is used to receive non-
*  control data from a USB device.  Device drivers may allocate internal
*  data structures and modify registers to initiate a bus transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_recv_data_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct pipe_tr_struct _PTR_ pipe_tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_RECV != NULL)
   {
      error = temp_ptr->HOST_RECV (handle, pipe_descr_ptr, pipe_tr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_cancel_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_cancel_call_interface routine is used to cancel pending
*  transactions.  Device drivers may update/free internal data structures
* and/or modify registers to cancel the transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_cancel_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct pipe_tr_struct _PTR_ pipe_tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_CANCEL != NULL)
   {
      error = temp_ptr->HOST_CANCEL (handle, pipe_descr_ptr, pipe_tr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_bus_control_data_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_bus_control_data_call_interface routine is used to perform
*  low-level operations on the USB bus (e.g. asserting/deasserting resume).
*  Device drivers may modify device registers to accomplish the requested
*  operation.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_bus_control_data_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [IN] The operation to be performed on the bus */
   uint_8                        bcontrol
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_BUS_CONTROL != NULL)
   {
      error = temp_ptr->HOST_BUS_CONTROL (handle, bcontrol);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_alloc_bandwidth_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_alloc_bandwidth_call_interface routine is used to allocate
*  bandwidth for interrupt and isochronous transactions.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_alloc_bandwidth_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_ALLOC_BANDWIDTH != NULL)
   {
      error = temp_ptr->HOST_ALLOC_BANDWIDTH (handle, pipe_descr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_close_pipe_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_close_pipe_call_interface routine is used to notify device drivers
*  when a pipe is closed.  Device drivers may reclaim internal data
*  structures allocated in _usb_host_open_pipe_call_interface when the pipe
*  was opened.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_close_pipe_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_FREE_CONTROLLER_RESOURCE != NULL)
   {
      error = temp_ptr->HOST_FREE_CONTROLLER_RESOURCE (handle, pipe_descr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_open_pipe_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_open_pipe_call_interface routine is used to notify device drivers
*  when a pipe is opened.  Device drivers may allocate internal data
*  structures and associate them with pipe descriptors for reclaimation
*  when the pipe is closed.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_open_pipe_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_OPEN_PIPE != NULL)
   {
      error = temp_ptr->HOST_OPEN_PIPE (handle, pipe_descr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_update_max_packet_size_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_update_max_packet_size_call_interface is used to notify device
*  drivers of the maximum packet size for a USB device.  Host controller
*  device drivers may update internal data structures associated with
*  open pipes on the device.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_update_max_packet_size_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_UPDATE_MAX_PACKET_SIZE != NULL)
   {
      error = temp_ptr->HOST_UPDATE_MAX_PACKET_SIZE (handle, pipe_descr_ptr);
   }

   return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_update_device_address_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_update_device_address_call_interface is used to notify device
*  drivers of the new device address for a USB device.  Host controller
*  device drivers may update internal data structures associated with
*  open pipes on the device.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_update_device_address_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_UPDATE_DEVICE_ADDRESS != NULL)
   {
      error = temp_ptr->HOST_UPDATE_DEVICE_ADDRESS (handle, pipe_descr_ptr);
   }

   return error;
}

USB_STATUS _usb_host_iso_packet_desc_pool_create
(
   uint_32 num_descs
)
{
   uchar_ptr   mem_ptr;
   uint_32     pool_num_bytes;
   uint_32     i;
   
   pool_num_bytes = sizeof (USB_ISO_PACKET_DESC_STRUCT) * num_descs;

   mem_ptr = (uchar_ptr)USB_mem_alloc_word_aligned(pool_num_bytes);
   
   if (mem_ptr == NULL)
   {
      return USBERR_ALLOC;
   }

   usb_host_iso_packet_desc_pool.mem_ptr = mem_ptr;   
   
   for (i = 0; i < num_descs; i++)
   {
      ((USB_ISO_PACKET_DESC_STRUCT_PTR)mem_ptr)[i].buf_offset = 0;
      ((USB_ISO_PACKET_DESC_STRUCT_PTR)mem_ptr)[i].buf_length = 0;
      ((USB_ISO_PACKET_DESC_STRUCT_PTR)mem_ptr)[i].packet_status = USB_OK;
   }
   
   return USB_OK;
}

void _usb_host_iso_packet_desc_pool_destroy (void)
{
   free(usb_host_iso_packet_desc_pool.mem_ptr);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_iso_packet_desc_list_alloc
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_iso_packet_desc_list_alloc is used by Host Controller Drivers (HCDs)
*  to allocate Isochronous Packet Descriptors.  HCDs allocate Isochronous Packet
*  Descriptors in response to HCD Requests from the USB Host Stack to initiate
*  isochronous transfers over the USB bus.  Note that the USB Host Stack passes
*  a PIPE_TR_STRUCT along with each data transfer request made to the HCD (see
*  definition of USB_CALLBACK_FUNCTIONS_STRUCT for a list of all HCD Requests).
*  
*  As isochronous packets are completed by physical host controllers, HCDs
*  store status, transfer size, and memory offsets of data for each completed
*  isochronous packet in Isochronous Packet Descriptors for later consumption
*  by USB Clients (e.g. class drivers, applications, etc.).  Note that a single
*  PIPE_TR_STRUCT representing an isochronous transfer may transfer multiple
*  isochronous packets across the USB bus.
*  
*  As an implementation note, each PIPE_TR_STRUCT structure provides ISO_PACKET_DESC_LIST
*  and ISO_PACKET_DESC_CURRENT_PTR members.  These members are owned by the HCD until
*  the HCD invokes the ISO_TR_CALLBACK function pointer in the PIPE_TR_STRUCT.  After
*  ISO_TR_CALLBACK is invoked the HCD reliquishes control of the PIPE_TR_STRUCT to
*  the USB Client which registered the callback (see usb_hostdev_iso_tr_init).  While
*  the HCD has ownership of the PIPE_TR_STRUCT, the ISO_PACKET_DESC_LIST member is used
*  to store the HCD-allocated descriptors while the ISO_PACKET_DESC_CURRENT_PTR member
*  is used by the HCD to keep track of the next unused descriptor on the list.
*  
*  Note: Isochronous Packet Descriptors are allocated from the Isochronous Packet
*  Descriptor Pool created during USB Host Stack initialization.  The number of descriptors
*  in the Isochronous Packet Descriptor Pool is user-configurable and can be changed by
*  adjusting the value of the USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS macro in user_config.h.
*
*  Note: This function is not thread-safe
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_iso_packet_desc_list_alloc
(
   uint_32 num_descs,   /* must not be zero */
   USB_ISO_PACKET_DESC_LIST_STRUCT_PTR desc_list_ptr  /* output */
)
{
   UNUSED(num_descs)
   UNUSED(desc_list_ptr)
   
   return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_update_device_address_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_iso_packet_desc_list_free is required to be used by USB Clients to free
*  Isochronous Packet Descriptors allocated by HCDs.  See usb_hostdev_iso_tr_init for
*  more information.
*
*  Note: This function is not thread-safe
*  
*END*-----------------------------------------------------------------*/
void _usb_host_iso_packet_desc_list_free
(
   USB_ISO_PACKET_DESC_LIST_STRUCT_PTR desc_list_ptr
)
{
	UNUSED(desc_list_ptr)
	
	return;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initializes the USB hardware and installs the USB 
*  interrupt handler
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_host_init
   (
      /* [IN] device number */
      uint_8                  devnum,
      
      /* [IN] maximum number of Pipes */
      uint_32                 frame_list_size,
      
      /* [OUT] the USB host handle */
      _usb_host_handle _PTR_  hci_handle
   )
{ /* Body */
   USB_STATUS error;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR call_back_table_ptr;
   int_32 mqx_status;

   /* Initialize the USB interface. */   
   usb_host_state_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)USB_mem_alloc_word_aligned(sizeof(USB_HOST_STATE_STRUCT));
   
   memset(usb_host_state_struct_ptr, 0, sizeof(USB_HOST_STATE_STRUCT));
   
   if (usb_host_state_struct_ptr == NULL) {
      return USBERR_ALLOC;
   } /* Endif */

   /* No devices attached yet */
   /*  
   ** We should maintain device lists per controller handle
   */
   usb_host_state_struct_ptr->DEVICE_LIST_PTR = NULL;
   
   usb_host_state_struct_ptr->DEV_NUM = devnum;
   usb_host_state_struct_ptr->FRAME_LIST_SIZE = frame_list_size;

   /* Allocate the USB Host Pipe Descriptors */   
   usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR = 
      (PIPE_DESCRIPTOR_STRUCT_PTR)USB_mem_alloc_word_aligned(((sizeof(PIPE_DESCRIPTOR_STRUCT)) *  USBCFG_MAX_PIPES));
   memset(usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR, 0, ((sizeof(PIPE_DESCRIPTOR_STRUCT)) *  USBCFG_MAX_PIPES));
   
   if (usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR == NULL) {
      free(usb_host_state_struct_ptr);
      return USBERR_ALLOC;
   } /* Endif */

   memset(usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR,0, 
      ((sizeof(PIPE_DESCRIPTOR_STRUCT)) *  USBCFG_MAX_PIPES));

   call_back_table_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)&_bsp_usb_host_callback_table;
     
   
   usb_host_state_struct_ptr->CALLBACK_STRUCT_PTR = (call_back_table_ptr + devnum);
      
   if (!usb_host_state_struct_ptr->CALLBACK_STRUCT_PTR)
   {
      free(usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR);
      free(usb_host_state_struct_ptr);
      USB_unlock();
      return USBERR_DRIVER_NOT_INSTALLED;
   } /* Endif */

#if (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS != 0)
   error = _usb_host_iso_packet_desc_pool_create (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS);
   
   if (error != USB_OK)
   {
      free(usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR);
      free(usb_host_state_struct_ptr);
      USB_unlock();
      return USBERR_ALLOC;
   }
#endif

   mqx_status = _bsp_usb_host_init ((pointer)devnum);

   if (mqx_status != 0)
   {
#if (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS != 0)
      _usb_host_iso_packet_desc_pool_destroy ();
#endif
      free(usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR);
      free(usb_host_state_struct_ptr);
      USB_unlock();
      return USBERR_UNKNOWN_ERROR;
   }

   error = _usb_host_init_call_interface (usb_host_state_struct_ptr);
   
   if (error)
   {
#if (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS != 0)
      _usb_host_iso_packet_desc_pool_destroy ();
#endif
      free(usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR);
      free(usb_host_state_struct_ptr);
      return error;
   } /* Endif */
   
   *hci_handle = (pointer)usb_host_state_struct_ptr;

   return 0;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_register_service
* Returned Value : USB_OK or error code
* Comments       :
*     Registers a callback routine for a specified event.
* 
*END*--------------------------------------------------------------------*/
uint_32 _usb_host_register_service
   (
      /* [IN] Handle to the USB device */
      _usb_host_handle         hci_handle,
      
      /* [IN] type of event or endpoint number to service */
      uint_8                   type,
      
      /* [IN] Pointer to the service's callback function */
      void(_CODE_PTR_ service)(pointer callbk_ptr, uint_32 event_param)
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   USB_SERVICE_STRUCT_PTR _PTR_        search_ptr;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (search_ptr = &usb_host_ptr->SERVICE_HEAD_PTR;
      *search_ptr;
      search_ptr = &(*search_ptr)->NEXT) 
   {
      if ((*search_ptr)->TYPE == type) {
         /* Found an existing entry */
         USB_unlock();
         return USBERR_OPEN_SERVICE;
      } /* Endif */
   } /* Endfor */
   
   /* No existing entry found - create a new one */
   service_ptr = (USB_SERVICE_STRUCT_PTR)USB_mem_alloc_word_aligned(sizeof(USB_SERVICE_STRUCT));
   
   if (!service_ptr) {
      USB_unlock();

      return USBERR_ALLOC_SERVICE;
   } /* Endif */
   
   service_ptr->TYPE = type;
   service_ptr->SERVICE = service;
   service_ptr->NEXT = NULL;
   *search_ptr = service_ptr;
   
   USB_unlock();

   return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_host_call_service
   (
      /* [IN] Handle to the USB device */
      _usb_host_handle handle,

      /* [IN] Type of service or endpoint */
      uint_8           type,
      
      /* [IN] Number of bytes in transmission */
      uint_32          length
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (service_ptr = usb_host_ptr->SERVICE_HEAD_PTR;
      service_ptr;
      service_ptr = service_ptr->NEXT) 
   {
      if (service_ptr->TYPE == type) {
         if (service_ptr->SERVICE) {
            service_ptr->SERVICE(handle, length);
         }
         USB_unlock();
         return USB_OK;
      } /* Endif */
      
   } /* Endfor */

   USB_unlock();
   return USBERR_CLOSED_SERVICE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_check_service
* Returned Value : USB_OK or error code
* Comments       :
*     Check if a service is registered or not.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_host_check_service
   (
      /* [IN] Handle to the USB device */
      _usb_host_handle handle,

      /* [IN] Type of service or endpoint */
      uint_8           type,
      
      /* [IN] Number of bytes in transmission */
      uint_32          length
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   
   UNUSED(length)
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (service_ptr = usb_host_ptr->SERVICE_HEAD_PTR;
      service_ptr;
      service_ptr = service_ptr->NEXT) 
   {
      if (service_ptr->TYPE == type) {
         USB_unlock();
         return USB_OK;
      } /* Endif */
   } /* Endfor */

   USB_unlock();
   return USBERR_CLOSED_SERVICE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_unregister_service
* Returned Value : USB_OK or error code
* Comments       :
*     Unregisters a callback routine for a specified event or endpoint.
* 
*END*--------------------------------------------------------------------*/
uint_32 _usb_host_unregister_service
   (
      /* [IN] USB Host controller handle */
      _usb_host_handle           hci_handle,

      /* [IN] Service to unregister */
      uint_8                     event
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   USB_SERVICE_STRUCT_PTR _PTR_        search_ptr;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (search_ptr = &usb_host_ptr->SERVICE_HEAD_PTR;
      *search_ptr;
      search_ptr = &(*search_ptr)->NEXT) 
   {
      if ((*search_ptr)->TYPE == event) {
         /* Found an existing entry - delete it */
         break;
      } /* Endif */
   } /* Endfor */
   
   /* No existing entry found */
   if (!*search_ptr) {
      USB_unlock();
      return USBERR_CLOSED_SERVICE;
   } /* Endif */
   
   service_ptr = *search_ptr;
   *search_ptr = service_ptr->NEXT;

   USB_unlock();
   
   free((pointer)service_ptr);
   
   return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_open_pipe
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_open_pipe routine initializes a new pipe descriptor and 
*  returns USB_OK or error and the pipe descriptor for the USB bus address, 
*  endpoint number, direction of transfer, maximum packet size, interval 
*  and the type of pipe
*
*END*-----------------------------------------------------------------*/

void _usb_host_open_pipe_cleanup(
         /* [IN] the USB Host state structure */
      _usb_host_handle              handle,
      
      PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr,
      
      boolean                    close_pipe_call
   );
void _usb_host_open_pipe_cleanup(
         /* [IN] the USB Host state structure */
      _usb_host_handle              handle,
      
      PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr,
      
      boolean                    close_pipe_call
   )
{ /* Body */
   PIPE_TR_STRUCT_PTR  temp;
   
   if (close_pipe_call) {
      _usb_host_close_pipe_call_interface (handle, pipe_descr_ptr);
   }
   
   while  (pipe_descr_ptr->tr_list_ptr != NULL) {
      temp = pipe_descr_ptr->tr_list_ptr;
      pipe_descr_ptr->tr_list_ptr = pipe_descr_ptr->tr_list_ptr->next;
      free((pointer)temp);
   }
   pipe_descr_ptr->OPEN = FALSE;

}


USB_STATUS _usb_host_open_pipe
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              hci_handle,
      
      /* [IN] Pipe initialization parameters */
      PIPE_INIT_PARAM_STRUCT_PTR    pipe_init_params_ptr,

      /* [OUT] Pipe handle */
      _usb_pipe_handle _PTR_        pipe_handle
   )
{ /* Body */
   PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
   PIPE_INIT_PARAM_STRUCT_PTR pipe_descr_as_params;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   USB_STATUS                 error;
   uint_32                    i, open_pipes;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;
   USB_lock();

   /* EAI more debug code */
   open_pipes=0;
   for (i=0; i < USBCFG_MAX_PIPES; i++) {
      /* Check if a pipe is not open */
      if (usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i].OPEN) {
         open_pipes++;
      } /* Endif */
   } /* Endfor */
   
   for (i=0; i < USBCFG_MAX_PIPES; i++) {
      /* Check if a pipe is not open */
      if (!(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i].OPEN)) {
         break;
      } /* Endif */
   } /* Endfor */

   USB_unlock();

   if (i >= USBCFG_MAX_PIPES) {
      return USBERR_OPEN_PIPE_FAILED;
   }
   
   pipe_descr_ptr = 
      &usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i];
      
   *pipe_handle = (pointer)pipe_descr_ptr;
   pipe_descr_as_params =  
      (PIPE_INIT_PARAM_STRUCT_PTR)pipe_descr_ptr;
   
   /* copy identical fields params --> descriptor */
   *pipe_descr_as_params = *pipe_init_params_ptr;
   /* additional fields following identical header */
   pipe_descr_ptr->NEXTDATA01 = 0;       /* Start DTC at Data0 */
   pipe_descr_ptr->PIPE_ID = (int_16)i;
   pipe_descr_ptr->OPEN = TRUE;

   error = _usb_host_open_pipe_call_interface (hci_handle, pipe_descr_ptr);
   
   if (error != USB_OK) {
      _usb_host_open_pipe_cleanup(hci_handle, pipe_descr_ptr, FALSE);
      return USBERR_OPEN_PIPE_FAILED;
   }

#ifdef __USB_HOST_EHCI__
   if (pipe_descr_ptr->PIPETYPE <= USB_INTERRUPT_PIPE) {
      /* Call the low-level routine to send a setup packet */
      error = _usb_host_alloc_bandwidth_call_interface (hci_handle, pipe_descr_ptr);
      
      if (error != USB_OK){
         _usb_host_open_pipe_cleanup(hci_handle, pipe_descr_ptr,TRUE);
         return ,USBERR_BANDWIDTH_ALLOC_FAILED;
      } /* Endif */
   } /* Endif */
#endif

   return USB_OK;


} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_new_tr_element
*  Returned Value : pointer to new host tr struct, NULL if error
*  Comments       :
*         This function will allocate and link a new TR element in the 
*  list of TRs for the specified pipe. It is assumed the caller has issued 
*  USB_lock() before entry.
*
*END*-----------------------------------------------------------------*/
static PIPE_TR_STRUCT_PTR _usb_host_new_tr_element
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle  pipe_handle
   )
{ /* Body */

   PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr;
   PIPE_TR_STRUCT_PTR            pipe_tr_ptr, temp_pipe_tr_ptr;

   pipe_tr_ptr = (PIPE_TR_STRUCT_PTR)USB_mem_alloc_word_aligned(sizeof(PIPE_TR_STRUCT));
   if (pipe_tr_ptr == NULL)
   {
      return NULL;
   }
   memset((pointer)pipe_tr_ptr, 0, sizeof(PIPE_TR_STRUCT));

   /* Link pipe's TR structs in circular list of 1 or more items,
   ** where the pipe descriptor's tr list pointer holds the
   ** address of the next struct for scheduling (i.e. the
   ** "beginning" of the list).  
   */
   pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
   temp_pipe_tr_ptr = pipe_descr_ptr->tr_list_ptr;

   if (temp_pipe_tr_ptr == NULL) { 
      /* No existing items, make a circle of one */
      pipe_descr_ptr->tr_list_ptr = pipe_tr_ptr->next = pipe_tr_ptr;
   } else {
      /* Add new item to the "end" of the existing list */
      while (temp_pipe_tr_ptr->next != pipe_descr_ptr->tr_list_ptr)
         temp_pipe_tr_ptr = temp_pipe_tr_ptr->next;

      temp_pipe_tr_ptr->next = pipe_tr_ptr;  
      pipe_tr_ptr->next = pipe_descr_ptr->tr_list_ptr;
   } /* EndIf */

   return pipe_tr_ptr;

} /* Endbody */
 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_tr_element
*  Returned Value : pointer to host's pipe TR struct, NULL if error
*  Comments       :
*     If the tr_index is zero or exists on the list, returns NULL
*     Otherwise initializes to zero a TR element with all linkages,
*       and sets its tr_index to the parameter value.  If no unused
*       tr_struct exists on the list (i.e. none has tr_index = 0),
*       a new struct is allocated and linked into the circular list.
*
*END*-----------------------------------------------------------------*/
PIPE_TR_STRUCT_PTR _usb_host_get_tr_element
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle  pipe_handle,

      /* [IN] transfer parameters */
      uint_32           tr_index
   )
{ /* Body */
   PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
   PIPE_TR_STRUCT_PTR         pipe_tr_list, temp_pipe_tr_list;

   if (tr_index == 0)
   {           
      return NULL;   /* zero is unused-TR flag */
   }

   pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
   pipe_tr_list = pipe_descr_ptr->tr_list_ptr;

   USB_lock();
   if (pipe_tr_list == NULL)  {  /* No list exists, start one */
      pipe_tr_list = _usb_host_new_tr_element(pipe_handle);
      if (pipe_tr_list == NULL) {
         USB_unlock();
         return NULL;
      } /* Endif */
   } else {  /* scan for unused TR's, and for index already in list */
      temp_pipe_tr_list = pipe_tr_list;
      for (; ;) {
         /* TR index is either unused, or */
         if (temp_pipe_tr_list->TR_INDEX == 0) {
            pipe_tr_list = temp_pipe_tr_list;
            break;
         } /* Endif */
         /* TR index is a duplicate (= error), or */
         if (temp_pipe_tr_list->TR_INDEX == tr_index) {
            USB_unlock();
            return NULL;
         } /* Endif */
         /* TR is last in list, so insert a new one, or */
         if (temp_pipe_tr_list->next == pipe_tr_list) {
            pipe_tr_list = _usb_host_new_tr_element(pipe_handle);
         
            if (pipe_tr_list == NULL)
            {
               USB_unlock();
               return NULL;
            }
            
            break;
         } /* Endif */

         /* Step over item to next on the list */
         temp_pipe_tr_list = temp_pipe_tr_list->next;

      } /* EndFor */
   } /* EndIf */
   pipe_tr_list->TR_INDEX = tr_index;  /* mark this TR busy */
   USB_unlock();
   return pipe_tr_list;   

} /* Endbody */
 
 
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_set_up_TR
*  Returned Value :
*     USBERR_INVALID_PIPE_HANDLE if the pipe is invalid
*     USBERR_ALLOC if unable to get a new TR
*     USB_STATUS_TRANSFER_QUEUED upon success
*
*  Comments       :
*     Sets up a TR from init parameters.
*
*END*-----------------------------------------------------------------*/
USB_STATUS  _usb_host_set_up_TR
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr,

      /* [OUT] the TR address on the pipe's TR list */
      PIPE_TR_STRUCT_PTR _PTR_   pipe_tr_ptr_ptr
   )
{ /* Body */
   PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
   PIPE_TR_STRUCT_PTR         pipe_tr_ptr;
   TR_STRUCT_PTR              tr_ptr;

   pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

   /* Check if the pipe id is valid */
   if (pipe_descr_ptr->PIPE_ID > USBCFG_MAX_PIPES) {
      return USBERR_INVALID_PIPE_HANDLE;
   } /* Endif */

   pipe_tr_ptr = _usb_host_get_tr_element(pipe_handle, 
      tr_params_ptr->TR_INDEX);

   if (pipe_tr_ptr == NULL) {
      return USBERR_ALLOC;
   } /* Endif */

   tr_ptr = (TR_STRUCT_PTR)pipe_tr_ptr;

   /* Copy user-provided TR to system-linked TR */
   /* todo AI: change with memcpy */
   *tr_ptr = *((TR_STRUCT_PTR)tr_params_ptr);
   //temp = sizeof(TR_STRUCT);
   //memcpy(tr_ptr, tr_params_ptr, temp);

   /* If TR's setup packet pointer is non-null, copy the packet */
   if (tr_params_ptr->DEV_REQ_PTR != NULL) {
      /*pipe_tr_ptr->setup_packet = 
         *((USB_SETUP_PTR)tr_params_ptr->DEV_REQ_PTR);*/
	 memcpy(&pipe_tr_ptr->setup_packet, 
			(USB_SETUP_PTR)tr_params_ptr->DEV_REQ_PTR,
			sizeof(USB_SETUP));
   } /* Endif */

   /* return successful transfer initiation status */
   pipe_tr_ptr->status = USB_STATUS_TRANSFER_QUEUED;
   *pipe_tr_ptr_ptr = pipe_tr_ptr;

   return USB_STATUS_TRANSFER_QUEUED;

} /* EndBody */
 
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_setup
*  Returned Value : None
*  Comments       :
*  Sends a Setup packet. Internally, following the SendSetup call
*  this takes care of the whole transaction (including receiving or sending 
*  any data to or from the device.
*
*END*-----------------------------------------------------------------*/

uint_32 _usb_host_send_setup
   (
      /* [IN] the USB Host state structure   */
      _usb_host_handle           hci_handle, 

      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
   )
{ /* Body */
   PIPE_TR_STRUCT_PTR         pipe_tr_ptr;
   PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
   uint_32                    return_code;   

   USB_lock();

   /* Initialize a TR and link it into the pipe's TR list */
   return_code = _usb_host_set_up_TR(pipe_handle, tr_params_ptr, &pipe_tr_ptr);
   
   if (return_code != USB_STATUS_TRANSFER_QUEUED) {
      USB_unlock();
      return return_code;
   } /* Endif */
      
   pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

   pipe_descr_ptr->NEXTDATA01 = 0;       /* reset DATA0/1 */

   /* We have obtained the current TR on the Pipe's TR list 
   ** from _usb_host_set_up_TR
   */
   /* true if this setup packet will have a send data phase */
   pipe_tr_ptr->SEND_PHASE = (boolean)!(pipe_tr_ptr->setup_packet.BMREQUESTTYPE & 
      USB_SETUP_DATA_XFER_DIRECTION);
      
   /***************************************************************
   For data caching it is important that we update the memory
   with the intended contents.
   ***************************************************************/
#if PSP_HAS_DATA_CACHE      /* TODO check - maybe uncached allocation used/can be used */
    USB_dcache_flush_mlines(
         (void *)&pipe_tr_ptr->setup_packet,
         1);
         
    if(pipe_descr_ptr->DIRECTION == USB_SEND)
    {
      if(pipe_tr_ptr->TX_LENGTH > 0)
      {
          USB_dcache_flush_mlines(
               (void *)pipe_tr_ptr->TX_BUFFER,
               pipe_tr_ptr->TX_LENGTH
               );
      }
    }
    else
    {
      if(pipe_tr_ptr->RX_LENGTH > 0)
      {
          USB_dcache_invalidate_mlines(
               (void *)pipe_tr_ptr->RX_BUFFER,
               pipe_tr_ptr->RX_LENGTH
               );
      }
    }
    
#endif /* PSP_HAS_DATA_CACHE */
      

   /* Call the low-level routine to send a setup packet */
   return_code = _usb_host_send_setup_call_interface (hci_handle, pipe_descr_ptr, pipe_tr_ptr);

   USB_unlock();

   if(return_code == USB_OK)
   {
      return USB_STATUS_TRANSFER_QUEUED;
   }
   else
   {

      return return_code;
   }

} /* Endbody */

  
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_transfer_status
*  Returned Value : Status
*  Comments       :
* _usb_host_get_transfer_status is a routine that returns status of a transfer. 
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_get_transfer_status
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle     pipe_handle,

      /* [IN] Specific transfer number on the pipe */
      uint_32              transfer_number
   )
{ /* Body */
   PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;

   PIPE_TR_STRUCT_PTR         pipe_tr_list_ptr;
   uint_32                    status = USBERR_INVALID_PIPE_HANDLE;

   USB_lock();

   pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
   if (pipe_descr_ptr!= NULL) {
      pipe_tr_list_ptr = pipe_descr_ptr->tr_list_ptr;
      if (pipe_tr_list_ptr != NULL) {
         do {
            if (pipe_tr_list_ptr->TR_INDEX == transfer_number) {
               status = pipe_tr_list_ptr->status;
               break;
            } /* Endif */
            pipe_tr_list_ptr = pipe_tr_list_ptr->next;   
         } while (pipe_tr_list_ptr != pipe_descr_ptr->tr_list_ptr);
      } /* Endif */
      if (pipe_tr_list_ptr->TR_INDEX != transfer_number) {
         /* Transaction done */
         status = USB_STATUS_IDLE;
      } /* Endif */
   } /* Endif */
   
   USB_unlock();
   return status;
} /* Endbody */


  
/* EOF */
