#ifndef __ehci_main_h__
#define __ehci_main_h__
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
* $FileName: ehci_main.h$
* $Version : 3.5.6.0$
* $Date    : Oct-22-2009$
*
* Comments:
*
*   
*
*END************************************************************************/
#include "types.h"
#include "host_common.h"
#include "ehci_usbprv_host.h"
#include "ehci.h"

#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS _usb_hci_vusb20_send_data (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern USB_STATUS _usb_hci_vusb20_send_setup (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern USB_STATUS _usb_hci_vusb20_recv_data (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern USB_STATUS _usb_hci_ehci_queue_pkts ( _usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern void _usb_ehci_init_Q_HEAD (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, EHCI_QH_STRUCT_PTR, EHCI_QH_STRUCT_PTR, EHCI_QTD_STRUCT_PTR);
extern void _usb_ehci_init_QTD (_usb_host_handle, EHCI_QTD_STRUCT_PTR, uchar_ptr, uint_32);
extern uint_32 _usb_ehci_init_Q_element (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern uint_32 _usb_ehci_add_xfer_to_asynch_schedule_list (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern boolean _usb_ehci_process_port_change (_usb_host_handle);
extern void _usb_ehci_reset_and_enable_port (_usb_host_handle, uint_8);
extern void _usb_host_process_reset_recovery_done (_usb_host_handle);

extern void _usb_hci_vusb20_isr (_usb_host_handle);

extern USB_STATUS _usb_hci_vusb20_init (_usb_host_handle);
extern void _usb_hci_vusb20_free_QTD (_usb_host_handle, pointer);
extern void _usb_hci_vusb20_free_QH (_usb_host_handle, pointer);
extern USB_STATUS _usb_ehci_free_resources (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);
extern void _usb_ehci_process_tr_complete (_usb_host_handle);
extern void _usb_host_delay (_usb_host_handle, uint_32);

extern USB_STATUS _usb_ehci_open_pipe(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR);

extern void _usb_ehci_process_timer(_usb_host_handle);
extern USB_STATUS _usb_ehci_update_dev_address(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);
#ifdef __cplusplus
}
#endif

#endif