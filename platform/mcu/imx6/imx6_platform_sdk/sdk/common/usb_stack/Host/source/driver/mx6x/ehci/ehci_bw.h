#ifndef __ehci_bw_h__
#define __ehci_bw_h__
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
* $FileName: ehci_bw.h$
* $Version : 3.0.2.0$
* $Date    : Aug-12-2009$
*
* Comments:
*
*   
*
*END************************************************************************/
#include "types.h"
#include "host_common.h"
#include "user_config.h"

#if HIGH_SPEED_DEVICE
#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 _usb_ehci_calculate_uframe_tr_time (uint_32, uint_8);
extern uint_32 _usb_ehci_get_frame_number (_usb_host_handle);
extern uint_32 _usb_ehci_get_micro_frame_number (_usb_host_handle);
extern uint_32 _usb_ehci_allocate_bandwidth (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);
extern void unlink_periodic_data_structure_from_frame (USB_Uncached uint_32 _PTR_, USB_Uncached uint_32 _PTR_);
extern void reclaim_band_width (_usb_host_handle, uint_32, USB_Uncached uint_32 _PTR_, PIPE_DESCRIPTOR_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif // HIGH_SPEED_DEVICE

#endif