#ifndef __host_main_h__
#define __host_main_h__
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
* $FileName: host_main.h$
* $Version : 3.0.3.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   
*
*END************************************************************************/
#include "psptypes.h"
#include "host_common.h"

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS _usb_host_init_call_interface (_usb_host_handle);
extern USB_STATUS _usb_host_shutdown_call_interface (_usb_host_handle);
extern USB_STATUS _usb_host_send_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_, struct pipe_tr_struct _PTR_);
extern USB_STATUS _usb_host_send_setup_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_, struct pipe_tr_struct _PTR_);
extern USB_STATUS _usb_host_recv_data_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_ pipe_descr_ptr, struct pipe_tr_struct _PTR_);
extern USB_STATUS _usb_host_cancel_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_, struct pipe_tr_struct _PTR_);
extern USB_STATUS _usb_host_bus_control_data_call_interface (_usb_host_handle, uint_8);
extern USB_STATUS _usb_host_alloc_bandwidth_call_interface (_usb_host_handle, struct pipe_descriptor_struct _PTR_);
extern USB_STATUS _usb_host_close_pipe_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_);
extern USB_STATUS _usb_host_open_pipe_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_);
extern USB_STATUS _usb_host_update_max_packet_size_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_);
extern USB_STATUS _usb_host_update_device_address_call_interface (_usb_host_handle,
   struct pipe_descriptor_struct _PTR_);
extern USB_STATUS _usb_host_iso_packet_desc_pool_create (uint_32);
extern void _usb_host_iso_packet_desc_pool_destroy (void);
extern USB_STATUS _usb_host_iso_packet_desc_list_alloc (uint_32,
   USB_ISO_PACKET_DESC_LIST_STRUCT_PTR);
extern void _usb_host_iso_packet_desc_list_free (USB_ISO_PACKET_DESC_LIST_STRUCT_PTR);
extern USB_STATUS _usb_host_init (uint_8, uint_32, _usb_host_handle _PTR_);
extern uint_32 _usb_host_register_service (_usb_host_handle,
   uint_8, void(_CODE_PTR_ service)(pointer, uint_32));
extern USB_STATUS _usb_host_call_service (_usb_host_handle, uint_8, uint_32);
extern USB_STATUS _usb_host_check_service (_usb_host_handle, uint_8, uint_32);
extern uint_32 _usb_host_unregister_service (_usb_host_handle, uint_8);
extern USB_STATUS _usb_host_open_pipe (_usb_host_handle,
   PIPE_INIT_PARAM_STRUCT_PTR, _usb_pipe_handle _PTR_);
extern struct pipe_tr_struct _PTR_ _usb_host_get_tr_element (_usb_pipe_handle, uint_32);
extern USB_STATUS  _usb_host_set_up_TR (_usb_pipe_handle,
   TR_INIT_PARAM_STRUCT_PTR, struct pipe_tr_struct _PTR_ _PTR_);
extern uint_32 _usb_host_send_setup (_usb_host_handle,
   _usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR);
extern uint_32 _usb_host_get_transfer_status (_usb_pipe_handle, uint_32);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
